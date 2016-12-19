/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 27th November 2010.
****    Description: OpenGL extensions implementations.
****
********************************************************************************************************************************/
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef  UNICODE
#define  UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "System\CrusSystem.h"
#include "System\CrusWindow.h"
#include "Renderer\CrusExts.h"

#pragma comment(lib, "OpenGL32.lib")

#define GL_PROC_PREFIX(_proc)               PFN##_proc##PROC
#define GL_PROC(_proc, _name)               static GL_PROC_PREFIX(_proc) _name

#if _CRUS_CHECK_GL_CALLS
    #define GL_PROC_INIT(_proc, _name)\
        GL_PROC(_proc, _name) = (GL_PROC_PREFIX(_proc))wglGetProcAddress(#_name);\
        if(_name == nullptr)\
            isle::Book::AddEvent(isle::eNOTE::nALERT, "can't get \"%s()\" procedure.", #_name);
#else
    #define GL_PROC_INIT(_proc, _name)\
        GL_PROC(_proc, _name) = (GL_PROC_PREFIX(_proc))wglGetProcAddress(#_name);
#endif

// glA ---------------------------------------------------------------------------------
__forceinline void APIENTRY glActiveTexture(GLenum texture)
{
    GL_PROC_INIT(GLACTIVETEXTURE, glActiveTexture);
    return glActiveTexture(texture);
}

__forceinline void APIENTRY glAttachShader(GLuint program, GLuint shader)
{
    GL_PROC_INIT(GLATTACHSHADER, glAttachShader);
    return glAttachShader(program, shader);
}

// glB ---------------------------------------------------------------------------------
__forceinline void APIENTRY glBeginConditionalRender(GLuint id, GLenum mode)
{
    GL_PROC_INIT(GLBEGINCONDITIONALRENDER, glBeginConditionalRender);
    return glBeginConditionalRender(id, mode);
}

__forceinline void APIENTRY glBeginQuery(GLenum target, GLuint id)
{
    GL_PROC_INIT(GLBEGINQUERY, glBeginQuery);
    return glBeginQuery(target, id);
}

__forceinline void APIENTRY glBeginTransformFeedback(GLenum primitiveMode)
{
    GL_PROC_INIT(GLBEGINTRANSFORMFEEDBACK, glBeginTransformFeedback);
    return glBeginTransformFeedback(primitiveMode);
}

__forceinline void APIENTRY glBindAttribLocation(GLuint program, GLuint index, const GLchar *name)
{
    GL_PROC_INIT(GLBINDATTRIBLOCATION, glBindAttribLocation);
    return glBindAttribLocation(program, index, name);
}

__forceinline void APIENTRY glBindBuffer(GLenum target, GLuint buffer)
{
    GL_PROC_INIT(GLBINDBUFFER, glBindBuffer);
    return glBindBuffer(target, buffer);
}

__forceinline void APIENTRY glBindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
    GL_PROC_INIT(GLBINDBUFFERBASE, glBindBufferBase);
    return glBindBufferBase(target, index, buffer);
}

__forceinline void APIENTRY glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    GL_PROC_INIT(GLBINDBUFFERRANGE, glBindBufferRange);
    return glBindBufferRange(target, index, buffer, offset, size);
}

__forceinline void APIENTRY glBindFragDataLocation(GLuint program, GLuint color, const GLchar *name)
{
    GL_PROC_INIT(GLBINDFRAGDATALOCATION, glBindFragDataLocation);
    return glBindFragDataLocation(program, color, name);
}

__forceinline void APIENTRY glBindFragDataLocationIndexed(GLuint program, GLuint colorNumber, GLuint index, const GLchar *name)
{
    GL_PROC_INIT(GLBINDFRAGDATALOCATIONINDEXED, glBindFragDataLocationIndexed);
    return glBindFragDataLocationIndexed(program, colorNumber, index, name);
}

__forceinline void APIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer)
{
    GL_PROC_INIT(GLBINDFRAMEBUFFER, glBindFramebuffer);
    return glBindFramebuffer(target, framebuffer);
}

__forceinline void APIENTRY glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
    GL_PROC_INIT(GLBINDRENDERBUFFER, glBindRenderbuffer);
    return glBindRenderbuffer(target, renderbuffer);
}

__forceinline void APIENTRY glBindSampler(GLuint unit, GLuint sampler)
{
    GL_PROC_INIT(GLBINDSAMPLER, glBindSampler);
    return glBindSampler(unit, sampler);
}

__forceinline void APIENTRY glBindVertexArray(GLuint array)
{
    GL_PROC_INIT(GLBINDVERTEXARRAY, glBindVertexArray);
    return glBindVertexArray(array);
}

__forceinline void APIENTRY glBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    GL_PROC_INIT(GLBLENDCOLOR, glBlendColor);
    return glBlendColor(red, green, blue, alpha);
}

__forceinline void APIENTRY glBlendEquation(GLenum mode)
{
    GL_PROC_INIT(GLBLENDEQUATION, glBlendEquation);
    return glBlendEquation(mode);
}

__forceinline void APIENTRY glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
    GL_PROC_INIT(GLBLENDEQUATIONSEPARATE, glBlendEquationSeparate);
    return glBlendEquationSeparate(modeRGB, modeAlpha);
}

__forceinline void APIENTRY glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    GL_PROC_INIT(GLBLENDFUNCSEPARATE, glBlendFuncSeparate);
    return glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

__forceinline void APIENTRY glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    GL_PROC_INIT(GLBLITFRAMEBUFFER, glBlitFramebuffer);
    return glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

__forceinline void APIENTRY glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
    GL_PROC_INIT(GLBUFFERDATA, glBufferData);
    return glBufferData(target, size, data, usage);
}

__forceinline void APIENTRY glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
    GL_PROC_INIT(GLBUFFERSUBDATA, glBufferSubData);
    return glBufferSubData(target, offset, size, data);
}

// glC ---------------------------------------------------------------------------------
__forceinline GLenum APIENTRY glCheckFramebufferStatus(GLenum target)
{
    GL_PROC_INIT(GLCHECKFRAMEBUFFERSTATUS, glCheckFramebufferStatus);
    return glCheckFramebufferStatus(target);
}

__forceinline void APIENTRY glClampColor(GLenum target, GLenum clamp)
{
    GL_PROC_INIT(GLCLAMPCOLOR, glClampColor);
    return glClampColor(target, clamp);
}

__forceinline GLenum APIENTRY glClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    GL_PROC_INIT(GLCLIENTWAITSYNC, glClientWaitSync);
    return glClientWaitSync(sync, flags, timeout);
}

__forceinline void APIENTRY glCompileShader(GLuint shader)
{
    GL_PROC_INIT(GLCOMPILESHADER, glCompileShader);
    return glCompileShader(shader);
}

__forceinline void APIENTRY glCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data)
{
    GL_PROC_INIT(GLCOMPRESSEDTEXIMAGE1D, glCompressedTexImage1D);
    return glCompressedTexImage1D(target, level, internalformat, width, border, imageSize, data);
}

__forceinline void APIENTRY glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{
    GL_PROC_INIT(GLCOMPRESSEDTEXIMAGE2D, glCompressedTexImage2D);
    return glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

__forceinline void APIENTRY glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data)
{
    GL_PROC_INIT(GLCOMPRESSEDTEXIMAGE3D, glCompressedTexImage3D);
    return glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);
}

__forceinline void APIENTRY glCompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    GL_PROC_INIT(GLCOMPRESSEDTEXSUBIMAGE1D, glCompressedTexSubImage1D);
    return glCompressedTexSubImage1D(target, level, xoffset, width, format, imageSize, data);
}

__forceinline void APIENTRY glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    GL_PROC_INIT(GLCOMPRESSEDTEXSUBIMAGE2D, glCompressedTexSubImage2D);
    return glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

__forceinline void APIENTRY glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    GL_PROC_INIT(GLCOMPRESSEDTEXSUBIMAGE3D, glCompressedTexSubImage3D);
    return glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

__forceinline void APIENTRY glCopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    GL_PROC_INIT(GLCOPYBUFFERSUBDATA, glCopyBufferSubData);
    return glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);
}

__forceinline GLuint APIENTRY glCreateProgram(void)
{
    GL_PROC_INIT(GLCREATEPROGRAM, glCreateProgram);
    return glCreateProgram();
}

