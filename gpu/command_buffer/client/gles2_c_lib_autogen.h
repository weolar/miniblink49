// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

// These functions emulate GLES2 over command buffers.
#ifndef GPU_COMMAND_BUFFER_CLIENT_GLES2_C_LIB_AUTOGEN_H_
#define GPU_COMMAND_BUFFER_CLIENT_GLES2_C_LIB_AUTOGEN_H_

void GLES2ActiveTexture(GLenum texture) {
  gles2::GetGLContext()->ActiveTexture(texture);
}
void GLES2AttachShader(GLuint program, GLuint shader) {
  gles2::GetGLContext()->AttachShader(program, shader);
}
void GLES2BindAttribLocation(GLuint program, GLuint index, const char* name) {
  gles2::GetGLContext()->BindAttribLocation(program, index, name);
}
void GLES2BindBuffer(GLenum target, GLuint buffer) {
  gles2::GetGLContext()->BindBuffer(target, buffer);
}
void GLES2BindFramebuffer(GLenum target, GLuint framebuffer) {
  gles2::GetGLContext()->BindFramebuffer(target, framebuffer);
}
void GLES2BindRenderbuffer(GLenum target, GLuint renderbuffer) {
  gles2::GetGLContext()->BindRenderbuffer(target, renderbuffer);
}
void GLES2BindTexture(GLenum target, GLuint texture) {
  gles2::GetGLContext()->BindTexture(target, texture);
}
void GLES2BlendColor(GLclampf red,
                     GLclampf green,
                     GLclampf blue,
                     GLclampf alpha) {
  gles2::GetGLContext()->BlendColor(red, green, blue, alpha);
}
void GLES2BlendEquation(GLenum mode) {
  gles2::GetGLContext()->BlendEquation(mode);
}
void GLES2BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) {
  gles2::GetGLContext()->BlendEquationSeparate(modeRGB, modeAlpha);
}
void GLES2BlendFunc(GLenum sfactor, GLenum dfactor) {
  gles2::GetGLContext()->BlendFunc(sfactor, dfactor);
}
void GLES2BlendFuncSeparate(GLenum srcRGB,
                            GLenum dstRGB,
                            GLenum srcAlpha,
                            GLenum dstAlpha) {
  gles2::GetGLContext()->BlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}
