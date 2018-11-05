/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: main cxx file - the beginning and the application end.
****
********************************************************************************************************************************/
#include <variant>
#include <array>
#include <bitset>
#include <iomanip>
#include <regex>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "engine.h"
#include "Math/GaussRoutines.h"

using namespace std::string_literals;
using namespace std::string_view_literals;


namespace cubemap {
bool InitCubemap();
void DrawCubemap();
}

namespace app {
using namespace std::string_literals;
using namespace std::string_view_literals;

intf::Grid grid;

auto constexpr kUSE_MS = false;

auto constexpr width = 1920;
auto constexpr height = 1080;

auto constexpr clear_colors = isle::make_array(0.f, 1.f);

u32 constexpr index0 = 0, index1 = 1, index2 = 2, index3 = 3, index4 = 4;

u32 main_fbo;
u32 rt_0, rt_1, rt_depth;
u64 rt_0_handle, rt_1_handle, rt_depth_handle;

u32 out_fbo;
u64 out_rt0_handle, out_rt1_handle;
u32 out_rt0, out_rt1;// , out_depth;

u32 ms_fbo;
u64 ms_rt_0_handle, ms_rt_1_handle, ms_rt_depth_handle;

Program hemisphere_program;
u32 hemisphere_vao, hemisphere_count;

Program geom_program;
u32 geom_vao, geom_count;

Program ssao_program;
u32 quad_vao, noise_tex;
u64 noise_tex_handle;

u32 radiance_tex;
u64 radiance_tex_handle;
Program radiance_program;
u32 radiance_vao, radiance_indirect_buffer;

Texture tempTexture(Texture::eTEXTURE_TYPE::nCUBE, R"(Skybox/skybox)"s);
Texture tempTexture2(Texture::eTEXTURE_TYPE::n2D, R"(newport-loft-ref)"s);
u32 temp_tex;


auto matrices = make_array(
    math::Matrix::Identity(),
    math::Matrix::Identity(),
    math::Matrix::Identity()
);

struct Vertex {
    Position pos;
    math::Vector normal;
    UV uv;

    Vertex() = default;

    template<class P, class C, class U, typename std::enable_if_t<std::is_same_v<Position, std::decay_t<P>> && std::is_same_v<math::Vector, std::decay_t<C>> && std::is_same_v<UV, std::decay_t<U>>>...>
    constexpr Vertex(P &&position, C &&color, U &&uv)
    {
        pos = std::forward<P>(position);
        color = std::forward<C>(color);
        uv = std::forward<UV>(uv);
    }

