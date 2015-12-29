/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    29th September 2013 / 27th November 2010.
****    Description: OpenGL extensions declarations.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_EXTS_H                 // Include guard "CrusExts.h"
#define CRUS_EXTS_H

/*#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B*/

#define GL_GLEXT_PROTOTYPES 1
#define WGL_WGLEXT_PROTOTYPES 1

#include <GL\GLcoreARB.h>
#include <GL\GLext.h>
#include <GL\wGLext.h>

#define _CRUS_CHECK_GL_CALLS  1

#endif // CRUS_EXTS_H