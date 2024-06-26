// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_BLINK_WEBGRAPHICSCONTEXT3D_IMPL_H_
#define GPU_BLINK_WEBGRAPHICSCONTEXT3D_IMPL_H_

#include <string>
#include <vector>

#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "gpu/blink/gpu_blink_export.h"
#include "third_party/WebKit/public/platform/WebGraphicsContext3D.h"
#include "third_party/WebKit/public/platform/WebString.h"

namespace gpu {

namespace gles2 {
    class GLES2Interface;
    class GLES2ImplementationErrorMessageCallback;
    struct ContextCreationAttribHelper;
}
}

namespace gpu_blink {

class WebGraphicsContext3DErrorMessageCallback;

class GPU_BLINK_EXPORT WebGraphicsContext3DImpl
    : public NON_EXPORTED_BASE(blink::WebGraphicsContext3D) {
public:
    ~WebGraphicsContext3DImpl() override;

    //----------------------------------------------------------------------
    // WebGraphicsContext3D methods

    uint32_t lastFlushID() override;

    bool insertSyncPoint(blink::WGC3Dbyte* sync_token) override;
    void waitSyncToken(const blink::WGC3Dbyte* sync_token) override;

    void loseContextCHROMIUM(blink::WGC3Denum current,
        blink::WGC3Denum other) override;

    void reshapeWithScaleFactor(
        int width, int height, float scale_factor) override;

    void prepareTexture() override;
    void postSubBufferCHROMIUM(int x, int y, int width, int height) override;

    void activeTexture(blink::WGC3Denum texture) override;
    void applyScreenSpaceAntialiasingCHROMIUM() override;
    void attachShader(blink::WebGLId program, blink::WebGLId shader) override;
    void bindAttribLocation(blink::WebGLId program,
        blink::WGC3Duint index,
        const blink::WGC3Dchar* name) override;
    void bindBuffer(blink::WGC3Denum target, blink::WebGLId buffer) override;
    void bindFramebuffer(blink::WGC3Denum target,
        blink::WebGLId framebuffer) override;
    void bindRenderbuffer(blink::WGC3Denum target,
        blink::WebGLId renderbuffer) override;
    void bindTexture(blink::WGC3Denum target, blink::WebGLId texture) override;
    void blendColor(blink::WGC3Dclampf red,
        blink::WGC3Dclampf green,
        blink::WGC3Dclampf blue,
        blink::WGC3Dclampf alpha) override;
    void blendEquation(blink::WGC3Denum mode) override;
    void blendEquationSeparate(blink::WGC3Denum modeRGB,
        blink::WGC3Denum modeAlpha) override;
    void blendFunc(blink::WGC3Denum sfactor, blink::WGC3Denum dfactor) override;
    void blendFuncSeparate(blink::WGC3Denum srcRGB,
        blink::WGC3Denum dstRGB,
        blink::WGC3Denum srcAlpha,
        blink::WGC3Denum dstAlpha) override;

    void bufferData(blink::WGC3Denum target,
        blink::WGC3Dsizeiptr size,
        const void* data,
        blink::WGC3Denum usage) override;
    void bufferSubData(blink::WGC3Denum target,
        blink::WGC3Dintptr offset,
        blink::WGC3Dsizeiptr size,
        const void* data) override;

    blink::WGC3Denum checkFramebufferStatus(blink::WGC3Denum target) override;
    void clear(blink::WGC3Dbitfield mask) override;
    void clearColor(blink::WGC3Dclampf red,
        blink::WGC3Dclampf green,
        blink::WGC3Dclampf blue,
        blink::WGC3Dclampf alpha) override;
    void clearDepth(blink::WGC3Dclampf depth) override;
    void clearStencil(blink::WGC3Dint s) override;
    void colorMask(blink::WGC3Dboolean red,
        blink::WGC3Dboolean green,
        blink::WGC3Dboolean blue,
        blink::WGC3Dboolean alpha) override;
    void compileShader(blink::WebGLId shader) override;

    void compressedTexImage2D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Denum internalformat,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Dint border,
        blink::WGC3Dsizei imageSize,
        const void* data) override;
    void compressedTexSubImage2D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Dint xoffset,
        blink::WGC3Dint yoffset,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Denum format,
        blink::WGC3Dsizei imageSize,
        const void* data) override;
    void copyTexImage2D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Denum internalformat,
        blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Dint border) override;
    void copyTexSubImage2D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Dint xoffset,
        blink::WGC3Dint yoffset,
        blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height) override;
    void cullFace(blink::WGC3Denum mode) override;
    void depthFunc(blink::WGC3Denum func) override;
    void depthMask(blink::WGC3Dboolean flag) override;
    void depthRange(blink::WGC3Dclampf zNear, blink::WGC3Dclampf zFar) override;
    void detachShader(blink::WebGLId program, blink::WebGLId shader) override;
    void disable(blink::WGC3Denum cap) override;
    void disableVertexAttribArray(blink::WGC3Duint index) override;
    void drawArrays(blink::WGC3Denum mode,
        blink::WGC3Dint first,
        blink::WGC3Dsizei count) override;
    void drawElements(blink::WGC3Denum mode,
        blink::WGC3Dsizei count,
        blink::WGC3Denum type,
        blink::WGC3Dintptr offset) override;

    void enable(blink::WGC3Denum cap) override;
    void enableVertexAttribArray(blink::WGC3Duint index) override;
    void finish() override;
    void flush() override;
    void framebufferRenderbuffer(blink::WGC3Denum target,
        blink::WGC3Denum attachment,
        blink::WGC3Denum renderbuffertarget,
        blink::WebGLId renderbuffer) override;
    void framebufferTexture2D(blink::WGC3Denum target,
        blink::WGC3Denum attachment,
        blink::WGC3Denum textarget,
        blink::WebGLId texture,
        blink::WGC3Dint level) override;
    void frontFace(blink::WGC3Denum mode) override;
    void generateMipmap(blink::WGC3Denum target) override;

    bool getActiveAttrib(blink::WebGLId program,
        blink::WGC3Duint index,
        ActiveInfo&) override;
    bool getActiveUniform(blink::WebGLId program,
        blink::WGC3Duint index,
        ActiveInfo&) override;

    void getAttachedShaders(blink::WebGLId program,
        blink::WGC3Dsizei maxCount,
        blink::WGC3Dsizei* count,
        blink::WebGLId* shaders) override;

    blink::WGC3Dint getAttribLocation(blink::WebGLId program,
        const blink::WGC3Dchar* name) override;

    void getBooleanv(blink::WGC3Denum pname, blink::WGC3Dboolean* value) override;

    void getBufferParameteriv(blink::WGC3Denum target,
        blink::WGC3Denum pname,
        blink::WGC3Dint* value) override;

    blink::WGC3Denum getError() override;

    void getFloatv(blink::WGC3Denum pname, blink::WGC3Dfloat* value) override;

    void getFramebufferAttachmentParameteriv(blink::WGC3Denum target,
        blink::WGC3Denum attachment,
        blink::WGC3Denum pname,
        blink::WGC3Dint* value) override;

    void getIntegerv(blink::WGC3Denum pname, blink::WGC3Dint* value) override;

    void getInteger64v(blink::WGC3Denum pname, blink::WGC3Dint64* value) override;

    void getIntegeri_v(blink::WGC3Denum target,
        blink::WGC3Duint index,
        blink::WGC3Dint* data) override;

    void getInteger64i_v(blink::WGC3Denum pname,
        blink::WGC3Duint index,
        blink::WGC3Dint64* value) override;

    void getProgramiv(blink::WebGLId program,
        blink::WGC3Denum pname,
        blink::WGC3Dint* value) override;

    blink::WebString getProgramInfoLog(blink::WebGLId program) override;

    void getRenderbufferParameteriv(blink::WGC3Denum target,
        blink::WGC3Denum pname,
        blink::WGC3Dint* value) override;

    void getShaderiv(blink::WebGLId shader,
        blink::WGC3Denum pname,
        blink::WGC3Dint* value) override;

    blink::WebString getShaderInfoLog(blink::WebGLId shader) override;

    void getShaderPrecisionFormat(blink::WGC3Denum shadertype,
        blink::WGC3Denum precisiontype,
        blink::WGC3Dint* range,
        blink::WGC3Dint* precision) override;

    blink::WebString getShaderSource(blink::WebGLId shader) override;
    blink::WebString getString(blink::WGC3Denum name) override;

    void getTexParameterfv(blink::WGC3Denum target,
        blink::WGC3Denum pname,
        blink::WGC3Dfloat* value) override;
    void getTexParameteriv(blink::WGC3Denum target,
        blink::WGC3Denum pname,
        blink::WGC3Dint* value) override;

    void getUniformfv(blink::WebGLId program,
        blink::WGC3Dint location,
        blink::WGC3Dfloat* value) override;
    void getUniformiv(blink::WebGLId program,
        blink::WGC3Dint location,
        blink::WGC3Dint* value) override;

    blink::WGC3Dint getUniformLocation(blink::WebGLId program,
        const blink::WGC3Dchar* name) override;

    void getVertexAttribfv(blink::WGC3Duint index,
        blink::WGC3Denum pname,
        blink::WGC3Dfloat* value) override;
    void getVertexAttribiv(blink::WGC3Duint index,
        blink::WGC3Denum pname,
        blink::WGC3Dint* value) override;

    blink::WGC3Dsizeiptr getVertexAttribOffset(blink::WGC3Duint index,
        blink::WGC3Denum pname) override;

    void hint(blink::WGC3Denum target, blink::WGC3Denum mode) override;
    blink::WGC3Dboolean isBuffer(blink::WebGLId buffer) override;
    blink::WGC3Dboolean isEnabled(blink::WGC3Denum cap) override;
    blink::WGC3Dboolean isFramebuffer(blink::WebGLId framebuffer) override;
    blink::WGC3Dboolean isProgram(blink::WebGLId program) override;
    blink::WGC3Dboolean isRenderbuffer(blink::WebGLId renderbuffer) override;
    blink::WGC3Dboolean isShader(blink::WebGLId shader) override;
    blink::WGC3Dboolean isTexture(blink::WebGLId texture) override;
    void lineWidth(blink::WGC3Dfloat) override;
    void linkProgram(blink::WebGLId program) override;
    void pixelStorei(blink::WGC3Denum pname, blink::WGC3Dint param) override;
    void polygonOffset(blink::WGC3Dfloat factor,
        blink::WGC3Dfloat units) override;

    void readPixels(blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Denum format,
        blink::WGC3Denum type,
        void* pixels) override;

    void releaseShaderCompiler() override;
    void renderbufferStorage(blink::WGC3Denum target,
        blink::WGC3Denum internalformat,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height) override;
    void sampleCoverage(blink::WGC3Dfloat value,
        blink::WGC3Dboolean invert) override;
    void scissor(blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height) override;
    void shaderSource(blink::WebGLId shader,
        const blink::WGC3Dchar* string) override;
    void stencilFunc(blink::WGC3Denum func,
        blink::WGC3Dint ref,
        blink::WGC3Duint mask) override;
    void stencilFuncSeparate(blink::WGC3Denum face,
        blink::WGC3Denum func,
        blink::WGC3Dint ref,
        blink::WGC3Duint mask) override;
    void stencilMask(blink::WGC3Duint mask) override;
    void stencilMaskSeparate(blink::WGC3Denum face,
        blink::WGC3Duint mask) override;
    void stencilOp(blink::WGC3Denum fail,
        blink::WGC3Denum zfail,
        blink::WGC3Denum zpass) override;
    void stencilOpSeparate(blink::WGC3Denum face,
        blink::WGC3Denum fail,
        blink::WGC3Denum zfail,
        blink::WGC3Denum zpass) override;

    void texImage2D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Denum internalformat,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Dint border,
        blink::WGC3Denum format,
        blink::WGC3Denum type,
        const void* pixels) override;

    void texParameterf(blink::WGC3Denum target,
        blink::WGC3Denum pname,
        blink::WGC3Dfloat param) override;
    void texParameteri(blink::WGC3Denum target,
        blink::WGC3Denum pname,
        blink::WGC3Dint param) override;

    void texSubImage2D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Dint xoffset,
        blink::WGC3Dint yoffset,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Denum format,
        blink::WGC3Denum type,
        const void* pixels) override;

    void uniform1f(blink::WGC3Dint location, blink::WGC3Dfloat x) override;
    void uniform1fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Dfloat* v) override;
    void uniform1i(blink::WGC3Dint location, blink::WGC3Dint x) override;
    void uniform1iv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Dint* v) override;
    void uniform2f(blink::WGC3Dint location,
        blink::WGC3Dfloat x,
        blink::WGC3Dfloat y) override;
    void uniform2fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Dfloat* v) override;
    void uniform2i(blink::WGC3Dint location,
        blink::WGC3Dint x,
        blink::WGC3Dint y) override;
    void uniform2iv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Dint* v) override;
    void uniform3f(blink::WGC3Dint location,
        blink::WGC3Dfloat x,
        blink::WGC3Dfloat y,
        blink::WGC3Dfloat z) override;
    void uniform3fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Dfloat* v) override;
    void uniform3i(blink::WGC3Dint location,
        blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dint z) override;
    void uniform3iv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Dint* v) override;
    void uniform4f(blink::WGC3Dint location,
        blink::WGC3Dfloat x,
        blink::WGC3Dfloat y,
        blink::WGC3Dfloat z,
        blink::WGC3Dfloat w) override;
    void uniform4fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Dfloat* v) override;
    void uniform4i(blink::WGC3Dint location,
        blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dint z,
        blink::WGC3Dint w) override;
    void uniform4iv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Dint* v) override;
    void uniformMatrix2fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        blink::WGC3Dboolean transpose,
        const blink::WGC3Dfloat* value) override;
    void uniformMatrix3fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        blink::WGC3Dboolean transpose,
        const blink::WGC3Dfloat* value) override;
    void uniformMatrix4fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        blink::WGC3Dboolean transpose,
        const blink::WGC3Dfloat* value) override;

    void useProgram(blink::WebGLId program) override;
    void validateProgram(blink::WebGLId program) override;

    void vertexAttrib1f(blink::WGC3Duint index, blink::WGC3Dfloat x) override;
    void vertexAttrib1fv(blink::WGC3Duint index,
        const blink::WGC3Dfloat* values) override;
    void vertexAttrib2f(blink::WGC3Duint index,
        blink::WGC3Dfloat x,
        blink::WGC3Dfloat y) override;
    void vertexAttrib2fv(blink::WGC3Duint index,
        const blink::WGC3Dfloat* values) override;
    void vertexAttrib3f(blink::WGC3Duint index,
        blink::WGC3Dfloat x,
        blink::WGC3Dfloat y,
        blink::WGC3Dfloat z) override;
    void vertexAttrib3fv(blink::WGC3Duint index,
        const blink::WGC3Dfloat* values) override;
    void vertexAttrib4f(blink::WGC3Duint index,
        blink::WGC3Dfloat x,
        blink::WGC3Dfloat y,
        blink::WGC3Dfloat z,
        blink::WGC3Dfloat w) override;
    void vertexAttrib4fv(blink::WGC3Duint index,
        const blink::WGC3Dfloat* values) override;
    void vertexAttribPointer(blink::WGC3Duint index,
        blink::WGC3Dint size,
        blink::WGC3Denum type,
        blink::WGC3Dboolean normalized,
        blink::WGC3Dsizei stride,
        blink::WGC3Dintptr offset) override;

    void viewport(blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height) override;

    blink::WebGLId createBuffer() override;
    blink::WebGLId createFramebuffer() override;
    blink::WebGLId createRenderbuffer() override;
    blink::WebGLId createTexture() override;

    void deleteBuffer(blink::WebGLId) override;
    void deleteFramebuffer(blink::WebGLId) override;
    void deleteRenderbuffer(blink::WebGLId) override;
    void deleteTexture(blink::WebGLId) override;

    blink::WebGLId createProgram() override;
    blink::WebGLId createShader(blink::WGC3Denum) override;

    void deleteProgram(blink::WebGLId) override;
    void deleteShader(blink::WebGLId) override;

    void synthesizeGLError(blink::WGC3Denum) override;

    void* mapBufferSubDataCHROMIUM(blink::WGC3Denum target,
        blink::WGC3Dintptr offset,
        blink::WGC3Dsizeiptr size,
        blink::WGC3Denum access) override;
    void unmapBufferSubDataCHROMIUM(const void*) override;
    void* mapTexSubImage2DCHROMIUM(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Dint xoffset,
        blink::WGC3Dint yoffset,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Denum format,
        blink::WGC3Denum type,
        blink::WGC3Denum access) override;
    void unmapTexSubImage2DCHROMIUM(const void*) override;

    void setVisibilityCHROMIUM(bool visible) override;

    void discardFramebufferEXT(blink::WGC3Denum target,
        blink::WGC3Dsizei numAttachments,
        const blink::WGC3Denum* attachments) override;
    virtual void copyTextureToParentTextureCHROMIUM(blink::WebGLId texture,
        blink::WebGLId parentTexture);

    blink::WebString getRequestableExtensionsCHROMIUM() override;
    void requestExtensionCHROMIUM(const char*) override;

    void blitFramebufferCHROMIUM(blink::WGC3Dint srcX0,
        blink::WGC3Dint srcY0,
        blink::WGC3Dint srcX1,
        blink::WGC3Dint srcY1,
        blink::WGC3Dint dstX0,
        blink::WGC3Dint dstY0,
        blink::WGC3Dint dstX1,
        blink::WGC3Dint dstY1,
        blink::WGC3Dbitfield mask,
        blink::WGC3Denum filter) override;
    void renderbufferStorageMultisampleCHROMIUM(
        blink::WGC3Denum target,
        blink::WGC3Dsizei samples,
        blink::WGC3Denum internalformat,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height) override;

    blink::WebString getTranslatedShaderSourceANGLE(
        blink::WebGLId shader) override;

    void setContextLostCallback(
        WebGraphicsContext3D::WebGraphicsContextLostCallback* callback) override;

    void setErrorMessageCallback(
        WebGraphicsContext3D::WebGraphicsErrorMessageCallback* callback) override;

    void texImageIOSurface2DCHROMIUM(blink::WGC3Denum target,
        blink::WGC3Dint width,
        blink::WGC3Dint height,
        blink::WGC3Duint ioSurfaceId,
        blink::WGC3Duint plane) override;

    void texStorage2DEXT(blink::WGC3Denum target,
        blink::WGC3Dint levels,
        blink::WGC3Duint internalformat,
        blink::WGC3Dint width,
        blink::WGC3Dint height) override;

    blink::WebGLId createQueryEXT() override;
    void deleteQueryEXT(blink::WebGLId query) override;
    blink::WGC3Dboolean isQueryEXT(blink::WGC3Duint query) override;
    void beginQueryEXT(blink::WGC3Denum target, blink::WebGLId query) override;
    void endQueryEXT(blink::WGC3Denum target) override;
    void getQueryivEXT(blink::WGC3Denum target,
        blink::WGC3Denum pname,
        blink::WGC3Dint* params) override;
    void getQueryObjectuivEXT(blink::WebGLId query,
        blink::WGC3Denum pname,
        blink::WGC3Duint* params) override;

    void queryCounterEXT(blink::WebGLId query, blink::WGC3Denum target) override;
    void getQueryObjectui64vEXT(blink::WebGLId query,
        blink::WGC3Denum pname,
        blink::WGC3Duint64* params) override;

    void copyTextureCHROMIUM(
        blink::WGC3Denum target,
        blink::WebGLId source_id,
        blink::WebGLId dest_id,
        blink::WGC3Denum internal_format,
        blink::WGC3Denum dest_type,
        blink::WGC3Dboolean unpack_flip_y,
        blink::WGC3Dboolean unpack_premultiply_alpha,
        blink::WGC3Dboolean unpack_unmultiply_alpha) override;

    void copySubTextureCHROMIUM(
        blink::WGC3Denum target,
        blink::WebGLId source_id,
        blink::WebGLId dest_id,
        blink::WGC3Dint xoffset,
        blink::WGC3Dint yoffset,
        blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Dboolean unpack_flip_y,
        blink::WGC3Dboolean unpack_premultiply_alpha,
        blink::WGC3Dboolean unpack_unmultiply_alpha) override;

    void bindUniformLocationCHROMIUM(blink::WebGLId program,
        blink::WGC3Dint location,
        const blink::WGC3Dchar* uniform) override;

    void shallowFlushCHROMIUM() override;
    void shallowFinishCHROMIUM() override;

    void genMailboxCHROMIUM(blink::WGC3Dbyte* mailbox) override;
    void produceTextureCHROMIUM(blink::WGC3Denum target,
        const blink::WGC3Dbyte* mailbox) override;
    void produceTextureDirectCHROMIUM(blink::WebGLId texture,
        blink::WGC3Denum target,
        const blink::WGC3Dbyte* mailbox) override;
    void consumeTextureCHROMIUM(blink::WGC3Denum target,
        const blink::WGC3Dbyte* mailbox) override;
    blink::WebGLId createAndConsumeTextureCHROMIUM(
        blink::WGC3Denum target,
        const blink::WGC3Dbyte* mailbox) override;

    void genValuebuffersCHROMIUM(blink::WGC3Dsizei count,
        blink::WebGLId* ids) override;
    blink::WebGLId createValuebufferCHROMIUM() override;
    void deleteValuebuffersCHROMIUM(blink::WGC3Dsizei count,
        blink::WebGLId* ids) override;
    void deleteValuebufferCHROMIUM(blink::WebGLId) override;
    void bindValuebufferCHROMIUM(blink::WGC3Denum target,
        blink::WebGLId valuebuffer) override;
    blink::WGC3Dboolean isValuebufferCHROMIUM(
        blink::WebGLId renderbuffer) override;
    void subscribeValueCHROMIUM(blink::WGC3Denum target,
        blink::WGC3Denum subscription) override;
    void populateSubscribedValuesCHROMIUM(blink::WGC3Denum target) override;
    void uniformValuebufferCHROMIUM(blink::WGC3Dint location,
        blink::WGC3Denum target,
        blink::WGC3Denum subscription) override;
    void traceBeginCHROMIUM(const blink::WGC3Dchar* category_name,
        const blink::WGC3Dchar* trace_name) override;
    void traceEndCHROMIUM() override;

    void insertEventMarkerEXT(const blink::WGC3Dchar* marker) override;
    void pushGroupMarkerEXT(const blink::WGC3Dchar* marker) override;
    void popGroupMarkerEXT() override;

    // GL_OES_vertex_array_object
    blink::WebGLId createVertexArrayOES() override;
    void deleteVertexArrayOES(blink::WebGLId array) override;
    blink::WGC3Dboolean isVertexArrayOES(blink::WebGLId array) override;
    void bindVertexArrayOES(blink::WebGLId array) override;

    void bindTexImage2DCHROMIUM(blink::WGC3Denum target,
        blink::WGC3Dint image_id) override;
    void releaseTexImage2DCHROMIUM(blink::WGC3Denum target,
        blink::WGC3Dint image_id) override;

    void* mapBufferCHROMIUM(blink::WGC3Denum target,
        blink::WGC3Denum access) override;
    blink::WGC3Dboolean unmapBufferCHROMIUM(blink::WGC3Denum target) override;

    // GL_EXT_draw_buffers
    void drawBuffersEXT(blink::WGC3Dsizei n,
        const blink::WGC3Denum* bufs) override;

    // GL_ANGLE_instanced_arrays
    void drawArraysInstancedANGLE(blink::WGC3Denum mode,
        blink::WGC3Dint first,
        blink::WGC3Dsizei count,
        blink::WGC3Dsizei primcount) override;
    void drawElementsInstancedANGLE(blink::WGC3Denum mode,
        blink::WGC3Dsizei count,
        blink::WGC3Denum type,
        blink::WGC3Dintptr offset,
        blink::WGC3Dsizei primcount) override;
    void vertexAttribDivisorANGLE(blink::WGC3Duint index,
        blink::WGC3Duint divisor) override;

    // GL_CHROMIUM_gpu_memory_buffer_image
    blink::WGC3Duint createGpuMemoryBufferImageCHROMIUM(
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Denum internalformat,
        blink::WGC3Denum usage) override;
    void destroyImageCHROMIUM(blink::WGC3Duint image_id) override;

    // GL_EXT_multisampled_render_to_texture
    void framebufferTexture2DMultisampleEXT(blink::WGC3Denum target,
        blink::WGC3Denum attachment,
        blink::WGC3Denum textarget,
        blink::WebGLId texture,
        blink::WGC3Dint level,
        blink::WGC3Dsizei samples) override;
    void renderbufferStorageMultisampleEXT(
        blink::WGC3Denum target,
        blink::WGC3Dsizei samples,
        blink::WGC3Denum internalformat,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height) override;

    // OpenGL ES 3.0 functions not represented by pre-existing extensions
    void beginTransformFeedback(blink::WGC3Denum primitiveMode) override;
    void bindBufferBase(blink::WGC3Denum target,
        blink::WGC3Duint index,
        blink::WGC3Duint buffer) override;
    void bindBufferRange(blink::WGC3Denum target,
        blink::WGC3Duint index,
        blink::WGC3Duint buffer,
        blink::WGC3Dintptr offset,
        blink::WGC3Dsizeiptr size) override;
    void bindSampler(blink::WGC3Duint unit, blink::WebGLId sampler) override;
    void bindTransformFeedback(blink::WGC3Denum target,
        blink::WebGLId transformfeedback) override;
    void clearBufferfi(blink::WGC3Denum buffer,
        blink::WGC3Dint drawbuffer,
        blink::WGC3Dfloat depth,
        blink::WGC3Dint stencil) override;
    void clearBufferfv(blink::WGC3Denum buffer,
        blink::WGC3Dint drawbuffer,
        const blink::WGC3Dfloat* value) override;
    void clearBufferiv(blink::WGC3Denum buffer,
        blink::WGC3Dint drawbuffer,
        const blink::WGC3Dint* value) override;
    void clearBufferuiv(blink::WGC3Denum buffer,
        blink::WGC3Dint drawbuffer,
        const blink::WGC3Duint* value) override;
    blink::WGC3Denum clientWaitSync(blink::WGC3Dsync sync,
        blink::WGC3Dbitfield flags,
        blink::WGC3Duint64 timeout) override;
    void compressedTexImage3D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Denum internalformat,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Dsizei depth,
        blink::WGC3Dint border,
        blink::WGC3Dsizei imageSize,
        const void* data) override;
    void compressedTexSubImage3D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Dint xoffset,
        blink::WGC3Dint yoffset,
        blink::WGC3Dint zoffset,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Dsizei depth,
        blink::WGC3Denum format,
        blink::WGC3Dsizei imageSize,
        const void* data) override;
    void copyBufferSubData(blink::WGC3Denum readTarget,
        blink::WGC3Denum writeTarget,
        blink::WGC3Dintptr readOffset,
        blink::WGC3Dintptr writeOffset,
        blink::WGC3Dsizeiptr size) override;
    void copyTexSubImage3D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Dint xoffset,
        blink::WGC3Dint yoffset,
        blink::WGC3Dint zoffset,
        blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height) override;
    blink::WebGLId createSampler() override;
    blink::WebGLId createTransformFeedback() override;
    void deleteSampler(blink::WebGLId sampler) override;
    void deleteSync(blink::WGC3Dsync sync) override;
    void deleteTransformFeedback(blink::WebGLId transformfeedback) override;
    void drawRangeElements(blink::WGC3Denum mode,
        blink::WGC3Duint start,
        blink::WGC3Duint end,
        blink::WGC3Dsizei count,
        blink::WGC3Denum type,
        blink::WGC3Dintptr offset) override;
    void endTransformFeedback(void) override;
    blink::WGC3Dsync fenceSync(blink::WGC3Denum condition,
        blink::WGC3Dbitfield flags) override;
    void framebufferTextureLayer(blink::WGC3Denum target,
        blink::WGC3Denum attachment,
        blink::WGC3Duint texture,
        blink::WGC3Dint level,
        blink::WGC3Dint layer) override;
    void getActiveUniformBlockName(blink::WGC3Duint program,
        blink::WGC3Duint uniformBlockIndex,
        blink::WGC3Dsizei bufSize,
        blink::WGC3Dsizei* length,
        blink::WGC3Dchar* uniformBlockName) override;
    void getActiveUniformBlockiv(blink::WGC3Duint program,
        blink::WGC3Duint uniformBlockIndex,
        blink::WGC3Denum pname,
        blink::WGC3Dint* params) override;
    void getActiveUniformsiv(blink::WGC3Duint program,
        blink::WGC3Dsizei uniformCount,
        const blink::WGC3Duint* uniformIndices,
        blink::WGC3Denum pname,
        blink::WGC3Dint* params) override;
    blink::WGC3Dint getFragDataLocation(blink::WGC3Duint program,
        const blink::WGC3Dchar* name) override;
    void getInternalformativ(blink::WGC3Denum target,
        blink::WGC3Denum internalformat,
        blink::WGC3Denum pname,
        blink::WGC3Dsizei bufSize,
        blink::WGC3Dint* params) override;
    void getSamplerParameterfv(blink::WGC3Duint sampler,
        blink::WGC3Denum pname,
        blink::WGC3Dfloat* params) override;
    void getSamplerParameteriv(blink::WGC3Duint sampler,
        blink::WGC3Denum pname,
        blink::WGC3Dint* params) override;
    void getTransformFeedbackVarying(blink::WGC3Duint program,
        blink::WGC3Duint index,
        blink::WGC3Dsizei bufSize,
        blink::WGC3Dsizei* length,
        blink::WGC3Dsizei* size,
        blink::WGC3Denum* type,
        blink::WGC3Dchar* name) override;
    blink::WGC3Duint getUniformBlockIndex(
        blink::WGC3Duint program,
        const blink::WGC3Dchar* uniformBlockName) override;
    void getUniformIndices(blink::WGC3Duint program,
        blink::WGC3Dsizei uniformCount,
        const blink::WGC3Dchar* const* uniformNames,
        blink::WGC3Duint* uniformIndices) override;
    void getUniformuiv(blink::WGC3Duint program,
        blink::WGC3Dint location,
        blink::WGC3Duint* params) override;
    void getVertexAttribIiv(blink::WGC3Duint index,
        blink::WGC3Denum pname,
        blink::WGC3Dint* params) override;
    void getVertexAttribIuiv(blink::WGC3Duint index,
        blink::WGC3Denum pname,
        blink::WGC3Duint* params) override;
    void invalidateFramebuffer(blink::WGC3Denum target,
        blink::WGC3Dsizei numAttachments,
        const blink::WGC3Denum* attachments) override;
    void invalidateSubFramebuffer(blink::WGC3Denum target,
        blink::WGC3Dsizei numAttachments,
        const blink::WGC3Denum* attachments,
        blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height) override;
    blink::WGC3Dboolean isSampler(blink::WebGLId sampler) override;
    blink::WGC3Dboolean isSync(blink::WGC3Dsync sync) override;
    blink::WGC3Dboolean isTransformFeedback(blink::WGC3Duint id) override;
    void* mapBufferRange(blink::WGC3Denum target,
        blink::WGC3Dintptr offset,
        blink::WGC3Dsizeiptr length,
        blink::WGC3Dbitfield access) override;
    void pauseTransformFeedback(void) override;
    //void programParameteri(blink::WGC3Duint program,
    //                       blink::WGC3Denum pname,
    //                       blink::WGC3Dint value) override;
    void readBuffer(blink::WGC3Denum src) override;
    void resumeTransformFeedback(void) override;
    void samplerParameterf(blink::WGC3Duint sampler,
        blink::WGC3Denum pname,
        blink::WGC3Dfloat param) override;
    void samplerParameterfv(blink::WGC3Duint sampler,
        blink::WGC3Denum pname,
        const blink::WGC3Dfloat* param) override;
    void samplerParameteri(blink::WGC3Duint sampler,
        blink::WGC3Denum pname,
        blink::WGC3Dint param) override;
    void samplerParameteriv(blink::WGC3Duint sampler,
        blink::WGC3Denum pname,
        const blink::WGC3Dint* param) override;
    void texImage3D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Dint internalformat,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Dsizei depth,
        blink::WGC3Dint border,
        blink::WGC3Denum format,
        blink::WGC3Denum type,
        const void* pixels) override;
    void texStorage3D(blink::WGC3Denum target,
        blink::WGC3Dsizei levels,
        blink::WGC3Denum internalformat,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Dsizei depth) override;
    void texSubImage3D(blink::WGC3Denum target,
        blink::WGC3Dint level,
        blink::WGC3Dint xoffset,
        blink::WGC3Dint yoffset,
        blink::WGC3Dint zoffset,
        blink::WGC3Dsizei width,
        blink::WGC3Dsizei height,
        blink::WGC3Dsizei depth,
        blink::WGC3Denum format,
        blink::WGC3Denum type,
        const void* pixels) override;
    void transformFeedbackVaryings(
        blink::WGC3Duint program,
        blink::WGC3Dsizei count,
        const blink::WGC3Dchar* const* varyings,
        blink::WGC3Denum bufferMode) override;
    void uniform1ui(blink::WGC3Dint location, blink::WGC3Duint x) override;
    void uniform1uiv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Duint* value) override;
    void uniform2ui(blink::WGC3Dint location,
        blink::WGC3Duint x,
        blink::WGC3Duint y) override;
    void uniform2uiv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Duint* value) override;
    void uniform3ui(blink::WGC3Dint location,
        blink::WGC3Duint x,
        blink::WGC3Duint y,
        blink::WGC3Duint z) override;
    void uniform3uiv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Duint* value) override;
    void uniform4ui(blink::WGC3Dint location,
        blink::WGC3Duint x,
        blink::WGC3Duint y,
        blink::WGC3Duint z,
        blink::WGC3Duint w) override;
    void uniform4uiv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        const blink::WGC3Duint* value) override;
    void uniformBlockBinding(blink::WGC3Duint program,
        blink::WGC3Duint uniformBlockIndex,
        blink::WGC3Duint uniformBlockBinding) override;
    void uniformMatrix2x3fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        blink::WGC3Dboolean transpose,
        const blink::WGC3Dfloat* value) override;
    void uniformMatrix2x4fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        blink::WGC3Dboolean transpose,
        const blink::WGC3Dfloat* value) override;
    void uniformMatrix3x2fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        blink::WGC3Dboolean transpose,
        const blink::WGC3Dfloat* value) override;
    void uniformMatrix3x4fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        blink::WGC3Dboolean transpose,
        const blink::WGC3Dfloat* value) override;
    void uniformMatrix4x2fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        blink::WGC3Dboolean transpose,
        const blink::WGC3Dfloat* value) override;
    void uniformMatrix4x3fv(blink::WGC3Dint location,
        blink::WGC3Dsizei count,
        blink::WGC3Dboolean transpose,
        const blink::WGC3Dfloat* value) override;
    blink::WGC3Dboolean unmapBuffer(blink::WGC3Denum target) override;
    void vertexAttribI4i(blink::WGC3Duint index,
        blink::WGC3Dint x,
        blink::WGC3Dint y,
        blink::WGC3Dint z,
        blink::WGC3Dint w) override;
    void vertexAttribI4iv(blink::WGC3Duint index,
        const blink::WGC3Dint* v) override;
    void vertexAttribI4ui(blink::WGC3Duint index,
        blink::WGC3Duint x,
        blink::WGC3Duint y,
        blink::WGC3Duint z,
        blink::WGC3Duint w) override;
    void vertexAttribI4uiv(blink::WGC3Duint index,
        const blink::WGC3Duint* v) override;
    void vertexAttribIPointer(blink::WGC3Duint index,
        blink::WGC3Dint size,
        blink::WGC3Denum type,
        blink::WGC3Dsizei stride,
        blink::WGC3Dintptr pointer) override;
    void waitSync(blink::WGC3Dsync sync,
        blink::WGC3Dbitfield flags,
        blink::WGC3Duint64 timeout) override;

    bool isContextLost() override;
    blink::WGC3Denum getGraphicsResetStatusARB() override;

    ::gpu::gles2::GLES2Interface* GetGLInterface()
    {
        return gl_;
    }

    // Convert WebGL context creation attributes into command buffer / EGL size
    // requests.
    static void ConvertAttributes(
        const blink::WebGraphicsContext3D::Attributes& attributes,
        ::gpu::gles2::ContextCreationAttribHelper* output_attribs);

protected:
    friend class WebGraphicsContext3DErrorMessageCallback;

    WebGraphicsContext3DImpl();

    ::gpu::gles2::GLES2ImplementationErrorMessageCallback*
    getErrorMessageCallback();
    virtual void OnErrorMessage(const std::string& message, int id);

    void setGLInterface(::gpu::gles2::GLES2Interface* gl)
    {
        gl_ = gl;
    }

    bool initialized_;
    bool initialize_failed_;

    WebGraphicsContext3D::WebGraphicsContextLostCallback* context_lost_callback_;

    WebGraphicsContext3D::WebGraphicsErrorMessageCallback*
        error_message_callback_;
    scoped_ptr<WebGraphicsContext3DErrorMessageCallback>
        client_error_message_callback_;

    // Errors raised by synthesizeGLError().
    std::vector<blink::WGC3Denum> synthetic_errors_;

    ::gpu::gles2::GLES2Interface* gl_;
    bool lose_context_when_out_of_memory_;
    uint32_t flush_id_;
};

} // namespace gpu_blink

#endif // GPU_BLINK_WEBGRAPHICSCONTEXT3D_IMPL_H_
