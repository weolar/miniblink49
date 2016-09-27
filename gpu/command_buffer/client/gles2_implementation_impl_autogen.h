// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

// This file is included by gles2_implementation.cc to define the
// GL api functions.
#ifndef GPU_COMMAND_BUFFER_CLIENT_GLES2_IMPLEMENTATION_IMPL_AUTOGEN_H_
#define GPU_COMMAND_BUFFER_CLIENT_GLES2_IMPLEMENTATION_IMPL_AUTOGEN_H_

void GLES2Implementation::AttachShader(GLuint program, GLuint shader) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glAttachShader(" << program << ", "
                     << shader << ")");
  helper_->AttachShader(program, shader);
  CheckGLError();
}

void GLES2Implementation::BindBuffer(GLenum target, GLuint buffer) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBindBuffer("
                     << GLES2Util::GetStringBufferTarget(target) << ", "
                     << buffer << ")");
  if (IsBufferReservedId(buffer)) {
    SetGLError(GL_INVALID_OPERATION, "BindBuffer", "buffer reserved id");
    return;
  }
  if (BindBufferHelper(target, buffer)) {
    helper_->BindBuffer(target, buffer);
  }
  CheckGLError();
}

void GLES2Implementation::BindFramebuffer(GLenum target, GLuint framebuffer) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBindFramebuffer("
                     << GLES2Util::GetStringFrameBufferTarget(target) << ", "
                     << framebuffer << ")");
  if (IsFramebufferReservedId(framebuffer)) {
    SetGLError(
        GL_INVALID_OPERATION, "BindFramebuffer", "framebuffer reserved id");
    return;
  }
  if (BindFramebufferHelper(target, framebuffer)) {
    helper_->BindFramebuffer(target, framebuffer);
  }
  CheckGLError();
}

void GLES2Implementation::BindRenderbuffer(GLenum target, GLuint renderbuffer) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBindRenderbuffer("
                     << GLES2Util::GetStringRenderBufferTarget(target) << ", "
                     << renderbuffer << ")");
  if (IsRenderbufferReservedId(renderbuffer)) {
    SetGLError(
        GL_INVALID_OPERATION, "BindRenderbuffer", "renderbuffer reserved id");
    return;
  }
  if (BindRenderbufferHelper(target, renderbuffer)) {
    helper_->BindRenderbuffer(target, renderbuffer);
  }
  CheckGLError();
}

void GLES2Implementation::BindTexture(GLenum target, GLuint texture) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBindTexture("
                     << GLES2Util::GetStringTextureBindTarget(target) << ", "
                     << texture << ")");
  if (IsTextureReservedId(texture)) {
    SetGLError(GL_INVALID_OPERATION, "BindTexture", "texture reserved id");
    return;
  }
  if (BindTextureHelper(target, texture)) {
    helper_->BindTexture(target, texture);
  }
  CheckGLError();
}

void GLES2Implementation::BlendColor(GLclampf red,
                                     GLclampf green,
                                     GLclampf blue,
                                     GLclampf alpha) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBlendColor(" << red << ", "
                     << green << ", " << blue << ", " << alpha << ")");
  helper_->BlendColor(red, green, blue, alpha);
  CheckGLError();
}

void GLES2Implementation::BlendEquation(GLenum mode) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBlendEquation("
                     << GLES2Util::GetStringEquation(mode) << ")");
  helper_->BlendEquation(mode);
  CheckGLError();
}

void GLES2Implementation::BlendEquationSeparate(GLenum modeRGB,
                                                GLenum modeAlpha) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBlendEquationSeparate("
                     << GLES2Util::GetStringEquation(modeRGB) << ", "
                     << GLES2Util::GetStringEquation(modeAlpha) << ")");
  helper_->BlendEquationSeparate(modeRGB, modeAlpha);
  CheckGLError();
}

void GLES2Implementation::BlendFunc(GLenum sfactor, GLenum dfactor) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBlendFunc("
                     << GLES2Util::GetStringSrcBlendFactor(sfactor) << ", "
                     << GLES2Util::GetStringDstBlendFactor(dfactor) << ")");
  helper_->BlendFunc(sfactor, dfactor);
  CheckGLError();
}

void GLES2Implementation::BlendFuncSeparate(GLenum srcRGB,
                                            GLenum dstRGB,
                                            GLenum srcAlpha,
                                            GLenum dstAlpha) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBlendFuncSeparate("
                     << GLES2Util::GetStringSrcBlendFactor(srcRGB) << ", "
                     << GLES2Util::GetStringDstBlendFactor(dstRGB) << ", "
                     << GLES2Util::GetStringSrcBlendFactor(srcAlpha) << ", "
                     << GLES2Util::GetStringDstBlendFactor(dstAlpha) << ")");
  helper_->BlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
  CheckGLError();
}

GLenum GLES2Implementation::CheckFramebufferStatus(GLenum target) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  TRACE_EVENT0("gpu", "GLES2Implementation::CheckFramebufferStatus");
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glCheckFramebufferStatus("
                     << GLES2Util::GetStringFrameBufferTarget(target) << ")");
  typedef cmds::CheckFramebufferStatus::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return GL_FRAMEBUFFER_UNSUPPORTED;
  }
  *result = 0;
  helper_->CheckFramebufferStatus(
      target, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  GLenum result_value = *result;
  GPU_CLIENT_LOG("returned " << result_value);
  CheckGLError();
  return result_value;
}

void GLES2Implementation::Clear(GLbitfield mask) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glClear(" << mask << ")");
  helper_->Clear(mask);
  CheckGLError();
}

void GLES2Implementation::ClearColor(GLclampf red,
                                     GLclampf green,
                                     GLclampf blue,
                                     GLclampf alpha) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glClearColor(" << red << ", "
                     << green << ", " << blue << ", " << alpha << ")");
  helper_->ClearColor(red, green, blue, alpha);
  CheckGLError();
}

void GLES2Implementation::ClearDepthf(GLclampf depth) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glClearDepthf(" << depth << ")");
  helper_->ClearDepthf(depth);
  CheckGLError();
}

void GLES2Implementation::ClearStencil(GLint s) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glClearStencil(" << s << ")");
  helper_->ClearStencil(s);
  CheckGLError();
}

void GLES2Implementation::ColorMask(GLboolean red,
                                    GLboolean green,
                                    GLboolean blue,
                                    GLboolean alpha) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glColorMask("
                     << GLES2Util::GetStringBool(red) << ", "
                     << GLES2Util::GetStringBool(green) << ", "
                     << GLES2Util::GetStringBool(blue) << ", "
                     << GLES2Util::GetStringBool(alpha) << ")");
  helper_->ColorMask(red, green, blue, alpha);
  CheckGLError();
}

void GLES2Implementation::CompileShader(GLuint shader) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glCompileShader(" << shader
                     << ")");
  helper_->CompileShader(shader);
  CheckGLError();
}

void GLES2Implementation::CopyTexImage2D(GLenum target,
                                         GLint level,
                                         GLenum internalformat,
                                         GLint x,
                                         GLint y,
                                         GLsizei width,
                                         GLsizei height,
                                         GLint border) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG(
      "[" << GetLogPrefix() << "] glCopyTexImage2D("
          << GLES2Util::GetStringTextureTarget(target) << ", " << level << ", "
          << GLES2Util::GetStringTextureInternalFormat(internalformat) << ", "
          << x << ", " << y << ", " << width << ", " << height << ", " << border
          << ")");
  if (width < 0) {
    SetGLError(GL_INVALID_VALUE, "glCopyTexImage2D", "width < 0");
    return;
  }
  if (height < 0) {
    SetGLError(GL_INVALID_VALUE, "glCopyTexImage2D", "height < 0");
    return;
  }
  if (border != 0) {
    SetGLError(GL_INVALID_VALUE, "glCopyTexImage2D", "border GL_INVALID_VALUE");
    return;
  }
  helper_->CopyTexImage2D(target, level, internalformat, x, y, width, height);
  CheckGLError();
}

