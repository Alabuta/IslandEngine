/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2014 Crusoe's Island LLC.
****
****    Started at 23th July 2009.
****    Description: main cpp file - the beginning and the application end.
****
****************************************************************************************/
#include "engine.h"

#include <string>
#include <random>
#include <algorithm>

namespace app
{
size_t score = 0;
float const kMetersInPxl = 0.002511f;

Texture atlas;
Textout text;

isle::Program background;
uint32 background_vao;

Ground ground;
Bird bird;
std::vector<Pillar> pillars(99);
Title title;
Button button;

void InitBackground()
{
    //512x288
    float const quad_plane[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 288.0f / atlas.w(), 1.0f,
         1.0f, -1.0f, 0.0f, 288.0f / atlas.w(), 0.0f
    };

    background.AssignNew({"FlappyBird/background.glsl"});

    isle::Renderer::inst().CreateVAO(background_vao);

    {
        uint32 vbo = 0;
        isle::Renderer::inst().CreateVBO(GL_ARRAY_BUFFER, vbo);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_plane), quad_plane, GL_STATIC_DRAW);

    glVertexAttribPointer(isle::Program::nVERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
    glEnableVertexAttribArray(isle::Program::nVERTEX);

    glVertexAttribPointer(isle::Program::nTEXCRD, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void const *>(sizeof(float) * 3));
    glEnableVertexAttribArray(isle::Program::nTEXCRD);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawBackgorund()
{
    background.SwitchOn();

    glBindVertexArray(background_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Init()
{
    Camera::inst().Create(Camera::eCAM_BEHAVIOR::nFREE);
    Camera::inst().SetPos(0.0f, 0.0f, 4.0f);
    Camera::inst().LookAt(0.0f, 0.0f, 0.0f);

    // Application intialization function.
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Just one atlas texture for hole objects.
    atlas.Init("atlas.tga");
    atlas.Bind();

    // Full screen quad based background.
    InitBackground();

    Object::program.AssignNew({"FlappyBird/animated.glsl"});

    // Pillars initialization...
    {
        float const zoom = kMetersInPxl * Renderer::inst().vp_.w() * 52 * 0.5f / 288.0f;

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dist(1.0f, 1.2f), height(-0.24f, 0.64f);

        float disp{0.2f}, y{0};

        for (auto &pillar : pillars) {
            disp += dist(mt);
            y = height(mt);

            pillar.Init(&atlas, zoom, 290, 190, 52, 320);
            pillar.mModel().Translate(disp, y, 0);
        }
    }

    // Animated ground initialization...
    {
        float const zoom = kMetersInPxl * Renderer::inst().vp_.w() * 336 * 0.5f / 288.0f;
        ground.Init(&atlas, zoom, 584, 398, 336, 112);
        ground.mModel().Translate(0, -0.64f, 0);
    }

    // Flappy bird initialization...
    {
        float const zoom = kMetersInPxl * Renderer::inst().vp_.w() * 38 * 0.5f / 288.0f;
        bird.Init(&atlas, zoom, 290, 0, 38, 28);
    }

    // Textout initialization...
    {
        float const zoom = 0.64f * kMetersInPxl * Renderer::inst().vp_.w() * 28 / 288.0f;
        text.Init(&atlas, zoom, 290, 30, 28, 40);
    }

    // Title initialization...
    {
        float const zoom = kMetersInPxl * Renderer::inst().vp_.w() * 196 * 0.64f / 288.0f;
        title.Init(&atlas, zoom, 632, 342, 196, 54);
        title.mModel().Translate(Camera::inst().pos().x(), 0.4f, 0.0f);
    }

    // Reset button initialization...
    {
        float const zoom = kMetersInPxl * Renderer::inst().vp_.w() * 118 * 0.5f / 288.0f;
        button.Init(&atlas, zoom, 432, 0, 118, 102);
        button.mModel().Translate(Camera::inst().pos().x(), -0.32f, 0.0f);
    }

    // Setting mouse cursor to screen center.
    RECT ws;
    GetWindowRect(Window::inst().hWnd(), &ws);

    SetCursorPos(ws.left + (Renderer::inst().vp_.w() >> 1), ws.top + (Renderer::inst().vp_.h() >> 1));
}

void CheckCollisions()
{
    float const zoom = kMetersInPxl * Renderer::inst().vp_.w() * 52 * 0.5f / 288.0f;

    if (bird.y() < ground.y() + bird.kRadius) {
        Object::state = Object::eSTATE::nSTOPPED;

        //bird.y() += bird.kRadius;
        bird.Start();
    }

    else if (bird.x() + bird.kRadius > pillars[score].x() - zoom) {
        if (bird.y() + bird.kRadius > pillars[score].y() + 0.16f || bird.y() - bird.kRadius < pillars[score].y() - 0.16f)
            Object::state = Object::eSTATE::nSTOPPED;
    }
}

void Update()
{
    switch (Object::state) {
        case Object::eSTATE::nIDLE:
            score = 0;

            Input::Mouse::CheckKeySync(VK_LBUTTON,
                [&] () {
                Object::state = Object::eSTATE::nPLAYING;
                bird.Start();
            }, [] () {}, [] () {}, [] () {});

            break;

        case Object::eSTATE::nPLAYING:
            if (score >= pillars.size())
                break;

            CheckCollisions();

            // Incrementing score value then passing pillars one by one.
            if (bird.x() - bird.kRadius > pillars[score].x() + bird.kRadius)
                ++score;

            break;

        case Object::eSTATE::nSTOPPED:
            break;
    }

    bird.Update();
    ground.Update();
    title.Update();
    button.Update();
}

void DrawFrame()
{
    DrawBackgorund();

    Object::program.SwitchOn();

    for (auto &pillar : pillars)
        pillar.Render();

    ground.Render();

    title.Render();
    button.Render();

    bird.Render();

    text.SetText(std::to_string(score));
    text.Render();
}
};
//
//int32 main()
//{
//    using namespace isle;
//
//    Window::inst().Create("w-", /*288, 512,*/450, 800);
//     
//    Book::AddEvent(isle::NOTE::nSEPAR);
//    Book::AddEvent(isle::NOTE::nDEBUG, "...");
//
//    Camera::inst().Create(Camera::eCAM_BEHAVIOR::nFREE);
//    Camera::inst().SetPos(0.0f, 0.0f, 4.0f);
//    Camera::inst().LookAt(0.0f, 0.0f, 0.0f);
//
//    return isle::System::Loop();
//}

void EnumScreenModes()
{
    DISPLAY_DEVICEA device = {
        sizeof(DISPLAY_DEVICEA),
        "", "", 0, "", ""
    };

    for (auto iDevNum = 0; EnumDisplayDevicesA(nullptr, iDevNum, &device, 0) != 0; ++iDevNum) {
        isle::Book::AddEvent(isle::NOTE::nDEBUG, "%s|%s", device.DeviceName, device.DeviceString);
    }

    DEVMODEW devmode = {
        L"", 0, 0,
        sizeof(DEVMODEW),
        {0}
    };

    struct DisplaySettings {
        uint32 width, height, frequency;
    };

    std::vector<DisplaySettings> settings(0);

    for (auto iModeNum = 0; EnumDisplaySettingsW(nullptr, iModeNum, &devmode) != 0; ++iModeNum) {
        if (devmode.dmBitsPerPel != 32)
            continue;

        settings.push_back({devmode.dmPelsWidth, devmode.dmPelsHeight, devmode.dmDisplayFrequency});
    }

    std::sort(settings.begin(), settings.end(), [] (DisplaySettings const &a, DisplaySettings const &b) -> bool {
        if (b.width < a.width)
            return true;

        if (b.width == a.width && b.height < a.height)
            return true;

        if (b.width == a.width && b.height == a.height && b.frequency < a.frequency)
            return true;

        return false;
    });

    auto last = std::unique(settings.begin(), settings.end(), [] (DisplaySettings const &a, DisplaySettings const &b) {
        return b.width == a.width && b.height == a.height && b.frequency == a.frequency;
    });

    settings.erase(last, settings.end());

    for (auto const &setting : settings)
        isle::Book::AddEvent(isle::NOTE::nDEBUG, "%dx%d @%d", setting.width, setting.height, setting.frequency);
}

int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int32 nShowCmd)
{
    using namespace isle;

    Window::inst().Create("w-", 450, 800);

    Book::AddEvent(isle::NOTE::nSEPAR);
    Book::AddEvent(isle::NOTE::nDEBUG, "...");

    EnumScreenModes();

    return isle::System::Loop();
}