__forceinline GLuint APIENTRY glCreateShader(GLenum type)
{
    GL_PROC_INIT(GLCREATESHADER, glCreateShader);
    return glCreateShader(type);
}

// glD ---------------------------------------------------------------------------------
__forceinline void APIENTRY glDebugMessageCallbackARB(GLDEBUGPROCARB callback, const GLvoid *userParam)
{
    GL_PROC_INIT(GLDEBUGMESSAGECALLBACKARB, glDebugMessageCallbackARB);
    return glDebugMessageCallbackARB(callback, userParam);
}

__forceinline void APIENTRY glDebugMessageControlARB(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled)
{
    GL_PROC_INIT(GLDEBUGMESSAGECONTROLARB, glDebugMessageControlARB);
    return glDebugMessageControlARB(source, type, severity, count, ids, enabled);
}

__forceinline void APIENTRY glDebugMessageInsertARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf)
{
    GL_PROC_INIT(GLDEBUGMESSAGEINSERTARB, glDebugMessageInsertARB);
    return glDebugMessageInsertARB(source, type, id, severity, length, buf);
}

__forceinline void APIENTRY glDeleteBuffers(GLsizei n, const GLuint *buffers)
{
    GL_PROC_INIT(GLDELETEBUFFERS, glDeleteBuffers);
    return glDeleteBuffers(n, buffers);
}

__forceinline void APIENTRY glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
    GL_PROC_INIT(GLDELETEFRAMEBUFFERS, glDeleteFramebuffers);
    return glDeleteFramebuffers(n, framebuffers);
}

__forceinline void APIENTRY glDeleteProgram(GLuint program)
{
    GL_PROC_INIT(GLDELETEPROGRAM, glDeleteProgram);
    return glDeleteProgram(program);
}

__forceinline void APIENTRY glDeleteQueries(GLsizei n, const GLuint *ids)
{
    GL_PROC_INIT(GLDELETEQUERIES, glDeleteQueries);
    return glDeleteQueries(n, ids);
}

__forceinline void APIENTRY glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
{
    GL_PROC_INIT(GLDELETERENDERBUFFERS, glDeleteRenderbuffers);
    return glDeleteRenderbuffers(n, renderbuffers);
}

__forceinline void APIENTRY glDeleteSamplers(GLsizei count, const GLuint *samplers)
{
    GL_PROC_INIT(GLDELETESAMPLERS, glDeleteSamplers);
    return glDeleteSamplers(count, samplers);
}

__forceinline void APIENTRY glDeleteShader(GLuint shader)
{
    GL_PROC_INIT(GLDELETESHADER, glDeleteShader);
    return glDeleteShader(shader);
}

__forceinline void APIENTRY glDeleteSync(GLsync sync)
{
    GL_PROC_INIT(GLDELETESYNC, glDeleteSync);
    return glDeleteSync(sync);
}

__forceinline void APIENTRY glDeleteVertexArrays(GLsizei n, const GLuint *arrays)
{
    GL_PROC_INIT(GLDELETEVERTEXARRAYS, glDeleteVertexArrays);
    return glDeleteVertexArrays(n, arrays);
}

__forceinline void APIENTRY glDepthRangedNV(GLdouble zNear, GLdouble zFar)
{
    GL_PROC_INIT(GLDEPTHRANGEDNV, glDepthRangedNV);
    return glDepthRangedNV(zNear, zFar);
}

__forceinline void APIENTRY glDetachShader(GLuint program, GLuint shader)
{
    GL_PROC_INIT(GLDETACHSHADER, glDetachShader);
    return glDetachShader(program, shader);
}

__forceinline void APIENTRY glDisableVertexAttribArray(GLuint index)
{
    GL_PROC_INIT(GLDISABLEVERTEXATTRIBARRAY, glDisableVertexAttribArray);
    return glDisableVertexAttribArray(index);
}

__forceinline void APIENTRY glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
    GL_PROC_INIT(GLDRAWARRAYSINSTANCED, glDrawArraysInstanced);
    return glDrawArraysInstanced(mode, first, count, primcount);
}

__forceinline void APIENTRY glDrawBuffers(GLsizei n, const GLenum *bufs)
{
    GL_PROC_INIT(GLDRAWBUFFERS, glDrawBuffers);
    return glDrawBuffers(n, bufs);
}

__forceinline void APIENTRY glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex)
{
    GL_PROC_INIT(GLDRAWELEMENTSBASEVERTEX, glDrawElementsBaseVertex);
    return glDrawElementsBaseVertex(mode, count, type, indices, basevertex);
}

__forceinline void APIENTRY glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount)
{
    GL_PROC_INIT(GLDRAWELEMENTSINSTANCED, glDrawElementsInstanced);
    return glDrawElementsInstanced(mode, count, type, indices, primcount);
}

__forceinline void APIENTRY glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount, GLint basevertex)
{
    GL_PROC_INIT(GLDRAWELEMENTSINSTANCEDBASEVERTEX, glDrawElementsInstancedBaseVertex);
    return glDrawElementsInstancedBaseVertex(mode, count, type, indices, primcount, basevertex);
}

__forceinline void APIENTRY glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices)
{
    GL_PROC_INIT(GLDRAWRANGEELEMENTS, glDrawRangeElements);
    return glDrawRangeElements(mode, start, end, count, type, indices);
}

__forceinline void APIENTRY glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex)
{
    GL_PROC_INIT(GLDRAWRANGEELEMENTSBASEVERTEX, glDrawRangeElementsBaseVertex);
    return glDrawRangeElementsBaseVertex(mode, start, end, count, type, indices, basevertex);
}

// glE ---------------------------------------------------------------------------------
__forceinline void APIENTRY glEnableVertexAttribArray(GLuint index)
{
    GL_PROC_INIT(GLENABLEVERTEXATTRIBARRAY, glEnableVertexAttribArray);
    return glEnableVertexAttribArray(index);
}

__forceinline void APIENTRY glEndConditionalRender(void)
{
    GL_PROC_INIT(GLENDCONDITIONALRENDER, glEndConditionalRender);
    return glEndConditionalRender();
}

__forceinline void APIENTRY glEndQuery(GLenum target)
{
    GL_PROC_INIT(GLENDQUERY, glEndQuery);
    return glEndQuery(target);
}

__forceinline void APIENTRY glEndTransformFeedback(void)
{
    GL_PROC_INIT(GLENDTRANSFORMFEEDBACK, glEndTransformFeedback);
    return glEndTransformFeedback();
}

// glF ---------------------------------------------------------------------------------
__forceinline GLsync APIENTRY glFenceSync(GLenum condition, GLbitfield flags)
{
    GL_PROC_INIT(GLFENCESYNC, glFenceSync);
    return glFenceSync(condition, flags);
}

__forceinline void APIENTRY glFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length)
{
    GL_PROC_INIT(GLFLUSHMAPPEDBUFFERRANGE, glFlushMappedBufferRange);
    return glFlushMappedBufferRange(target, offset, length);
}

__forceinline void APIENTRY glFlushMappedNamedBufferRangeEXT(GLenum target, GLintptr offset, GLsizeiptr length)
{
    GL_PROC_INIT(GLFLUSHMAPPEDNAMEDBUFFERRANGEEXT, glFlushMappedNamedBufferRangeEXT);
    return glFlushMappedNamedBufferRangeEXT(target, offset, length);
}

__forceinline void APIENTRY glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    GL_PROC_INIT(GLFRAMEBUFFERRENDERBUFFER, glFramebufferRenderbuffer);
    return glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

__forceinline void APIENTRY glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level)
{
    GL_PROC_INIT(GLFRAMEBUFFERTEXTURE, glFramebufferTexture);
    return glFramebufferTexture(target, attachment, texture, level);
}

__forceinline void APIENTRY glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    GL_PROC_INIT(GLFRAMEBUFFERTEXTURELAYER, glFramebufferTextureLayer);
    return glFramebufferTextureLayer(target, attachment, texture, level, layer);
}

// glG ---------------------------------------------------------------------------------
__forceinline void APIENTRY glGenBuffers(GLsizei n, GLuint *buffers)
{
    GL_PROC_INIT(GLGENBUFFERS, glGenBuffers);
    return glGenBuffers(n, buffers);
}

__forceinline void APIENTRY glGenerateTextureMipmapEXT(GLuint texture, GLenum target)
{
    GL_PROC_INIT(GLGENERATETEXTUREMIPMAPEXT, glGenerateTextureMipmapEXT);
    return glGenerateTextureMipmapEXT(texture, target);
}

