/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2014 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****    Description: implementation of renderer system.
****
********************************************************************************************************************************/
#include "System\CrusSystem.h"
#include "System\CrusWindow.h"
#include "System\CrusSplash.h"

#include "Renderer\CrusRenderer.h"
#include "Renderer\CrusProgram.h"

#include "Interface\CrusCamera.h"

namespace isle
{
Renderer::Renderer(){};
Renderer::~Renderer(){};

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

    if (hDummyWnd == nullptr)
        DestroyDummy();

    hDummyDC = GetDC(hDummyWnd);

    if (hDummyDC == nullptr)
        DestroyDummy();

    SetWindowPos(hDummyWnd, nullptr, 256, 256, 256, 256, SWP_DRAWFRAME | SWP_NOZORDER);
    ShowWindow(hDummyWnd, SW_HIDE);
}

void Renderer::SetupContext()
{
    CreateDummy();

    PIXELFORMATDESCRIPTOR hPFD;
    if (SetPixelFormat(hDummyDC, 1, &hPFD) == FALSE)
        Book::AddEvent(eNOTE::nALERT, "can't set dummy pixel format.");

    HGLRC const hShareRC = wglCreateContext(hDummyDC);
    if (hShareRC == nullptr)
        Book::AddEvent(eNOTE::nALERT, "can't create share context.");

    if (wglMakeCurrent(hDummyDC, hShareRC) == FALSE)
        Book::AddEvent(eNOTE::nALERT, "can't make share context current.");

    hDC_ = GetDC(Window::inst().hWnd());
    if (hDC_ == nullptr)
        Book::AddEvent(eNOTE::nALERT, "can't get window context.");

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
        Book::AddEvent(eNOTE::nERROR, "wglChoosePixelFormatARB().");

    if (pfs == -1 || num < 1)
        Book::AddEvent(eNOTE::nALERT, "can't choose pixel format.");

    if (SetPixelFormat(hDC_, pfs, &hPFD) == FALSE)
        Book::AddEvent(eNOTE::nALERT, "can't set renderer pixel format.");

    int32 const attrlist[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,  4,
        WGL_CONTEXT_MINOR_VERSION_ARB,  5,
        WGL_CONTEXT_FLAGS_ARB,          WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,   WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC const hRC = wglCreateContextAttribsARB(hDC_, nullptr, attrlist);
    if (hRC == nullptr)
        Book::AddEvent(eNOTE::nALERT, "can't create required context.");

    if (wglMakeCurrent(hDC_, hRC) == FALSE)
        Book::AddEvent(eNOTE::nALERT, "can't to make required context current.");

    if (wglDeleteContext(hShareRC) == FALSE)
        Book::AddEvent(eNOTE::nALERT, "share context is not deleted.");

    DestroyDummy();

    wglSwapIntervalEXT(1);

    //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    //glDebugMessageCallbackARB(Renderer::DebugCallback, nullptr);

    if (glGetError() != GL_NO_ERROR) {
        Book::AddEvent(eNOTE::nERROR, "%d", __LINE__);
        return;
    }

    InitUniformBuffer();

    if (glGetError() != GL_NO_ERROR) {
        Book::AddEvent(eNOTE::nERROR, "%d", __LINE__);
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //glDepthRangedNV(0, 1);

    vp_.Create(intf::Viewport::eVIEWPORT_TYPE::nPERSPECTIVE);
    vp_.SetCamera(&Camera::inst());

    if (glGetError() != GL_NO_ERROR) {
        Book::AddEvent(eNOTE::nERROR, "%d", __LINE__);
        return;
    }

    //glClearColor(kCLEARGRAY.r(), kCLEARGRAY.g(), kCLEARGRAY.b(), 1.0f);
    glClearColor(41 / 256.0f, 34 / 256.0f, 37 / 256.0f, 1.0f);
}

void Renderer::DeleteRC()
{
    HGLRC const hRC = wglGetCurrentContext();

    if(hRC != nullptr){
        CleanUp();

        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hRC);
    }

    if(hDC_ != nullptr && Window::inst().hWnd() != nullptr){
        ReleaseDC(Window::inst().hWnd(), hDC_);
        hDC_ = nullptr;
    }

    Book::AddEvent(eNOTE::nNOTICE, "renderer context deleted.");
}

bool Renderer::CreateProgram(uint32 &_program)
{
    if(glIsProgram(_program) == GL_TRUE){
        Book::AddEvent(eNOTE::nWARN, "already used program index: #%u.", _program);
        return false;
    }

    _program = glCreateProgram();

    auto codeError = glGetError();

    if (codeError != GL_NO_ERROR) {
        Book::AddEvent(eNOTE::nERROR, "some problem with program index: 0%x.", codeError);
        return false;
    }

    POs_ = _program;
    return true;
}

bool Renderer::CreateVBO(uint32 _target, uint32 &_vbo)
{
    if(_vbo != 0 && glIsBuffer(_vbo) == GL_TRUE){
        Book::AddEvent(eNOTE::nWARN, "already used VBO index: #%u.", _vbo);
        return false;
    }

    glGenBuffers(1, &_vbo);
    glBindBuffer(_target, _vbo);

    if(glGetError() != GL_NO_ERROR){
        Book::AddEvent(eNOTE::nERROR, "some problem with vertex buffer index.");
        return false;
    }

    VBOs_ = _vbo;
    return true;
}

bool Renderer::CreateVAO(uint32 &_vao)
{
    if(glIsVertexArray(_vao) == GL_TRUE){
        Book::AddEvent(eNOTE::nWARN, "already used VAO index: #%u.", _vao);
        return false;
    }

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    if(glGetError() != GL_NO_ERROR){
        Book::AddEvent(eNOTE::nERROR, "some problem with vertex array index.");
        return false;
    }

    VAOs_ = _vao;
    return true;
}

bool Renderer::CreateTBO(uint32 _target, uint32 &_tbo)
{
    if(glIsTexture(_tbo) == GL_TRUE){
        Book::AddEvent(eNOTE::nWARN, "already used TBO index: #%u.", _tbo);
        return false;
    }

    glGenTextures(1, &_tbo);
    glBindTexture(_target, _tbo);

    if(glGetError() != GL_NO_ERROR){
        Book::AddEvent(eNOTE::nERROR, "some problem with texture buffer index.");
        return false;
    }

    TBOs_ = _tbo;
    return true;
}

void Renderer::SetViewport(int16 _x, int16 _y, int16 _w, int16 _h)
{
    vp_.SetViewport(_x, _y, _w, _h);
}

void Renderer::Update()
{
}

void Renderer::DrawFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vp_.MakeCurrent();
    vp_.cam().Update();

    math::Matrix matrices[] = {vp_.projView(), vp_.cam().view(), vp_.proj()};

    UpdateTRSM(2, 3, matrices);

    //(*_appLoop)();

    app::DrawFrame();

    SwapBuffers(hDC_);

    if (glGetError() != GL_NO_ERROR)
        Book::AddEvent(eNOTE::nERROR, "some problem with OpenGL.");

#if _CRUS_SHADER_DEBUG
    Program::CheckError();
#endif
}

