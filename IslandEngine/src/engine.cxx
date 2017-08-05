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

#include "engine.h"


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

Program flipbookProgram;
uint32 flipbook_vao;
Texture flipbookTexture(Texture::eTEXTURE_TYPE::n2D, "sprites-cat-running");//RobotBoyWalkSprite

std::array<math::Matrix, 2> matrices = {
    math::Matrix::Identity(),
    math::Matrix::Identity()
};

template<typename T>
std::future<bool> AssignNewProgram(Program &_program, T &&_names)
{
    auto handle = [] (Program &program, T &&names)
    {
        return program.AssignNew(std::forward<T>(names));
    };

#if 1
    std::packaged_task<bool(Program &, T &&)> task(handle);

    auto future = task.get_future();

    std::thread thread(std::move(task), std::ref(_program), _names);
    thread.detach();

    return std::move(future);
#else
    return std::move(std::async(/*std::launch::deferred, */handle, std::ref(_program), _names));
#endif
}

void InitBackground()
{
    transform.MakeIdentity();
    matrices[1].Translate(0, 1, 0);

    auto assignedNewProgram = std::move(AssignNewProgram<std::initializer_list<char const *>>(flipbookProgram, {R"(Defaults/Sprites-Default.glsl)"}));
    //flipbookProgram.AssignNew({"Defaults/Sprites-Default.glsl"});

    if (!flipbookTexture.Init())
        return;

    auto imageWidth = flipbookTexture.w(), imageHeight = flipbookTexture.h();
    auto spritesNumberHorizontally = 4u, spritesNumberVertically = 2u;
    //auto spritesNumberHorizontally = 7u, spritesNumberVertically = 3u;
    auto spriteWidth = 512u, spriteHeight = 256u;
    //auto const spriteWidth = 275u, spriteHeight = 275u;
    auto const pixelsPerUnit = 200.0f;
    samples = 12;
    //samples = 24;

    if (spritesNumberHorizontally * spriteWidth > imageWidth) {
        spritesNumberHorizontally = static_cast<uint32>(imageWidth / spriteWidth);
        log::Warning() << "sprites' number was fitted horizontally.";
    }

    if (spritesNumberVertically * spriteHeight > imageHeight) {
        spritesNumberVertically = static_cast<uint32>(imageHeight / spriteHeight);
        log::Warning() << "sprites' number was fitted vertically.";
    }

    std::vector<Sprite> spriteSheet;

    auto threadsNumber = std::max(std::thread::hardware_concurrency(), 1u);
    log::Debug() << "hardware concurrency: " << threadsNumber;

    auto const totalSpritesNumber = spritesNumberVertically * spritesNumberHorizontally;

    threadsNumber = std::min(threadsNumber, totalSpritesNumber);
    log::Debug() << "threads number: " << threadsNumber;

    auto CreateSpritesRoutine = [&] (std::vector<Sprite> &_spriteSheet, uint32 _start, uint32 _end)
    {
        auto x = 0u, y = 0u;

        for (auto i = _start; i < _end; ++i) {
            y = i / spritesNumberHorizontally;
            x = i - y * spritesNumberHorizontally;

            auto sprite = Sprite::Create(
                std::make_shared<Texture>(flipbookTexture), i,
                Rect(x * static_cast<float>(spriteWidth), y * static_cast<float>(spriteHeight), static_cast<float>(spriteWidth), static_cast<float>(spriteHeight)),
                Point{spriteWidth * 0.5f, spriteHeight * 0.5f}, pixelsPerUnit);

            if (sprite)
                _spriteSheet.push_back(std::move(sprite));

            //std::this_thread::yield();
        }
    };

    //std::vector<std::thread> threads;
    std::vector<std::future<void>> futures;
    std::vector<std::vector<Sprite>> spriteSheets(threadsNumber);

    std::vector<int32> counts(threadsNumber, totalSpritesNumber / threadsNumber);

    for (auto i = 0u; i < totalSpritesNumber - (totalSpritesNumber / threadsNumber) * threadsNumber; ++i)
        ++counts[i];

    auto start = 0u, end = 0u;

    for (auto i = 0u; i < threadsNumber; ++i) {
        end = start + counts[i];
        //threads.emplace_back(CreateSpritesRoutine, std::ref(spriteSheets[i]), start, end);
        futures.emplace_back(std::async(CreateSpritesRoutine, std::ref(spriteSheets[i]), start, end));
        start = end;
    }

    /*for (auto &thread : threads)
        thread.join();*/

    for (auto &future : futures)
        future.wait();

    for (auto &sheet : spriteSheets)
        spriteSheet.insert(spriteSheet.end(), sheet.begin(), sheet.end());

    spriteSheet.shrink_to_fit();
    spritesNumber = static_cast<decltype(spritesNumber)>(spriteSheet.size());

    InitBuffers(spriteSheet);

    if (!newProgram.get()) {
        log::Debug() << "invalid shader.";
        return;
    }
}

