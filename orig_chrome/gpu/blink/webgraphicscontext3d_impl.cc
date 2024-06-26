// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/blink/webgraphicscontext3d_impl.h"

#include "base/atomicops.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "gpu/command_buffer/client/gles2_implementation.h"
#include "gpu/command_buffer/client/gles2_lib.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/common/sync_token.h"

#include "third_party/khronos/GLES2/gl2.h"
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include "third_party/khronos/GLES2/gl2ext.h"

using blink::WebGLId;
using blink::WGC3Dbitfield;
using blink::WGC3Dboolean;
using blink::WGC3Dbyte;
using blink::WGC3Dchar;
using blink::WGC3Dclampf;
using blink::WGC3Denum;
using blink::WGC3Dfloat;
using blink::WGC3Dint;
using blink::WGC3Dint64;
using blink::WGC3Dintptr;
using blink::WGC3Dsizei;
using blink::WGC3Dsizeiptr;
using blink::WGC3Dsync;
using blink::WGC3Duint;
using blink::WGC3Duint64;

namespace gpu_blink {

namespace {

    uint32_t GenFlushID()
    {
        static base::subtle::Atomic32 flush_id = 0;

        base::subtle::Atomic32 my_id = base::subtle::Barrier_AtomicIncrement(
            &flush_id, 1);
        return static_cast<uint32_t>(my_id);
    }

} // namespace anonymous

class WebGraphicsContext3DErrorMessageCallback
    : public ::gpu::gles2::GLES2ImplementationErrorMessageCallback {
public:
    WebGraphicsContext3DErrorMessageCallback(
        WebGraphicsContext3DImpl* context)
        : graphics_context_(context)
    {
    }

    void OnErrorMessage(const char* msg, int id) override;

private:
    WebGraphicsContext3DImpl* graphics_context_;

    DISALLOW_COPY_AND_ASSIGN(WebGraphicsContext3DErrorMessageCallback);
};

void WebGraphicsContext3DErrorMessageCallback::OnErrorMessage(
    const char* msg, int id)
{
    graphics_context_->OnErrorMessage(msg, id);
}

// Helper macros to reduce the amount of code.

#define DELEGATE_TO_GL(name, glname)      \
    void WebGraphicsContext3DImpl::name() \
    {                                     \
        gl_->glname();                    \
    }

#define DELEGATE_TO_GL_R(name, glname, rt) \
    rt WebGraphicsContext3DImpl::name()    \
    {                                      \
        return gl_->glname();              \
    }

#define DELEGATE_TO_GL_1(name, glname, t1)     \
    void WebGraphicsContext3DImpl::name(t1 a1) \
    {                                          \
        gl_->glname(a1);                       \
    }

#define DELEGATE_TO_GL_1R(name, glname, t1, rt) \
    rt WebGraphicsContext3DImpl::name(t1 a1)    \
    {                                           \
        return gl_->glname(a1);                 \
    }

#define DELEGATE_TO_GL_1RB(name, glname, t1, rt) \
    rt WebGraphicsContext3DImpl::name(t1 a1)     \
    {                                            \
        return gl_->glname(a1) ? true : false;   \
    }

#define DELEGATE_TO_GL_2(name, glname, t1, t2)        \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2) \
    {                                                 \
        gl_->glname(a1, a2);                          \
    }

#define DELEGATE_TO_GL_2R(name, glname, t1, t2, rt) \
    rt WebGraphicsContext3DImpl::name(t1 a1, t2 a2) \
    {                                               \
        return gl_->glname(a1, a2);                 \
    }

#define DELEGATE_TO_GL_3(name, glname, t1, t2, t3)           \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3) \
    {                                                        \
        gl_->glname(a1, a2, a3);                             \
    }

#define DELEGATE_TO_GL_3R(name, glname, t1, t2, t3, rt)    \
    rt WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3) \
    {                                                      \
        return gl_->glname(a1, a2, a3);                    \
    }

#define DELEGATE_TO_GL_4(name, glname, t1, t2, t3, t4)              \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4) \
    {                                                               \
        gl_->glname(a1, a2, a3, a4);                                \
    }

#define DELEGATE_TO_GL_4R(name, glname, t1, t2, t3, t4, rt)       \
    rt WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4) \
    {                                                             \
        return gl_->glname(a1, a2, a3, a4);                       \
    }

#define DELEGATE_TO_GL_5(name, glname, t1, t2, t3, t4, t5)                 \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5) \
    {                                                                      \
                                                                           \
        gl_->glname(a1, a2, a3, a4, a5);                                   \
    }

#define DELEGATE_TO_GL_6(name, glname, t1, t2, t3, t4, t5, t6)             \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, \
        t6 a6)                                                             \
    {                                                                      \
        gl_->glname(a1, a2, a3, a4, a5, a6);                               \
    }

#define DELEGATE_TO_GL_7(name, glname, t1, t2, t3, t4, t5, t6, t7)         \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, \
        t6 a6, t7 a7)                                                      \
    {                                                                      \
        gl_->glname(a1, a2, a3, a4, a5, a6, a7);                           \
    }

#define DELEGATE_TO_GL_8(name, glname, t1, t2, t3, t4, t5, t6, t7, t8)     \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, \
        t6 a6, t7 a7, t8 a8)                                               \
    {                                                                      \
        gl_->glname(a1, a2, a3, a4, a5, a6, a7, a8);                       \
    }

#define DELEGATE_TO_GL_9(name, glname, t1, t2, t3, t4, t5, t6, t7, t8, t9) \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, \
        t6 a6, t7 a7, t8 a8, t9 a9)                                        \
    {                                                                      \
        gl_->glname(a1, a2, a3, a4, a5, a6, a7, a8, a9);                   \
    }

#define DELEGATE_TO_GL_9R(name, glname, t1, t2, t3, t4, t5, t6, t7, t8,  \
    t9, rt)                                                              \
    rt WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, \
        t6 a6, t7 a7, t8 a8, t9 a9)                                      \
    {                                                                    \
        return gl_->glname(a1, a2, a3, a4, a5, a6, a7, a8, a9);          \
    }

#define DELEGATE_TO_GL_10(name, glname, t1, t2, t3, t4, t5, t6, t7, t8,    \
    t9, t10)                                                               \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, \
        t6 a6, t7 a7, t8 a8, t9 a9,                                        \
        t10 a10)                                                           \
    {                                                                      \
        gl_->glname(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);              \
    }

#define DELEGATE_TO_GL_11(name, glname, t1, t2, t3, t4, t5, t6, t7, t8,    \
    t9, t10, t11)                                                          \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, \
        t6 a6, t7 a7, t8 a8, t9 a9, t10 a10,                               \
        t11 a11)                                                           \
    {                                                                      \
        gl_->glname(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);         \
    }

#define DELEGATE_TO_GL_12(name, glname, t1, t2, t3, t4, t5, t6, t7, t8,    \
    t9, t10, t11, t12)                                                     \
    void WebGraphicsContext3DImpl::name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, \
        t6 a6, t7 a7, t8 a8, t9 a9, t10 a10,                               \
        t11 a11, t12 a12)                                                  \
    {                                                                      \
        gl_->glname(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);    \
    }

WebGraphicsContext3DImpl::WebGraphicsContext3DImpl()
    : initialized_(false)
    , initialize_failed_(false)
    , context_lost_callback_(0)
    , error_message_callback_(0)
    , gl_(NULL)
    , flush_id_(0)
{
}

WebGraphicsContext3DImpl::~WebGraphicsContext3DImpl()
{
}

void WebGraphicsContext3DImpl::synthesizeGLError(WGC3Denum error)
{
    if (std::find(synthetic_errors_.begin(), synthetic_errors_.end(), error) == synthetic_errors_.end()) {
        synthetic_errors_.push_back(error);
    }
}

