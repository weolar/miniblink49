// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_CLIENT_PROGRAM_INFO_MANAGER_H_
#define GPU_COMMAND_BUFFER_CLIENT_PROGRAM_INFO_MANAGER_H_

#include <GLES2/gl2.h>
#include "gles2_impl_export.h"

namespace gpu {
namespace gles2 {

class GLES2Implementation;

// Manages info about OpenGL ES Programs.
class GLES2_IMPL_EXPORT ProgramInfoManager {
 public:
  virtual ~ProgramInfoManager();

  static ProgramInfoManager* Create(bool shared_resources_across_processes);

  virtual void CreateInfo(GLuint program) = 0;

  virtual void DeleteInfo(GLuint program) = 0;

  virtual bool GetProgramiv(
      GLES2Implementation* gl, GLuint program, GLenum pname, GLint* params) = 0;

  virtual GLint GetAttribLocation(
      GLES2Implementation* gl, GLuint program, const char* name) = 0;

  virtual GLint GetUniformLocation(
      GLES2Implementation* gl, GLuint program, const char* name) = 0;

  virtual bool GetActiveAttrib(
      GLES2Implementation* gl,
      GLuint program, GLuint index, GLsizei bufsize, GLsizei* length,
      GLint* size, GLenum* type, char* name) = 0;

  virtual bool GetActiveUniform(
      GLES2Implementation* gl,
      GLuint program, GLuint index, GLsizei bufsize, GLsizei* length,
      GLint* size, GLenum* type, char* name) = 0;

 protected:
  ProgramInfoManager();
};

}  // namespace gles2
}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_CLIENT_PROGRAM_INFO_MANAGER_H_
