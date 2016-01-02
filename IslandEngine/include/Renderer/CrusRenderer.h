/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****    Description: declaration of renderer system.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_RENDERER_H             // Include guard "CrusRenderer.h"
#define CRUS_RENDERER_H

#include <vector>

#include "Math\CrusMatrix.h"

#include "Renderer\CrusExts.h"
#include "Renderer\CrusColors.h"

#include "Interface\CrusViewport.h"

namespace isle
{
class Renderer {
public:

    intf::Viewport vp_;

    void SetupContext();
    void DeleteRC();

    bool CreateProgram(uint32 &program);

    bool CreateVBO(uint32 target, uint32 &vbo);
    bool CreateVAO(uint32 &vao);
    bool CreateTBO(uint32 target, uint32 &tbo);

    void SetViewport(int16 x, int16 y, int16 w, int16 h);

    void Update();
    void DrawFrame();

    void UpdateCMTS(size_t offset, size_t size, Color const *const colors) const;
    void UpdateTRSM(size_t offset, size_t count, math::Matrix const *const matrices) const;
    void UpdateATLAS(size_t offset, std::vector<float> vecs) const;

    static Renderer &inst();

private:
    HDC hDC_{nullptr};

    uint32 VBOs_{0}, VAOs_{0}, TBOs_{0};
    uint32 POs_{0};                            // The POs is the program objects.

    // Transform-scale-rotate matrices and color values UBOs indices.
    uint32 TRSM_{0}, CMTS_{0}, ATLAS_{0};

    //intf::Viewport vp_;

    Renderer();
    ~Renderer();

    void InitUniformBuffer();

    static void CALLBACK DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                       GLsizei length, char const *message, void const *userParam);

    void CleanUp();
};

inline void Renderer::UpdateCMTS(size_t _offset, size_t _size, Color const *const _colors) const
{
    glNamedBufferSubDataEXT(CMTS_, _offset, _size, _colors->c());
}

inline void Renderer::UpdateTRSM(size_t _offset, size_t _count, math::Matrix const *const _matrices) const
{
    glNamedBufferSubDataEXT(TRSM_, _offset * sizeof(math::Matrix), _count * sizeof(math::Matrix), _matrices->m());
}

inline void Renderer::UpdateATLAS(size_t _offset, std::vector<float> _vecs) const
{
    glNamedBufferSubDataEXT(ATLAS_, _offset * sizeof(float) * 2, _vecs.size() * sizeof(float), _vecs.data());
}
};

#endif // CRUS_RENDERER_H