uint32_t WebGraphicsContext3DImpl::lastFlushID()
{
    return flush_id_;
}

bool WebGraphicsContext3DImpl::insertSyncPoint(WGC3Dbyte* sync_token)
{
    const uint32_t sync_point = gl_->InsertSyncPointCHROMIUM();
    if (!sync_point)
        return false;

    gpu::SyncToken sync_token_data(sync_point);
    memcpy(sync_token, &sync_token_data, sizeof(sync_token_data));
    return true;
}

DELEGATE_TO_GL_3(reshapeWithScaleFactor, ResizeCHROMIUM, int, int, float)

DELEGATE_TO_GL_4R(mapBufferSubDataCHROMIUM, MapBufferSubDataCHROMIUM, WGC3Denum,
    WGC3Dintptr, WGC3Dsizeiptr, WGC3Denum, void*)

DELEGATE_TO_GL_1(unmapBufferSubDataCHROMIUM, UnmapBufferSubDataCHROMIUM,
    const void*)

DELEGATE_TO_GL_9R(mapTexSubImage2DCHROMIUM, MapTexSubImage2DCHROMIUM, WGC3Denum,
    WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dsizei, WGC3Dsizei,
    WGC3Denum, WGC3Denum, WGC3Denum, void*)

DELEGATE_TO_GL_1(unmapTexSubImage2DCHROMIUM, UnmapTexSubImage2DCHROMIUM,
    const void*)

DELEGATE_TO_GL_3(discardFramebufferEXT, DiscardFramebufferEXT, WGC3Denum,
    WGC3Dsizei, const WGC3Denum*)

blink::WebString WebGraphicsContext3DImpl::
    getRequestableExtensionsCHROMIUM()
{
    return blink::WebString::fromUTF8(
        gl_->GetRequestableExtensionsCHROMIUM());
}

DELEGATE_TO_GL_1(requestExtensionCHROMIUM, RequestExtensionCHROMIUM,
    const char*)

void WebGraphicsContext3DImpl::blitFramebufferCHROMIUM(
    WGC3Dint srcX0, WGC3Dint srcY0, WGC3Dint srcX1, WGC3Dint srcY1,
    WGC3Dint dstX0, WGC3Dint dstY0, WGC3Dint dstX1, WGC3Dint dstY1,
    WGC3Dbitfield mask, WGC3Denum filter)
{
    gl_->BlitFramebufferCHROMIUM(
        srcX0, srcY0, srcX1, srcY1,
        dstX0, dstY0, dstX1, dstY1,
        mask, filter);
}

DELEGATE_TO_GL_5(renderbufferStorageMultisampleCHROMIUM,
    RenderbufferStorageMultisampleCHROMIUM, WGC3Denum, WGC3Dsizei,
    WGC3Denum, WGC3Dsizei, WGC3Dsizei)

DELEGATE_TO_GL_1(activeTexture, ActiveTexture, WGC3Denum)

DELEGATE_TO_GL(applyScreenSpaceAntialiasingCHROMIUM,
    ApplyScreenSpaceAntialiasingCHROMIUM);

DELEGATE_TO_GL_2(attachShader, AttachShader, WebGLId, WebGLId)

DELEGATE_TO_GL_3(bindAttribLocation, BindAttribLocation, WebGLId,
    WGC3Duint, const WGC3Dchar*)

DELEGATE_TO_GL_2(bindBuffer, BindBuffer, WGC3Denum, WebGLId)

DELEGATE_TO_GL_2(bindFramebuffer, BindFramebuffer, WGC3Denum, WebGLId)

DELEGATE_TO_GL_2(bindRenderbuffer, BindRenderbuffer, WGC3Denum, WebGLId)

DELEGATE_TO_GL_2(bindTexture, BindTexture, WGC3Denum, WebGLId)

DELEGATE_TO_GL_4(blendColor, BlendColor,
    WGC3Dclampf, WGC3Dclampf, WGC3Dclampf, WGC3Dclampf)

DELEGATE_TO_GL_1(blendEquation, BlendEquation, WGC3Denum)

DELEGATE_TO_GL_2(blendEquationSeparate, BlendEquationSeparate,
    WGC3Denum, WGC3Denum)

DELEGATE_TO_GL_2(blendFunc, BlendFunc, WGC3Denum, WGC3Denum)

DELEGATE_TO_GL_4(blendFuncSeparate, BlendFuncSeparate,
    WGC3Denum, WGC3Denum, WGC3Denum, WGC3Denum)

DELEGATE_TO_GL_4(bufferData, BufferData,
    WGC3Denum, WGC3Dsizeiptr, const void*, WGC3Denum)

DELEGATE_TO_GL_4(bufferSubData, BufferSubData,
    WGC3Denum, WGC3Dintptr, WGC3Dsizeiptr, const void*)

DELEGATE_TO_GL_1R(checkFramebufferStatus, CheckFramebufferStatus,
    WGC3Denum, WGC3Denum)

DELEGATE_TO_GL_1(clear, Clear, WGC3Dbitfield)

DELEGATE_TO_GL_4(clearColor, ClearColor,
    WGC3Dclampf, WGC3Dclampf, WGC3Dclampf, WGC3Dclampf)

DELEGATE_TO_GL_1(clearDepth, ClearDepthf, WGC3Dclampf)

DELEGATE_TO_GL_1(clearStencil, ClearStencil, WGC3Dint)

DELEGATE_TO_GL_4(colorMask, ColorMask,
    WGC3Dboolean, WGC3Dboolean, WGC3Dboolean, WGC3Dboolean)

DELEGATE_TO_GL_1(compileShader, CompileShader, WebGLId)

DELEGATE_TO_GL_8(compressedTexImage2D, CompressedTexImage2D,
    WGC3Denum, WGC3Dint, WGC3Denum, WGC3Dint, WGC3Dint,
    WGC3Dsizei, WGC3Dsizei, const void*)

DELEGATE_TO_GL_9(compressedTexSubImage2D, CompressedTexSubImage2D,
    WGC3Denum, WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dint,
    WGC3Denum, WGC3Dsizei, const void*)

DELEGATE_TO_GL_8(copyTexImage2D, CopyTexImage2D,
    WGC3Denum, WGC3Dint, WGC3Denum, WGC3Dint, WGC3Dint,
    WGC3Dsizei, WGC3Dsizei, WGC3Dint)

DELEGATE_TO_GL_8(copyTexSubImage2D, CopyTexSubImage2D,
    WGC3Denum, WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dint,
    WGC3Dsizei, WGC3Dsizei)

DELEGATE_TO_GL_1(cullFace, CullFace, WGC3Denum)

DELEGATE_TO_GL_1(depthFunc, DepthFunc, WGC3Denum)

DELEGATE_TO_GL_1(depthMask, DepthMask, WGC3Dboolean)

DELEGATE_TO_GL_2(depthRange, DepthRangef, WGC3Dclampf, WGC3Dclampf)

DELEGATE_TO_GL_2(detachShader, DetachShader, WebGLId, WebGLId)

DELEGATE_TO_GL_1(disable, Disable, WGC3Denum)

DELEGATE_TO_GL_1(disableVertexAttribArray, DisableVertexAttribArray,
    WGC3Duint)

DELEGATE_TO_GL_3(drawArrays, DrawArrays, WGC3Denum, WGC3Dint, WGC3Dsizei)

void WebGraphicsContext3DImpl::drawElements(WGC3Denum mode,
    WGC3Dsizei count,
    WGC3Denum type,
    WGC3Dintptr offset)
{
    gl_->DrawElements(
        mode, count, type,
        reinterpret_cast<void*>(static_cast<intptr_t>(offset)));
}

DELEGATE_TO_GL_1(enable, Enable, WGC3Denum)

DELEGATE_TO_GL_1(enableVertexAttribArray, EnableVertexAttribArray,
    WGC3Duint)

