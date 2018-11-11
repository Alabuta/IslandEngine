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

#undef max

namespace {
auto constexpr kGLSL_VERSION = R"(#version 450 core)";
auto constexpr kSHADERS_PATH = R"(../contents/shaders/)";
};

namespace isle {
//std::unordered_map<std::string, std::string> Program::cachedIncludeFiles;
thread_local std::set<std::string> Program::cachedIncludeNames;
thread_local std::vector<std::string> Program::cachedIncludeFiles;

std::string Program::ReadShaderSource(std::string const &_parentPath, std::string const &_name)
{
    if (_parentPath.empty() || _name.empty()) {
        log::Error() << "file name is invalid.";
        return {};
    }

    std::string path(_parentPath + _name);
    std::ifstream file(path, std::ios::in);

    if (!file.is_open()) {
        log::Error() << "can't open file: " << _name;
        return {};
    }

    auto source = [&file]
    {
        std::ostringstream stream;
        stream << file.rdbuf() << '\n';

        return stream.good() ? stream.str() : std::string();
    } ();

    file.close();

    return source;
}

std::unordered_map<u32, std::string> Program::SeparateByStages(std::string const &_name, std::string &_includes, std::string const &_source)
{
    using namespace std::string_literals;
    thread_local static std::regex const rex_shader_stage_pattern("^[ |\t]*#[ |\t]*pragma[ |\t]+stage[ |\t]*[(][\"](.*)[\"][)].*", std::regex::optimize);

    thread_local static std::unordered_map<std::string, u32> const stagesTypes{
        {"vertex"s, GL_VERTEX_SHADER},
		{"fragment"s, GL_FRAGMENT_SHADER},
		{"geometry"s, GL_GEOMETRY_SHADER}
    };

    std::unordered_map<u32, std::string> stages;

    auto begin = std::sregex_token_iterator(_source.cbegin(), _source.cend(), rex_shader_stage_pattern, {-1, 1});
    auto end = std::sregex_token_iterator();

    _includes = std::move(*begin);
    auto line = std::count(_includes.cbegin(), _includes.cend(), '\n');

    for (auto it = std::next(begin); it != end; std::advance(it, 2)) {
        auto const &stageSource = stages.try_emplace(
            stagesTypes.at(*it),
#if CRUS_SPECIFY_SHADER_NAMES
            "#line "s + std::to_string(line + 1) + " \""s + _name + "\"\n"s + std::string(*std::next(it))
#else
            "#line "s + std::to_string(line + 1) + "\n"s + std::string(*std::next(it))
#endif
        ).first;

        line += std::count(stageSource->second.cbegin(), stageSource->second.cend(), '\n') - 1;
    }

    return stages;
}

void Program::PreprocessIncludes(std::string const &_source, i32 _includingLevel)
{
    using namespace std::string_literals;

    if (_includingLevel > Program::kINCLUDING_LEVEL) {
        log::Error() << "maximum source file including level is " << Program::kINCLUDING_LEVEL;
        return;
    }

    thread_local static std::regex const rex_shader_include_pattern("^[ |\t]*#[ |\t]*pragma[ |\t]+include[ |\t]*[(][\"](.*)[\"][)].*", std::regex::optimize);

    auto begin = std::sregex_iterator(_source.cbegin(), _source.cend(), rex_shader_include_pattern);
    auto end = std::sregex_iterator();

    std::string include_file_name;
    std::string include_file_source;

    for (auto it = begin; it != end; ++it) {
        include_file_name = (*it)[1];

        if (cachedIncludeNames.find(include_file_name) == cachedIncludeNames.cend()) {
            if (include_file_source = ReadShaderSource(kSHADERS_PATH, include_file_name); include_file_source.empty())
                return;

            cachedIncludeNames.emplace(include_file_name);
#if CRUS_SPECIFY_SHADER_NAMES
            cachedIncludeFiles.emplace_back("#line -1 \""s + include_file_name + "\"\n"s + std::move(include_file_source));
#else
            cachedIncludeFiles.emplace_back("#line -1\n"s + std::move(include_file_source));
#endif
        }

        PreprocessIncludes(include_file_source, _includingLevel + 1);
    }
}

bool Program::AssignNew(std::initializer_list<std::string> &&_names, std::string _options)
{
    using namespace std::string_literals;

    // If current shader program do exist, destroy it and create the new shader.
    if (glIsProgram(program_) == GL_TRUE) {
        Destroy();

        return AssignNew(std::move(_names), _options);
    }

    auto names = std::move(_names);

    if (!Render::inst().CreateProgram(program_))
        return false;

    std::vector<u32> shaderObjects;

    std::string includes, source;

    for (auto &&name : names) {
        if (source = ReadShaderSource(kSHADERS_PATH, name); source.empty()) {
            log::Error() << "can't read file: " << name;
            return false;
        }

        auto stages = SeparateByStages(name, includes, source);

        PreprocessIncludes(includes);

        for (auto &&[type, src] : stages) {
            if (auto shaderObject = CreateShaderObject(cachedIncludeFiles, src, type, _options); shaderObject != 0)
                shaderObjects.push_back(shaderObject);

            else return false;
        }
    }

    for (auto shaderObject : shaderObjects)
        glAttachShader(program_, shaderObject);

    auto const linkAndValidationStatus = LinkAndValidateProgram();

    for (auto shaderObject : shaderObjects) {
        glDetachShader(program_, shaderObject);
        glDeleteShader(shaderObject);
    }

    if (linkAndValidationStatus) {
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

    std::array<u32, 16> shaders{0};
    i32 count = -1;

    glGetAttachedShaders(program_, 3, &count, shaders.data());

    count = std::clamp(count, 0, static_cast<i32>(shaders.size()));

    while (--count > -1) {
        if (glIsShader(shaders[count]) != 0) {
            glDetachShader(program_, shaders[count]);
            glDeleteShader(shaders[count]);
        }
    }

    glDeleteProgram(program_);
}

u32 Program::CreateShaderObject(std::vector<std::string> const &_includes, std::string_view _source, u32 _type, std::string _options)
{
    using namespace std::string_literals;
    static std::unordered_map<u32, std::string> const shaderTypes{
        {GL_VERTEX_SHADER, "vertex"s},
        {GL_FRAGMENT_SHADER, "fragment"s},
        {GL_GEOMETRY_SHADER, "geometry"s}
    };

    auto const shaderObject = glCreateShader(_type);

    const auto preprocessorDirectives = [&_options] (u32 type)
    {
        // Used for shader source file preprocessing.
        std::ostringstream preprocessor_directives;

        preprocessor_directives
            << '\n'
            << kGLSL_VERSION
            << "\n#extension GL_ARB_bindless_texture : require";

        switch (type) {
            case GL_VERTEX_SHADER:
                preprocessor_directives
                    << '\n'
                    << "\n#define CRUS_VERTEX_STAGE 1\n"
                    << "\n#define nPOSITION      " << Render::eVERTEX_IN::nPOSITION
                    << "\n#define nNORMAL        " << Render::eVERTEX_IN::nNORMAL
                    << "\n#define nTEX_COORD     " << Render::eVERTEX_IN::nTEX_COORD
                    << "\n#define nCOLOR         " << Render::eVERTEX_IN::nCOLOR

                    << "\n#define nVIEWPORT      " << Render::eBUFFERS_BINDING::nVIEWPORT
                    << "\n#define nTRANSFORM     " << Render::eBUFFERS_BINDING::nTRANSFORM

                    << "\n#define nALBEDO        " << Render::eSAMPLERS_BINDING::nALBEDO
                    << "\n#define nNORMAL_MAP    " << Render::eSAMPLERS_BINDING::nNORMAL_MAP
                    << "\n#define nDEPTH         " << Render::eSAMPLERS_BINDING::nDEPTH
                    /*<< "\n#define nVIEWPORT_RECT " << nVIEWPORT_RECT
                    << "\n#define nNEAR_FAR      " << nNEAR_FAR*/
                    << '\n';
                break;

            case GL_GEOMETRY_SHADER:
                preprocessor_directives
                    << '\n'
                    << "\n#define CRUS_GEOMETRY_STAGE 1\n"
                    << "\n#define nVIEWPORT      " << Render::eBUFFERS_BINDING::nVIEWPORT
                    << "\n#define nTRANSFORM     " << Render::eBUFFERS_BINDING::nTRANSFORM

                    << "\n#define nALBEDO        " << Render::eSAMPLERS_BINDING::nALBEDO
                    << "\n#define nNORMAL_MAP    " << Render::eSAMPLERS_BINDING::nNORMAL_MAP
                    << "\n#define nDEPTH         " << Render::eSAMPLERS_BINDING::nDEPTH
                    /*<< "\n#define nVIEWPORT_RECT " << nVIEWPORT_RECT
                    << "\n#define nNEAR_FAR      " << nNEAR_FAR*/
                    << '\n';
                break;

            case GL_FRAGMENT_SHADER:
                preprocessor_directives
                    << '\n'
                    << "\n#define CRUS_FRAGMENT_STAGE 1\n"
                    << "\n#define nVIEWPORT      " << Render::eBUFFERS_BINDING::nVIEWPORT
                    << "\n#define nTRANSFORM     " << Render::eBUFFERS_BINDING::nTRANSFORM

                    << "\n#define nBASE_COLOR    " << Render::eFRAGMENT_OUT::nBASE_COLOR
                    << "\n#define nNORMALS       " << Render::eFRAGMENT_OUT::nNORMALS

                    //<< "\n#define nMAIN_COLOR    " << Render::eUNIFORMS_BINDING::nMAIN_COLOR

                    << "\n#define nALBEDO        " << Render::eSAMPLERS_BINDING::nALBEDO
                    << "\n#define nNORMAL_MAP    " << Render::eSAMPLERS_BINDING::nNORMAL_MAP
                    << "\n#define nDEPTH         " << Render::eSAMPLERS_BINDING::nDEPTH
                    /*<< "\n#define nVIEWPORT_RECT " << nVIEWPORT_RECT
                    << "\n#define nNEAR_FAR      " << nNEAR_FAR*/
                    << '\n';
                break;

            case GL_COMPUTE_SHADER:
                preprocessor_directives
                    << '\n'
                    << "\n#define CRUS_COMPUTE_STAGE 1\n"
                    << '\n';
                break;
        }
        
        preprocessor_directives << std::endl;

        preprocessor_directives << "#define CRUS_REVERSED_DEPTH " << static_cast<i32>(Render::kREVERSED_DEPTH) << '\n';
        preprocessor_directives << "#define CRUS_INFINITE_FAR_PLANE " << static_cast<i32>(Render::kINFINITE_FAR_PLANE) << '\n';
        preprocessor_directives << "#define CRUS_DEPTH_CLIPPED_FROM_ZERO_TO_ONE " << static_cast<i32>(Render::kDEPTH_CLIPPED_FROM_ZERO_TO_ONE) << '\n';

		preprocessor_directives << _options << '\n';

        using namespace std::string_literals;
        return preprocessor_directives.good() ? preprocessor_directives.str() : ""s;

    } (_type);

    std::vector<char const *> sources;

    sources.push_back(preprocessorDirectives.data());
    
    for (auto const &include : _includes)
        sources.push_back(include.data());

    sources.push_back(_source.data());

    glShaderSource(shaderObject, static_cast<i32>(sources.size()), sources.data(), nullptr);
    glCompileShader(shaderObject);

    auto success = 0;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE) {
        auto length = -1;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> log(length, '\0');
        glGetShaderInfoLog(shaderObject, static_cast<i32>(log.size()), &length, log.data());

        glDeleteShader(shaderObject);

        if (length < 1)
            return false;

        log::Error() << "["s << shaderTypes.at(_type) << " shader] "s << log.data();

        return 0;
    }

    glObjectLabel(GL_SHADER, shaderObject, -1, shaderTypes.at(_type).data());

    return shaderObject;
}

bool Program::LinkAndValidateProgram() const
{
    i32 status[2], length = -1;

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

void Program::bind() const
{
    glUseProgram(program_);
}

i32 Program::GetAttributeLoc(std::string_view _name) const
{
    auto attributeLocation = glGetAttribLocation(program_, _name.data());

    if (attributeLocation < 0)
        log::Warning() << "attribute is not exist or isn't being used: " << _name;

    return attributeLocation;
}

i32 Program::GetUniformLoc(std::string_view _name) const
{
    auto uniformLocation = glGetUniformLocation(program_, _name.data());

    if (uniformLocation < 0)
        log::Warning() << "uniform is not exist or isn't being used: " << _name;

    return uniformLocation;
}
};