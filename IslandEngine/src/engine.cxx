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
#include <iostream>

#include <boost/signals2.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#pragma warning(push, 3)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#pragma warning(pop)

#include "System/InputManager.hxx"
#include "Camera/CameraController.hxx"

#include "glTF.hxx"


namespace glm
{
template<class T, std::enable_if_t<std::is_same_v<std::decay_t<T>, glm::mat4>>...>
std::ostream &operator<< (std::ostream &stream, T &&m)
{
    stream << std::setprecision(0) << std::fixed;

    stream << m[0][0] << ' ' << m[0][1] << ' ' << m[0][2] << ' ' << m[0][3] << '\n';
    stream << m[1][0] << ' ' << m[1][1] << ' ' << m[1][2] << ' ' << m[1][3] << '\n';
    stream << m[2][0] << ' ' << m[2][1] << ' ' << m[2][2] << ' ' << m[2][3] << '\n';
    stream << m[3][0] << ' ' << m[3][1] << ' ' << m[3][2] << ' ' << m[3][3];

    return stream;
}
}

#define RENDER_TO_CUBEMAP 1

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "engine.h"
#include "Math/GaussRoutines.h"


struct per_object_t {
    glm::mat4 world{1};
    glm::mat4 normal{1};  // Transposed of the inversed of the upper left 3x3 sub-matrix of model(world)-view matrix.
};

//struct per_camera_t {
//    glm::mat4 view;
//    glm::mat4 projection;
//
//    glm::mat4 invertedView;
//    glm::mat4 invertedProjection;
//
//    glm::mat4 projectionView;
//};


struct application_t {
    isle::CameraSystem cameraSystem;
    std::shared_ptr<isle::Camera> camera;

    std::unique_ptr<isle::OrbitController> cameraController;

    per_object_t object;

    i32 width{800}, height{1080};

} app;

namespace cubemap {
bool InitCubemap();
void DrawCubemap();
isle::Texture const &texture();
}

namespace application {
auto constexpr meshName = "Hebe.obj"sv;

//intf::Grid grid;

auto constexpr kUSE_MS = false;

auto constexpr clear_colors = isle::make_array(0.f, 1.f);

u32 constexpr index0 = 0, index1 = 1, index2 = 2, index3 = 3, index4 = 4;

Program mesh_program;
u32 mesh_vao, mesh_count;

u32 quad_vao;



auto matrices = make_array(
    math::Matrix::Identity(),
    math::Matrix::Identity(),
    math::Matrix::Identity()
);

namespace ibl
{
u32 fbo;
Program program;

u32 rt_depth, equirectangular_tex;

#if RENDER_TO_CUBEMAP
u32 rt_cubemap, rt_irradiance, rt_depth_irradiance;

u32 irradiance_width = 32, irradiance_height = 32;
#else
u32 rt_2D;
#endif

u32 width = 1024, height = 1024;

u32 cube_vao, cube_ibo;

auto projection = glm::perspective(glm::radians(90.f), 1.f, .1f, 10.f);

auto views = make_array(
    glm::lookAt(glm::vec3(0), glm::vec3(+1, +0, +0), glm::vec3(0, -1, +0)),
    glm::lookAt(glm::vec3(0), glm::vec3(-1, +0, +0), glm::vec3(0, -1, +0)),
    glm::lookAt(glm::vec3(0), glm::vec3(+0, +1, +0), glm::vec3(0, +0, +1)),
    glm::lookAt(glm::vec3(0), glm::vec3(+0, -1, +0), glm::vec3(0, +0, -1)),
    glm::lookAt(glm::vec3(0), glm::vec3(+0, +0, +1), glm::vec3(0, -1, +0)),
    glm::lookAt(glm::vec3(0), glm::vec3(+0, +0, -1), glm::vec3(0, -1, +0))
);


void initFramebuffer()
{
    glCreateFramebuffers(1, &fbo);

#if !RENDER_TO_CUBEMAP
    width = app::width;
    height = app::height;
#endif

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_depth);