void WebGraphicsContext3DImpl::finish()
{
    flush_id_ = GenFlushID();
    gl_->Finish();
}

void WebGraphicsContext3DImpl::flush()
{
    flush_id_ = GenFlushID();
    gl_->Flush();
}

DELEGATE_TO_GL_4(framebufferRenderbuffer, FramebufferRenderbuffer,
    WGC3Denum, WGC3Denum, WGC3Denum, WebGLId)

DELEGATE_TO_GL_5(framebufferTexture2D, FramebufferTexture2D,
    WGC3Denum, WGC3Denum, WGC3Denum, WebGLId, WGC3Dint)

DELEGATE_TO_GL_1(frontFace, FrontFace, WGC3Denum)

DELEGATE_TO_GL_1(generateMipmap, GenerateMipmap, WGC3Denum)

bool WebGraphicsContext3DImpl::getActiveAttrib(
    WebGLId program, WGC3Duint index, ActiveInfo& info)
{
    if (!program) {
        synthesizeGLError(GL_INVALID_VALUE);
        return false;
    }
    GLint max_name_length = -1;
    gl_->GetProgramiv(
        program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_length);
    if (max_name_length < 0)
        return false;
    if (max_name_length == 0) {
        // No active attributes exist.
        synthesizeGLError(GL_INVALID_VALUE);
        return false;
    }
    scoped_ptr<GLchar[]> name(new GLchar[max_name_length]);
    GLsizei length = 0;
    GLint size = -1;
    GLenum type = 0;
    gl_->GetActiveAttrib(
        program, index, max_name_length, &length, &size, &type, name.get());
    if (size < 0) {
        return false;
    }
    info.name = blink::WebString::fromUTF8(name.get(), length);
    info.type = type;
    info.size = size;
    return true;
}

bool WebGraphicsContext3DImpl::getActiveUniform(
    WebGLId program, WGC3Duint index, ActiveInfo& info)
{
    GLint max_name_length = -1;
    gl_->GetProgramiv(
        program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_length);
    if (max_name_length < 0)
        return false;
    if (max_name_length == 0) {
        // No active uniforms exist.
        synthesizeGLError(GL_INVALID_VALUE);
        return false;
    }
    scoped_ptr<GLchar[]> name(new GLchar[max_name_length]);
    GLsizei length = 0;
    GLint size = -1;
    GLenum type = 0;
    gl_->GetActiveUniform(
        program, index, max_name_length, &length, &size, &type, name.get());
    if (size < 0) {
        return false;
    }
    info.name = blink::WebString::fromUTF8(name.get(), length);
    info.type = type;
    info.size = size;
    return true;
}

DELEGATE_TO_GL_4(getAttachedShaders, GetAttachedShaders,
    WebGLId, WGC3Dsizei, WGC3Dsizei*, WebGLId*)

DELEGATE_TO_GL_2R(getAttribLocation, GetAttribLocation,
    WebGLId, const WGC3Dchar*, WGC3Dint)

DELEGATE_TO_GL_2(getBooleanv, GetBooleanv, WGC3Denum, WGC3Dboolean*)

DELEGATE_TO_GL_3(getBufferParameteriv, GetBufferParameteriv,
    WGC3Denum, WGC3Denum, WGC3Dint*)

WGC3Denum WebGraphicsContext3DImpl::getError()
{
    if (!synthetic_errors_.empty()) {
        std::vector<WGC3Denum>::iterator iter = synthetic_errors_.begin();
        WGC3Denum err = *iter;
        synthetic_errors_.erase(iter);
        return err;
    }

    return gl_->GetError();
}

DELEGATE_TO_GL_2(getFloatv, GetFloatv, WGC3Denum, WGC3Dfloat*)

DELEGATE_TO_GL_4(getFramebufferAttachmentParameteriv,
    GetFramebufferAttachmentParameteriv,
    WGC3Denum, WGC3Denum, WGC3Denum, WGC3Dint*)

DELEGATE_TO_GL_2(getIntegerv, GetIntegerv, WGC3Denum, WGC3Dint*)

DELEGATE_TO_GL_2(getInteger64v, GetInteger64v, WGC3Denum, WGC3Dint64*)

DELEGATE_TO_GL_3(getIntegeri_v, GetIntegeri_v, WGC3Denum, WGC3Duint, WGC3Dint*)

DELEGATE_TO_GL_3(getInteger64i_v, GetInteger64i_v,
    WGC3Denum, WGC3Duint, WGC3Dint64*)

DELEGATE_TO_GL_3(getProgramiv, GetProgramiv, WebGLId, WGC3Denum, WGC3Dint*)

blink::WebString WebGraphicsContext3DImpl::getProgramInfoLog(
    WebGLId program)
{
    GLint logLength = 0;
    gl_->GetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (!logLength)
        return blink::WebString();
    scoped_ptr<GLchar[]> log(new GLchar[logLength]);
    if (!log)
        return blink::WebString();
    GLsizei returnedLogLength = 0;
    gl_->GetProgramInfoLog(
        program, logLength, &returnedLogLength, log.get());
    DCHECK_EQ(logLength, returnedLogLength + 1);
    blink::WebString res = blink::WebString::fromUTF8(log.get(), returnedLogLength);
    return res;
}

DELEGATE_TO_GL_3(getRenderbufferParameteriv, GetRenderbufferParameteriv,
    WGC3Denum, WGC3Denum, WGC3Dint*)

DELEGATE_TO_GL_3(getShaderiv, GetShaderiv, WebGLId, WGC3Denum, WGC3Dint*)

blink::WebString WebGraphicsContext3DImpl::getShaderInfoLog(
    WebGLId shader)
{
    GLint logLength = 0;
    gl_->GetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (!logLength)
        return blink::WebString();
    scoped_ptr<GLchar[]> log(new GLchar[logLength]);
    if (!log)
        return blink::WebString();
    GLsizei returnedLogLength = 0;
    gl_->GetShaderInfoLog(
        shader, logLength, &returnedLogLength, log.get());
    DCHECK_EQ(logLength, returnedLogLength + 1);
    blink::WebString res = blink::WebString::fromUTF8(log.get(), returnedLogLength);
    return res;
}

DELEGATE_TO_GL_4(getShaderPrecisionFormat, GetShaderPrecisionFormat,
    WGC3Denum, WGC3Denum, WGC3Dint*, WGC3Dint*)

blink::WebString WebGraphicsContext3DImpl::getShaderSource(
    WebGLId shader)
{
    GLint logLength = 0;
    gl_->GetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &logLength);
    if (!logLength)
        return blink::WebString();
    scoped_ptr<GLchar[]> log(new GLchar[logLength]);
    if (!log)
        return blink::WebString();
    GLsizei returnedLogLength = 0;
    gl_->GetShaderSource(
        shader, logLength, &returnedLogLength, log.get());
    if (!returnedLogLength)
        return blink::WebString();
    DCHECK_EQ(logLength, returnedLogLength + 1);
    blink::WebString res = blink::WebString::fromUTF8(log.get(), returnedLogLength);
    return res;
}

blink::WebString WebGraphicsContext3DImpl::
    getTranslatedShaderSourceANGLE(WebGLId shader)
{
    GLint logLength = 0;
    gl_->GetShaderiv(
        shader, GL_TRANSLATED_SHADER_SOURCE_LENGTH_ANGLE, &logLength);
    if (!logLength)
        return blink::WebString();
    scoped_ptr<GLchar[]> log(new GLchar[logLength]);
    if (!log)
        return blink::WebString();
    GLsizei returnedLogLength = 0;
    gl_->GetTranslatedShaderSourceANGLE(
        shader, logLength, &returnedLogLength, log.get());
    if (!returnedLogLength)
        return blink::WebString();
    DCHECK_EQ(logLength, returnedLogLength + 1);
    blink::WebString res = blink::WebString::fromUTF8(log.get(), returnedLogLength);
    return res;
}

