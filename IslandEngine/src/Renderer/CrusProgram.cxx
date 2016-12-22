/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: shader programs loading routines.
****
********************************************************************************************************************************/
#include <cstdio>
#include <stdarg.h>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <regex>
//#include <experimental/filesystem>


#include "System\CrusWindow.h"

#include "Renderer\CrusRender.h"
#include "Renderer\CrusProgram.h"

namespace {
acstr kGLSL_VERSION = "#version 450 core";

std::initializer_list<std::string> const kINCLUDE_SHADERS{
    "Includes/ShaderVariables.glsl"
};
};

namespace isle {
int32 Program::lastAttribute_{-1};
int32 Program::lastUniform_{-1};

#if _CRUS_SHADER_DEBUG
bool Program::checked_{false};
#endif
bool ReadShaderSource(std::string &_source, std::string const &_parentPath, std::string const &_name);

std::string Program::PreprocessIncludes(std::string const &_source, std::string const &_name, int32 _includingLevel) const
{
    if (_includingLevel > Program::kINCLUDING_LEVEL) {
        Book::AddEvent(eNOTE::nERROR, "maximum source file including level is .", Program::kINCLUDING_LEVEL);
        return _source;
    }

    static std::regex const re("^[ ]*#[ ]*pragma[ ]+include[(][\"](.*)[\"][)].*");

    std::stringstream input;
    std::ostringstream output;

    size_t line_number = 1;

    input << "#line " << line_number << " \"" << _name << "\"" << std::endl;
    input << _source;

    std::smatch matches;
    std::string line;

    while (std::getline(input, line)) {
        if (std::regex_search(line, matches, re)) {
            std::string include_file = matches[1];
            std::string include_string;

            if (!ReadShaderSource(include_string, "../contents/shaders/", include_file))
                return _source;

            output << PreprocessIncludes(include_string, include_file, _includingLevel + 1) << std::endl;
            output << "#line " << line_number << " \"" << _name << "\"" << std::endl;
        }

        else {
            //output << "#line " << line_number << " \"" << _name << "\"" << std::endl;
            output << line << std::endl;
        }

        ++line_number;
    }

    return output.good() ? output.str() : _source;
}

bool ReadShaderSource(std::string &_source, std::string const &_parentPath, std::string const &_name)
{
    if (_parentPath.empty() || _name.empty()) {
        Book::AddEvent(eNOTE::nERROR, "file name is invalid.");
        return false;
    }

    std::string path(_parentPath + _name);
    std::ifstream file(path, std::ios::in);

    if (!file.is_open()) {
        Book::AddEvent(eNOTE::nERROR, "can't open file: \"%s\".", _name.c_str());
        return false;
    }

    _source = [&file] {
        std::ostringstream stream;
        stream << file.rdbuf() << '\n';

        return stream.good() ? stream.str() : nullptr;
    } ();

    file.close();

    if (_source.c_str() == nullptr) {
        Book::AddEvent(eNOTE::nERROR, "can't read file: \"%s\".", _name.c_str());
        return false;
    }

    return true;
}

bool Program::AssignNew(std::initializer_list<std::string> _names)
{
    // If current shader program exist, destroy it and create the new shader.
    if (glIsProgram(program_) == GL_TRUE) {
        Destroy();

        return AssignNew(_names);
    }

    if (!Render::inst().CreateProgram(program_))
        return false;

    std::string source;

    for (auto const &name : _names) {
        if (!ReadShaderSource(source, "../contents/shaders/", name))
            return false;

        source = PreprocessIncludes(source, name);

        if (!CreateShader(name, source, GL_VERTEX_SHADER))
            return false;

        if (!CreateShader(name, source, GL_FRAGMENT_SHADER))
            return false;
    }

    glLinkProgram(program_);

    if (glGetError() != GL_NO_ERROR)
        Book::AddEvent(eNOTE::nERROR, "...");

    if (CheckProgram(reinterpret_cast<astr>(_names.begin()->data()))) {
        glUseProgram(program_);

        if (glIsProgram(program_) != GL_TRUE) {
            Book::AddEvent(eNOTE::nERROR, "invalid program number: %d (%s)", program_, _names.begin()->c_str());
            return false;
        }

        else return true;
    }

    return false;
}

void Program::Destroy()
{
    if (glIsProgram(program_) == GL_FALSE)
        return;

    glUseProgram(0);

    uint32 shaders[3] = {0, 0, 0};
    int32 count = -1;

    glGetAttachedShaders(program_, 3, &count, shaders);

    while (--count > -1) {
        if (glIsShader(shaders[count]) != 0) {
            glDetachShader(program_, shaders[count]);
            glDeleteShader(shaders[count]);
        }
    }

    glDeleteProgram(program_);
}

#if _CRUS_SHADER_DEBUG
/*static*/ void Program::CheckError()
{
    if (lastAttribute_ < 0 || lastUniform_ < 0)
        checked_ = true;

    else checked_ = false;
}
#endif

bool Program::CreateShader(std::string const &_name, std::string _source, uint32 _type)
{
    ShaderInfo shaderInfo{"undefined shader", glCreateShader(_type)};

    if (glGetError() != GL_NO_ERROR) {
        Book::AddEvent(eNOTE::nERROR, "can't create shader.");
        return false;
    }

    // Are used for shader source file preprocessing.
    std::ostringstream preprocessor_directives;

    switch (_type) {
        case GL_VERTEX_SHADER:
            preprocessor_directives
                << '\n'
                << kGLSL_VERSION
                << "\n#define CRUS_VERTEX_SHADER 1\n"
                << "\n#define nVERTEX       " << nVERTEX
                << "\n#define nNORMAL       " << nNORMAL
                << "\n#define nTEXCRD       " << nTEXCRD
                << "\n#define nVIEWPORT     " << nVIEWPORT
                << "\n#define nTRANSFORM    " << nTRANSFORM
                << '\n';

            shaderInfo.type = "vertex shader";
            break;

        case GL_FRAGMENT_SHADER:
            preprocessor_directives
                << '\n'
                << kGLSL_VERSION
                << "\n#define CRUS_FRAGMENT_SHADER 1\n"
                << "\n#define nFRAG_COLOR   " << nFRAG_COLOR
                << "\n#define nMAIN_COLOR   " << nMAIN_COLOR
                << '\n';

            shaderInfo.type = "fragment shader";
            break;

        case GL_GEOMETRY_SHADER:
            preprocessor_directives
                << '\n'
                << kGLSL_VERSION
                << "\n#define CRUS_GEOMETRY_SHADER 1\n"
                << '\n';

            shaderInfo.type = "geometry shader";
            break;
    }

    std::string sources{preprocessor_directives.str() + _source};

    auto const t = sources.c_str();

    glShaderSource(shaderInfo.shader, 1, &t, nullptr);
    glCompileShader(shaderInfo.shader);

    if (!CheckShader(_name, shaderInfo))
        return false;

    glAttachShader(program_, shaderInfo.shader);
    return true;
}

bool Program::CheckShader(std::string _name, ShaderInfo const &_shaderInfo) const
{
    int32 status, length;
#if _CRUS_BUG_HUNTING
    char log[1024];
#else
    char log[256];
#endif

    glGetShaderiv(_shaderInfo.shader, GL_COMPILE_STATUS, &status);
    if (status != 0)
        return true;

    glGetShaderiv(_shaderInfo.shader, GL_INFO_LOG_LENGTH, &length);
    glGetShaderInfoLog(_shaderInfo.shader, sizeof(log), &length, log);

    if (length < 1)
        return false;

    Book::AddEvent(eNOTE::nERROR, "\"%s\" {%s}:", _name.data(), _shaderInfo.type.c_str());
    Book::AddEvent(eNOTE::nHYPHEN, "%s", log);

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

    if (status[0] != 0 && status[1] != 0)
        return true;

    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
    glGetProgramInfoLog(program_, sizeof(log), &length, log);

    if (length < 1)
        return false;

    Book::AddEvent(eNOTE::nERROR, "\"%s\":", _name.c_str());
    Book::AddEvent(eNOTE::nHYPHEN, "%s", log);

    return false;
}
};