__forceinline void APIENTRY glGenFramebuffers(GLsizei n, GLuint *framebuffers)
{
    GL_PROC_INIT(GLGENFRAMEBUFFERS, glGenFramebuffers);
    return glGenFramebuffers(n, framebuffers);
}

__forceinline void APIENTRY glGenQueries(GLsizei n, GLuint *ids)
{
    GL_PROC_INIT(GLGENQUERIES, glGenQueries);
    return glGenQueries(n, ids);
}

__forceinline void APIENTRY glGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
    GL_PROC_INIT(GLGENRENDERBUFFERS, glGenRenderbuffers);
    return glGenRenderbuffers(n, renderbuffers);
}

__forceinline void APIENTRY glGenSamplers(GLsizei count, GLuint *samplers)
{
    GL_PROC_INIT(GLGENSAMPLERS, glGenSamplers);
    return glGenSamplers(count, samplers);
}

__forceinline void APIENTRY glGenVertexArrays(GLsizei n, GLuint *arrays)
{
    GL_PROC_INIT(GLGENVERTEXARRAYS, glGenVertexArrays);
    return glGenVertexArrays(n, arrays);
}

__forceinline void APIENTRY glGenerateMipmap(GLenum target)
{
    GL_PROC_INIT(GLGENERATEMIPMAP, glGenerateMipmap);
    return glGenerateMipmap(target);
}

__forceinline void APIENTRY glGetBooleani_v(GLenum target, GLuint index, GLboolean *data)
{
    GL_PROC_INIT(GLGETBOOLEANI_V, glGetBooleani_v);
    return glGetBooleani_v(target, index, data);
}

__forceinline void APIENTRY glGetIntegeri_v(GLenum target, GLuint index, GLint *data)
{
    GL_PROC_INIT(GLGETINTEGERI_V, glGetIntegeri_v);
    return glGetIntegeri_v(target, index, data);
}

__forceinline void APIENTRY glGetInteger64v(GLenum pname, GLint64 *params)
{
    GL_PROC_INIT(GLGETINTEGER64V, glGetInteger64v);
    return glGetInteger64v(pname, params);
}

__forceinline void APIENTRY glGetInteger64i_v(GLenum target, GLuint index, GLint64 *data)
{
    GL_PROC_INIT(GLGETINTEGER64I_V, glGetInteger64i_v);
    return glGetInteger64i_v(target, index, data);
}

__forceinline void APIENTRY glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    GL_PROC_INIT(GLGETACTIVEATTRIB, glGetActiveAttrib);
    return glGetActiveAttrib(program, index, bufSize, length, size, type, name);
}

__forceinline void APIENTRY glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    GL_PROC_INIT(GLGETACTIVEUNIFORM, glGetActiveUniform);
    return glGetActiveUniform(program, index, bufSize, length, size, type, name);
}

__forceinline void APIENTRY glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETACTIVEUNIFORMBLOCKIV, glGetActiveUniformBlockiv);
    return glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
}

__forceinline void APIENTRY glGetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName)
{
    GL_PROC_INIT(GLGETACTIVEUNIFORMBLOCKNAME, glGetActiveUniformBlockName);
    return glGetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName);
}

__forceinline void APIENTRY glGetActiveUniformName(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName)
{
    GL_PROC_INIT(GLGETACTIVEUNIFORMNAME, glGetActiveUniformName);
    return glGetActiveUniformName(program, uniformIndex, bufSize, length, uniformName);
}

__forceinline void APIENTRY glGetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETACTIVEUNIFORMSIV, glGetActiveUniformsiv);
    return glGetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params);
}

__forceinline void APIENTRY glGetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *obj)
{
    GL_PROC_INIT(GLGETATTACHEDSHADERS, glGetAttachedShaders);
    return glGetAttachedShaders(program, maxCount, count, obj);
}

__forceinline GLint APIENTRY glGetAttribLocation(GLuint program, const GLchar *name)
{
    GL_PROC_INIT(GLGETATTRIBLOCATION, glGetAttribLocation);
    return glGetAttribLocation(program, name);
}

__forceinline void APIENTRY glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETBUFFERPARAMETERIV, glGetBufferParameteriv);
    return glGetBufferParameteriv(target, pname, params);
}

__forceinline void APIENTRY glGetBufferPointerv(GLenum target, GLenum pname, GLvoid **params)
{
    GL_PROC_INIT(GLGETBUFFERPOINTERV, glGetBufferPointerv);
    return glGetBufferPointerv(target, pname, params);
}

__forceinline void APIENTRY glGetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data)
{
    GL_PROC_INIT(GLGETBUFFERSUBDATA, glGetBufferSubData);
    return glGetBufferSubData(target, offset, size, data);
}

__forceinline void APIENTRY glGetCompressedTexImage(GLenum target, GLint level, GLvoid *img)
{
    GL_PROC_INIT(GLGETCOMPRESSEDTEXIMAGE, glGetCompressedTexImage);
    return glGetCompressedTexImage(target, level, img);
}

__forceinline GLuint APIENTRY glGetDebugMessageLogARB(GLuint count, GLsizei bufsize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog)
{
    GL_PROC_INIT(GLGETDEBUGMESSAGELOGARB, glGetDebugMessageLogARB);
    return glGetDebugMessageLogARB(count, bufsize, sources, types, ids, severities, lengths, messageLog);
}

__forceinline GLint APIENTRY glGetFragDataIndex(GLuint program, const GLchar *name)
{
    GL_PROC_INIT(GLGETFRAGDATAINDEX, glGetFragDataIndex);
    return glGetFragDataIndex(program, name);
}

__forceinline GLint APIENTRY glGetFragDataLocation(GLuint program, const GLchar *name)
{
    GL_PROC_INIT(GLGETFRAGDATALOCATION, glGetFragDataLocation);
    return glGetFragDataLocation(program, name);
}

__forceinline void APIENTRY glGetMultisamplefv(GLenum pname, GLuint index, GLfloat *val)
{
    GL_PROC_INIT(GLGETMULTISAMPLEFV, glGetMultisamplefv);
    return glGetMultisamplefv(pname, index, val);
}

__forceinline void APIENTRY glGetProgramiv(GLuint program, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETPROGRAMIV, glGetProgramiv);
    return glGetProgramiv(program, pname, params);
}

__forceinline void APIENTRY glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    GL_PROC_INIT(GLGETPROGRAMINFOLOG, glGetProgramInfoLog);
    return glGetProgramInfoLog(program, bufSize, length, infoLog);
}

__forceinline GLuint APIENTRY glGetProgramResourceIndex(GLuint program, GLenum programInterface, const GLchar *name)
{
    GL_PROC_INIT(GLGETPROGRAMRESOURCEINDEX, glGetProgramResourceIndex);
    return glGetProgramResourceIndex(program, programInterface, name);
}

__forceinline void APIENTRY glGetQueryObjectiv(GLuint id, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETQUERYOBJECTIV, glGetQueryObjectiv);
    return glGetQueryObjectiv(id, pname, params);
}

__forceinline void APIENTRY glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params)
{
    GL_PROC_INIT(GLGETQUERYOBJECTUIV, glGetQueryObjectuiv);
    return glGetQueryObjectuiv(id, pname, params);
}

__forceinline void APIENTRY glGetQueryObjecti64v(GLuint id, GLenum pname, GLint64 *params)
{
    GL_PROC_INIT(GLGETQUERYOBJECTI64V, glGetQueryObjecti64v);
    return glGetQueryObjecti64v(id, pname, params);
}

__forceinline void APIENTRY glGetQueryObjectui64v(GLuint id, GLenum pname, GLuint64 *params)
{
    GL_PROC_INIT(GLGETQUERYOBJECTUI64V, glGetQueryObjectui64v);
    return glGetQueryObjectui64v(id, pname, params);
}

__forceinline void APIENTRY glGetQueryiv(GLenum target, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETQUERYIV, glGetQueryiv);
    return glGetQueryiv(target, pname, params);
}

__forceinline void APIENTRY glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETRENDERBUFFERPARAMETERIV, glGetRenderbufferParameteriv);
    return glGetRenderbufferParameteriv(target, pname, params);
}

__forceinline void APIENTRY glGetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat *params)
{
    GL_PROC_INIT(GLGETSAMPLERPARAMETERFV, glGetSamplerParameterfv);
    return glGetSamplerParameterfv(sampler, pname, params);
}

