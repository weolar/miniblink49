// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gles2_cmd_copy_texture_chromium.h"

#include <algorithm>

#include "base/basictypes.h"
#include "gpu/command_buffer/service/gl_utils.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"

#define SHADER(src)                     \
  "#ifdef GL_ES\n"                      \
  "precision mediump float;\n"          \
  "#define TexCoordPrecision mediump\n" \
  "#else\n"                             \
  "#define TexCoordPrecision\n"         \
  "#endif\n" #src
#define SHADER_2D(src)              \
  "#define SamplerType sampler2D\n" \
  "#define TextureLookup texture2D\n" SHADER(src)
#define SHADER_RECTANGLE_ARB(src)     \
  "#define SamplerType samplerRect\n" \
  "#define TextureLookup textureRect\n" SHADER(src)
#define SHADER_EXTERNAL_OES(src)                     \
  "#extension GL_OES_EGL_image_external : require\n" \
  "#define SamplerType samplerExternalOES\n"         \
  "#define TextureLookup texture2D\n" SHADER(src)
#define FRAGMENT_SHADERS(src) \
  SHADER_2D(src), SHADER_RECTANGLE_ARB(src), SHADER_EXTERNAL_OES(src)

namespace {

enum VertexShaderId {
  VERTEX_SHADER_COPY_TEXTURE,
  VERTEX_SHADER_COPY_TEXTURE_FLIP_Y,
  NUM_VERTEX_SHADERS,
};

enum FragmentShaderId {
  FRAGMENT_SHADER_COPY_TEXTURE_2D,
  FRAGMENT_SHADER_COPY_TEXTURE_RECTANGLE_ARB,
  FRAGMENT_SHADER_COPY_TEXTURE_EXTERNAL_OES,
  FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_2D,
  FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_RECTANGLE_ARB,
  FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_EXTERNAL_OES,
  FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_2D,
  FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_RECTANGLE_ARB,
  FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_EXTERNAL_OES,
  NUM_FRAGMENT_SHADERS,
};

const char* vertex_shader_source[NUM_VERTEX_SHADERS] = {
  // VERTEX_SHADER_COPY_TEXTURE
  SHADER(
    uniform mat4 u_matrix;
    uniform vec2 u_half_size;
    attribute vec4 a_position;
    varying TexCoordPrecision vec2 v_uv;
    void main(void) {
      gl_Position = u_matrix * a_position;
      v_uv = a_position.xy * vec2(u_half_size.s, u_half_size.t) +
             vec2(u_half_size.s, u_half_size.t);
    }),
  // VERTEX_SHADER_COPY_TEXTURE_FLIP_Y
  SHADER(
    uniform mat4 u_matrix;
    uniform vec2 u_half_size;
    attribute vec4 a_position;
    varying TexCoordPrecision vec2 v_uv;
    void main(void) {
      gl_Position = u_matrix * a_position;
      v_uv = a_position.xy * vec2(u_half_size.s, -u_half_size.t) +
             vec2(u_half_size.s, u_half_size.t);
    }),
};

const char* fragment_shader_source[NUM_FRAGMENT_SHADERS] = {
  // FRAGMENT_SHADER_COPY_TEXTURE_*
  FRAGMENT_SHADERS(
    uniform SamplerType u_sampler;
    varying TexCoordPrecision vec2 v_uv;
    void main(void) {
      gl_FragColor = TextureLookup(u_sampler, v_uv.st);
    }),
  // FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_*
  FRAGMENT_SHADERS(
    uniform SamplerType u_sampler;
    varying TexCoordPrecision vec2 v_uv;
    void main(void) {
      gl_FragColor = TextureLookup(u_sampler, v_uv.st);
      gl_FragColor.rgb *= gl_FragColor.a;
    }),
  // FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_*
  FRAGMENT_SHADERS(
    uniform SamplerType u_sampler;
    varying TexCoordPrecision vec2 v_uv;
    void main(void) {
      gl_FragColor = TextureLookup(u_sampler, v_uv.st);
      if (gl_FragColor.a > 0.0)
        gl_FragColor.rgb /= gl_FragColor.a;
    }),
};

// Returns the correct vertex shader id to evaluate the copy operation for
// the CHROMIUM_flipy setting.
VertexShaderId GetVertexShaderId(bool flip_y) {
  // bit 0: flip y
  static VertexShaderId shader_ids[] = {
      VERTEX_SHADER_COPY_TEXTURE,
      VERTEX_SHADER_COPY_TEXTURE_FLIP_Y,
  };

  unsigned index = flip_y ? 1 : 0;
  return shader_ids[index];
}

// Returns the correct fragment shader id to evaluate the copy operation for
// the premultiply alpha pixel store settings and target.
FragmentShaderId GetFragmentShaderId(bool premultiply_alpha,
                                     bool unpremultiply_alpha,
                                     GLenum target) {
  enum {
    SAMPLER_2D,
    SAMPLER_RECTANGLE_ARB,
    SAMPLER_EXTERNAL_OES,
    NUM_SAMPLERS
  };

  // bit 0: premultiply alpha
  // bit 1: unpremultiply alpha
  static FragmentShaderId shader_ids[][NUM_SAMPLERS] = {
      {
       FRAGMENT_SHADER_COPY_TEXTURE_2D,
       FRAGMENT_SHADER_COPY_TEXTURE_RECTANGLE_ARB,
       FRAGMENT_SHADER_COPY_TEXTURE_EXTERNAL_OES,
      },
      {
       FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_2D,
       FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_RECTANGLE_ARB,
       FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_EXTERNAL_OES,
      },
      {
       FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_2D,
       FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_RECTANGLE_ARB,
       FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_EXTERNAL_OES,
      },
      {
       FRAGMENT_SHADER_COPY_TEXTURE_2D,
       FRAGMENT_SHADER_COPY_TEXTURE_RECTANGLE_ARB,
       FRAGMENT_SHADER_COPY_TEXTURE_EXTERNAL_OES,
      }};

  unsigned index = (premultiply_alpha   ? (1 << 0) : 0) |
                   (unpremultiply_alpha ? (1 << 1) : 0);

  switch (target) {
    case GL_TEXTURE_2D:
      return shader_ids[index][SAMPLER_2D];
    case GL_TEXTURE_RECTANGLE_ARB:
      return shader_ids[index][SAMPLER_RECTANGLE_ARB];
    case GL_TEXTURE_EXTERNAL_OES:
      return shader_ids[index][SAMPLER_EXTERNAL_OES];
    default:
      break;
  }

  NOTREACHED();
  return shader_ids[index][SAMPLER_2D];
}

void CompileShader(GLuint shader, const char* shader_source) {
  glShaderSource(shader, 1, &shader_source, 0);
  glCompileShader(shader);
#ifndef NDEBUG
  GLint compile_status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
  if (GL_TRUE != compile_status)
    DLOG(ERROR) << "CopyTextureCHROMIUM: shader compilation failure.";
#endif
}

void DeleteShader(GLuint shader) {
  if (shader)
    glDeleteShader(shader);
}

}  // namespace

