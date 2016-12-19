/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2014 Crusoe's Island LLC.
****
****    Started at 23th July 2009.
****    Description: main cpp file - the beginning and the application end.
****
********************************************************************************************************************************/
#include "engine.h"
#include "Game\CrusBounds.h"
#include "Game\CrusRect.h"
#include "Game\CrusSprite.h"

#include <iostream>

#include <string>
#include <random>
#include <algorithm>
#include <set>

extern void f();

namespace app {
Program background;
uint32 background_vao;

intf::Grid grid;

Texture texture("cat.tga");

void InitBackground()
{
    //512x288
    float const quad_plane[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };

    background.AssignNew({"test.glsl"});

    Render::inst().CreateVAO(background_vao);

    {
        uint32 vbo = 0;
        Render::inst().CreateVBO(GL_ARRAY_BUFFER, vbo);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_plane), quad_plane, GL_STATIC_DRAW);

    glVertexAttribPointer(Program::eLAYOUT_ID::nVERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
    glEnableVertexAttribArray(Program::nVERTEX);

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
    Camera::inst().SetPos(0.0f, 1.0f, 2.0f);
    Camera::inst().LookAt(0.0f, 1.0f, 0.0f);

    // Application intialization function.
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    grid.Update();

    texture.Init();
    texture.Bind();

    InitBackground();

    auto sprite = Sprite::Create(std::make_shared<Texture>(texture), Rect(512, 512, 512, 256), math::Point{256, 128}, 200);
}

void Update()
{}

void DrawFrame()
{
    grid.Draw();

    DrawBackgorund();
}
};

void EnumScreenModes()
{
    DISPLAY_DEVICEA device = {
        sizeof(DISPLAY_DEVICEA),
        "", "", 0, "", ""
    };

    for (auto iDevNum = 0; EnumDisplayDevicesA(nullptr, iDevNum, &device, 0) != 0; ++iDevNum)
        isle::Book::AddEvent(isle::eNOTE::nDEBUG, "%s|%s", device.DeviceName, device.DeviceString);

    DEVMODEW devmode = {
        L"", 0, 0,
        sizeof(DEVMODEW),
        {0}
    };

    struct DisplaySettings {
        uint32 width, height, frequency;

        bool operator< (DisplaySettings const &b) const
        {
            if (b.width < width)
                return true;

            if (b.width == width && b.height < height)
                return true;

            if (b.width == width && b.height == height && b.frequency < frequency)
                return true;

            return false;
        }
    };

    std::set<DisplaySettings> settings;
    //std::vector<DisplaySettings> settings(0);

    for (auto iModeNum = 0; EnumDisplaySettingsW(nullptr, iModeNum, &devmode) != 0; ++iModeNum) {
        if (devmode.dmBitsPerPel != 32)
            continue;

        settings.insert({devmode.dmPelsWidth, devmode.dmPelsHeight, devmode.dmDisplayFrequency});
        //settings.push_back({devmode.dmPelsWidth, devmode.dmPelsHeight, devmode.dmDisplayFrequency});
    }

    /*std::sort(settings.begin(), settings.end(), [] (DisplaySettings const &a, DisplaySettings const &b) -> bool {
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

    settings.erase(last, settings.end());*/

    for (auto const &setting : settings)
        isle::Book::AddEvent(isle::eNOTE::nDEBUG, "%dx%d @%d", setting.width, setting.height, setting.frequency);
}

int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int32 nShowCmd)
{
    using namespace isle;

    Window::inst().Create(hInstance, "w-", 600, 400);

    Book::AddEvent(isle::eNOTE::nSEPAR);

    //EnumScreenModes();

    return System::Loop();
}