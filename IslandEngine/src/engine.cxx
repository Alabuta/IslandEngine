/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2014 Crusoe's Island LLC.
****
****    Description: main cpp file - the beginning and the application end.
****
********************************************************************************************************************************/
#include <map>
#include <random>
#include <algorithm>
#include <thread>
#include <mutex>
#include <future>

#include "engine.h"
#include "Game\CrusBounds.h"
#include "Game\CrusRect.h"
#include "Game\CrusSprite.h"
#include "Renderer\CrusUV.h"

auto index = 0u;
auto spritesNumber = 0u;
auto samples = 0u;

class ISystem {
public:

    virtual ~ISystem() = default;

    virtual void Update() = 0;
};

class RenderSystem final : public ISystem {
public:

    void Update() override
    {
        isle::log::Debug() << __FUNCTION__;
    }
};

class MoveSystem final : public ISystem {
public:

    void Update() override
    {
        isle::log::Debug() << __FUNCTION__;
    }
};

namespace app {
void InitBuffers(std::vector<isle::Sprite> const &spriteSheet);
intf::Grid grid;

Program flipbookProgram;
uint32 flipbook_vao;
Texture flipbookTexture("sprites-cat-running.tga");//RobotBoyWalkSprite
math::Matrix transform;

math::Matrix matrices[] = {
    math::Matrix::GetIdentity(),
    math::Matrix::GetIdentity(),
    math::Matrix::GetIdentity()
};

template <typename T>
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
                _spriteSheet.emplace_back(sprite);

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

    if (!assignedNewProgram.get()) {
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
            vertex_buffer.emplace_back(std::move(Vertex{sprite.vertices()[i], sprite.uvs()[i]}));

    vertex_buffer.shrink_to_fit();

    Render::inst().CreateVAO(flipbook_vao);

    {
        uint16 const indices[] = {
            0, 1, 2, 3
        };

        auto bo = 0u;
        Render::inst().CreateBO(GL_ELEMENT_ARRAY_BUFFER, bo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }

    {
        auto bo = 0u;
        Render::inst().CreateBO(GL_ARRAY_BUFFER, bo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex_buffer.size(), vertex_buffer.data(), GL_STATIC_DRAW);
    }

    glVertexAttribPointer(Program::eIN_OUT_ID::nVERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(Program::eIN_OUT_ID::nVERTEX);

    glVertexAttribPointer(isle::Program::eIN_OUT_ID::nTEXCRD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void const *>(sizeof(Position)));
    glEnableVertexAttribArray(isle::Program::eIN_OUT_ID::nTEXCRD);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawBackgorund()
{
    flipbookTexture.Bind();
    flipbookProgram.UseThis();

    matrices[0] = Render::inst().vp_.projView() * matrices[1];

    Render::inst().UpdateTransform(0, 3, &matrices[0]);

    //glUniformMatrix4fv(Program::eUNIFORM_ID::nTRANSFORM, 1, GL_FALSE, mkas.data());

    index = static_cast<decltype(index)>(std::fmodf(System::time.elapsed() * samples, static_cast<float>(spritesNumber)) * 1);

    glBindVertexArray(flipbook_vao);
    //glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr);
    glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr, index * 4);
}

void Init()
{
    /*std::map<int, std::unique_ptr<ISystem>> systems;

    systems.emplace(0, std::make_unique<RenderSystem>());
    systems.emplace(1, std::make_unique<MoveSystem>());

    for (auto const &system : systems)
        system.second->Update();*/

    Camera::inst().Create(Camera::eCAM_BEHAVIOR::nFREE);
    Camera::inst().SetPos(0.0f, 1.0f, 2.0f);
    Camera::inst().LookAt(0.0f, 0.0f, 0.0f);

    // Application intialization function.
    glDisable(GL_CULL_FACE);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    grid.Update();

    log::Debug() << measure<>::execution(InitBackground);
}

void Update()
{ }

void DrawFrame()
{
    grid.Draw();

    DrawBackgorund();
}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    isle::Window window(crus::names::kMAIN_WINDOW_NAME, hInstance, 600, 400);

    return isle::System::Loop();
}