void GLES2Implementation::CopyTexSubImage2D(GLenum target,
                                            GLint level,
                                            GLint xoffset,
                                            GLint yoffset,
                                            GLint x,
                                            GLint y,
                                            GLsizei width,
                                            GLsizei height) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glCopyTexSubImage2D("
                     << GLES2Util::GetStringTextureTarget(target) << ", "
                     << level << ", " << xoffset << ", " << yoffset << ", " << x
                     << ", " << y << ", " << width << ", " << height << ")");
  if (width < 0) {
    SetGLError(GL_INVALID_VALUE, "glCopyTexSubImage2D", "width < 0");
    return;
  }
  if (height < 0) {
    SetGLError(GL_INVALID_VALUE, "glCopyTexSubImage2D", "height < 0");
    return;
  }
  helper_->CopyTexSubImage2D(
      target, level, xoffset, yoffset, x, y, width, height);
  CheckGLError();
}

GLuint GLES2Implementation::CreateProgram() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glCreateProgram("
                     << ")");
  GLuint client_id;
  GetIdHandler(id_namespaces::kProgramsAndShaders)
      ->MakeIds(this, 0, 1, &client_id);
  helper_->CreateProgram(client_id);
  GPU_CLIENT_LOG("returned " << client_id);
  CheckGLError();
  return client_id;
}

GLuint GLES2Implementation::CreateShader(GLenum type) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glCreateShader("
                     << GLES2Util::GetStringShaderType(type) << ")");
  GLuint client_id;
  GetIdHandler(id_namespaces::kProgramsAndShaders)
      ->MakeIds(this, 0, 1, &client_id);
  helper_->CreateShader(type, client_id);
  GPU_CLIENT_LOG("returned " << client_id);
  CheckGLError();
  return client_id;
}

void GLES2Implementation::CullFace(GLenum mode) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glCullFace("
                     << GLES2Util::GetStringFaceType(mode) << ")");
  helper_->CullFace(mode);
  CheckGLError();
}

void GLES2Implementation::DeleteBuffers(GLsizei n, const GLuint* buffers) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDeleteBuffers(" << n << ", "
                     << static_cast<const void*>(buffers) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << buffers[i]);
    }
  });
  GPU_CLIENT_DCHECK_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      DCHECK(buffers[i] != 0);
    }
  });
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glDeleteBuffers", "n < 0");
    return;
  }
  DeleteBuffersHelper(n, buffers);
  CheckGLError();
}

void GLES2Implementation::DeleteFramebuffers(GLsizei n,
                                             const GLuint* framebuffers) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDeleteFramebuffers(" << n << ", "
                     << static_cast<const void*>(framebuffers) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << framebuffers[i]);
    }
  });
  GPU_CLIENT_DCHECK_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      DCHECK(framebuffers[i] != 0);
    }
  });
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glDeleteFramebuffers", "n < 0");
    return;
  }
  DeleteFramebuffersHelper(n, framebuffers);
  CheckGLError();
}

void GLES2Implementation::DeleteProgram(GLuint program) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDeleteProgram(" << program
                     << ")");
  GPU_CLIENT_DCHECK(program != 0);
  DeleteProgramHelper(program);
  CheckGLError();
}

void GLES2Implementation::DeleteRenderbuffers(GLsizei n,
                                              const GLuint* renderbuffers) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDeleteRenderbuffers(" << n
                     << ", " << static_cast<const void*>(renderbuffers) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << renderbuffers[i]);
    }
  });
  GPU_CLIENT_DCHECK_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      DCHECK(renderbuffers[i] != 0);
    }
  });
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glDeleteRenderbuffers", "n < 0");
    return;
  }
  DeleteRenderbuffersHelper(n, renderbuffers);
  CheckGLError();
}

void GLES2Implementation::DeleteShader(GLuint shader) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDeleteShader(" << shader << ")");
  GPU_CLIENT_DCHECK(shader != 0);
  DeleteShaderHelper(shader);
  CheckGLError();
}

void GLES2Implementation::DeleteTextures(GLsizei n, const GLuint* textures) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDeleteTextures(" << n << ", "
                     << static_cast<const void*>(textures) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << textures[i]);
    }
  });
  GPU_CLIENT_DCHECK_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      DCHECK(textures[i] != 0);
    }
  });
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glDeleteTextures", "n < 0");
    return;
  }
  DeleteTexturesHelper(n, textures);
  CheckGLError();
}

void GLES2Implementation::DepthFunc(GLenum func) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDepthFunc("
                     << GLES2Util::GetStringCmpFunction(func) << ")");
  helper_->DepthFunc(func);
  CheckGLError();
}

void GLES2Implementation::DepthMask(GLboolean flag) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDepthMask("
                     << GLES2Util::GetStringBool(flag) << ")");
  helper_->DepthMask(flag);
  CheckGLError();
}

void GLES2Implementation::DepthRangef(GLclampf zNear, GLclampf zFar) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDepthRangef(" << zNear << ", "
                     << zFar << ")");
  helper_->DepthRangef(zNear, zFar);
  CheckGLError();
}

void GLES2Implementation::DetachShader(GLuint program, GLuint shader) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDetachShader(" << program << ", "
                     << shader << ")");
  helper_->DetachShader(program, shader);
  CheckGLError();
}

void GLES2Implementation::FramebufferRenderbuffer(GLenum target,
                                                  GLenum attachment,
                                                  GLenum renderbuffertarget,
                                                  GLuint renderbuffer) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glFramebufferRenderbuffer("
                     << GLES2Util::GetStringFrameBufferTarget(target) << ", "
                     << GLES2Util::GetStringAttachment(attachment) << ", "
                     << GLES2Util::GetStringRenderBufferTarget(
                            renderbuffertarget) << ", " << renderbuffer << ")");
  helper_->FramebufferRenderbuffer(
      target, attachment, renderbuffertarget, renderbuffer);
  CheckGLError();
}

void GLES2Implementation::FramebufferTexture2D(GLenum target,
                                               GLenum attachment,
                                               GLenum textarget,
                                               GLuint texture,
                                               GLint level) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glFramebufferTexture2D("
                     << GLES2Util::GetStringFrameBufferTarget(target) << ", "
                     << GLES2Util::GetStringAttachment(attachment) << ", "
                     << GLES2Util::GetStringTextureTarget(textarget) << ", "
                     << texture << ", " << level << ")");
  if (level != 0) {
    SetGLError(
        GL_INVALID_VALUE, "glFramebufferTexture2D", "level GL_INVALID_VALUE");
    return;
  }
  helper_->FramebufferTexture2D(target, attachment, textarget, texture);
  CheckGLError();
}

void GLES2Implementation::FrontFace(GLenum mode) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glFrontFace("
                     << GLES2Util::GetStringFaceMode(mode) << ")");
  helper_->FrontFace(mode);
  CheckGLError();
}

void GLES2Implementation::GenBuffers(GLsizei n, GLuint* buffers) {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGenBuffers(" << n << ", "
                     << static_cast<const void*>(buffers) << ")");
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glGenBuffers", "n < 0");
    return;
  }
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GetIdHandler(id_namespaces::kBuffers)->MakeIds(this, 0, n, buffers);
  GenBuffersHelper(n, buffers);
  helper_->GenBuffersImmediate(n, buffers);
  if (share_group_->bind_generates_resource())
    helper_->CommandBufferHelper::Flush();
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << buffers[i]);
    }
  });
  CheckGLError();
}

