// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file defines the GLES2 command buffer commands.

#ifndef GPU_COMMAND_BUFFER_COMMON_GLES2_CMD_FORMAT_H_
#define GPU_COMMAND_BUFFER_COMMON_GLES2_CMD_FORMAT_H_


#include <KHR/khrplatform.h>

#include <stdint.h>
#include <string.h>

#include "base/atomicops.h"
#include "base/logging.h"
#include "base/macros.h"
#include "gpu/command_buffer/common/bitfield_helpers.h"
#include "gpu/command_buffer/common/cmd_buffer_common.h"
#include "gpu/command_buffer/common/gles2_cmd_ids.h"

// GL types are forward declared to avoid including the GL headers. The problem
// is determining which GL headers to include from code that is common to the
// client and service sides (GLES2 or one of several GL implementations).
typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef signed char GLbyte;
typedef short GLshort;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned long GLulong;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;
typedef khronos_intptr_t GLintptr;
typedef khronos_ssize_t  GLsizeiptr;

namespace gpu {
namespace gles2 {

// Command buffer is GPU_COMMAND_BUFFER_ENTRY_ALIGNMENT byte aligned.
#pragma pack(push, GPU_COMMAND_BUFFER_ENTRY_ALIGNMENT)

namespace id_namespaces {

// These are used when contexts share resources.
enum IdNamespaces {
  kBuffers,
  kFramebuffers,
  kProgramsAndShaders,
  kRenderbuffers,
  kTextures,
  kQueries,
  kVertexArrays,
  kNumIdNamespaces
};

// These numbers must not change
COMPILE_ASSERT(kBuffers == 0, kBuffers_is_not_0);
COMPILE_ASSERT(kFramebuffers == 1, kFramebuffers_is_not_1);
COMPILE_ASSERT(kProgramsAndShaders == 2, kProgramsAndShaders_is_not_2);
COMPILE_ASSERT(kRenderbuffers == 3, kRenderbuffers_is_not_3);
COMPILE_ASSERT(kTextures == 4, kTextures_is_not_4);

}  // namespace id_namespaces

// Used for some glGetXXX commands that return a result through a pointer. We
// need to know if the command succeeded or not and the size of the result. If
// the command failed its result size will 0.
template <typename T>
struct SizedResult {
  typedef T Type;

  T* GetData() {
    return static_cast<T*>(static_cast<void*>(&data));
  }

  // Returns the total size in bytes of the SizedResult for a given number of
  // results including the size field.
  static size_t ComputeSize(size_t num_results) {
    return sizeof(T) * num_results + sizeof(uint32_t);  // NOLINT
  }

  // Returns the total size in bytes of the SizedResult for a given size of
  // results.
  static size_t ComputeSizeFromBytes(size_t size_of_result_in_bytes) {
    return size_of_result_in_bytes + sizeof(uint32_t);  // NOLINT
  }

  // Returns the maximum number of results for a given buffer size.
  static uint32_t ComputeMaxResults(size_t size_of_buffer) {
    return (size_of_buffer >= sizeof(uint32_t)) ?
        ((size_of_buffer - sizeof(uint32_t)) / sizeof(T)) : 0;  // NOLINT
  }

  // Set the size for a given number of results.
  void SetNumResults(size_t num_results) {
    size = sizeof(T) * num_results;  // NOLINT
  }

  // Get the number of elements in the result
  int32_t GetNumResults() const {
    return size / sizeof(T);  // NOLINT
  }

  // Copy the result.
  void CopyResult(void* dst) const {
    memcpy(dst, &data, size);
  }

  uint32_t size;  // in bytes.
  int32_t data;  // this is just here to get an offset.
};

COMPILE_ASSERT(sizeof(SizedResult<int8_t>) == 8, SizedResult_size_not_8);
COMPILE_ASSERT(offsetof(SizedResult<int8_t>, size) == 0,
               OffsetOf_SizedResult_size_not_0);
COMPILE_ASSERT(offsetof(SizedResult<int8_t>, data) == 4,
               OffsetOf_SizedResult_data_not_4);

// The data for one attrib or uniform from GetProgramInfoCHROMIUM.
struct ProgramInput {
  uint32_t type;             // The type (GL_VEC3, GL_MAT3, GL_SAMPLER_2D, etc.
  int32_t size;              // The size (how big the array is for uniforms)
  uint32_t location_offset;  // offset from ProgramInfoHeader to 'size'
                             // locations for uniforms, 1 for attribs.
  uint32_t name_offset;      // offset from ProgrmaInfoHeader to start of name.
  uint32_t name_length;      // length of the name.
};

// The format of the bucket filled out by GetProgramInfoCHROMIUM
struct ProgramInfoHeader {
  uint32_t link_status;
  uint32_t num_attribs;
  uint32_t num_uniforms;
  // ProgramInput inputs[num_attribs + num_uniforms];
};

// The format of QuerySync used by EXT_occlusion_query_boolean
struct QuerySync {
  void Reset() {
    process_count = 0;
    result = 0;
  }