blink::WebString WebGraphicsContext3DImpl::getString(
    WGC3Denum name)
{
    return blink::WebString::fromUTF8(
        reinterpret_cast<const char*>(gl_->GetString(name)));
}

DELEGATE_TO_GL_3(getTexParameterfv, GetTexParameterfv,
    WGC3Denum, WGC3Denum, WGC3Dfloat*)

DELEGATE_TO_GL_3(getTexParameteriv, GetTexParameteriv,
    WGC3Denum, WGC3Denum, WGC3Dint*)

DELEGATE_TO_GL_3(getUniformfv, GetUniformfv, WebGLId, WGC3Dint, WGC3Dfloat*)

DELEGATE_TO_GL_3(getUniformiv, GetUniformiv, WebGLId, WGC3Dint, WGC3Dint*)

DELEGATE_TO_GL_2R(getUniformLocation, GetUniformLocation,
    WebGLId, const WGC3Dchar*, WGC3Dint)

DELEGATE_TO_GL_3(getVertexAttribfv, GetVertexAttribfv,
    WGC3Duint, WGC3Denum, WGC3Dfloat*)

DELEGATE_TO_GL_3(getVertexAttribiv, GetVertexAttribiv,
    WGC3Duint, WGC3Denum, WGC3Dint*)

WGC3Dsizeiptr WebGraphicsContext3DImpl::getVertexAttribOffset(
    WGC3Duint index, WGC3Denum pname)
{
    GLvoid* value = NULL;
    // NOTE: If pname is ever a value that returns more then 1 element
    // this will corrupt memory.
    gl_->GetVertexAttribPointerv(index, pname, &value);
    return static_cast<WGC3Dsizeiptr>(reinterpret_cast<intptr_t>(value));
}

DELEGATE_TO_GL_2(hint, Hint, WGC3Denum, WGC3Denum)

DELEGATE_TO_GL_1RB(isBuffer, IsBuffer, WebGLId, WGC3Dboolean)

DELEGATE_TO_GL_1RB(isEnabled, IsEnabled, WGC3Denum, WGC3Dboolean)

DELEGATE_TO_GL_1RB(isFramebuffer, IsFramebuffer, WebGLId, WGC3Dboolean)

DELEGATE_TO_GL_1RB(isProgram, IsProgram, WebGLId, WGC3Dboolean)

DELEGATE_TO_GL_1RB(isRenderbuffer, IsRenderbuffer, WebGLId, WGC3Dboolean)

DELEGATE_TO_GL_1RB(isShader, IsShader, WebGLId, WGC3Dboolean)

DELEGATE_TO_GL_1RB(isTexture, IsTexture, WebGLId, WGC3Dboolean)

DELEGATE_TO_GL_1(lineWidth, LineWidth, WGC3Dfloat)

DELEGATE_TO_GL_1(linkProgram, LinkProgram, WebGLId)

DELEGATE_TO_GL_2(pixelStorei, PixelStorei, WGC3Denum, WGC3Dint)

DELEGATE_TO_GL_2(polygonOffset, PolygonOffset, WGC3Dfloat, WGC3Dfloat)

DELEGATE_TO_GL_7(readPixels, ReadPixels,
    WGC3Dint, WGC3Dint, WGC3Dsizei, WGC3Dsizei, WGC3Denum,
    WGC3Denum, void*)

DELEGATE_TO_GL_4(renderbufferStorage, RenderbufferStorage,
    WGC3Denum, WGC3Denum, WGC3Dsizei, WGC3Dsizei)

DELEGATE_TO_GL_2(sampleCoverage, SampleCoverage, WGC3Dfloat, WGC3Dboolean)

DELEGATE_TO_GL_4(scissor, Scissor, WGC3Dint, WGC3Dint, WGC3Dsizei, WGC3Dsizei)

void WebGraphicsContext3DImpl::shaderSource(
    WebGLId shader, const WGC3Dchar* string)
{
    GLint length = strlen(string);
    gl_->ShaderSource(shader, 1, &string, &length);
}

DELEGATE_TO_GL_3(stencilFunc, StencilFunc, WGC3Denum, WGC3Dint, WGC3Duint)

DELEGATE_TO_GL_4(stencilFuncSeparate, StencilFuncSeparate,
    WGC3Denum, WGC3Denum, WGC3Dint, WGC3Duint)

DELEGATE_TO_GL_1(stencilMask, StencilMask, WGC3Duint)

DELEGATE_TO_GL_2(stencilMaskSeparate, StencilMaskSeparate,
    WGC3Denum, WGC3Duint)

DELEGATE_TO_GL_3(stencilOp, StencilOp,
    WGC3Denum, WGC3Denum, WGC3Denum)

DELEGATE_TO_GL_4(stencilOpSeparate, StencilOpSeparate,
    WGC3Denum, WGC3Denum, WGC3Denum, WGC3Denum)

DELEGATE_TO_GL_9(texImage2D, TexImage2D,
    WGC3Denum, WGC3Dint, WGC3Denum, WGC3Dsizei, WGC3Dsizei,
    WGC3Dint, WGC3Denum, WGC3Denum, const void*)

DELEGATE_TO_GL_3(texParameterf, TexParameterf,
    WGC3Denum, WGC3Denum, WGC3Dfloat);

DELEGATE_TO_GL_3(texParameteri, TexParameteri,
    WGC3Denum, WGC3Denum, WGC3Dint);

DELEGATE_TO_GL_9(texSubImage2D, TexSubImage2D,
    WGC3Denum, WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dsizei,
    WGC3Dsizei, WGC3Denum, WGC3Denum, const void*)

DELEGATE_TO_GL_2(uniform1f, Uniform1f, WGC3Dint, WGC3Dfloat)

DELEGATE_TO_GL_3(uniform1fv, Uniform1fv, WGC3Dint, WGC3Dsizei,
    const WGC3Dfloat*)

DELEGATE_TO_GL_2(uniform1i, Uniform1i, WGC3Dint, WGC3Dint)

DELEGATE_TO_GL_3(uniform1iv, Uniform1iv, WGC3Dint, WGC3Dsizei, const WGC3Dint*)

DELEGATE_TO_GL_3(uniform2f, Uniform2f, WGC3Dint, WGC3Dfloat, WGC3Dfloat)

DELEGATE_TO_GL_3(uniform2fv, Uniform2fv, WGC3Dint, WGC3Dsizei,
    const WGC3Dfloat*)

DELEGATE_TO_GL_3(uniform2i, Uniform2i, WGC3Dint, WGC3Dint, WGC3Dint)

DELEGATE_TO_GL_3(uniform2iv, Uniform2iv, WGC3Dint, WGC3Dsizei, const WGC3Dint*)

DELEGATE_TO_GL_4(uniform3f, Uniform3f, WGC3Dint,
    WGC3Dfloat, WGC3Dfloat, WGC3Dfloat)

DELEGATE_TO_GL_3(uniform3fv, Uniform3fv, WGC3Dint, WGC3Dsizei,
    const WGC3Dfloat*)

DELEGATE_TO_GL_4(uniform3i, Uniform3i, WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dint)

DELEGATE_TO_GL_3(uniform3iv, Uniform3iv, WGC3Dint, WGC3Dsizei, const WGC3Dint*)

DELEGATE_TO_GL_5(uniform4f, Uniform4f, WGC3Dint,
    WGC3Dfloat, WGC3Dfloat, WGC3Dfloat, WGC3Dfloat)

DELEGATE_TO_GL_3(uniform4fv, Uniform4fv, WGC3Dint, WGC3Dsizei,
    const WGC3Dfloat*)

DELEGATE_TO_GL_5(uniform4i, Uniform4i, WGC3Dint,
    WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dint)

DELEGATE_TO_GL_3(uniform4iv, Uniform4iv, WGC3Dint, WGC3Dsizei, const WGC3Dint*)