void GLES2Implementation::GenerateMipmap(GLenum target) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGenerateMipmap("
                     << GLES2Util::GetStringTextureBindTarget(target) << ")");
  helper_->GenerateMipmap(target);
  CheckGLError();
}

void GLES2Implementation::GenFramebuffers(GLsizei n, GLuint* framebuffers) {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGenFramebuffers(" << n << ", "
                     << static_cast<const void*>(framebuffers) << ")");
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glGenFramebuffers", "n < 0");
    return;
  }
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GetIdHandler(id_namespaces::kFramebuffers)->MakeIds(this, 0, n, framebuffers);
  GenFramebuffersHelper(n, framebuffers);
  helper_->GenFramebuffersImmediate(n, framebuffers);
  if (share_group_->bind_generates_resource())
    helper_->CommandBufferHelper::Flush();
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << framebuffers[i]);
    }
  });
  CheckGLError();
}

void GLES2Implementation::GenRenderbuffers(GLsizei n, GLuint* renderbuffers) {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGenRenderbuffers(" << n << ", "
                     << static_cast<const void*>(renderbuffers) << ")");
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glGenRenderbuffers", "n < 0");
    return;
  }
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GetIdHandler(id_namespaces::kRenderbuffers)
      ->MakeIds(this, 0, n, renderbuffers);
  GenRenderbuffersHelper(n, renderbuffers);
  helper_->GenRenderbuffersImmediate(n, renderbuffers);
  if (share_group_->bind_generates_resource())
    helper_->CommandBufferHelper::Flush();
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << renderbuffers[i]);
    }
  });
  CheckGLError();
}

void GLES2Implementation::GenTextures(GLsizei n, GLuint* textures) {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGenTextures(" << n << ", "
                     << static_cast<const void*>(textures) << ")");
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glGenTextures", "n < 0");
    return;
  }
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GetIdHandler(id_namespaces::kTextures)->MakeIds(this, 0, n, textures);
  GenTexturesHelper(n, textures);
  helper_->GenTexturesImmediate(n, textures);
  if (share_group_->bind_generates_resource())
    helper_->CommandBufferHelper::Flush();
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << textures[i]);
    }
  });
  CheckGLError();
}