__forceinline void APIENTRY glGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETSAMPLERPARAMETERIV, glGetSamplerParameteriv);
    return glGetSamplerParameteriv(sampler, pname, params);
}

__forceinline void APIENTRY glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETSHADERIV, glGetShaderiv);
    return glGetShaderiv(shader, pname, params);
}

__forceinline void APIENTRY glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    GL_PROC_INIT(GLGETSHADERINFOLOG, glGetShaderInfoLog);
    return glGetShaderInfoLog(shader, bufSize, length, infoLog);
}

__forceinline void APIENTRY glGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source)
{
    GL_PROC_INIT(GLGETSHADERSOURCE, glGetShaderSource);
    return glGetShaderSource(shader, bufSize, length, source);
}

__forceinline const GLubyte * APIENTRY glGetStringi(GLenum name, GLuint index)
{
    GL_PROC_INIT(GLGETSTRINGI, glGetStringi);
    return glGetStringi(name, index);
}

__forceinline void APIENTRY glGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values)
{
    GL_PROC_INIT(GLGETSYNCIV, glGetSynciv);
    return glGetSynciv(sync, pname, bufSize, length, values);
}

__forceinline void APIENTRY glGetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name)
{
    GL_PROC_INIT(GLGETTRANSFORMFEEDBACKVARYING, glGetTransformFeedbackVarying);
    return glGetTransformFeedbackVarying(program, index, bufSize, length, size, type, name);
}

__forceinline void APIENTRY glGetUniformfv(GLuint program, GLint location, GLfloat *params)
{
    GL_PROC_INIT(GLGETUNIFORMFV, glGetUniformfv);
    return glGetUniformfv(program, location, params);
}

__forceinline void APIENTRY glGetUniformiv(GLuint program, GLint location, GLint *params)
{
    GL_PROC_INIT(GLGETUNIFORMIV, glGetUniformiv);
    return glGetUniformiv(program, location, params);
}

__forceinline GLuint APIENTRY glGetUniformBlockIndex(GLuint program, const GLchar *uniformBlockName)
{
    GL_PROC_INIT(GLGETUNIFORMBLOCKINDEX, glGetUniformBlockIndex);
    return glGetUniformBlockIndex(program, uniformBlockName);
}

__forceinline void APIENTRY glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar* const *uniformNames, GLuint *uniformIndices)
{
    GL_PROC_INIT(GLGETUNIFORMINDICES, glGetUniformIndices);
    return glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices);
}

__forceinline GLint APIENTRY glGetUniformLocation(GLuint program, const GLchar *name)
{
    GL_PROC_INIT(GLGETUNIFORMLOCATION, glGetUniformLocation);
    return glGetUniformLocation(program, name);
}

__forceinline void APIENTRY glGetVertexAttribdv(GLuint index, GLenum pname, GLdouble *params)
{
    GL_PROC_INIT(GLGETVERTEXATTRIBDV, glGetVertexAttribdv);
    return glGetVertexAttribdv(index, pname, params);
}

__forceinline void APIENTRY glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat *params)
{
    GL_PROC_INIT(GLGETVERTEXATTRIBFV, glGetVertexAttribfv);
    return glGetVertexAttribfv(index, pname, params);
}

__forceinline void APIENTRY glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETVERTEXATTRIBIV, glGetVertexAttribiv);
    return glGetVertexAttribiv(index, pname, params);
}

__forceinline void APIENTRY glGetVertexAttribIiv(GLuint index, GLenum pname, GLint *params)
{
    GL_PROC_INIT(GLGETVERTEXATTRIBIIV, glGetVertexAttribIiv);
    return glGetVertexAttribIiv(index, pname, params);
}

__forceinline void APIENTRY glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint *params)
{
    GL_PROC_INIT(GLGETVERTEXATTRIBIUIV, glGetVertexAttribIuiv);
    return glGetVertexAttribIuiv(index, pname, params);
}

__forceinline void APIENTRY glGetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid **pointer)
{
    GL_PROC_INIT(GLGETVERTEXATTRIBPOINTERV, glGetVertexAttribPointerv);
    return glGetVertexAttribPointerv(index, pname, pointer);
}

// glI ---------------------------------------------------------------------------------
__forceinline GLboolean APIENTRY glIsBuffer(GLuint buffer)
{
    GL_PROC_INIT(GLISBUFFER, glIsBuffer);
    return glIsBuffer(buffer);
}

__forceinline GLboolean APIENTRY glIsFramebuffer(GLuint framebuffer)
{
    GL_PROC_INIT(GLISFRAMEBUFFER, glIsFramebuffer);
    return glIsFramebuffer(framebuffer);
}

__forceinline GLboolean APIENTRY glIsProgram(GLuint program)
{
    GL_PROC_INIT(GLISPROGRAM, glIsProgram);
    return glIsProgram(program);
}

__forceinline GLboolean APIENTRY glIsQuery(GLuint id)
{
    GL_PROC_INIT(GLISQUERY, glIsQuery);
    return glIsQuery(id);
}

__forceinline GLboolean APIENTRY glIsRenderbuffer(GLuint renderbuffer)
{
    GL_PROC_INIT(GLISRENDERBUFFER, glIsRenderbuffer);
    return glIsRenderbuffer(renderbuffer);
}

__forceinline GLboolean APIENTRY glIsSampler(GLuint sampler)
{
    GL_PROC_INIT(GLISSAMPLER, glIsSampler);
    return glIsSampler(sampler);
}

__forceinline GLboolean APIENTRY glIsShader(GLuint shader)
{
    GL_PROC_INIT(GLISSHADER, glIsShader);
    return glIsShader(shader);
}

__forceinline GLboolean APIENTRY glIsSync(GLsync sync)
{
    GL_PROC_INIT(GLISSYNC, glIsSync);
    return glIsSync(sync);
}

__forceinline GLboolean APIENTRY glIsVertexArray(GLuint array)
{
    GL_PROC_INIT(GLISVERTEXARRAY, glIsVertexArray);
    return glIsVertexArray(array);
}

// glL ---------------------------------------------------------------------------------
__forceinline void APIENTRY glLinkProgram(GLuint program)
{
    GL_PROC_INIT(GLLINKPROGRAM, glLinkProgram);
    return glLinkProgram(program);
}

// glM ---------------------------------------------------------------------------------
__forceinline GLvoid* APIENTRY glMapBuffer(GLenum target, GLenum access)
{
    GL_PROC_INIT(GLMAPBUFFER, glMapBuffer);
    return glMapBuffer(target, access);
}

__forceinline GLvoid* APIENTRY glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
    GL_PROC_INIT(GLMAPBUFFERRANGE, glMapBufferRange);
    return glMapBufferRange(target, offset, length, access);
}

__forceinline GLvoid* APIENTRY glMapNamedBufferEXT(GLuint buffer, GLenum access)
{
    GL_PROC_INIT(GLMAPNAMEDBUFFEREXT, glMapNamedBufferEXT);
    return glMapNamedBufferEXT(buffer, access);
}

__forceinline GLvoid* APIENTRY glMapNamedBufferRangeEXT(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
    GL_PROC_INIT(GLMAPNAMEDBUFFERRANGEEXT, glMapNamedBufferRangeEXT);
    return glMapNamedBufferRangeEXT(target, offset, length, access);
}

__forceinline void APIENTRY glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount)
{
    GL_PROC_INIT(GLMULTIDRAWARRAYS, glMultiDrawArrays);
    return glMultiDrawArrays(mode, first, count, primcount);
}

__forceinline void APIENTRY glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLvoid* const *indices, GLsizei primcount)
{
    GL_PROC_INIT(GLMULTIDRAWELEMENTS, glMultiDrawElements);
    return glMultiDrawElements(mode, count, type, indices, primcount);
}

__forceinline void APIENTRY glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLvoid* const *indices, GLsizei primcount, const GLint *basevertex)
{
    GL_PROC_INIT(GLMULTIDRAWELEMENTSBASEVERTEX, glMultiDrawElementsBaseVertex);
    return glMultiDrawElementsBaseVertex(mode, count, type, indices, primcount, basevertex);
}

// glN ---------------------------------------------------------------------------------
__forceinline void APIENTRY glNamedCopyBufferSubDataEXT(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    GL_PROC_INIT(GLNAMEDCOPYBUFFERSUBDATAEXT, glNamedCopyBufferSubDataEXT);
    return glNamedCopyBufferSubDataEXT(readBuffer, writeBuffer, readOffset, writeOffset, size);
}