DELEGATE_TO_GL_4(uniformMatrix2fv, UniformMatrix2fv,
    WGC3Dint, WGC3Dsizei, WGC3Dboolean, const WGC3Dfloat*)

DELEGATE_TO_GL_4(uniformMatrix3fv, UniformMatrix3fv,
    WGC3Dint, WGC3Dsizei, WGC3Dboolean, const WGC3Dfloat*)

DELEGATE_TO_GL_4(uniformMatrix4fv, UniformMatrix4fv,
    WGC3Dint, WGC3Dsizei, WGC3Dboolean, const WGC3Dfloat*)

DELEGATE_TO_GL_1(useProgram, UseProgram, WebGLId)

DELEGATE_TO_GL_1(validateProgram, ValidateProgram, WebGLId)

DELEGATE_TO_GL_2(vertexAttrib1f, VertexAttrib1f, WGC3Duint, WGC3Dfloat)

DELEGATE_TO_GL_2(vertexAttrib1fv, VertexAttrib1fv, WGC3Duint,
    const WGC3Dfloat*)

DELEGATE_TO_GL_3(vertexAttrib2f, VertexAttrib2f, WGC3Duint,
    WGC3Dfloat, WGC3Dfloat)

DELEGATE_TO_GL_2(vertexAttrib2fv, VertexAttrib2fv, WGC3Duint,
    const WGC3Dfloat*)

DELEGATE_TO_GL_4(vertexAttrib3f, VertexAttrib3f, WGC3Duint,
    WGC3Dfloat, WGC3Dfloat, WGC3Dfloat)

DELEGATE_TO_GL_2(vertexAttrib3fv, VertexAttrib3fv, WGC3Duint,
    const WGC3Dfloat*)

DELEGATE_TO_GL_5(vertexAttrib4f, VertexAttrib4f, WGC3Duint,
    WGC3Dfloat, WGC3Dfloat, WGC3Dfloat, WGC3Dfloat)

DELEGATE_TO_GL_2(vertexAttrib4fv, VertexAttrib4fv, WGC3Duint,
    const WGC3Dfloat*)

void WebGraphicsContext3DImpl::vertexAttribPointer(
    WGC3Duint index, WGC3Dint size, WGC3Denum type, WGC3Dboolean normalized,
    WGC3Dsizei stride, WGC3Dintptr offset)
{
    gl_->VertexAttribPointer(
        index, size, type, normalized, stride,
        reinterpret_cast<void*>(static_cast<intptr_t>(offset)));
}

DELEGATE_TO_GL_4(viewport, Viewport,
    WGC3Dint, WGC3Dint, WGC3Dsizei, WGC3Dsizei)

WebGLId WebGraphicsContext3DImpl::createBuffer()
{
    GLuint o;
    gl_->GenBuffers(1, &o);
    return o;
}

WebGLId WebGraphicsContext3DImpl::createFramebuffer()
{
    GLuint o = 0;
    gl_->GenFramebuffers(1, &o);
    return o;
}

WebGLId WebGraphicsContext3DImpl::createRenderbuffer()
{
    GLuint o;
    gl_->GenRenderbuffers(1, &o);
    return o;
}

WebGLId WebGraphicsContext3DImpl::createTexture()
{
    GLuint o;
    gl_->GenTextures(1, &o);
    return o;
}

void WebGraphicsContext3DImpl::deleteBuffer(WebGLId buffer)
{
    gl_->DeleteBuffers(1, &buffer);
}

void WebGraphicsContext3DImpl::deleteFramebuffer(
    WebGLId framebuffer)
{
    gl_->DeleteFramebuffers(1, &framebuffer);
}

void WebGraphicsContext3DImpl::deleteRenderbuffer(
    WebGLId renderbuffer)
{
    gl_->DeleteRenderbuffers(1, &renderbuffer);
}

void WebGraphicsContext3DImpl::deleteTexture(WebGLId texture)
{
    gl_->DeleteTextures(1, &texture);
}

DELEGATE_TO_GL_R(createProgram, CreateProgram, WebGLId)

DELEGATE_TO_GL_1R(createShader, CreateShader, WGC3Denum, WebGLId)

DELEGATE_TO_GL_1(deleteProgram, DeleteProgram, WebGLId)

DELEGATE_TO_GL_1(deleteShader, DeleteShader, WebGLId)

void WebGraphicsContext3DImpl::setErrorMessageCallback(
    WebGraphicsContext3D::WebGraphicsErrorMessageCallback* cb)
{
    error_message_callback_ = cb;
}

void WebGraphicsContext3DImpl::setContextLostCallback(
    WebGraphicsContext3D::WebGraphicsContextLostCallback* cb)
{
    context_lost_callback_ = cb;
}

DELEGATE_TO_GL_5(texImageIOSurface2DCHROMIUM, TexImageIOSurface2DCHROMIUM,
    WGC3Denum, WGC3Dint, WGC3Dint, WGC3Duint, WGC3Duint)

DELEGATE_TO_GL_5(texStorage2DEXT, TexStorage2DEXT,
    WGC3Denum, WGC3Dint, WGC3Duint, WGC3Dint, WGC3Dint)

WebGLId WebGraphicsContext3DImpl::createQueryEXT()
{
    GLuint o;
    gl_->GenQueriesEXT(1, &o);
    return o;
}

void WebGraphicsContext3DImpl::deleteQueryEXT(
    WebGLId query)
{
    gl_->DeleteQueriesEXT(1, &query);
}

DELEGATE_TO_GL_1R(isQueryEXT, IsQueryEXT, WebGLId, WGC3Dboolean)
DELEGATE_TO_GL_2(beginQueryEXT, BeginQueryEXT, WGC3Denum, WebGLId)
DELEGATE_TO_GL_1(endQueryEXT, EndQueryEXT, WGC3Denum)
DELEGATE_TO_GL_3(getQueryivEXT, GetQueryivEXT, WGC3Denum, WGC3Denum, WGC3Dint*)
DELEGATE_TO_GL_3(getQueryObjectuivEXT, GetQueryObjectuivEXT,
    WebGLId, WGC3Denum, WGC3Duint*)

DELEGATE_TO_GL_2(queryCounterEXT, QueryCounterEXT, WebGLId, WGC3Denum)
DELEGATE_TO_GL_3(getQueryObjectui64vEXT,
    GetQueryObjectui64vEXT,
    WebGLId,
    WGC3Denum,
    WGC3Duint64*)

DELEGATE_TO_GL_8(copyTextureCHROMIUM,
    CopyTextureCHROMIUM,
    WGC3Denum,
    WebGLId,
    WebGLId,
    WGC3Denum,
    WGC3Denum,
    WGC3Dboolean,
    WGC3Dboolean,
    WGC3Dboolean);

DELEGATE_TO_GL_12(copySubTextureCHROMIUM,
    CopySubTextureCHROMIUM,
    WGC3Denum,
    WebGLId,
    WebGLId,
    WGC3Dint,
    WGC3Dint,
    WGC3Dint,
    WGC3Dint,
    WGC3Dsizei,
    WGC3Dsizei,
    WGC3Dboolean,
    WGC3Dboolean,
    WGC3Dboolean);

DELEGATE_TO_GL_3(bindUniformLocationCHROMIUM, BindUniformLocationCHROMIUM,
    WebGLId, WGC3Dint, const WGC3Dchar*)

void WebGraphicsContext3DImpl::shallowFlushCHROMIUM()
{
    flush_id_ = GenFlushID();
    gl_->ShallowFlushCHROMIUM();
}

void WebGraphicsContext3DImpl::shallowFinishCHROMIUM()
{
    flush_id_ = GenFlushID();
    gl_->ShallowFinishCHROMIUM();
}

DELEGATE_TO_GL_1(waitSyncToken, WaitSyncTokenCHROMIUM, const WGC3Dbyte*)

