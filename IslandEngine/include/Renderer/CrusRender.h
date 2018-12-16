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


#pragma warning(push, 3)
#pragma warning(disable: 4201)
#define GLM_FORCE_CXX17
#include <glm/glm.hpp>
#pragma warning(pop)


#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <mutex>

#include "Math\CrusMatrix.h"

#include "Renderer\CrusOpenGLExts.h"
#include "Renderer\CrusOpenGLContext.h"

#include "Renderer\CrusColors.h"


namespace isle {
class Render {
public:

    static auto constexpr kREVERSED_DEPTH = false;
    static auto constexpr kINFINITE_FAR_PLANE = true;
    static auto constexpr kDEPTH_CLIPPED_FROM_ZERO_TO_ONE = true;

    enum eVERTEX_IN {
        nCOLOR = 3, nTANGENT
    };

    enum eFRAGMENT_OUT {
        nBASE_COLOR = 0, nNORMALS
    };

    enum eBUFFERS_BINDING {
        nVIEWPORT = 0, nTRANSFORM
    };

    enum eSAMPLERS_BINDING {
        nALBEDO = 0, nNORMAL_MAP, nTEMP, nDEPTH
    };

    void Init();
    void DeleteContext();

    bool CreateProgram(u32 &program);

    u32 createBO();
    u32 createVAO();
    bool CreateTBO(u32 target, u32 &tbo);

    void Update();
    void DrawFrame();

    template<class T>
    void CreateSSBO(u32 programHandle, u32 binding, std::string name)
    {
        auto const index = glGetProgramResourceIndex(programHandle, GL_SHADER_STORAGE_BLOCK, name.c_str());

        if (index == GL_INVALID_INDEX)
            log::Fatal() << "can't init the SSBO: invalid index param: PER_OBJECT"s;

        auto objectBuffer = Render::inst().createBO();

        glNamedBufferStorage(objectBuffer, sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, objectBuffer);
        glShaderStorageBlockBinding(programHandle, index, binding);

        namedSSBOs_[name] = objectBuffer;
    }

    template<class D>
    void UpdateSSBO(std::string &&name, D &&data) const
    {
        using T = std::decay_t<D>;

        glNamedBufferSubData(namedSSBOs_.at(name), 0, sizeof(T), &data);
    }

    static Render &inst();

private:
    std::unordered_map<std::string, u32> namedSSBOs_;
    //std::mutex mutex_;

    //HDC hDC_{nullptr};
    //HGLRC hRC_{nullptr};

    u32 BOs_{0}, VAOs_{0}, TBOs_{0};
    u32 POs_{0};                            // The POs is the program objects.

    // Transform-scale-rotate matrices and color values UBOs indices.
    u32 VIEWPORT_{0}, CMTS_{0}, TRANSFORM_{0};

    Render() = default;
    ~Render();

    void CleanUp();
};
};

#endif // CRUS_RENDERER_H