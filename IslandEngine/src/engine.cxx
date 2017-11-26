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
#include <regex>

#include "engine.h"
#include "../../contents/geometry/Hebe"
#include "geometry/torus.h"

auto constexpr width = 1920;
auto constexpr height = 1080;

std::array<float, 5> constexpr clear_colors = { 0.f, 0.f, 0.f, 0.f, 1.f };

uint32 main_fbo, rt_0, rt_1, rt_depth;
uint32 out_fbo, out_rt, out_depth;

uint32 constexpr index0 = 0, index1 = 1, index2 = 2;

isle::Program ssao_program;

namespace cubemap {
bool InitCubemap();
void DrawCubemap();
}

namespace instanced {
bool Init();
void Draw();
}

auto index = 0u;
auto spritesNumber = 0u;
auto samples = 0u;

auto spriteIndirectBO = 0u;

uint32 INSTANCING = 0;

namespace app {
void InitBuffers(std::vector<isle::Sprite> const &spriteSheet);
intf::Grid grid;

uint32 gen_vao, gen_count;

Program hemisphere_program;
uint32 hemisphere_vao, hemisphere_count;

Program geom_program;
uint32 geom_vao, geom_count;

Program quad_program;
uint32 quad_vao, quad_tid, quad_fbo, quad_depth, quad_inter, quad_blur;

uint32 pos_tex, norm_tex, depth_tex, color_tex, noise_tex, ssao_tex, blured_tex;


bool LoadOBJ(std::string const &path, std::vector<Position> &positions, std::vector<math::Vector> &normals, std::vector<UV> &uvs, std::vector<std::vector<std::size_t>> &faces)
{
    if (path.empty()) {
        log::Error() << "file name is invalid.";
        return false;
    }

    std::ifstream file(path, std::ios::in);

    if (!file.is_open()) {
        log::Error() << "can't open file: " << path;
        return false;
    }

    std::vector<std::size_t> face;

    std::array<char, 256> line;
    std::string attribute;

    float x, y, z;
    std::size_t inx;

    while (file.getline(line.data(), line.size())) {
        std::istringstream stream(line.data());

        stream >> attribute;

        if (attribute == "v") {
            stream >> x >> y >> z;
            positions.emplace_back(x, y, z);
        }

        else if (attribute == "vn") {
            stream >> x >> y >> z;
            normals.emplace_back(x, y, z);
        }

        else if (attribute == "vt") {
            stream >> x >> y;
            uvs.emplace_back(x, y);
        }

        else if (attribute == "f") {
            face.clear();

            std::istringstream in(&line[1]);

            bool b = true;

            while (b) {
                in >> inx;

                if (in.eof() || in.bad())
                    b = false;

                else if (in.fail()) {
                    in.clear();
                    in.ignore(std::numeric_limits<std::streamsize>::max(), '/');
                    continue;
                }

                face.emplace_back(inx);
            }

            faces.push_back(face);
        }
    }

    file.close();

    return true;
}

std::array<math::Matrix, 3> matrices = {
    math::Matrix::Identity(),
    math::Matrix::Identity(),
    math::Matrix::Identity()
};


void InitFullscreenQuad()
{
    if (!quad_program.AssignNew({R"(Defaults/SSAO.glsl)"}))
        return;

    std::uniform_real_distribution<float> floats(0.1f, 1.f);
    std::default_random_engine generator;

    // The seed.
    std::random_device rd;

    // Mersenne-Twister engine.
    std::mt19937_64 mt(rd());

    std::array<math::Vector, 64> kernel;
    std::generate(kernel.begin(), kernel.end(), [&floats, &mt, size = kernel.size(), i = 0] () mutable
    {
        math::Vector sample(floats(mt) * 2 - 1, floats(mt) * 2 - 1, floats(mt));

        sample.Normalize();
        sample *= floats(mt);

        auto scale = static_cast<float>(i++) / static_cast<float>(size);
        scale = math::lerp(.1f, 1.f, scale * scale);

        sample *= scale;

        return sample;
    });

    glProgramUniform3fv(quad_program.program(), 8, static_cast<uint32>(kernel.size()), &kernel.data()->x);

    if (hemisphere_program.AssignNew({R"(Defaults/Unlit-Simple.glsl)"})) {
        Render::inst().CreateVAO(hemisphere_vao);

        auto bo = 0u;
        Render::inst().CreateBO(bo);

        hemisphere_count = static_cast<decltype(hemisphere_count)>(kernel.size());

        glNamedBufferStorage(bo, sizeof(math::Vector) * kernel.size(), kernel.data(), 0);

        glVertexArrayAttribBinding(hemisphere_vao, Render::eVERTEX_IN::nPOSITION, 0);
        glVertexArrayAttribFormat(hemisphere_vao, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(hemisphere_vao, Render::eVERTEX_IN::nPOSITION);

        glVertexArrayVertexBuffer(hemisphere_vao, 0, bo, 0, sizeof(math::Vector));
    }

    {
        std::array<math::Vector, 16> noise;
        std::generate(noise.begin(), noise.end(), [&floats, &mt] ()
        {
            return math::Vector(floats(mt) * 2 - 1, floats(mt) * 2 - 1, 0.f).Normalize();
        });

        glBindTextureUnit(4, noise_tex);
        Render::inst().CreateTBO(GL_TEXTURE_2D, noise_tex);

        glTextureParameteri(noise_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(noise_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(noise_tex, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(noise_tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(noise_tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureStorage2D(noise_tex, 1, GL_RGB32F, 4, 4);
        glTextureSubImage2D(noise_tex, 0, 0, 0, 4, 4, GL_RGB, GL_FLOAT, noise.data());
    }

    glCreateFramebuffers(1, &quad_fbo);

    //glCreateRenderbuffers(1, &quad_depth);
    //glNamedRenderbufferStorage(quad_depth, GL_DEPTH24_STENCIL8, width, height);

    {
        glBindTextureUnit(0, color_tex);
        Render::inst().CreateTBO(GL_TEXTURE_2D, color_tex);

        glTextureParameteri(color_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(color_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(color_tex, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(color_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(color_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTextureStorage2D(color_tex, 1, GL_RGBA8, width, height);
        //glTextureSubImage2D(quad_tid, 0, 0, 0, width, height, GL_RGBA, GL_RGBA8, nullptr);
    }

    {
        glBindTextureUnit(1, pos_tex);
        Render::inst().CreateTBO(GL_TEXTURE_2D, pos_tex);

        glTextureParameteri(pos_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(pos_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(pos_tex, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(pos_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(pos_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTextureStorage2D(pos_tex, 1, GL_R32F, width, height);
    }

    {
        glBindTextureUnit(2, norm_tex);
        Render::inst().CreateTBO(GL_TEXTURE_2D, norm_tex);

        glTextureParameteri(norm_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(norm_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(norm_tex, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(norm_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(norm_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTextureStorage2D(norm_tex, 1, GL_RG16F, width, height);
    }

    {
        glBindTextureUnit(3, depth_tex);
        Render::inst().CreateTBO(GL_TEXTURE_2D, depth_tex);

        glTextureParameteri(depth_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(depth_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(depth_tex, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(depth_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(depth_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTextureStorage2D(depth_tex, 1, GL_DEPTH24_STENCIL8, width, height);
    }

    glNamedFramebufferTexture(quad_fbo, GL_COLOR_ATTACHMENT0, color_tex, 0);
    glNamedFramebufferTexture(quad_fbo, GL_COLOR_ATTACHMENT1, pos_tex, 0);
    glNamedFramebufferTexture(quad_fbo, GL_COLOR_ATTACHMENT2, norm_tex, 0);
    glNamedFramebufferTexture(quad_fbo, GL_DEPTH_STENCIL_ATTACHMENT, depth_tex, 0);

    //glNamedFramebufferRenderbuffer(quad_fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, quad_depth);

    std::array<std::uint32_t, 3> constexpr drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,  GL_COLOR_ATTACHMENT2 };
    glNamedFramebufferDrawBuffers(quad_fbo, static_cast<int32>(drawBuffers.size()), drawBuffers.data());

    glDisablei(GL_BLEND, 1);
    glDisablei(GL_BLEND, 2);
    glDisablei(GL_BLEND, 3);

    glBlendFunci(1, GL_ONE, GL_ONE);
    glBlendFunci(2, GL_ONE, GL_ONE);
    glBlendFunci(3, GL_ONE, GL_ONE);

    if (auto result = glCheckNamedFramebufferStatus(quad_fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        log::Error() << "framebuffer error:" << result;

    {
        glCreateFramebuffers(1, &quad_inter);

        glBindTextureUnit(0, ssao_tex);
        Render::inst().CreateTBO(GL_TEXTURE_2D, ssao_tex);

        glTextureParameteri(ssao_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(ssao_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(ssao_tex, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(ssao_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(ssao_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTextureStorage2D(ssao_tex, 1, GL_RGB8, width, height);

        glNamedFramebufferTexture(quad_inter, GL_COLOR_ATTACHMENT0, ssao_tex, 0);

        std::array<std::uint32_t, 1> constexpr drawBuffers1 = {GL_COLOR_ATTACHMENT0};
        glNamedFramebufferDrawBuffers(quad_inter, static_cast<int32>(drawBuffers1.size()), drawBuffers1.data());

        if (auto result = glCheckNamedFramebufferStatus(quad_inter, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
            log::Error() << "framebuffer error:" << result;
    }

    /*{
        glCreateFramebuffers(1, &quad_blur);

        glBindTextureUnit(1, blured_tex);
        Render::inst().CreateTBO(GL_TEXTURE_2D, blured_tex);

        glTextureParameteri(blured_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(blured_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(blured_tex, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(blured_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(blured_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTextureStorage2D(blured_tex, 1, GL_RGBA8, width, height);
    }*/

    Render::inst().CreateVAO(quad_vao);

    struct Vertex {
        Position pos;
        math::Vector normal;
    };

    std::array<Vertex, 4> vertices = {{
        {{-1.f, +1.f, 0.f}, {}},
        {{-1.f, -1.f, 0.f}, {}},
        {{+1.f, +1.f, 0.f}, {}},
        {{+1.f, -1.f, 0.f}, {}}
    }};

    auto bo = 0u;
    Render::inst().CreateBO(bo);

    glNamedBufferStorage(bo, sizeof(Vertex) * vertices.size(), vertices.data(), 0);

    glVertexArrayAttribBinding(quad_vao, Render::eVERTEX_IN::nPOSITION, 0);
    glVertexArrayAttribFormat(quad_vao, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(quad_vao, Render::eVERTEX_IN::nPOSITION);

    glVertexArrayAttribBinding(quad_vao, Render::eVERTEX_IN::nNORMAL, 0);
    glVertexArrayAttribFormat(quad_vao, Render::eVERTEX_IN::nNORMAL, 3, GL_FLOAT, GL_TRUE, sizeof(Position));
    glEnableVertexArrayAttrib(quad_vao, Render::eVERTEX_IN::nNORMAL);

    glVertexArrayVertexBuffer(quad_vao, 0, bo, 0, sizeof(Vertex));
}

void RenderFullscreenQuad()
{
    //quad_texture.Bind();
    //quad_program.UseThis();
    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
}

template<typename T>
bool LoadModel(std::string const &path, uint32 &count, std::vector<T> &vertex_buffer)
{
    std::vector<Position> positions;
    std::vector<math::Vector> normals;
    std::vector<UV> uvs;

    std::vector<std::vector<std::size_t>> faces;

    if (!LoadOBJ(path, positions, normals, uvs, faces))
        return false;

    count = static_cast<std::decay_t<decltype(count)>>(faces.size());

    for (auto &face : faces)
        std::transform(face.begin(), face.end(), face.begin(), [] (auto &&a) { return a - 1; });

    for (auto const &face : faces) {
        for (auto it_index = face.cbegin(); it_index < face.cend(); std::advance(it_index, 2))
            vertex_buffer.emplace_back(T{
            positions.at(*it_index),
            normals.at(*std::next(it_index, 2)),
            uvs.at(*++it_index)
         });
    }

    return true;
}


void InitFramebuffer()
{
    glCreateFramebuffers(1, &main_fbo);

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_depth);

    glTextureParameteri(rt_depth, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_depth, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_depth, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_depth, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_depth, 1, GL_DEPTH_COMPONENT32F, width, height);

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_0);

    glTextureParameteri(rt_0, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rt_0, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_0, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_0, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_0, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_0, 1, GL_RGBA8, width, height);
    //glTextureSubImage2D(quad_tid, 0, 0, 0, width, height, GL_RGBA, GL_RGBA8, nullptr);

    Render::inst().CreateTBO(GL_TEXTURE_2D, rt_1);

    glTextureParameteri(rt_1, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rt_1, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(rt_1, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(rt_1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rt_1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(rt_1, 1, GL_RG16F, width, height);

    glNamedFramebufferTexture(main_fbo, GL_DEPTH_ATTACHMENT, rt_depth, 0);
    glNamedFramebufferTexture(main_fbo, GL_COLOR_ATTACHMENT0, rt_0, 0);
    glNamedFramebufferTexture(main_fbo, GL_COLOR_ATTACHMENT1, rt_1, 0);

    {
        std::uint32_t constexpr drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glNamedFramebufferDrawBuffers(main_fbo, 2, drawBuffers);
    }

    glDisablei(GL_BLEND, 0);
    glBlendFunci(0, GL_ONE, GL_ONE);

    glDisablei(GL_BLEND, 1);
    glBlendFunci(1, GL_ONE, GL_ONE);

    if (auto result = glCheckNamedFramebufferStatus(main_fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        log::Fatal() << "framebuffer error:" << result;

    glCreateFramebuffers(1, &out_fbo);

    /*glCreateRenderbuffers(1, &out_rt);
    glNamedRenderbufferStorage(out_rt, GL_DEPTH24_STENCIL8, width, height);
    glNamedFramebufferRenderbuffer(quad_fbo, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, out_rt);*/

    Render::inst().CreateTBO(GL_TEXTURE_2D, out_rt);
    glBindTextureUnit(0, out_rt);

    glTextureParameteri(out_rt, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(out_rt, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(out_rt, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(out_rt, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(out_rt, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(out_rt, 1, GL_RGBA8, width, height);
    //glTextureSubImage2D(quad_tid, 0, 0, 0, width, height, GL_RGBA, GL_RGBA8, nullptr);

    glNamedFramebufferTexture(out_fbo, GL_COLOR_ATTACHMENT0, out_rt, 0);

    /*glCreateRenderbuffers(1, &out_depth);
    glNamedRenderbufferStorage(out_depth, GL_DEPTH_COMPONENT32F, width, height);
    glNamedFramebufferRenderbuffer(out_fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, out_depth);*/

    {
        std::uint32_t constexpr drawBuffers[] = {GL_COLOR_ATTACHMENT0};
        glNamedFramebufferDrawBuffers(out_fbo, 1, drawBuffers);
    }

    if (auto result = glCheckNamedFramebufferStatus(out_fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        log::Fatal() << "framebuffer error:" << result;
}

void Init()
{
    struct Vertex {
        Position pos;
        math::Vector normal;
        UV uv;
    };

    std::vector<Vertex> vertex_buffer;

    auto future = std::async(std::launch::async, LoadModel<Vertex>, "../objects.obj", std::ref(geom_count), std::ref(vertex_buffer));

    Camera::inst().Create(Camera::eCAM_BEHAVIOR::nFREE);
    Camera::inst().SetPos(0, 0, 2);
    Camera::inst().LookAt(0, 0, 0);

    grid.Update(15, 1, 5);

    // log::Debug() << measure<>::execution(InitBackground);

    cubemap::InitCubemap();

    /*EntityManager entities;
    auto entity = entities.CreateEntity();*/

    InitFullscreenQuad();

    InitFramebuffer();

    if (!geom_program.AssignNew({R"(Defaults/Diffuse-Lambert.glsl)"}))
        return;

    if (!ssao_program.AssignNew({R"(Defaults/SSAO1.glsl)"}))
        return;

    //InitGeometry();
    //InitGeometryGen();

    if (future.get()) {
        Render::inst().CreateVAO(geom_vao);

        auto bo = 0u;
        Render::inst().CreateBO(bo);

        glNamedBufferStorage(bo, sizeof(Vertex) * vertex_buffer.size(), vertex_buffer.data(), 0);

        glVertexArrayAttribBinding(geom_vao, Render::eVERTEX_IN::nPOSITION, 0);
        glVertexArrayAttribFormat(geom_vao, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(geom_vao, Render::eVERTEX_IN::nPOSITION);

        glVertexArrayAttribBinding(geom_vao, Render::eVERTEX_IN::nNORMAL, 0);
        glVertexArrayAttribFormat(geom_vao, Render::eVERTEX_IN::nNORMAL, 3, GL_FLOAT, GL_TRUE, sizeof(Position));
        glEnableVertexArrayAttrib(geom_vao, Render::eVERTEX_IN::nNORMAL);

        glVertexArrayAttribBinding(geom_vao, Render::eVERTEX_IN::nTEX_COORD, 0);
        glVertexArrayAttribFormat(geom_vao, Render::eVERTEX_IN::nTEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(Position) + sizeof(math::Vector));
        glEnableVertexArrayAttrib(geom_vao, Render::eVERTEX_IN::nTEX_COORD);

        glVertexArrayVertexBuffer(geom_vao, 0, bo, 0, sizeof(Vertex));
    }
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
    Render::inst().UpdateTransform(0, 3, matrices.data());

    glBindFramebuffer(GL_FRAMEBUFFER, main_fbo);
    glViewport(0, 0, width, height);

    glClearNamedFramebufferfv(main_fbo, GL_COLOR, 0, &clear_colors[0]);
    glClearNamedFramebufferfv(main_fbo, GL_DEPTH, 0, &clear_colors[0]);

    //cubemap::DrawCubemap();
    //grid.Draw();

    //geom_program.UseThis();

    ssao_program.UseThis();

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index0);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index0);

    glBindVertexArray(geom_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * geom_count);

    glFinish();

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index1);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index2);

    glBindTextureUnit(0, rt_0);
    glBindTextureUnit(1, rt_1);
    glBindTextureUnit(2, rt_depth);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

    glFinish();

    glBlitNamedFramebuffer(main_fbo, 0, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST); //

#if 0

    glFinish();

    glClearNamedFramebufferfv(main_fbo, GL_COLOR, 0, &clear_colors[0]);
    glClearNamedFramebufferfv(main_fbo, GL_DEPTH, 0, &clear_colors[0]);

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index1);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index2);

    glBindTextureUnit(0, rt_depth);
    glBindTextureUnit(1, rt_0);
    glBindTextureUnit(2, rt_1);
    RenderFullscreenQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFinish();

    glBlitNamedFramebuffer(main_fbo, 0, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST); //  | GL_DEPTH_BUFFER_BIT
#endif

    /*glBindFramebuffer(GL_FRAMEBUFFER, out_fbo);

    glViewport(0, 0, width, height);

    glDisablei(GL_DEPTH_TEST, 0);

    glClearNamedFramebufferfv(out_fbo, GL_COLOR, 0, &clear_colors[0]);
    glClearNamedFramebufferfv(out_fbo, GL_DEPTH, 0, &clear_colors[4]);*/

    /*glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glFinish();

    glBlitNamedFramebuffer(out_fbo, 0, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);*/

    /*hemisphere_program.UseThis();

    glBindVertexArray(hemisphere_vao);
    glDrawArrays(GL_POINTS, 0, hemisphere_count);*/

    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 * 6 + 4);
    //glDrawArraysInstancedBaseInstance(GL_TRIANGLE_STRIP, command.first, command.count, command.instanceCount, command.baseInstance);
    //glMultiDrawArrays(GL_TRIANGLE_FAN, first, count, 6);
    //glMultiDrawElements(GL_TRIANGLE_FAN, count, GL_UNSIGNED_BYTE, reinterpret_cast<void const *const *>(indicies), 6);
    //glDrawElements(GL_TRIANGLE_FAN, count[5], GL_UNSIGNED_BYTE, reinterpret_cast<void const *>(sizeof(uint8) * 4));
    
    //flipbookTexture.Bind();
    //flipbookProgram.UseThis();

#if 1
    /*geom_program.UseThis();
    glBindTextureUnit(0, rt_0);

    glBindVertexArray(geom_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * geom_count);*/
#else
    glFinish();

    glBindFramebuffer(GL_FRAMEBUFFER, quad_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearNamedFramebufferfi(quad_fbo, GL_DEPTH_STENCIL, 0, 0.f, 0);

    quad_program.UseThis();

    uint32 const index0 = 0, index1 = 1, index2 = 2;

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index0);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index0);

    glBindVertexArray(geom_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * geom_count);

    /*glBindVertexArray(gen_vao);
    glDrawElements(GL_TRIANGLES, gen_count, GL_UNSIGNED_SHORT, nullptr);*/

    glFinish();

    glBindFramebuffer(GL_FRAMEBUFFER, quad_inter);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index1);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index1);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glProgramUniformMatrix4fv(quad_program.program(), 4, 1, GL_TRUE, Render::inst().vp_.proj().m.data());

    glBindTextureUnit(0, color_tex);
    glBindTextureUnit(1, pos_tex);
    glBindTextureUnit(2, norm_tex);
    glBindTextureUnit(3, depth_tex);
    glBindTextureUnit(4, noise_tex);
    RenderFullscreenQuad();

    glFinish();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTextureUnit(0, ssao_tex);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index2);
    RenderFullscreenQuad();
#endif

    /*glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glFinish();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    quad_program.UseThis();

    uint32 const index0 = 0, index1 = 1, index2 = 2;

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index1);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index2);

    glBindTextureUnit(0, rt_0);
    RenderFullscreenQuad();*/

}
};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    isle::Window window(crus::names::kMAIN_WINDOW_NAME, hInstance, width, height);

    return isle::System::Loop();
}