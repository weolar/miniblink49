// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

// This file is included by gles2_interface_stub.h.
#ifndef GPU_COMMAND_BUFFER_CLIENT_GLES2_INTERFACE_STUB_AUTOGEN_H_
#define GPU_COMMAND_BUFFER_CLIENT_GLES2_INTERFACE_STUB_AUTOGEN_H_

virtual void ActiveTexture(GLenum texture) OVERRIDE;
virtual void AttachShader(GLuint program, GLuint shader) OVERRIDE;
virtual void BindAttribLocation(GLuint program,
                                GLuint index,
                                const char* name) OVERRIDE;
virtual void BindBuffer(GLenum target, GLuint buffer) OVERRIDE;
virtual void BindFramebuffer(GLenum target, GLuint framebuffer) OVERRIDE;
virtual void BindRenderbuffer(GLenum target, GLuint renderbuffer) OVERRIDE;
virtual void BindTexture(GLenum target, GLuint texture) OVERRIDE;
virtual void BlendColor(GLclampf red,
                        GLclampf green,
                        GLclampf blue,
                        GLclampf alpha) OVERRIDE;
virtual void BlendEquation(GLenum mode) OVERRIDE;
virtual void BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) OVERRIDE;
virtual void BlendFunc(GLenum sfactor, GLenum dfactor) OVERRIDE;
virtual void BlendFuncSeparate(GLenum srcRGB,
                               GLenum dstRGB,
                               GLenum srcAlpha,
                               GLenum dstAlpha) OVERRIDE;
virtual void BufferData(GLenum target,
                        GLsizeiptr size,
                        const void* data,
                        GLenum usage) OVERRIDE;
virtual void BufferSubData(GLenum target,
                           GLintptr offset,
                           GLsizeiptr size,
                           const void* data) OVERRIDE;
virtual GLenum CheckFramebufferStatus(GLenum target) OVERRIDE;
virtual void Clear(GLbitfield mask) OVERRIDE;
virtual void ClearColor(GLclampf red,
                        GLclampf green,
                        GLclampf blue,
                        GLclampf alpha) OVERRIDE;
virtual void ClearDepthf(GLclampf depth) OVERRIDE;
virtual void ClearStencil(GLint s) OVERRIDE;
virtual void ColorMask(GLboolean red,
                       GLboolean green,
                       GLboolean blue,
                       GLboolean alpha) OVERRIDE;
virtual void CompileShader(GLuint shader) OVERRIDE;
virtual void CompressedTexImage2D(GLenum target,
                                  GLint level,
                                  GLenum internalformat,
                                  GLsizei width,
                                  GLsizei height,
                                  GLint border,
                                  GLsizei imageSize,
                                  const void* data) OVERRIDE;
virtual void CompressedTexSubImage2D(GLenum target,
                                     GLint level,
                                     GLint xoffset,
                                     GLint yoffset,
                                     GLsizei width,
                                     GLsizei height,
                                     GLenum format,
                                     GLsizei imageSize,
                                     const void* data) OVERRIDE;
virtual void CopyTexImage2D(GLenum target,
                            GLint level,
                            GLenum internalformat,
                            GLint x,
                            GLint y,
                            GLsizei width,
                            GLsizei height,
                            GLint border) OVERRIDE;
virtual void CopyTexSubImage2D(GLenum target,
                               GLint level,
                               GLint xoffset,
                               GLint yoffset,
                               GLint x,
                               GLint y,
                               GLsizei width,
                               GLsizei height) OVERRIDE;
virtual GLuint CreateProgram() OVERRIDE;
virtual GLuint CreateShader(GLenum type) OVERRIDE;
virtual void CullFace(GLenum mode) OVERRIDE;
virtual void DeleteBuffers(GLsizei n, const GLuint* buffers) OVERRIDE;
virtual void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers) OVERRIDE;
virtual void DeleteProgram(GLuint program) OVERRIDE;
virtual void DeleteRenderbuffers(GLsizei n,
                                 const GLuint* renderbuffers) OVERRIDE;
virtual void DeleteShader(GLuint shader) OVERRIDE;
virtual void DeleteTextures(GLsizei n, const GLuint* textures) OVERRIDE;
virtual void DepthFunc(GLenum func) OVERRIDE;
virtual void DepthMask(GLboolean flag) OVERRIDE;
virtual void DepthRangef(GLclampf zNear, GLclampf zFar) OVERRIDE;
virtual void DetachShader(GLuint program, GLuint shader) OVERRIDE;
virtual void Disable(GLenum cap) OVERRIDE;
virtual void DisableVertexAttribArray(GLuint index) OVERRIDE;
virtual void DrawArrays(GLenum mode, GLint first, GLsizei count) OVERRIDE;
virtual void DrawElements(GLenum mode,
                          GLsizei count,
                          GLenum type,
                          const void* indices) OVERRIDE;
