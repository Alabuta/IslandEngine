/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: OpenGL context routine implementations.
****
********************************************************************************************************************************/
#include <algorithm>

#include "Renderer\CrusOpenGLExts.h"
#include "Renderer\CrusOpenGLContext.h"
#include "Renderer\CrusRender.h"

#pragma comment(lib, "OpenGL32.lib")

#define CRUS_OPENGL_DEBUG_OUTPUT 1

#undef max

namespace {

#if CRUS_OPENGL_DEBUG_OUTPUT
auto constexpr kOpenGLDebugSeverityHigh{true};
auto constexpr kOpenGLDebugSeverityMedium{true};
auto constexpr kOpenGLDebugSeverityLow{true};
auto constexpr kOpenGLDebugSeverityNotification{false};

void CALLBACK DebugCallback(GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei _length, GLchar const *_message, void const *_userParam);
#endif

std::pair<HWND, HDC> CreateDummyWindow();
void DestroyDummyWindow(HWND hDummyWnd, HDC hDummyDC);
}

namespace isle {
std::mutex OpenGLContext::mutex_;

HDC OpenGLContext::hMainWndDC_{nullptr};
HGLRC OpenGLContext::hMainRC_{nullptr};

std::stack<HGLRC> OpenGLContext::hSharedRCs_;

OpenGLContext::OpenGLContext() : hRC_(nullptr)
{
    isle::log::Debug() << "OpenGLContext(): " << std::this_thread::get_id();

    std::lock_guard<std::mutex> lock(mutex_);

    if (hMainWndDC_ == nullptr)
        SetupContext();

    else if (hSharedRCs_.size() != 0) {
        hRC_ = hSharedRCs_.top();

        if (wglMakeCurrent(hMainWndDC_, hRC_) == FALSE)
            isle::log::Fatal() << "can't make share context current, thread id: " << std::this_thread::get_id();

        hSharedRCs_.pop();
    }
}

OpenGLContext::~OpenGLContext()
{
    isle::log::Debug() << "~OpenGLContext(): " << std::this_thread::get_id();

    DeleteContext();
}

/*__forceinline*/ isle::OpenGLContext &isle::OpenGLContext::ThreadLocal()
{
    thread_local static OpenGLContext context;
    return context;
}

void OpenGLContext::SetupContext()
{
    auto hMainWnd = FindWindowW(crus::names::kMAIN_WINDOW_CLASS, crus::names::kMAIN_WINDOW_NAME);

    if (hMainWnd == nullptr)
        isle::log::Fatal() << "main window dosen't exist.";

    hMainWndDC_ = GetDC(hMainWnd);

    if (hMainWndDC_ == nullptr)
        isle::log::Fatal() << "can't get window context.";

    auto handlesPair = CreateDummyWindow();

    PIXELFORMATDESCRIPTOR hPFD = {0};

    if (SetPixelFormat(handlesPair.second, 1, &hPFD) == FALSE)
        isle::log::Fatal() << "can't set dummy pixel format.";

    auto const hShareRC = wglCreateContext(handlesPair.second);

    if (hShareRC == nullptr)
        isle::log::Fatal() << "can't create share context.";

    if (wglMakeCurrent(handlesPair.second, hShareRC) == FALSE)
        isle::log::Fatal() << "can't make share context current.";

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

    auto pfs = -1;
    auto num = 0u;

    if (wglChoosePixelFormatARB(hMainWndDC_, attrilist, nullptr, 1, &pfs, &num) == FALSE)
        isle::log::Error() << "wglChoosePixelFormatARB().";

    if (pfs == -1 || num < 1)
        isle::log::Fatal() << "can't choose pixel format.";

    if (SetPixelFormat(hMainWndDC_, pfs, &hPFD) == FALSE)
        isle::log::Fatal() << "can't set renderer pixel format.";

    int32 const attrlist[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,  4,
        WGL_CONTEXT_MINOR_VERSION_ARB,  5,
        WGL_CONTEXT_FLAGS_ARB,          WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,   WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    hMainRC_ = wglCreateContextAttribsARB(hMainWndDC_, nullptr, attrlist);

    if (hMainRC_ == nullptr)
        isle::log::Fatal() << "can't create required render context.";

    hRC_ = hMainRC_;

    auto shareContextsNumber = std::max(std::thread::hardware_concurrency(), 1u) * 2;

    for (auto i = 0u; i < shareContextsNumber; ++i) {
        auto hRC = wglCreateContextAttribsARB(hMainWndDC_, hMainRC_, attrlist);

        if (hRC != nullptr)
            hSharedRCs_.push(hRC);

        else isle::log::Fatal() << "can't create shared render context.";
    }

    if (wglMakeCurrent(hMainWndDC_, hMainRC_) == FALSE)
        isle::log::Fatal() << "can't to make required context current.";

    if (wglDeleteContext(hShareRC) == FALSE)
        isle::log::Fatal() << "share context is not deleted.";

    DestroyDummyWindow(handlesPair.first, handlesPair.second);

#if CRUS_OPENGL_DEBUG_OUTPUT
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallback(DebugCallback, nullptr);

    //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, kOpenGLDebugSeverityHigh ? GL_TRUE : GL_FALSE);
    //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, kOpenGLDebugSeverityMedium ? GL_TRUE : GL_FALSE);
    //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, kOpenGLDebugSeverityLow ? GL_TRUE : GL_FALSE);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, kOpenGLDebugSeverityNotification ? GL_TRUE : GL_FALSE);
    glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_TRUE);
#endif
}

void OpenGLContext::DeleteContext()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (hRC_ != nullptr) {
        if (hRC_ == hMainRC_) {
            while (hSharedRCs_.size() > 0) {
                wglDeleteContext(hSharedRCs_.top());
                hSharedRCs_.pop();
            }

            isle::Render::inst().DeleteContext();


            if (wglMakeCurrent(hMainWndDC_, nullptr) == TRUE) {
                wglDeleteContext(hMainRC_);
                hMainRC_ = nullptr;
            }

            /*if (hMainWndDC_ != nullptr) {
            auto hWnd = WindowFromDC(hMainWndDC_);

            if (hWnd != nullptr)
            ReleaseDC(hWnd, hMainWndDC_);

            hMainWndDC_ = nullptr;
            }*/
        }

        else if (wglMakeCurrent(hMainWndDC_, nullptr) == TRUE) {
            hSharedRCs_.push(hRC_);
        }

        hRC_ = nullptr;
    }
}
}