void GLES2Implementation::GetBooleanv(GLenum pname, GLboolean* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_INITALIZATION(GLboolean, params);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetBooleanv("
                     << GLES2Util::GetStringGLState(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2Implementation::GetBooleanv");
  if (GetBooleanvHelper(pname, params)) {
    return;
  }
  typedef cmds::GetBooleanv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetBooleanv(pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32_t i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}
void GLES2Implementation::GetBufferParameteriv(GLenum target,
                                               GLenum pname,
                                               GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_INITALIZATION(GLint, params);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetBufferParameteriv("
                     << GLES2Util::GetStringBufferTarget(target) << ", "
                     << GLES2Util::GetStringBufferParameter(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2Implementation::GetBufferParameteriv");
  if (GetBufferParameterivHelper(target, pname, params)) {
    return;
  }
  typedef cmds::GetBufferParameteriv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetBufferParameteriv(
      target, pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32_t i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}
void GLES2Implementation::GetFloatv(GLenum pname, GLfloat* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetFloatv("
                     << GLES2Util::GetStringGLState(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2Implementation::GetFloatv");
  if (GetFloatvHelper(pname, params)) {
    return;
  }
  typedef cmds::GetFloatv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetFloatv(pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32_t i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}
void GLES2Implementation::GetFramebufferAttachmentParameteriv(GLenum target,
                                                              GLenum attachment,
                                                              GLenum pname,
                                                              GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_INITALIZATION(GLint, params);
  GPU_CLIENT_LOG("[" << GetLogPrefix()
                     << "] glGetFramebufferAttachmentParameteriv("
                     << GLES2Util::GetStringFrameBufferTarget(target) << ", "
                     << GLES2Util::GetStringAttachment(attachment) << ", "
                     << GLES2Util::GetStringFrameBufferParameter(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu",
               "GLES2Implementation::GetFramebufferAttachmentParameteriv");
  if (GetFramebufferAttachmentParameterivHelper(
          target, attachment, pname, params)) {
    return;
  }
  typedef cmds::GetFramebufferAttachmentParameteriv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetFramebufferAttachmentParameteriv(
      target, attachment, pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32_t i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}
void GLES2Implementation::GetIntegerv(GLenum pname, GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_INITALIZATION(GLint, params);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetIntegerv("
                     << GLES2Util::GetStringGLState(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2Implementation::GetIntegerv");
  if (GetIntegervHelper(pname, params)) {
    return;
  }
  typedef cmds::GetIntegerv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetIntegerv(pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32_t i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}
void GLES2Implementation::GetProgramiv(GLuint program,
                                       GLenum pname,
                                       GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_INITALIZATION(GLint, params);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetProgramiv(" << program << ", "
                     << GLES2Util::GetStringProgramParameter(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2Implementation::GetProgramiv");
  if (GetProgramivHelper(program, pname, params)) {
    return;
  }
  typedef cmds::GetProgramiv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetProgramiv(program, pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32_t i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}
void GLES2Implementation::GetProgramInfoLog(GLuint program,
                                            GLsizei bufsize,
                                            GLsizei* length,
                                            char* infolog) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_OPTIONAL_INITALIZATION(GLsizei, length);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetProgramInfoLog"
                     << "(" << program << ", " << bufsize << ", "
                     << static_cast<void*>(length) << ", "
                     << static_cast<void*>(infolog) << ")");
  helper_->SetBucketSize(kResultBucketId, 0);
  helper_->GetProgramInfoLog(program, kResultBucketId);
  std::string str;
  GLsizei max_size = 0;
  if (GetBucketAsString(kResultBucketId, &str)) {
    if (bufsize > 0) {
      max_size = std::min(static_cast<size_t>(bufsize) - 1, str.size());
      memcpy(infolog, str.c_str(), max_size);
      infolog[max_size] = '\0';
      GPU_CLIENT_LOG("------\n" << infolog << "\n------");
    }
  }
  if (length != NULL) {
    *length = max_size;
  }
  CheckGLError();
}
void GLES2Implementation::GetRenderbufferParameteriv(GLenum target,
                                                     GLenum pname,
                                                     GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_INITALIZATION(GLint, params);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetRenderbufferParameteriv("
                     << GLES2Util::GetStringRenderBufferTarget(target) << ", "
                     << GLES2Util::GetStringRenderBufferParameter(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2Implementation::GetRenderbufferParameteriv");
  if (GetRenderbufferParameterivHelper(target, pname, params)) {
    return;
  }
  typedef cmds::GetRenderbufferParameteriv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetRenderbufferParameteriv(
      target, pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32_t i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}
void GLES2Implementation::GetShaderiv(GLuint shader,
                                      GLenum pname,
                                      GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_INITALIZATION(GLint, params);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetShaderiv(" << shader << ", "
                     << GLES2Util::GetStringShaderParameter(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2Implementation::GetShaderiv");
  if (GetShaderivHelper(shader, pname, params)) {
    return;
  }
  typedef cmds::GetShaderiv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetShaderiv(shader, pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32_t i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}
void GLES2Implementation::GetShaderInfoLog(GLuint shader,
                                           GLsizei bufsize,
                                           GLsizei* length,
                                           char* infolog) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_OPTIONAL_INITALIZATION(GLsizei, length);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetShaderInfoLog"
                     << "(" << shader << ", " << bufsize << ", "
                     << static_cast<void*>(length) << ", "
                     << static_cast<void*>(infolog) << ")");
  helper_->SetBucketSize(kResultBucketId, 0);
  helper_->GetShaderInfoLog(shader, kResultBucketId);
  std::string str;
  GLsizei max_size = 0;
  if (GetBucketAsString(kResultBucketId, &str)) {
    if (bufsize > 0) {
      max_size = std::min(static_cast<size_t>(bufsize) - 1, str.size());
      memcpy(infolog, str.c_str(), max_size);
      infolog[max_size] = '\0';
      GPU_CLIENT_LOG("------\n" << infolog << "\n------");
    }
  }
  if (length != NULL) {
    *length = max_size;
  }
  CheckGLError();
}
void GLES2Implementation::GetShaderSource(GLuint shader,
                                          GLsizei bufsize,
                                          GLsizei* length,
                                          char* source) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_OPTIONAL_INITALIZATION(GLsizei, length);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetShaderSource"
                     << "(" << shader << ", " << bufsize << ", "
                     << static_cast<void*>(length) << ", "
                     << static_cast<void*>(source) << ")");
  helper_->SetBucketSize(kResultBucketId, 0);
  helper_->GetShaderSource(shader, kResultBucketId);
  std::string str;
  GLsizei max_size = 0;
  if (GetBucketAsString(kResultBucketId, &str)) {
    if (bufsize > 0) {
      max_size = std::min(static_cast<size_t>(bufsize) - 1, str.size());
      memcpy(source, str.c_str(), max_size);
      source[max_size] = '\0';
      GPU_CLIENT_LOG("------\n" << source << "\n------");
    }
  }
  if (length != NULL) {
    *length = max_size;
  }
  CheckGLError();
}
void GLES2Implementation::GetTexParameterfv(GLenum target,
                                            GLenum pname,
                                            GLfloat* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetTexParameterfv("
                     << GLES2Util::GetStringGetTexParamTarget(target) << ", "
                     << GLES2Util::GetStringTextureParameter(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2Implementation::GetTexParameterfv");
  if (GetTexParameterfvHelper(target, pname, params)) {
    return;
  }
  typedef cmds::GetTexParameterfv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetTexParameterfv(
      target, pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32_t i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}
void GLES2Implementation::GetTexParameteriv(GLenum target,
                                            GLenum pname,
                                            GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_INITALIZATION(GLint, params);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetTexParameteriv("
                     << GLES2Util::GetStringGetTexParamTarget(target) << ", "
                     << GLES2Util::GetStringTextureParameter(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2Implementation::GetTexParameteriv");
  if (GetTexParameterivHelper(target, pname, params)) {
    return;
  }
  typedef cmds::GetTexParameteriv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetTexParameteriv(
      target, pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32_t i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}
void GLES2Implementation::Hint(GLenum target, GLenum mode) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glHint("
                     << GLES2Util::GetStringHintTarget(target) << ", "
                     << GLES2Util::GetStringHintMode(mode) << ")");
  helper_->Hint(target, mode);
  CheckGLError();
}

GLboolean GLES2Implementation::IsBuffer(GLuint buffer) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  TRACE_EVENT0("gpu", "GLES2Implementation::IsBuffer");
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glIsBuffer(" << buffer << ")");
  typedef cmds::IsBuffer::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return GL_FALSE;
  }
  *result = 0;
  helper_->IsBuffer(buffer, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  GLboolean result_value = *result;
  GPU_CLIENT_LOG("returned " << result_value);
  CheckGLError();
  return result_value;
}

GLboolean GLES2Implementation::IsFramebuffer(GLuint framebuffer) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  TRACE_EVENT0("gpu", "GLES2Implementation::IsFramebuffer");
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glIsFramebuffer(" << framebuffer
                     << ")");
  typedef cmds::IsFramebuffer::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return GL_FALSE;
  }
  *result = 0;
  helper_->IsFramebuffer(framebuffer, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  GLboolean result_value = *result;
  GPU_CLIENT_LOG("returned " << result_value);
  CheckGLError();
  return result_value;
}

GLboolean GLES2Implementation::IsProgram(GLuint program) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  TRACE_EVENT0("gpu", "GLES2Implementation::IsProgram");
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glIsProgram(" << program << ")");
  typedef cmds::IsProgram::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return GL_FALSE;
  }
  *result = 0;
  helper_->IsProgram(program, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  GLboolean result_value = *result;
  GPU_CLIENT_LOG("returned " << result_value);
  CheckGLError();
  return result_value;
}

GLboolean GLES2Implementation::IsRenderbuffer(GLuint renderbuffer) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  TRACE_EVENT0("gpu", "GLES2Implementation::IsRenderbuffer");
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glIsRenderbuffer(" << renderbuffer
                     << ")");
  typedef cmds::IsRenderbuffer::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return GL_FALSE;
  }
  *result = 0;
  helper_->IsRenderbuffer(renderbuffer, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  GLboolean result_value = *result;
  GPU_CLIENT_LOG("returned " << result_value);
  CheckGLError();
  return result_value;
}

GLboolean GLES2Implementation::IsShader(GLuint shader) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  TRACE_EVENT0("gpu", "GLES2Implementation::IsShader");
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glIsShader(" << shader << ")");
  typedef cmds::IsShader::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return GL_FALSE;
  }
  *result = 0;
  helper_->IsShader(shader, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  GLboolean result_value = *result;
  GPU_CLIENT_LOG("returned " << result_value);
  CheckGLError();
  return result_value;
}

GLboolean GLES2Implementation::IsTexture(GLuint texture) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  TRACE_EVENT0("gpu", "GLES2Implementation::IsTexture");
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glIsTexture(" << texture << ")");
  typedef cmds::IsTexture::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return GL_FALSE;
  }
  *result = 0;
  helper_->IsTexture(texture, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  GLboolean result_value = *result;
  GPU_CLIENT_LOG("returned " << result_value);
  CheckGLError();
  return result_value;
}

void GLES2Implementation::LineWidth(GLfloat width) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glLineWidth(" << width << ")");
  helper_->LineWidth(width);
  CheckGLError();
}

void GLES2Implementation::PolygonOffset(GLfloat factor, GLfloat units) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glPolygonOffset(" << factor << ", "
                     << units << ")");
  helper_->PolygonOffset(factor, units);
  CheckGLError();
}

void GLES2Implementation::ReleaseShaderCompiler() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glReleaseShaderCompiler("
                     << ")");
  helper_->ReleaseShaderCompiler();
  CheckGLError();
}

void GLES2Implementation::RenderbufferStorage(GLenum target,
                                              GLenum internalformat,
                                              GLsizei width,
                                              GLsizei height) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glRenderbufferStorage("
                     << GLES2Util::GetStringRenderBufferTarget(target) << ", "
                     << GLES2Util::GetStringRenderBufferFormat(internalformat)
                     << ", " << width << ", " << height << ")");
  if (width < 0) {
    SetGLError(GL_INVALID_VALUE, "glRenderbufferStorage", "width < 0");
    return;
  }
  if (height < 0) {
    SetGLError(GL_INVALID_VALUE, "glRenderbufferStorage", "height < 0");
    return;
  }
  helper_->RenderbufferStorage(target, internalformat, width, height);
  CheckGLError();
}

void GLES2Implementation::SampleCoverage(GLclampf value, GLboolean invert) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glSampleCoverage(" << value << ", "
                     << GLES2Util::GetStringBool(invert) << ")");
  helper_->SampleCoverage(value, invert);
  CheckGLError();
}

void GLES2Implementation::Scissor(GLint x,
                                  GLint y,
                                  GLsizei width,
                                  GLsizei height) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glScissor(" << x << ", " << y
                     << ", " << width << ", " << height << ")");
  if (width < 0) {
    SetGLError(GL_INVALID_VALUE, "glScissor", "width < 0");
    return;
  }
  if (height < 0) {
    SetGLError(GL_INVALID_VALUE, "glScissor", "height < 0");
    return;
  }
  helper_->Scissor(x, y, width, height);
  CheckGLError();
}

void GLES2Implementation::StencilFunc(GLenum func, GLint ref, GLuint mask) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glStencilFunc("
                     << GLES2Util::GetStringCmpFunction(func) << ", " << ref
                     << ", " << mask << ")");
  helper_->StencilFunc(func, ref, mask);
  CheckGLError();
}

void GLES2Implementation::StencilFuncSeparate(GLenum face,
                                              GLenum func,
                                              GLint ref,
                                              GLuint mask) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glStencilFuncSeparate("
                     << GLES2Util::GetStringFaceType(face) << ", "
                     << GLES2Util::GetStringCmpFunction(func) << ", " << ref
                     << ", " << mask << ")");
  helper_->StencilFuncSeparate(face, func, ref, mask);
  CheckGLError();
}

void GLES2Implementation::StencilMask(GLuint mask) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glStencilMask(" << mask << ")");
  helper_->StencilMask(mask);
  CheckGLError();
}

void GLES2Implementation::StencilMaskSeparate(GLenum face, GLuint mask) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glStencilMaskSeparate("
                     << GLES2Util::GetStringFaceType(face) << ", " << mask
                     << ")");
  helper_->StencilMaskSeparate(face, mask);
  CheckGLError();
}

void GLES2Implementation::StencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glStencilOp("
                     << GLES2Util::GetStringStencilOp(fail) << ", "
                     << GLES2Util::GetStringStencilOp(zfail) << ", "
                     << GLES2Util::GetStringStencilOp(zpass) << ")");
  helper_->StencilOp(fail, zfail, zpass);
  CheckGLError();
}

void GLES2Implementation::StencilOpSeparate(GLenum face,
                                            GLenum fail,
                                            GLenum zfail,
                                            GLenum zpass) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glStencilOpSeparate("
                     << GLES2Util::GetStringFaceType(face) << ", "
                     << GLES2Util::GetStringStencilOp(fail) << ", "
                     << GLES2Util::GetStringStencilOp(zfail) << ", "
                     << GLES2Util::GetStringStencilOp(zpass) << ")");
  helper_->StencilOpSeparate(face, fail, zfail, zpass);
  CheckGLError();
}

void GLES2Implementation::TexParameterf(GLenum target,
                                        GLenum pname,
                                        GLfloat param) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glTexParameterf("
                     << GLES2Util::GetStringTextureBindTarget(target) << ", "
                     << GLES2Util::GetStringTextureParameter(pname) << ", "
                     << param << ")");
  helper_->TexParameterf(target, pname, param);
  CheckGLError();
}

void GLES2Implementation::TexParameterfv(GLenum target,
                                         GLenum pname,
                                         const GLfloat* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glTexParameterfv("
                     << GLES2Util::GetStringTextureBindTarget(target) << ", "
                     << GLES2Util::GetStringTextureParameter(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  GPU_CLIENT_LOG("values: " << params[0]);
  helper_->TexParameterfvImmediate(target, pname, params);
  CheckGLError();
}

void GLES2Implementation::TexParameteri(GLenum target,
                                        GLenum pname,
                                        GLint param) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glTexParameteri("
                     << GLES2Util::GetStringTextureBindTarget(target) << ", "
                     << GLES2Util::GetStringTextureParameter(pname) << ", "
                     << param << ")");
  helper_->TexParameteri(target, pname, param);
  CheckGLError();
}

void GLES2Implementation::TexParameteriv(GLenum target,
                                         GLenum pname,
                                         const GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glTexParameteriv("
                     << GLES2Util::GetStringTextureBindTarget(target) << ", "
                     << GLES2Util::GetStringTextureParameter(pname) << ", "
                     << static_cast<const void*>(params) << ")");
  GPU_CLIENT_LOG("values: " << params[0]);
  helper_->TexParameterivImmediate(target, pname, params);
  CheckGLError();
}

void GLES2Implementation::Uniform1f(GLint location, GLfloat x) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform1f(" << location << ", "
                     << x << ")");
  helper_->Uniform1f(location, x);
  CheckGLError();
}

void GLES2Implementation::Uniform1fv(GLint location,
                                     GLsizei count,
                                     const GLfloat* v) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform1fv(" << location << ", "
                     << count << ", " << static_cast<const void*>(v) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << v[0 + i * 1]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniform1fv", "count < 0");
    return;
  }
  helper_->Uniform1fvImmediate(location, count, v);
  CheckGLError();
}

void GLES2Implementation::Uniform1i(GLint location, GLint x) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform1i(" << location << ", "
                     << x << ")");
  helper_->Uniform1i(location, x);
  CheckGLError();
}

void GLES2Implementation::Uniform1iv(GLint location,
                                     GLsizei count,
                                     const GLint* v) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform1iv(" << location << ", "
                     << count << ", " << static_cast<const void*>(v) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << v[0 + i * 1]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniform1iv", "count < 0");
    return;
  }
  helper_->Uniform1ivImmediate(location, count, v);
  CheckGLError();
}