virtual void Enable(GLenum cap) OVERRIDE;
virtual void EnableVertexAttribArray(GLuint index) OVERRIDE;
virtual void Finish() OVERRIDE;
virtual void Flush() OVERRIDE;
virtual void FramebufferRenderbuffer(GLenum target,
                                     GLenum attachment,
                                     GLenum renderbuffertarget,
                                     GLuint renderbuffer) OVERRIDE;
virtual void FramebufferTexture2D(GLenum target,
                                  GLenum attachment,
                                  GLenum textarget,
                                  GLuint texture,
                                  GLint level) OVERRIDE;
virtual void FrontFace(GLenum mode) OVERRIDE;
virtual void GenBuffers(GLsizei n, GLuint* buffers) OVERRIDE;
virtual void GenerateMipmap(GLenum target) OVERRIDE;
virtual void GenFramebuffers(GLsizei n, GLuint* framebuffers) OVERRIDE;
virtual void GenRenderbuffers(GLsizei n, GLuint* renderbuffers) OVERRIDE;
virtual void GenTextures(GLsizei n, GLuint* textures) OVERRIDE;
virtual void GetActiveAttrib(GLuint program,
                             GLuint index,
                             GLsizei bufsize,
                             GLsizei* length,
                             GLint* size,
                             GLenum* type,
                             char* name) OVERRIDE;
virtual void GetActiveUniform(GLuint program,
                              GLuint index,
                              GLsizei bufsize,
                              GLsizei* length,
                              GLint* size,
                              GLenum* type,
                              char* name) OVERRIDE;
virtual void GetAttachedShaders(GLuint program,
                                GLsizei maxcount,
                                GLsizei* count,
                                GLuint* shaders) OVERRIDE;
virtual GLint GetAttribLocation(GLuint program, const char* name) OVERRIDE;
virtual void GetBooleanv(GLenum pname, GLboolean* params) OVERRIDE;
virtual void GetBufferParameteriv(GLenum target,
                                  GLenum pname,
                                  GLint* params) OVERRIDE;
virtual GLenum GetError() OVERRIDE;
virtual void GetFloatv(GLenum pname, GLfloat* params) OVERRIDE;
virtual void GetFramebufferAttachmentParameteriv(GLenum target,
                                                 GLenum attachment,
                                                 GLenum pname,
                                                 GLint* params) OVERRIDE;
virtual void GetIntegerv(GLenum pname, GLint* params) OVERRIDE;
virtual void GetProgramiv(GLuint program, GLenum pname, GLint* params) OVERRIDE;
virtual void GetProgramInfoLog(GLuint program,
                               GLsizei bufsize,
                               GLsizei* length,
                               char* infolog) OVERRIDE;
virtual void GetRenderbufferParameteriv(GLenum target,
                                        GLenum pname,
                                        GLint* params) OVERRIDE;
virtual void GetShaderiv(GLuint shader, GLenum pname, GLint* params) OVERRIDE;
virtual void GetShaderInfoLog(GLuint shader,
                              GLsizei bufsize,
                              GLsizei* length,
                              char* infolog) OVERRIDE;
virtual void GetShaderPrecisionFormat(GLenum shadertype,
                                      GLenum precisiontype,
                                      GLint* range,
                                      GLint* precision) OVERRIDE;
virtual void GetShaderSource(GLuint shader,
                             GLsizei bufsize,
                             GLsizei* length,
                             char* source) OVERRIDE;
virtual const GLubyte* GetString(GLenum name) OVERRIDE;
virtual void GetTexParameterfv(GLenum target,
                               GLenum pname,
                               GLfloat* params) OVERRIDE;
virtual void GetTexParameteriv(GLenum target,
                               GLenum pname,
                               GLint* params) OVERRIDE;
virtual void GetUniformfv(GLuint program,
                          GLint location,
                          GLfloat* params) OVERRIDE;
virtual void GetUniformiv(GLuint program,
                          GLint location,
                          GLint* params) OVERRIDE;