__forceinline void APIENTRY glNamedBufferDataEXT(GLuint buffer, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
    GL_PROC_INIT(GLNAMEDBUFFERDATAEXT, glNamedBufferDataEXT);
    return glNamedBufferDataEXT(buffer, size, data, usage);
}

__forceinline void APIENTRY glNamedBufferSubDataEXT(GLuint buffer, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
    GL_PROC_INIT(GLNAMEDBUFFERSUBDATAEXT, glNamedBufferSubDataEXT);
    return glNamedBufferSubDataEXT(buffer, offset, size, data);
}

// glP ---------------------------------------------------------------------------------
__forceinline void APIENTRY glPointParameterf(GLenum pname, GLfloat param)
{
    GL_PROC_INIT(GLPOINTPARAMETERF, glPointParameterf);
    return glPointParameterf(pname, param);
}

__forceinline void APIENTRY glPointParameteri(GLenum pname, GLint param)
{
    GL_PROC_INIT(GLPOINTPARAMETERI, glPointParameteri);
    return glPointParameteri(pname, param);
}

__forceinline void APIENTRY glPrimitiveRestartIndex(GLuint index)
{
    GL_PROC_INIT(GLPRIMITIVERESTARTINDEX, glPrimitiveRestartIndex);
    return glPrimitiveRestartIndex(index);
}

__forceinline void APIENTRY glGetProgramBinary(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, GLvoid *binary)
{
    GL_PROC_INIT(GLGETPROGRAMBINARY, glGetProgramBinary);
    return glGetProgramBinary(program, bufSize, length, binaryFormat, binary);
}

__forceinline void APIENTRY glProgramParameteri(GLuint program, GLenum pname, GLint value)
{
    GL_PROC_INIT(GLPROGRAMPARAMETERI, glProgramParameteri);
    return glProgramParameteri(program, pname, value);
}

__forceinline void APIENTRY glProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM2F, glProgramUniform2f);
    return glProgramUniform2f(program, location, v0, v1);
}

__forceinline void APIENTRY glProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM3F, glProgramUniform3f);
    return glProgramUniform3f(program, location, v0, v1, v2);
}

__forceinline void APIENTRY glProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM4F, glProgramUniform4f);
    return glProgramUniform4f(program, location, v0, v1, v2, v3);
}

__forceinline void APIENTRY glProgramUniform1i(GLuint program, GLint location, GLint v0)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM1I, glProgramUniform1i);
    return glProgramUniform1i(program, location, v0);
}

__forceinline void APIENTRY glProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM2I, glProgramUniform2i);
    return glProgramUniform2i(program, location, v0, v1);
}

__forceinline void APIENTRY glProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM3I, glProgramUniform3i);
    return glProgramUniform3i(program, location, v0, v1, v2);
}

__forceinline void APIENTRY glProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM4I, glProgramUniform4i);
    return glProgramUniform4i(program, location, v0, v1, v2, v3);
}

__forceinline void APIENTRY glProgramUniform1ui(GLuint program, GLint location, GLuint v0)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM1UI, glProgramUniform1ui);
    return glProgramUniform1ui(program, location, v0);
}

__forceinline void APIENTRY glProgramUniform2ui(GLuint program, GLint location, GLuint v0, GLuint v1)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM2UI, glProgramUniform2ui);
    return glProgramUniform2ui(program, location, v0, v1);
}

__forceinline void APIENTRY glProgramUniform3ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM3UI, glProgramUniform3ui);
    return glProgramUniform3ui(program, location, v0, v1, v2);
}

__forceinline void APIENTRY glProgramUniform4ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM4UI, glProgramUniform4ui);
    return glProgramUniform4ui(program, location, v0, v1, v2, v3);
}

__forceinline void APIENTRY glProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM1FV, glProgramUniform1fv);
    return glProgramUniform1fv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM2FV, glProgramUniform2fv);
    return glProgramUniform2fv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM3FV, glProgramUniform3fv);
    return glProgramUniform3fv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM4FV, glProgramUniform4fv);
    return glProgramUniform4fv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM1IV, glProgramUniform1iv);
    return glProgramUniform1iv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM2IV, glProgramUniform2iv);
    return glProgramUniform2iv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM3IV, glProgramUniform3iv);
    return glProgramUniform3iv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM4IV, glProgramUniform4iv);
    return glProgramUniform4iv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform1uiv(GLuint program, GLint location, GLsizei count, const GLuint *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM1UIV, glProgramUniform1uiv);
    return glProgramUniform1uiv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform2uiv(GLuint program, GLint location, GLsizei count, const GLuint *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM2UIV, glProgramUniform2uiv);
    return glProgramUniform2uiv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform3uiv(GLuint program, GLint location, GLsizei count, const GLuint *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM3UIV, glProgramUniform3uiv);
    return glProgramUniform3uiv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniform4uiv(GLuint program, GLint location, GLsizei count, const GLuint *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORM4UIV, glProgramUniform4uiv);
    return glProgramUniform4uiv(program, location, count, value);
}

__forceinline void APIENTRY glProgramUniformMatrix2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORMMATRIX2FV, glProgramUniformMatrix2fv);
    return glProgramUniformMatrix2fv(program, location, count, transpose, value);
}

__forceinline void APIENTRY glProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORMMATRIX3FV, glProgramUniformMatrix3fv);
    return glProgramUniformMatrix3fv(program, location, count, transpose, value);
}

__forceinline void APIENTRY glProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORMMATRIX4FV, glProgramUniformMatrix4fv);
    return glProgramUniformMatrix4fv(program, location, count, transpose, value);
}

__forceinline void APIENTRY glProgramUniformMatrix2x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORMMATRIX2X3FV, glProgramUniformMatrix2x3fv);
    return glProgramUniformMatrix2x3fv(program, location, count, transpose, value);
}

__forceinline void APIENTRY glProgramUniformMatrix3x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORMMATRIX3X2FV, glProgramUniformMatrix3x2fv);
    return glProgramUniformMatrix3x2fv(program, location, count, transpose, value);
}

__forceinline void APIENTRY glProgramUniformMatrix2x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORMMATRIX2X4FV, glProgramUniformMatrix2x4fv);
    return glProgramUniformMatrix2x4fv(program, location, count, transpose, value);
}

__forceinline void APIENTRY glProgramUniformMatrix4x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORMMATRIX4X2FV, glProgramUniformMatrix4x2fv);
    return glProgramUniformMatrix4x2fv(program, location, count, transpose, value);
}

__forceinline void APIENTRY glProgramUniformMatrix3x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORMMATRIX3X4FV, glProgramUniformMatrix3x4fv);
    return glProgramUniformMatrix3x4fv(program, location, count, transpose, value);
}

__forceinline void APIENTRY glProgramUniformMatrix4x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLPROGRAMUNIFORMMATRIX4X3FV, glProgramUniformMatrix4x3fv);
    return glProgramUniformMatrix4x3fv(program, location, count, transpose, value);
}

__forceinline void APIENTRY glProvokingVertex(GLenum mode)
{
    GL_PROC_INIT(GLPROVOKINGVERTEX, glProvokingVertex);
    return glProvokingVertex(mode);
}

// glQ ---------------------------------------------------------------------------------
__forceinline void APIENTRY glQueryCounter(GLuint id, GLenum target)
{
    GL_PROC_INIT(GLQUERYCOUNTER, glQueryCounter);
    return glQueryCounter(id, target);
}

// glR ---------------------------------------------------------------------------------
__forceinline void APIENTRY glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    GL_PROC_INIT(GLRENDERBUFFERSTORAGE, glRenderbufferStorage);
    return glRenderbufferStorage(target, internalformat, width, height);
}

__forceinline void APIENTRY glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    GL_PROC_INIT(GLRENDERBUFFERSTORAGEMULTISAMPLE, glRenderbufferStorageMultisample);
    return glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
}

// glS ---------------------------------------------------------------------------------
__forceinline void APIENTRY glSampleCoverage(GLfloat value, GLboolean invert)
{
    GL_PROC_INIT(GLSAMPLECOVERAGE, glSampleCoverage);
    return glSampleCoverage(value, invert);
}

