/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: shader programs loading routines.
****
********************************************************************************************************************************/
#include <sstream>
#include <fstream>
#include <streambuf>
#include <regex>
#include <cctype>
//#include <filesystem>


#include "Renderer\CrusRender.h"
#include "Renderer\CrusProgram.h"

namespace {
auto constexpr kGLSL_VERSION = R"(#version 450 core)";
auto constexpr kSHADERS_PATH = R"(../contents/shaders/)";
};

namespace isle {
bool ReadShaderSource(std::string &_source, std::string const &_parentPath, std::string const &_name)
{
    if (_parentPath.empty() || _name.empty()) {
        log::Error() << "file name is invalid.";
        return false;
    }

    std::string path(_parentPath + _name);
    std::ifstream file(path, std::ios::in);

    if (!file.is_open()) {
        log::Error() << "can't open file: " << _name;
        return false;
    }

    _source = [&file = file]
    {
        std::ostringstream stream;
        stream << file.rdbuf() << '\n';

        return stream.good() ? stream.str() : "\0";
    } ();

    file.close();

    if (_source.empty()) {
        log::Error() << "can't read file: " << _name;
        return false;
    }

    return true;
}

std::string Program::PreprocessIncludes(std::string const &_source, std::string const &_name, int32 _includingLevel) const
{
    if (_includingLevel > Program::kINCLUDING_LEVEL) {
        log::Error() << "maximum source file including level is " << Program::kINCLUDING_LEVEL;
        return _source;
    }

    static std::regex const re(R"(^\s*#\s*pragma\s+include[(]["](.*)["][)].*)");

    std::stringstream input;
    std::ostringstream output;

    auto fileName = std::regex_replace(_name, std::regex(R"(-|�|\.|/|\\)"), "_");
    std::transform(fileName.begin(), fileName.end(), fileName.begin(), [] (auto c) { return std::toupper<char>(c, std::locale()); });

    input << "#ifndef " << fileName << '\n';
    input << "#define " << fileName << '\n';

    int64 line_number = -1;

    input << "#line " << line_number << " \"" << _name << "\"\n";
    input << _source;

    std::smatch matches;
    std::string line;

    while (std::getline(input, line)) {
        if (std::regex_search(line, matches, re)) {
            std::string include_file_name(matches[1]);
            std::string include_file_source;

            if (!ReadShaderSource(include_file_source, kSHADERS_PATH, include_file_name))
                return _source;

            output << PreprocessIncludes(include_file_source, include_file_name, _includingLevel + 1) << '\n';
            output << "#line " << line_number << " \"" << _name << "\"\n";
        }

        else output << line << '\n';

        ++line_number;
    }

    output << "#endif // " << fileName << std::endl;

    return output.good() ? output.str() : _source;
}

bool Program::AssignNew(std::initializer_list<char const *> &&_names)
{
    // If current shader program exist, destroy it and create the new shader.
    if (glIsProgram(program_) == GL_TRUE) {
        Destroy();

        return AssignNew(std::move(_names));
    }

    auto names = std::move(_names);

    if (!Render::inst().CreateProgram(program_))
        return false;

    std::string source;

    for (auto const &name : names) {
        if (!ReadShaderSource(source, kSHADERS_PATH, name))
            return false;

        source = PreprocessIncludes(source, name);

        if (!CreateShader(source, GL_VERTEX_SHADER))
            return false;

        if (!CreateShader(source, GL_FRAGMENT_SHADER))
            return false;
    }

    if (LinkAndValidateProgram()) {
        if (glIsProgram(program_) != GL_TRUE) {
            log::Error() << "invalid program number: " << program_ << " from " << names.begin();
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

bool Program::CreateShader(std::string const &_source, uint32 _type)
{
    std::pair<uint32, astr> shaderInfo(glCreateShader(_type), "undefined shader");

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

            shaderInfo.second = "vertex shader";
            break;

        case GL_FRAGMENT_SHADER:
            preprocessor_directives
                << '\n'
                << kGLSL_VERSION
                << "\n#define CRUS_FRAGMENT_SHADER 1\n"
                << "\n#define nFRAG_COLOR   " << nFRAG_COLOR
                << "\n#define nMAIN_COLOR   " << nMAIN_COLOR
                << '\n';

            shaderInfo.second = "fragment shader";
            break;

        case GL_GEOMETRY_SHADER:
            preprocessor_directives
                << '\n'
                << kGLSL_VERSION
                << "\n#define CRUS_GEOMETRY_SHADER 1\n"
                << '\n';

            shaderInfo.second = "geometry shader";
            break;

        case GL_COMPUTE_SHADER:
            preprocessor_directives
                << '\n'
                << kGLSL_VERSION
                << "\n#define CRUS_COMPUTE_SHADER 1\n"
                << '\n';

            shaderInfo.second = "compute shader";
            break;
    }

    std::string sources(preprocessor_directives.str() + _source);
    auto const str = sources.c_str();

    glShaderSource(shaderInfo.first, 1, &str, nullptr);

    if (!CompileShader(shaderInfo))
        return false;

    glObjectLabel(GL_SHADER, shaderInfo.first, -1, shaderInfo.second);

    glAttachShader(program_, shaderInfo.first);
    return true;
}

bool Program::CompileShader(std::pair<uint32, astr> const &_shaderInfo) const
{
    auto status = 0, length = -1;

    glCompileShader(_shaderInfo.first);
    glGetShaderiv(_shaderInfo.first, GL_COMPILE_STATUS, &status);

    if (status == GL_TRUE)
        return true;

    glGetShaderiv(_shaderInfo.first, GL_INFO_LOG_LENGTH, &length);

    std::vector<char> log(length, '\0');
    glGetShaderInfoLog(_shaderInfo.first, static_cast<GLsizei>(log.size()), &length, log.data());

    if (length < 1)
        return false;

    log::Error() << "{" << _shaderInfo.second << "} " << log.data();

    return false;
}

bool Program::LinkAndValidateProgram() const
{
    int32 status[2], length = -1;

    glLinkProgram(program_);
    glGetProgramiv(program_, GL_LINK_STATUS, &status[0]);

    glValidateProgram(program_);
    glGetProgramiv(program_, GL_VALIDATE_STATUS, &status[1]);

    if (status[0] == GL_TRUE && status[1] == GL_TRUE)
        return true;

    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);

    std::vector<char> log(length, '\0');
    glGetProgramInfoLog(program_, static_cast<GLsizei>(log.size()), &length, log.data());

    if (length < 1)
        return false;

    log::Error() << "{shader program} " << log.data();

    return false;
}

void Program::UseThis() const
{
    glUseProgram(program_);
}

int32 Program::GetAttributeLoc(astr _name) const
{
    auto attributeLocation = glGetAttribLocation(program_, _name);

    if (attributeLocation < 0)
        log::Warning() << "attribute is not exist or isn't being used: " << _name;

    return attributeLocation;
}

int32 Program::GetUniformLoc(astr _name) const
{
    auto uniformLocation = glGetUniformLocation(program_, _name);

    if (uniformLocation < 0)
        log::Warning() << "uniform is not exist or isn't being used: " << _name;

    return uniformLocation;
}
};