  base::subtle::Atomic32 process_count;
  uint64_t result;
};

struct AsyncUploadSync {
  void Reset() {
    base::subtle::Release_Store(&async_upload_token, 0);
  }

  void SetAsyncUploadToken(uint32_t token) {
    DCHECK_NE(token, 0u);
    base::subtle::Release_Store(&async_upload_token, token);
  }

  bool HasAsyncUploadTokenPassed(uint32_t token) {
    DCHECK_NE(token, 0u);
    uint32_t current_token = base::subtle::Acquire_Load(&async_upload_token);
    return (current_token - token < 0x80000000);
  }

  base::subtle::Atomic32 async_upload_token;
};

COMPILE_ASSERT(sizeof(ProgramInput) == 20, ProgramInput_size_not_20);
COMPILE_ASSERT(offsetof(ProgramInput, type) == 0,
               OffsetOf_ProgramInput_type_not_0);
COMPILE_ASSERT(offsetof(ProgramInput, size) == 4,
               OffsetOf_ProgramInput_size_not_4);
COMPILE_ASSERT(offsetof(ProgramInput, location_offset) == 8,
               OffsetOf_ProgramInput_location_offset_not_8);
COMPILE_ASSERT(offsetof(ProgramInput, name_offset) == 12,
               OffsetOf_ProgramInput_name_offset_not_12);
COMPILE_ASSERT(offsetof(ProgramInput, name_length) == 16,
               OffsetOf_ProgramInput_name_length_not_16);

COMPILE_ASSERT(sizeof(ProgramInfoHeader) == 12, ProgramInfoHeader_size_not_12);
COMPILE_ASSERT(offsetof(ProgramInfoHeader, link_status) == 0,
               OffsetOf_ProgramInfoHeader_link_status_not_0);
COMPILE_ASSERT(offsetof(ProgramInfoHeader, num_attribs) == 4,
               OffsetOf_ProgramInfoHeader_num_attribs_not_4);
COMPILE_ASSERT(offsetof(ProgramInfoHeader, num_uniforms) == 8,
               OffsetOf_ProgramInfoHeader_num_uniforms_not_8);

namespace cmds {

#include "../common/gles2_cmd_format_autogen.h"

// These are hand written commands.
// TODO(gman): Attempt to make these auto-generated.
struct GetAttribLocation {
  typedef GetAttribLocation ValueType;
  static const CommandId kCmdId = kGetAttribLocation;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef GLint Result;