    template<class T, typename std::enable_if_t<std::is_same_v<Vertex, std::decay_t<T>>>...>
    constexpr bool operator== (T &&rhs) const
    {
        return pos == rhs.pos && color == rhs.color && uv == rhs.uv;
    }
};

std::ostream &operator<< (std::ostream &_stream, Vertex const &v)
{
    return _stream << v.pos << " " << v.normal << " " << v.uv;
}

void InitSSAO()
{
    std::uniform_real_distribution<float> floats(.1f, 1.f);
    std::default_random_engine generator;

    // The seed.
    std::random_device rd;

    // Mersenne-Twister engine.
    std::mt19937_64 mt(rd());

    auto constexpr kernel_size = 64u;

    std::array<math::Vector, kernel_size> kernel;
    std::generate(kernel.begin(), kernel.end(), [&floats, &mt, size = std::size(kernel), i = 0] () mutable
    {
        math::Vector sample(floats(mt) * 2 - 1, floats(mt) * 2 - 1, floats(mt));

        sample.Normalize();
        sample *= floats(mt);

        auto scale = static_cast<float>(++i) / static_cast<float>(size);
        scale = math::lerp(.1f, 1.f, scale * scale);

        sample *= scale;

        return sample;
    });

    glProgramUniform3fv(ssao_program.program(), 20, kernel_size, &std::data(kernel)->x);

    if (hemisphere_program.AssignNew({R"(Defaults/Unlit-Simple.glsl)"})) {
        Render::inst().CreateVAO(hemisphere_vao);

        auto bo = 0u;
        Render::inst().CreateBO(bo);

        hemisphere_count = static_cast<decltype(hemisphere_count)>(std::size(kernel));

        glNamedBufferStorage(bo, sizeof(decltype(kernel)::value_type) * std::size(kernel), std::data(kernel), 0);

        glVertexArrayAttribBinding(hemisphere_vao, Render::eVERTEX_IN::nPOSITION, 0);
        glVertexArrayAttribFormat(hemisphere_vao, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(hemisphere_vao, Render::eVERTEX_IN::nPOSITION);

        glVertexArrayVertexBuffer(hemisphere_vao, 0, bo, 0, sizeof(math::Vector));
    }

    {
        std::array<math::Vector, 16> noise;
        std::generate(noise.begin(), noise.end(), [&floats, &mt] ()
        {
            return math::Vector(floats(mt) * 2 - 1, floats(mt) * 2 - 1, 0).Normalize();
        });

        Render::inst().CreateTBO(GL_TEXTURE_2D, noise_tex);

        glTextureParameteri(noise_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(noise_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(noise_tex, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(noise_tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(noise_tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureStorage2D(noise_tex, 1, GL_RGB32F, 4, 4);
        glTextureSubImage2D(noise_tex, 0, 0, 0, 4, 4, GL_RGB, GL_FLOAT, std::data(noise));
    }
}

void InitMultisampledFramebuffers()
{
    auto constexpr samples = 16;

    glCreateFramebuffers(1, &ms_fbo);

    u32 ms_rt_0, ms_rt_1, ms_rt_depth;

    Render::inst().CreateTBO(GL_TEXTURE_2D_MULTISAMPLE, ms_rt_depth);

    glTextureParameteri(ms_rt_depth, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(ms_rt_depth, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(ms_rt_depth, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(ms_rt_depth, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(ms_rt_depth, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2DMultisample(ms_rt_depth, samples, GL_DEPTH_COMPONENT32F, width, height, GL_TRUE);

    ms_rt_depth_handle = glGetTextureHandleARB(ms_rt_depth);
    glMakeTextureHandleResidentARB(ms_rt_depth_handle);

    Render::inst().CreateTBO(GL_TEXTURE_2D_MULTISAMPLE, ms_rt_0);

    glTextureParameteri(ms_rt_0, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(ms_rt_0, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTextureParameteri(rt_0, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTextureParameteri(rt_0, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(ms_rt_0, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(ms_rt_0, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(ms_rt_0, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2DMultisample(ms_rt_0, samples, GL_RGBA8, width, height, GL_TRUE);
    //glTextureSubImage2D(quad_tid, 0, 0, 0, width, height, GL_RGBA, GL_RGBA8, nullptr);

    ms_rt_0_handle = glGetTextureHandleARB(ms_rt_0);
    glMakeTextureHandleResidentARB(ms_rt_0_handle);

    Render::inst().CreateTBO(GL_TEXTURE_2D_MULTISAMPLE, ms_rt_1);

    glTextureParameteri(ms_rt_1, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(ms_rt_1, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(ms_rt_1, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(ms_rt_1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(ms_rt_1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2DMultisample(ms_rt_1, samples, GL_RG16F, width, height, GL_TRUE);

    ms_rt_1_handle = glGetTextureHandleARB(ms_rt_1);
    glMakeTextureHandleResidentARB(ms_rt_1_handle);

    glNamedFramebufferTexture(ms_fbo, GL_DEPTH_ATTACHMENT, ms_rt_depth, 0);
    glNamedFramebufferTexture(ms_fbo, GL_COLOR_ATTACHMENT0, ms_rt_0, 0);
    glNamedFramebufferTexture(ms_fbo, GL_COLOR_ATTACHMENT1, ms_rt_1, 0);

    {
        auto constexpr drawBuffers = make_array<u32>(GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1);
        glNamedFramebufferDrawBuffers(ms_fbo, static_cast<i32>(std::size(drawBuffers)), std::data(drawBuffers));
    }

    if (auto result = glCheckNamedFramebufferStatus(ms_fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        log::Fatal() << "framebuffer error:" << std::hex << result;

    auto samplesParam = 0;
    glGetNamedFramebufferParameteriv(ms_fbo, GL_SAMPLES, &samplesParam);
    if (samples != samples)
        log::Fatal() << "unexpected framebuffer samples count";
}

void InitFramebuffer()
{
    if constexpr (kUSE_MS)
        InitMultisampledFramebuffers();

    glCreateFramebuffers(1, &main_fbo);

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_depth);

    glTextureParameteri(rt_depth, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_depth, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_depth, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_depth, 1, GL_DEPTH_COMPONENT32F, width, height);

    rt_depth_handle = glGetTextureHandleARB(rt_depth);
    glMakeTextureHandleResidentARB(rt_depth_handle);

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_0);

    glTextureParameteri(rt_0, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(rt_0, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTextureParameteri(rt_0, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTextureParameteri(rt_0, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_0, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_0, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_0, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_0, 1, GL_RGBA8, width, height);
    //glTextureSubImage2D(quad_tid, 0, 0, 0, width, height, GL_RGBA, GL_RGBA8, nullptr);

    rt_0_handle = glGetTextureHandleARB(rt_0);
    glMakeTextureHandleResidentARB(rt_0_handle);

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_1);

    glTextureParameteri(rt_1, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rt_1, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_1, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_1, 1, GL_RG16F, width, height);

    rt_1_handle = glGetTextureHandleARB(rt_1);
    glMakeTextureHandleResidentARB(rt_1_handle);

    glNamedFramebufferTexture(main_fbo, GL_DEPTH_ATTACHMENT, rt_depth, 0);
    glNamedFramebufferTexture(main_fbo, GL_COLOR_ATTACHMENT0, rt_0, 0);
    glNamedFramebufferTexture(main_fbo, GL_COLOR_ATTACHMENT1, rt_1, 0);

    {
        auto constexpr drawBuffers = make_array<u32>(GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1);
        glNamedFramebufferDrawBuffers(main_fbo, static_cast<i32>(std::size(drawBuffers)), std::data(drawBuffers));
    }

    //glDisablei(GL_BLEND, 0);
    //glBlendFunci(0, GL_ONE, GL_ONE);
    glEnablei(GL_BLEND, 0);
    glBlendFunci(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisablei(GL_BLEND, 1);
    glBlendFunci(1, GL_ONE, GL_ONE);

    glDisablei(GL_BLEND, 2);
    glBlendFunci(2, GL_ONE, GL_ONE);

    if (auto result = glCheckNamedFramebufferStatus(main_fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        log::Fatal() << "framebuffer error:" << result;

    glCreateFramebuffers(1, &out_fbo);

    Render::inst().CreateTBO(GL_TEXTURE_2D, out_rt0);

    glTextureParameteri(out_rt0, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(out_rt0, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTextureParameteri(out_rt0, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTextureParameteri(out_rt0, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(out_rt0, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(out_rt0, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(out_rt0, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(out_rt0, 1, GL_RGBA8, width, height);

    out_rt0_handle = glGetTextureHandleARB(out_rt0);
    glMakeTextureHandleResidentARB(out_rt0_handle);

    Render::inst().CreateTBO(GL_TEXTURE_2D, out_rt1);

    glTextureParameteri(out_rt1, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(out_rt1, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(out_rt1, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(out_rt1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(out_rt1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(out_rt1, 1, GL_RGBA8, width, height);

    out_rt1_handle = glGetTextureHandleARB(out_rt1);
    glMakeTextureHandleResidentARB(out_rt1_handle);

    glNamedFramebufferTexture(out_fbo, GL_COLOR_ATTACHMENT0, out_rt0, 0);

    /*glCreateRenderbuffers(1, &out_depth);
    glNamedRenderbufferStorage(out_depth, GL_DEPTH_COMPONENT32F, width, height);
    glNamedFramebufferRenderbuffer(out_fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, out_depth);*/

    {
        auto constexpr drawBuffers = make_array<u32>(GL_COLOR_ATTACHMENT0);
        glNamedFramebufferDrawBuffers(out_fbo, static_cast<i32>(std::size(drawBuffers)), std::data(drawBuffers));
    }

    if (auto result = glCheckNamedFramebufferStatus(out_fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        log::Fatal() << "framebuffer error:" << result;

    if (auto const code = glGetError(); code != GL_NO_ERROR)
        log::Error() << __FUNCTION__ << ": " << code;
}

void InitGaussFilter(Program &program)
{
    using namespace math;

#if 0
    auto constexpr variance = 1.5765f;
    auto kernelSize = PixelsNeededForSigma(kernelSize, 4.f);
#else
    auto kernelSize = 15;
    auto const variance = GetSigmaBasedOnTapSize(kernelSize, 4.f);
#endif

    auto constexpr use_bf = true;
    auto constexpr use_gpu = true;

    auto weights = GaussianKernelIntegrals(variance, kernelSize, true, !use_bf);

    decltype(weights) offsets;

    if constexpr (use_gpu && !use_bf)
    {
        decltype(weights) new_weights;

        new_weights.emplace_back(weights.front());
        offsets.emplace_back(0.f);

        for (auto it = std::next(weights.cbegin()); it < weights.cend(); std::advance(it, 2)) {
            auto it_next = std::next(it);

            auto const w0 = *it;
            auto const w1 = it_next < weights.cend() ? *it_next : 0;

            auto const weight = w0 + w1;

            auto const a = std::distance(weights.cbegin(), it) * w0;
            auto const b = std::distance(weights.cbegin(), it_next) * w1;

            offsets.emplace_back((a + b) / weight);
            new_weights.emplace_back(weight);

            it = it < std::prev(weights.cend()) ? it : std::prev(it);
        }

        weights = std::move(new_weights);
    }

    else
        if (weights.size() != kernelSize / 2 + 1) {
            log::Error() << "requested blur kernel size is not equal to weights' vector size.";
            return;
        }

    using namespace std::string_literals;
    if (!program.AssignNew({R"(Defaults/SSAO.glsl)"s}, "kKERNEL_SIZE"s, std::size(weights), "GPU_FILTERED_GAUSSIAN_BLUR"s, use_gpu, "BILATERAL_GAUSSIAN_GILTER"s, use_bf))
        return;

    {
        auto index = glGetProgramResourceIndex(program.program(), GL_SHADER_STORAGE_BLOCK, "GAUSS_FILTER_COLOR_WEIGHTS");

        if (index != GL_INVALID_INDEX) {
            u32 GAUSS_FILTER_COLOR_WEIGHTS = 0;

            Render::inst().CreateBO(GAUSS_FILTER_COLOR_WEIGHTS);
            glNamedBufferStorage(GAUSS_FILTER_COLOR_WEIGHTS, sizeof(decltype(weights)::value_type) * std::size(weights), std::data(weights), 0);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, GAUSS_FILTER_COLOR_WEIGHTS);
            glShaderStorageBlockBinding(program.program(), index, 5);
        }

        else log::Error() << "can't init the SSBO: invalid index param: " << "GAUSS_FILTER_COLOR_WEIGHTS";
    }

    if constexpr (use_gpu && !use_bf)
    {
        auto index = glGetProgramResourceIndex(program.program(), GL_SHADER_STORAGE_BLOCK, "GAUSS_FILTER_OFFSETS");

        if (index != GL_INVALID_INDEX) {
            u32 GAUSS_FILTER_OFFSETS = 0;

            Render::inst().CreateBO(GAUSS_FILTER_OFFSETS);
            glNamedBufferStorage(GAUSS_FILTER_OFFSETS, sizeof(decltype(offsets)::value_type) * std::size(offsets), std::data(offsets), 0);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, GAUSS_FILTER_OFFSETS);
            glShaderStorageBlockBinding(program.program(), index, 6);
        }

        else log::Error() << "can't init the SSBO: invalid index param: " << "GAUSS_FILTER_OFFSETS";
    }
}

void RenderCubeMap()
{
    static auto invocked = false;

    if (invocked)
        return;

    else invocked = true;

    i32 width = 512, height = 512;

    u32 envmap_fbo, rt_depth, rt_0;

    glCreateFramebuffers(1, &envmap_fbo);

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_depth);

    glTextureParameteri(rt_depth, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_depth, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_depth, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_depth, 1, GL_DEPTH_COMPONENT32F, width, height);

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_0);

    glTextureParameteri(rt_0, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(rt_0, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTextureParameteri(rt_0, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTextureParameteri(rt_0, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_0, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_0, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_0, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_0, 1, GL_RGBA8, width, height);


    glNamedFramebufferTexture(envmap_fbo, GL_DEPTH_ATTACHMENT, rt_depth, 0);
    glNamedFramebufferTexture(envmap_fbo, GL_COLOR_ATTACHMENT0, rt_0, 0);

    {
        auto constexpr drawBuffers = make_array<u32>(GL_COLOR_ATTACHMENT0);
        glNamedFramebufferDrawBuffers(envmap_fbo, static_cast<i32>(std::size(drawBuffers)), std::data(drawBuffers));
    }

    if (auto result = glCheckNamedFramebufferStatus(envmap_fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        log::Fatal() << "framebuffer error:" << result;

    if (auto const code = glGetError(); code != GL_NO_ERROR)
        log::Error() << __FUNCTION__ << ": " << code;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, envmap_fbo);

    glViewport(0, 0, width, height);

    glClearNamedFramebufferfv(envmap_fbo, GL_COLOR, 0, std::data(colors::kBLACK.rgba));
    glClearNamedFramebufferfv(envmap_fbo, GL_DEPTH, 0, &clear_colors[Render::kREVERSED_DEPTH ? 0 : 1]);

    radiance_program.UseThis();

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index2);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index2);

    glProgramUniformHandleui64ARB(radiance_program.program(), Render::eSAMPLERS_BINDING::nALBEDO, radiance_tex_handle);

    glBindVertexArray(radiance_vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, radiance_indirect_buffer);
    glDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr);
}

void InitIBL()
{
    stbi_set_flip_vertically_on_load(true);

    i32 width, height, nrComponents;

    auto const path = R"(../contents/textures/newport-loft-ref.hdr)"s;

    auto data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);

    if (data == nullptr)
        return;

    Render::inst().CreateTBO(GL_TEXTURE_2D, radiance_tex);

    glTextureParameteri(radiance_tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(radiance_tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(radiance_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(radiance_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(radiance_tex, 1, GL_RGB16F, width, height);
    glTextureSubImage2D(radiance_tex, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);

    stbi_image_free(data);

    radiance_tex_handle = glGetTextureHandleARB(radiance_tex);
    glMakeTextureHandleResidentARB(radiance_tex_handle);


    if (!tempTexture.Init())
        return;

    /*if (!tempTexture2.Init())
        return;*/


    radiance_program.AssignNew({R"(Defaults/Radiance-Cube-Map.glsl)"s});

    auto constexpr vertices = make_array(
        Position{+.5f, +.5f, +.5f},
        Position{-.5f, +.5f, +.5f},
        Position{+.5f, +.5f, -.5f},
        Position{-.5f, +.5f, -.5f},

        Position{-.5f, +.5f, -.5f},
        Position{-.5f, +.5f, -.5f},

        Position{-.5f, +.5f, -.5f},
        Position{-.5f, -.5f, -.5f},
        Position{+.5f, +.5f, -.5f},
        Position{+.5f, -.5f, -.5f},

        Position{+.5f, +.5f, -.5f},
        Position{+.5f, -.5f, -.5f},
        Position{+.5f, +.5f, +.5f},
        Position{+.5f, -.5f, +.5f},

        Position{+.5f, +.5f, +.5f},
        Position{+.5f, -.5f, +.5f},
        Position{-.5f, +.5f, +.5f},
        Position{-.5f, -.5f, +.5f},

        Position{-.5f, +.5f, +.5f},
        Position{-.5f, -.5f, +.5f},
        Position{-.5f, +.5f, -.5f},
        Position{-.5f, -.5f, -.5f},

        Position{-.5f, -.5f, -.5f},
        Position{-.5f, -.5f, -.5f},

        Position{-.5f, -.5f, -.5f},
        Position{-.5f, -.5f, +.5f},
        Position{+.5f, -.5f, -.5f},
        Position{+.5f, -.5f, +.5f}
    );

    Render::inst().CreateBO(radiance_indirect_buffer);

    struct command_t {
        u32  count;
        u32  instanceCount;
        u32  first;
        u32  baseInstance;
    };
    
    command_t constexpr command = {
        static_cast<u32>(std::size(vertices)), 1, 0, 0
    };

    glNamedBufferStorage(radiance_indirect_buffer, sizeof(command), &command, GL_DYNAMIC_STORAGE_BIT);


    Render::inst().CreateVAO(radiance_vao);

    auto bo = 0u;
    Render::inst().CreateBO(bo);

    glNamedBufferStorage(bo, sizeof(vertices), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayAttribBinding(radiance_vao, Render::eVERTEX_IN::nPOSITION, 0);
    glVertexArrayAttribFormat(radiance_vao, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(radiance_vao, 0, bo, 0, sizeof(Position));

    glEnableVertexArrayAttrib(radiance_vao, Render::eVERTEX_IN::nPOSITION);
}

void Init()
{
    std::vector<Vertex> vertex_buffer;

    //auto future = std::async(std::launch::async, LoadModel<Vertex>, "chalet.obj", std::ref(geom_count), std::ref(vertex_buffer));
    LoadModel<Vertex>("Hebe.obj"sv, std::ref(geom_count), std::ref(vertex_buffer));

    Camera::inst().Create(Camera::eCAM_BEHAVIOR::nFREE);
    Camera::inst().SetPos(-1, 3, 1);
    Camera::inst().LookAt(0, 2, 0);
    //Camera::inst().LookAt(math::Vector{0z, 7, -0}, math::Vector{0, 0, 0});

    grid.Update(15, 1, 5);

    cubemap::InitCubemap();

    InitFramebuffer();

    if (!geom_program.AssignNew({ R"(Defaults/Diffuse-Lambert.glsl)" }))
        return;

	InitGaussFilter(ssao_program);

    InitSSAO();

    {
		Render::inst().CreateVAO(quad_vao);

        auto constexpr vertices = make_array(
            Position{-1, +1, 0},
            Position{-1, -1, 0},
            Position{+1, +1, 0},
            Position{+1, -1, 0}
        );

        auto bo = 0u;
        Render::inst().CreateBO(bo);

        glNamedBufferStorage(bo, sizeof(decltype(vertices)::value_type) * std::size(vertices), std::data(vertices), 0);

        glVertexArrayAttribBinding(quad_vao, Render::eVERTEX_IN::nPOSITION, 0);
        glVertexArrayAttribFormat(quad_vao, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(quad_vao, Render::eVERTEX_IN::nPOSITION);

        glVertexArrayVertexBuffer(quad_vao, 0, bo, 0, sizeof(Position));
    }

    InitIBL();

    //if (future.get()) {
        Render::inst().CreateVAO(geom_vao);

        auto bo = 0u;
        Render::inst().CreateBO(bo);

        glNamedBufferStorage(bo, sizeof(decltype(vertex_buffer)::value_type) * std::size(vertex_buffer), std::data(vertex_buffer), 0);

        glVertexArrayAttribBinding(geom_vao, Render::eVERTEX_IN::nPOSITION, 0);
        glVertexArrayAttribFormat(geom_vao, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
        glEnableVertexArrayAttrib(geom_vao, Render::eVERTEX_IN::nPOSITION);

        glVertexArrayAttribBinding(geom_vao, Render::eVERTEX_IN::nNORMAL, 0);
        glVertexArrayAttribFormat(geom_vao, Render::eVERTEX_IN::nNORMAL, 3, GL_FLOAT, GL_TRUE, offsetof(Vertex, normal));
        glEnableVertexArrayAttrib(geom_vao, Render::eVERTEX_IN::nNORMAL);

        glVertexArrayAttribBinding(geom_vao, Render::eVERTEX_IN::nTEX_COORD, 0);
        glVertexArrayAttribFormat(geom_vao, Render::eVERTEX_IN::nTEX_COORD, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
        glEnableVertexArrayAttrib(geom_vao, Render::eVERTEX_IN::nTEX_COORD);

        glVertexArrayVertexBuffer(geom_vao, 0, bo, 0, sizeof(decltype(vertex_buffer)::value_type));
    //}
}

void Update()
{ }

void DrawFrame()
{
    matrices[1].Translate(0, 0, 0);
    // matrices[1].Rotate(math::Vector(1, 1, 0), 45.f);
    // matrices[1].Translate(0.2f, 0.78f, -0.75f);
    matrices[1].Scale(.01f, .01f, .01f);
    matrices[0] = Render::inst().vp_.projView() * matrices[1];
    matrices[2] = math::Matrix::GetNormalMatrix(Render::inst().vp_.cam().view() * matrices[1]);
    Render::inst().UpdateTransform(0, 3, std::data(matrices));

    Render::inst().UpdateViewport(1, 1, &Render::inst().vp_.proj());

    RenderCubeMap();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearNamedFramebufferfv(0, GL_COLOR | GL_DEPTH, 0, std::data(colors::kPOWDERBLUE.rgba));

    glViewport(0, 0, width, height);

#if 1
    radiance_program.UseThis();

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index0);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index0);

    //glBindTextureUnit(Render::eSAMPLERS_BINDING::nALBEDO, radiance_tex);
    glProgramUniformHandleui64ARB(radiance_program.program(), Render::eSAMPLERS_BINDING::nALBEDO, radiance_tex_handle);
    glBindTextureUnit(Render::eSAMPLERS_BINDING::nNORMAL_MAP, tempTexture.id());

    glBindVertexArray(radiance_vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, radiance_indirect_buffer);
    glDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr);
#endif

    //cubemap::DrawCubemap();

#if 0
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, kUSE_MS ? ms_fbo : main_fbo);

    glViewport(0, 0, width, height);

    glClearNamedFramebufferfv(kUSE_MS ? ms_fbo : main_fbo, GL_COLOR, 0, std::data(colors::kPOWDERBLUE.rgba));
    glClearNamedFramebufferfv(kUSE_MS ? ms_fbo : main_fbo, GL_COLOR, 1, std::data(colors::kBLACK.rgba));
    glClearNamedFramebufferfv(kUSE_MS ? ms_fbo : main_fbo, GL_DEPTH, 0, &clear_colors[Render::kREVERSED_DEPTH ? 0 : 1]);

    //cubemap::DrawCubemap();
    //grid.Draw();

    //geom_program.UseThis();

    ssao_program.UseThis();

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index0);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index0);

    glBindVertexArray(geom_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * geom_count);

    glFinish();

    if constexpr (kUSE_MS)
        glBlitNamedFramebuffer(ms_fbo, main_fbo, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, out_fbo);
    glViewport(0, 0, width, height);

    glNamedFramebufferTexture(out_fbo, GL_COLOR_ATTACHMENT0, out_rt0, 0);

    glClearNamedFramebufferfv(out_fbo, GL_COLOR, 0, std::data(colors::kPOWDERBLUE.rgba));
    glClearNamedFramebufferfv(out_fbo, GL_DEPTH, 0, &clear_colors[Render::kREVERSED_DEPTH ? 0 : 1]);

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index1);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index1);

    glProgramUniformHandleui64ARB(ssao_program.program(), Render::eSAMPLERS_BINDING::nALBEDO, rt_0_handle);
    glProgramUniformHandleui64ARB(ssao_program.program(), Render::eSAMPLERS_BINDING::nNORMAL_MAP, rt_1_handle);
    glProgramUniformHandleui64ARB(ssao_program.program(), Render::eSAMPLERS_BINDING::nDEPTH, rt_depth_handle);
    //glProgramUniformHandleui64ARB(ssao_program.program(), 4, noise_tex_handle);

    //glBindTextureUnit(Render::eSAMPLERS_BINDING::nALBEDO, rt_0);
    //glBindTextureUnit(Render::eSAMPLERS_BINDING::nNORMAL_MAP, rt_1);
    //glBindTextureUnit(Render::nDEPTH, rt_depth);
    glBindTextureUnit(4, noise_tex);
    glBindTextureUnit(5, radiance_tex);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

#ifndef BLUR
    glFinish();

    glNamedFramebufferTexture(out_fbo, GL_COLOR_ATTACHMENT0, out_rt1, 0);

    /*glClearNamedFramebufferfv(out_fbo, GL_COLOR, 0, colors::kPOWDERBLUE.rgba.data());
    glClearNamedFramebufferfv(out_fbo, GL_DEPTH, 0, &clear_colors[Render::kREVERSED_DEPTH ? 0 : 1]);*/

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index1);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index3);

    glProgramUniformHandleui64ARB(ssao_program.program(), Render::eSAMPLERS_BINDING::nALBEDO, out_rt0_handle);
    glProgramUniformHandleui64ARB(ssao_program.program(), Render::eSAMPLERS_BINDING::nNORMAL_MAP, rt_1_handle);
    glProgramUniformHandleui64ARB(ssao_program.program(), Render::eSAMPLERS_BINDING::nDEPTH, rt_depth_handle);
    //glBindTextureUnit(Render::eSAMPLERS_BINDING::nALBEDO, out_rt0);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
#if 1

    glNamedFramebufferTexture(out_fbo, GL_COLOR_ATTACHMENT0, out_rt0, 0);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index4);

    glProgramUniformHandleui64ARB(ssao_program.program(), Render::eSAMPLERS_BINDING::nALBEDO, out_rt1_handle);
    //glBindTextureUnit(Render::eSAMPLERS_BINDING::nALBEDO, out_rt1);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
#endif
#endif

    glBlitNamedFramebuffer(out_fbo, 0, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
#endif
    //cubemap::DrawCubemap();

#if 0
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    hemisphere_program.UseThis();
    matrices[1].Scale(1, 1, 1);
    matrices[0] = Render::inst().vp_.projView() * matrices[1];
    Render::inst().UpdateTransform(0, 2, matrices.data());

    glBindVertexArray(hemisphere_vao);
    glDrawArrays(GL_POINTS, 0, hemisphere_count);
#endif

#if 0
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	test_program.UseThis();
	matrices[1].Scale(1, 1, 1);
	matrices[0] = Render::inst().vp_.projView() * matrices[1];
	Render::inst().UpdateTransform(0, 2, matrices.data());

	glBindVertexArray(test_vao);
	glDrawArrays(GL_LINE_STRIP, 0, test_count);
#endif
}
};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    isle::Window window(crus::names::kMAIN_WINDOW_NAME, hInstance, app::width, app::height);

    return isle::System::Loop();
}
