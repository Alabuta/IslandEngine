/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: implementation of renderer system.
****
********************************************************************************************************************************/
#include <set>

#include "System\CrusSystem.h"
#include "System\CrusWindow.h"
#include "System\CrusSplash.h"

#include "Renderer\CrusRender.h"
#include "Renderer\CrusProgram.h"

#include "Camera\CrusCamera.h"

namespace isle {
Render::Render() { };
Render::~Render() { };

HWND hDummyWnd{nullptr};
HDC hDummyDC{nullptr};

void DestroyDummy()
{
    if (hDummyDC != nullptr && hDummyWnd != nullptr) {
        ReleaseDC(hDummyWnd, hDummyDC);
        hDummyDC = nullptr;
    }

    if (hDummyWnd != nullptr) {
        DestroyWindow(hDummyWnd);
        hDummyWnd = nullptr;
    };

    UnregisterClassW(L"DummyWindow", GetModuleHandle(nullptr));
}

void CreateDummy()
{
    WNDCLASSW const wcs = {
        CS_NOCLOSE,
        (WNDPROC)DefWindowProcW,
        0, 0,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr, nullptr,
        L"DummyWindow"
    };

    if (RegisterClassW(&wcs) == 0ui16) return;

    hDummyWnd = CreateWindowW(wcs.lpszClassName, L"", WS_POPUP,
                              256, 256, 256, 256, nullptr, nullptr, wcs.hInstance, nullptr);

    if (hDummyWnd == nullptr) {
        DestroyDummy();
        return;
    }

    hDummyDC = GetDC(hDummyWnd);

    if (hDummyDC == nullptr) {
        DestroyDummy();
        return;
    }

    SetWindowPos(hDummyWnd, nullptr, 256, 256, 256, 256, SWP_DRAWFRAME | SWP_NOZORDER);
    ShowWindow(hDummyWnd, SW_HIDE);
}

void Render::SetupContext()
{
    CreateDummy();

    PIXELFORMATDESCRIPTOR hPFD = {0};
    if (SetPixelFormat(hDummyDC, 1, &hPFD) == FALSE)
        log::Fatal() << "can't set dummy pixel format.";

    HGLRC const hShareRC = wglCreateContext(hDummyDC);
    if (hShareRC == nullptr)
        log::Fatal() << "can't create share context.";

    if (wglMakeCurrent(hDummyDC, hShareRC) == FALSE)
        log::Fatal() << "can't make share context current.";

    hDC_ = GetDC(Window::inst().hWnd());
    if (hDC_ == nullptr)
        log::Fatal() << "can't get window context.";

    int32 const attrilist[] = {
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
        WGL_SWAP_METHOD_ARB,    WGL_SWAP_EXCHANGE_ARB,
        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB,     24,
        WGL_ALPHA_BITS_ARB,     8,
        WGL_DEPTH_BITS_ARB,     24,
        WGL_STENCIL_BITS_ARB,   8,
        0
    };

    int32 pfs = -1;
    uint32 num = 0;

    if (wglChoosePixelFormatARB(hDC_, attrilist, nullptr, 1, &pfs, &num) == FALSE)
        log::Error() << "wglChoosePixelFormatARB().";

    if (pfs == -1 || num < 1)
        log::Fatal() << "can't choose pixel format.";

    if (SetPixelFormat(hDC_, pfs, &hPFD) == FALSE)
        log::Fatal() << "can't set renderer pixel format.";

    int32 const attrlist[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,  4,
        WGL_CONTEXT_MINOR_VERSION_ARB,  5,
        WGL_CONTEXT_FLAGS_ARB,          WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,   WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC const hRC = wglCreateContextAttribsARB(hDC_, nullptr, attrlist);
    if (hRC == nullptr)
        log::Fatal() << "can't create required context.";

    if (wglMakeCurrent(hDC_, hRC) == FALSE)
        log::Fatal() << "can't to make required context current.";

    if (wglDeleteContext(hShareRC) == FALSE)
        log::Fatal() << "share context is not deleted.";

    DestroyDummy();

    wglSwapIntervalEXT(1);

    //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    //glDebugMessageCallbackARB(Render::DebugCallback, nullptr);

    if (glGetError() != GL_NO_ERROR) {
        log::Error() << __LINE__;
        return;
    }

    InitBufferObjects();

    if (glGetError() != GL_NO_ERROR) {
        log::Error() << __LINE__;
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //glDepthRangedNV(0, 1);

    vp_.Create(intf::Viewport::eVIEWPORT_TYPE::nPERSPECTIVE);
    vp_.SetCamera(&Camera::inst());

    int32 maxAttribs = -1;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);

    if (maxAttribs < 16)
        log::Fatal() << "maximum supported number of vertex attributes is less than required.";

    if (glGetError() != GL_NO_ERROR)
        log::Error() << __LINE__;

    using namespace colors;
    glClearColor(kCLEARGRAY.r(), kCLEARGRAY.g(), kCLEARGRAY.b(), 1.0f);
    //glClearColor(41 / 256.0f, 34 / 256.0f, 37 / 256.0f, 1.0f);
}

void Render::DeleteRC()
{
    HGLRC const hRC = wglGetCurrentContext();

    if (hRC != nullptr) {
        CleanUp();

        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hRC);
    }

    if (hDC_ != nullptr && Window::inst().hWnd() != nullptr) {
        ReleaseDC(Window::inst().hWnd(), hDC_);
        hDC_ = nullptr;
    }
}

bool Render::CreateProgram(uint32 &_program)
{
    if (glIsProgram(_program) == GL_TRUE) {
        log::Warning() << "already used program index: " << _program;
        return false;
    }

    _program = glCreateProgram();

    auto codeError = glGetError();

    if (codeError != GL_NO_ERROR) {
        log::Error() << "some problem with program index: " << codeError;
        return false;
    }

    POs_ = _program;
    return true;
}

bool Render::CreateBO(uint32 _target, uint32 &_bo)
{
    if (_bo != 0 && glIsBuffer(_bo) == GL_TRUE) {
        log::Warning() << "already used buffer object index: " << _bo;
        return false;
    }

    glGenBuffers(1, &_bo);
    glBindBuffer(_target, _bo);

    if (glGetError() != GL_NO_ERROR) {
        log::Error() << "some problem with buffer index.";
        return false;
    }

    BOs_ = _bo;
    return true;
}

bool Render::CreateVAO(uint32 &_vao)
{
    if (glIsVertexArray(_vao) == GL_TRUE) {
        log::Warning() << "already used VAO index: " << _vao;
        return false;
    }

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    if (glGetError() != GL_NO_ERROR) {
        log::Error() << "some problem with vertex array index.";
        return false;
    }

    VAOs_ = _vao;
    return true;
}

bool Render::CreateTBO(uint32 _target, uint32 &_tbo)
{
    if (glIsTexture(_tbo) == GL_TRUE) {
        log::Warning() << "already used texture buffer object index: " << _tbo;
        return false;
    }

    glGenTextures(1, &_tbo);
    glBindTexture(_target, _tbo);

    if (glGetError() != GL_NO_ERROR) {
        log::Error() << "some problem with texture buffer index.";
        return false;
    }

    TBOs_ = _tbo;
    return true;
}

void Render::SetViewport(int16 _x, int16 _y, int16 _w, int16 _h)
{
    vp_.SetViewport(_x, _y, _w, _h);
}

void Render::Update()
{ }

math::Matrix identity = math::Matrix::GetIdentity().Translate(0, 1, 0);

void Render::DrawFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vp_.MakeCurrent();
    vp_.cam().Update();