void GLES2Implementation::Uniform2f(GLint location, GLfloat x, GLfloat y) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform2f(" << location << ", "
                     << x << ", " << y << ")");
  helper_->Uniform2f(location, x, y);
  CheckGLError();
}

void GLES2Implementation::Uniform2fv(GLint location,
                                     GLsizei count,
                                     const GLfloat* v) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform2fv(" << location << ", "
                     << count << ", " << static_cast<const void*>(v) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << v[0 + i * 2] << ", " << v[1 + i * 2]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniform2fv", "count < 0");
    return;
  }
  helper_->Uniform2fvImmediate(location, count, v);
  CheckGLError();
}

void GLES2Implementation::Uniform2i(GLint location, GLint x, GLint y) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform2i(" << location << ", "
                     << x << ", " << y << ")");
  helper_->Uniform2i(location, x, y);
  CheckGLError();
}

void GLES2Implementation::Uniform2iv(GLint location,
                                     GLsizei count,
                                     const GLint* v) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform2iv(" << location << ", "
                     << count << ", " << static_cast<const void*>(v) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << v[0 + i * 2] << ", " << v[1 + i * 2]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniform2iv", "count < 0");
    return;
  }
  helper_->Uniform2ivImmediate(location, count, v);
  CheckGLError();
}

void GLES2Implementation::Uniform3f(GLint location,
                                    GLfloat x,
                                    GLfloat y,
                                    GLfloat z) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform3f(" << location << ", "
                     << x << ", " << y << ", " << z << ")");
  helper_->Uniform3f(location, x, y, z);
  CheckGLError();
}

void GLES2Implementation::Uniform3fv(GLint location,
                                     GLsizei count,
                                     const GLfloat* v) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform3fv(" << location << ", "
                     << count << ", " << static_cast<const void*>(v) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << v[0 + i * 3] << ", " << v[1 + i * 3]
                          << ", " << v[2 + i * 3]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniform3fv", "count < 0");
    return;
  }
  helper_->Uniform3fvImmediate(location, count, v);
  CheckGLError();
}

void GLES2Implementation::Uniform3i(GLint location, GLint x, GLint y, GLint z) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform3i(" << location << ", "
                     << x << ", " << y << ", " << z << ")");
  helper_->Uniform3i(location, x, y, z);
  CheckGLError();
}

void GLES2Implementation::Uniform3iv(GLint location,
                                     GLsizei count,
                                     const GLint* v) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform3iv(" << location << ", "
                     << count << ", " << static_cast<const void*>(v) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << v[0 + i * 3] << ", " << v[1 + i * 3]
                          << ", " << v[2 + i * 3]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniform3iv", "count < 0");
    return;
  }
  helper_->Uniform3ivImmediate(location, count, v);
  CheckGLError();
}

