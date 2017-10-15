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
std::tuple<bool, bool, bool> GetShaderStages(std::string const &_source)
{
    static std::regex const rex_shader_stage_pattern(R"(^\s*#\s*pragma\s+shader_stage[(]["](.*)["][)].*)");

    {
        std::vector<std::string> sources;
        //std::unordered_map<std::string, std::string> sources;

        auto begin = std::sregex_token_iterator(_source.cbegin(), _source.cend(), rex_shader_stage_pattern, -1);
        auto end = std::sregex_token_iterator();

        for (auto it = begin; it != end; ++it) {
            sources.emplace_back(std::move(*it));

            /*log::Debug() << it->first;
            log::Debug() << it->second;*/
        }

        for (auto const &source : sources)
            log::Debug() << source;
    }

    auto begin = std::sregex_iterator(_source.cbegin(), _source.cend(), rex_shader_stage_pattern);
    auto end = std::sregex_iterator();

    log::Debug() << "found " << std::distance(begin, end);

    auto stages = std::make_tuple(false, false, false);

    using namespace std::string_literals;
    static const std::array<std::string, 3> names = {"vertex"s, "geometry"s, "fragment"s};

    for (auto it = begin; it != end; ++it) {
        if (names[0] == (*it)[1])
            std::get<0>(stages) = true;

        else if (names[1] == (*it)[1])
            std::get<1>(stages) = true;

        else if (names[2] == (*it)[1])
            std::get<2>(stages) = true;
    }

    return stages;
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

    static std::regex const re(R"(^\s*#\s*pragma\s+include[(]["](.*)["][)].*)");

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

        auto const [vertex, geometry, fragment] = GetShaderStages(source);

        if (source = PreprocessIncludes(source, name); source.empty()) {
            log::Error() << "can't read file: " << name;
            return false;
        }

        if (vertex)
            if (auto vertexShaderObject = CreateShaderObject(source, GL_VERTEX_SHADER); vertexShaderObject != 0)
                shaderObjects.push_back(vertexShaderObject);

            else return false;

        if (geometry)
            if (auto geometryShaderObject = CreateShaderObject(source, GL_GEOMETRY_SHADER); geometryShaderObject != 0)
                shaderObjects.push_back(geometryShaderObject);

            else return false;

        if (fragment)
            if (auto fragmentShaderObject = CreateShaderObject(source, GL_FRAGMENT_SHADER); fragmentShaderObject != 0)
                shaderObjects.push_back(fragmentShaderObject);

            else return false;
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

uint32 Program::CreateShaderObject(std::string_view _source, uint32 _type)
{
    static std::unordered_map<uint32, std::string> const shaderTypes = {
        {GL_VERTEX_SHADER, "vertex shader"},
        {GL_FRAGMENT_SHADER, "fragment shader"},
        {GL_GEOMETRY_SHADER, "geometry shader"}
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

    std::array<char const *const, 2> const sources = {
        preprocessorDirectives.data(), _source.data()
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

        log::Error() << "{" << shaderTypes.at(_type) << "} " << log.data();

        return 0;
    }

    glObjectLabel(GL_SHADER, shaderObject, -1, shaderTypes.at(_type).data());

    return shaderObject;
}

bool Program::CreateShader(std::string_view const &_source, uint32 _type)
{
    auto shaderInfo = std::make_pair(glCreateShader(_type), "undefined shader");

    const auto preprocessor_directives = [&shaderInfo] (uint32 type)
    {
        // Are used for shader source file preprocessing.
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

                shaderInfo.second = "vertex shader";
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

                shaderInfo.second = "geometry shader";
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

            case GL_COMPUTE_SHADER:
                preprocessor_directives
                    << '\n'
                    << kGLSL_VERSION
                    << "\n#define CRUS_COMPUTE_SHADER 1\n"
                    << '\n';

                shaderInfo.second = "compute shader";
                break;
        }

        using namespace std::string_literals;
        return preprocessor_directives.good() ? preprocessor_directives.str() : ""s;

    } (_type);

    std::array<char const *const, 2> const sources = {
        preprocessor_directives.data(), _source.data()
    };

    glShaderSource(shaderInfo.first, static_cast<int32>(sources.size()), sources.data(), nullptr);

    if (!CompileShader(shaderInfo))
        return false;

    glObjectLabel(GL_SHADER, shaderInfo.first, -1, shaderInfo.second);

    glAttachShader(program_, shaderInfo.first);
    return true;
}

bool Program::CompileShader(std::pair<uint32, std::string_view> const &_shaderInfo) const
{
    auto status = 0, length = -1;

    auto const &[object, type] = _shaderInfo;

    glCompileShader(object);
    glGetShaderiv(object, GL_COMPILE_STATUS, &status);

    if (status == GL_TRUE)
        return true;

    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &length);

    std::vector<char> log(length, '\0');
    glGetShaderInfoLog(object, static_cast<GLsizei>(log.size()), &length, log.data());

    if (length < 1)
        return false;

    log::Error() << "{" << type << "} " << log.data();

    return false;
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