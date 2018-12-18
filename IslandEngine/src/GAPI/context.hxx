#pragma once

#define GL_GLEXT_PROTOTYPES 0

#include <GL/glew.h>

#include <GL/glcorearb.h>
//#include <GL/glext.h>

#ifdef _MSC_VER
    #define WGL_WGLEXT_PROTOTYPES 1
    //#include <GL/wglext.h>

#elif defined(__GNUG__)
    #include <GL/glx.h>
    #include <GL/glxext.h>
#endif