void GLES2BufferData(GLenum target,
                     GLsizeiptr size,
                     const void* data,
                     GLenum usage) {
  gles2::GetGLContext()->BufferData(target, size, data, usage);
}
void GLES2BufferSubData(GLenum target,
                        GLintptr offset,
                        GLsizeiptr size,
                        const void* data) {
  gles2::GetGLContext()->BufferSubData(target, offset, size, data);
}
GLenum GLES2CheckFramebufferStatus(GLenum target) {
  return gles2::GetGLContext()->CheckFramebufferStatus(target);
}
void GLES2Clear(GLbitfield mask) {
  gles2::GetGLContext()->Clear(mask);
}
void GLES2ClearColor(GLclampf red,
                     GLclampf green,
                     GLclampf blue,
                     GLclampf alpha) {
  gles2::GetGLContext()->ClearColor(red, green, blue, alpha);
}
void GLES2ClearDepthf(GLclampf depth) {
  gles2::GetGLContext()->ClearDepthf(depth);
}
void GLES2ClearStencil(GLint s) {
  gles2::GetGLContext()->ClearStencil(s);
}
void GLES2ColorMask(GLboolean red,
                    GLboolean green,
                    GLboolean blue,
                    GLboolean alpha) {
  gles2::GetGLContext()->ColorMask(red, green, blue, alpha);
}
void GLES2CompileShader(GLuint shader) {
  gles2::GetGLContext()->CompileShader(shader);
}
void GLES2CompressedTexImage2D(GLenum target,
                               GLint level,
                               GLenum internalformat,
                               GLsizei width,
                               GLsizei height,
                               GLint border,
                               GLsizei imageSize,
                               const void* data) {
  gles2::GetGLContext()->CompressedTexImage2D(
      target, level, internalformat, width, height, border, imageSize, data);
}
void GLES2CompressedTexSubImage2D(GLenum target,
                                  GLint level,
                                  GLint xoffset,
                                  GLint yoffset,
                                  GLsizei width,
                                  GLsizei height,
                                  GLenum format,
                                  GLsizei imageSize,
                                  const void* data) {
  gles2::GetGLContext()->CompressedTexSubImage2D(
      target, level, xoffset, yoffset, width, height, format, imageSize, data);
}
void GLES2CopyTexImage2D(GLenum target,
                         GLint level,
                         GLenum internalformat,
                         GLint x,
                         GLint y,
                         GLsizei width,
                         GLsizei height,
                         GLint border) {
  gles2::GetGLContext()->CopyTexImage2D(
      target, level, internalformat, x, y, width, height, border);
}
void GLES2CopyTexSubImage2D(GLenum target,
                            GLint level,
                            GLint xoffset,
                            GLint yoffset,
                            GLint x,
                            GLint y,
                            GLsizei width,
                            GLsizei height) {
  gles2::GetGLContext()->CopyTexSubImage2D(
      target, level, xoffset, yoffset, x, y, width, height);
}
GLuint GLES2CreateProgram() {
  return gles2::GetGLContext()->CreateProgram();
}
GLuint GLES2CreateShader(GLenum type) {
  return gles2::GetGLContext()->CreateShader(type);
}
void GLES2CullFace(GLenum mode) {
  gles2::GetGLContext()->CullFace(mode);
}
void GLES2DeleteBuffers(GLsizei n, const GLuint* buffers) {
  gles2::GetGLContext()->DeleteBuffers(n, buffers);
}
void GLES2DeleteFramebuffers(GLsizei n, const GLuint* framebuffers) {
  gles2::GetGLContext()->DeleteFramebuffers(n, framebuffers);
}
void GLES2DeleteProgram(GLuint program) {
  gles2::GetGLContext()->DeleteProgram(program);
}
void GLES2DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers) {
  gles2::GetGLContext()->DeleteRenderbuffers(n, renderbuffers);
}
void GLES2DeleteShader(GLuint shader) {
  gles2::GetGLContext()->DeleteShader(shader);
}
void GLES2DeleteTextures(GLsizei n, const GLuint* textures) {
  gles2::GetGLContext()->DeleteTextures(n, textures);
}
void GLES2DepthFunc(GLenum func) {
  gles2::GetGLContext()->DepthFunc(func);
}
void GLES2DepthMask(GLboolean flag) {
  gles2::GetGLContext()->DepthMask(flag);
}
void GLES2DepthRangef(GLclampf zNear, GLclampf zFar) {
  gles2::GetGLContext()->DepthRangef(zNear, zFar);
}
void GLES2DetachShader(GLuint program, GLuint shader) {
  gles2::GetGLContext()->DetachShader(program, shader);
}
void GLES2Disable(GLenum cap) {
  gles2::GetGLContext()->Disable(cap);
}
void GLES2DisableVertexAttribArray(GLuint index) {
  gles2::GetGLContext()->DisableVertexAttribArray(index);
}
void GLES2DrawArrays(GLenum mode, GLint first, GLsizei count) {
  gles2::GetGLContext()->DrawArrays(mode, first, count);
}
void GLES2DrawElements(GLenum mode,
                       GLsizei count,
                       GLenum type,
                       const void* indices) {
  gles2::GetGLContext()->DrawElements(mode, count, type, indices);
}
void GLES2Enable(GLenum cap) {
  gles2::GetGLContext()->Enable(cap);
}
void GLES2EnableVertexAttribArray(GLuint index) {
  gles2::GetGLContext()->EnableVertexAttribArray(index);
}
void GLES2Finish() {
  gles2::GetGLContext()->Finish();
}
void GLES2Flush() {
  gles2::GetGLContext()->Flush();
}
void GLES2FramebufferRenderbuffer(GLenum target,
                                  GLenum attachment,
                                  GLenum renderbuffertarget,
                                  GLuint renderbuffer) {
  gles2::GetGLContext()->FramebufferRenderbuffer(
      target, attachment, renderbuffertarget, renderbuffer);
}
void GLES2FramebufferTexture2D(GLenum target,
                               GLenum attachment,
                               GLenum textarget,
                               GLuint texture,
                               GLint level) {
  gles2::GetGLContext()->FramebufferTexture2D(
      target, attachment, textarget, texture, level);
}
void GLES2FrontFace(GLenum mode) {
  gles2::GetGLContext()->FrontFace(mode);
}
void GLES2GenBuffers(GLsizei n, GLuint* buffers) {
  gles2::GetGLContext()->GenBuffers(n, buffers);
}
void GLES2GenerateMipmap(GLenum target) {
  gles2::GetGLContext()->GenerateMipmap(target);
}
void GLES2GenFramebuffers(GLsizei n, GLuint* framebuffers) {
  gles2::GetGLContext()->GenFramebuffers(n, framebuffers);
}
void GLES2GenRenderbuffers(GLsizei n, GLuint* renderbuffers) {
  gles2::GetGLContext()->GenRenderbuffers(n, renderbuffers);
}
void GLES2GenTextures(GLsizei n, GLuint* textures) {
  gles2::GetGLContext()->GenTextures(n, textures);
}
void GLES2GetActiveAttrib(GLuint program,
                          GLuint index,
                          GLsizei bufsize,
                          GLsizei* length,
                          GLint* size,
                          GLenum* type,
                          char* name) {
  gles2::GetGLContext()->GetActiveAttrib(
      program, index, bufsize, length, size, type, name);
}
void GLES2GetActiveUniform(GLuint program,
                           GLuint index,
                           GLsizei bufsize,
                           GLsizei* length,
                           GLint* size,
                           GLenum* type,
                           char* name) {
  gles2::GetGLContext()->GetActiveUniform(
      program, index, bufsize, length, size, type, name);
}
void GLES2GetAttachedShaders(GLuint program,
                             GLsizei maxcount,
                             GLsizei* count,
                             GLuint* shaders) {
  gles2::GetGLContext()->GetAttachedShaders(program, maxcount, count, shaders);
}
GLint GLES2GetAttribLocation(GLuint program, const char* name) {
  return gles2::GetGLContext()->GetAttribLocation(program, name);
}
void GLES2GetBooleanv(GLenum pname, GLboolean* params) {
  gles2::GetGLContext()->GetBooleanv(pname, params);
}
void GLES2GetBufferParameteriv(GLenum target, GLenum pname, GLint* params) {
  gles2::GetGLContext()->GetBufferParameteriv(target, pname, params);
}
GLenum GLES2GetError() {
  return gles2::GetGLContext()->GetError();
}
void GLES2GetFloatv(GLenum pname, GLfloat* params) {
  gles2::GetGLContext()->GetFloatv(pname, params);
}
void GLES2GetFramebufferAttachmentParameteriv(GLenum target,
                                              GLenum attachment,
                                              GLenum pname,
                                              GLint* params) {
  gles2::GetGLContext()->GetFramebufferAttachmentParameteriv(
      target, attachment, pname, params);
}
void GLES2GetIntegerv(GLenum pname, GLint* params) {
  gles2::GetGLContext()->GetIntegerv(pname, params);
}
void GLES2GetProgramiv(GLuint program, GLenum pname, GLint* params) {
  gles2::GetGLContext()->GetProgramiv(program, pname, params);
}
void GLES2GetProgramInfoLog(GLuint program,
                            GLsizei bufsize,
                            GLsizei* length,
                            char* infolog) {
  gles2::GetGLContext()->GetProgramInfoLog(program, bufsize, length, infolog);
}
void GLES2GetRenderbufferParameteriv(GLenum target,
                                     GLenum pname,
                                     GLint* params) {
  gles2::GetGLContext()->GetRenderbufferParameteriv(target, pname, params);
}
void GLES2GetShaderiv(GLuint shader, GLenum pname, GLint* params) {
  gles2::GetGLContext()->GetShaderiv(shader, pname, params);
}
void GLES2GetShaderInfoLog(GLuint shader,
                           GLsizei bufsize,
                           GLsizei* length,
                           char* infolog) {
  gles2::GetGLContext()->GetShaderInfoLog(shader, bufsize, length, infolog);
}
void GLES2GetShaderPrecisionFormat(GLenum shadertype,
                                   GLenum precisiontype,
                                   GLint* range,
                                   GLint* precision) {
  gles2::GetGLContext()->GetShaderPrecisionFormat(
      shadertype, precisiontype, range, precision);
}
void GLES2GetShaderSource(GLuint shader,
                          GLsizei bufsize,
                          GLsizei* length,
                          char* source) {
  gles2::GetGLContext()->GetShaderSource(shader, bufsize, length, source);
}
const GLubyte* GLES2GetString(GLenum name) {
  return gles2::GetGLContext()->GetString(name);
}
void GLES2GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params) {
  gles2::GetGLContext()->GetTexParameterfv(target, pname, params);
}
void GLES2GetTexParameteriv(GLenum target, GLenum pname, GLint* params) {
  gles2::GetGLContext()->GetTexParameteriv(target, pname, params);
}
void GLES2GetUniformfv(GLuint program, GLint location, GLfloat* params) {
  gles2::GetGLContext()->GetUniformfv(program, location, params);
}
void GLES2GetUniformiv(GLuint program, GLint location, GLint* params) {
  gles2::GetGLContext()->GetUniformiv(program, location, params);
}
GLint GLES2GetUniformLocation(GLuint program, const char* name) {
  return gles2::GetGLContext()->GetUniformLocation(program, name);
}
void GLES2GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params) {
  gles2::GetGLContext()->GetVertexAttribfv(index, pname, params);
}
void GLES2GetVertexAttribiv(GLuint index, GLenum pname, GLint* params) {
  gles2::GetGLContext()->GetVertexAttribiv(index, pname, params);
}
void GLES2GetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer) {
  gles2::GetGLContext()->GetVertexAttribPointerv(index, pname, pointer);
}
void GLES2Hint(GLenum target, GLenum mode) {
  gles2::GetGLContext()->Hint(target, mode);
}
GLboolean GLES2IsBuffer(GLuint buffer) {
  return gles2::GetGLContext()->IsBuffer(buffer);
}
GLboolean GLES2IsEnabled(GLenum cap) {
  return gles2::GetGLContext()->IsEnabled(cap);
}
GLboolean GLES2IsFramebuffer(GLuint framebuffer) {
  return gles2::GetGLContext()->IsFramebuffer(framebuffer);
}
GLboolean GLES2IsProgram(GLuint program) {
  return gles2::GetGLContext()->IsProgram(program);
}
GLboolean GLES2IsRenderbuffer(GLuint renderbuffer) {
  return gles2::GetGLContext()->IsRenderbuffer(renderbuffer);
}
GLboolean GLES2IsShader(GLuint shader) {
  return gles2::GetGLContext()->IsShader(shader);
}
GLboolean GLES2IsTexture(GLuint texture) {
  return gles2::GetGLContext()->IsTexture(texture);
}
void GLES2LineWidth(GLfloat width) {
  gles2::GetGLContext()->LineWidth(width);
}
void GLES2LinkProgram(GLuint program) {
  gles2::GetGLContext()->LinkProgram(program);
}
void GLES2PixelStorei(GLenum pname, GLint param) {
  gles2::GetGLContext()->PixelStorei(pname, param);
}
void GLES2PolygonOffset(GLfloat factor, GLfloat units) {
  gles2::GetGLContext()->PolygonOffset(factor, units);
}
void GLES2ReadPixels(GLint x,
                     GLint y,
                     GLsizei width,
                     GLsizei height,
                     GLenum format,
                     GLenum type,
                     void* pixels) {
  gles2::GetGLContext()->ReadPixels(x, y, width, height, format, type, pixels);
}
void GLES2ReleaseShaderCompiler() {
  gles2::GetGLContext()->ReleaseShaderCompiler();
}
void GLES2RenderbufferStorage(GLenum target,
                              GLenum internalformat,
                              GLsizei width,
                              GLsizei height) {
  gles2::GetGLContext()->RenderbufferStorage(
      target, internalformat, width, height);
}
void GLES2SampleCoverage(GLclampf value, GLboolean invert) {
  gles2::GetGLContext()->SampleCoverage(value, invert);
}
void GLES2Scissor(GLint x, GLint y, GLsizei width, GLsizei height) {
  gles2::GetGLContext()->Scissor(x, y, width, height);
}
void GLES2ShaderBinary(GLsizei n,
                       const GLuint* shaders,
                       GLenum binaryformat,
                       const void* binary,
                       GLsizei length) {
  gles2::GetGLContext()->ShaderBinary(n, shaders, binaryformat, binary, length);
}
void GLES2ShaderSource(GLuint shader,
                       GLsizei count,
                       const GLchar* const* str,
                       const GLint* length) {
  gles2::GetGLContext()->ShaderSource(shader, count, str, length);
}
void GLES2ShallowFinishCHROMIUM() {
  gles2::GetGLContext()->ShallowFinishCHROMIUM();
}
void GLES2ShallowFlushCHROMIUM() {
  gles2::GetGLContext()->ShallowFlushCHROMIUM();
}
void GLES2StencilFunc(GLenum func, GLint ref, GLuint mask) {
  gles2::GetGLContext()->StencilFunc(func, ref, mask);
}
void GLES2StencilFuncSeparate(GLenum face,
                              GLenum func,
                              GLint ref,
                              GLuint mask) {
  gles2::GetGLContext()->StencilFuncSeparate(face, func, ref, mask);
}
void GLES2StencilMask(GLuint mask) {
  gles2::GetGLContext()->StencilMask(mask);
}
void GLES2StencilMaskSeparate(GLenum face, GLuint mask) {
  gles2::GetGLContext()->StencilMaskSeparate(face, mask);
}
void GLES2StencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
  gles2::GetGLContext()->StencilOp(fail, zfail, zpass);
}
void GLES2StencilOpSeparate(GLenum face,
                            GLenum fail,
                            GLenum zfail,
                            GLenum zpass) {
  gles2::GetGLContext()->StencilOpSeparate(face, fail, zfail, zpass);
}
void GLES2TexImage2D(GLenum target,
                     GLint level,
                     GLint internalformat,
                     GLsizei width,
                     GLsizei height,
                     GLint border,
                     GLenum format,
                     GLenum type,
                     const void* pixels) {
  gles2::GetGLContext()->TexImage2D(target,
                                    level,
                                    internalformat,
                                    width,
                                    height,
                                    border,
                                    format,
                                    type,
                                    pixels);
}
void GLES2TexParameterf(GLenum target, GLenum pname, GLfloat param) {
  gles2::GetGLContext()->TexParameterf(target, pname, param);
}
void GLES2TexParameterfv(GLenum target, GLenum pname, const GLfloat* params) {
  gles2::GetGLContext()->TexParameterfv(target, pname, params);
}
void GLES2TexParameteri(GLenum target, GLenum pname, GLint param) {
  gles2::GetGLContext()->TexParameteri(target, pname, param);
}
void GLES2TexParameteriv(GLenum target, GLenum pname, const GLint* params) {
  gles2::GetGLContext()->TexParameteriv(target, pname, params);
}
void GLES2TexSubImage2D(GLenum target,
                        GLint level,
                        GLint xoffset,
                        GLint yoffset,
                        GLsizei width,
                        GLsizei height,
                        GLenum format,
                        GLenum type,
                        const void* pixels) {
  gles2::GetGLContext()->TexSubImage2D(
      target, level, xoffset, yoffset, width, height, format, type, pixels);
}
void GLES2Uniform1f(GLint location, GLfloat x) {
  gles2::GetGLContext()->Uniform1f(location, x);
}
void GLES2Uniform1fv(GLint location, GLsizei count, const GLfloat* v) {
  gles2::GetGLContext()->Uniform1fv(location, count, v);
}
void GLES2Uniform1i(GLint location, GLint x) {
  gles2::GetGLContext()->Uniform1i(location, x);
}
void GLES2Uniform1iv(GLint location, GLsizei count, const GLint* v) {
  gles2::GetGLContext()->Uniform1iv(location, count, v);
}
void GLES2Uniform2f(GLint location, GLfloat x, GLfloat y) {
  gles2::GetGLContext()->Uniform2f(location, x, y);
}
void GLES2Uniform2fv(GLint location, GLsizei count, const GLfloat* v) {
  gles2::GetGLContext()->Uniform2fv(location, count, v);
}
void GLES2Uniform2i(GLint location, GLint x, GLint y) {
  gles2::GetGLContext()->Uniform2i(location, x, y);
}
void GLES2Uniform2iv(GLint location, GLsizei count, const GLint* v) {
  gles2::GetGLContext()->Uniform2iv(location, count, v);
}
void GLES2Uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z) {
  gles2::GetGLContext()->Uniform3f(location, x, y, z);
}
void GLES2Uniform3fv(GLint location, GLsizei count, const GLfloat* v) {
  gles2::GetGLContext()->Uniform3fv(location, count, v);
}
void GLES2Uniform3i(GLint location, GLint x, GLint y, GLint z) {
  gles2::GetGLContext()->Uniform3i(location, x, y, z);
}
void GLES2Uniform3iv(GLint location, GLsizei count, const GLint* v) {
  gles2::GetGLContext()->Uniform3iv(location, count, v);
}
void GLES2Uniform4f(GLint location,
                    GLfloat x,
                    GLfloat y,
                    GLfloat z,
                    GLfloat w) {
  gles2::GetGLContext()->Uniform4f(location, x, y, z, w);
}
void GLES2Uniform4fv(GLint location, GLsizei count, const GLfloat* v) {
  gles2::GetGLContext()->Uniform4fv(location, count, v);
}
void GLES2Uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w) {
  gles2::GetGLContext()->Uniform4i(location, x, y, z, w);
}
void GLES2Uniform4iv(GLint location, GLsizei count, const GLint* v) {
  gles2::GetGLContext()->Uniform4iv(location, count, v);
}
void GLES2UniformMatrix2fv(GLint location,
                           GLsizei count,
                           GLboolean transpose,
                           const GLfloat* value) {
  gles2::GetGLContext()->UniformMatrix2fv(location, count, transpose, value);
}
void GLES2UniformMatrix3fv(GLint location,
                           GLsizei count,
                           GLboolean transpose,
                           const GLfloat* value) {
  gles2::GetGLContext()->UniformMatrix3fv(location, count, transpose, value);
}
void GLES2UniformMatrix4fv(GLint location,
                           GLsizei count,
                           GLboolean transpose,
                           const GLfloat* value) {
  gles2::GetGLContext()->UniformMatrix4fv(location, count, transpose, value);
}
void GLES2UseProgram(GLuint program) {
  gles2::GetGLContext()->UseProgram(program);
}
void GLES2ValidateProgram(GLuint program) {
  gles2::GetGLContext()->ValidateProgram(program);
}
void GLES2VertexAttrib1f(GLuint indx, GLfloat x) {
  gles2::GetGLContext()->VertexAttrib1f(indx, x);
}
void GLES2VertexAttrib1fv(GLuint indx, const GLfloat* values) {
  gles2::GetGLContext()->VertexAttrib1fv(indx, values);
}
void GLES2VertexAttrib2f(GLuint indx, GLfloat x, GLfloat y) {
  gles2::GetGLContext()->VertexAttrib2f(indx, x, y);
}
void GLES2VertexAttrib2fv(GLuint indx, const GLfloat* values) {
  gles2::GetGLContext()->VertexAttrib2fv(indx, values);
}
void GLES2VertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z) {
  gles2::GetGLContext()->VertexAttrib3f(indx, x, y, z);
}
void GLES2VertexAttrib3fv(GLuint indx, const GLfloat* values) {
  gles2::GetGLContext()->VertexAttrib3fv(indx, values);
}
void GLES2VertexAttrib4f(GLuint indx,
                         GLfloat x,
                         GLfloat y,
                         GLfloat z,
                         GLfloat w) {
  gles2::GetGLContext()->VertexAttrib4f(indx, x, y, z, w);
}
void GLES2VertexAttrib4fv(GLuint indx, const GLfloat* values) {
  gles2::GetGLContext()->VertexAttrib4fv(indx, values);
}
void GLES2VertexAttribPointer(GLuint indx,
                              GLint size,
                              GLenum type,
                              GLboolean normalized,
                              GLsizei stride,
                              const void* ptr) {
  gles2::GetGLContext()->VertexAttribPointer(
      indx, size, type, normalized, stride, ptr);
}
void GLES2Viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
  gles2::GetGLContext()->Viewport(x, y, width, height);
}
void GLES2BlitFramebufferCHROMIUM(GLint srcX0,
                                  GLint srcY0,
                                  GLint srcX1,
                                  GLint srcY1,
                                  GLint dstX0,
                                  GLint dstY0,
                                  GLint dstX1,
                                  GLint dstY1,
                                  GLbitfield mask,
                                  GLenum filter) {
  gles2::GetGLContext()->BlitFramebufferCHROMIUM(
      srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}
void GLES2RenderbufferStorageMultisampleCHROMIUM(GLenum target,
                                                 GLsizei samples,
                                                 GLenum internalformat,
                                                 GLsizei width,
                                                 GLsizei height) {
  gles2::GetGLContext()->RenderbufferStorageMultisampleCHROMIUM(
      target, samples, internalformat, width, height);
}
void GLES2RenderbufferStorageMultisampleEXT(GLenum target,
                                            GLsizei samples,
                                            GLenum internalformat,
                                            GLsizei width,
                                            GLsizei height) {
  gles2::GetGLContext()->RenderbufferStorageMultisampleEXT(
      target, samples, internalformat, width, height);
}
void GLES2FramebufferTexture2DMultisampleEXT(GLenum target,
                                             GLenum attachment,
                                             GLenum textarget,
                                             GLuint texture,
                                             GLint level,
                                             GLsizei samples) {
  gles2::GetGLContext()->FramebufferTexture2DMultisampleEXT(
      target, attachment, textarget, texture, level, samples);
}
void GLES2TexStorage2DEXT(GLenum target,
                          GLsizei levels,
                          GLenum internalFormat,
                          GLsizei width,
                          GLsizei height) {
  gles2::GetGLContext()->TexStorage2DEXT(
      target, levels, internalFormat, width, height);
}
void GLES2GenQueriesEXT(GLsizei n, GLuint* queries) {
  gles2::GetGLContext()->GenQueriesEXT(n, queries);
}
void GLES2DeleteQueriesEXT(GLsizei n, const GLuint* queries) {
  gles2::GetGLContext()->DeleteQueriesEXT(n, queries);
}
GLboolean GLES2IsQueryEXT(GLuint id) {
  return gles2::GetGLContext()->IsQueryEXT(id);
}
void GLES2BeginQueryEXT(GLenum target, GLuint id) {
  gles2::GetGLContext()->BeginQueryEXT(target, id);
}
void GLES2EndQueryEXT(GLenum target) {
  gles2::GetGLContext()->EndQueryEXT(target);
}
void GLES2GetQueryivEXT(GLenum target, GLenum pname, GLint* params) {
  gles2::GetGLContext()->GetQueryivEXT(target, pname, params);
}
void GLES2GetQueryObjectuivEXT(GLuint id, GLenum pname, GLuint* params) {
  gles2::GetGLContext()->GetQueryObjectuivEXT(id, pname, params);
}
void GLES2InsertEventMarkerEXT(GLsizei length, const GLchar* marker) {
  gles2::GetGLContext()->InsertEventMarkerEXT(length, marker);
}
void GLES2PushGroupMarkerEXT(GLsizei length, const GLchar* marker) {
  gles2::GetGLContext()->PushGroupMarkerEXT(length, marker);
}
void GLES2PopGroupMarkerEXT() {
  gles2::GetGLContext()->PopGroupMarkerEXT();
}
void GLES2GenVertexArraysOES(GLsizei n, GLuint* arrays) {
  gles2::GetGLContext()->GenVertexArraysOES(n, arrays);
}
void GLES2DeleteVertexArraysOES(GLsizei n, const GLuint* arrays) {
  gles2::GetGLContext()->DeleteVertexArraysOES(n, arrays);
}
GLboolean GLES2IsVertexArrayOES(GLuint array) {
  return gles2::GetGLContext()->IsVertexArrayOES(array);
}
void GLES2BindVertexArrayOES(GLuint array) {
  gles2::GetGLContext()->BindVertexArrayOES(array);
}
void GLES2SwapBuffers() {
  gles2::GetGLContext()->SwapBuffers();
}
GLuint GLES2GetMaxValueInBufferCHROMIUM(GLuint buffer_id,
                                        GLsizei count,
                                        GLenum type,
                                        GLuint offset) {
  return gles2::GetGLContext()->GetMaxValueInBufferCHROMIUM(
      buffer_id, count, type, offset);
}
void GLES2GenSharedIdsCHROMIUM(GLuint namespace_id,
                               GLuint id_offset,
                               GLsizei n,
                               GLuint* ids) {
  gles2::GetGLContext()->GenSharedIdsCHROMIUM(namespace_id, id_offset, n, ids);
}
void GLES2DeleteSharedIdsCHROMIUM(GLuint namespace_id,
                                  GLsizei n,
                                  const GLuint* ids) {
  gles2::GetGLContext()->DeleteSharedIdsCHROMIUM(namespace_id, n, ids);
}
void GLES2RegisterSharedIdsCHROMIUM(GLuint namespace_id,
                                    GLsizei n,
                                    const GLuint* ids) {
  gles2::GetGLContext()->RegisterSharedIdsCHROMIUM(namespace_id, n, ids);
}
GLboolean GLES2EnableFeatureCHROMIUM(const char* feature) {
  return gles2::GetGLContext()->EnableFeatureCHROMIUM(feature);
}
void* GLES2MapBufferCHROMIUM(GLuint target, GLenum access) {
  return gles2::GetGLContext()->MapBufferCHROMIUM(target, access);
}
GLboolean GLES2UnmapBufferCHROMIUM(GLuint target) {
  return gles2::GetGLContext()->UnmapBufferCHROMIUM(target);
}
void* GLES2MapImageCHROMIUM(GLuint image_id) {
  return gles2::GetGLContext()->MapImageCHROMIUM(image_id);
}
void GLES2UnmapImageCHROMIUM(GLuint image_id) {
  gles2::GetGLContext()->UnmapImageCHROMIUM(image_id);
}
void* GLES2MapBufferSubDataCHROMIUM(GLuint target,
                                    GLintptr offset,
                                    GLsizeiptr size,
                                    GLenum access) {
  return gles2::GetGLContext()->MapBufferSubDataCHROMIUM(
      target, offset, size, access);
}
void GLES2UnmapBufferSubDataCHROMIUM(const void* mem) {
  gles2::GetGLContext()->UnmapBufferSubDataCHROMIUM(mem);
}
void* GLES2MapTexSubImage2DCHROMIUM(GLenum target,
                                    GLint level,
                                    GLint xoffset,
                                    GLint yoffset,
                                    GLsizei width,
                                    GLsizei height,
                                    GLenum format,
                                    GLenum type,
                                    GLenum access) {
  return gles2::GetGLContext()->MapTexSubImage2DCHROMIUM(
      target, level, xoffset, yoffset, width, height, format, type, access);
}
void GLES2UnmapTexSubImage2DCHROMIUM(const void* mem) {
  gles2::GetGLContext()->UnmapTexSubImage2DCHROMIUM(mem);
}
void GLES2ResizeCHROMIUM(GLuint width, GLuint height, GLfloat scale_factor) {
  gles2::GetGLContext()->ResizeCHROMIUM(width, height, scale_factor);
}
const GLchar* GLES2GetRequestableExtensionsCHROMIUM() {
  return gles2::GetGLContext()->GetRequestableExtensionsCHROMIUM();
}
void GLES2RequestExtensionCHROMIUM(const char* extension) {
  gles2::GetGLContext()->RequestExtensionCHROMIUM(extension);
}
void GLES2RateLimitOffscreenContextCHROMIUM() {
  gles2::GetGLContext()->RateLimitOffscreenContextCHROMIUM();
}
void GLES2GetMultipleIntegervCHROMIUM(const GLenum* pnames,
                                      GLuint count,
                                      GLint* results,
                                      GLsizeiptr size) {
  gles2::GetGLContext()->GetMultipleIntegervCHROMIUM(
      pnames, count, results, size);
}
void GLES2GetProgramInfoCHROMIUM(GLuint program,
                                 GLsizei bufsize,
                                 GLsizei* size,
                                 void* info) {
  gles2::GetGLContext()->GetProgramInfoCHROMIUM(program, bufsize, size, info);
}
GLuint GLES2CreateStreamTextureCHROMIUM(GLuint texture) {
  return gles2::GetGLContext()->CreateStreamTextureCHROMIUM(texture);
}
GLuint GLES2CreateImageCHROMIUM(GLsizei width,
                                GLsizei height,
                                GLenum internalformat,
                                GLenum usage) {
  return gles2::GetGLContext()->CreateImageCHROMIUM(
      width, height, internalformat, usage);
}
void GLES2DestroyImageCHROMIUM(GLuint image_id) {
  gles2::GetGLContext()->DestroyImageCHROMIUM(image_id);
}
void GLES2GetImageParameterivCHROMIUM(GLuint image_id,
                                      GLenum pname,
                                      GLint* params) {
  gles2::GetGLContext()->GetImageParameterivCHROMIUM(image_id, pname, params);
}
void GLES2GetTranslatedShaderSourceANGLE(GLuint shader,
                                         GLsizei bufsize,
                                         GLsizei* length,
                                         char* source) {
  gles2::GetGLContext()->GetTranslatedShaderSourceANGLE(
      shader, bufsize, length, source);
}
void GLES2PostSubBufferCHROMIUM(GLint x, GLint y, GLint width, GLint height) {
  gles2::GetGLContext()->PostSubBufferCHROMIUM(x, y, width, height);
}
void GLES2TexImageIOSurface2DCHROMIUM(GLenum target,
                                      GLsizei width,
                                      GLsizei height,
                                      GLuint ioSurfaceId,
                                      GLuint plane) {
  gles2::GetGLContext()->TexImageIOSurface2DCHROMIUM(
      target, width, height, ioSurfaceId, plane);
}
void GLES2CopyTextureCHROMIUM(GLenum target,
                              GLenum source_id,
                              GLenum dest_id,
                              GLint level,
                              GLint internalformat,
                              GLenum dest_type) {
  gles2::GetGLContext()->CopyTextureCHROMIUM(
      target, source_id, dest_id, level, internalformat, dest_type);
}
void GLES2DrawArraysInstancedANGLE(GLenum mode,
                                   GLint first,
                                   GLsizei count,
                                   GLsizei primcount) {
  gles2::GetGLContext()->DrawArraysInstancedANGLE(
      mode, first, count, primcount);
}
void GLES2DrawElementsInstancedANGLE(GLenum mode,
                                     GLsizei count,
                                     GLenum type,
                                     const void* indices,
                                     GLsizei primcount) {
  gles2::GetGLContext()->DrawElementsInstancedANGLE(
      mode, count, type, indices, primcount);
}
void GLES2VertexAttribDivisorANGLE(GLuint index, GLuint divisor) {
  gles2::GetGLContext()->VertexAttribDivisorANGLE(index, divisor);
}
void GLES2GenMailboxCHROMIUM(GLbyte* mailbox) {
  gles2::GetGLContext()->GenMailboxCHROMIUM(mailbox);
}
void GLES2ProduceTextureCHROMIUM(GLenum target, const GLbyte* mailbox) {
  gles2::GetGLContext()->ProduceTextureCHROMIUM(target, mailbox);
}
void GLES2ProduceTextureDirectCHROMIUM(GLuint texture,
                                       GLenum target,
                                       const GLbyte* mailbox) {
  gles2::GetGLContext()->ProduceTextureDirectCHROMIUM(texture, target, mailbox);
}
void GLES2ConsumeTextureCHROMIUM(GLenum target, const GLbyte* mailbox) {
  gles2::GetGLContext()->ConsumeTextureCHROMIUM(target, mailbox);
}
GLuint GLES2CreateAndConsumeTextureCHROMIUM(GLenum target,
                                            const GLbyte* mailbox) {
  return gles2::GetGLContext()->CreateAndConsumeTextureCHROMIUM(target,
                                                                mailbox);
}
void GLES2BindUniformLocationCHROMIUM(GLuint program,
                                      GLint location,
                                      const char* name) {
  gles2::GetGLContext()->BindUniformLocationCHROMIUM(program, location, name);
}
void GLES2BindTexImage2DCHROMIUM(GLenum target, GLint imageId) {
  gles2::GetGLContext()->BindTexImage2DCHROMIUM(target, imageId);
}
void GLES2ReleaseTexImage2DCHROMIUM(GLenum target, GLint imageId) {
  gles2::GetGLContext()->ReleaseTexImage2DCHROMIUM(target, imageId);
}
void GLES2TraceBeginCHROMIUM(const char* name) {
  gles2::GetGLContext()->TraceBeginCHROMIUM(name);
}
void GLES2TraceEndCHROMIUM() {
  gles2::GetGLContext()->TraceEndCHROMIUM();
}
void GLES2AsyncTexSubImage2DCHROMIUM(GLenum target,
                                     GLint level,
                                     GLint xoffset,
                                     GLint yoffset,
                                     GLsizei width,
                                     GLsizei height,
                                     GLenum format,
                                     GLenum type,
                                     const void* data) {
  gles2::GetGLContext()->AsyncTexSubImage2DCHROMIUM(
      target, level, xoffset, yoffset, width, height, format, type, data);
}
void GLES2AsyncTexImage2DCHROMIUM(GLenum target,
                                  GLint level,
                                  GLenum internalformat,
                                  GLsizei width,
                                  GLsizei height,
                                  GLint border,
                                  GLenum format,
                                  GLenum type,
                                  const void* pixels) {
  gles2::GetGLContext()->AsyncTexImage2DCHROMIUM(target,
                                                 level,
                                                 internalformat,
                                                 width,
                                                 height,
                                                 border,
                                                 format,
                                                 type,
                                                 pixels);
}
void GLES2WaitAsyncTexImage2DCHROMIUM(GLenum target) {
  gles2::GetGLContext()->WaitAsyncTexImage2DCHROMIUM(target);
}
void GLES2WaitAllAsyncTexImage2DCHROMIUM() {
  gles2::GetGLContext()->WaitAllAsyncTexImage2DCHROMIUM();
}
void GLES2DiscardFramebufferEXT(GLenum target,
                                GLsizei count,
                                const GLenum* attachments) {
  gles2::GetGLContext()->DiscardFramebufferEXT(target, count, attachments);
}
void GLES2LoseContextCHROMIUM(GLenum current, GLenum other) {
  gles2::GetGLContext()->LoseContextCHROMIUM(current, other);
}
GLuint GLES2InsertSyncPointCHROMIUM() {
  return gles2::GetGLContext()->InsertSyncPointCHROMIUM();
}
void GLES2WaitSyncPointCHROMIUM(GLuint sync_point) {
  gles2::GetGLContext()->WaitSyncPointCHROMIUM(sync_point);
}
void GLES2DrawBuffersEXT(GLsizei count, const GLenum* bufs) {
  gles2::GetGLContext()->DrawBuffersEXT(count, bufs);
}
void GLES2DiscardBackbufferCHROMIUM() {
  gles2::GetGLContext()->DiscardBackbufferCHROMIUM();
}
void GLES2ScheduleOverlayPlaneCHROMIUM(GLint plane_z_order,
                                       GLenum plane_transform,
                                       GLuint overlay_texture_id,
                                       GLint bounds_x,
                                       GLint bounds_y,
                                       GLint bounds_width,
                                       GLint bounds_height,
                                       GLfloat uv_x,
                                       GLfloat uv_y,
                                       GLfloat uv_width,
                                       GLfloat uv_height) {
  gles2::GetGLContext()->ScheduleOverlayPlaneCHROMIUM(plane_z_order,
                                                      plane_transform,
                                                      overlay_texture_id,
                                                      bounds_x,
                                                      bounds_y,
                                                      bounds_width,
                                                      bounds_height,
                                                      uv_x,
                                                      uv_y,
                                                      uv_width,
                                                      uv_height);
}

namespace gles2 {

extern const NameToFunc g_gles2_function_table[] = {
    {
     "glActiveTexture", reinterpret_cast<GLES2FunctionPointer>(glActiveTexture),
    },
    {
     "glAttachShader", reinterpret_cast<GLES2FunctionPointer>(glAttachShader),
    },
    {
     "glBindAttribLocation",
     reinterpret_cast<GLES2FunctionPointer>(glBindAttribLocation),
    },
    {
     "glBindBuffer", reinterpret_cast<GLES2FunctionPointer>(glBindBuffer),
    },
    {
     "glBindFramebuffer",
     reinterpret_cast<GLES2FunctionPointer>(glBindFramebuffer),
    },
    {
     "glBindRenderbuffer",
     reinterpret_cast<GLES2FunctionPointer>(glBindRenderbuffer),
    },
    {
     "glBindTexture", reinterpret_cast<GLES2FunctionPointer>(glBindTexture),
    },
    {
     "glBlendColor", reinterpret_cast<GLES2FunctionPointer>(glBlendColor),
    },
    {
     "glBlendEquation", reinterpret_cast<GLES2FunctionPointer>(glBlendEquation),
    },
    {
     "glBlendEquationSeparate",
     reinterpret_cast<GLES2FunctionPointer>(glBlendEquationSeparate),
    },
    {
     "glBlendFunc", reinterpret_cast<GLES2FunctionPointer>(glBlendFunc),
    },
    {
     "glBlendFuncSeparate",
     reinterpret_cast<GLES2FunctionPointer>(glBlendFuncSeparate),
    },
    {
     "glBufferData", reinterpret_cast<GLES2FunctionPointer>(glBufferData),
    },
    {
     "glBufferSubData", reinterpret_cast<GLES2FunctionPointer>(glBufferSubData),
    },
    {
     "glCheckFramebufferStatus",
     reinterpret_cast<GLES2FunctionPointer>(glCheckFramebufferStatus),
    },
    {
     "glClear", reinterpret_cast<GLES2FunctionPointer>(glClear),
    },
    {
     "glClearColor", reinterpret_cast<GLES2FunctionPointer>(glClearColor),
    },
    {
     "glClearDepthf", reinterpret_cast<GLES2FunctionPointer>(glClearDepthf),
    },
    {
     "glClearStencil", reinterpret_cast<GLES2FunctionPointer>(glClearStencil),
    },
    {
     "glColorMask", reinterpret_cast<GLES2FunctionPointer>(glColorMask),
    },
    {
     "glCompileShader", reinterpret_cast<GLES2FunctionPointer>(glCompileShader),
    },
    {
     "glCompressedTexImage2D",
     reinterpret_cast<GLES2FunctionPointer>(glCompressedTexImage2D),
    },
    {
     "glCompressedTexSubImage2D",
     reinterpret_cast<GLES2FunctionPointer>(glCompressedTexSubImage2D),
    },
    {
     "glCopyTexImage2D",
     reinterpret_cast<GLES2FunctionPointer>(glCopyTexImage2D),
    },
    {
     "glCopyTexSubImage2D",
     reinterpret_cast<GLES2FunctionPointer>(glCopyTexSubImage2D),
    },
    {
     "glCreateProgram", reinterpret_cast<GLES2FunctionPointer>(glCreateProgram),
    },
    {
     "glCreateShader", reinterpret_cast<GLES2FunctionPointer>(glCreateShader),
    },
    {
     "glCullFace", reinterpret_cast<GLES2FunctionPointer>(glCullFace),
    },
    {
     "glDeleteBuffers", reinterpret_cast<GLES2FunctionPointer>(glDeleteBuffers),
    },
    {
     "glDeleteFramebuffers",
     reinterpret_cast<GLES2FunctionPointer>(glDeleteFramebuffers),
    },
    {
     "glDeleteProgram", reinterpret_cast<GLES2FunctionPointer>(glDeleteProgram),
    },
    {
     "glDeleteRenderbuffers",
     reinterpret_cast<GLES2FunctionPointer>(glDeleteRenderbuffers),
    },
    {
     "glDeleteShader", reinterpret_cast<GLES2FunctionPointer>(glDeleteShader),
    },
    {
     "glDeleteTextures",
     reinterpret_cast<GLES2FunctionPointer>(glDeleteTextures),
    },
    {
     "glDepthFunc", reinterpret_cast<GLES2FunctionPointer>(glDepthFunc),
    },
    {
     "glDepthMask", reinterpret_cast<GLES2FunctionPointer>(glDepthMask),
    },
    {
     "glDepthRangef", reinterpret_cast<GLES2FunctionPointer>(glDepthRangef),
    },
    {
     "glDetachShader", reinterpret_cast<GLES2FunctionPointer>(glDetachShader),
    },
    {
     "glDisable", reinterpret_cast<GLES2FunctionPointer>(glDisable),
    },
    {
     "glDisableVertexAttribArray",
     reinterpret_cast<GLES2FunctionPointer>(glDisableVertexAttribArray),
    },
    {
     "glDrawArrays", reinterpret_cast<GLES2FunctionPointer>(glDrawArrays),
    },
    {
     "glDrawElements", reinterpret_cast<GLES2FunctionPointer>(glDrawElements),
    },
    {
     "glEnable", reinterpret_cast<GLES2FunctionPointer>(glEnable),
    },
    {
     "glEnableVertexAttribArray",
     reinterpret_cast<GLES2FunctionPointer>(glEnableVertexAttribArray),
    },
    {
     "glFinish", reinterpret_cast<GLES2FunctionPointer>(glFinish),
    },
    {
     "glFlush", reinterpret_cast<GLES2FunctionPointer>(glFlush),
    },
    {
     "glFramebufferRenderbuffer",
     reinterpret_cast<GLES2FunctionPointer>(glFramebufferRenderbuffer),
    },
    {
     "glFramebufferTexture2D",
     reinterpret_cast<GLES2FunctionPointer>(glFramebufferTexture2D),
    },
    {
     "glFrontFace", reinterpret_cast<GLES2FunctionPointer>(glFrontFace),
    },
    {
     "glGenBuffers", reinterpret_cast<GLES2FunctionPointer>(glGenBuffers),
    },
    {
     "glGenerateMipmap",
     reinterpret_cast<GLES2FunctionPointer>(glGenerateMipmap),
    },
    {
     "glGenFramebuffers",
     reinterpret_cast<GLES2FunctionPointer>(glGenFramebuffers),
    },
    {
     "glGenRenderbuffers",
     reinterpret_cast<GLES2FunctionPointer>(glGenRenderbuffers),
    },
    {
     "glGenTextures", reinterpret_cast<GLES2FunctionPointer>(glGenTextures),
    },
    {
     "glGetActiveAttrib",
     reinterpret_cast<GLES2FunctionPointer>(glGetActiveAttrib),
    },
    {
     "glGetActiveUniform",
     reinterpret_cast<GLES2FunctionPointer>(glGetActiveUniform),
    },
    {
     "glGetAttachedShaders",
     reinterpret_cast<GLES2FunctionPointer>(glGetAttachedShaders),
    },
    {
     "glGetAttribLocation",
     reinterpret_cast<GLES2FunctionPointer>(glGetAttribLocation),
    },
    {
     "glGetBooleanv", reinterpret_cast<GLES2FunctionPointer>(glGetBooleanv),
    },
    {
     "glGetBufferParameteriv",
     reinterpret_cast<GLES2FunctionPointer>(glGetBufferParameteriv),
    },
    {
     "glGetError", reinterpret_cast<GLES2FunctionPointer>(glGetError),
    },
    {
     "glGetFloatv", reinterpret_cast<GLES2FunctionPointer>(glGetFloatv),
    },
    {
     "glGetFramebufferAttachmentParameteriv",
     reinterpret_cast<GLES2FunctionPointer>(
         glGetFramebufferAttachmentParameteriv),
    },
    {
     "glGetIntegerv", reinterpret_cast<GLES2FunctionPointer>(glGetIntegerv),
    },
    {
     "glGetProgramiv", reinterpret_cast<GLES2FunctionPointer>(glGetProgramiv),
    },
    {
     "glGetProgramInfoLog",
     reinterpret_cast<GLES2FunctionPointer>(glGetProgramInfoLog),
    },
    {
     "glGetRenderbufferParameteriv",
     reinterpret_cast<GLES2FunctionPointer>(glGetRenderbufferParameteriv),
    },
    {
     "glGetShaderiv", reinterpret_cast<GLES2FunctionPointer>(glGetShaderiv),
    },
    {
     "glGetShaderInfoLog",
     reinterpret_cast<GLES2FunctionPointer>(glGetShaderInfoLog),
    },
    {
     "glGetShaderPrecisionFormat",
     reinterpret_cast<GLES2FunctionPointer>(glGetShaderPrecisionFormat),
    },
    {
     "glGetShaderSource",
     reinterpret_cast<GLES2FunctionPointer>(glGetShaderSource),
    },
    {
     "glGetString", reinterpret_cast<GLES2FunctionPointer>(glGetString),
    },
    {
     "glGetTexParameterfv",
     reinterpret_cast<GLES2FunctionPointer>(glGetTexParameterfv),
    },
    {
     "glGetTexParameteriv",
     reinterpret_cast<GLES2FunctionPointer>(glGetTexParameteriv),
    },
    {
     "glGetUniformfv", reinterpret_cast<GLES2FunctionPointer>(glGetUniformfv),
    },
    {
     "glGetUniformiv", reinterpret_cast<GLES2FunctionPointer>(glGetUniformiv),
    },
    {
     "glGetUniformLocation",
     reinterpret_cast<GLES2FunctionPointer>(glGetUniformLocation),
    },
    {
     "glGetVertexAttribfv",
     reinterpret_cast<GLES2FunctionPointer>(glGetVertexAttribfv),
    },
    {
     "glGetVertexAttribiv",
     reinterpret_cast<GLES2FunctionPointer>(glGetVertexAttribiv),
    },
    {
     "glGetVertexAttribPointerv",
     reinterpret_cast<GLES2FunctionPointer>(glGetVertexAttribPointerv),
    },
    {
     "glHint", reinterpret_cast<GLES2FunctionPointer>(glHint),
    },
    {
     "glIsBuffer", reinterpret_cast<GLES2FunctionPointer>(glIsBuffer),
    },
    {
     "glIsEnabled", reinterpret_cast<GLES2FunctionPointer>(glIsEnabled),
    },
    {
     "glIsFramebuffer", reinterpret_cast<GLES2FunctionPointer>(glIsFramebuffer),
    },
    {
     "glIsProgram", reinterpret_cast<GLES2FunctionPointer>(glIsProgram),
    },
    {
     "glIsRenderbuffer",
     reinterpret_cast<GLES2FunctionPointer>(glIsRenderbuffer),
    },
    {
     "glIsShader", reinterpret_cast<GLES2FunctionPointer>(glIsShader),
    },
    {
     "glIsTexture", reinterpret_cast<GLES2FunctionPointer>(glIsTexture),
    },
    {
     "glLineWidth", reinterpret_cast<GLES2FunctionPointer>(glLineWidth),
    },
    {
     "glLinkProgram", reinterpret_cast<GLES2FunctionPointer>(glLinkProgram),
    },
    {
     "glPixelStorei", reinterpret_cast<GLES2FunctionPointer>(glPixelStorei),
    },
    {
     "glPolygonOffset", reinterpret_cast<GLES2FunctionPointer>(glPolygonOffset),
    },
    {
     "glReadPixels", reinterpret_cast<GLES2FunctionPointer>(glReadPixels),
    },
    {
     "glReleaseShaderCompiler",
     reinterpret_cast<GLES2FunctionPointer>(glReleaseShaderCompiler),
    },
    {
     "glRenderbufferStorage",
     reinterpret_cast<GLES2FunctionPointer>(glRenderbufferStorage),
    },
    {
     "glSampleCoverage",
     reinterpret_cast<GLES2FunctionPointer>(glSampleCoverage),
    },
    {
     "glScissor", reinterpret_cast<GLES2FunctionPointer>(glScissor),
    },
    {
     "glShaderBinary", reinterpret_cast<GLES2FunctionPointer>(glShaderBinary),
    },
    {
     "glShaderSource", reinterpret_cast<GLES2FunctionPointer>(glShaderSource),
    },
    {
     "glShallowFinishCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glShallowFinishCHROMIUM),
    },
    {
     "glShallowFlushCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glShallowFlushCHROMIUM),
    },
    {
     "glStencilFunc", reinterpret_cast<GLES2FunctionPointer>(glStencilFunc),
    },
    {
     "glStencilFuncSeparate",
     reinterpret_cast<GLES2FunctionPointer>(glStencilFuncSeparate),
    },
    {
     "glStencilMask", reinterpret_cast<GLES2FunctionPointer>(glStencilMask),
    },
    {
     "glStencilMaskSeparate",
     reinterpret_cast<GLES2FunctionPointer>(glStencilMaskSeparate),
    },
    {
     "glStencilOp", reinterpret_cast<GLES2FunctionPointer>(glStencilOp),
    },
    {
     "glStencilOpSeparate",
     reinterpret_cast<GLES2FunctionPointer>(glStencilOpSeparate),
    },
    {
     "glTexImage2D", reinterpret_cast<GLES2FunctionPointer>(glTexImage2D),
    },
    {
     "glTexParameterf", reinterpret_cast<GLES2FunctionPointer>(glTexParameterf),
    },
    {
     "glTexParameterfv",
     reinterpret_cast<GLES2FunctionPointer>(glTexParameterfv),
    },
    {
     "glTexParameteri", reinterpret_cast<GLES2FunctionPointer>(glTexParameteri),
    },
    {
     "glTexParameteriv",
     reinterpret_cast<GLES2FunctionPointer>(glTexParameteriv),
    },
    {
     "glTexSubImage2D", reinterpret_cast<GLES2FunctionPointer>(glTexSubImage2D),
    },
    {
     "glUniform1f", reinterpret_cast<GLES2FunctionPointer>(glUniform1f),
    },
    {
     "glUniform1fv", reinterpret_cast<GLES2FunctionPointer>(glUniform1fv),
    },
    {
     "glUniform1i", reinterpret_cast<GLES2FunctionPointer>(glUniform1i),
    },
    {
     "glUniform1iv", reinterpret_cast<GLES2FunctionPointer>(glUniform1iv),
    },
    {
     "glUniform2f", reinterpret_cast<GLES2FunctionPointer>(glUniform2f),
    },
    {
     "glUniform2fv", reinterpret_cast<GLES2FunctionPointer>(glUniform2fv),
    },
    {
     "glUniform2i", reinterpret_cast<GLES2FunctionPointer>(glUniform2i),
    },
    {
     "glUniform2iv", reinterpret_cast<GLES2FunctionPointer>(glUniform2iv),
    },
    {
     "glUniform3f", reinterpret_cast<GLES2FunctionPointer>(glUniform3f),
    },
    {
     "glUniform3fv", reinterpret_cast<GLES2FunctionPointer>(glUniform3fv),
    },
    {
     "glUniform3i", reinterpret_cast<GLES2FunctionPointer>(glUniform3i),
    },
    {
     "glUniform3iv", reinterpret_cast<GLES2FunctionPointer>(glUniform3iv),
    },
    {
     "glUniform4f", reinterpret_cast<GLES2FunctionPointer>(glUniform4f),
    },
    {
     "glUniform4fv", reinterpret_cast<GLES2FunctionPointer>(glUniform4fv),
    },
    {
     "glUniform4i", reinterpret_cast<GLES2FunctionPointer>(glUniform4i),
    },
    {
     "glUniform4iv", reinterpret_cast<GLES2FunctionPointer>(glUniform4iv),
    },
    {
     "glUniformMatrix2fv",
     reinterpret_cast<GLES2FunctionPointer>(glUniformMatrix2fv),
    },
    {
     "glUniformMatrix3fv",
     reinterpret_cast<GLES2FunctionPointer>(glUniformMatrix3fv),
    },
    {
     "glUniformMatrix4fv",
     reinterpret_cast<GLES2FunctionPointer>(glUniformMatrix4fv),
    },
    {
     "glUseProgram", reinterpret_cast<GLES2FunctionPointer>(glUseProgram),
    },
    {
     "glValidateProgram",
     reinterpret_cast<GLES2FunctionPointer>(glValidateProgram),
    },
    {
     "glVertexAttrib1f",
     reinterpret_cast<GLES2FunctionPointer>(glVertexAttrib1f),
    },
    {
     "glVertexAttrib1fv",
     reinterpret_cast<GLES2FunctionPointer>(glVertexAttrib1fv),
    },
    {
     "glVertexAttrib2f",
     reinterpret_cast<GLES2FunctionPointer>(glVertexAttrib2f),
    },
    {
     "glVertexAttrib2fv",
     reinterpret_cast<GLES2FunctionPointer>(glVertexAttrib2fv),
    },
    {
     "glVertexAttrib3f",
     reinterpret_cast<GLES2FunctionPointer>(glVertexAttrib3f),
    },
    {
     "glVertexAttrib3fv",
     reinterpret_cast<GLES2FunctionPointer>(glVertexAttrib3fv),
    },
    {
     "glVertexAttrib4f",
     reinterpret_cast<GLES2FunctionPointer>(glVertexAttrib4f),
    },
    {
     "glVertexAttrib4fv",
     reinterpret_cast<GLES2FunctionPointer>(glVertexAttrib4fv),
    },
    {
     "glVertexAttribPointer",
     reinterpret_cast<GLES2FunctionPointer>(glVertexAttribPointer),
    },
    {
     "glViewport", reinterpret_cast<GLES2FunctionPointer>(glViewport),
    },
    {
     "glBlitFramebufferCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glBlitFramebufferCHROMIUM),
    },
    {
     "glRenderbufferStorageMultisampleCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(
         glRenderbufferStorageMultisampleCHROMIUM),
    },
    {
     "glRenderbufferStorageMultisampleEXT",
     reinterpret_cast<GLES2FunctionPointer>(
         glRenderbufferStorageMultisampleEXT),
    },
    {
     "glFramebufferTexture2DMultisampleEXT",
     reinterpret_cast<GLES2FunctionPointer>(
         glFramebufferTexture2DMultisampleEXT),
    },
    {
     "glTexStorage2DEXT",
     reinterpret_cast<GLES2FunctionPointer>(glTexStorage2DEXT),
    },
    {
     "glGenQueriesEXT", reinterpret_cast<GLES2FunctionPointer>(glGenQueriesEXT),
    },
    {
     "glDeleteQueriesEXT",
     reinterpret_cast<GLES2FunctionPointer>(glDeleteQueriesEXT),
    },
    {
     "glIsQueryEXT", reinterpret_cast<GLES2FunctionPointer>(glIsQueryEXT),
    },
    {
     "glBeginQueryEXT", reinterpret_cast<GLES2FunctionPointer>(glBeginQueryEXT),
    },
    {
     "glEndQueryEXT", reinterpret_cast<GLES2FunctionPointer>(glEndQueryEXT),
    },
    {
     "glGetQueryivEXT", reinterpret_cast<GLES2FunctionPointer>(glGetQueryivEXT),
    },
    {
     "glGetQueryObjectuivEXT",
     reinterpret_cast<GLES2FunctionPointer>(glGetQueryObjectuivEXT),
    },
    {
     "glInsertEventMarkerEXT",
     reinterpret_cast<GLES2FunctionPointer>(glInsertEventMarkerEXT),
    },
    {
     "glPushGroupMarkerEXT",
     reinterpret_cast<GLES2FunctionPointer>(glPushGroupMarkerEXT),
    },
    {
     "glPopGroupMarkerEXT",
     reinterpret_cast<GLES2FunctionPointer>(glPopGroupMarkerEXT),
    },
    {
     "glGenVertexArraysOES",
     reinterpret_cast<GLES2FunctionPointer>(glGenVertexArraysOES),
    },
    {
     "glDeleteVertexArraysOES",
     reinterpret_cast<GLES2FunctionPointer>(glDeleteVertexArraysOES),
    },
    {
     "glIsVertexArrayOES",
     reinterpret_cast<GLES2FunctionPointer>(glIsVertexArrayOES),
    },
    {
     "glBindVertexArrayOES",
     reinterpret_cast<GLES2FunctionPointer>(glBindVertexArrayOES),
    },
    {
     "glSwapBuffers", reinterpret_cast<GLES2FunctionPointer>(glSwapBuffers),
    },
    {
     "glGetMaxValueInBufferCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glGetMaxValueInBufferCHROMIUM),
    },
    {
     "glGenSharedIdsCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glGenSharedIdsCHROMIUM),
    },
    {
     "glDeleteSharedIdsCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glDeleteSharedIdsCHROMIUM),
    },
    {
     "glRegisterSharedIdsCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glRegisterSharedIdsCHROMIUM),
    },
    {
     "glEnableFeatureCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glEnableFeatureCHROMIUM),
    },
    {
     "glMapBufferCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glMapBufferCHROMIUM),
    },
    {
     "glUnmapBufferCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glUnmapBufferCHROMIUM),
    },
    {
     "glMapImageCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glMapImageCHROMIUM),
    },
    {
     "glUnmapImageCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glUnmapImageCHROMIUM),
    },
    {
     "glMapBufferSubDataCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glMapBufferSubDataCHROMIUM),
    },
    {
     "glUnmapBufferSubDataCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glUnmapBufferSubDataCHROMIUM),
    },
    {
     "glMapTexSubImage2DCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glMapTexSubImage2DCHROMIUM),
    },
    {
     "glUnmapTexSubImage2DCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glUnmapTexSubImage2DCHROMIUM),
    },
    {
     "glResizeCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glResizeCHROMIUM),
    },
    {
     "glGetRequestableExtensionsCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glGetRequestableExtensionsCHROMIUM),
    },
    {
     "glRequestExtensionCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glRequestExtensionCHROMIUM),
    },
    {
     "glRateLimitOffscreenContextCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(
         glRateLimitOffscreenContextCHROMIUM),
    },
    {
     "glGetMultipleIntegervCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glGetMultipleIntegervCHROMIUM),
    },
    {
     "glGetProgramInfoCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glGetProgramInfoCHROMIUM),
    },
    {
     "glCreateStreamTextureCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glCreateStreamTextureCHROMIUM),
    },
    {
     "glCreateImageCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glCreateImageCHROMIUM),
    },
    {
     "glDestroyImageCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glDestroyImageCHROMIUM),
    },
    {
     "glGetImageParameterivCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glGetImageParameterivCHROMIUM),
    },
    {
     "glGetTranslatedShaderSourceANGLE",
     reinterpret_cast<GLES2FunctionPointer>(glGetTranslatedShaderSourceANGLE),
    },
    {
     "glPostSubBufferCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glPostSubBufferCHROMIUM),
    },
    {
     "glTexImageIOSurface2DCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glTexImageIOSurface2DCHROMIUM),
    },
    {
     "glCopyTextureCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glCopyTextureCHROMIUM),
    },
    {
     "glDrawArraysInstancedANGLE",
     reinterpret_cast<GLES2FunctionPointer>(glDrawArraysInstancedANGLE),
    },
    {
     "glDrawElementsInstancedANGLE",
     reinterpret_cast<GLES2FunctionPointer>(glDrawElementsInstancedANGLE),
    },
    {
     "glVertexAttribDivisorANGLE",
     reinterpret_cast<GLES2FunctionPointer>(glVertexAttribDivisorANGLE),
    },
    {
     "glGenMailboxCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glGenMailboxCHROMIUM),
    },
    {
     "glProduceTextureCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glProduceTextureCHROMIUM),
    },
    {
     "glProduceTextureDirectCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glProduceTextureDirectCHROMIUM),
    },
    {
     "glConsumeTextureCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glConsumeTextureCHROMIUM),
    },
    {
     "glCreateAndConsumeTextureCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glCreateAndConsumeTextureCHROMIUM),
    },
    {
     "glBindUniformLocationCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glBindUniformLocationCHROMIUM),
    },
    {
     "glBindTexImage2DCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glBindTexImage2DCHROMIUM),
    },
    {
     "glReleaseTexImage2DCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glReleaseTexImage2DCHROMIUM),
    },
    {
     "glTraceBeginCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glTraceBeginCHROMIUM),
    },
    {
     "glTraceEndCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glTraceEndCHROMIUM),
    },
    {
     "glAsyncTexSubImage2DCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glAsyncTexSubImage2DCHROMIUM),
    },
    {
     "glAsyncTexImage2DCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glAsyncTexImage2DCHROMIUM),
    },
    {
     "glWaitAsyncTexImage2DCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glWaitAsyncTexImage2DCHROMIUM),
    },
    {
     "glWaitAllAsyncTexImage2DCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glWaitAllAsyncTexImage2DCHROMIUM),
    },
    {
     "glDiscardFramebufferEXT",
     reinterpret_cast<GLES2FunctionPointer>(glDiscardFramebufferEXT),
    },
    {
     "glLoseContextCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glLoseContextCHROMIUM),
    },
    {
     "glInsertSyncPointCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glInsertSyncPointCHROMIUM),
    },
    {
     "glWaitSyncPointCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glWaitSyncPointCHROMIUM),
    },
    {
     "glDrawBuffersEXT",
     reinterpret_cast<GLES2FunctionPointer>(glDrawBuffersEXT),
    },
    {
     "glDiscardBackbufferCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glDiscardBackbufferCHROMIUM),
    },
    {
     "glScheduleOverlayPlaneCHROMIUM",
     reinterpret_cast<GLES2FunctionPointer>(glScheduleOverlayPlaneCHROMIUM),
    },
    {
     NULL, NULL,
    },
};

}  // namespace gles2
#endif  // GPU_COMMAND_BUFFER_CLIENT_GLES2_C_LIB_AUTOGEN_H_
