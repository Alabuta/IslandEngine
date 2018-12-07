#pragma once

#include <iostream>

#include <string>
using namespace std::string_literals;

#include <string_view>
using namespace std::string_view_literals;

#define GL_GLEXT_PROTOTYPES 0

#include <GL/glcorearb.h>
#include <GL/glext.h>

#ifdef _MSC_VER
#define WGL_WGLEXT_PROTOTYPES 1
#include <GL/wglext.h>

// #pragma comment(lib, "OpenGL32.lib")
#elif defined(__GNUG__)
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