__forceinline void APIENTRY glSampleMaski(GLuint index, GLbitfield mask)
{
    GL_PROC_INIT(GLSAMPLEMASKI, glSampleMaski);
    return glSampleMaski(index, mask);
}

__forceinline void APIENTRY glSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param)
{
    GL_PROC_INIT(GLSAMPLERPARAMETERF, glSamplerParameterf);
    return glSamplerParameterf(sampler, pname, param);
}

__forceinline void APIENTRY glSamplerParameteri(GLuint sampler, GLenum pname, GLint param)
{
    GL_PROC_INIT(GLSAMPLERPARAMETERI, glSamplerParameteri);
    return glSamplerParameteri(sampler, pname, param);
}

__forceinline void APIENTRY glShaderSource(GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length)
{
    GL_PROC_INIT(GLSHADERSOURCE, glShaderSource);
    return glShaderSource(shader, count, string, length);
}

__forceinline void APIENTRY glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
    GL_PROC_INIT(GLSTENCILFUNCSEPARATE, glStencilFuncSeparate);
    return glStencilFuncSeparate(face, func, ref, mask);
}

__forceinline void APIENTRY glStencilMaskSeparate(GLenum face, GLuint mask)
{
    GL_PROC_INIT(GLSTENCILMASKSEPARATE, glStencilMaskSeparate);
    return glStencilMaskSeparate(face, mask);
}

__forceinline void APIENTRY glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    GL_PROC_INIT(GLSTENCILOPSEPARATE, glStencilOpSeparate);
    return glStencilOpSeparate(face, sfail, dpfail, dppass);
}

// glT ---------------------------------------------------------------------------------
__forceinline void APIENTRY glTexBuffer(GLenum target, GLenum internalformat, GLuint buffer)
{
    GL_PROC_INIT(GLTEXBUFFER, glTexBuffer);
    return glTexBuffer(target, internalformat, buffer);
}

__forceinline void APIENTRY glTexImage2DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations)
{
    GL_PROC_INIT(GLTEXIMAGE2DMULTISAMPLE, glTexImage2DMultisample);
    return glTexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations);
}

__forceinline void APIENTRY glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    GL_PROC_INIT(GLTEXIMAGE3D, glTexImage3D);
    return glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
}

__forceinline void APIENTRY glTexImage3DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations)
{
    GL_PROC_INIT(GLTEXIMAGE3DMULTISAMPLE, glTexImage3DMultisample);
    return glTexImage3DMultisample(target, samples, internalformat, width, height, depth, fixedsamplelocations);
}

__forceinline void APIENTRY glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels)
{
    GL_PROC_INIT(GLTEXSUBIMAGE3D, glTexSubImage3D);
    return glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

__forceinline void APIENTRY glTransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar* const *varyings, GLenum bufferMode)
{
    GL_PROC_INIT(GLTRANSFORMFEEDBACKVARYINGS, glTransformFeedbackVaryings);
    return glTransformFeedbackVaryings(program, count, varyings, bufferMode);
}

// glU ---------------------------------------------------------------------------------
__forceinline void APIENTRY glUniform1f(GLint location, GLfloat v0)
{
    GL_PROC_INIT(GLUNIFORM1F, glUniform1f);
    return glUniform1f(location, v0);
}

__forceinline void APIENTRY glUniform2f(GLint location, GLfloat v0, GLfloat v1)
{
    GL_PROC_INIT(GLUNIFORM2F, glUniform2f);
    return glUniform2f(location, v0, v1);
}

__forceinline void APIENTRY glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    GL_PROC_INIT(GLUNIFORM3F, glUniform3f);
    return glUniform3f(location, v0, v1, v2);
}

__forceinline void APIENTRY glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GL_PROC_INIT(GLUNIFORM4F, glUniform4f);
    return glUniform4f(location, v0, v1, v2, v3);
}

__forceinline void APIENTRY glUniform1i(GLint location, GLint v0)
{
    GL_PROC_INIT(GLUNIFORM1I, glUniform1i);
    return glUniform1i(location, v0);
}

__forceinline void APIENTRY glUniform2i(GLint location, GLint v0, GLint v1)
{
    GL_PROC_INIT(GLUNIFORM2I, glUniform2i);
    return glUniform2i(location, v0, v1);
}

__forceinline void APIENTRY glUniform3i(GLint location, GLint v0, GLint v1, GLint v2)
{
    GL_PROC_INIT(GLUNIFORM3I, glUniform3i);
    return glUniform3i(location, v0, v1, v2);
}

__forceinline void APIENTRY glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    GL_PROC_INIT(GLUNIFORM4I, glUniform4i);
    return glUniform4i(location, v0, v1, v2, v3);
}

__forceinline void APIENTRY glUniform1ui(GLint location, GLuint v0)
{
    GL_PROC_INIT(GLUNIFORM1UI, glUniform1ui);
    return glUniform1ui(location, v0);
}

__forceinline void APIENTRY glUniform2ui(GLint location, GLuint v0, GLuint v1)
{
    GL_PROC_INIT(GLUNIFORM2UI, glUniform2ui);
    return glUniform2ui(location, v0, v1);
}

__forceinline void APIENTRY glUniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    GL_PROC_INIT(GLUNIFORM3UI, glUniform3ui);
    return glUniform3ui(location, v0, v1, v2);
}

__forceinline void APIENTRY glUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    GL_PROC_INIT(GLUNIFORM4UI, glUniform4ui);
    return glUniform4ui(location, v0, v1, v2, v3);
}

__forceinline void APIENTRY glUniform1fv(GLint location, GLsizei count, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORM1FV, glUniform1fv);
    return glUniform1fv(location, count, value);
}

__forceinline void APIENTRY glUniform2fv(GLint location, GLsizei count, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORM2FV, glUniform2fv);
    return glUniform2fv(location, count, value);
}

__forceinline void APIENTRY glUniform3fv(GLint location, GLsizei count, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORM3FV, glUniform3fv);
    return glUniform3fv(location, count, value);
}

__forceinline void APIENTRY glUniform4fv(GLint location, GLsizei count, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORM4FV, glUniform4fv);
    return glUniform4fv(location, count, value);
}

__forceinline void APIENTRY glUniform1iv(GLint location, GLsizei count, const GLint *value)
{
    GL_PROC_INIT(GLUNIFORM1IV, glUniform1iv);
    return glUniform1iv(location, count, value);
}

__forceinline void APIENTRY glUniform2iv(GLint location, GLsizei count, const GLint *value)
{
    GL_PROC_INIT(GLUNIFORM2IV, glUniform2iv);
    return glUniform2iv(location, count, value);
}

__forceinline void APIENTRY glUniform3iv(GLint location, GLsizei count, const GLint *value)
{
    GL_PROC_INIT(GLUNIFORM3IV, glUniform3iv);
    return glUniform3iv(location, count, value);
}

__forceinline void APIENTRY glUniform4iv(GLint location, GLsizei count, const GLint *value)
{
    GL_PROC_INIT(GLUNIFORM4IV, glUniform4iv);
    return glUniform4iv(location, count, value);
}

__forceinline void APIENTRY glUniform1uiv(GLint location, GLsizei count, const GLuint *value)
{
    GL_PROC_INIT(GLUNIFORM1UIV, glUniform1uiv);
    return glUniform1uiv(location, count, value);
}

__forceinline void APIENTRY glUniform2uiv(GLint location, GLsizei count, const GLuint *value)
{
    GL_PROC_INIT(GLUNIFORM2UIV, glUniform2uiv);
    return glUniform2uiv(location, count, value);
}

__forceinline void APIENTRY glUniform3uiv(GLint location, GLsizei count, const GLuint *value)
{
    GL_PROC_INIT(GLUNIFORM3UIV, glUniform3uiv);
    return glUniform3uiv(location, count, value);
}

__forceinline void APIENTRY glUniform4uiv(GLint location, GLsizei count, const GLuint *value)
{
    GL_PROC_INIT(GLUNIFORM4UIV, glUniform4uiv);
    return glUniform4uiv(location, count, value);
}

__forceinline void APIENTRY glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORMMATRIX2FV, glUniformMatrix2fv);
    return glUniformMatrix2fv(location, count, transpose, value);
}

__forceinline void APIENTRY glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORMMATRIX3FV, glUniformMatrix3fv);
    return glUniformMatrix3fv(location, count, transpose, value);
}

