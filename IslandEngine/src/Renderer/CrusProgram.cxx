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
#include <array>
#include <cctype>
#include <unordered_map>
//#include <filesystem>


#include "Renderer\CrusRender.h"
#include "Renderer\CrusProgram.h"

namespace {
auto constexpr kGLSL_VERSION = R"(#version 450 core)";
auto constexpr kSHADERS_PATH = R"(../contents/shaders/)";
};

namespace isle {
auto GetShaderStages(std::string const &_source)
{
    using namespace std::string_literals;
    static std::regex const rex_shader_stage_pattern(R"(^\s*#\s*pragma\s+stage[(]["](.*)["][)].*)", std::regex::optimize);

    std::unordered_map<std::string, std::string> sources;

    auto begin = std::sregex_token_iterator(_source.cbegin(), _source.cend(), rex_shader_stage_pattern, {-1, 1});
    auto end = std::sregex_token_iterator();

    sources.emplace("undefined"s, std::move(*begin));

    for (auto it = std::next(begin); it != end; std::advance(it, 2))
        sources.emplace(std::move(*it), std::move(*std::next(it)));

    return sources;
}

std::string Program::ReadShaderSource(std::string const &_parentPath, std::string const &_name)
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

    auto source = [&file = file]
    {
        std::ostringstream stream;
        stream << file.rdbuf() << '\n';

        return stream.good() ? stream.str() : std::string();
    } ();

    file.close();

    return source;
}

std::string Program::PreprocessIncludes(std::string const &_source, std::string_view _name, int32 _includingLevel)
{
    if (_includingLevel > Program::kINCLUDING_LEVEL) {
        log::Error() << "maximum source file including level is " << Program::kINCLUDING_LEVEL;
        return _source;
    }

    static std::regex const re(R"(^\s*#\s*pragma\s+include[(]["](.*)["][)].*)", std::regex::optimize);

    std::stringstream input;
    std::ostringstream output;

    auto fileName = std::regex_replace(_name.data(), std::regex(R"(-|–|\.|/|\\)"), "_");
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

            if (include_file_source = ReadShaderSource(kSHADERS_PATH, include_file_name); include_file_source.empty())
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

bool Program::AssignNew(std::initializer_list<std::string> &&_names)
{
    using namespace std::string_literals;

    // If current shader program do exist, destroy it and create the new shader.
    if (glIsProgram(program_) == GL_TRUE) {
        Destroy();

        return AssignNew(std::move(_names));
    }

    auto names = std::move(_names);

    if (!Render::inst().CreateProgram(program_))
        return false;

    std::string source;

    std::vector<uint32> shaderObjects;

    for (auto const &name : names) {
        if (source = ReadShaderSource(kSHADERS_PATH, name); source.empty()) {
            log::Error() << "can't read file: " << name;
            return false;
        }

        auto sources = GetShaderStages(source);

        /*for (auto &[type, src] : sources) {
            if (src = PreprocessIncludes(src, name); src.empty()) {
                log::Error() << "can't read file: " << name;
                return false;
            }
        }*/

        auto includes = std::move(sources.at("undefined"s));
        if (includes = PreprocessIncludes(includes, name); includes.empty()) {
            log::Error() << "can't read file: " << name;
            return false;
        }
        sources.extract("undefined"s);

        static std::unordered_map<std::string, uint32> const shaderTypes = {
            {"vertex", GL_VERTEX_SHADER},
            {"fragment", GL_FRAGMENT_SHADER},
            {"geometry", GL_GEOMETRY_SHADER}
        };

        for (auto &[type, src] : sources) {
            if (auto shaderObject = CreateShaderObject(includes, src, shaderTypes.at(type)); shaderObject != 0)
                shaderObjects.push_back(shaderObject);

            else return false;
        }
    }

    for (auto shaderObject : shaderObjects)
        glAttachShader(program_, shaderObject);

    if (LinkAndValidateProgram()) {
        for (auto shaderObject : shaderObjects)
            glDetachShader(program_, shaderObject);

        if (glIsProgram(program_) != GL_TRUE) {
            log::Error() << "invalid program number: " << program_ << " from " << names.begin();
            return false;
        }

        else return true;
    }

    for (auto shaderObject : shaderObjects)
        glDetachShader(program_, shaderObject);

    return false;
}

void Program::Destroy()
{
    if (glIsProgram(program_) == GL_FALSE)
        return;

    glUseProgram(0);

    std::array<uint32, 16> shaders{0};
    int32 count = -1;

    glGetAttachedShaders(program_, 3, &count, shaders.data());

    count = std::clamp(count, 0, static_cast<int32>(shaders.size()));

    while (--count > -1) {
        if (glIsShader(shaders[count]) != 0) {
            glDetachShader(program_, shaders[count]);
            glDeleteShader(shaders[count]);
        }
    }

    glDeleteProgram(program_);
}

uint32 Program::CreateShaderObject(std::string_view _includes, std::string_view _source, uint32 _type)
{
    static std::unordered_map<uint32, std::string> const shaderTypes = {
        {GL_VERTEX_SHADER, "vertex"},
        {GL_FRAGMENT_SHADER, "fragment"},
        {GL_GEOMETRY_SHADER, "geometry"}
    };

    auto const shaderObject = glCreateShader(_type);

    const auto preprocessorDirectives = [] (uint32 type)
    {
        // Used for shader source file preprocessing.
        std::ostringstream preprocessor_directives;

        switch (type) {
            case GL_VERTEX_SHADER:
                preprocessor_directives
                    << '\n'
                    << kGLSL_VERSION
                    << "\n#define CRUS_VERTEX_SHADER 1\n"
                    << "\n#define nVERTEX        " << nVERTEX
                    << "\n#define nNORMAL        " << nNORMAL
                    << "\n#define nTEXCRD        " << nTEXCRD
                    << "\n#define nCOLOR         " << nCOLOR
                    << "\n#define nVIEWPORT      " << nVIEWPORT
                    << "\n#define nTRANSFORM     " << nTRANSFORM
                    << "\n#define nVIEWPORT_RECT " << nVIEWPORT_RECT
                    << '\n';
                break;

            case GL_GEOMETRY_SHADER:
                preprocessor_directives
                    << '\n'
                    << kGLSL_VERSION
                    << "\n#define CRUS_GEOMETRY_SHADER 1\n"
                    << "\n#define nVIEWPORT      " << nVIEWPORT
                    << "\n#define nTRANSFORM     " << nTRANSFORM
                    << "\n#define nVIEWPORT_RECT " << nVIEWPORT_RECT
                    << '\n';
                break;

            case GL_FRAGMENT_SHADER:
                preprocessor_directives
                    << '\n'
                    << kGLSL_VERSION
                    << "\n#define CRUS_FRAGMENT_SHADER 1\n"
                    << "\n#define nFRAG_COLOR   " << nFRAG_COLOR
                    << "\n#define nMAIN_COLOR   " << nMAIN_COLOR
                    << '\n';
                break;

            case GL_COMPUTE_SHADER:
                preprocessor_directives
                    << '\n'
                    << kGLSL_VERSION
                    << "\n#define CRUS_COMPUTE_SHADER 1\n"
                    << '\n';
                break;
        }

        using namespace std::string_literals;
        return preprocessor_directives.good() ? preprocessor_directives.str() : ""s;

    } (_type);

    std::array<char const *const, 3> const sources = {
        preprocessorDirectives.data(), _includes.data(), _source.data()
    };

    glShaderSource(shaderObject, static_cast<int32>(sources.size()), sources.data(), nullptr);
    glCompileShader(shaderObject);

    auto success = 0;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE) {
        auto length = -1;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> log(length, '\0');
        glGetShaderInfoLog(shaderObject, static_cast<int32>(log.size()), &length, log.data());

        glDeleteShader(shaderObject);

        if (length < 1)
            return false;

        log::Error() << "[" << shaderTypes.at(_type) << " shader] " << log.data();

        return 0;
    }

    glObjectLabel(GL_SHADER, shaderObject, -1, shaderTypes.at(_type).data());

    return shaderObject;
}

