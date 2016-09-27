// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/error_state.h"

#include <string>

#include "base/strings/stringprintf.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/service/logger.h"
#include "ui/gl/gl_bindings.h"

namespace gpu {
namespace gles2 {

class ErrorStateImpl : public ErrorState {
 public:
  explicit ErrorStateImpl(ErrorStateClient* client, Logger* logger);
  virtual ~ErrorStateImpl();

  virtual uint32 GetGLError() OVERRIDE;

  virtual void SetGLError(
      const char* filename,
      int line,
      unsigned int error,
      const char* function_name,
      const char* msg) OVERRIDE;
  virtual void SetGLErrorInvalidEnum(
      const char* filename,
      int line,
      const char* function_name,
      unsigned int value,
      const char* label) OVERRIDE;
  virtual void SetGLErrorInvalidParami(
      const char* filename,
      int line,
      unsigned int error,
      const char* function_name,
      unsigned int pname,
      int param) OVERRIDE;
  virtual void SetGLErrorInvalidParamf(
      const char* filename,
      int line,
      unsigned int error,
      const char* function_name,
      unsigned int pname,
      float param) OVERRIDE;

  virtual unsigned int PeekGLError(
      const char* filename, int line, const char* function_name) OVERRIDE;

  virtual void CopyRealGLErrorsToWrapper(
      const char* filename, int line, const char* function_name) OVERRIDE;

  virtual void ClearRealGLErrors(
      const char* filename, int line, const char* function_name) OVERRIDE;

 private:
  // The last error message set.
  std::string last_error_;
  // Current GL error bits.
  uint32 error_bits_;

  ErrorStateClient* client_;
  Logger* logger_;

  DISALLOW_COPY_AND_ASSIGN(ErrorStateImpl);
};

ErrorState::ErrorState() {}

ErrorState::~ErrorState() {}

ErrorState* ErrorState::Create(ErrorStateClient* client, Logger* logger) {
  return new ErrorStateImpl(client, logger);
}

ErrorStateImpl::ErrorStateImpl(ErrorStateClient* client, Logger* logger)
    : error_bits_(0), client_(client), logger_(logger) {}

ErrorStateImpl::~ErrorStateImpl() {}

uint32 ErrorStateImpl::GetGLError() {
  // Check the GL error first, then our wrapped error.
  GLenum error = glGetError();
  if (error == GL_NO_ERROR && error_bits_ != 0) {
    for (uint32 mask = 1; mask != 0; mask = mask << 1) {
      if ((error_bits_ & mask) != 0) {
        error = GLES2Util::GLErrorBitToGLError(mask);
        break;
      }
    }
  }

  if (error != GL_NO_ERROR) {
    // There was an error, clear the corresponding wrapped error.
    error_bits_ &= ~GLES2Util::GLErrorToErrorBit(error);
  }
  return error;
}

unsigned int ErrorStateImpl::PeekGLError(
    const char* filename, int line, const char* function_name) {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    SetGLError(filename, line, error, function_name, "");
  }
  return error;
}

void ErrorStateImpl::SetGLError(
    const char* filename,
    int line,
    unsigned int error,
    const char* function_name,
    const char* msg) {
  if (msg) {
    last_error_ = msg;
    logger_->LogMessage(
        filename, line,
        std::string("GL ERROR :") +
        GLES2Util::GetStringEnum(error) + " : " +
        function_name + ": " + msg);
  }
  error_bits_ |= GLES2Util::GLErrorToErrorBit(error);
  if (error == GL_OUT_OF_MEMORY)
    client_->OnOutOfMemoryError();
}

void ErrorStateImpl::SetGLErrorInvalidEnum(
    const char* filename,
    int line,
    const char* function_name,
    unsigned int value,
    const char* label) {
  SetGLError(filename, line, GL_INVALID_ENUM, function_name,
             (std::string(label) + " was " +
             GLES2Util::GetStringEnum(value)).c_str());
}

void ErrorStateImpl::SetGLErrorInvalidParami(
    const char* filename,
    int line,
    unsigned int error,
    const char* function_name,
    unsigned int pname, int param) {
  if (error == GL_INVALID_ENUM) {
    SetGLError(
        filename, line, GL_INVALID_ENUM, function_name,
        (std::string("trying to set ") +
         GLES2Util::GetStringEnum(pname) + " to " +
         GLES2Util::GetStringEnum(param)).c_str());
  } else {
    SetGLError(
        filename, line, error, function_name,
        (std::string("trying to set ") +
         GLES2Util::GetStringEnum(pname) + " to " +
         base::StringPrintf("%d", param)).c_str());
  }
}

void ErrorStateImpl::SetGLErrorInvalidParamf(
    const char* filename,
    int line,
    unsigned int error,
    const char* function_name,
    unsigned int pname, float param) {
  SetGLError(
      filename, line, error, function_name,
      (std::string("trying to set ") +
       GLES2Util::GetStringEnum(pname) + " to " +
       base::StringPrintf("%G", param)).c_str());
}

void ErrorStateImpl::CopyRealGLErrorsToWrapper(
    const char* filename, int line, const char* function_name) {
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    SetGLError(filename, line, error, function_name,
               "<- error from previous GL command");
  }
}

void ErrorStateImpl::ClearRealGLErrors(
    const char* filename, int line, const char* function_name) {
  // Clears and logs all current gl errors.
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    if (error != GL_OUT_OF_MEMORY) {
      // GL_OUT_OF_MEMORY can legally happen on lost device.
      logger_->LogMessage(
          filename, line,
          std::string("GL ERROR :") +
          GLES2Util::GetStringEnum(error) + " : " +
          function_name + ": was unhandled");
      NOTREACHED() << "GL error " << error << " was unhandled.";
    }
  }
}

}  // namespace gles2
}  // namespace gpu