__forceinline void APIENTRY glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORMMATRIX4FV, glUniformMatrix4fv);
    return glUniformMatrix4fv(location, count, transpose, value);
}

__forceinline void APIENTRY glUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORMMATRIX2X3FV, glUniformMatrix2x3fv);
    return glUniformMatrix2x3fv(location, count, transpose, value);
}

__forceinline void APIENTRY glUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORMMATRIX3X2FV, glUniformMatrix3x2fv);
    return glUniformMatrix3x2fv(location, count, transpose, value);
}

__forceinline void APIENTRY glUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORMMATRIX2X4FV, glUniformMatrix2x4fv);
    return glUniformMatrix2x4fv(location, count, transpose, value);
}

__forceinline void APIENTRY glUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORMMATRIX4X2FV, glUniformMatrix4x2fv);
    return glUniformMatrix4x2fv(location, count, transpose, value);
}

__forceinline void APIENTRY glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORMMATRIX3X4FV, glUniformMatrix3x4fv);
    return glUniformMatrix3x4fv(location, count, transpose, value);
}

__forceinline void APIENTRY glUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GL_PROC_INIT(GLUNIFORMMATRIX4X3FV, glUniformMatrix4x3fv);
    return glUniformMatrix4x3fv(location, count, transpose, value);
}

__forceinline void APIENTRY glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    GL_PROC_INIT(GLUNIFORMBLOCKBINDING, glUniformBlockBinding);
    return glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
}

__forceinline GLboolean APIENTRY glUnmapBuffer(GLenum target)
{
    GL_PROC_INIT(GLUNMAPBUFFER, glUnmapBuffer);
    return glUnmapBuffer(target);
}

__forceinline GLboolean APIENTRY glUnmapNamedBufferEXT(GLenum target)
{
    GL_PROC_INIT(GLUNMAPNAMEDBUFFEREXT, glUnmapNamedBufferEXT);
    return glUnmapNamedBufferEXT(target);
}

__forceinline void APIENTRY glUseProgram(GLuint program)
{
    GL_PROC_INIT(GLUSEPROGRAM, glUseProgram);
    return glUseProgram(program);
}

// glV ---------------------------------------------------------------------------------
__forceinline void APIENTRY glValidateProgram(GLuint program)
{
    GL_PROC_INIT(GLVALIDATEPROGRAM, glValidateProgram);
    return glValidateProgram(program);
}

__forceinline void APIENTRY glVertexAttrib1f(GLuint index, GLfloat x)
{
    GL_PROC_INIT(GLVERTEXATTRIB1F, glVertexAttrib1f);
    return glVertexAttrib1f(index, x);
}

__forceinline void APIENTRY glVertexAttrib1s(GLuint index, GLshort x)
{
    GL_PROC_INIT(GLVERTEXATTRIB1S, glVertexAttrib1s);
    return glVertexAttrib1s(index, x);
}

__forceinline void APIENTRY glVertexAttrib1d(GLuint index, GLdouble x)
{
    GL_PROC_INIT(GLVERTEXATTRIB1D, glVertexAttrib1d);
    return glVertexAttrib1d(index, x);
}

__forceinline void APIENTRY glVertexAttribI1i(GLuint index, GLint x)
{
    GL_PROC_INIT(GLVERTEXATTRIBI1I, glVertexAttribI1i);
    return glVertexAttribI1i(index, x);
}

__forceinline void APIENTRY glVertexAttribI1ui(GLuint index, GLuint x)
{
    GL_PROC_INIT(GLVERTEXATTRIBI1UI, glVertexAttribI1ui);
    return glVertexAttribI1ui(index, x);
}

__forceinline void APIENTRY glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y)
{
    GL_PROC_INIT(GLVERTEXATTRIB2F, glVertexAttrib2f);
    return glVertexAttrib2f(index, x, y);
}

__forceinline void APIENTRY glVertexAttrib2s(GLuint index, GLshort x, GLshort y)
{
    GL_PROC_INIT(GLVERTEXATTRIB2S, glVertexAttrib2s);
    return glVertexAttrib2s(index, x, y);
}

__forceinline void APIENTRY glVertexAttrib2d(GLuint index, GLdouble x, GLdouble y)
{
    GL_PROC_INIT(GLVERTEXATTRIB2D, glVertexAttrib2d);
    return glVertexAttrib2d(index, x, y);
}

__forceinline void APIENTRY glVertexAttribI2i(GLuint index, GLint x, GLint y)
{
    GL_PROC_INIT(GLVERTEXATTRIBI2I, glVertexAttribI2i);
    return glVertexAttribI2i(index, x, y);
}

__forceinline void APIENTRY glVertexAttribI2ui(GLuint index, GLuint x, GLuint y)
{
    GL_PROC_INIT(GLVERTEXATTRIBI2UI, glVertexAttribI2ui);
    return glVertexAttribI2ui(index, x, y);
}

__forceinline void APIENTRY glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    GL_PROC_INIT(GLVERTEXATTRIB3F, glVertexAttrib3f);
    return glVertexAttrib3f(index, x, y, z);
}

__forceinline void APIENTRY glVertexAttrib3s(GLuint index, GLshort x, GLshort y, GLshort z)
{
    GL_PROC_INIT(GLVERTEXATTRIB3S, glVertexAttrib3s);
    return glVertexAttrib3s(index, x, y, z);
}

__forceinline void APIENTRY glVertexAttrib3d(GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    GL_PROC_INIT(GLVERTEXATTRIB3D, glVertexAttrib3d);
    return glVertexAttrib3d(index, x, y, z);
}

__forceinline void APIENTRY glVertexAttribI3i(GLuint index, GLint x, GLint y, GLint z)
{
    GL_PROC_INIT(GLVERTEXATTRIBI3I, glVertexAttribI3i);
    return glVertexAttribI3i(index, x, y, z);
}

__forceinline void APIENTRY glVertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z)
{
    GL_PROC_INIT(GLVERTEXATTRIBI3UI, glVertexAttribI3ui);
    return glVertexAttribI3ui(index, x, y, z);
}

__forceinline void APIENTRY glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GL_PROC_INIT(GLVERTEXATTRIB4F, glVertexAttrib4f);
    return glVertexAttrib4f(index, x, y, z, w);
}

__forceinline void APIENTRY glVertexAttrib4s(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    GL_PROC_INIT(GLVERTEXATTRIB4S, glVertexAttrib4s);
    return glVertexAttrib4s(index, x, y, z, w);
}

__forceinline void APIENTRY glVertexAttrib4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GL_PROC_INIT(GLVERTEXATTRIB4D, glVertexAttrib4d);
    return glVertexAttrib4d(index, x, y, z, w);
}

__forceinline void APIENTRY glVertexAttrib4Nub(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    GL_PROC_INIT(GLVERTEXATTRIB4NUB, glVertexAttrib4Nub);
    return glVertexAttrib4Nub(index, x, y, z, w);
}

__forceinline void APIENTRY glVertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    GL_PROC_INIT(GLVERTEXATTRIBI4I, glVertexAttribI4i);
    return glVertexAttribI4i(index, x, y, z, w);
}

__forceinline void APIENTRY glVertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    GL_PROC_INIT(GLVERTEXATTRIBI4UI, glVertexAttribI4ui);
    return glVertexAttribI4ui(index, x, y, z, w);
}

__forceinline void APIENTRY glVertexAttrib1fv(GLuint index, const GLfloat *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB1FV, glVertexAttrib1fv);
    return glVertexAttrib1fv(index, v);
}

__forceinline void APIENTRY glVertexAttrib1sv(GLuint index, const GLshort *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB1SV, glVertexAttrib1sv);
    return glVertexAttrib1sv(index, v);
}

__forceinline void APIENTRY glVertexAttrib1dv(GLuint index, const GLdouble *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB1DV, glVertexAttrib1dv);
    return glVertexAttrib1dv(index, v);
}

__forceinline void APIENTRY glVertexAttribI1iv(GLuint index, const GLint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI1IV, glVertexAttribI1iv);
    return glVertexAttribI1iv(index, v);
}

__forceinline void APIENTRY glVertexAttribI1uiv(GLuint index, const GLuint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI1UIV, glVertexAttribI1uiv);
    return glVertexAttribI1uiv(index, v);
}

__forceinline void APIENTRY glVertexAttrib2fv(GLuint index, const GLfloat *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB2FV, glVertexAttrib2fv);
    return glVertexAttrib2fv(index, v);
}