void GLES2Implementation::Uniform4f(GLint location,
                                    GLfloat x,
                                    GLfloat y,
                                    GLfloat z,
                                    GLfloat w) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform4f(" << location << ", "
                     << x << ", " << y << ", " << z << ", " << w << ")");
  helper_->Uniform4f(location, x, y, z, w);
  CheckGLError();
}

void GLES2Implementation::Uniform4fv(GLint location,
                                     GLsizei count,
                                     const GLfloat* v) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform4fv(" << location << ", "
                     << count << ", " << static_cast<const void*>(v) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << v[0 + i * 4] << ", " << v[1 + i * 4]
                          << ", " << v[2 + i * 4] << ", " << v[3 + i * 4]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniform4fv", "count < 0");
    return;
  }
  helper_->Uniform4fvImmediate(location, count, v);
  CheckGLError();
}

void GLES2Implementation::Uniform4i(GLint location,
                                    GLint x,
                                    GLint y,
                                    GLint z,
                                    GLint w) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform4i(" << location << ", "
                     << x << ", " << y << ", " << z << ", " << w << ")");
  helper_->Uniform4i(location, x, y, z, w);
  CheckGLError();
}

void GLES2Implementation::Uniform4iv(GLint location,
                                     GLsizei count,
                                     const GLint* v) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniform4iv(" << location << ", "
                     << count << ", " << static_cast<const void*>(v) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << v[0 + i * 4] << ", " << v[1 + i * 4]
                          << ", " << v[2 + i * 4] << ", " << v[3 + i * 4]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniform4iv", "count < 0");
    return;
  }
  helper_->Uniform4ivImmediate(location, count, v);
  CheckGLError();
}

void GLES2Implementation::UniformMatrix2fv(GLint location,
                                           GLsizei count,
                                           GLboolean transpose,
                                           const GLfloat* value) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniformMatrix2fv(" << location
                     << ", " << count << ", "
                     << GLES2Util::GetStringBool(transpose) << ", "
                     << static_cast<const void*>(value) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << value[0 + i * 4] << ", "
                          << value[1 + i * 4] << ", " << value[2 + i * 4]
                          << ", " << value[3 + i * 4]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniformMatrix2fv", "count < 0");
    return;
  }
  if (transpose != false) {
    SetGLError(
        GL_INVALID_VALUE, "glUniformMatrix2fv", "transpose GL_INVALID_VALUE");
    return;
  }
  helper_->UniformMatrix2fvImmediate(location, count, value);
  CheckGLError();
}

void GLES2Implementation::UniformMatrix3fv(GLint location,
                                           GLsizei count,
                                           GLboolean transpose,
                                           const GLfloat* value) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniformMatrix3fv(" << location
                     << ", " << count << ", "
                     << GLES2Util::GetStringBool(transpose) << ", "
                     << static_cast<const void*>(value) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << value[0 + i * 9] << ", "
                          << value[1 + i * 9] << ", " << value[2 + i * 9]
                          << ", " << value[3 + i * 9] << ", "
                          << value[4 + i * 9] << ", " << value[5 + i * 9]
                          << ", " << value[6 + i * 9] << ", "
                          << value[7 + i * 9] << ", " << value[8 + i * 9]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniformMatrix3fv", "count < 0");
    return;
  }
  if (transpose != false) {
    SetGLError(
        GL_INVALID_VALUE, "glUniformMatrix3fv", "transpose GL_INVALID_VALUE");
    return;
  }
  helper_->UniformMatrix3fvImmediate(location, count, value);
  CheckGLError();
}

void GLES2Implementation::UniformMatrix4fv(GLint location,
                                           GLsizei count,
                                           GLboolean transpose,
                                           const GLfloat* value) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUniformMatrix4fv(" << location
                     << ", " << count << ", "
                     << GLES2Util::GetStringBool(transpose) << ", "
                     << static_cast<const void*>(value) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG(
          "  " << i << ": " << value[0 + i * 16] << ", " << value[1 + i * 16]
               << ", " << value[2 + i * 16] << ", " << value[3 + i * 16] << ", "
               << value[4 + i * 16] << ", " << value[5 + i * 16] << ", "
               << value[6 + i * 16] << ", " << value[7 + i * 16] << ", "
               << value[8 + i * 16] << ", " << value[9 + i * 16] << ", "
               << value[10 + i * 16] << ", " << value[11 + i * 16] << ", "
               << value[12 + i * 16] << ", " << value[13 + i * 16] << ", "
               << value[14 + i * 16] << ", " << value[15 + i * 16]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glUniformMatrix4fv", "count < 0");
    return;
  }
  if (transpose != false) {
    SetGLError(
        GL_INVALID_VALUE, "glUniformMatrix4fv", "transpose GL_INVALID_VALUE");
    return;
  }
  helper_->UniformMatrix4fvImmediate(location, count, value);
  CheckGLError();
}

void GLES2Implementation::UseProgram(GLuint program) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUseProgram(" << program << ")");
  if (IsProgramReservedId(program)) {
    SetGLError(GL_INVALID_OPERATION, "UseProgram", "program reserved id");
    return;
  }
  if (UseProgramHelper(program)) {
    helper_->UseProgram(program);
  }
  CheckGLError();
}

void GLES2Implementation::ValidateProgram(GLuint program) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glValidateProgram(" << program
                     << ")");
  helper_->ValidateProgram(program);
  CheckGLError();
}

void GLES2Implementation::VertexAttrib1f(GLuint indx, GLfloat x) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glVertexAttrib1f(" << indx << ", "
                     << x << ")");
  helper_->VertexAttrib1f(indx, x);
  CheckGLError();
}

void GLES2Implementation::VertexAttrib1fv(GLuint indx, const GLfloat* values) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glVertexAttrib1fv(" << indx << ", "
                     << static_cast<const void*>(values) << ")");
  GPU_CLIENT_LOG("values: " << values[0]);
  helper_->VertexAttrib1fvImmediate(indx, values);
  CheckGLError();
}

void GLES2Implementation::VertexAttrib2f(GLuint indx, GLfloat x, GLfloat y) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glVertexAttrib2f(" << indx << ", "
                     << x << ", " << y << ")");
  helper_->VertexAttrib2f(indx, x, y);
  CheckGLError();
}

void GLES2Implementation::VertexAttrib2fv(GLuint indx, const GLfloat* values) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glVertexAttrib2fv(" << indx << ", "
                     << static_cast<const void*>(values) << ")");
  GPU_CLIENT_LOG("values: " << values[0] << ", " << values[1]);
  helper_->VertexAttrib2fvImmediate(indx, values);
  CheckGLError();
}

void GLES2Implementation::VertexAttrib3f(GLuint indx,
                                         GLfloat x,
                                         GLfloat y,
                                         GLfloat z) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glVertexAttrib3f(" << indx << ", "
                     << x << ", " << y << ", " << z << ")");
  helper_->VertexAttrib3f(indx, x, y, z);
  CheckGLError();
}

void GLES2Implementation::VertexAttrib3fv(GLuint indx, const GLfloat* values) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glVertexAttrib3fv(" << indx << ", "
                     << static_cast<const void*>(values) << ")");
  GPU_CLIENT_LOG("values: " << values[0] << ", " << values[1] << ", "
                            << values[2]);
  helper_->VertexAttrib3fvImmediate(indx, values);
  CheckGLError();
}

void GLES2Implementation::VertexAttrib4f(GLuint indx,
                                         GLfloat x,
                                         GLfloat y,
                                         GLfloat z,
                                         GLfloat w) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glVertexAttrib4f(" << indx << ", "
                     << x << ", " << y << ", " << z << ", " << w << ")");
  helper_->VertexAttrib4f(indx, x, y, z, w);
  CheckGLError();
}

