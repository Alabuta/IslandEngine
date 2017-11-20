/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: OpenGL extensions declarations.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_CRUSOPENGLEXTS_H       // Include guard "CrusOpenGLExts.h"
#define CRUS_CRUSOPENGLEXTS_H

/*#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B*/

#define GL_GLEXT_PROTOTYPES 0
#define WGL_WGLEXT_PROTOTYPES 1

#define CRUS_USE_GL_EXTENSIONS 0

#include <GL\GLcoreARB.h>
#include <GL\GLext.h>
#include <GL\wGLext.h>

#endif // CRUS_CRUSOPENGLEXTS_H