    glTextureParameteri(rt_depth, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_depth, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_depth, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_depth, 1, GL_DEPTH_COMPONENT32F, width, height);

    glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, rt_depth, 0);

#if RENDER_TO_CUBEMAP
    Render::inst().CreateTBO(GL_TEXTURE_CUBE_MAP, rt_cubemap);

    glTextureParameteri(rt_cubemap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(rt_cubemap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(rt_cubemap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_cubemap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_cubemap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_cubemap, 1, GL_RGB16F, width, height);

    std::vector<float> buffer(width * height * 3, .25f);

    for (auto face : {0, 1, 2, 3, 4, 5})
        glTextureSubImage3D(rt_cubemap, 0, 0, 0, face, width, height, 1, GL_RGB, GL_FLOAT, std::data(buffer));

    glNamedFramebufferTextureLayer(fbo, GL_COLOR_ATTACHMENT0, rt_cubemap, 0, 0);



    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_depth_irradiance);

    glTextureParameteri(rt_depth_irradiance, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth_irradiance, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth_irradiance, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_depth_irradiance, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_depth_irradiance, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_depth_irradiance, 1, GL_DEPTH_COMPONENT32F, irradiance_width, irradiance_height);


    Render::inst().CreateTBO(GL_TEXTURE_CUBE_MAP, rt_irradiance);

    glTextureParameteri(rt_irradiance, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(rt_irradiance, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(rt_irradiance, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_irradiance, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_irradiance, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_irradiance, 1, GL_RGB16F, irradiance_width, irradiance_height);

#else
    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_2D);

    glTextureParameteri(rt_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(rt_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(rt_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_2D, 1, GL_RGBA8, width, height);

    glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, rt_2D, 0);
#endif

    auto constexpr drawBuffers = make_array<u32>(GL_COLOR_ATTACHMENT0);
    glNamedFramebufferDrawBuffers(fbo, static_cast<i32>(std::size(drawBuffers)), std::data(drawBuffers));

    if (auto result = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        log::Fatal() << "framebuffer error:" << std::hex << result;

    if (auto const code = glGetError(); code != GL_NO_ERROR)
        log::Error() << __FUNCTION__ << ": " << std::hex << code;
}

void render()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);

#if !RENDER_TO_CUBEMAP
    glClearNamedFramebufferfv(fbo, GL_COLOR, 0, std::data(colors::kPOWDERBLUE.rgba));
    glClearNamedFramebufferfv(fbo, GL_DEPTH, 0, &clear_colors[Render::kREVERSED_DEPTH ? 0 : 1]);
#endif

    program.bind();

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index1);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index0);

    glProgramUniformMatrix4fv(program.handle(), 8, 1, GL_FALSE, glm::value_ptr(projection));

    glBindTextureUnit(Render::eSAMPLERS_BINDING::nALBEDO, equirectangular_tex);

#if RENDER_TO_CUBEMAP
    std::for_each(std::begin(views), std::end(views), [face = 0] (auto &&view) mutable
    {
        glProgramUniformMatrix4fv(program.handle(), 9, 1, GL_FALSE, glm::value_ptr(view));

        glNamedFramebufferTextureLayer(fbo, GL_COLOR_ATTACHMENT0, rt_cubemap, 0, face++);

        glClearNamedFramebufferfv(fbo, GL_COLOR, 0, std::data(colors::kPOWDERBLUE.rgba));
        glClearNamedFramebufferfv(fbo, GL_DEPTH, 0, &clear_colors[Render::kREVERSED_DEPTH ? 0 : 1]);

        glBindVertexArray(cube_vao);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cube_ibo);
        glDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr);
    });


    glViewport(0, 0, irradiance_width, irradiance_height);

    glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, rt_depth_irradiance, 0);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index3);

    glBindTextureUnit(Render::eSAMPLERS_BINDING::nNORMAL_MAP, rt_cubemap);

    std::for_each(std::begin(views), std::end(views), [face = 0] (auto &&view) mutable
    {
        glProgramUniformMatrix4fv(program.handle(), 9, 1, GL_FALSE, glm::value_ptr(view));

        glNamedFramebufferTextureLayer(fbo, GL_COLOR_ATTACHMENT0, rt_irradiance, 0, face++);

        glClearNamedFramebufferfv(fbo, GL_COLOR, 0, std::data(colors::kPOWDERBLUE.rgba));
        glClearNamedFramebufferfv(fbo, GL_DEPTH, 0, &clear_colors[Render::kREVERSED_DEPTH ? 0 : 1]);

        glBindVertexArray(cube_vao);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cube_ibo);
        glDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr);
    });