virtual GLint GetUniformLocation(GLuint program, const char* name) OVERRIDE;
virtual void GetVertexAttribfv(GLuint index,
                               GLenum pname,
                               GLfloat* params) OVERRIDE;
virtual void GetVertexAttribiv(GLuint index,
                               GLenum pname,
                               GLint* params) OVERRIDE;
virtual void GetVertexAttribPointerv(GLuint index,
                                     GLenum pname,
                                     void** pointer) OVERRIDE;
virtual void Hint(GLenum target, GLenum mode) OVERRIDE;
virtual GLboolean IsBuffer(GLuint buffer) OVERRIDE;
virtual GLboolean IsEnabled(GLenum cap) OVERRIDE;
virtual GLboolean IsFramebuffer(GLuint framebuffer) OVERRIDE;
virtual GLboolean IsProgram(GLuint program) OVERRIDE;
virtual GLboolean IsRenderbuffer(GLuint renderbuffer) OVERRIDE;
virtual GLboolean IsShader(GLuint shader) OVERRIDE;
virtual GLboolean IsTexture(GLuint texture) OVERRIDE;
virtual void LineWidth(GLfloat width) OVERRIDE;
virtual void LinkProgram(GLuint program) OVERRIDE;
virtual void PixelStorei(GLenum pname, GLint param) OVERRIDE;
virtual void PolygonOffset(GLfloat factor, GLfloat units) OVERRIDE;
virtual void ReadPixels(GLint x,
                        GLint y,
                        GLsizei width,
                        GLsizei height,
                        GLenum format,
                        GLenum type,
                        void* pixels) OVERRIDE;
virtual void ReleaseShaderCompiler() OVERRIDE;
virtual void RenderbufferStorage(GLenum target,
                                 GLenum internalformat,
                                 GLsizei width,
                                 GLsizei height) OVERRIDE;
virtual void SampleCoverage(GLclampf value, GLboolean invert) OVERRIDE;
virtual void Scissor(GLint x, GLint y, GLsizei width, GLsizei height) OVERRIDE;
virtual void ShaderBinary(GLsizei n,
                          const GLuint* shaders,
                          GLenum binaryformat,
                          const void* binary,
                          GLsizei length) OVERRIDE;
virtual void ShaderSource(GLuint shader,
                          GLsizei count,
                          const GLchar* const* str,
                          const GLint* length) OVERRIDE;
virtual void ShallowFinishCHROMIUM() OVERRIDE;
virtual void ShallowFlushCHROMIUM() OVERRIDE;
virtual void StencilFunc(GLenum func, GLint ref, GLuint mask) OVERRIDE;
virtual void StencilFuncSeparate(GLenum face,
                                 GLenum func,
                                 GLint ref,
                                 GLuint mask) OVERRIDE;
virtual void StencilMask(GLuint mask) OVERRIDE;
virtual void StencilMaskSeparate(GLenum face, GLuint mask) OVERRIDE;
virtual void StencilOp(GLenum fail, GLenum zfail, GLenum zpass) OVERRIDE;
virtual void StencilOpSeparate(GLenum face,
                               GLenum fail,
                               GLenum zfail,
                               GLenum zpass) OVERRIDE;
virtual void TexImage2D(GLenum target,
                        GLint level,
                        GLint internalformat,
                        GLsizei width,
                        GLsizei height,
                        GLint border,
                        GLenum format,
                        GLenum type,
                        const void* pixels) OVERRIDE;
virtual void TexParameterf(GLenum target, GLenum pname, GLfloat param) OVERRIDE;
virtual void TexParameterfv(GLenum target,
                            GLenum pname,
                            const GLfloat* params) OVERRIDE;
virtual void TexParameteri(GLenum target, GLenum pname, GLint param) OVERRIDE;
virtual void TexParameteriv(GLenum target,
                            GLenum pname,
                            const GLint* params) OVERRIDE;
virtual void TexSubImage2D(GLenum target,
                           GLint level,
                           GLint xoffset,
                           GLint yoffset,
                           GLsizei width,
                           GLsizei height,
                           GLenum format,
                           GLenum type,
                           const void* pixels) OVERRIDE;
virtual void Uniform1f(GLint location, GLfloat x) OVERRIDE;
virtual void Uniform1fv(GLint location,
                        GLsizei count,
                        const GLfloat* v) OVERRIDE;
virtual void Uniform1i(GLint location, GLint x) OVERRIDE;
virtual void Uniform1iv(GLint location, GLsizei count, const GLint* v) OVERRIDE;
virtual void Uniform2f(GLint location, GLfloat x, GLfloat y) OVERRIDE;
virtual void Uniform2fv(GLint location,
                        GLsizei count,
                        const GLfloat* v) OVERRIDE;
