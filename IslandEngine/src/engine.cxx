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
#include "../contents/geometry/cube.h"


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

//std::future<std::result_of_t<std::decay_t<Function>(std::decay_t<Args>...)>>
//std::future<std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>>
//async(Function &&f, Args &&...args)

template<typename F, typename ...Args>
auto async1(F func, Args &&...args)
{
    std::packaged_task<std::decay_t<Function>(std::decay_t<Args>...) >> task(func);

    auto future = task.get_future();

    std::thread thread(std::move(task), std::forward<Args>(args)...);
    thread.detach();

    return std::move(future);
}

auto async(std::function<bool()> func)
{
    std::packaged_task<bool()> task(func);

    auto future = task.get_future();

    std::thread thread(std::move(task));
    thread.detach();

    return std::move(future);
}

void InitBackground()
{
    //math::Vector v(0, 1, 0);
    /*auto m = math::Matrix::Identity();

    m.Translate(0, 1, 0);
    isle::log::Debug() << m.TransformVector(math::Vector(0, 1, 0));

    m.Translate(0, 2, 0);
    isle::log::Debug() << m.TransformVector(math::Vector(0, 1, 0));

    m.Translate(1, 1, 1);
    isle::log::Debug() << m.TransformVector(math::Vector(0, 1, 0));*/

    matrices[1].Translate(0, 1, -1);
    matrices[0].Translate(-1, 1, 0);

    //auto newProgram = std::move(AssignNewProgram<std::initializer_list<char const *>>(flipbookProgram, {R"(Defaults/Sprites-Default.glsl)"}));
    //flipbookProgram.AssignNew({"Defaults/Sprites-Default.glsl"});

    auto newProgram = std::move(app::async([&program = flipbookProgram]
    {
        if (!program.AssignNew({R"(Defaults/Sprites-Default.glsl)"}))
            return false;

        /*auto index = glGetProgramResourceIndex(program.program(), GL_SHADER_STORAGE_BLOCK, "INSTANCING");

        if (index == GL_INVALID_INDEX)
        log::Fatal() << "can't init the SSBO: invalid index param: " << "INSTANCING";

        Render::inst().CreateBO(INSTANCING);
        glNamedBufferStorage(INSTANCING, sizeof(math::Matrix) * 2, nullptr, GL_DYNAMIC_STORAGE_BIT);

        glShaderStorageBlockBinding(program.program(), index, Program::nINSTANCING);*/

        return true;
    }));

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

            if (auto sprite = Sprite::Create(
                std::make_shared<Texture>(flipbookTexture), i,
                Rect(x * static_cast<float>(spriteWidth), y * static_cast<float>(spriteHeight), static_cast<float>(spriteWidth), static_cast<float>(spriteHeight)),
                Point{spriteWidth * 0.5f, spriteHeight * 0.5f}, pixelsPerUnit))
                _spriteSheet.push_back(std::move(*sprite));

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

    //else glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Program::nINSTANCING, INSTANCING);
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

    /*{
        Render::inst().CreateBO(spriteIndirectBO);

        struct {
            uint32 count;
            uint32 primCount;
            uint32 firstIndex;
            uint32 baseVertex;
            uint32 baseInstance;
        } const command = {
            4, 1, 0, 0, 0
        };

        glNamedBufferStorage(spriteIndirectBO, sizeof(command), &command, GL_DYNAMIC_STORAGE_BIT);
    }*/

    {
        uint16 const indicies[] = {
            0, 1, 2, 3
        };

        auto bo = 0u;
        Render::inst().CreateBO(bo);

        glNamedBufferStorage(bo, sizeof(indicies), indicies, 0);
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

void DrawSprite()
{
    flipbookTexture.Bind();
    flipbookProgram.UseThis();

    glBindVertexArray(flipbook_vao);

    index = static_cast<decltype(index)>(std::fmod(System::time.elapsed() * samples, spritesNumber));

    /*std::sort(matrices.begin(), matrices.end(), [] (auto const &a, auto const &b)
    {
    auto dir_a = a.origin() - Render::inst().vp_.cam().view().origin();
    auto dir_b = b.origin() - Render::inst().vp_.cam().view().origin();

    return dir_a.GetLenght() < dir_b.GetLenght();
    });*/

    matrices[0] = Render::inst().vp_.projView() * matrices[1];

    Render::inst().UpdateTransform(0, 2, matrices.data());

    //glNamedBufferSubData(INSTANCING, 0 * sizeof(math::Matrix), 2 * sizeof(math::Matrix), matrices[0].m().data());

    //glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr);
    glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr, index * 4);
    //glDrawElementsInstancedBaseVertex(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr, 2, index * 4);
    //glBindBuffer(GL_DRAW_INDIRECT_BUFFER, spriteIndirectBO);
    //glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_SHORT, nullptr);

    /*Render::inst().UpdateTransform(0, 1, &ms[1]);
    glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr, index * 4);*/
}



constexpr auto kMAX_COMPONENTS = 64;

class EntityManager;

template<class C>
class ComponentHandle;

class Entity final {
public:

    constexpr Entity(gsl::not_null<EntityManager *> manager, uint64_t id) noexcept : manager_(manager), id_(id) { };

    constexpr bool operator== (Entity const &entity) const noexcept { return entity.id_ == id_; }
    constexpr bool operator!= (Entity const &entity) const noexcept { return entity.id_ != id_; }
    constexpr bool operator< (Entity const &entity) const noexcept { return entity.id_ < id_; }

    operator bool() const
    {
        // TODO: implement validation routine.
        return true;
    }

    void invalidate()
    {
        // TODO: implement invalidation routine.
    }

    void destroy()
    {
        // TODO: destroy and invalidate routine.
    }

    class ID final {
    public:

        constexpr ID() : id_(0) { };
        constexpr explicit ID(uint64_t id) noexcept : id_(id) { };

        constexpr uint64_t operator()() const noexcept { return id_; }

        constexpr bool operator== (ID const &id) const noexcept { return id.id_ == id_; }
        constexpr bool operator!= (ID const &id) const noexcept { return id.id_ != id_; }
        constexpr bool operator< (ID const &id) const noexcept { return id.id_ < id_; }

    private:
        uint64_t id_;
    };

    static constexpr ID INVALID_ID()
    {
        return ID();
    }

    ID id() const noexcept { return id_; }

    std::bitset<kMAX_COMPONENTS> component_mask() const
    {
        // TODO: implement entity's components bitset getter.
        return {};
    }

private:
    ID id_;
    EntityManager *manager_;
};



template<class C>
class ComponentHandle final {
public:

    constexpr ComponentHandle() noexcept : manager_(nullptr) { };

    operator bool() const
    {
        // TODO: implement validation routine.
        return true;
    }

    C const *get() const
    {
        // TODO: asssert validation and return pointer to component
        return nullptr;
    }

    void remove()
    {
        // TODO: remove and destroy routine.
    }

    Entity entity()
    {
        // TODO: validate and return associated entity.
        return {};
    }

    constexpr bool operator== (ComponentHandle<C> const &component) const noexcept { return component.id_ == id_; }
    constexpr bool operator!= (ComponentHandle<C> const &component) const noexcept { return component.id_ != id_; }

private:
    Entity::ID id_;
    EntityManager *manager_;

    constexpr ComponentHandle(gsl::not_null<EntityManager *> manager, Entity::ID id) noexcept : manager_(manager), id_(id) { };
};



class EntityManager final {
public:

    Entity CreateEntity() noexcept
    {
        uint64_t index;

        if (free_list_.empty()) {
            index = ++index_counter_;
        }

        return Entity(this, 0); //++entities
    }

    template<class C, class ... Args, typename = std::enable_if_t<std::is_base_of_v<Component, std::decay_t<C>>>>
    decltype(auto) AddComponent(Args &&...args)
    {
        ;
    }

private:

    uint64_t index_counter_ = 0;
    std::vector<Entity::ID> free_list_;

    std::bitset<kMAX_COMPONENTS> componentsMask;
};

/*struct Component {
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
};*/


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



namespace isle {

template<class C, class = std::enable_if_t<!is_printable_t<std::decay_t<C>> && is_iterable_v<std::decay_t<C>>>>
std::ostream &operator<< (std::ostream &stream, C &&container)
{
    using T = std::decay_t<C>;

    stream << "[ ";
    std::copy(std::cbegin(container), std::cend(container), std::ostream_iterator<typename T::value_type>(stream, " "));
    return stream << "]";
}

struct S {
    int x;
} s;

std::ostream &operator<< (std::ostream &stream, S s)
{
    return stream << "x: " << s.x;
}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    isle::Window window(crus::names::kMAIN_WINDOW_NAME, hInstance, 800, 600);

    using namespace isle;

    std::array<int, 4> array = {1, 2, 3, 4};
    std::string string = "1234";
    std::array<S, 4> ss = {1, 2, 3, 4};

    std::cout << s;
    std::cout << ss;
    std::cout << array;
    std::cout << string;

    //log::Debug() << array;
    //log::Debug() << string;

    //std::copy(cube::positions.cbegin(), cube::positions.cend(), std::ostream_iterator<isle::Position>(std::cerr, " "));

    //isle::log::Debug() << cube::positions;


    return isle::System::Loop();
}