    math::Matrix const matrices[] = {vp_.projView(), vp_.proj(), vp_.cam().view()};

    UpdateViewport(0, 3, matrices);

    app::DrawFrame();

    SwapBuffers(hDC_);

    if (glGetError() != GL_NO_ERROR)
        log::Error() << "some problem with OpenGL.";

#if _CRUS_SHADER_DEBUG
    Program::CheckError();
#endif
}

/*static*/ __declspec(noinline) Render &Render::inst()
{
    static Render inst;
    return inst;
}

void Render::InitBufferObjects()
{
    Program ubo;

    if (!ubo.AssignNew({"Defaults/Buffer-Objects-Initialization.glsl"}))
        log::Fatal() << "can't init the buffer objects.";

    /*{

        uint32 index = glGetUniformBlockIndex(ubo.GetName(), "CMTS");
        glGetActiveUniformBlockiv(ubo.GetName(), Program::nMATERIAL, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

        if (index == GL_INVALID_INDEX || size < 1)
            log::Fatal() << "can't init the UBO: invalid index param (%s).", "CMTS");

        using namespace colors;
        Color const colors[4] = {
            kDARKGREEN, kSPRINGGREEN, kRED, kNAVYBLUE
        };

        CreateBO(GL_UNIFORM_BUFFER, CMTS_);
        glBufferData(GL_UNIFORM_BUFFER, size, colors, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, index, CMTS_);
        glUniformBlockBinding(ubo.GetName(), Program::nMATERIAL, index);
    }*/

    {
        int32 size = -1;
        uint32 index = glGetUniformBlockIndex(ubo.program(), "VIEWPORT");

        glGetActiveUniformBlockiv(ubo.program(), Program::nVIEWPORT, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

        if (index == GL_INVALID_INDEX || size < 1)
            log::Fatal() << "can't init the UBO: invalid index param: " << "VIEWPORT";

        CreateBO(GL_UNIFORM_BUFFER, VIEWPORT_);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, Program::nVIEWPORT, VIEWPORT_);
        glUniformBlockBinding(ubo.program(), index, Program::nVIEWPORT);
    }

    {
        uint32 index = glGetProgramResourceIndex(ubo.program(), GL_SHADER_STORAGE_BLOCK, "TRANSFORM");

        if (index == GL_INVALID_INDEX)
            log::Fatal() << "can't init the UBO: invalid index param: " << "TRANSFORM";

        CreateBO(GL_SHADER_STORAGE_BUFFER, TRANSFORM_);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(math::Matrix) * 3, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Program::nTRANSFORM, TRANSFORM_);
        glShaderStorageBlockBinding(ubo.program(), index, Program::nTRANSFORM);
    }
}

