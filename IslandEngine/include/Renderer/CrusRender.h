/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: declaration of renderer system.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_RENDERER_H             // Include guard "CrusRenderer.h"
#define CRUS_RENDERER_H

#include <vector>
#include <mutex>

#include "Math\CrusMatrix.h"

#include "Renderer\CrusOpenGLExts.h"
#include "Renderer\CrusOpenGLContext.h"

#include "Renderer\CrusColors.h"

#include "Interface\CrusViewport.h"

namespace isle {
class Render {
public:

    static auto constexpr kREVERSED_DEPTH = true;
    static auto constexpr kINFINITE_FAR_PLANE = true;
    static auto constexpr kDEPTH_CLIPPED_FROM_ZERO_TO_ONE = true;

    enum eVERTEX_IN {
        nPOSITION = 0, nNORMAL, nTEX_COORD, nCOLOR, nTANGENT
    };

    enum eFRAGMENT_OUT {
        nBASE_COLOR = 0, nNORMALS
    };

    /*enum eUNIFORMS_BINDING {
        nMAIN_COLOR = 0
    };*/

    enum eBUFFERS_BINDING {
        nVIEWPORT = 0, nTRANSFORM
    };

    enum eSAMPLERS_BINDING {
        nALBEDO = 0, nNORMAL_MAP, nTEMP, nDEPTH
    };

    intf::Viewport vp_;

    void Init();
    void DeleteContext();

    bool CreateProgram(u32 &program);

    bool CreateBO(u32 &vbo);
    bool CreateVAO(u32 &vao);
    bool CreateTBO(u32 target, u32 &tbo);

    void SetViewport(i16 x, i16 y, i16 w, i16 h);

    void Update();
    void DrawFrame();

    void UpdateCMTS(size_t offset, size_t size, Color const *const colors) const;
    void UpdateViewport(size_t offset, size_t count, math::Matrix const *const matrices) const;
    void UpdateTransform(size_t offset, size_t count, math::Matrix const *const matrices) const;

    static Render &inst();

private:
    //std::mutex mutex_;

    //HDC hDC_{nullptr};
    //HGLRC hRC_{nullptr};

    u32 BOs_{0}, VAOs_{0}, TBOs_{0};
    u32 POs_{0};                            // The POs is the program objects.

    // Transform-scale-rotate matrices and color values UBOs indices.
    u32 VIEWPORT_{0}, CMTS_{0}, TRANSFORM_{0};

    //intf::Viewport vp_;

    Render() = default;
    ~Render();

    void InitBufferObjects();

    void CleanUp();
};

inline void Render::UpdateCMTS(size_t _offset, size_t _size, Color const *const _colors) const
{
    glNamedBufferSubData(CMTS_, _offset, _size, _colors->rgba.data());
}

inline void Render::UpdateViewport(size_t _offset, size_t _count, math::Matrix const *const _matrices) const
{
    glNamedBufferSubData(VIEWPORT_, _offset * sizeof(math::Matrix), _count * sizeof(math::Matrix), _matrices->m.data());
}

inline void Render::UpdateTransform(size_t _offset, size_t _count, math::Matrix const *const _matrices) const
{
    glNamedBufferSubData(TRANSFORM_, _offset * sizeof(math::Matrix), _count * sizeof(math::Matrix), _matrices->m.data());
}
};

#endif // CRUS_RENDERER_H