namespace gpu {

CopyTextureCHROMIUMResourceManager::CopyTextureCHROMIUMResourceManager()
    : initialized_(false),
      vertex_shaders_(NUM_VERTEX_SHADERS, 0u),
      fragment_shaders_(NUM_FRAGMENT_SHADERS, 0u),
      buffer_id_(0u),
      framebuffer_(0u) {}

CopyTextureCHROMIUMResourceManager::~CopyTextureCHROMIUMResourceManager() {}

void CopyTextureCHROMIUMResourceManager::Initialize(
    const gles2::GLES2Decoder* decoder) {
  COMPILE_ASSERT(
      kVertexPositionAttrib == 0u,
      Position_attribs_must_be_0);

  // Initialize all of the GPU resources required to perform the copy.
  glGenBuffersARB(1, &buffer_id_);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id_);
  const GLfloat kQuadVertices[] = {-1.0f, -1.0f,
                                    1.0f, -1.0f,
                                    1.0f,  1.0f,
                                   -1.0f,  1.0f};
  glBufferData(
      GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);

  glGenFramebuffersEXT(1, &framebuffer_);

  decoder->RestoreBufferBindings();

  initialized_ = true;
}

void CopyTextureCHROMIUMResourceManager::Destroy() {
  if (!initialized_)
    return;

  glDeleteFramebuffersEXT(1, &framebuffer_);

  std::for_each(vertex_shaders_.begin(), vertex_shaders_.end(), DeleteShader);
  std::for_each(
      fragment_shaders_.begin(), fragment_shaders_.end(), DeleteShader);

  for (ProgramMap::const_iterator it = programs_.begin(); it != programs_.end();
       ++it) {
    const ProgramInfo& info = it->second;
    glDeleteProgram(info.program);
  }

  glDeleteBuffersARB(1, &buffer_id_);
}

void CopyTextureCHROMIUMResourceManager::DoCopyTexture(
    const gles2::GLES2Decoder* decoder,
    GLenum source_target,
    GLenum dest_target,
    GLuint source_id,
    GLuint dest_id,
    GLint level,
    GLsizei width,
    GLsizei height,
    bool flip_y,
    bool premultiply_alpha,
    bool unpremultiply_alpha) {
  // Use default transform matrix if no transform passed in.
  const static GLfloat default_matrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 1.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 1.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 1.0f};
  DoCopyTextureWithTransform(decoder, source_target, dest_target, source_id,
      dest_id, level, width, height, flip_y, premultiply_alpha,
      unpremultiply_alpha, default_matrix);
}