void CALLBACK Render::DebugCallback(GLenum _source, GLenum _type, GLuint _id, GLenum _severity,
                                    GLsizei _length, char const *_message, void const *_userParam)
{
    UNREFERENCED_PARAMETER(_source);
    UNREFERENCED_PARAMETER(_id);
    UNREFERENCED_PARAMETER(_severity);
    UNREFERENCED_PARAMETER(_length);
    UNREFERENCED_PARAMETER(_userParam);

    /*eNOTE note;

    switch (_type) {
        case GL_DEBUG_TYPE_ERROR_ARB:
            note = eNOTE::nERROR;
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
            note = eNOTE::nWARN;
            break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
            note = eNOTE::nWARN;
            break;
            break;

        case GL_DEBUG_TYPE_PORTABILITY_ARB:
            note = eNOTE::nINFO;
            break;

        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
            note = eNOTE::nWARN;
            break;
            break;

        case GL_DEBUG_TYPE_OTHER_ARB:
            note = eNOTE::nNOTICE;
            break;

        default:
            note = eNOTE::nDEBUG;
    }

    Book::AddEvent(note, _message);*/
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

    for (auto iDevNum = 0; EnumDisplayDevicesA(nullptr, iDevNum, &device, 0) != 0; ++iDevNum)
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
        isle::log::Info() << setting.width << "x" << setting.height << "@" << setting.frequency;
}
};