void InitBuffers(std::vector<isle::Sprite> const &_spriteSheet)
{
    struct Vertex {
        Position pos;
        UV uv;
    };

    std::vector<Vertex> vertex_buffer;

    for (auto const &sprite : _spriteSheet)
        for (auto i = 0; i < sprite.vertices().size(); ++i)
            vertex_buffer.emplace_back(Vertex{sprite.vertices()[i], sprite.uvs()[i]});

    vertex_buffer.shrink_to_fit();

    Render::inst().CreateVAO(flipbook_vao);

    {
        uint16 const indices[] = {
            0, 1, 2, 3
        };

        auto bo = 0u;
        Render::inst().CreateBO(bo);

        glNamedBufferStorage(bo, sizeof(indices), indices, 0);
        glVertexArrayElementBuffer(flipbook_vao, bo);
    }

    {
        auto bo = 0u;
        Render::inst().CreateBO(bo);

        glNamedBufferStorage(bo, sizeof(Vertex) * vertex_buffer.size(), vertex_buffer.data(), 0);

        glVertexArrayAttribBinding(flipbook_vao, Program::eIN_OUT_ID::nVERTEX, 0);
        glVertexArrayAttribFormat(flipbook_vao, Program::eIN_OUT_ID::nVERTEX, 3, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(flipbook_vao, Program::eIN_OUT_ID::nVERTEX);

        glVertexArrayAttribBinding(flipbook_vao, Program::eIN_OUT_ID::nTEXCRD, 0);
        glVertexArrayAttribFormat(flipbook_vao, Program::eIN_OUT_ID::nTEXCRD, 2, GL_FLOAT, GL_FALSE, sizeof(Position));
        glEnableVertexArrayAttrib(flipbook_vao, Program::eIN_OUT_ID::nTEXCRD);

        glVertexArrayVertexBuffer(flipbook_vao, 0, bo, 0, sizeof(Vertex));
    }
}

void DrawBackgorund()
{
    flipbookTexture.Bind();
    flipbookProgram.UseThis();

    glBindVertexArray(flipbook_vao);

    index = static_cast<decltype(index)>(std::fmod(System::time.elapsed() * samples, static_cast<float>(spritesNumber)) * 1);

    /*std::sort(matrices.begin(), matrices.end(), [&] (auto const &a, auto const &b)
    {
        auto dir_a = a.origin() - Render::inst().vp_.cam().view().origin();
        auto dir_b = b.origin() - Render::inst().vp_.cam().view().origin();

        return dir_a.GetLenght() < dir_b.GetLenght();
    });

    std::array<math::Matrix, 2> ms = {
        Render::inst().vp_.projView() * matrices[0],
        Render::inst().vp_.projView() * matrices[1]
    };*/

    matrices[0] = Render::inst().vp_.projView() * matrices[1];

    Render::inst().UpdateTransform(0, 2, matrices.data());

    //glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr);
    glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr, index * 4);

    /*Render::inst().UpdateTransform(0, 1, &ms[1]);
    glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr, index * 4);*/
}





class Entity final {
public:

    constexpr Entity(/*gsl::not_null<EntityManager *const> manager, */uint64_t id) noexcept : id(id) { };

    class ID final {
    public:
        constexpr explicit ID(uint64_t id) noexcept : id_(id) { };

        constexpr uint64_t operator()() const noexcept { return id_; }

        constexpr bool operator== (ID const &id) const noexcept { return id.id_ == id_; }
        constexpr bool operator!= (ID const &id) const noexcept { return id.id_ != id_; }
        constexpr bool operator< (ID const &id) const noexcept { return id.id_ < id_; }

    private:
        uint64_t id_;

        ID() = delete;
    } id;

private:
    ;
};

class EntityManager final {
public:

    constexpr static auto kMAX_COMPONENTS = 64;

    Entity CreateEntity() noexcept
    {
        uint64_t index;

        if (free_list_.empty()) {
            index = ++index_counter_;
        }

        return Entity(0); //++entities
    }

    template<class C, class ... Args, typename = std::enable_if_t<std::is_base_of_v<Component, std::decay_t<C>>>>
    decltype(auto) AddComponent(Args &&...args)
    {
        ;
    }

private:

    uint64_t index_counter_ = 0;
    std::vector<decltype(Entity::id)> free_list_;

    std::bitset<kMAX_COMPONENTS> componentsMask;
};

struct Component {
    static auto constexpr ID = 0;

    Entity entity;

    Component() = default;
    virtual ~Component() = default;

    Component(Entity entity) : entity(entity) { };
};

struct AudioComponent final : public Component {
    static auto constexpr ID = 1;

    float volume;

    AudioComponent(Entity entity, float volume) : Component(entity), volume(volume) { };
};

struct MovementComponent final : Component {
    static auto constexpr ID = 2;

    float initial_speed;
    float max_speed;

    MovementComponent(Entity entity, float initial_speed, float max_speed) : Component(entity), initial_speed(initial_speed), max_speed(max_speed) { };
};


void Init()
{
    Camera::inst().Create(Camera::eCAM_BEHAVIOR::nFREE);
    Camera::inst().SetPos(0.f, 1.f, 1.f);
    Camera::inst().LookAt(0.f, 1.f, 0.f);

    // Application intialization function.
    glEnable(GL_CULL_FACE);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    grid.Update(12.3f, 1.7f, 5);

    log::Debug() << measure<>::execution(InitBackground);

    cubemap::InitCubemap();

    EntityManager entities;
    auto entity = entities.CreateEntity();
}



void Update()
{ }

void DrawFrame()
{
    grid.Draw();

    cubemap::DrawCubemap();

    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 * 6 + 4);
    //glDrawArraysInstancedBaseInstance(GL_TRIANGLE_STRIP, command.first, command.count, command.instanceCount, command.baseInstance);
    //glMultiDrawArrays(GL_TRIANGLE_FAN, first, count, 6);
    //glMultiDrawElements(GL_TRIANGLE_FAN, count, GL_UNSIGNED_BYTE, reinterpret_cast<void const *const *>(indicies), 6);
    //glDrawElements(GL_TRIANGLE_FAN, count[5], GL_UNSIGNED_BYTE, reinterpret_cast<void const *>(sizeof(uint8) * 4));

    glDepthMask(GL_FALSE);
    DrawSprite();
    glDepthMask(GL_TRUE);
}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    isle::Window window(crus::names::kMAIN_WINDOW_NAME, hInstance, 800, 600);

    return isle::System::Loop();
}