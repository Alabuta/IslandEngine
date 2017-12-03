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

auto constexpr width = 1920;
auto constexpr height = 1080;

std::array<float, 2> constexpr clear_colors = { 0.f, 1.f };

uint32 main_fbo, rt_0, rt_1, rt_2, rt_depth;
uint32 out_fbo, out_rt, out_depth;

uint32 constexpr index0 = 0, index1 = 1, index2 = 2;

isle::Program ssao_program;

namespace cubemap {
bool InitCubemap();
void DrawCubemap();
}

namespace app {
intf::Grid grid;


Program hemisphere_program;
uint32 hemisphere_vao, hemisphere_count;

Program geom_program;
uint32 geom_vao, geom_count;

Program ssao_program;
uint32 quad_vao, quad_tid, quad_fbo, quad_depth, quad_inter, quad_blur;

uint32 pos_tex, norm_tex, depth_tex, color_tex, noise_tex, ssao_tex, blured_tex;


std::array<math::Matrix, 3> matrices = {
    math::Matrix::Identity(),
    math::Matrix::Identity(),
    math::Matrix::Identity()
};
struct Vertex {
    Position pos;
    math::Vector normal;
    UV uv;
};

std::ostream &operator<< (std::ostream &_stream, Vertex const &v)
{
    return _stream << v.pos << " " << v.normal << " " << v.uv;
}

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

template<typename T>
bool SaveBinaryModel(std::string const &path, std::vector<T> const &vertex_buffer)
{
    if (path.empty()) {
        log::Error() << "file name is invalid.";
        return false;
    }

    std::ofstream file(path + ".bin", std::ios::out | std::ios::trunc | std::ios::binary);

    if (!file.is_open()) {
        log::Error() << "can't open file: " << path;
        return false;
    }

    auto count = static_cast<std::size_t>(vertex_buffer.size());
    file.write(reinterpret_cast<char const *>(&count), sizeof(count));

    file.write(reinterpret_cast<char const *>(vertex_buffer.data()), vertex_buffer.size() * sizeof(std::decay_t<T>));

    return true;
}

template<typename T>
bool LoadBinaryModel(std::string const &path, std::vector<T> &vertex_buffer)
{
    if (path.empty()) {
        log::Error() << "file name is invalid.";
        return false;
    }

    std::ifstream file(path + ".bin", std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        log::Error() << "can't open file: " << path;
        return false;
    }

    std::size_t count = 18348;
    file.read(reinterpret_cast<char *>(&count), sizeof(count));

    if (count < 1 || count % 3 != 0)
        return false;

    vertex_buffer.resize(count);
    file.read(reinterpret_cast<char *>(vertex_buffer.data()), vertex_buffer.size() * sizeof(T));

    return true;
}

void InitSSAO()
{
    std::uniform_real_distribution<float> floats(0.1f, 1.f);
    std::default_random_engine generator;

    // The seed.
    std::random_device rd;

    // Mersenne-Twister engine.
    std::mt19937_64 mt(rd());

    auto constexpr kernel_size = 64u;

    std::array<math::Vector, kernel_size> kernel;
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

    glProgramUniform3fv(ssao_program.program(), 8, kernel_size, &kernel.data()->x);

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

        Render::inst().CreateTBO(GL_TEXTURE_2D, noise_tex);

        glTextureParameteri(noise_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(noise_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(noise_tex, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(noise_tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(noise_tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureStorage2D(noise_tex, 1, GL_RGB32F, 4, 4);
        glTextureSubImage2D(noise_tex, 0, 0, 0, 4, 4, GL_RGB, GL_FLOAT, noise.data());
    }
}

template<typename T>
bool LoadModel(std::string const &path, uint32 &count, std::vector<T> &vertex_buffer)
{
    std::vector<Position> positions;
    std::vector<math::Vector> normals;
    std::vector<UV> uvs;

    std::vector<std::vector<std::size_t>> faces;

    if (!LoadBinaryModel(path, vertex_buffer)) {
        if (LoadOBJ(path, positions, normals, uvs, faces)) {
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

            SaveBinaryModel(path, vertex_buffer);
        }

        else return false;
    }

    count = static_cast<std::decay_t<decltype(count)>>(vertex_buffer.size() / 3);

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
        glNamedFramebufferDrawBuffers(main_fbo, static_cast<int32>(std::size(drawBuffers)), drawBuffers);
    }

    glDisablei(GL_BLEND, 0);
    glBlendFunci(0, GL_ONE, GL_ONE);

    glDisablei(GL_BLEND, 1);
    glBlendFunci(1, GL_ONE, GL_ONE);

    glDisablei(GL_BLEND, 2);
    glBlendFunci(2, GL_ONE, GL_ONE);

    if (auto result = glCheckNamedFramebufferStatus(main_fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        log::Fatal() << "framebuffer error:" << result;

    glCreateFramebuffers(1, &out_fbo);

    Render::inst().CreateTBO(GL_TEXTURE_2D, out_rt);
    glBindTextureUnit(0, out_rt);

    glTextureParameteri(out_rt, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(out_rt, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(out_rt, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(out_rt, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(out_rt, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(out_rt, 1, GL_RGBA8, width, height);

    glNamedFramebufferTexture(out_fbo, GL_COLOR_ATTACHMENT0, out_rt, 0);

    /*glCreateRenderbuffers(1, &out_depth);
    glNamedRenderbufferStorage(out_depth, GL_DEPTH_COMPONENT32F, width, height);
    glNamedFramebufferRenderbuffer(out_fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, out_depth);*/

    {
        std::uint32_t constexpr drawBuffers[] = {GL_COLOR_ATTACHMENT0};
        glNamedFramebufferDrawBuffers(out_fbo, static_cast<int32>(std::size(drawBuffers)), drawBuffers);
    }

    if (auto result = glCheckNamedFramebufferStatus(out_fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        log::Fatal() << "framebuffer error:" << result;

    if (auto const code = glGetError(); code != GL_NO_ERROR)
        log::Error() << __FUNCTION__ << ": " << code;
}

void Init()
{
    std::vector<Vertex> vertex_buffer;

    auto future = std::async(std::launch::async, LoadModel<Vertex>, "../objects.obj", std::ref(geom_count), std::ref(vertex_buffer));

    Camera::inst().Create(Camera::eCAM_BEHAVIOR::nFREE);
    Camera::inst().SetPos(0, 0, 2);
    Camera::inst().LookAt(0, 0, 0);

    grid.Update(15, 1, 5);

    cubemap::InitCubemap();

    InitFramebuffer();

    if (!geom_program.AssignNew({R"(Defaults/Diffuse-Lambert.glsl)"}))
        return;

    if (!ssao_program.AssignNew({R"(Defaults/SSAO1.glsl)"}))
        return;

    InitSSAO();

    Render::inst().CreateVAO(quad_vao);

    {
        std::array<Position, 4> vertices = {{
            {-1.f, +1.f, 0.f},
            {-1.f, -1.f, 0.f},
            {+1.f, +1.f, 0.f},
            {+1.f, -1.f, 0.f}
        }};

        auto bo = 0u;
        Render::inst().CreateBO(bo);

        glNamedBufferStorage(bo, sizeof(vertices), vertices.data(), 0);

        glVertexArrayAttribBinding(quad_vao, Render::eVERTEX_IN::nPOSITION, 0);
        glVertexArrayAttribFormat(quad_vao, Render::eVERTEX_IN::nPOSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(quad_vao, Render::eVERTEX_IN::nPOSITION);

        glVertexArrayVertexBuffer(quad_vao, 0, bo, 0, sizeof(Position));

    }

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

    Render::inst().UpdateViewport(1, 1, &Render::inst().vp_.proj());

    glBindFramebuffer(GL_FRAMEBUFFER, main_fbo);
    glViewport(0, 0, width, height);

    glClearNamedFramebufferfv(main_fbo, GL_COLOR, 0, colors::kPOWDERBLUE.rgba.data());
    glClearNamedFramebufferfv(main_fbo, GL_COLOR, 1, colors::kBLACK.rgba.data());
    //glClearNamedFramebufferfv(main_fbo, GL_COLOR, 2, colors::kBLACK.rgba.data());
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

    glBindFramebuffer(GL_FRAMEBUFFER, out_fbo);
    glViewport(0, 0, width, height);

    glClearNamedFramebufferfv(out_fbo, GL_COLOR, 0, colors::kPOWDERBLUE.rgba.data());
    glClearNamedFramebufferfv(out_fbo, GL_DEPTH, 0, &clear_colors[0]);

    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &index1);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index1);

    glBindTextureUnit(Render::nALBEDO, rt_0);
    glBindTextureUnit(Render::nNORMAL_MAP, rt_1);
    glBindTextureUnit(Render::nDEPTH, rt_depth);
    glBindTextureUnit(4, noise_tex);

    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

    glFinish();

    glBlitNamedFramebuffer(out_fbo, 0, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

#if 0
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    hemisphere_program.UseThis();
    matrices[1].Scale(1, 1, 1);
    matrices[0] = Render::inst().vp_.projView() * matrices[1];
    Render::inst().UpdateTransform(0, 2, matrices.data());

    glBindVertexArray(hemisphere_vao);
    glDrawArrays(GL_POINTS, 0, hemisphere_count);
#endif
}
};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    isle::Window window(crus::names::kMAIN_WINDOW_NAME, hInstance, width, height);

    return isle::System::Loop();
}