bool Program::LinkAndValidateProgram() const
{
    int32 status[2], length = -1;

    glLinkProgram(program_);
    glGetProgramiv(program_, GL_LINK_STATUS, &status[0]);

    if (status[0] != GL_TRUE) {
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);

        if (length < 1)
            return false;

        std::vector<char> log(length, '\0');
        glGetProgramInfoLog(program_, static_cast<GLsizei>(log.size()), &length, log.data());

        log::Error() << "{shader program} " << log.data();

        return false;
    }

    glValidateProgram(program_);
    glGetProgramiv(program_, GL_VALIDATE_STATUS, &status[1]);

    if (status[1] == GL_TRUE)
        return true;

    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);

    if (length < 1)
        return false;

    std::vector<char> log(length, '\0');
    glGetProgramInfoLog(program_, static_cast<GLsizei>(log.size()), &length, log.data());

    log::Error() << "{shader program} " << log.data();

    return false;
}

void Program::UseThis() const
{
    glUseProgram(program_);
}

int32 Program::GetAttributeLoc(std::string_view _name) const
{
    auto attributeLocation = glGetAttribLocation(program_, _name.data());

    if (attributeLocation < 0)
        log::Warning() << "attribute is not exist or isn't being used: " << _name;

    return attributeLocation;
}

int32 Program::GetUniformLoc(std::string_view _name) const
{
    auto uniformLocation = glGetUniformLocation(program_, _name.data());

    if (uniformLocation < 0)
        log::Warning() << "uniform is not exist or isn't being used: " << _name;

    return uniformLocation;
}
};