virtual void Uniform2i(GLint location, GLint x, GLint y) OVERRIDE;
virtual void Uniform2iv(GLint location, GLsizei count, const GLint* v) OVERRIDE;
virtual void Uniform3f(GLint location,
                       GLfloat x,
                       GLfloat y,
                       GLfloat z) OVERRIDE;
virtual void Uniform3fv(GLint location,
                        GLsizei count,
                        const GLfloat* v) OVERRIDE;
virtual void Uniform3i(GLint location, GLint x, GLint y, GLint z) OVERRIDE;
virtual void Uniform3iv(GLint location, GLsizei count, const GLint* v) OVERRIDE;
virtual void Uniform4f(GLint location,
                       GLfloat x,
                       GLfloat y,
                       GLfloat z,
                       GLfloat w) OVERRIDE;
virtual void Uniform4fv(GLint location,
                        GLsizei count,
                        const GLfloat* v) OVERRIDE;
virtual void Uniform4i(GLint location,
                       GLint x,
                       GLint y,
                       GLint z,
                       GLint w) OVERRIDE;
virtual void Uniform4iv(GLint location, GLsizei count, const GLint* v) OVERRIDE;
virtual void UniformMatrix2fv(GLint location,
                              GLsizei count,
                              GLboolean transpose,
                              const GLfloat* value) OVERRIDE;
virtual void UniformMatrix3fv(GLint location,
                              GLsizei count,
                              GLboolean transpose,
                              const GLfloat* value) OVERRIDE;
virtual void UniformMatrix4fv(GLint location,
                              GLsizei count,
                              GLboolean transpose,
                              const GLfloat* value) OVERRIDE;
virtual void UseProgram(GLuint program) OVERRIDE;
virtual void ValidateProgram(GLuint program) OVERRIDE;
virtual void VertexAttrib1f(GLuint indx, GLfloat x) OVERRIDE;
virtual void VertexAttrib1fv(GLuint indx, const GLfloat* values) OVERRIDE;
virtual void VertexAttrib2f(GLuint indx, GLfloat x, GLfloat y) OVERRIDE;
virtual void VertexAttrib2fv(GLuint indx, const GLfloat* values) OVERRIDE;
virtual void VertexAttrib3f(GLuint indx,
                            GLfloat x,
                            GLfloat y,
                            GLfloat z) OVERRIDE;
virtual void VertexAttrib3fv(GLuint indx, const GLfloat* values) OVERRIDE;
virtual void VertexAttrib4f(GLuint indx,
                            GLfloat x,
                            GLfloat y,
                            GLfloat z,
                            GLfloat w) OVERRIDE;
virtual void VertexAttrib4fv(GLuint indx, const GLfloat* values) OVERRIDE;
virtual void VertexAttribPointer(GLuint indx,
                                 GLint size,
                                 GLenum type,
                                 GLboolean normalized,
                                 GLsizei stride,
                                 const void* ptr) OVERRIDE;
virtual void Viewport(GLint x, GLint y, GLsizei width, GLsizei height) OVERRIDE;
virtual void BlitFramebufferCHROMIUM(GLint srcX0,
                                     GLint srcY0,
                                     GLint srcX1,
                                     GLint srcY1,
                                     GLint dstX0,
                                     GLint dstY0,
                                     GLint dstX1,
                                     GLint dstY1,
                                     GLbitfield mask,
                                     GLenum filter) OVERRIDE;
virtual void RenderbufferStorageMultisampleCHROMIUM(GLenum target,
                                                    GLsizei samples,
                                                    GLenum internalformat,
                                                    GLsizei width,
                                                    GLsizei height) OVERRIDE;
virtual void RenderbufferStorageMultisampleEXT(GLenum target,
                                               GLsizei samples,
                                               GLenum internalformat,
                                               GLsizei width,
                                               GLsizei height) OVERRIDE;
virtual void FramebufferTexture2DMultisampleEXT(GLenum target,
                                                GLenum attachment,
                                                GLenum textarget,
                                                GLuint texture,
                                                GLint level,
                                                GLsizei samples) OVERRIDE;
virtual void TexStorage2DEXT(GLenum target,
                             GLsizei levels,
                             GLenum internalFormat,
                             GLsizei width,
                             GLsizei height) OVERRIDE;
