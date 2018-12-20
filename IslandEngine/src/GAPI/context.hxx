#pragma once

//#define GL_GLEXT_PROTOTYPES 0

#include <GL/glew.h>
#include <GL/glcorearb.h>

#ifdef _MSC_VER
    //#define WGL_WGLEXT_PROTOTYPES 0
    //#include <GL/wglext.h>

#elif defined(__GNUG__)
    #include <GL/glx.h>
    #include <GL/glxext.h>
#endif

//#include <GL/glext.h>

#ifdef _MSC_VER
    #pragma comment(lib, "OpenGL32.lib")
    #pragma comment(lib, "glew32d.lib")
    #pragma comment(lib, "glfw3dll.lib")
#endif