void WebGraphicsContext3DImpl::loseContextCHROMIUM(
    WGC3Denum current, WGC3Denum other)
{
    gl_->LoseContextCHROMIUM(current, other);
    gl_->Flush();
}

DELEGATE_TO_GL_1(genMailboxCHROMIUM, GenMailboxCHROMIUM, WGC3Dbyte*)
DELEGATE_TO_GL_2(produceTextureCHROMIUM, ProduceTextureCHROMIUM,
    WGC3Denum, const WGC3Dbyte*)
DELEGATE_TO_GL_3(produceTextureDirectCHROMIUM, ProduceTextureDirectCHROMIUM,
    WebGLId, WGC3Denum, const WGC3Dbyte*)
DELEGATE_TO_GL_2(consumeTextureCHROMIUM, ConsumeTextureCHROMIUM,
    WGC3Denum, const WGC3Dbyte*)
DELEGATE_TO_GL_2R(createAndConsumeTextureCHROMIUM,
    CreateAndConsumeTextureCHROMIUM,
    WGC3Denum, const WGC3Dbyte*, WebGLId)

DELEGATE_TO_GL_2(genValuebuffersCHROMIUM,
    GenValuebuffersCHROMIUM,
    WGC3Dsizei,
    WebGLId*);

WebGLId WebGraphicsContext3DImpl::createValuebufferCHROMIUM()
{
    GLuint o;
    gl_->GenValuebuffersCHROMIUM(1, &o);
    return o;
}

DELEGATE_TO_GL_2(deleteValuebuffersCHROMIUM,
    DeleteValuebuffersCHROMIUM,
    WGC3Dsizei,
    WebGLId*);

void WebGraphicsContext3DImpl::deleteValuebufferCHROMIUM(WebGLId valuebuffer)
{
    gl_->DeleteValuebuffersCHROMIUM(1, &valuebuffer);
}

DELEGATE_TO_GL_1RB(isValuebufferCHROMIUM,
    IsValuebufferCHROMIUM,
    WebGLId,
    WGC3Dboolean)
DELEGATE_TO_GL_2(bindValuebufferCHROMIUM,
    BindValuebufferCHROMIUM,
    WGC3Denum,
    WebGLId)
DELEGATE_TO_GL_2(subscribeValueCHROMIUM,
    SubscribeValueCHROMIUM,
    WGC3Denum,
    WGC3Denum);
DELEGATE_TO_GL_1(populateSubscribedValuesCHROMIUM,
    PopulateSubscribedValuesCHROMIUM,
    WGC3Denum);
DELEGATE_TO_GL_3(uniformValuebufferCHROMIUM,
    UniformValuebufferCHROMIUM,
    WGC3Dint,
    WGC3Denum,
    WGC3Denum);
DELEGATE_TO_GL_2(traceBeginCHROMIUM,
    TraceBeginCHROMIUM,
    const WGC3Dchar*,
    const WGC3Dchar*);
DELEGATE_TO_GL(traceEndCHROMIUM, TraceEndCHROMIUM);

void WebGraphicsContext3DImpl::insertEventMarkerEXT(
    const WGC3Dchar* marker)
{
    gl_->InsertEventMarkerEXT(0, marker);
}

void WebGraphicsContext3DImpl::pushGroupMarkerEXT(
    const WGC3Dchar* marker)
{
    gl_->PushGroupMarkerEXT(0, marker);
}

DELEGATE_TO_GL(popGroupMarkerEXT, PopGroupMarkerEXT);

WebGLId WebGraphicsContext3DImpl::createVertexArrayOES()
{
    GLuint array;
    gl_->GenVertexArraysOES(1, &array);
    return array;
}

void WebGraphicsContext3DImpl::deleteVertexArrayOES(
    WebGLId array)
{
    gl_->DeleteVertexArraysOES(1, &array);
}

DELEGATE_TO_GL_1R(isVertexArrayOES, IsVertexArrayOES, WebGLId, WGC3Dboolean)
DELEGATE_TO_GL_1(bindVertexArrayOES, BindVertexArrayOES, WebGLId)

DELEGATE_TO_GL_2(bindTexImage2DCHROMIUM, BindTexImage2DCHROMIUM,
    WGC3Denum, WGC3Dint)
DELEGATE_TO_GL_2(releaseTexImage2DCHROMIUM, ReleaseTexImage2DCHROMIUM,
    WGC3Denum, WGC3Dint)

DELEGATE_TO_GL_2R(mapBufferCHROMIUM, MapBufferCHROMIUM, WGC3Denum, WGC3Denum,
    void*)
DELEGATE_TO_GL_1R(unmapBufferCHROMIUM, UnmapBufferCHROMIUM, WGC3Denum,
    WGC3Dboolean)

DELEGATE_TO_GL_2(drawBuffersEXT, DrawBuffersEXT, WGC3Dsizei, const WGC3Denum*)

DELEGATE_TO_GL_4(drawArraysInstancedANGLE, DrawArraysInstancedANGLE, WGC3Denum,
    WGC3Dint, WGC3Dsizei, WGC3Dsizei)

void WebGraphicsContext3DImpl::drawElementsInstancedANGLE(
    WGC3Denum mode,
    WGC3Dsizei count,
    WGC3Denum type,
    WGC3Dintptr offset,
    WGC3Dsizei primcount)
{
    gl_->DrawElementsInstancedANGLE(
        mode, count, type,
        reinterpret_cast<void*>(static_cast<intptr_t>(offset)), primcount);
}

DELEGATE_TO_GL_2(vertexAttribDivisorANGLE, VertexAttribDivisorANGLE, WGC3Duint,
    WGC3Duint)

DELEGATE_TO_GL_4R(createGpuMemoryBufferImageCHROMIUM,
    CreateGpuMemoryBufferImageCHROMIUM,
    WGC3Dsizei,
    WGC3Dsizei,
    WGC3Denum,
    WGC3Denum,
    WGC3Duint);

DELEGATE_TO_GL_1(destroyImageCHROMIUM, DestroyImageCHROMIUM, WGC3Duint);

DELEGATE_TO_GL_6(framebufferTexture2DMultisampleEXT,
    FramebufferTexture2DMultisampleEXT,
    WGC3Denum, WGC3Denum, WGC3Denum, WebGLId, WGC3Dint, WGC3Dsizei)

DELEGATE_TO_GL_5(renderbufferStorageMultisampleEXT,
    RenderbufferStorageMultisampleEXT, WGC3Denum, WGC3Dsizei,
    WGC3Denum, WGC3Dsizei, WGC3Dsizei)

DELEGATE_TO_GL_1(beginTransformFeedback, BeginTransformFeedback, WGC3Denum)
DELEGATE_TO_GL_3(bindBufferBase, BindBufferBase, WGC3Denum, WGC3Duint,
    WGC3Duint)
DELEGATE_TO_GL_5(bindBufferRange, BindBufferRange, WGC3Denum, WGC3Duint,
    WGC3Duint, WGC3Dintptr, WGC3Dsizeiptr)
DELEGATE_TO_GL_2(bindSampler, BindSampler, WGC3Duint, WebGLId)
DELEGATE_TO_GL_2(bindTransformFeedback, BindTransformFeedback, WGC3Denum,
    WebGLId)
DELEGATE_TO_GL_4(clearBufferfi, ClearBufferfi, WGC3Denum, WGC3Dint, WGC3Dfloat,
    WGC3Dint)
DELEGATE_TO_GL_3(clearBufferfv, ClearBufferfv, WGC3Denum, WGC3Dint,
    const WGC3Dfloat*)
DELEGATE_TO_GL_3(clearBufferiv, ClearBufferiv, WGC3Denum, WGC3Dint,
    const WGC3Dint*)
DELEGATE_TO_GL_3(clearBufferuiv, ClearBufferuiv, WGC3Denum, WGC3Dint,
    const WGC3Duint*)