virtual void GenQueriesEXT(GLsizei n, GLuint* queries) OVERRIDE;
virtual void DeleteQueriesEXT(GLsizei n, const GLuint* queries) OVERRIDE;
virtual GLboolean IsQueryEXT(GLuint id) OVERRIDE;
virtual void BeginQueryEXT(GLenum target, GLuint id) OVERRIDE;
virtual void EndQueryEXT(GLenum target) OVERRIDE;
virtual void GetQueryivEXT(GLenum target, GLenum pname, GLint* params) OVERRIDE;
virtual void GetQueryObjectuivEXT(GLuint id,
                                  GLenum pname,
                                  GLuint* params) OVERRIDE;
virtual void InsertEventMarkerEXT(GLsizei length,
                                  const GLchar* marker) OVERRIDE;
virtual void PushGroupMarkerEXT(GLsizei length, const GLchar* marker) OVERRIDE;
virtual void PopGroupMarkerEXT() OVERRIDE;
virtual void GenVertexArraysOES(GLsizei n, GLuint* arrays) OVERRIDE;
virtual void DeleteVertexArraysOES(GLsizei n, const GLuint* arrays) OVERRIDE;
virtual GLboolean IsVertexArrayOES(GLuint array) OVERRIDE;
virtual void BindVertexArrayOES(GLuint array) OVERRIDE;
virtual void SwapBuffers() OVERRIDE;
virtual GLuint GetMaxValueInBufferCHROMIUM(GLuint buffer_id,
                                           GLsizei count,
                                           GLenum type,
                                           GLuint offset) OVERRIDE;
virtual void GenSharedIdsCHROMIUM(GLuint namespace_id,
                                  GLuint id_offset,
                                  GLsizei n,
                                  GLuint* ids) OVERRIDE;
virtual void DeleteSharedIdsCHROMIUM(GLuint namespace_id,
                                     GLsizei n,
                                     const GLuint* ids) OVERRIDE;
virtual void RegisterSharedIdsCHROMIUM(GLuint namespace_id,
                                       GLsizei n,
                                       const GLuint* ids) OVERRIDE;
virtual GLboolean EnableFeatureCHROMIUM(const char* feature) OVERRIDE;
virtual void* MapBufferCHROMIUM(GLuint target, GLenum access) OVERRIDE;
virtual GLboolean UnmapBufferCHROMIUM(GLuint target) OVERRIDE;
virtual void* MapImageCHROMIUM(GLuint image_id) OVERRIDE;
virtual void UnmapImageCHROMIUM(GLuint image_id) OVERRIDE;
virtual void* MapBufferSubDataCHROMIUM(GLuint target,
                                       GLintptr offset,
                                       GLsizeiptr size,
                                       GLenum access) OVERRIDE;
virtual void UnmapBufferSubDataCHROMIUM(const void* mem) OVERRIDE;
virtual void* MapTexSubImage2DCHROMIUM(GLenum target,
                                       GLint level,
                                       GLint xoffset,
                                       GLint yoffset,
                                       GLsizei width,
                                       GLsizei height,
                                       GLenum format,
                                       GLenum type,
                                       GLenum access) OVERRIDE;
virtual void UnmapTexSubImage2DCHROMIUM(const void* mem) OVERRIDE;
virtual void ResizeCHROMIUM(GLuint width,
                            GLuint height,
                            GLfloat scale_factor) OVERRIDE;
virtual const GLchar* GetRequestableExtensionsCHROMIUM() OVERRIDE;
virtual void RequestExtensionCHROMIUM(const char* extension) OVERRIDE;
virtual void RateLimitOffscreenContextCHROMIUM() OVERRIDE;
virtual void GetMultipleIntegervCHROMIUM(const GLenum* pnames,
                                         GLuint count,
                                         GLint* results,
                                         GLsizeiptr size) OVERRIDE;
virtual void GetProgramInfoCHROMIUM(GLuint program,
                                    GLsizei bufsize,
                                    GLsizei* size,
                                    void* info) OVERRIDE;
virtual GLuint CreateStreamTextureCHROMIUM(GLuint texture) OVERRIDE;
virtual GLuint CreateImageCHROMIUM(GLsizei width,
                                   GLsizei height,
                                   GLenum internalformat,
                                   GLenum usage) OVERRIDE;
virtual void DestroyImageCHROMIUM(GLuint image_id) OVERRIDE;
virtual void GetImageParameterivCHROMIUM(GLuint image_id,
                                         GLenum pname,
                                         GLint* params) OVERRIDE;