namespace {
#if CRUS_OPENGL_DEBUG_OUTPUT
void CALLBACK DebugCallback(GLenum _source, GLenum _type, GLuint _id, GLenum _severity,
                            GLsizei _length, GLchar const *_message, void const *_userParam)
{
    UNREFERENCED_PARAMETER(_id);
    UNREFERENCED_PARAMETER(_length);
    UNREFERENCED_PARAMETER(_userParam);

    std::ostringstream debug_message;

    switch (_source) {
        case GL_DEBUG_SOURCE_API:
            debug_message << "[OpenGL API]";
            break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            debug_message << "[Window API]";
            break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            debug_message << "[GLSL]";
            break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
            debug_message << "[Third-party]";
            break;

        case GL_DEBUG_SOURCE_APPLICATION:
            debug_message << "[Application]";
            break;

        case GL_DEBUG_SOURCE_OTHER:
            debug_message << "[Other]";
            break;
    }

    switch (_type) {
        case GL_DEBUG_TYPE_ERROR:
            debug_message << "[Error]";
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            debug_message << "[Deprecated]";
            break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            debug_message << "[Undefined]";
            break;

        case GL_DEBUG_TYPE_PORTABILITY:
            debug_message << "[Portability]";
            break;

        case GL_DEBUG_TYPE_PERFORMANCE:
            debug_message << "[Perfomance]";
            break;

        case GL_DEBUG_TYPE_MARKER:
            debug_message << "[Annotation]";
            break;

        case GL_DEBUG_TYPE_PUSH_GROUP:
            debug_message << "[Group-push]";
            break;

        case GL_DEBUG_TYPE_POP_GROUP:
            debug_message << "[Group-pop]";
            break;

        case GL_DEBUG_TYPE_OTHER:
            debug_message << "[Other]";
            break;
    }

    debug_message << _message;

    switch (_severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            isle::log::Error() << (debug_message.good() ? debug_message.str() : "<empty>");
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            isle::log::Warning() << (debug_message.good() ? debug_message.str() : "<empty>");
            break;

        case GL_DEBUG_SEVERITY_LOW:
            isle::log::Info() << (debug_message.good() ? debug_message.str() : "<empty>");
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            isle::log::Info() << (debug_message.good() ? debug_message.str() : "<empty>");
            break;
    }
}
#endif

std::pair<HWND, HDC> CreateDummyWindow()
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

    if (RegisterClassW(&wcs) == 0ui16)
        return{nullptr, nullptr};

    auto hDummyWnd = CreateWindowW(wcs.lpszClassName, L"", WS_POPUP,
                                   256, 256, 256, 256, nullptr, nullptr, wcs.hInstance, nullptr);

    if (hDummyWnd == nullptr)
        return{nullptr, nullptr};

    auto hDummyDC = GetDC(hDummyWnd);

    if (hDummyDC == nullptr) {
        DestroyWindow(hDummyWnd);
        return{nullptr, nullptr};
    }

    SetWindowPos(hDummyWnd, nullptr, 256, 256, 256, 256, SWP_DRAWFRAME | SWP_NOZORDER);
    ShowWindow(hDummyWnd, SW_HIDE);

    return std::pair<HWND, HDC>(hDummyWnd, hDummyDC);
}

void DestroyDummyWindow(HWND _hDummyWnd, HDC _hDummyDC)
{
    if (_hDummyDC != nullptr && _hDummyWnd != nullptr) {
        ReleaseDC(_hDummyWnd, _hDummyDC);
        _hDummyDC = nullptr;
    }

    if (_hDummyWnd != nullptr) {
        DestroyWindow(_hDummyWnd);
        _hDummyWnd = nullptr;
    };

    UnregisterClassW(L"DummyWindow", GetModuleHandle(nullptr));
}
}