  static uint32 ComputeSize() {
    return static_cast<uint32>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() {
    header.SetCmd<ValueType>();
  }

  void Init(
      GLuint _program, uint32 _name_bucket_id,
      uint32 _location_shm_id, uint32 _location_shm_offset) {
    SetHeader();
    program = _program;
    name_bucket_id = _name_bucket_id;
    location_shm_id = _location_shm_id;
    location_shm_offset = _location_shm_offset;
  }

  void* Set(
      void* cmd, GLuint _program, uint32 _name_bucket_id,
      uint32 _location_shm_id, uint32 _location_shm_offset) {
    static_cast<ValueType*>(
        cmd)->Init(
            _program, _name_bucket_id, _location_shm_id,
            _location_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  CommandHeader header;
  uint32 program;
  uint32 name_bucket_id;
  uint32 location_shm_id;
  uint32 location_shm_offset;
};

COMPILE_ASSERT(sizeof(GetAttribLocation) == 20,
               Sizeof_GetAttribLocation_is_not_24);
COMPILE_ASSERT(offsetof(GetAttribLocation, header) == 0,
               OffsetOf_GetAttribLocation_header_not_0);
COMPILE_ASSERT(offsetof(GetAttribLocation, program) == 4,
               OffsetOf_GetAttribLocation_program_not_4);
COMPILE_ASSERT(offsetof(GetAttribLocation, name_bucket_id) == 8,
               OffsetOf_GetAttribLocation_name_bucket_id_not_8);
COMPILE_ASSERT(offsetof(GetAttribLocation, location_shm_id) == 12,
               OffsetOf_GetAttribLocation_location_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetAttribLocation, location_shm_offset) == 16,
               OffsetOf_GetAttribLocation_location_shm_offset_not_16);

struct GetUniformLocation {
  typedef GetUniformLocation ValueType;
  static const CommandId kCmdId = kGetUniformLocation;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef GLint Result;

  static uint32 ComputeSize() {
    return static_cast<uint32>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() {
    header.SetCmd<ValueType>();
  }

  void Init(
      GLuint _program, uint32 _name_bucket_id,
      uint32 _location_shm_id, uint32 _location_shm_offset) {
    SetHeader();
    program = _program;
    name_bucket_id = _name_bucket_id;
    location_shm_id = _location_shm_id;
    location_shm_offset = _location_shm_offset;
  }

  void* Set(
      void* cmd, GLuint _program, uint32 _name_bucket_id,
      uint32 _location_shm_id, uint32 _location_shm_offset) {
    static_cast<ValueType*>(
        cmd)->Init(
            _program, _name_bucket_id, _location_shm_id,
            _location_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  CommandHeader header;
  uint32 program;
  uint32 name_bucket_id;
  uint32 location_shm_id;
  uint32 location_shm_offset;
};

COMPILE_ASSERT(sizeof(GetUniformLocation) == 20,
               Sizeof_GetUniformLocation_is_not_24);
COMPILE_ASSERT(offsetof(GetUniformLocation, header) == 0,
               OffsetOf_GetUniformLocation_header_not_0);
COMPILE_ASSERT(offsetof(GetUniformLocation, program) == 4,
               OffsetOf_GetUniformLocation_program_not_4);
COMPILE_ASSERT(offsetof(GetUniformLocation, name_bucket_id) == 8,
               OffsetOf_GetUniformLocation_name_bucket_id_not_8);
COMPILE_ASSERT(offsetof(GetUniformLocation, location_shm_id) == 12,
               OffsetOf_GetUniformLocation_location_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetUniformLocation, location_shm_offset) == 16,
               OffsetOf_GetUniformLocation_location_shm_offset_not_16);

struct GenMailboxCHROMIUM {
  typedef GenMailboxCHROMIUM ValueType;
  static const CommandId kCmdId = kGenMailboxCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);
  CommandHeader header;
};

struct InsertSyncPointCHROMIUM {
  typedef InsertSyncPointCHROMIUM ValueType;
  static const CommandId kCmdId = kInsertSyncPointCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);
  CommandHeader header;
};

struct CreateAndConsumeTextureCHROMIUMImmediate {
  typedef CreateAndConsumeTextureCHROMIUMImmediate ValueType;
  static const CommandId kCmdId = kCreateAndConsumeTextureCHROMIUMImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeDataSize() {
    return static_cast<uint32_t>(sizeof(GLbyte) * 64);  // NOLINT
  }

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize());  // NOLINT
  }

  void SetHeader(uint32_t size_in_bytes) {
    header.SetCmdByTotalSize<ValueType>(size_in_bytes);
  }

  void Init(GLenum _target, uint32_t _client_id, const GLbyte* _mailbox) {
    SetHeader(ComputeSize());
    target = _target;
    client_id = _client_id;
    memcpy(ImmediateDataAddress(this), _mailbox, ComputeDataSize());
  }

  void* Set(void* cmd,
            GLenum _target,
            uint32_t _client_id,
            const GLbyte* _mailbox) {
    static_cast<ValueType*>(cmd)->Init(_target, _client_id, _mailbox);
    const uint32_t size = ComputeSize();
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t client_id;
};

COMPILE_ASSERT(sizeof(CreateAndConsumeTextureCHROMIUMImmediate) == 12,
               Sizeof_CreateAndConsumeTextureCHROMIUMImmediate_is_not_12);
COMPILE_ASSERT(offsetof(CreateAndConsumeTextureCHROMIUMImmediate, header) == 0,
               OffsetOf_CreateAndConsumeTextureCHROMIUMImmediate_header_not_0);
COMPILE_ASSERT(offsetof(CreateAndConsumeTextureCHROMIUMImmediate, target) == 4,
               OffsetOf_CreateAndConsumeTextureCHROMIUMImmediate_target_not_4);
COMPILE_ASSERT(
    offsetof(CreateAndConsumeTextureCHROMIUMImmediate, client_id) == 8,
    OffsetOf_CreateAndConsumeTextureCHROMIUMImmediate_client_id_not_8);


#pragma pack(pop)

}  // namespace cmd
}  // namespace gles2
}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_COMMON_GLES2_CMD_FORMAT_H_