void GLES2Implementation::VertexAttrib4fv(GLuint indx, const GLfloat* values) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glVertexAttrib4fv(" << indx << ", "
                     << static_cast<const void*>(values) << ")");
  GPU_CLIENT_LOG("values: " << values[0] << ", " << values[1] << ", "
                            << values[2] << ", " << values[3]);
  helper_->VertexAttrib4fvImmediate(indx, values);
  CheckGLError();
}

void GLES2Implementation::Viewport(GLint x,
                                   GLint y,
                                   GLsizei width,
                                   GLsizei height) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glViewport(" << x << ", " << y
                     << ", " << width << ", " << height << ")");
  if (width < 0) {
    SetGLError(GL_INVALID_VALUE, "glViewport", "width < 0");
    return;
  }
  if (height < 0) {
    SetGLError(GL_INVALID_VALUE, "glViewport", "height < 0");
    return;
  }
  helper_->Viewport(x, y, width, height);
  CheckGLError();
}

void GLES2Implementation::BlitFramebufferCHROMIUM(GLint srcX0,
                                                  GLint srcY0,
                                                  GLint srcX1,
                                                  GLint srcY1,
                                                  GLint dstX0,
                                                  GLint dstY0,
                                                  GLint dstX1,
                                                  GLint dstY1,
                                                  GLbitfield mask,
                                                  GLenum filter) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBlitFramebufferCHROMIUM("
                     << srcX0 << ", " << srcY0 << ", " << srcX1 << ", " << srcY1
                     << ", " << dstX0 << ", " << dstY0 << ", " << dstX1 << ", "
                     << dstY1 << ", " << mask << ", "
                     << GLES2Util::GetStringBlitFilter(filter) << ")");
  helper_->BlitFramebufferCHROMIUM(
      srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
  CheckGLError();
}

void GLES2Implementation::RenderbufferStorageMultisampleCHROMIUM(
    GLenum target,
    GLsizei samples,
    GLenum internalformat,
    GLsizei width,
    GLsizei height) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG(
      "[" << GetLogPrefix() << "] glRenderbufferStorageMultisampleCHROMIUM("
          << GLES2Util::GetStringRenderBufferTarget(target) << ", " << samples
          << ", " << GLES2Util::GetStringRenderBufferFormat(internalformat)
          << ", " << width << ", " << height << ")");
  if (samples < 0) {
    SetGLError(GL_INVALID_VALUE,
               "glRenderbufferStorageMultisampleCHROMIUM",
               "samples < 0");
    return;
  }
  if (width < 0) {
    SetGLError(GL_INVALID_VALUE,
               "glRenderbufferStorageMultisampleCHROMIUM",
               "width < 0");
    return;
  }
  if (height < 0) {
    SetGLError(GL_INVALID_VALUE,
               "glRenderbufferStorageMultisampleCHROMIUM",
               "height < 0");
    return;
  }
  helper_->RenderbufferStorageMultisampleCHROMIUM(
      target, samples, internalformat, width, height);
  CheckGLError();
}

void GLES2Implementation::RenderbufferStorageMultisampleEXT(
    GLenum target,
    GLsizei samples,
    GLenum internalformat,
    GLsizei width,
    GLsizei height) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG(
      "[" << GetLogPrefix() << "] glRenderbufferStorageMultisampleEXT("
          << GLES2Util::GetStringRenderBufferTarget(target) << ", " << samples
          << ", " << GLES2Util::GetStringRenderBufferFormat(internalformat)
          << ", " << width << ", " << height << ")");
  if (samples < 0) {
    SetGLError(
        GL_INVALID_VALUE, "glRenderbufferStorageMultisampleEXT", "samples < 0");
    return;
  }
  if (width < 0) {
    SetGLError(
        GL_INVALID_VALUE, "glRenderbufferStorageMultisampleEXT", "width < 0");
    return;
  }
  if (height < 0) {
    SetGLError(
        GL_INVALID_VALUE, "glRenderbufferStorageMultisampleEXT", "height < 0");
    return;
  }
  helper_->RenderbufferStorageMultisampleEXT(
      target, samples, internalformat, width, height);
  CheckGLError();
}

void GLES2Implementation::FramebufferTexture2DMultisampleEXT(GLenum target,
                                                             GLenum attachment,
                                                             GLenum textarget,
                                                             GLuint texture,
                                                             GLint level,
                                                             GLsizei samples) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix()
                     << "] glFramebufferTexture2DMultisampleEXT("
                     << GLES2Util::GetStringFrameBufferTarget(target) << ", "
                     << GLES2Util::GetStringAttachment(attachment) << ", "
                     << GLES2Util::GetStringTextureTarget(textarget) << ", "
                     << texture << ", " << level << ", " << samples << ")");
  if (level != 0) {
    SetGLError(GL_INVALID_VALUE,
               "glFramebufferTexture2DMultisampleEXT",
               "level GL_INVALID_VALUE");
    return;
  }
  if (samples < 0) {
    SetGLError(GL_INVALID_VALUE,
               "glFramebufferTexture2DMultisampleEXT",
               "samples < 0");
    return;
  }
  helper_->FramebufferTexture2DMultisampleEXT(
      target, attachment, textarget, texture, samples);
  CheckGLError();
}

void GLES2Implementation::TexStorage2DEXT(GLenum target,
                                          GLsizei levels,
                                          GLenum internalFormat,
                                          GLsizei width,
                                          GLsizei height) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG(
      "[" << GetLogPrefix() << "] glTexStorage2DEXT("
          << GLES2Util::GetStringTextureTarget(target) << ", " << levels << ", "
          << GLES2Util::GetStringTextureInternalFormatStorage(internalFormat)
          << ", " << width << ", " << height << ")");
  if (levels < 0) {
    SetGLError(GL_INVALID_VALUE, "glTexStorage2DEXT", "levels < 0");
    return;
  }
  if (width < 0) {
    SetGLError(GL_INVALID_VALUE, "glTexStorage2DEXT", "width < 0");
    return;
  }
  if (height < 0) {
    SetGLError(GL_INVALID_VALUE, "glTexStorage2DEXT", "height < 0");
    return;
  }
  helper_->TexStorage2DEXT(target, levels, internalFormat, width, height);
  CheckGLError();
}

void GLES2Implementation::GenQueriesEXT(GLsizei n, GLuint* queries) {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGenQueriesEXT(" << n << ", "
                     << static_cast<const void*>(queries) << ")");
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glGenQueriesEXT", "n < 0");
    return;
  }
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GetIdHandler(id_namespaces::kQueries)->MakeIds(this, 0, n, queries);
  GenQueriesEXTHelper(n, queries);
  helper_->GenQueriesEXTImmediate(n, queries);
  if (share_group_->bind_generates_resource())
    helper_->CommandBufferHelper::Flush();
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << queries[i]);
    }
  });
  CheckGLError();
}

void GLES2Implementation::DeleteQueriesEXT(GLsizei n, const GLuint* queries) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDeleteQueriesEXT(" << n << ", "
                     << static_cast<const void*>(queries) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << queries[i]);
    }
  });
  GPU_CLIENT_DCHECK_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      DCHECK(queries[i] != 0);
    }
  });
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glDeleteQueriesEXT", "n < 0");
    return;
  }
  DeleteQueriesEXTHelper(n, queries);
  CheckGLError();
}

void GLES2Implementation::GenVertexArraysOES(GLsizei n, GLuint* arrays) {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGenVertexArraysOES(" << n << ", "
                     << static_cast<const void*>(arrays) << ")");
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glGenVertexArraysOES", "n < 0");
    return;
  }
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GetIdHandler(id_namespaces::kVertexArrays)->MakeIds(this, 0, n, arrays);
  GenVertexArraysOESHelper(n, arrays);
  helper_->GenVertexArraysOESImmediate(n, arrays);
  if (share_group_->bind_generates_resource())
    helper_->CommandBufferHelper::Flush();
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << arrays[i]);
    }
  });
  CheckGLError();
}