void CopyTextureCHROMIUMResourceManager::DoCopyTextureWithTransform(
    const gles2::GLES2Decoder* decoder,
    GLenum source_target,
    GLenum dest_target,
    GLuint source_id,
    GLuint dest_id,
    GLint level,
    GLsizei width,
    GLsizei height,
    bool flip_y,
    bool premultiply_alpha,
    bool unpremultiply_alpha,
    const GLfloat transform_matrix[16]) {
  DCHECK(source_target == GL_TEXTURE_2D ||
         source_target == GL_TEXTURE_RECTANGLE_ARB ||
         source_target == GL_TEXTURE_EXTERNAL_OES);
  if (!initialized_) {
    DLOG(ERROR) << "CopyTextureCHROMIUM: Uninitialized manager.";
    return;
  }

  VertexShaderId vertex_shader_id = GetVertexShaderId(flip_y);
  DCHECK_LT(static_cast<size_t>(vertex_shader_id), vertex_shaders_.size());
  GLuint* vertex_shader = &vertex_shaders_[vertex_shader_id];
  if (!*vertex_shader) {
    *vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    CompileShader(*vertex_shader, vertex_shader_source[vertex_shader_id]);
  }

  FragmentShaderId fragment_shader_id = GetFragmentShaderId(
      premultiply_alpha, unpremultiply_alpha, source_target);
  DCHECK_LT(static_cast<size_t>(fragment_shader_id), fragment_shaders_.size());
  GLuint* fragment_shader = &fragment_shaders_[fragment_shader_id];
  if (!*fragment_shader) {
    *fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    CompileShader(*fragment_shader, fragment_shader_source[fragment_shader_id]);
  }

  ProgramMapKey key(vertex_shader_id, fragment_shader_id);
  ProgramInfo* info = &programs_[key];
  // Create program if necessary.
  if (!info->program) {
    info->program = glCreateProgram();
    glAttachShader(info->program, *vertex_shader);
    glAttachShader(info->program, *fragment_shader);
    glBindAttribLocation(info->program, kVertexPositionAttrib, "a_position");
    glLinkProgram(info->program);
#ifndef NDEBUG
    GLint linked;
    glGetProgramiv(info->program, GL_LINK_STATUS, &linked);
    if (!linked)
      DLOG(ERROR) << "CopyTextureCHROMIUM: program link failure.";
#endif
    info->matrix_handle = glGetUniformLocation(info->program, "u_matrix");
    info->half_size_handle = glGetUniformLocation(info->program, "u_half_size");
    info->sampler_handle = glGetUniformLocation(info->program, "u_sampler");
  }
  glUseProgram(info->program);

#ifndef NDEBUG
  glValidateProgram(info->program);
  GLint validation_status;
  glGetProgramiv(info->program, GL_VALIDATE_STATUS, &validation_status);
  if (GL_TRUE != validation_status) {
    DLOG(ERROR) << "CopyTextureCHROMIUM: Invalid shader.";
    return;
  }
#endif

  glUniformMatrix4fv(info->matrix_handle, 1, GL_FALSE, transform_matrix);
  if (source_target == GL_TEXTURE_RECTANGLE_ARB)
    glUniform2f(info->half_size_handle, width / 2.0f, height / 2.0f);
  else
    glUniform2f(info->half_size_handle, 0.5f, 0.5f);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, dest_id);
  // NVidia drivers require texture settings to be a certain way
  // or they won't report FRAMEBUFFER_COMPLETE.
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer_);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dest_target,
                            dest_id, level);

#ifndef NDEBUG
  GLenum fb_status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
  if (GL_FRAMEBUFFER_COMPLETE != fb_status) {
    DLOG(ERROR) << "CopyTextureCHROMIUM: Incomplete framebuffer.";
  } else
#endif
  {
    decoder->ClearAllAttributes();
    glEnableVertexAttribArray(kVertexPositionAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, buffer_id_);
    glVertexAttribPointer(kVertexPositionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUniform1i(info->sampler_handle, 0);

    glBindTexture(source_target, source_id);
    glTexParameterf(source_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(source_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(source_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(source_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);
    glDisable(GL_BLEND);

    glViewport(0, 0, width, height);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }

  decoder->RestoreAllAttributes();
  decoder->RestoreTextureState(source_id);
  decoder->RestoreTextureState(dest_id);
  decoder->RestoreTextureUnitBindings(0);
  decoder->RestoreActiveTexture();
  decoder->RestoreProgramBindings();
  decoder->RestoreBufferBindings();
  decoder->RestoreFramebufferBindings();
  decoder->RestoreGlobalState();
}

}  // namespace gpu