__forceinline void APIENTRY glVertexAttrib2sv(GLuint index, const GLshort *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB2SV, glVertexAttrib2sv);
    return glVertexAttrib2sv(index, v);
}

__forceinline void APIENTRY glVertexAttrib2dv(GLuint index, const GLdouble *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB2DV, glVertexAttrib2dv);
    return glVertexAttrib2dv(index, v);
}

__forceinline void APIENTRY glVertexAttribI2iv(GLuint index, const GLint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI2IV, glVertexAttribI2iv);
    return glVertexAttribI2iv(index, v);
}

__forceinline void APIENTRY glVertexAttribI2uiv(GLuint index, const GLuint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI2UIV, glVertexAttribI2uiv);
    return glVertexAttribI2uiv(index, v);
}

__forceinline void APIENTRY glVertexAttrib3fv(GLuint index, const GLfloat *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB3FV, glVertexAttrib3fv);
    return glVertexAttrib3fv(index, v);
}

__forceinline void APIENTRY glVertexAttrib3sv(GLuint index, const GLshort *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB3SV, glVertexAttrib3sv);
    return glVertexAttrib3sv(index, v);
}

__forceinline void APIENTRY glVertexAttrib3dv(GLuint index, const GLdouble *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB3DV, glVertexAttrib3dv);
    return glVertexAttrib3dv(index, v);
}

__forceinline void APIENTRY glVertexAttribI3iv(GLuint index, const GLint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI3IV, glVertexAttribI3iv);
    return glVertexAttribI3iv(index, v);
}

__forceinline void APIENTRY glVertexAttribI3uiv(GLuint index, const GLuint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI3UIV, glVertexAttribI3uiv);
    return glVertexAttribI3uiv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4fv(GLuint index, const GLfloat *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4FV, glVertexAttrib4fv);
    return glVertexAttrib4fv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4sv(GLuint index, const GLshort *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4SV, glVertexAttrib4sv);
    return glVertexAttrib4sv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4dv(GLuint index, const GLdouble *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4DV, glVertexAttrib4dv);
    return glVertexAttrib4dv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4iv(GLuint index, const GLint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4IV, glVertexAttrib4iv);
    return glVertexAttrib4iv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4bv(GLuint index, const GLbyte *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4BV, glVertexAttrib4bv);
    return glVertexAttrib4bv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4ubv(GLuint index, const GLubyte *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4UBV, glVertexAttrib4ubv);
    return glVertexAttrib4ubv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4usv(GLuint index, const GLushort *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4USV, glVertexAttrib4usv);
    return glVertexAttrib4usv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4uiv(GLuint index, const GLuint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4UIV, glVertexAttrib4uiv);
    return glVertexAttrib4uiv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4Nbv(GLuint index, const GLbyte *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4NBV, glVertexAttrib4Nbv);
    return glVertexAttrib4Nbv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4Nsv(GLuint index, const GLshort *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4NSV, glVertexAttrib4Nsv);
    return glVertexAttrib4Nsv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4Niv(GLuint index, const GLint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4NIV, glVertexAttrib4Niv);
    return glVertexAttrib4Niv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4Nubv(GLuint index, const GLubyte *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4NUBV, glVertexAttrib4Nubv);
    return glVertexAttrib4Nubv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4Nusv(GLuint index, const GLushort *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4NUSV, glVertexAttrib4Nusv);
    return glVertexAttrib4Nusv(index, v);
}

__forceinline void APIENTRY glVertexAttrib4Nuiv(GLuint index, const GLuint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIB4NUIV, glVertexAttrib4Nuiv);
    return glVertexAttrib4Nuiv(index, v);
}

__forceinline void APIENTRY glVertexAttribI4bv(GLuint index, const GLbyte *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI4BV, glVertexAttribI4bv);
    return glVertexAttribI4bv(index, v);
}

__forceinline void APIENTRY glVertexAttribI4ubv(GLuint index, const GLubyte *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI4UBV, glVertexAttribI4ubv);
    return glVertexAttribI4ubv(index, v);
}

__forceinline void APIENTRY glVertexAttribI4sv(GLuint index, const GLshort *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI4SV, glVertexAttribI4sv);
    return glVertexAttribI4sv(index, v);
}

__forceinline void APIENTRY glVertexAttribI4usv(GLuint index, const GLushort *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI4USV, glVertexAttribI4usv);
    return glVertexAttribI4usv(index, v);
}

__forceinline void APIENTRY glVertexAttribI4iv(GLuint index, const GLint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI4IV, glVertexAttribI4iv);
    return glVertexAttribI4iv(index, v);
}

__forceinline void APIENTRY glVertexAttribI4uiv(GLuint index, const GLuint *v)
{
    GL_PROC_INIT(GLVERTEXATTRIBI4UIV, glVertexAttribI4uiv);
    return glVertexAttribI4uiv(index, v);
}

__forceinline void APIENTRY glVertexAttribP1ui(GLuint index, GLenum type, GLboolean normalized, GLuint value)
{
    GL_PROC_INIT(GLVERTEXATTRIBP1UI, glVertexAttribP1ui);
    return glVertexAttribP1ui(index, type, normalized, value);
}

__forceinline void APIENTRY glVertexAttribP2ui(GLuint index, GLenum type, GLboolean normalized, GLuint value)
{
    GL_PROC_INIT(GLVERTEXATTRIBP2UI, glVertexAttribP2ui);
    return glVertexAttribP2ui(index, type, normalized, value);
}

__forceinline void APIENTRY glVertexAttribP3ui(GLuint index, GLenum type, GLboolean normalized, GLuint value)
{
    GL_PROC_INIT(GLVERTEXATTRIBP3UI, glVertexAttribP3ui);
    return glVertexAttribP3ui(index, type, normalized, value);
}

__forceinline void APIENTRY glVertexAttribP4ui(GLuint index, GLenum type, GLboolean normalized, GLuint value)
{
    GL_PROC_INIT(GLVERTEXATTRIBP4UI, glVertexAttribP4ui);
    return glVertexAttribP4ui(index, type, normalized, value);
}

__forceinline void APIENTRY glVertexAttribDivisor(GLuint index, GLuint divisor)
{
    GL_PROC_INIT(GLVERTEXATTRIBDIVISOR, glVertexAttribDivisor);
    return glVertexAttribDivisor(index, divisor);
}

__forceinline void APIENTRY glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
    GL_PROC_INIT(GLVERTEXATTRIBPOINTER, glVertexAttribPointer);
    return glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

// glW ---------------------------------------------------------------------------------
__forceinline void APIENTRY glWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    GL_PROC_INIT(GLWAITSYNC, glWaitSync);
    return glWaitSync(sync, flags, timeout);
}

// wglC ---------------------------------------------------------------------------------
BOOL APIENTRY wglChoosePixelFormatARB(HDC hDC, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats)
{
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    wglChoosePixelFormatARB = 
        (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

    if(wglChoosePixelFormatARB == nullptr)
        isle::Book::AddEvent(isle::eNOTE::nALERT, "can't get \"wglChoosePixelFormatARB()\" procedure address.");

    return wglChoosePixelFormatARB(hDC, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats);
}

HGLRC APIENTRY wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList)
{
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    wglCreateContextAttribsARB = 
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if(wglCreateContextAttribsARB == nullptr)
        isle::Book::AddEvent(isle::eNOTE::nALERT, "can't get \"wglCreateContextAttribsARB( )\" procedure address.");

    return wglCreateContextAttribsARB(hDC, hShareContext, attribList);
}

// wglG ---------------------------------------------------------------------------------
int WINAPI wglGetSwapIntervalEXT(void)
{
    PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = nullptr;
    wglGetSwapIntervalEXT = 
        (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

    if(wglGetSwapIntervalEXT == nullptr)
        isle::Book::AddEvent(isle::eNOTE::nALERT, "can't get \"wglGetSwapIntervalEXT()\" procedure address.");

    return wglGetSwapIntervalEXT();
}

// wglS ---------------------------------------------------------------------------------
BOOL WINAPI wglSwapIntervalEXT(int interval)
{
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
    wglSwapIntervalEXT = 
        (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

    if(wglSwapIntervalEXT == nullptr)
        isle::Book::AddEvent(isle::eNOTE::nALERT, "can't get \"wglSwapIntervalEXT()\" procedure address.");

    return wglSwapIntervalEXT(interval);
}

#undef GL_PROC_INIT
#undef GL_PROC
#undef GL_PROC_PREFIX