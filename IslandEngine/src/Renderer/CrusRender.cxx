/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: implementation of renderer system.
****
********************************************************************************************************************************/
#include <utility>
#include <set>

#include "System\CrusSystem.h"
#include "System\CrusWindow.h"
#include "System\CrusSplash.h"

#include "Renderer\CrusRender.h"
#include "Renderer\CrusProgram.h"

#include "Camera\CrusCamera.h"

namespace isle {
Render::~Render()
{
    //DeleteContext();
}

void Render::Init()
{
    InitBufferObjects();

    wglSwapIntervalEXT(1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //glDepthRangedNV(0, 1);

    auto maxAttribs = -1;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);

    if (maxAttribs < 16)
        log::Fatal() << "maximum supported number of vertex attributes is less than required.";

    using namespace colors;
    glClearColor(kCLEARGRAY.r(), kCLEARGRAY.g(), kCLEARGRAY.b(), 1.0f);
    //glClearColor(41 / 256.0f, 34 / 256.0f, 37 / 256.0f, 1.0f);

    vp_.Create(intf::Viewport::eVIEWPORT_TYPE::nPERSPECTIVE);
    vp_.SetCamera(&Camera::inst());
}

void Render::DeleteContext()
{
    CleanUp();
}

bool Render::CreateProgram(uint32 &_program)
{
    if (glIsProgram(_program) == GL_TRUE) {
        log::Warning() << "already used program index: " << _program;
        return false;
    }

    _program = glCreateProgram();

    glObjectLabel(GL_PROGRAM, _program, -1, "[Program]");

    if (glGetError() != GL_NO_ERROR)
        return false;

    POs_ = _program;
    return true;
}

bool Render::CreateBO(uint32 &_bo)
{
    if (_bo != 0 && glIsBuffer(_bo) == GL_TRUE) {
        log::Warning() << "already used buffer object index: " << _bo;
        return false;
    }

    glCreateBuffers(1, &_bo);

    glObjectLabel(GL_BUFFER, _bo, -1, "[BO]");

    if (glGetError() != GL_NO_ERROR)
        return false;

    BOs_ = _bo;
    return true;
}

bool Render::CreateVAO(uint32 &_vao)
{
    if (glIsVertexArray(_vao) == GL_TRUE) {
        log::Warning() << "already used VAO index: " << _vao;
        return false;
    }

    glCreateVertexArrays(1, &_vao);

    glObjectLabel(GL_VERTEX_ARRAY, _vao, -1, "[VAO]");

    if (glGetError() != GL_NO_ERROR)
        return false;

    VAOs_ = _vao;
    return true;
}

bool Render::CreateTBO(uint32 _target, uint32 &_tbo)
{
    if (glIsTexture(_tbo) == GL_TRUE) {
        log::Warning() << "already used texture buffer object index: " << _tbo;
        return false;
    }

    glCreateTextures(_target, 1, &_tbo);

    glObjectLabel(GL_TEXTURE, _tbo, -1, "[TO]");

    if (glGetError() != GL_NO_ERROR)
        return false;

    TBOs_ = _tbo;
    return true;
}

void Render::SetViewport(int16 _x, int16 _y, int16 _w, int16 _h)
{
    vp_.SetViewport(_x, _y, _w, _h);
}

void Render::Update()
{ }

math::Matrix identity = math::Matrix::Identity().Translate(0, 1, 0);

void Render::DrawFrame()
{
    //vp_.MakeCurrent();
    vp_.cam().Update();

    math::Matrix const matrices[] = {vp_.projView(), vp_.proj(), vp_.cam().view(), vp_.viewport_};

    UpdateViewport(0, 4, matrices);

    //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "[Frame]");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    app::DrawFrame();

    SwapBuffers(OpenGLContext::hMainWndDC());

    //glPopDebugGroup();
}

/*static*/ __declspec(noinline) Render &Render::inst()
{
    static Render inst;

    return inst;
}

void Render::InitBufferObjects()
{
    Program ubo;

    if (!ubo.AssignNew({R"(Defaults/Buffer-Objects-Initialization.glsl)"}))
        log::Fatal() << "can't init the buffer objects.";

    {
        auto index = glGetUniformBlockIndex(ubo.program(), "VIEWPORT");

        auto size = -1;
        glGetActiveUniformBlockiv(ubo.program(), Program::nVIEWPORT, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

        if (index == GL_INVALID_INDEX || size < 1)
            log::Fatal() << "can't init the UBO: invalid index param: " << "VIEWPORT";

        CreateBO(VIEWPORT_);
        glNamedBufferStorage(VIEWPORT_, size, nullptr, GL_DYNAMIC_STORAGE_BIT);

        glBindBufferBase(GL_UNIFORM_BUFFER, Program::nVIEWPORT, VIEWPORT_);
        glUniformBlockBinding(ubo.program(), index, Program::nVIEWPORT);
    }

#if _CRUS_TEMP_DISABLED
    {
        auto index = glGetProgramResourceIndex(ubo.program(), GL_SHADER_STORAGE_BLOCK, "VIEWPORT");

        if (index == GL_INVALID_INDEX)
            log::Fatal() << "can't init the SSBO: invalid index param: " << "VIEWPORT";

        CreateBO(VIEWPORT_);
        glNamedBufferStorage(VIEWPORT_, sizeof(math::Matrix) * 3, nullptr, GL_DYNAMIC_STORAGE_BIT);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Program::nVIEWPORT, VIEWPORT_);
        glShaderStorageBlockBinding(ubo.program(), index, Program::nVIEWPORT);
    }
#endif

    {
        auto index = glGetProgramResourceIndex(ubo.program(), GL_SHADER_STORAGE_BLOCK, "TRANSFORM");

        if (index == GL_INVALID_INDEX)
            log::Fatal() << "can't init the SSBO: invalid index param: " << "TRANSFORM";

        CreateBO(TRANSFORM_);
        glNamedBufferStorage(TRANSFORM_, sizeof(math::Matrix) * 4, nullptr, GL_DYNAMIC_STORAGE_BIT);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Program::nTRANSFORM, TRANSFORM_);
        glShaderStorageBlockBinding(ubo.program(), index, Program::nTRANSFORM);
    }
}

