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

    intf::Viewport vp_;

    void Init();
    void DeleteContext();

    bool CreateProgram(uint32 &program);

    bool CreateBO(uint32 target, uint32 &vbo);
    bool CreateVAO(uint32 &vao);
    bool CreateTBO(uint32 target, uint32 &tbo);

    void SetViewport(int16 x, int16 y, int16 w, int16 h);

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

    uint32 BOs_{0}, VAOs_{0}, TBOs_{0};
    uint32 POs_{0};                            // The POs is the program objects.

    // Transform-scale-rotate matrices and color values UBOs indices.
    uint32 VIEWPORT_{0}, CMTS_{0}, TRANSFORM_{0};

    //intf::Viewport vp_;

    Render() = default;
    ~Render();

    void InitBufferObjects();

    void CleanUp();
};

inline void Render::UpdateCMTS(size_t _offset, size_t _size, Color const *const _colors) const
{
    glNamedBufferSubData(CMTS_, _offset, _size, _colors->c());
}

inline void Render::UpdateViewport(size_t _offset, size_t _count, math::Matrix const *const _matrices) const
{
    glNamedBufferSubData(VIEWPORT_, _offset * sizeof(math::Matrix), _count * sizeof(math::Matrix), _matrices->m());
}

inline void Render::UpdateTransform(size_t _offset, size_t _count, math::Matrix const *const _matrices) const
{
    glNamedBufferSubData(TRANSFORM_, _offset * sizeof(math::Matrix), _count * sizeof(math::Matrix), _matrices->m());
}
};

#endif // CRUS_RENDERER_H