WGC3Denum WebGraphicsContext3DImpl::clientWaitSync(WGC3Dsync sync,
    WGC3Dbitfield flags,
    WGC3Duint64 timeout)
{
    return gl_->ClientWaitSync(reinterpret_cast<GLsync>(sync), flags, timeout);
}
DELEGATE_TO_GL_9(compressedTexImage3D, CompressedTexImage3D, WGC3Denum,
    WGC3Dint, WGC3Denum, WGC3Dsizei, WGC3Dsizei, WGC3Dsizei,
    WGC3Dint, WGC3Dsizei, const void*)
DELEGATE_TO_GL_11(compressedTexSubImage3D, CompressedTexSubImage3D, WGC3Denum,
    WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dsizei,
    WGC3Dsizei, WGC3Dsizei, WGC3Denum, WGC3Dsizei, const void*)
DELEGATE_TO_GL_5(copyBufferSubData, CopyBufferSubData, WGC3Denum, WGC3Denum,
    WGC3Dintptr, WGC3Dintptr, WGC3Dsizeiptr)
DELEGATE_TO_GL_9(copyTexSubImage3D, CopyTexSubImage3D, WGC3Denum, WGC3Dint,
    WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dint, WGC3Dsizei,
    WGC3Dsizei)
WebGLId WebGraphicsContext3DImpl::createSampler()
{
    GLuint sampler;
    gl_->GenSamplers(1, &sampler);
    return sampler;
}
WebGLId WebGraphicsContext3DImpl::createTransformFeedback()
{
    GLuint tf;
    gl_->GenTransformFeedbacks(1, &tf);
    return tf;
}
void WebGraphicsContext3DImpl::deleteSampler(WebGLId sampler)
{
    gl_->DeleteSamplers(1, &sampler);
}
void WebGraphicsContext3DImpl::deleteSync(WGC3Dsync sync)
{
    gl_->DeleteSync(reinterpret_cast<GLsync>(sync));
}
void WebGraphicsContext3DImpl::deleteTransformFeedback(WebGLId tf)
{
    gl_->DeleteTransformFeedbacks(1, &tf);
}
void WebGraphicsContext3DImpl::drawRangeElements(
    WGC3Denum mode, WGC3Duint start, WGC3Duint end, WGC3Dsizei count,
    WGC3Denum type, WGC3Dintptr offset)
{
    gl_->DrawRangeElements(mode, start, end, count, type,
        reinterpret_cast<void*>(static_cast<intptr_t>(offset)));
}
DELEGATE_TO_GL(endTransformFeedback, EndTransformFeedback)
WGC3Dsync WebGraphicsContext3DImpl::fenceSync(WGC3Denum condition,
    WGC3Dbitfield flags)
{
    return reinterpret_cast<WGC3Dsync>(gl_->FenceSync(condition, flags));
}
DELEGATE_TO_GL_5(framebufferTextureLayer, FramebufferTextureLayer, WGC3Denum,
    WGC3Denum, WGC3Duint, WGC3Dint, WGC3Dint)
DELEGATE_TO_GL_5(getActiveUniformBlockName, GetActiveUniformBlockName,
    WGC3Duint, WGC3Duint, WGC3Dsizei, WGC3Dsizei*, WGC3Dchar*)
DELEGATE_TO_GL_4(getActiveUniformBlockiv, GetActiveUniformBlockiv, WGC3Duint,
    WGC3Duint, WGC3Denum, WGC3Dint*)
DELEGATE_TO_GL_5(getActiveUniformsiv, GetActiveUniformsiv, WGC3Duint,
    WGC3Dsizei, const WGC3Duint*, WGC3Denum, WGC3Dint*)
DELEGATE_TO_GL_2R(getFragDataLocation, GetFragDataLocation, WGC3Duint,
    const WGC3Dchar*, WGC3Dint)
DELEGATE_TO_GL_5(getInternalformativ, GetInternalformativ, WGC3Denum, WGC3Denum,
    WGC3Denum, WGC3Dsizei, WGC3Dint*)
DELEGATE_TO_GL_3(getSamplerParameterfv, GetSamplerParameterfv, WGC3Duint,
    WGC3Denum, WGC3Dfloat*)
DELEGATE_TO_GL_3(getSamplerParameteriv, GetSamplerParameteriv, WGC3Duint,
    WGC3Denum, WGC3Dint*)
DELEGATE_TO_GL_7(getTransformFeedbackVarying, GetTransformFeedbackVarying,
    WGC3Duint, WGC3Duint, WGC3Dsizei, WGC3Dsizei*, WGC3Dsizei*,
    WGC3Denum*, WGC3Dchar*)
DELEGATE_TO_GL_2R(getUniformBlockIndex, GetUniformBlockIndex, WGC3Duint,
    const WGC3Dchar*, WGC3Duint)
DELEGATE_TO_GL_4(getUniformIndices, GetUniformIndices, WGC3Duint, WGC3Dsizei,
    const WGC3Dchar* const*, WGC3Duint*)
DELEGATE_TO_GL_3(getUniformuiv, GetUniformuiv, WGC3Duint, WGC3Dint,
    WGC3Duint*)
DELEGATE_TO_GL_3(getVertexAttribIiv, GetVertexAttribIiv, WGC3Duint,
    WGC3Denum, WGC3Dint*)
DELEGATE_TO_GL_3(getVertexAttribIuiv, GetVertexAttribIuiv, WGC3Duint,
    WGC3Denum, WGC3Duint*)
DELEGATE_TO_GL_3(invalidateFramebuffer, InvalidateFramebuffer, WGC3Denum,
    WGC3Dsizei, const WGC3Denum*)
DELEGATE_TO_GL_7(invalidateSubFramebuffer, InvalidateSubFramebuffer, WGC3Denum,
    WGC3Dsizei, const WGC3Denum*, WGC3Dint, WGC3Dint, WGC3Dsizei,
    WGC3Dsizei)
DELEGATE_TO_GL_1R(isSampler, IsSampler, WebGLId, WGC3Dboolean)
WGC3Dboolean WebGraphicsContext3DImpl::isSync(WGC3Dsync sync)
{
    return gl_->IsSync(reinterpret_cast<GLsync>(sync));
}
DELEGATE_TO_GL_1R(isTransformFeedback, IsTransformFeedback, WGC3Duint,
    WGC3Dboolean)
DELEGATE_TO_GL_4R(mapBufferRange, MapBufferRange, WGC3Denum, WGC3Dintptr,
    WGC3Dsizeiptr, WGC3Dbitfield, void*);
DELEGATE_TO_GL(pauseTransformFeedback, PauseTransformFeedback)
//DELEGATE_TO_GL_3(programParameteri, ProgramParameteri, WGC3Duint, WGC3Denum,
//                 WGC3Dint)
DELEGATE_TO_GL_1(readBuffer, ReadBuffer, WGC3Denum)
DELEGATE_TO_GL(resumeTransformFeedback, ResumeTransformFeedback)
DELEGATE_TO_GL_3(samplerParameterf, SamplerParameterf, WGC3Duint, WGC3Denum,
    WGC3Dfloat)
DELEGATE_TO_GL_3(samplerParameterfv, SamplerParameterfv, WGC3Duint, WGC3Denum,
    const WGC3Dfloat*)
DELEGATE_TO_GL_3(samplerParameteri, SamplerParameteri, WGC3Duint, WGC3Denum,
    WGC3Dint)
DELEGATE_TO_GL_3(samplerParameteriv, SamplerParameteriv, WGC3Duint, WGC3Denum,
    const WGC3Dint*)
DELEGATE_TO_GL_10(texImage3D, TexImage3D, WGC3Denum, WGC3Dint, WGC3Dint,
    WGC3Dsizei, WGC3Dsizei, WGC3Dsizei, WGC3Dint, WGC3Denum,
    WGC3Denum, const void*)