void GLES2Implementation::DeleteVertexArraysOES(GLsizei n,
                                                const GLuint* arrays) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDeleteVertexArraysOES(" << n
                     << ", " << static_cast<const void*>(arrays) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << arrays[i]);
    }
  });
  GPU_CLIENT_DCHECK_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      DCHECK(arrays[i] != 0);
    }
  });
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glDeleteVertexArraysOES", "n < 0");
    return;
  }
  DeleteVertexArraysOESHelper(n, arrays);
  CheckGLError();
}

GLboolean GLES2Implementation::IsVertexArrayOES(GLuint array) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  TRACE_EVENT0("gpu", "GLES2Implementation::IsVertexArrayOES");
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glIsVertexArrayOES(" << array
                     << ")");
  typedef cmds::IsVertexArrayOES::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return GL_FALSE;
  }
  *result = 0;
  helper_->IsVertexArrayOES(array, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  GLboolean result_value = *result;
  GPU_CLIENT_LOG("returned " << result_value);
  CheckGLError();
  return result_value;
}

void GLES2Implementation::BindVertexArrayOES(GLuint array) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBindVertexArrayOES(" << array
                     << ")");
  if (IsVertexArrayReservedId(array)) {
    SetGLError(GL_INVALID_OPERATION, "BindVertexArrayOES", "array reserved id");
    return;
  }
  if (BindVertexArrayOESHelper(array)) {
    helper_->BindVertexArrayOES(array);
  }
  CheckGLError();
}

void GLES2Implementation::GetTranslatedShaderSourceANGLE(GLuint shader,
                                                         GLsizei bufsize,
                                                         GLsizei* length,
                                                         char* source) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_OPTIONAL_INITALIZATION(GLsizei, length);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetTranslatedShaderSourceANGLE"
                     << "(" << shader << ", " << bufsize << ", "
                     << static_cast<void*>(length) << ", "
                     << static_cast<void*>(source) << ")");
  helper_->SetBucketSize(kResultBucketId, 0);
  helper_->GetTranslatedShaderSourceANGLE(shader, kResultBucketId);
  std::string str;
  GLsizei max_size = 0;
  if (GetBucketAsString(kResultBucketId, &str)) {
    if (bufsize > 0) {
      max_size = std::min(static_cast<size_t>(bufsize) - 1, str.size());
      memcpy(source, str.c_str(), max_size);
      source[max_size] = '\0';
      GPU_CLIENT_LOG("------\n" << source << "\n------");
    }
  }
  if (length != NULL) {
    *length = max_size;
  }
  CheckGLError();
}
void GLES2Implementation::TexImageIOSurface2DCHROMIUM(GLenum target,
                                                      GLsizei width,
                                                      GLsizei height,
                                                      GLuint ioSurfaceId,
                                                      GLuint plane) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glTexImageIOSurface2DCHROMIUM("
                     << GLES2Util::GetStringTextureBindTarget(target) << ", "
                     << width << ", " << height << ", " << ioSurfaceId << ", "
                     << plane << ")");
  if (width < 0) {
    SetGLError(GL_INVALID_VALUE, "glTexImageIOSurface2DCHROMIUM", "width < 0");
    return;
  }
  if (height < 0) {
    SetGLError(GL_INVALID_VALUE, "glTexImageIOSurface2DCHROMIUM", "height < 0");
    return;
  }
  helper_->TexImageIOSurface2DCHROMIUM(
      target, width, height, ioSurfaceId, plane);
  CheckGLError();
}

void GLES2Implementation::CopyTextureCHROMIUM(GLenum target,
                                              GLenum source_id,
                                              GLenum dest_id,
                                              GLint level,
                                              GLint internalformat,
                                              GLenum dest_type) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glCopyTextureCHROMIUM("
                     << GLES2Util::GetStringEnum(target) << ", "
                     << GLES2Util::GetStringEnum(source_id) << ", "
                     << GLES2Util::GetStringEnum(dest_id) << ", " << level
                     << ", " << internalformat << ", "
                     << GLES2Util::GetStringPixelType(dest_type) << ")");
  helper_->CopyTextureCHROMIUM(
      target, source_id, dest_id, level, internalformat, dest_type);
  CheckGLError();
}

void GLES2Implementation::BindTexImage2DCHROMIUM(GLenum target, GLint imageId) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBindTexImage2DCHROMIUM("
                     << GLES2Util::GetStringTextureBindTarget(target) << ", "
                     << imageId << ")");
  helper_->BindTexImage2DCHROMIUM(target, imageId);
  CheckGLError();
}

void GLES2Implementation::ReleaseTexImage2DCHROMIUM(GLenum target,
                                                    GLint imageId) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glReleaseTexImage2DCHROMIUM("
                     << GLES2Util::GetStringTextureBindTarget(target) << ", "
                     << imageId << ")");
  helper_->ReleaseTexImage2DCHROMIUM(target, imageId);
  CheckGLError();
}

void GLES2Implementation::DiscardFramebufferEXT(GLenum target,
                                                GLsizei count,
                                                const GLenum* attachments) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDiscardFramebufferEXT("
                     << GLES2Util::GetStringEnum(target) << ", " << count
                     << ", " << static_cast<const void*>(attachments) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << attachments[0 + i * 1]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glDiscardFramebufferEXT", "count < 0");
    return;
  }
  helper_->DiscardFramebufferEXTImmediate(target, count, attachments);
  CheckGLError();
}

void GLES2Implementation::LoseContextCHROMIUM(GLenum current, GLenum other) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glLoseContextCHROMIUM("
                     << GLES2Util::GetStringResetStatus(current) << ", "
                     << GLES2Util::GetStringResetStatus(other) << ")");
  helper_->LoseContextCHROMIUM(current, other);
  CheckGLError();
}

void GLES2Implementation::WaitSyncPointCHROMIUM(GLuint sync_point) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glWaitSyncPointCHROMIUM("
                     << sync_point << ")");
  helper_->WaitSyncPointCHROMIUM(sync_point);
  CheckGLError();
}

void GLES2Implementation::DrawBuffersEXT(GLsizei count, const GLenum* bufs) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDrawBuffersEXT(" << count << ", "
                     << static_cast<const void*>(bufs) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < count; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << bufs[0 + i * 1]);
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glDrawBuffersEXT", "count < 0");
    return;
  }
  helper_->DrawBuffersEXTImmediate(count, bufs);
  CheckGLError();
}

void GLES2Implementation::DiscardBackbufferCHROMIUM() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDiscardBackbufferCHROMIUM("
                     << ")");
  helper_->DiscardBackbufferCHROMIUM();
  CheckGLError();
}

void GLES2Implementation::ScheduleOverlayPlaneCHROMIUM(
    GLint plane_z_order,
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
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG(
      "[" << GetLogPrefix() << "] glScheduleOverlayPlaneCHROMIUM("
          << plane_z_order << ", " << GLES2Util::GetStringEnum(plane_transform)
          << ", " << overlay_texture_id << ", " << bounds_x << ", " << bounds_y
          << ", " << bounds_width << ", " << bounds_height << ", " << uv_x
          << ", " << uv_y << ", " << uv_width << ", " << uv_height << ")");
  helper_->ScheduleOverlayPlaneCHROMIUM(plane_z_order,
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
  CheckGLError();
}

#endif  // GPU_COMMAND_BUFFER_CLIENT_GLES2_IMPLEMENTATION_IMPL_AUTOGEN_H_