virtual void GetTranslatedShaderSourceANGLE(GLuint shader,
                                            GLsizei bufsize,
                                            GLsizei* length,
                                            char* source) OVERRIDE;
virtual void PostSubBufferCHROMIUM(GLint x,
                                   GLint y,
                                   GLint width,
                                   GLint height) OVERRIDE;
virtual void TexImageIOSurface2DCHROMIUM(GLenum target,
                                         GLsizei width,
                                         GLsizei height,
                                         GLuint ioSurfaceId,
                                         GLuint plane) OVERRIDE;
virtual void CopyTextureCHROMIUM(GLenum target,
                                 GLenum source_id,
                                 GLenum dest_id,
                                 GLint level,
                                 GLint internalformat,
                                 GLenum dest_type) OVERRIDE;
virtual void DrawArraysInstancedANGLE(GLenum mode,
                                      GLint first,
                                      GLsizei count,
                                      GLsizei primcount) OVERRIDE;
virtual void DrawElementsInstancedANGLE(GLenum mode,
                                        GLsizei count,
                                        GLenum type,
                                        const void* indices,
                                        GLsizei primcount) OVERRIDE;
virtual void VertexAttribDivisorANGLE(GLuint index, GLuint divisor) OVERRIDE;
virtual void GenMailboxCHROMIUM(GLbyte* mailbox) OVERRIDE;
virtual void ProduceTextureCHROMIUM(GLenum target,
                                    const GLbyte* mailbox) OVERRIDE;
virtual void ProduceTextureDirectCHROMIUM(GLuint texture,
                                          GLenum target,
                                          const GLbyte* mailbox) OVERRIDE;
virtual void ConsumeTextureCHROMIUM(GLenum target,
                                    const GLbyte* mailbox) OVERRIDE;
virtual GLuint CreateAndConsumeTextureCHROMIUM(GLenum target,
                                               const GLbyte* mailbox) OVERRIDE;
virtual void BindUniformLocationCHROMIUM(GLuint program,
                                         GLint location,
                                         const char* name) OVERRIDE;
virtual void BindTexImage2DCHROMIUM(GLenum target, GLint imageId) OVERRIDE;
virtual void ReleaseTexImage2DCHROMIUM(GLenum target, GLint imageId) OVERRIDE;
virtual void TraceBeginCHROMIUM(const char* name) OVERRIDE;
virtual void TraceEndCHROMIUM() OVERRIDE;
virtual void AsyncTexSubImage2DCHROMIUM(GLenum target,
                                        GLint level,
                                        GLint xoffset,
                                        GLint yoffset,
                                        GLsizei width,
                                        GLsizei height,
                                        GLenum format,
                                        GLenum type,
                                        const void* data) OVERRIDE;
virtual void AsyncTexImage2DCHROMIUM(GLenum target,
                                     GLint level,
                                     GLenum internalformat,
                                     GLsizei width,
                                     GLsizei height,
                                     GLint border,
                                     GLenum format,
                                     GLenum type,
                                     const void* pixels) OVERRIDE;
virtual void WaitAsyncTexImage2DCHROMIUM(GLenum target) OVERRIDE;
virtual void WaitAllAsyncTexImage2DCHROMIUM() OVERRIDE;
virtual void DiscardFramebufferEXT(GLenum target,
                                   GLsizei count,
                                   const GLenum* attachments) OVERRIDE;
virtual void LoseContextCHROMIUM(GLenum current, GLenum other) OVERRIDE;
virtual GLuint InsertSyncPointCHROMIUM() OVERRIDE;
virtual void WaitSyncPointCHROMIUM(GLuint sync_point) OVERRIDE;
virtual void DrawBuffersEXT(GLsizei count, const GLenum* bufs) OVERRIDE;
virtual void DiscardBackbufferCHROMIUM() OVERRIDE;
virtual void ScheduleOverlayPlaneCHROMIUM(GLint plane_z_order,
                                          GLenum plane_transform,
                                          GLuint overlay_texture_id,
                                          GLint bounds_x,
                                          GLint bounds_y,
                                          GLint bounds_width,
                                          GLint bounds_height,
                                          GLfloat uv_x,
                                          GLfloat uv_y,
                                          GLfloat uv_width,
                                          GLfloat uv_height) OVERRIDE;
#endif  // GPU_COMMAND_BUFFER_CLIENT_GLES2_INTERFACE_STUB_AUTOGEN_H_
