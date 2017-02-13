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

#include "engine.h"
#include "Game\CrusBounds.h"
#include "Game\CrusRect.h"
#include "Game\CrusSprite.h"
#include "Renderer\CrusUV.h"

int32 index = 0;
int32 spritesNumber = 0;
int32 samples = 0;

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
intf::Grid grid;

Program flipbookProgram;
uint32 flipbook_vao;
Texture flipbookTexture("cat.tga");//RobotBoyWalkSprite
math::Matrix transform;

math::Matrix matrices[] = {
    math::Matrix::GetIdentity(),
    math::Matrix::GetIdentity(),
    math::Matrix::GetIdentity()
};

void InitBackground()
{
    std::map<int, std::unique_ptr<ISystem>> systems;

    systems.emplace(0, std::make_unique<RenderSystem>());
    systems.emplace(1, std::make_unique<MoveSystem>());

    for (auto const &system : systems)
        system.second->Update();

    transform.MakeIdentity();
    matrices[1].Translate(0, 1, 0);

    flipbookTexture.Init();

    flipbookProgram.AssignNew({"Defaults/Sprites-Default.glsl"});

    auto imageWidth = flipbookTexture.w(), imageHeight = flipbookTexture.h();
    auto spritesNumberHorizontally = 2, spritesNumberVertically = 4;
    //auto spritesNumberHorizontally = 7, spritesNumberVertically = 3;
    auto spriteWidth = 512, spriteHeight = 256;
    //auto const spriteWidth = 275.0f, spriteHeight = 275.0f;
    auto const pixelsPerUnit = 200.0f;
    samples = 12;
    //samples = 24;

    if (spritesNumberHorizontally * spriteWidth > imageWidth) {
        spritesNumberHorizontally = static_cast<int32>(imageWidth / spriteWidth);
        log::Warning() << "sprite number was fitted horizontally.";
    }

    if (spritesNumberVertically * spriteHeight > imageHeight) {
        spritesNumberVertically = static_cast<int32>(imageHeight / spriteHeight);
        log::Warning() << "sprite number was fitted vertically.";
    }

    std::vector<Sprite> spriteSheet;

    for (auto j = 0; j < spritesNumberVertically; ++j) {
        for (auto i = 0; i < spritesNumberHorizontally; ++i) {
            auto sprite = Sprite::Create(
                std::make_shared<Texture>(flipbookTexture),
                i + j * spritesNumberHorizontally,
                Rect(i * spriteWidth, j * spriteHeight, spriteWidth, spriteHeight),
                Point{spriteWidth * 0.5f, spriteHeight * 0.5f},
                pixelsPerUnit);

            if (sprite)
                spriteSheet.emplace_back(sprite);
        }
    }

    spriteSheet.shrink_to_fit();

    spritesNumber = static_cast<int32>(spriteSheet.size());

    struct Vertex {
        Position pos;
        UV uv;
    };

    std::vector<Vertex> vertex_buffer;

    for (auto const &sprite : spriteSheet)
        for (auto i = 0; i < sprite.vertices().size(); ++i)
            vertex_buffer.emplace_back(Vertex{sprite.vertices()[i], sprite.uvs()[i]});

    vertex_buffer.shrink_to_fit();

    Render::inst().CreateVAO(flipbook_vao);

    {
        uint16 const indices[] = {
            0, 1, 2, 3
        };

        uint32 bo = 0;
        Render::inst().CreateBO(GL_ELEMENT_ARRAY_BUFFER, bo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }

    {
        uint32 bo = 0;
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
    flipbookProgram.SwitchOn();

    matrices[0] = Render::inst().vp_.projView() * matrices[1];

    Render::inst().UpdateTransform(0, 3, &matrices[0]);

    //glUniformMatrix4fv(Program::eUNIFORM_ID::nTRANSFORM, 1, GL_FALSE, mkas.data());

    index = static_cast<int32>(std::fmodf(System::time.elapsed() * samples, static_cast<float>(spritesNumber)) * 1);

    glBindVertexArray(flipbook_vao);
    //glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr);
    glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr, index * 4);
}

void Init()
{
    Camera::inst().Create(Camera::eCAM_BEHAVIOR::nFREE);
    Camera::inst().SetPos(0.0f, 1.0f, 2.0f);
    Camera::inst().LookAt(0.0f, 0.0f, 0.0f);

    // Application intialization function.
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    grid.Update();

    InitBackground();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

    isle::Window::inst().Create(hInstance, "w-", 600, 400);

    return isle::System::Loop();
}