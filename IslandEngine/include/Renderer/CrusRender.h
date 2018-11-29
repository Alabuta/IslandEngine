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

    void Init();
    void DeleteContext();

    bool CreateProgram(u32 &program);

    u32 createBO();
    u32 createVAO();
    bool CreateTBO(u32 target, u32 &tbo);

    void Update();
    void DrawFrame();

    void UpdateCMTS(size_t offset, size_t size, Color const *const colors) const;
    void UpdateViewport(size_t offset, size_t count, math::Matrix const *const matrices) const;
    void UpdateTransform(size_t offset, size_t count, math::Matrix const *const matrices) const;

    template<class T, typename std::enable_if_t<is_container_v<std::decay_t<T>>>...>
    void UpdateViewport(std::size_t offset, T &&matrices) const
    {
        static_assert(
            std::is_same_v<typename std::decay_t<T>::value_type, glm::mat4>,
            "container object has to contain glm::mat4 elements"
        );

        auto constexpr bytes = sizeof(glm::mat4);

        glNamedBufferSubData(VIEWPORT_, offset * bytes, std::size(matrices) * bytes, std::data(matrices));
    }

    template<class T, typename std::enable_if_t<is_container_v<std::decay_t<T>>>...>
    void UpdateTransform(std::size_t offset, T &&matrices) const
    {
        static_assert(
            std::is_same_v<typename std::decay_t<T>::value_type, glm::mat4>,
            "container object has to contain glm::mat4 elements"
        );

        auto constexpr bytes = sizeof(glm::mat4);

        glNamedBufferSubData(TRANSFORM_, offset * bytes, std::size(matrices) * bytes, std::data(matrices));
    }

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