void Render::CleanUp()
{
    BOs_++, VAOs_++, TBOs_++, POs_++;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_1D, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_3D, 0);

    glUseProgram(0);

    uint32 shaders[3];
    int32 count;

    while (--POs_) {
        if (glIsProgram(POs_) == GL_FALSE)
            continue;

        glGetAttachedShaders(POs_, 3, &count, shaders);

        while (--count > -1) {
            if (glIsShader(shaders[count]) == GL_FALSE)
                continue;

            glDetachShader(POs_, shaders[count]);
            glDeleteShader(shaders[count]);
        }

        glDeleteProgram(POs_);
    }

    while (--TBOs_)
        if (glIsTexture(TBOs_) == GL_TRUE)
            glDeleteTextures(1, &TBOs_);

    while (--VAOs_)
        if (glIsVertexArray(VAOs_) == GL_TRUE)
            glDeleteVertexArrays(1, &VAOs_);

    while (--BOs_)
        if (glIsBuffer(BOs_) == GL_TRUE)
            glDeleteBuffers(1, &BOs_);
}

void EnumScreenModes()
{
    DISPLAY_DEVICEA device = {
        sizeof(DISPLAY_DEVICEA),
        "", "", 0, "", ""
    };

    for (auto iDevNum = 0; EnumDisplayDevicesA(nullptr, iDevNum, &device, 0) != FALSE; ++iDevNum)
        isle::log::Debug() << device.DeviceName << "|" << device.DeviceString;

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

    for (auto iModeNum = 0; EnumDisplaySettingsW(nullptr, iModeNum, &devmode) != 0; ++iModeNum) {
        if (devmode.dmBitsPerPel != 32)
            continue;

        settings.insert({devmode.dmPelsWidth, devmode.dmPelsHeight, devmode.dmDisplayFrequency});
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
        isle::log::Info() << setting.width << "x" << setting.height << "@" << setting.frequency;
}
};