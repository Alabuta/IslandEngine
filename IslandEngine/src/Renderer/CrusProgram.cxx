/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2014 Crusoe's Island LLC.
****
****    11h December 2015 / 27th June 2010.
****    Description: shader programs loading routines.
****
****************************************************************************************/
#include <cstdio>
#include <stdarg.h>
#include <fstream>
#include <streambuf>

#include "System\CrusSystem.h"
#include "System\CrusWindow.h"

#include "Renderer\CrusRenderer.h"
#include "Renderer\CrusProgram.h"

namespace isle
{
int32 Program::lastAttribute_{-1};
int32 Program::lastUniform_{-1};

#if _CRUS_SHADER_DEBUG
bool Program::checked_{false};
#endif

bool ReadShaderSources(std::vector<std::string> &_sources, std::initializer_list<std::string> const &_names)
{
    for (auto const &name : _names) {
        if (name.data() == nullptr) {
            Book::AddEvent(NOTE::nERROR, "shader file name is invalid: \"%s\".", name.data());
            continue;
        }

        //SetFullPath("..\\shaders\\", name);
        std::string path("../shaders/" + name);
        std::ifstream file(path, std::ios::ate);

        if(!file.is_open()) {
            Book::AddEvent(NOTE::nERROR, "can't open shader source file: \"%s\".", name.data());
            return false;
        }

        //file.seekg(0, std::ios::end);

        std::string str;
        str.reserve(file.tellg());

        file.seekg(0, std::ios::beg);
        str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        file.close();

        _sources.push_back(str.data());

        if (_sources.back().data() == nullptr) {
            Book::AddEvent(NOTE::nERROR, "can't read shader source file: \"%s\".", path.data());
            return false;
        }
    }

    return true;
}

void Program::SwitchOn() const
{
    glUseProgram(program_);

#if _CRUS_OBSOLETE
    if (glGetError() != GL_NO_ERROR) {
        Book::AddEvent(NOTE::nERROR, "invalid program number: %d (%s)", program_, name_.c_str());
    }
#endif
}

/*static*/ void Program::SwitchOff()
{
    glUseProgram(0);
}

uint32 Program::GetName() const
{
    return program_;
}

int32 Program::GetAttributeLoc(astr _name) const
{
    lastAttribute_ = glGetAttribLocation(program_, _name);

#if _CRUS_SHADER_DEBUG
    if (lastAttribute_ < 0 && !checked_)
        Book::AddEvent(NOTE::nWARN, "attribute \"%s\" unexist or are not used.", _name);
#endif

    return lastAttribute_;
}

int32 Program::GetUniformLoc(astr _name) const
{
    lastUniform_ = glGetUniformLocation(program_, _name);

#if _CRUS_SHADER_DEBUG
    if (lastUniform_ < 0 && !checked_)
        Book::AddEvent(NOTE::nWARN, "uniform \"%s\" unexist or are not used.", _name);
#endif

    return lastUniform_;
}

/*static*/ int32 Program::GetLastAttributeLoc()
{
    return lastAttribute_;
}

/*static*/ int32 Program::GetLasUniformLoc()
{
    return lastUniform_;
}

bool Program::AssignNew(std::initializer_list<std::string> _names, astr _options, ...)
{
    //name_.shrink_to_fit();

    // If current shader program exist, destroy it and create the new shader.
    if(glIsProgram(program_) == GL_TRUE){
        Delete();

        va_list ap;
        va_start(ap, _options);

        return AssignNew(_names, ap);
    }

    //name_ = _names.begin()[0];
    //name_.shrink_to_fit();

    // Alloc memory for shader sources:
    //     1) sources[0] are used for shader preprocessing;
    //     2) sources[1] contains the passed options;
    //     3) sources[2] store first shader sources;
    //     4) sources[3] store next shader sources;
    //     5) and etc.
    std::vector<std::string> sources = {"", ""};

    if (!ReadShaderSources(sources, _names)) {
        Book::AddEvent(NOTE::nERROR, "invalid shader: \"%s\".", _names.begin()->data());
        return false;
    }

    // Read the passed options and write them in sources[1].
    if(_options != nullptr && _options[0] != '\0'){
        char options[128];
        va_list ap;

        va_start(ap, _options);
            vsprintf_s(options, sizeof(options), _options, ap);
        va_end(ap);

        sources[1] = options;
    }

    if(!Renderer::inst().CreateProgram(program_))
        return false;

    if (!CreateShader(_names.begin()->data(), sources, GL_VERTEX_SHADER))
        return false;

    if (!CreateShader(_names.begin()->data(), sources, GL_FRAGMENT_SHADER))
        return false;

    glLinkProgram(program_);

    if(glGetError() != GL_NO_ERROR)
        Book::AddEvent(NOTE::nERROR, "...");

    if (CheckProgram(reinterpret_cast<astr>(_names.begin()->data()))) {
        glUseProgram(program_);

        if (glIsProgram(program_) != GL_TRUE) {
            Book::AddEvent(NOTE::nERROR, "invalid program number: %d (%s)", program_, _names.begin()->c_str());
            return false;
        }

        else return true;
    }
    return false;
}

void Program::Delete()
{
    if(glIsProgram(program_) == GL_FALSE)
        return;

    glUseProgram(0);

    uint32 shaders[3] = {0, 0, 0};
    int32 count = -1;

    glGetAttachedShaders(program_, 3, &count, shaders);

    while(--count > -1){
        if(glIsShader(shaders[count]) != 0){
            glDetachShader(program_, shaders[count]);
            glDeleteShader(shaders[count]);
        }
    }

    glDeleteProgram(program_);
}

#if _CRUS_SHADER_DEBUG
/*static*/ void Program::CheckError()
{
    if(lastAttribute_ < 0 || lastUniform_ < 0)
        checked_ = true;

    else checked_ = false;
}
#endif

bool Program::CreateShader(std::string _name, std::vector<std::string> _sources, uint32 _type)
{
    uint32 shader = glCreateShader(_type);

    if(glGetError() != GL_NO_ERROR){
        Book::AddEvent(NOTE::nERROR, "can't create shader.");
        return false;
    }

    // Are used for shader preprocessing.
    char prepross[1024];
    astr type = "undefined shader";

    // Fill determinants.
    if(_type == GL_VERTEX_SHADER){
        sprintf_s(prepross, sizeof(prepross),
                  "\n#version 330 core\n\
                  \n#define CRUS_VERTEX_SHADER 1\n\
                  \n#define nVERTEX %u\
                  \n#define nNORMAL %u\
                  \n#define nTEXCRD %u\
                  \n#define nTRANSFORM %u\
                  \n#define nATLAS %u\n",
                  nVERTEX, nNORMAL, nTEXCRD, nTRANSFORM, nATLAS);

        type = "vertex shader";
    }

    else if(_type == GL_FRAGMENT_SHADER){
        sprintf_s(prepross, sizeof(prepross), "\n#version 330 core\n\
                                               \n#define CRUS_FRAGMENT_SHADER 1\n\
                                               \n#define nMATERIAL %u\
                                               \n#define nFRAG_COLOR %u\n",
                                               nMATERIAL, nFRAG_COLOR);

        type = "fragment shader";
    }

    else if(_type == GL_GEOMETRY_SHADER){
        sprintf_s(prepross, sizeof(prepross), "\n#version 330 core\n\
                                               \n#define CRUS_GEOMETRY_SHADER 1\n");

        //glProgramParameteri(program_, GL_GEOMETRY_INPUT_TYPE, GL_TRIANGLES);
        //glProgramParameteri(program_, GL_GEOMETRY_OUTPUT_TYPE, GL_TRIANGLE_STRIP);
        //glProgramParameteri(program_, GL_GEOMETRY_VERTICES_OUT, 3 * 5);

        type = "geometry shader";
    }

    _sources[0] = prepross;
    std::vector<astr> sources;

    for(auto const &src : _sources)
        sources.push_back(src.data());

    glShaderSource(shader, static_cast<GLsizei>(_sources.size()), sources.data(), nullptr);
    glCompileShader(shader);

    if(!CheckShader(_name, type, shader))
        return false;

    glAttachShader(program_, shader);
    return true;
}

bool Program::CheckShader(std::string _name, astr _type, uint32 _shader) const
{
    int32 status, length;
#if _CRUS_BUG_HUNTING
    char log[1024];
#else
    char log[256];
#endif

    glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);
    if(status != 0)
        return true;

    glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &length);
    glGetShaderInfoLog(_shader, sizeof(log), &length, log);

    if(length < 1)
        return false;

    Book::AddEvent(NOTE::nERROR, "\"%s\" {%s}:", _name.data(), _type);
    Book::AddEvent(NOTE::nHYPHEN, "%s", log);

    return false;
}

bool Program::CheckProgram(std::string _name) const
{
    int32 status[2], length;
#if _CRUS_BUG_HUNTING
    char log[1024];
#else
    char log[256];
#endif

    glGetProgramiv(program_, GL_LINK_STATUS, &status[0]);

    glValidateProgram(program_);
    glGetProgramiv(program_, GL_VALIDATE_STATUS, &status[1]);

    if(status[0] != 0 && status[1] != 0)
        return true;

    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
    glGetProgramInfoLog(program_, sizeof(log), &length, log);

    if(length < 1)
        return false;

    Book::AddEvent(NOTE::nERROR, "\"%s\":", _name.data());
    Book::AddEvent(NOTE::nHYPHEN, "%s", log);

    return false;
}
};