/*static*/ __declspec(noinline) Renderer &Renderer::inst()
{
    static Renderer inst;
    return inst;
}

void Renderer::InitUniformBuffer()
{
    Program ubo;

    if (!ubo.AssignNew({"ubo_init.glsl"}))
        Book::AddEvent(eNOTE::nCRITIC, "can't init the UBO.");

    int32 size = -1;

    /*{
        uint32 index = glGetUniformBlockIndex(ubo.GetName(), "ATLAS");
        glGetActiveUniformBlockiv(ubo.GetName(), Program::nATLAS, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

        if (index == GL_INVALID_INDEX || size < 1)
            Book::AddEvent(eNOTE::nCRITIC, "can't init the UBO: invalid index param (%s).", "ATLAS");

        float const vecs[2][2] = {
            { 0, 0 },{ 0, 0 }
        };

        CreateVBO(GL_UNIFORM_BUFFER, ATLAS_);
        glBufferData(GL_UNIFORM_BUFFER, size, vecs, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, index, ATLAS_);
        glUniformBlockBinding(ubo.GetName(), Program::nATLAS, index);
    }*/

    {
        uint32 index = glGetUniformBlockIndex(ubo.GetName(), "CMTS");
        glGetActiveUniformBlockiv(ubo.GetName(), Program::nMATERIAL, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

        if(index == GL_INVALID_INDEX || size < 1)
            Book::AddEvent(eNOTE::nCRITIC, "can't init the UBO: invalid index param (%s).", "CMTS");

        Color const colors[4] = {
            kDARKGREEN, kSPRINGGREEN, kRED, kNAVYBLUE
        };

        CreateVBO(GL_UNIFORM_BUFFER, CMTS_);
        glBufferData(GL_UNIFORM_BUFFER, size, colors, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, index, CMTS_);
        glUniformBlockBinding(ubo.GetName(), Program::nMATERIAL, index);
    }

    {
        uint32 index = glGetUniformBlockIndex(ubo.GetName(), "TRSM");
        glGetActiveUniformBlockiv(ubo.GetName(), Program::nTRANSFORM, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

        if(index == GL_INVALID_INDEX || size < 1)
            Book::AddEvent(eNOTE::nCRITIC, "can't init the UBO: invalid index param (%s).", "TRSM");

        CreateVBO(GL_UNIFORM_BUFFER, TRSM_);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, index, TRSM_);
        glUniformBlockBinding(ubo.GetName(), Program::nTRANSFORM, index);
    }
}

void CALLBACK Renderer::DebugCallback(GLenum _source, GLenum _type, GLuint _id, GLenum _severity,
                                      GLsizei _length, char const *_message, void const *_userParam)
{
    UNREFERENCED_PARAMETER(_source);
    UNREFERENCED_PARAMETER(_id);
    UNREFERENCED_PARAMETER(_severity);
    UNREFERENCED_PARAMETER(_length);
    UNREFERENCED_PARAMETER(_userParam);

    eNOTE note;

    switch(_type){
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

    Book::AddEvent(note, _message);
}

void Renderer::CleanUp()
{
    Book::AddEvent(eNOTE::nINFO, "VBOs|VAOs|TBOs|PBOs: %u|%u|%u|%u.",
                                 VBOs_++, VAOs_++, TBOs_++, POs_++);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_1D, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_3D, 0);

    glUseProgram(0);

    uint32 shaders[3];
    int32 count;

    while(--POs_){
        if(glIsProgram(POs_) == GL_FALSE)
            continue;

            glGetAttachedShaders(POs_, 3, &count, shaders);

            while(--count > -1){
                if(glIsShader(shaders[count]) == GL_FALSE)
                    continue;

                glDetachShader(POs_, shaders[count]);
                glDeleteShader(shaders[count]);
            }

            glDeleteProgram(POs_);
    }

    while(--TBOs_)
        if(glIsTexture(TBOs_) == GL_TRUE)
            glDeleteTextures(1, &TBOs_);

    while(--VAOs_)
        if(glIsVertexArray(VAOs_) == GL_TRUE)
            glDeleteVertexArrays(1, &VAOs_);

    while(--VBOs_)
        if(glIsBuffer(VBOs_) == GL_TRUE)
            glDeleteBuffers(1, &VBOs_);

    Book::AddEvent(eNOTE::nHYPHEN, "VBOs|VAOs|TBOs|PBOs: %u|%u|%u|%u.", 
                                   VBOs_, VAOs_, TBOs_, POs_);
}
};