#else
    glBindVertexArray(cube_vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cube_ibo);
    glDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr);
#endif

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void init()
{
    stbi_set_flip_vertically_on_load(true);

    i32 width, height, nrComponents;

    auto const path = R"(../contents/textures/newport-loft-ref.hdr)"s;

    auto data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);

    if (data == nullptr)
        return;

    Render::inst().CreateTBO(GL_TEXTURE_2D, equirectangular_tex);

    glTextureParameteri(equirectangular_tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(equirectangular_tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(equirectangular_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(equirectangular_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(equirectangular_tex, 1, GL_RGB16F, width, height);
    glTextureSubImage2D(equirectangular_tex, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);

    glGenerateTextureMipmap(equirectangular_tex);

    stbi_image_free(data);


    program.AssignNew({R"(Defaults/Radiance-Cube-Map.glsl)"s});

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

    Render::inst().CreateBO(cube_ibo);

    struct command_t {
        u32  count;
        u32  instanceCount;
        u32  first;
        u32  baseInstance;
    };

    command_t constexpr command = {
        static_cast<u32>(std::size(vertices)), 1, 0, 0
    };

    glNamedBufferStorage(cube_ibo, sizeof(command), &command, GL_DYNAMIC_STORAGE_BIT);


    Render::inst().CreateVAO(cube_vao);

    auto bo = 0u;
    Render::inst().CreateBO(bo);

    glNamedBufferStorage(bo, sizeof(vertices), std::data(vertices), GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayAttribBinding(cube_vao, Render::eVERTEX_IN::nPOSITION, 0);
    glVertexArrayAttribFormat(cube_vao, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(cube_vao, 0, bo, 0, sizeof(Position));

    glEnableVertexArrayAttrib(cube_vao, Render::eVERTEX_IN::nPOSITION);

    initFramebuffer();

    render();
}
}

namespace ssao
{
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

Program program;
u32 noise_tex;
u64 noise_tex_handle;

void initMultisampledFramebuffers()
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

    glTextureStorage2DMultisample(ms_rt_depth, samples, GL_DEPTH_COMPONENT32F, app.width, app.height, GL_TRUE);

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

    glTextureStorage2DMultisample(ms_rt_0, samples, GL_RGBA8, app.width, app.height, GL_TRUE);
    //glTextureSubImage2D(quad_tid, 0, 0, 0, width, height, GL_RGBA, GL_RGBA8, nullptr);

    ms_rt_0_handle = glGetTextureHandleARB(ms_rt_0);
    glMakeTextureHandleResidentARB(ms_rt_0_handle);

    Render::inst().CreateTBO(GL_TEXTURE_2D_MULTISAMPLE, ms_rt_1);

    glTextureParameteri(ms_rt_1, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(ms_rt_1, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(ms_rt_1, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(ms_rt_1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(ms_rt_1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2DMultisample(ms_rt_1, samples, GL_RG16F, app.width, app.height, GL_TRUE);

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

void initFramebuffer()
{
    if constexpr (kUSE_MS)
        initMultisampledFramebuffers();

    glCreateFramebuffers(1, &main_fbo);

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_depth);

    glTextureParameteri(rt_depth, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_depth, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_depth, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_depth, 1, GL_DEPTH_COMPONENT32F, app.width, app.height);

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

    glTextureStorage2D(rt_0, 1, GL_RGBA8, app.width, app.height);
    //glTextureSubImage2D(quad_tid, 0, 0, 0, width, height, GL_RGBA, GL_RGBA8, nullptr);

    rt_0_handle = glGetTextureHandleARB(rt_0);
    glMakeTextureHandleResidentARB(rt_0_handle);

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_1);

    glTextureParameteri(rt_1, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rt_1, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_1, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_1, 1, GL_RG16F, app.width, app.height);

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

    glTextureStorage2D(out_rt0, 1, GL_RGBA8, app.width, app.height);

    out_rt0_handle = glGetTextureHandleARB(out_rt0);
    glMakeTextureHandleResidentARB(out_rt0_handle);

    Render::inst().CreateTBO(GL_TEXTURE_2D, out_rt1);

    glTextureParameteri(out_rt1, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(out_rt1, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(out_rt1, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(out_rt1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(out_rt1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(out_rt1, 1, GL_RGBA8, app.width, app.height);

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
        log::Error() << __FUNCTION__ << ": " << std::hex << code;
}

void initGaussFilter(Program &program)
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
        auto index = glGetProgramResourceIndex(program.handle(), GL_SHADER_STORAGE_BLOCK, "GAUSS_FILTER_COLOR_WEIGHTS");

        if (index != GL_INVALID_INDEX) {
            u32 GAUSS_FILTER_COLOR_WEIGHTS = 0;

            Render::inst().CreateBO(GAUSS_FILTER_COLOR_WEIGHTS);
            glNamedBufferStorage(GAUSS_FILTER_COLOR_WEIGHTS, sizeof(decltype(weights)::value_type) * std::size(weights), std::data(weights), 0);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, GAUSS_FILTER_COLOR_WEIGHTS);
            glShaderStorageBlockBinding(program.handle(), index, 5);
        }

        else log::Error() << "can't init the SSBO: invalid index param: " << "GAUSS_FILTER_COLOR_WEIGHTS";
    }

    if constexpr (use_gpu && !use_bf)
    {
        auto index = glGetProgramResourceIndex(program.handle(), GL_SHADER_STORAGE_BLOCK, "GAUSS_FILTER_OFFSETS");

        if (index != GL_INVALID_INDEX) {
            u32 GAUSS_FILTER_OFFSETS = 0;

            Render::inst().CreateBO(GAUSS_FILTER_OFFSETS);
            glNamedBufferStorage(GAUSS_FILTER_OFFSETS, sizeof(decltype(offsets)::value_type) * std::size(offsets), std::data(offsets), 0);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, GAUSS_FILTER_OFFSETS);
            glShaderStorageBlockBinding(program.handle(), index, 6);
        }

        else log::Error() << "can't init the SSBO: invalid index param: " << "GAUSS_FILTER_OFFSETS";
    }
}

void init()
{
    initFramebuffer();

    initGaussFilter(ssao::program);

    std::uniform_real_distribution<float> floats(.1f, 1.f);
    std::default_random_engine generator;

    // The seed.
    std::random_device rd;

    // Mersenne-Twister engine.
    std::mt19937_64 mt(rd());

    auto constexpr kernel_size = 64u;

    std::array<math::Vector, kernel_size> kernel;
    std::generate(kernel.begin(), kernel.end(), [&floats, &mt, size = std::size(kernel), i = 0]() mutable
    {
        math::Vector sample(floats(mt) * 2 - 1, floats(mt) * 2 - 1, floats(mt));

        sample.Normalize();
        sample *= floats(mt);

        auto scale = static_cast<float>(++i) / static_cast<float>(size);
        scale = math::lerp(.1f, 1.f, scale * scale);

        sample *= scale;

        return sample;
    });

    glProgramUniform3fv(program.handle(), 20, kernel_size, &std::data(kernel)->x);

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
        std::generate(noise.begin(), noise.end(), [&floats, &mt] () {
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

void render()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, kUSE_MS ? ms_fbo : main_fbo);

    glViewport(0, 0, app.width, app.height);

    glClearNamedFramebufferfv(kUSE_MS ? ms_fbo : main_fbo, GL_COLOR, 0, std::data(colors::kPOWDERBLUE.rgba));
    glClearNamedFramebufferfv(kUSE_MS ? ms_fbo : main_fbo, GL_COLOR, 1, std::data(colors::kBLACK.rgba));
    glClearNamedFramebufferfv(kUSE_MS ? ms_fbo : main_fbo, GL_DEPTH, 0, &clear_colors[Render::kREVERSED_DEPTH ? 0 : 1]);

    program.bind();

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index0);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index0);

    glBindVertexArray(mesh_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * mesh_count);

    glFinish();

    if constexpr (kUSE_MS)
        glBlitNamedFramebuffer(ms_fbo, main_fbo, 0, 0, app.width, app.height, 0, 0, app.width, app.height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, out_fbo);
    glViewport(0, 0, app.width, app.height);

    glNamedFramebufferTexture(out_fbo, GL_COLOR_ATTACHMENT0, out_rt0, 0);

    glClearNamedFramebufferfv(out_fbo, GL_COLOR, 0, std::data(colors::kPOWDERBLUE.rgba));
    glClearNamedFramebufferfv(out_fbo, GL_DEPTH, 0, &clear_colors[Render::kREVERSED_DEPTH ? 0 : 1]);

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index1);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index1);

    glProgramUniformHandleui64ARB(program.handle(), Render::eSAMPLERS_BINDING::nALBEDO, rt_0_handle);
    glProgramUniformHandleui64ARB(program.handle(), Render::eSAMPLERS_BINDING::nNORMAL_MAP, rt_1_handle);
    glProgramUniformHandleui64ARB(program.handle(), Render::eSAMPLERS_BINDING::nDEPTH, rt_depth_handle);
    //glProgramUniformHandleui64ARB(ssao_program.program(), 4, noise_tex_handle);

    //glBindTextureUnit(Render::eSAMPLERS_BINDING::nALBEDO, rt_0);
    //glBindTextureUnit(Render::eSAMPLERS_BINDING::nNORMAL_MAP, rt_1);
    //glBindTextureUnit(Render::nDEPTH, rt_depth);
    glBindTextureUnit(4, noise_tex);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

#ifndef BLUR
    glFinish();

    glNamedFramebufferTexture(out_fbo, GL_COLOR_ATTACHMENT0, out_rt1, 0);

    /*glClearNamedFramebufferfv(out_fbo, GL_COLOR, 0, colors::kPOWDERBLUE.rgba.data());
    glClearNamedFramebufferfv(out_fbo, GL_DEPTH, 0, &clear_colors[Render::kREVERSED_DEPTH ? 0 : 1]);*/

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index1);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index3);

    glProgramUniformHandleui64ARB(program.handle(), Render::eSAMPLERS_BINDING::nALBEDO, out_rt0_handle);
    glProgramUniformHandleui64ARB(program.handle(), Render::eSAMPLERS_BINDING::nNORMAL_MAP, rt_1_handle);
    glProgramUniformHandleui64ARB(program.handle(), Render::eSAMPLERS_BINDING::nDEPTH, rt_depth_handle);
    //glBindTextureUnit(Render::eSAMPLERS_BINDING::nALBEDO, out_rt0);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
#if 1

    glNamedFramebufferTexture(out_fbo, GL_COLOR_ATTACHMENT0, out_rt0, 0);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index4);

    glProgramUniformHandleui64ARB(program.handle(), Render::eSAMPLERS_BINDING::nALBEDO, out_rt1_handle);
    //glBindTextureUnit(Render::eSAMPLERS_BINDING::nALBEDO, out_rt1);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
#endif
#endif

    glBlitNamedFramebuffer(out_fbo, 0, 0, 0, app.width, app.height, 0, 0, app.width, app.height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

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
}

void Init()
{
    std::vector<glTF::Vertex> vertex_buffer;

    auto future = std::async(std::launch::async, LoadModel<glTF::Vertex>, meshName, std::ref(mesh_count), std::ref(vertex_buffer));

    //grid.Update(15, 1, 5);

    //cubemap::InitCubemap();

    //ssao::init();

    if (!mesh_program.AssignNew({ R"(Defaults/Diffuse-Lambert.glsl)"s }))
        return;

    Render::inst().CreateSSBO<per_object_t>(mesh_program.handle(), 2, "PER_OBJECT"s);
    Render::inst().CreateSSBO<Camera::data_t>(mesh_program.handle(), 3, "PER_CAMERA"s);

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

    ibl::init();

    if (future.get()) {
        Render::inst().CreateVAO(mesh_vao);

        auto bo = 0u;
        Render::inst().CreateBO(bo);

        glNamedBufferStorage(bo, sizeof(decltype(vertex_buffer)::value_type) * std::size(vertex_buffer), std::data(vertex_buffer), 0);

        glVertexArrayAttribBinding(mesh_vao, Render::eVERTEX_IN::nPOSITION, 0);
        glVertexArrayAttribFormat(mesh_vao, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, offsetof(glTF::Vertex, pos));
        glEnableVertexArrayAttrib(mesh_vao, Render::eVERTEX_IN::nPOSITION);

        glVertexArrayAttribBinding(mesh_vao, Render::eVERTEX_IN::nNORMAL, 0);
        glVertexArrayAttribFormat(mesh_vao, Render::eVERTEX_IN::nNORMAL, 3, GL_FLOAT, GL_TRUE, offsetof(glTF::Vertex, normal));
        glEnableVertexArrayAttrib(mesh_vao, Render::eVERTEX_IN::nNORMAL);

        glVertexArrayAttribBinding(mesh_vao, Render::eVERTEX_IN::nTEX_COORD, 0);
        glVertexArrayAttribFormat(mesh_vao, Render::eVERTEX_IN::nTEX_COORD, 2, GL_FLOAT, GL_FALSE, offsetof(glTF::Vertex, uv));
        glEnableVertexArrayAttrib(mesh_vao, Render::eVERTEX_IN::nTEX_COORD);

        glVertexArrayVertexBuffer(mesh_vao, 0, bo, 0, sizeof(decltype(vertex_buffer)::value_type));
    }
}

void Update()
{
    app.cameraController->update();
    app.cameraSystem.update();

    Render::inst().UpdateSSBO("PER_CAMERA"s, app.camera->data);
}

void DrawFrame()
{
    glViewport(0, 0, app.width, app.height);


    ibl::program.bind();

    app.object.world = glm::mat4{1};
    app.object.normal = app.object.world;

    Render::inst().UpdateSSBO("PER_OBJECT"s, app.object);

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index0);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index1);

#if RENDER_TO_CUBEMAP
    glBindTextureUnit(Render::eSAMPLERS_BINDING::nNORMAL_MAP, ibl::rt_cubemap);

    glBindVertexArray(ibl::cube_vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ibl::cube_ibo);
    glDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr);
#else
    glBindTextureUnit(3, ibl::rt_2D);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
#endif

    mesh_program.bind();

    app.object.world = glm::scale(glm::mat4{1}, glm::vec3{.01f});
    app.object.normal = glm::inverseTranspose(app.object.world);

    Render::inst().UpdateSSBO("PER_OBJECT"s, app.object);

    glBindVertexArray(mesh_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * mesh_count);

    //glBlitNamedFramebuffer(ibl::fbo, 0, 0, 0, ibl::width, ibl::height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    isle::Window window(crus::names::kMAIN_WINDOW_NAME, hInstance, app.width, app.height);

    isle::InputManager inputManager{window.hWnd()};
    
    window.AddInputProcessCallback([&inputManager] (auto wParam, auto lParam)
    {
        return inputManager.Process(wParam, lParam);
    });

    window.AddResizeCallback([] (auto width, auto height)
    {
        app.width = width;
        app.height = height;

        app.camera->aspect = static_cast<float>(width) / static_cast<float>(height);
    });

    app.camera = app.cameraSystem.createCamera();
    app.camera->aspect = static_cast<float>(app.width) / static_cast<float>(app.height);

    app.cameraController = std::make_unique<isle::OrbitController>(app.camera, inputManager);

    app.cameraController->lookAt(glm::vec3{0, 4, 4}, {0, 2, 0});

    return isle::System::Loop();
}