DELEGATE_TO_GL_6(texStorage3D, TexStorage3D, WGC3Denum, WGC3Dsizei, WGC3Denum,
    WGC3Dsizei, WGC3Dsizei, WGC3Dsizei)
DELEGATE_TO_GL_11(texSubImage3D, TexSubImage3D, WGC3Denum, WGC3Dint, WGC3Dint,
    WGC3Dint, WGC3Dint, WGC3Dsizei, WGC3Dsizei, WGC3Dsizei,
    WGC3Denum, WGC3Denum, const void*)
DELEGATE_TO_GL_4(transformFeedbackVaryings, TransformFeedbackVaryings,
    WGC3Duint, WGC3Dsizei, const WGC3Dchar* const*, WGC3Denum)
DELEGATE_TO_GL_2(uniform1ui, Uniform1ui, WGC3Dint, WGC3Duint)
DELEGATE_TO_GL_3(uniform1uiv, Uniform1uiv, WGC3Dint, WGC3Dsizei,
    const WGC3Duint*)
DELEGATE_TO_GL_3(uniform2ui, Uniform2ui, WGC3Dint, WGC3Duint, WGC3Duint)
DELEGATE_TO_GL_3(uniform2uiv, Uniform2uiv, WGC3Dint, WGC3Dsizei,
    const WGC3Duint*)
DELEGATE_TO_GL_4(uniform3ui, Uniform3ui, WGC3Dint, WGC3Duint, WGC3Duint,
    WGC3Duint)
DELEGATE_TO_GL_3(uniform3uiv, Uniform3uiv, WGC3Dint, WGC3Dsizei,
    const WGC3Duint*)
DELEGATE_TO_GL_5(uniform4ui, Uniform4ui, WGC3Dint, WGC3Duint, WGC3Duint,
    WGC3Duint, WGC3Duint)
DELEGATE_TO_GL_3(uniform4uiv, Uniform4uiv, WGC3Dint, WGC3Dsizei,
    const WGC3Duint*)
DELEGATE_TO_GL_3(uniformBlockBinding, UniformBlockBinding, WGC3Duint, WGC3Duint,
    WGC3Duint)
DELEGATE_TO_GL_4(uniformMatrix2x3fv, UniformMatrix2x3fv, WGC3Dint, WGC3Dsizei,
    WGC3Dboolean, const WGC3Dfloat*)
DELEGATE_TO_GL_4(uniformMatrix2x4fv, UniformMatrix2x4fv, WGC3Dint, WGC3Dsizei,
    WGC3Dboolean, const WGC3Dfloat*)
DELEGATE_TO_GL_4(uniformMatrix3x2fv, UniformMatrix3x2fv, WGC3Dint, WGC3Dsizei,
    WGC3Dboolean, const WGC3Dfloat*)
DELEGATE_TO_GL_4(uniformMatrix3x4fv, UniformMatrix3x4fv, WGC3Dint, WGC3Dsizei,
    WGC3Dboolean, const WGC3Dfloat*)
DELEGATE_TO_GL_4(uniformMatrix4x2fv, UniformMatrix4x2fv, WGC3Dint, WGC3Dsizei,
    WGC3Dboolean, const WGC3Dfloat*)
DELEGATE_TO_GL_4(uniformMatrix4x3fv, UniformMatrix4x3fv, WGC3Dint, WGC3Dsizei,
    WGC3Dboolean, const WGC3Dfloat*)
DELEGATE_TO_GL_1R(unmapBuffer, UnmapBuffer, WGC3Denum, WGC3Dboolean);
DELEGATE_TO_GL_5(vertexAttribI4i, VertexAttribI4i, WGC3Duint, WGC3Dint,
    WGC3Dint, WGC3Dint, WGC3Dint)
DELEGATE_TO_GL_2(vertexAttribI4iv, VertexAttribI4iv, WGC3Duint,
    const WGC3Dint*)
DELEGATE_TO_GL_5(vertexAttribI4ui, VertexAttribI4ui, WGC3Duint, WGC3Duint,
    WGC3Duint, WGC3Duint, WGC3Duint)
DELEGATE_TO_GL_2(vertexAttribI4uiv, VertexAttribI4uiv, WGC3Duint,
    const WGC3Duint*)
void WebGraphicsContext3DImpl::vertexAttribIPointer(
    WGC3Duint index, WGC3Dint size, WGC3Denum type, WGC3Dsizei stride,
    WGC3Dintptr offset)
{
    gl_->VertexAttribIPointer(
        index, size, type, stride,
        reinterpret_cast<void*>(static_cast<intptr_t>(offset)));
}
void WebGraphicsContext3DImpl::waitSync(WGC3Dsync sync,
    WGC3Dbitfield flags,
    WGC3Duint64 timeout)
{
    gl_->WaitSync(reinterpret_cast<GLsync>(sync), flags, timeout);
}

bool WebGraphicsContext3DImpl::isContextLost()
{
    return getGraphicsResetStatusARB() != GL_NO_ERROR;
}

blink::WGC3Denum WebGraphicsContext3DImpl::getGraphicsResetStatusARB()
{
    return gl_->GetGraphicsResetStatusKHR();
}

::gpu::gles2::GLES2ImplementationErrorMessageCallback*
WebGraphicsContext3DImpl::getErrorMessageCallback()
{
    if (!client_error_message_callback_) {
        client_error_message_callback_.reset(
            new WebGraphicsContext3DErrorMessageCallback(this));
    }
    return client_error_message_callback_.get();
}

void WebGraphicsContext3DImpl::OnErrorMessage(
    const std::string& message, int id)
{
    if (error_message_callback_) {
        blink::WebString str = blink::WebString::fromUTF8(message.c_str());
        error_message_callback_->onErrorMessage(str, id);
    }
}

// TODO(bajones): Look into removing these functions from the blink interface
void WebGraphicsContext3DImpl::prepareTexture()
{
    NOTREACHED();
}

void WebGraphicsContext3DImpl::postSubBufferCHROMIUM(
    int x, int y, int width, int height)
{
    NOTREACHED();
}

void WebGraphicsContext3DImpl::setVisibilityCHROMIUM(
    bool visible)
{
    NOTREACHED();
}

void WebGraphicsContext3DImpl::copyTextureToParentTextureCHROMIUM(
    WebGLId texture, WebGLId parentTexture)
{
    NOTIMPLEMENTED();
}

void WebGraphicsContext3DImpl::releaseShaderCompiler()
{
}

// static
void WebGraphicsContext3DImpl::ConvertAttributes(
    const blink::WebGraphicsContext3D::Attributes& attributes,
    ::gpu::gles2::ContextCreationAttribHelper* output_attribs)
{
    output_attribs->alpha_size = attributes.alpha ? 8 : 0;
    output_attribs->depth_size = attributes.depth ? 24 : 0;
    output_attribs->stencil_size = attributes.stencil ? 8 : 0;
    output_attribs->samples = attributes.antialias ? 4 : 0;
    output_attribs->sample_buffers = attributes.antialias ? 1 : 0;
    output_attribs->fail_if_major_perf_caveat = attributes.failIfMajorPerformanceCaveat;
    output_attribs->bind_generates_resource = false;
    switch (attributes.webGLVersion) {
    case 0:
        output_attribs->context_type = ::gpu::gles2::CONTEXT_TYPE_OPENGLES2;
        break;
    case 1:
        output_attribs->context_type = ::gpu::gles2::CONTEXT_TYPE_WEBGL1;
        break;
    case 2:
        output_attribs->context_type = ::gpu::gles2::CONTEXT_TYPE_WEBGL2;
        break;
    default:
        NOTREACHED();
        output_attribs->context_type = ::gpu::gles2::CONTEXT_TYPE_OPENGLES2;
        break;
    }
}

} // namespace gpu_blink
