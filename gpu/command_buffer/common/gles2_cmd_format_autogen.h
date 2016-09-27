// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

#ifndef GPU_COMMAND_BUFFER_COMMON_GLES2_CMD_FORMAT_AUTOGEN_H_
#define GPU_COMMAND_BUFFER_COMMON_GLES2_CMD_FORMAT_AUTOGEN_H_

struct ActiveTexture {
  typedef ActiveTexture ValueType;
  static const CommandId kCmdId = kActiveTexture;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _texture) {
    SetHeader();
    texture = _texture;
  }

  void* Set(void* cmd, GLenum _texture) {
    static_cast<ValueType*>(cmd)->Init(_texture);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t texture;
};

COMPILE_ASSERT(sizeof(ActiveTexture) == 8, Sizeof_ActiveTexture_is_not_8);
COMPILE_ASSERT(offsetof(ActiveTexture, header) == 0,
               OffsetOf_ActiveTexture_header_not_0);
COMPILE_ASSERT(offsetof(ActiveTexture, texture) == 4,
               OffsetOf_ActiveTexture_texture_not_4);

struct AttachShader {
  typedef AttachShader ValueType;
  static const CommandId kCmdId = kAttachShader;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program, GLuint _shader) {
    SetHeader();
    program = _program;
    shader = _shader;
  }

  void* Set(void* cmd, GLuint _program, GLuint _shader) {
    static_cast<ValueType*>(cmd)->Init(_program, _shader);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  uint32_t shader;
};

COMPILE_ASSERT(sizeof(AttachShader) == 12, Sizeof_AttachShader_is_not_12);
COMPILE_ASSERT(offsetof(AttachShader, header) == 0,
               OffsetOf_AttachShader_header_not_0);
COMPILE_ASSERT(offsetof(AttachShader, program) == 4,
               OffsetOf_AttachShader_program_not_4);
COMPILE_ASSERT(offsetof(AttachShader, shader) == 8,
               OffsetOf_AttachShader_shader_not_8);

struct BindAttribLocationBucket {
  typedef BindAttribLocationBucket ValueType;
  static const CommandId kCmdId = kBindAttribLocationBucket;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program, GLuint _index, uint32_t _name_bucket_id) {
    SetHeader();
    program = _program;
    index = _index;
    name_bucket_id = _name_bucket_id;
  }

  void* Set(void* cmd,
            GLuint _program,
            GLuint _index,
            uint32_t _name_bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_program, _index, _name_bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  uint32_t index;
  uint32_t name_bucket_id;
};

COMPILE_ASSERT(sizeof(BindAttribLocationBucket) == 16,
               Sizeof_BindAttribLocationBucket_is_not_16);
COMPILE_ASSERT(offsetof(BindAttribLocationBucket, header) == 0,
               OffsetOf_BindAttribLocationBucket_header_not_0);
COMPILE_ASSERT(offsetof(BindAttribLocationBucket, program) == 4,
               OffsetOf_BindAttribLocationBucket_program_not_4);
COMPILE_ASSERT(offsetof(BindAttribLocationBucket, index) == 8,
               OffsetOf_BindAttribLocationBucket_index_not_8);
COMPILE_ASSERT(offsetof(BindAttribLocationBucket, name_bucket_id) == 12,
               OffsetOf_BindAttribLocationBucket_name_bucket_id_not_12);

struct BindBuffer {
  typedef BindBuffer ValueType;
  static const CommandId kCmdId = kBindBuffer;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target, GLuint _buffer) {
    SetHeader();
    target = _target;
    buffer = _buffer;
  }

  void* Set(void* cmd, GLenum _target, GLuint _buffer) {
    static_cast<ValueType*>(cmd)->Init(_target, _buffer);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t buffer;
};

COMPILE_ASSERT(sizeof(BindBuffer) == 12, Sizeof_BindBuffer_is_not_12);
COMPILE_ASSERT(offsetof(BindBuffer, header) == 0,
               OffsetOf_BindBuffer_header_not_0);
COMPILE_ASSERT(offsetof(BindBuffer, target) == 4,
               OffsetOf_BindBuffer_target_not_4);
COMPILE_ASSERT(offsetof(BindBuffer, buffer) == 8,
               OffsetOf_BindBuffer_buffer_not_8);

struct BindFramebuffer {
  typedef BindFramebuffer ValueType;
  static const CommandId kCmdId = kBindFramebuffer;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target, GLuint _framebuffer) {
    SetHeader();
    target = _target;
    framebuffer = _framebuffer;
  }

  void* Set(void* cmd, GLenum _target, GLuint _framebuffer) {
    static_cast<ValueType*>(cmd)->Init(_target, _framebuffer);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t framebuffer;
};

COMPILE_ASSERT(sizeof(BindFramebuffer) == 12, Sizeof_BindFramebuffer_is_not_12);
COMPILE_ASSERT(offsetof(BindFramebuffer, header) == 0,
               OffsetOf_BindFramebuffer_header_not_0);
COMPILE_ASSERT(offsetof(BindFramebuffer, target) == 4,
               OffsetOf_BindFramebuffer_target_not_4);
COMPILE_ASSERT(offsetof(BindFramebuffer, framebuffer) == 8,
               OffsetOf_BindFramebuffer_framebuffer_not_8);

struct BindRenderbuffer {
  typedef BindRenderbuffer ValueType;
  static const CommandId kCmdId = kBindRenderbuffer;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target, GLuint _renderbuffer) {
    SetHeader();
    target = _target;
    renderbuffer = _renderbuffer;
  }

  void* Set(void* cmd, GLenum _target, GLuint _renderbuffer) {
    static_cast<ValueType*>(cmd)->Init(_target, _renderbuffer);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t renderbuffer;
};

COMPILE_ASSERT(sizeof(BindRenderbuffer) == 12,
               Sizeof_BindRenderbuffer_is_not_12);
COMPILE_ASSERT(offsetof(BindRenderbuffer, header) == 0,
               OffsetOf_BindRenderbuffer_header_not_0);
COMPILE_ASSERT(offsetof(BindRenderbuffer, target) == 4,
               OffsetOf_BindRenderbuffer_target_not_4);
COMPILE_ASSERT(offsetof(BindRenderbuffer, renderbuffer) == 8,
               OffsetOf_BindRenderbuffer_renderbuffer_not_8);

struct BindTexture {
  typedef BindTexture ValueType;
  static const CommandId kCmdId = kBindTexture;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target, GLuint _texture) {
    SetHeader();
    target = _target;
    texture = _texture;
  }

  void* Set(void* cmd, GLenum _target, GLuint _texture) {
    static_cast<ValueType*>(cmd)->Init(_target, _texture);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t texture;
};

COMPILE_ASSERT(sizeof(BindTexture) == 12, Sizeof_BindTexture_is_not_12);
COMPILE_ASSERT(offsetof(BindTexture, header) == 0,
               OffsetOf_BindTexture_header_not_0);
COMPILE_ASSERT(offsetof(BindTexture, target) == 4,
               OffsetOf_BindTexture_target_not_4);
COMPILE_ASSERT(offsetof(BindTexture, texture) == 8,
               OffsetOf_BindTexture_texture_not_8);

struct BlendColor {
  typedef BlendColor ValueType;
  static const CommandId kCmdId = kBlendColor;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLclampf _red, GLclampf _green, GLclampf _blue, GLclampf _alpha) {
    SetHeader();
    red = _red;
    green = _green;
    blue = _blue;
    alpha = _alpha;
  }

  void* Set(void* cmd,
            GLclampf _red,
            GLclampf _green,
            GLclampf _blue,
            GLclampf _alpha) {
    static_cast<ValueType*>(cmd)->Init(_red, _green, _blue, _alpha);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  float red;
  float green;
  float blue;
  float alpha;
};

COMPILE_ASSERT(sizeof(BlendColor) == 20, Sizeof_BlendColor_is_not_20);
COMPILE_ASSERT(offsetof(BlendColor, header) == 0,
               OffsetOf_BlendColor_header_not_0);
COMPILE_ASSERT(offsetof(BlendColor, red) == 4, OffsetOf_BlendColor_red_not_4);
COMPILE_ASSERT(offsetof(BlendColor, green) == 8,
               OffsetOf_BlendColor_green_not_8);
COMPILE_ASSERT(offsetof(BlendColor, blue) == 12,
               OffsetOf_BlendColor_blue_not_12);
COMPILE_ASSERT(offsetof(BlendColor, alpha) == 16,
               OffsetOf_BlendColor_alpha_not_16);

struct BlendEquation {
  typedef BlendEquation ValueType;
  static const CommandId kCmdId = kBlendEquation;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _mode) {
    SetHeader();
    mode = _mode;
  }

  void* Set(void* cmd, GLenum _mode) {
    static_cast<ValueType*>(cmd)->Init(_mode);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t mode;
};

COMPILE_ASSERT(sizeof(BlendEquation) == 8, Sizeof_BlendEquation_is_not_8);
COMPILE_ASSERT(offsetof(BlendEquation, header) == 0,
               OffsetOf_BlendEquation_header_not_0);
COMPILE_ASSERT(offsetof(BlendEquation, mode) == 4,
               OffsetOf_BlendEquation_mode_not_4);

struct BlendEquationSeparate {
  typedef BlendEquationSeparate ValueType;
  static const CommandId kCmdId = kBlendEquationSeparate;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _modeRGB, GLenum _modeAlpha) {
    SetHeader();
    modeRGB = _modeRGB;
    modeAlpha = _modeAlpha;
  }

  void* Set(void* cmd, GLenum _modeRGB, GLenum _modeAlpha) {
    static_cast<ValueType*>(cmd)->Init(_modeRGB, _modeAlpha);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t modeRGB;
  uint32_t modeAlpha;
};

COMPILE_ASSERT(sizeof(BlendEquationSeparate) == 12,
               Sizeof_BlendEquationSeparate_is_not_12);
COMPILE_ASSERT(offsetof(BlendEquationSeparate, header) == 0,
               OffsetOf_BlendEquationSeparate_header_not_0);
COMPILE_ASSERT(offsetof(BlendEquationSeparate, modeRGB) == 4,
               OffsetOf_BlendEquationSeparate_modeRGB_not_4);
COMPILE_ASSERT(offsetof(BlendEquationSeparate, modeAlpha) == 8,
               OffsetOf_BlendEquationSeparate_modeAlpha_not_8);

struct BlendFunc {
  typedef BlendFunc ValueType;
  static const CommandId kCmdId = kBlendFunc;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _sfactor, GLenum _dfactor) {
    SetHeader();
    sfactor = _sfactor;
    dfactor = _dfactor;
  }

  void* Set(void* cmd, GLenum _sfactor, GLenum _dfactor) {
    static_cast<ValueType*>(cmd)->Init(_sfactor, _dfactor);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t sfactor;
  uint32_t dfactor;
};

COMPILE_ASSERT(sizeof(BlendFunc) == 12, Sizeof_BlendFunc_is_not_12);
COMPILE_ASSERT(offsetof(BlendFunc, header) == 0,
               OffsetOf_BlendFunc_header_not_0);
COMPILE_ASSERT(offsetof(BlendFunc, sfactor) == 4,
               OffsetOf_BlendFunc_sfactor_not_4);
COMPILE_ASSERT(offsetof(BlendFunc, dfactor) == 8,
               OffsetOf_BlendFunc_dfactor_not_8);

struct BlendFuncSeparate {
  typedef BlendFuncSeparate ValueType;
  static const CommandId kCmdId = kBlendFuncSeparate;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _srcRGB,
            GLenum _dstRGB,
            GLenum _srcAlpha,
            GLenum _dstAlpha) {
    SetHeader();
    srcRGB = _srcRGB;
    dstRGB = _dstRGB;
    srcAlpha = _srcAlpha;
    dstAlpha = _dstAlpha;
  }

  void* Set(void* cmd,
            GLenum _srcRGB,
            GLenum _dstRGB,
            GLenum _srcAlpha,
            GLenum _dstAlpha) {
    static_cast<ValueType*>(cmd)->Init(_srcRGB, _dstRGB, _srcAlpha, _dstAlpha);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t srcRGB;
  uint32_t dstRGB;
  uint32_t srcAlpha;
  uint32_t dstAlpha;
};

COMPILE_ASSERT(sizeof(BlendFuncSeparate) == 20,
               Sizeof_BlendFuncSeparate_is_not_20);
COMPILE_ASSERT(offsetof(BlendFuncSeparate, header) == 0,
               OffsetOf_BlendFuncSeparate_header_not_0);
COMPILE_ASSERT(offsetof(BlendFuncSeparate, srcRGB) == 4,
               OffsetOf_BlendFuncSeparate_srcRGB_not_4);
COMPILE_ASSERT(offsetof(BlendFuncSeparate, dstRGB) == 8,
               OffsetOf_BlendFuncSeparate_dstRGB_not_8);
COMPILE_ASSERT(offsetof(BlendFuncSeparate, srcAlpha) == 12,
               OffsetOf_BlendFuncSeparate_srcAlpha_not_12);
COMPILE_ASSERT(offsetof(BlendFuncSeparate, dstAlpha) == 16,
               OffsetOf_BlendFuncSeparate_dstAlpha_not_16);

struct BufferData {
  typedef BufferData ValueType;
  static const CommandId kCmdId = kBufferData;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLsizeiptr _size,
            uint32_t _data_shm_id,
            uint32_t _data_shm_offset,
            GLenum _usage) {
    SetHeader();
    target = _target;
    size = _size;
    data_shm_id = _data_shm_id;
    data_shm_offset = _data_shm_offset;
    usage = _usage;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLsizeiptr _size,
            uint32_t _data_shm_id,
            uint32_t _data_shm_offset,
            GLenum _usage) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _size, _data_shm_id, _data_shm_offset, _usage);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t size;
  uint32_t data_shm_id;
  uint32_t data_shm_offset;
  uint32_t usage;
};

COMPILE_ASSERT(sizeof(BufferData) == 24, Sizeof_BufferData_is_not_24);
COMPILE_ASSERT(offsetof(BufferData, header) == 0,
               OffsetOf_BufferData_header_not_0);
COMPILE_ASSERT(offsetof(BufferData, target) == 4,
               OffsetOf_BufferData_target_not_4);
COMPILE_ASSERT(offsetof(BufferData, size) == 8, OffsetOf_BufferData_size_not_8);
COMPILE_ASSERT(offsetof(BufferData, data_shm_id) == 12,
               OffsetOf_BufferData_data_shm_id_not_12);
COMPILE_ASSERT(offsetof(BufferData, data_shm_offset) == 16,
               OffsetOf_BufferData_data_shm_offset_not_16);
COMPILE_ASSERT(offsetof(BufferData, usage) == 20,
               OffsetOf_BufferData_usage_not_20);

struct BufferSubData {
  typedef BufferSubData ValueType;
  static const CommandId kCmdId = kBufferSubData;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLintptr _offset,
            GLsizeiptr _size,
            uint32_t _data_shm_id,
            uint32_t _data_shm_offset) {
    SetHeader();
    target = _target;
    offset = _offset;
    size = _size;
    data_shm_id = _data_shm_id;
    data_shm_offset = _data_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLintptr _offset,
            GLsizeiptr _size,
            uint32_t _data_shm_id,
            uint32_t _data_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _offset, _size, _data_shm_id, _data_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t offset;
  int32_t size;
  uint32_t data_shm_id;
  uint32_t data_shm_offset;
};

COMPILE_ASSERT(sizeof(BufferSubData) == 24, Sizeof_BufferSubData_is_not_24);
COMPILE_ASSERT(offsetof(BufferSubData, header) == 0,
               OffsetOf_BufferSubData_header_not_0);
COMPILE_ASSERT(offsetof(BufferSubData, target) == 4,
               OffsetOf_BufferSubData_target_not_4);
COMPILE_ASSERT(offsetof(BufferSubData, offset) == 8,
               OffsetOf_BufferSubData_offset_not_8);
COMPILE_ASSERT(offsetof(BufferSubData, size) == 12,
               OffsetOf_BufferSubData_size_not_12);
COMPILE_ASSERT(offsetof(BufferSubData, data_shm_id) == 16,
               OffsetOf_BufferSubData_data_shm_id_not_16);
COMPILE_ASSERT(offsetof(BufferSubData, data_shm_offset) == 20,
               OffsetOf_BufferSubData_data_shm_offset_not_20);

struct CheckFramebufferStatus {
  typedef CheckFramebufferStatus ValueType;
  static const CommandId kCmdId = kCheckFramebufferStatus;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef GLenum Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    target = _target;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(CheckFramebufferStatus) == 16,
               Sizeof_CheckFramebufferStatus_is_not_16);
COMPILE_ASSERT(offsetof(CheckFramebufferStatus, header) == 0,
               OffsetOf_CheckFramebufferStatus_header_not_0);
COMPILE_ASSERT(offsetof(CheckFramebufferStatus, target) == 4,
               OffsetOf_CheckFramebufferStatus_target_not_4);
COMPILE_ASSERT(offsetof(CheckFramebufferStatus, result_shm_id) == 8,
               OffsetOf_CheckFramebufferStatus_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(CheckFramebufferStatus, result_shm_offset) == 12,
               OffsetOf_CheckFramebufferStatus_result_shm_offset_not_12);

struct Clear {
  typedef Clear ValueType;
  static const CommandId kCmdId = kClear;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLbitfield _mask) {
    SetHeader();
    mask = _mask;
  }

  void* Set(void* cmd, GLbitfield _mask) {
    static_cast<ValueType*>(cmd)->Init(_mask);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t mask;
};

COMPILE_ASSERT(sizeof(Clear) == 8, Sizeof_Clear_is_not_8);
COMPILE_ASSERT(offsetof(Clear, header) == 0, OffsetOf_Clear_header_not_0);
COMPILE_ASSERT(offsetof(Clear, mask) == 4, OffsetOf_Clear_mask_not_4);

struct ClearColor {
  typedef ClearColor ValueType;
  static const CommandId kCmdId = kClearColor;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLclampf _red, GLclampf _green, GLclampf _blue, GLclampf _alpha) {
    SetHeader();
    red = _red;
    green = _green;
    blue = _blue;
    alpha = _alpha;
  }

  void* Set(void* cmd,
            GLclampf _red,
            GLclampf _green,
            GLclampf _blue,
            GLclampf _alpha) {
    static_cast<ValueType*>(cmd)->Init(_red, _green, _blue, _alpha);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  float red;
  float green;
  float blue;
  float alpha;
};

COMPILE_ASSERT(sizeof(ClearColor) == 20, Sizeof_ClearColor_is_not_20);
COMPILE_ASSERT(offsetof(ClearColor, header) == 0,
               OffsetOf_ClearColor_header_not_0);
COMPILE_ASSERT(offsetof(ClearColor, red) == 4, OffsetOf_ClearColor_red_not_4);
COMPILE_ASSERT(offsetof(ClearColor, green) == 8,
               OffsetOf_ClearColor_green_not_8);
COMPILE_ASSERT(offsetof(ClearColor, blue) == 12,
               OffsetOf_ClearColor_blue_not_12);
COMPILE_ASSERT(offsetof(ClearColor, alpha) == 16,
               OffsetOf_ClearColor_alpha_not_16);

struct ClearDepthf {
  typedef ClearDepthf ValueType;
  static const CommandId kCmdId = kClearDepthf;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLclampf _depth) {
    SetHeader();
    depth = _depth;
  }

  void* Set(void* cmd, GLclampf _depth) {
    static_cast<ValueType*>(cmd)->Init(_depth);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  float depth;
};

COMPILE_ASSERT(sizeof(ClearDepthf) == 8, Sizeof_ClearDepthf_is_not_8);
COMPILE_ASSERT(offsetof(ClearDepthf, header) == 0,
               OffsetOf_ClearDepthf_header_not_0);
COMPILE_ASSERT(offsetof(ClearDepthf, depth) == 4,
               OffsetOf_ClearDepthf_depth_not_4);

struct ClearStencil {
  typedef ClearStencil ValueType;
  static const CommandId kCmdId = kClearStencil;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _s) {
    SetHeader();
    s = _s;
  }

  void* Set(void* cmd, GLint _s) {
    static_cast<ValueType*>(cmd)->Init(_s);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t s;
};

COMPILE_ASSERT(sizeof(ClearStencil) == 8, Sizeof_ClearStencil_is_not_8);
COMPILE_ASSERT(offsetof(ClearStencil, header) == 0,
               OffsetOf_ClearStencil_header_not_0);
COMPILE_ASSERT(offsetof(ClearStencil, s) == 4, OffsetOf_ClearStencil_s_not_4);

struct ColorMask {
  typedef ColorMask ValueType;
  static const CommandId kCmdId = kColorMask;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLboolean _red,
            GLboolean _green,
            GLboolean _blue,
            GLboolean _alpha) {
    SetHeader();
    red = _red;
    green = _green;
    blue = _blue;
    alpha = _alpha;
  }

  void* Set(void* cmd,
            GLboolean _red,
            GLboolean _green,
            GLboolean _blue,
            GLboolean _alpha) {
    static_cast<ValueType*>(cmd)->Init(_red, _green, _blue, _alpha);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t red;
  uint32_t green;
  uint32_t blue;
  uint32_t alpha;
};

COMPILE_ASSERT(sizeof(ColorMask) == 20, Sizeof_ColorMask_is_not_20);
COMPILE_ASSERT(offsetof(ColorMask, header) == 0,
               OffsetOf_ColorMask_header_not_0);
COMPILE_ASSERT(offsetof(ColorMask, red) == 4, OffsetOf_ColorMask_red_not_4);
COMPILE_ASSERT(offsetof(ColorMask, green) == 8, OffsetOf_ColorMask_green_not_8);
COMPILE_ASSERT(offsetof(ColorMask, blue) == 12, OffsetOf_ColorMask_blue_not_12);
COMPILE_ASSERT(offsetof(ColorMask, alpha) == 16,
               OffsetOf_ColorMask_alpha_not_16);

struct CompileShader {
  typedef CompileShader ValueType;
  static const CommandId kCmdId = kCompileShader;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _shader) {
    SetHeader();
    shader = _shader;
  }

  void* Set(void* cmd, GLuint _shader) {
    static_cast<ValueType*>(cmd)->Init(_shader);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t shader;
};

COMPILE_ASSERT(sizeof(CompileShader) == 8, Sizeof_CompileShader_is_not_8);
COMPILE_ASSERT(offsetof(CompileShader, header) == 0,
               OffsetOf_CompileShader_header_not_0);
COMPILE_ASSERT(offsetof(CompileShader, shader) == 4,
               OffsetOf_CompileShader_shader_not_4);

struct CompressedTexImage2DBucket {
  typedef CompressedTexImage2DBucket ValueType;
  static const CommandId kCmdId = kCompressedTexImage2DBucket;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLint _level,
            GLenum _internalformat,
            GLsizei _width,
            GLsizei _height,
            GLuint _bucket_id) {
    SetHeader();
    target = _target;
    level = _level;
    internalformat = _internalformat;
    width = _width;
    height = _height;
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLint _level,
            GLenum _internalformat,
            GLsizei _width,
            GLsizei _height,
            GLuint _bucket_id) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _level, _internalformat, _width, _height, _bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t level;
  uint32_t internalformat;
  int32_t width;
  int32_t height;
  uint32_t bucket_id;
  static const int32_t border = 0;
};

COMPILE_ASSERT(sizeof(CompressedTexImage2DBucket) == 28,
               Sizeof_CompressedTexImage2DBucket_is_not_28);
COMPILE_ASSERT(offsetof(CompressedTexImage2DBucket, header) == 0,
               OffsetOf_CompressedTexImage2DBucket_header_not_0);
COMPILE_ASSERT(offsetof(CompressedTexImage2DBucket, target) == 4,
               OffsetOf_CompressedTexImage2DBucket_target_not_4);
COMPILE_ASSERT(offsetof(CompressedTexImage2DBucket, level) == 8,
               OffsetOf_CompressedTexImage2DBucket_level_not_8);
COMPILE_ASSERT(offsetof(CompressedTexImage2DBucket, internalformat) == 12,
               OffsetOf_CompressedTexImage2DBucket_internalformat_not_12);
COMPILE_ASSERT(offsetof(CompressedTexImage2DBucket, width) == 16,
               OffsetOf_CompressedTexImage2DBucket_width_not_16);
COMPILE_ASSERT(offsetof(CompressedTexImage2DBucket, height) == 20,
               OffsetOf_CompressedTexImage2DBucket_height_not_20);
COMPILE_ASSERT(offsetof(CompressedTexImage2DBucket, bucket_id) == 24,
               OffsetOf_CompressedTexImage2DBucket_bucket_id_not_24);

struct CompressedTexImage2D {
  typedef CompressedTexImage2D ValueType;
  static const CommandId kCmdId = kCompressedTexImage2D;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLint _level,
            GLenum _internalformat,
            GLsizei _width,
            GLsizei _height,
            GLsizei _imageSize,
            uint32_t _data_shm_id,
            uint32_t _data_shm_offset) {
    SetHeader();
    target = _target;
    level = _level;
    internalformat = _internalformat;
    width = _width;
    height = _height;
    imageSize = _imageSize;
    data_shm_id = _data_shm_id;
    data_shm_offset = _data_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLint _level,
            GLenum _internalformat,
            GLsizei _width,
            GLsizei _height,
            GLsizei _imageSize,
            uint32_t _data_shm_id,
            uint32_t _data_shm_offset) {
    static_cast<ValueType*>(cmd)->Init(_target,
                                       _level,
                                       _internalformat,
                                       _width,
                                       _height,
                                       _imageSize,
                                       _data_shm_id,
                                       _data_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t level;
  uint32_t internalformat;
  int32_t width;
  int32_t height;
  int32_t imageSize;
  uint32_t data_shm_id;
  uint32_t data_shm_offset;
  static const int32_t border = 0;
};

COMPILE_ASSERT(sizeof(CompressedTexImage2D) == 36,
               Sizeof_CompressedTexImage2D_is_not_36);
COMPILE_ASSERT(offsetof(CompressedTexImage2D, header) == 0,
               OffsetOf_CompressedTexImage2D_header_not_0);
COMPILE_ASSERT(offsetof(CompressedTexImage2D, target) == 4,
               OffsetOf_CompressedTexImage2D_target_not_4);
COMPILE_ASSERT(offsetof(CompressedTexImage2D, level) == 8,
               OffsetOf_CompressedTexImage2D_level_not_8);
COMPILE_ASSERT(offsetof(CompressedTexImage2D, internalformat) == 12,
               OffsetOf_CompressedTexImage2D_internalformat_not_12);
COMPILE_ASSERT(offsetof(CompressedTexImage2D, width) == 16,
               OffsetOf_CompressedTexImage2D_width_not_16);
COMPILE_ASSERT(offsetof(CompressedTexImage2D, height) == 20,
               OffsetOf_CompressedTexImage2D_height_not_20);
COMPILE_ASSERT(offsetof(CompressedTexImage2D, imageSize) == 24,
               OffsetOf_CompressedTexImage2D_imageSize_not_24);
COMPILE_ASSERT(offsetof(CompressedTexImage2D, data_shm_id) == 28,
               OffsetOf_CompressedTexImage2D_data_shm_id_not_28);
COMPILE_ASSERT(offsetof(CompressedTexImage2D, data_shm_offset) == 32,
               OffsetOf_CompressedTexImage2D_data_shm_offset_not_32);

struct CompressedTexSubImage2DBucket {
  typedef CompressedTexSubImage2DBucket ValueType;
  static const CommandId kCmdId = kCompressedTexSubImage2DBucket;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLint _level,
            GLint _xoffset,
            GLint _yoffset,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLuint _bucket_id) {
    SetHeader();
    target = _target;
    level = _level;
    xoffset = _xoffset;
    yoffset = _yoffset;
    width = _width;
    height = _height;
    format = _format;
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLint _level,
            GLint _xoffset,
            GLint _yoffset,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLuint _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_target,
                                       _level,
                                       _xoffset,
                                       _yoffset,
                                       _width,
                                       _height,
                                       _format,
                                       _bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t level;
  int32_t xoffset;
  int32_t yoffset;
  int32_t width;
  int32_t height;
  uint32_t format;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(CompressedTexSubImage2DBucket) == 36,
               Sizeof_CompressedTexSubImage2DBucket_is_not_36);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2DBucket, header) == 0,
               OffsetOf_CompressedTexSubImage2DBucket_header_not_0);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2DBucket, target) == 4,
               OffsetOf_CompressedTexSubImage2DBucket_target_not_4);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2DBucket, level) == 8,
               OffsetOf_CompressedTexSubImage2DBucket_level_not_8);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2DBucket, xoffset) == 12,
               OffsetOf_CompressedTexSubImage2DBucket_xoffset_not_12);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2DBucket, yoffset) == 16,
               OffsetOf_CompressedTexSubImage2DBucket_yoffset_not_16);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2DBucket, width) == 20,
               OffsetOf_CompressedTexSubImage2DBucket_width_not_20);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2DBucket, height) == 24,
               OffsetOf_CompressedTexSubImage2DBucket_height_not_24);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2DBucket, format) == 28,
               OffsetOf_CompressedTexSubImage2DBucket_format_not_28);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2DBucket, bucket_id) == 32,
               OffsetOf_CompressedTexSubImage2DBucket_bucket_id_not_32);

struct CompressedTexSubImage2D {
  typedef CompressedTexSubImage2D ValueType;
  static const CommandId kCmdId = kCompressedTexSubImage2D;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLint _level,
            GLint _xoffset,
            GLint _yoffset,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLsizei _imageSize,
            uint32_t _data_shm_id,
            uint32_t _data_shm_offset) {
    SetHeader();
    target = _target;
    level = _level;
    xoffset = _xoffset;
    yoffset = _yoffset;
    width = _width;
    height = _height;
    format = _format;
    imageSize = _imageSize;
    data_shm_id = _data_shm_id;
    data_shm_offset = _data_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLint _level,
            GLint _xoffset,
            GLint _yoffset,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLsizei _imageSize,
            uint32_t _data_shm_id,
            uint32_t _data_shm_offset) {
    static_cast<ValueType*>(cmd)->Init(_target,
                                       _level,
                                       _xoffset,
                                       _yoffset,
                                       _width,
                                       _height,
                                       _format,
                                       _imageSize,
                                       _data_shm_id,
                                       _data_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t level;
  int32_t xoffset;
  int32_t yoffset;
  int32_t width;
  int32_t height;
  uint32_t format;
  int32_t imageSize;
  uint32_t data_shm_id;
  uint32_t data_shm_offset;
};

COMPILE_ASSERT(sizeof(CompressedTexSubImage2D) == 44,
               Sizeof_CompressedTexSubImage2D_is_not_44);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, header) == 0,
               OffsetOf_CompressedTexSubImage2D_header_not_0);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, target) == 4,
               OffsetOf_CompressedTexSubImage2D_target_not_4);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, level) == 8,
               OffsetOf_CompressedTexSubImage2D_level_not_8);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, xoffset) == 12,
               OffsetOf_CompressedTexSubImage2D_xoffset_not_12);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, yoffset) == 16,
               OffsetOf_CompressedTexSubImage2D_yoffset_not_16);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, width) == 20,
               OffsetOf_CompressedTexSubImage2D_width_not_20);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, height) == 24,
               OffsetOf_CompressedTexSubImage2D_height_not_24);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, format) == 28,
               OffsetOf_CompressedTexSubImage2D_format_not_28);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, imageSize) == 32,
               OffsetOf_CompressedTexSubImage2D_imageSize_not_32);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, data_shm_id) == 36,
               OffsetOf_CompressedTexSubImage2D_data_shm_id_not_36);
COMPILE_ASSERT(offsetof(CompressedTexSubImage2D, data_shm_offset) == 40,
               OffsetOf_CompressedTexSubImage2D_data_shm_offset_not_40);

struct CopyTexImage2D {
  typedef CopyTexImage2D ValueType;
  static const CommandId kCmdId = kCopyTexImage2D;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLint _level,
            GLenum _internalformat,
            GLint _x,
            GLint _y,
            GLsizei _width,
            GLsizei _height) {
    SetHeader();
    target = _target;
    level = _level;
    internalformat = _internalformat;
    x = _x;
    y = _y;
    width = _width;
    height = _height;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLint _level,
            GLenum _internalformat,
            GLint _x,
            GLint _y,
            GLsizei _width,
            GLsizei _height) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _level, _internalformat, _x, _y, _width, _height);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t level;
  uint32_t internalformat;
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
  static const int32_t border = 0;
};

COMPILE_ASSERT(sizeof(CopyTexImage2D) == 32, Sizeof_CopyTexImage2D_is_not_32);
COMPILE_ASSERT(offsetof(CopyTexImage2D, header) == 0,
               OffsetOf_CopyTexImage2D_header_not_0);
COMPILE_ASSERT(offsetof(CopyTexImage2D, target) == 4,
               OffsetOf_CopyTexImage2D_target_not_4);
COMPILE_ASSERT(offsetof(CopyTexImage2D, level) == 8,
               OffsetOf_CopyTexImage2D_level_not_8);
COMPILE_ASSERT(offsetof(CopyTexImage2D, internalformat) == 12,
               OffsetOf_CopyTexImage2D_internalformat_not_12);
COMPILE_ASSERT(offsetof(CopyTexImage2D, x) == 16,
               OffsetOf_CopyTexImage2D_x_not_16);
COMPILE_ASSERT(offsetof(CopyTexImage2D, y) == 20,
               OffsetOf_CopyTexImage2D_y_not_20);
COMPILE_ASSERT(offsetof(CopyTexImage2D, width) == 24,
               OffsetOf_CopyTexImage2D_width_not_24);
COMPILE_ASSERT(offsetof(CopyTexImage2D, height) == 28,
               OffsetOf_CopyTexImage2D_height_not_28);

struct CopyTexSubImage2D {
  typedef CopyTexSubImage2D ValueType;
  static const CommandId kCmdId = kCopyTexSubImage2D;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLint _level,
            GLint _xoffset,
            GLint _yoffset,
            GLint _x,
            GLint _y,
            GLsizei _width,
            GLsizei _height) {
    SetHeader();
    target = _target;
    level = _level;
    xoffset = _xoffset;
    yoffset = _yoffset;
    x = _x;
    y = _y;
    width = _width;
    height = _height;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLint _level,
            GLint _xoffset,
            GLint _yoffset,
            GLint _x,
            GLint _y,
            GLsizei _width,
            GLsizei _height) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _level, _xoffset, _yoffset, _x, _y, _width, _height);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t level;
  int32_t xoffset;
  int32_t yoffset;
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
};

COMPILE_ASSERT(sizeof(CopyTexSubImage2D) == 36,
               Sizeof_CopyTexSubImage2D_is_not_36);
COMPILE_ASSERT(offsetof(CopyTexSubImage2D, header) == 0,
               OffsetOf_CopyTexSubImage2D_header_not_0);
COMPILE_ASSERT(offsetof(CopyTexSubImage2D, target) == 4,
               OffsetOf_CopyTexSubImage2D_target_not_4);
COMPILE_ASSERT(offsetof(CopyTexSubImage2D, level) == 8,
               OffsetOf_CopyTexSubImage2D_level_not_8);
COMPILE_ASSERT(offsetof(CopyTexSubImage2D, xoffset) == 12,
               OffsetOf_CopyTexSubImage2D_xoffset_not_12);
COMPILE_ASSERT(offsetof(CopyTexSubImage2D, yoffset) == 16,
               OffsetOf_CopyTexSubImage2D_yoffset_not_16);
COMPILE_ASSERT(offsetof(CopyTexSubImage2D, x) == 20,
               OffsetOf_CopyTexSubImage2D_x_not_20);
COMPILE_ASSERT(offsetof(CopyTexSubImage2D, y) == 24,
               OffsetOf_CopyTexSubImage2D_y_not_24);
COMPILE_ASSERT(offsetof(CopyTexSubImage2D, width) == 28,
               OffsetOf_CopyTexSubImage2D_width_not_28);
COMPILE_ASSERT(offsetof(CopyTexSubImage2D, height) == 32,
               OffsetOf_CopyTexSubImage2D_height_not_32);

struct CreateProgram {
  typedef CreateProgram ValueType;
  static const CommandId kCmdId = kCreateProgram;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(uint32_t _client_id) {
    SetHeader();
    client_id = _client_id;
  }

  void* Set(void* cmd, uint32_t _client_id) {
    static_cast<ValueType*>(cmd)->Init(_client_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t client_id;
};

COMPILE_ASSERT(sizeof(CreateProgram) == 8, Sizeof_CreateProgram_is_not_8);
COMPILE_ASSERT(offsetof(CreateProgram, header) == 0,
               OffsetOf_CreateProgram_header_not_0);
COMPILE_ASSERT(offsetof(CreateProgram, client_id) == 4,
               OffsetOf_CreateProgram_client_id_not_4);

struct CreateShader {
  typedef CreateShader ValueType;
  static const CommandId kCmdId = kCreateShader;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _type, uint32_t _client_id) {
    SetHeader();
    type = _type;
    client_id = _client_id;
  }

  void* Set(void* cmd, GLenum _type, uint32_t _client_id) {
    static_cast<ValueType*>(cmd)->Init(_type, _client_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t type;
  uint32_t client_id;
};

COMPILE_ASSERT(sizeof(CreateShader) == 12, Sizeof_CreateShader_is_not_12);
COMPILE_ASSERT(offsetof(CreateShader, header) == 0,
               OffsetOf_CreateShader_header_not_0);
COMPILE_ASSERT(offsetof(CreateShader, type) == 4,
               OffsetOf_CreateShader_type_not_4);
COMPILE_ASSERT(offsetof(CreateShader, client_id) == 8,
               OffsetOf_CreateShader_client_id_not_8);

struct CullFace {
  typedef CullFace ValueType;
  static const CommandId kCmdId = kCullFace;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _mode) {
    SetHeader();
    mode = _mode;
  }

  void* Set(void* cmd, GLenum _mode) {
    static_cast<ValueType*>(cmd)->Init(_mode);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t mode;
};

COMPILE_ASSERT(sizeof(CullFace) == 8, Sizeof_CullFace_is_not_8);
COMPILE_ASSERT(offsetof(CullFace, header) == 0, OffsetOf_CullFace_header_not_0);
COMPILE_ASSERT(offsetof(CullFace, mode) == 4, OffsetOf_CullFace_mode_not_4);

struct DeleteBuffersImmediate {
  typedef DeleteBuffersImmediate ValueType;
  static const CommandId kCmdId = kDeleteBuffersImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, const GLuint* _buffers) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _buffers, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, const GLuint* _buffers) {
    static_cast<ValueType*>(cmd)->Init(_n, _buffers);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(DeleteBuffersImmediate) == 8,
               Sizeof_DeleteBuffersImmediate_is_not_8);
COMPILE_ASSERT(offsetof(DeleteBuffersImmediate, header) == 0,
               OffsetOf_DeleteBuffersImmediate_header_not_0);
COMPILE_ASSERT(offsetof(DeleteBuffersImmediate, n) == 4,
               OffsetOf_DeleteBuffersImmediate_n_not_4);

struct DeleteFramebuffersImmediate {
  typedef DeleteFramebuffersImmediate ValueType;
  static const CommandId kCmdId = kDeleteFramebuffersImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, const GLuint* _framebuffers) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _framebuffers, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, const GLuint* _framebuffers) {
    static_cast<ValueType*>(cmd)->Init(_n, _framebuffers);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(DeleteFramebuffersImmediate) == 8,
               Sizeof_DeleteFramebuffersImmediate_is_not_8);
COMPILE_ASSERT(offsetof(DeleteFramebuffersImmediate, header) == 0,
               OffsetOf_DeleteFramebuffersImmediate_header_not_0);
COMPILE_ASSERT(offsetof(DeleteFramebuffersImmediate, n) == 4,
               OffsetOf_DeleteFramebuffersImmediate_n_not_4);

struct DeleteProgram {
  typedef DeleteProgram ValueType;
  static const CommandId kCmdId = kDeleteProgram;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program) {
    SetHeader();
    program = _program;
  }

  void* Set(void* cmd, GLuint _program) {
    static_cast<ValueType*>(cmd)->Init(_program);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
};

COMPILE_ASSERT(sizeof(DeleteProgram) == 8, Sizeof_DeleteProgram_is_not_8);
COMPILE_ASSERT(offsetof(DeleteProgram, header) == 0,
               OffsetOf_DeleteProgram_header_not_0);
COMPILE_ASSERT(offsetof(DeleteProgram, program) == 4,
               OffsetOf_DeleteProgram_program_not_4);

struct DeleteRenderbuffersImmediate {
  typedef DeleteRenderbuffersImmediate ValueType;
  static const CommandId kCmdId = kDeleteRenderbuffersImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, const GLuint* _renderbuffers) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _renderbuffers, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, const GLuint* _renderbuffers) {
    static_cast<ValueType*>(cmd)->Init(_n, _renderbuffers);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(DeleteRenderbuffersImmediate) == 8,
               Sizeof_DeleteRenderbuffersImmediate_is_not_8);
COMPILE_ASSERT(offsetof(DeleteRenderbuffersImmediate, header) == 0,
               OffsetOf_DeleteRenderbuffersImmediate_header_not_0);
COMPILE_ASSERT(offsetof(DeleteRenderbuffersImmediate, n) == 4,
               OffsetOf_DeleteRenderbuffersImmediate_n_not_4);

struct DeleteShader {
  typedef DeleteShader ValueType;
  static const CommandId kCmdId = kDeleteShader;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _shader) {
    SetHeader();
    shader = _shader;
  }

  void* Set(void* cmd, GLuint _shader) {
    static_cast<ValueType*>(cmd)->Init(_shader);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t shader;
};

COMPILE_ASSERT(sizeof(DeleteShader) == 8, Sizeof_DeleteShader_is_not_8);
COMPILE_ASSERT(offsetof(DeleteShader, header) == 0,
               OffsetOf_DeleteShader_header_not_0);
COMPILE_ASSERT(offsetof(DeleteShader, shader) == 4,
               OffsetOf_DeleteShader_shader_not_4);

struct DeleteTexturesImmediate {
  typedef DeleteTexturesImmediate ValueType;
  static const CommandId kCmdId = kDeleteTexturesImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, const GLuint* _textures) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _textures, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, const GLuint* _textures) {
    static_cast<ValueType*>(cmd)->Init(_n, _textures);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(DeleteTexturesImmediate) == 8,
               Sizeof_DeleteTexturesImmediate_is_not_8);
COMPILE_ASSERT(offsetof(DeleteTexturesImmediate, header) == 0,
               OffsetOf_DeleteTexturesImmediate_header_not_0);
COMPILE_ASSERT(offsetof(DeleteTexturesImmediate, n) == 4,
               OffsetOf_DeleteTexturesImmediate_n_not_4);

struct DepthFunc {
  typedef DepthFunc ValueType;
  static const CommandId kCmdId = kDepthFunc;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _func) {
    SetHeader();
    func = _func;
  }

  void* Set(void* cmd, GLenum _func) {
    static_cast<ValueType*>(cmd)->Init(_func);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t func;
};

COMPILE_ASSERT(sizeof(DepthFunc) == 8, Sizeof_DepthFunc_is_not_8);
COMPILE_ASSERT(offsetof(DepthFunc, header) == 0,
               OffsetOf_DepthFunc_header_not_0);
COMPILE_ASSERT(offsetof(DepthFunc, func) == 4, OffsetOf_DepthFunc_func_not_4);

struct DepthMask {
  typedef DepthMask ValueType;
  static const CommandId kCmdId = kDepthMask;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLboolean _flag) {
    SetHeader();
    flag = _flag;
  }

  void* Set(void* cmd, GLboolean _flag) {
    static_cast<ValueType*>(cmd)->Init(_flag);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t flag;
};

COMPILE_ASSERT(sizeof(DepthMask) == 8, Sizeof_DepthMask_is_not_8);
COMPILE_ASSERT(offsetof(DepthMask, header) == 0,
               OffsetOf_DepthMask_header_not_0);
COMPILE_ASSERT(offsetof(DepthMask, flag) == 4, OffsetOf_DepthMask_flag_not_4);

struct DepthRangef {
  typedef DepthRangef ValueType;
  static const CommandId kCmdId = kDepthRangef;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLclampf _zNear, GLclampf _zFar) {
    SetHeader();
    zNear = _zNear;
    zFar = _zFar;
  }

  void* Set(void* cmd, GLclampf _zNear, GLclampf _zFar) {
    static_cast<ValueType*>(cmd)->Init(_zNear, _zFar);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  float zNear;
  float zFar;
};

COMPILE_ASSERT(sizeof(DepthRangef) == 12, Sizeof_DepthRangef_is_not_12);
COMPILE_ASSERT(offsetof(DepthRangef, header) == 0,
               OffsetOf_DepthRangef_header_not_0);
COMPILE_ASSERT(offsetof(DepthRangef, zNear) == 4,
               OffsetOf_DepthRangef_zNear_not_4);
COMPILE_ASSERT(offsetof(DepthRangef, zFar) == 8,
               OffsetOf_DepthRangef_zFar_not_8);

struct DetachShader {
  typedef DetachShader ValueType;
  static const CommandId kCmdId = kDetachShader;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program, GLuint _shader) {
    SetHeader();
    program = _program;
    shader = _shader;
  }

  void* Set(void* cmd, GLuint _program, GLuint _shader) {
    static_cast<ValueType*>(cmd)->Init(_program, _shader);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  uint32_t shader;
};

COMPILE_ASSERT(sizeof(DetachShader) == 12, Sizeof_DetachShader_is_not_12);
COMPILE_ASSERT(offsetof(DetachShader, header) == 0,
               OffsetOf_DetachShader_header_not_0);
COMPILE_ASSERT(offsetof(DetachShader, program) == 4,
               OffsetOf_DetachShader_program_not_4);
COMPILE_ASSERT(offsetof(DetachShader, shader) == 8,
               OffsetOf_DetachShader_shader_not_8);

struct Disable {
  typedef Disable ValueType;
  static const CommandId kCmdId = kDisable;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _cap) {
    SetHeader();
    cap = _cap;
  }

  void* Set(void* cmd, GLenum _cap) {
    static_cast<ValueType*>(cmd)->Init(_cap);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t cap;
};

COMPILE_ASSERT(sizeof(Disable) == 8, Sizeof_Disable_is_not_8);
COMPILE_ASSERT(offsetof(Disable, header) == 0, OffsetOf_Disable_header_not_0);
COMPILE_ASSERT(offsetof(Disable, cap) == 4, OffsetOf_Disable_cap_not_4);

struct DisableVertexAttribArray {
  typedef DisableVertexAttribArray ValueType;
  static const CommandId kCmdId = kDisableVertexAttribArray;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _index) {
    SetHeader();
    index = _index;
  }

  void* Set(void* cmd, GLuint _index) {
    static_cast<ValueType*>(cmd)->Init(_index);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t index;
};

COMPILE_ASSERT(sizeof(DisableVertexAttribArray) == 8,
               Sizeof_DisableVertexAttribArray_is_not_8);
COMPILE_ASSERT(offsetof(DisableVertexAttribArray, header) == 0,
               OffsetOf_DisableVertexAttribArray_header_not_0);
COMPILE_ASSERT(offsetof(DisableVertexAttribArray, index) == 4,
               OffsetOf_DisableVertexAttribArray_index_not_4);

struct DrawArrays {
  typedef DrawArrays ValueType;
  static const CommandId kCmdId = kDrawArrays;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(2);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _mode, GLint _first, GLsizei _count) {
    SetHeader();
    mode = _mode;
    first = _first;
    count = _count;
  }

  void* Set(void* cmd, GLenum _mode, GLint _first, GLsizei _count) {
    static_cast<ValueType*>(cmd)->Init(_mode, _first, _count);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t mode;
  int32_t first;
  int32_t count;
};

COMPILE_ASSERT(sizeof(DrawArrays) == 16, Sizeof_DrawArrays_is_not_16);
COMPILE_ASSERT(offsetof(DrawArrays, header) == 0,
               OffsetOf_DrawArrays_header_not_0);
COMPILE_ASSERT(offsetof(DrawArrays, mode) == 4, OffsetOf_DrawArrays_mode_not_4);
COMPILE_ASSERT(offsetof(DrawArrays, first) == 8,
               OffsetOf_DrawArrays_first_not_8);
COMPILE_ASSERT(offsetof(DrawArrays, count) == 12,
               OffsetOf_DrawArrays_count_not_12);

struct DrawElements {
  typedef DrawElements ValueType;
  static const CommandId kCmdId = kDrawElements;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(2);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _mode, GLsizei _count, GLenum _type, GLuint _index_offset) {
    SetHeader();
    mode = _mode;
    count = _count;
    type = _type;
    index_offset = _index_offset;
  }

  void* Set(void* cmd,
            GLenum _mode,
            GLsizei _count,
            GLenum _type,
            GLuint _index_offset) {
    static_cast<ValueType*>(cmd)->Init(_mode, _count, _type, _index_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t mode;
  int32_t count;
  uint32_t type;
  uint32_t index_offset;
};

COMPILE_ASSERT(sizeof(DrawElements) == 20, Sizeof_DrawElements_is_not_20);
COMPILE_ASSERT(offsetof(DrawElements, header) == 0,
               OffsetOf_DrawElements_header_not_0);
COMPILE_ASSERT(offsetof(DrawElements, mode) == 4,
               OffsetOf_DrawElements_mode_not_4);
COMPILE_ASSERT(offsetof(DrawElements, count) == 8,
               OffsetOf_DrawElements_count_not_8);
COMPILE_ASSERT(offsetof(DrawElements, type) == 12,
               OffsetOf_DrawElements_type_not_12);
COMPILE_ASSERT(offsetof(DrawElements, index_offset) == 16,
               OffsetOf_DrawElements_index_offset_not_16);

struct Enable {
  typedef Enable ValueType;
  static const CommandId kCmdId = kEnable;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _cap) {
    SetHeader();
    cap = _cap;
  }

  void* Set(void* cmd, GLenum _cap) {
    static_cast<ValueType*>(cmd)->Init(_cap);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t cap;
};

COMPILE_ASSERT(sizeof(Enable) == 8, Sizeof_Enable_is_not_8);
COMPILE_ASSERT(offsetof(Enable, header) == 0, OffsetOf_Enable_header_not_0);
COMPILE_ASSERT(offsetof(Enable, cap) == 4, OffsetOf_Enable_cap_not_4);

struct EnableVertexAttribArray {
  typedef EnableVertexAttribArray ValueType;
  static const CommandId kCmdId = kEnableVertexAttribArray;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _index) {
    SetHeader();
    index = _index;
  }

  void* Set(void* cmd, GLuint _index) {
    static_cast<ValueType*>(cmd)->Init(_index);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t index;
};

COMPILE_ASSERT(sizeof(EnableVertexAttribArray) == 8,
               Sizeof_EnableVertexAttribArray_is_not_8);
COMPILE_ASSERT(offsetof(EnableVertexAttribArray, header) == 0,
               OffsetOf_EnableVertexAttribArray_header_not_0);
COMPILE_ASSERT(offsetof(EnableVertexAttribArray, index) == 4,
               OffsetOf_EnableVertexAttribArray_index_not_4);

struct Finish {
  typedef Finish ValueType;
  static const CommandId kCmdId = kFinish;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init() { SetHeader(); }

  void* Set(void* cmd) {
    static_cast<ValueType*>(cmd)->Init();
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
};

COMPILE_ASSERT(sizeof(Finish) == 4, Sizeof_Finish_is_not_4);
COMPILE_ASSERT(offsetof(Finish, header) == 0, OffsetOf_Finish_header_not_0);

struct Flush {
  typedef Flush ValueType;
  static const CommandId kCmdId = kFlush;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init() { SetHeader(); }

  void* Set(void* cmd) {
    static_cast<ValueType*>(cmd)->Init();
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
};

COMPILE_ASSERT(sizeof(Flush) == 4, Sizeof_Flush_is_not_4);
COMPILE_ASSERT(offsetof(Flush, header) == 0, OffsetOf_Flush_header_not_0);

struct FramebufferRenderbuffer {
  typedef FramebufferRenderbuffer ValueType;
  static const CommandId kCmdId = kFramebufferRenderbuffer;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLenum _attachment,
            GLenum _renderbuffertarget,
            GLuint _renderbuffer) {
    SetHeader();
    target = _target;
    attachment = _attachment;
    renderbuffertarget = _renderbuffertarget;
    renderbuffer = _renderbuffer;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLenum _attachment,
            GLenum _renderbuffertarget,
            GLuint _renderbuffer) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _attachment, _renderbuffertarget, _renderbuffer);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t attachment;
  uint32_t renderbuffertarget;
  uint32_t renderbuffer;
};

COMPILE_ASSERT(sizeof(FramebufferRenderbuffer) == 20,
               Sizeof_FramebufferRenderbuffer_is_not_20);
COMPILE_ASSERT(offsetof(FramebufferRenderbuffer, header) == 0,
               OffsetOf_FramebufferRenderbuffer_header_not_0);
COMPILE_ASSERT(offsetof(FramebufferRenderbuffer, target) == 4,
               OffsetOf_FramebufferRenderbuffer_target_not_4);
COMPILE_ASSERT(offsetof(FramebufferRenderbuffer, attachment) == 8,
               OffsetOf_FramebufferRenderbuffer_attachment_not_8);
COMPILE_ASSERT(offsetof(FramebufferRenderbuffer, renderbuffertarget) == 12,
               OffsetOf_FramebufferRenderbuffer_renderbuffertarget_not_12);
COMPILE_ASSERT(offsetof(FramebufferRenderbuffer, renderbuffer) == 16,
               OffsetOf_FramebufferRenderbuffer_renderbuffer_not_16);

struct FramebufferTexture2D {
  typedef FramebufferTexture2D ValueType;
  static const CommandId kCmdId = kFramebufferTexture2D;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLenum _attachment,
            GLenum _textarget,
            GLuint _texture) {
    SetHeader();
    target = _target;
    attachment = _attachment;
    textarget = _textarget;
    texture = _texture;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLenum _attachment,
            GLenum _textarget,
            GLuint _texture) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _attachment, _textarget, _texture);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t attachment;
  uint32_t textarget;
  uint32_t texture;
  static const int32_t level = 0;
};

COMPILE_ASSERT(sizeof(FramebufferTexture2D) == 20,
               Sizeof_FramebufferTexture2D_is_not_20);
COMPILE_ASSERT(offsetof(FramebufferTexture2D, header) == 0,
               OffsetOf_FramebufferTexture2D_header_not_0);
COMPILE_ASSERT(offsetof(FramebufferTexture2D, target) == 4,
               OffsetOf_FramebufferTexture2D_target_not_4);
COMPILE_ASSERT(offsetof(FramebufferTexture2D, attachment) == 8,
               OffsetOf_FramebufferTexture2D_attachment_not_8);
COMPILE_ASSERT(offsetof(FramebufferTexture2D, textarget) == 12,
               OffsetOf_FramebufferTexture2D_textarget_not_12);
COMPILE_ASSERT(offsetof(FramebufferTexture2D, texture) == 16,
               OffsetOf_FramebufferTexture2D_texture_not_16);

struct FrontFace {
  typedef FrontFace ValueType;
  static const CommandId kCmdId = kFrontFace;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _mode) {
    SetHeader();
    mode = _mode;
  }

  void* Set(void* cmd, GLenum _mode) {
    static_cast<ValueType*>(cmd)->Init(_mode);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t mode;
};

COMPILE_ASSERT(sizeof(FrontFace) == 8, Sizeof_FrontFace_is_not_8);
COMPILE_ASSERT(offsetof(FrontFace, header) == 0,
               OffsetOf_FrontFace_header_not_0);
COMPILE_ASSERT(offsetof(FrontFace, mode) == 4, OffsetOf_FrontFace_mode_not_4);

struct GenBuffersImmediate {
  typedef GenBuffersImmediate ValueType;
  static const CommandId kCmdId = kGenBuffersImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, GLuint* _buffers) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _buffers, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, GLuint* _buffers) {
    static_cast<ValueType*>(cmd)->Init(_n, _buffers);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(GenBuffersImmediate) == 8,
               Sizeof_GenBuffersImmediate_is_not_8);
COMPILE_ASSERT(offsetof(GenBuffersImmediate, header) == 0,
               OffsetOf_GenBuffersImmediate_header_not_0);
COMPILE_ASSERT(offsetof(GenBuffersImmediate, n) == 4,
               OffsetOf_GenBuffersImmediate_n_not_4);

struct GenerateMipmap {
  typedef GenerateMipmap ValueType;
  static const CommandId kCmdId = kGenerateMipmap;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target) {
    SetHeader();
    target = _target;
  }

  void* Set(void* cmd, GLenum _target) {
    static_cast<ValueType*>(cmd)->Init(_target);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
};

COMPILE_ASSERT(sizeof(GenerateMipmap) == 8, Sizeof_GenerateMipmap_is_not_8);
COMPILE_ASSERT(offsetof(GenerateMipmap, header) == 0,
               OffsetOf_GenerateMipmap_header_not_0);
COMPILE_ASSERT(offsetof(GenerateMipmap, target) == 4,
               OffsetOf_GenerateMipmap_target_not_4);

struct GenFramebuffersImmediate {
  typedef GenFramebuffersImmediate ValueType;
  static const CommandId kCmdId = kGenFramebuffersImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, GLuint* _framebuffers) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _framebuffers, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, GLuint* _framebuffers) {
    static_cast<ValueType*>(cmd)->Init(_n, _framebuffers);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(GenFramebuffersImmediate) == 8,
               Sizeof_GenFramebuffersImmediate_is_not_8);
COMPILE_ASSERT(offsetof(GenFramebuffersImmediate, header) == 0,
               OffsetOf_GenFramebuffersImmediate_header_not_0);
COMPILE_ASSERT(offsetof(GenFramebuffersImmediate, n) == 4,
               OffsetOf_GenFramebuffersImmediate_n_not_4);

struct GenRenderbuffersImmediate {
  typedef GenRenderbuffersImmediate ValueType;
  static const CommandId kCmdId = kGenRenderbuffersImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, GLuint* _renderbuffers) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _renderbuffers, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, GLuint* _renderbuffers) {
    static_cast<ValueType*>(cmd)->Init(_n, _renderbuffers);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(GenRenderbuffersImmediate) == 8,
               Sizeof_GenRenderbuffersImmediate_is_not_8);
COMPILE_ASSERT(offsetof(GenRenderbuffersImmediate, header) == 0,
               OffsetOf_GenRenderbuffersImmediate_header_not_0);
COMPILE_ASSERT(offsetof(GenRenderbuffersImmediate, n) == 4,
               OffsetOf_GenRenderbuffersImmediate_n_not_4);

struct GenTexturesImmediate {
  typedef GenTexturesImmediate ValueType;
  static const CommandId kCmdId = kGenTexturesImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, GLuint* _textures) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _textures, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, GLuint* _textures) {
    static_cast<ValueType*>(cmd)->Init(_n, _textures);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(GenTexturesImmediate) == 8,
               Sizeof_GenTexturesImmediate_is_not_8);
COMPILE_ASSERT(offsetof(GenTexturesImmediate, header) == 0,
               OffsetOf_GenTexturesImmediate_header_not_0);
COMPILE_ASSERT(offsetof(GenTexturesImmediate, n) == 4,
               OffsetOf_GenTexturesImmediate_n_not_4);

struct GetActiveAttrib {
  typedef GetActiveAttrib ValueType;
  static const CommandId kCmdId = kGetActiveAttrib;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  struct Result {
    int32_t success;
    int32_t size;
    uint32_t type;
  };

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program,
            GLuint _index,
            uint32_t _name_bucket_id,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    program = _program;
    index = _index;
    name_bucket_id = _name_bucket_id;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _program,
            GLuint _index,
            uint32_t _name_bucket_id,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)->Init(
        _program, _index, _name_bucket_id, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  uint32_t index;
  uint32_t name_bucket_id;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(GetActiveAttrib) == 24, Sizeof_GetActiveAttrib_is_not_24);
COMPILE_ASSERT(offsetof(GetActiveAttrib, header) == 0,
               OffsetOf_GetActiveAttrib_header_not_0);
COMPILE_ASSERT(offsetof(GetActiveAttrib, program) == 4,
               OffsetOf_GetActiveAttrib_program_not_4);
COMPILE_ASSERT(offsetof(GetActiveAttrib, index) == 8,
               OffsetOf_GetActiveAttrib_index_not_8);
COMPILE_ASSERT(offsetof(GetActiveAttrib, name_bucket_id) == 12,
               OffsetOf_GetActiveAttrib_name_bucket_id_not_12);
COMPILE_ASSERT(offsetof(GetActiveAttrib, result_shm_id) == 16,
               OffsetOf_GetActiveAttrib_result_shm_id_not_16);
COMPILE_ASSERT(offsetof(GetActiveAttrib, result_shm_offset) == 20,
               OffsetOf_GetActiveAttrib_result_shm_offset_not_20);
COMPILE_ASSERT(offsetof(GetActiveAttrib::Result, success) == 0,
               OffsetOf_GetActiveAttrib_Result_success_not_0);
COMPILE_ASSERT(offsetof(GetActiveAttrib::Result, size) == 4,
               OffsetOf_GetActiveAttrib_Result_size_not_4);
COMPILE_ASSERT(offsetof(GetActiveAttrib::Result, type) == 8,
               OffsetOf_GetActiveAttrib_Result_type_not_8);

struct GetActiveUniform {
  typedef GetActiveUniform ValueType;
  static const CommandId kCmdId = kGetActiveUniform;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  struct Result {
    int32_t success;
    int32_t size;
    uint32_t type;
  };

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program,
            GLuint _index,
            uint32_t _name_bucket_id,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    program = _program;
    index = _index;
    name_bucket_id = _name_bucket_id;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _program,
            GLuint _index,
            uint32_t _name_bucket_id,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)->Init(
        _program, _index, _name_bucket_id, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  uint32_t index;
  uint32_t name_bucket_id;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(GetActiveUniform) == 24,
               Sizeof_GetActiveUniform_is_not_24);
COMPILE_ASSERT(offsetof(GetActiveUniform, header) == 0,
               OffsetOf_GetActiveUniform_header_not_0);
COMPILE_ASSERT(offsetof(GetActiveUniform, program) == 4,
               OffsetOf_GetActiveUniform_program_not_4);
COMPILE_ASSERT(offsetof(GetActiveUniform, index) == 8,
               OffsetOf_GetActiveUniform_index_not_8);
COMPILE_ASSERT(offsetof(GetActiveUniform, name_bucket_id) == 12,
               OffsetOf_GetActiveUniform_name_bucket_id_not_12);
COMPILE_ASSERT(offsetof(GetActiveUniform, result_shm_id) == 16,
               OffsetOf_GetActiveUniform_result_shm_id_not_16);
COMPILE_ASSERT(offsetof(GetActiveUniform, result_shm_offset) == 20,
               OffsetOf_GetActiveUniform_result_shm_offset_not_20);
COMPILE_ASSERT(offsetof(GetActiveUniform::Result, success) == 0,
               OffsetOf_GetActiveUniform_Result_success_not_0);
COMPILE_ASSERT(offsetof(GetActiveUniform::Result, size) == 4,
               OffsetOf_GetActiveUniform_Result_size_not_4);
COMPILE_ASSERT(offsetof(GetActiveUniform::Result, type) == 8,
               OffsetOf_GetActiveUniform_Result_type_not_8);

struct GetAttachedShaders {
  typedef GetAttachedShaders ValueType;
  static const CommandId kCmdId = kGetAttachedShaders;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLuint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset,
            uint32_t _result_size) {
    SetHeader();
    program = _program;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
    result_size = _result_size;
  }

  void* Set(void* cmd,
            GLuint _program,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset,
            uint32_t _result_size) {
    static_cast<ValueType*>(cmd)
        ->Init(_program, _result_shm_id, _result_shm_offset, _result_size);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
  uint32_t result_size;
};

COMPILE_ASSERT(sizeof(GetAttachedShaders) == 20,
               Sizeof_GetAttachedShaders_is_not_20);
COMPILE_ASSERT(offsetof(GetAttachedShaders, header) == 0,
               OffsetOf_GetAttachedShaders_header_not_0);
COMPILE_ASSERT(offsetof(GetAttachedShaders, program) == 4,
               OffsetOf_GetAttachedShaders_program_not_4);
COMPILE_ASSERT(offsetof(GetAttachedShaders, result_shm_id) == 8,
               OffsetOf_GetAttachedShaders_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(GetAttachedShaders, result_shm_offset) == 12,
               OffsetOf_GetAttachedShaders_result_shm_offset_not_12);
COMPILE_ASSERT(offsetof(GetAttachedShaders, result_size) == 16,
               OffsetOf_GetAttachedShaders_result_size_not_16);

struct GetBooleanv {
  typedef GetBooleanv ValueType;
  static const CommandId kCmdId = kGetBooleanv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLboolean> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetBooleanv) == 16, Sizeof_GetBooleanv_is_not_16);
COMPILE_ASSERT(offsetof(GetBooleanv, header) == 0,
               OffsetOf_GetBooleanv_header_not_0);
COMPILE_ASSERT(offsetof(GetBooleanv, pname) == 4,
               OffsetOf_GetBooleanv_pname_not_4);
COMPILE_ASSERT(offsetof(GetBooleanv, params_shm_id) == 8,
               OffsetOf_GetBooleanv_params_shm_id_not_8);
COMPILE_ASSERT(offsetof(GetBooleanv, params_shm_offset) == 12,
               OffsetOf_GetBooleanv_params_shm_offset_not_12);

struct GetBufferParameteriv {
  typedef GetBufferParameteriv ValueType;
  static const CommandId kCmdId = kGetBufferParameteriv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    target = _target;
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetBufferParameteriv) == 20,
               Sizeof_GetBufferParameteriv_is_not_20);
COMPILE_ASSERT(offsetof(GetBufferParameteriv, header) == 0,
               OffsetOf_GetBufferParameteriv_header_not_0);
COMPILE_ASSERT(offsetof(GetBufferParameteriv, target) == 4,
               OffsetOf_GetBufferParameteriv_target_not_4);
COMPILE_ASSERT(offsetof(GetBufferParameteriv, pname) == 8,
               OffsetOf_GetBufferParameteriv_pname_not_8);
COMPILE_ASSERT(offsetof(GetBufferParameteriv, params_shm_id) == 12,
               OffsetOf_GetBufferParameteriv_params_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetBufferParameteriv, params_shm_offset) == 16,
               OffsetOf_GetBufferParameteriv_params_shm_offset_not_16);

struct GetError {
  typedef GetError ValueType;
  static const CommandId kCmdId = kGetError;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef GLenum Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(uint32_t _result_shm_id, uint32_t _result_shm_offset) {
    SetHeader();
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd, uint32_t _result_shm_id, uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)->Init(_result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(GetError) == 12, Sizeof_GetError_is_not_12);
COMPILE_ASSERT(offsetof(GetError, header) == 0, OffsetOf_GetError_header_not_0);
COMPILE_ASSERT(offsetof(GetError, result_shm_id) == 4,
               OffsetOf_GetError_result_shm_id_not_4);
COMPILE_ASSERT(offsetof(GetError, result_shm_offset) == 8,
               OffsetOf_GetError_result_shm_offset_not_8);

struct GetFloatv {
  typedef GetFloatv ValueType;
  static const CommandId kCmdId = kGetFloatv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLfloat> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetFloatv) == 16, Sizeof_GetFloatv_is_not_16);
COMPILE_ASSERT(offsetof(GetFloatv, header) == 0,
               OffsetOf_GetFloatv_header_not_0);
COMPILE_ASSERT(offsetof(GetFloatv, pname) == 4, OffsetOf_GetFloatv_pname_not_4);
COMPILE_ASSERT(offsetof(GetFloatv, params_shm_id) == 8,
               OffsetOf_GetFloatv_params_shm_id_not_8);
COMPILE_ASSERT(offsetof(GetFloatv, params_shm_offset) == 12,
               OffsetOf_GetFloatv_params_shm_offset_not_12);

struct GetFramebufferAttachmentParameteriv {
  typedef GetFramebufferAttachmentParameteriv ValueType;
  static const CommandId kCmdId = kGetFramebufferAttachmentParameteriv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLenum _attachment,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    target = _target;
    attachment = _attachment;
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLenum _attachment,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)->Init(
        _target, _attachment, _pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t attachment;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetFramebufferAttachmentParameteriv) == 24,
               Sizeof_GetFramebufferAttachmentParameteriv_is_not_24);
COMPILE_ASSERT(offsetof(GetFramebufferAttachmentParameteriv, header) == 0,
               OffsetOf_GetFramebufferAttachmentParameteriv_header_not_0);
COMPILE_ASSERT(offsetof(GetFramebufferAttachmentParameteriv, target) == 4,
               OffsetOf_GetFramebufferAttachmentParameteriv_target_not_4);
COMPILE_ASSERT(offsetof(GetFramebufferAttachmentParameteriv, attachment) == 8,
               OffsetOf_GetFramebufferAttachmentParameteriv_attachment_not_8);
COMPILE_ASSERT(offsetof(GetFramebufferAttachmentParameteriv, pname) == 12,
               OffsetOf_GetFramebufferAttachmentParameteriv_pname_not_12);
COMPILE_ASSERT(
    offsetof(GetFramebufferAttachmentParameteriv, params_shm_id) == 16,
    OffsetOf_GetFramebufferAttachmentParameteriv_params_shm_id_not_16);
COMPILE_ASSERT(
    offsetof(GetFramebufferAttachmentParameteriv, params_shm_offset) == 20,
    OffsetOf_GetFramebufferAttachmentParameteriv_params_shm_offset_not_20);

struct GetIntegerv {
  typedef GetIntegerv ValueType;
  static const CommandId kCmdId = kGetIntegerv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetIntegerv) == 16, Sizeof_GetIntegerv_is_not_16);
COMPILE_ASSERT(offsetof(GetIntegerv, header) == 0,
               OffsetOf_GetIntegerv_header_not_0);
COMPILE_ASSERT(offsetof(GetIntegerv, pname) == 4,
               OffsetOf_GetIntegerv_pname_not_4);
COMPILE_ASSERT(offsetof(GetIntegerv, params_shm_id) == 8,
               OffsetOf_GetIntegerv_params_shm_id_not_8);
COMPILE_ASSERT(offsetof(GetIntegerv, params_shm_offset) == 12,
               OffsetOf_GetIntegerv_params_shm_offset_not_12);

struct GetProgramiv {
  typedef GetProgramiv ValueType;
  static const CommandId kCmdId = kGetProgramiv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    program = _program;
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _program,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_program, _pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetProgramiv) == 20, Sizeof_GetProgramiv_is_not_20);
COMPILE_ASSERT(offsetof(GetProgramiv, header) == 0,
               OffsetOf_GetProgramiv_header_not_0);
COMPILE_ASSERT(offsetof(GetProgramiv, program) == 4,
               OffsetOf_GetProgramiv_program_not_4);
COMPILE_ASSERT(offsetof(GetProgramiv, pname) == 8,
               OffsetOf_GetProgramiv_pname_not_8);
COMPILE_ASSERT(offsetof(GetProgramiv, params_shm_id) == 12,
               OffsetOf_GetProgramiv_params_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetProgramiv, params_shm_offset) == 16,
               OffsetOf_GetProgramiv_params_shm_offset_not_16);

struct GetProgramInfoLog {
  typedef GetProgramInfoLog ValueType;
  static const CommandId kCmdId = kGetProgramInfoLog;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program, uint32_t _bucket_id) {
    SetHeader();
    program = _program;
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, GLuint _program, uint32_t _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_program, _bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(GetProgramInfoLog) == 12,
               Sizeof_GetProgramInfoLog_is_not_12);
COMPILE_ASSERT(offsetof(GetProgramInfoLog, header) == 0,
               OffsetOf_GetProgramInfoLog_header_not_0);
COMPILE_ASSERT(offsetof(GetProgramInfoLog, program) == 4,
               OffsetOf_GetProgramInfoLog_program_not_4);
COMPILE_ASSERT(offsetof(GetProgramInfoLog, bucket_id) == 8,
               OffsetOf_GetProgramInfoLog_bucket_id_not_8);

struct GetRenderbufferParameteriv {
  typedef GetRenderbufferParameteriv ValueType;
  static const CommandId kCmdId = kGetRenderbufferParameteriv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    target = _target;
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetRenderbufferParameteriv) == 20,
               Sizeof_GetRenderbufferParameteriv_is_not_20);
COMPILE_ASSERT(offsetof(GetRenderbufferParameteriv, header) == 0,
               OffsetOf_GetRenderbufferParameteriv_header_not_0);
COMPILE_ASSERT(offsetof(GetRenderbufferParameteriv, target) == 4,
               OffsetOf_GetRenderbufferParameteriv_target_not_4);
COMPILE_ASSERT(offsetof(GetRenderbufferParameteriv, pname) == 8,
               OffsetOf_GetRenderbufferParameteriv_pname_not_8);
COMPILE_ASSERT(offsetof(GetRenderbufferParameteriv, params_shm_id) == 12,
               OffsetOf_GetRenderbufferParameteriv_params_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetRenderbufferParameteriv, params_shm_offset) == 16,
               OffsetOf_GetRenderbufferParameteriv_params_shm_offset_not_16);

struct GetShaderiv {
  typedef GetShaderiv ValueType;
  static const CommandId kCmdId = kGetShaderiv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _shader,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    shader = _shader;
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _shader,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_shader, _pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t shader;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetShaderiv) == 20, Sizeof_GetShaderiv_is_not_20);
COMPILE_ASSERT(offsetof(GetShaderiv, header) == 0,
               OffsetOf_GetShaderiv_header_not_0);
COMPILE_ASSERT(offsetof(GetShaderiv, shader) == 4,
               OffsetOf_GetShaderiv_shader_not_4);
COMPILE_ASSERT(offsetof(GetShaderiv, pname) == 8,
               OffsetOf_GetShaderiv_pname_not_8);
COMPILE_ASSERT(offsetof(GetShaderiv, params_shm_id) == 12,
               OffsetOf_GetShaderiv_params_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetShaderiv, params_shm_offset) == 16,
               OffsetOf_GetShaderiv_params_shm_offset_not_16);

struct GetShaderInfoLog {
  typedef GetShaderInfoLog ValueType;
  static const CommandId kCmdId = kGetShaderInfoLog;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _shader, uint32_t _bucket_id) {
    SetHeader();
    shader = _shader;
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, GLuint _shader, uint32_t _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_shader, _bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t shader;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(GetShaderInfoLog) == 12,
               Sizeof_GetShaderInfoLog_is_not_12);
COMPILE_ASSERT(offsetof(GetShaderInfoLog, header) == 0,
               OffsetOf_GetShaderInfoLog_header_not_0);
COMPILE_ASSERT(offsetof(GetShaderInfoLog, shader) == 4,
               OffsetOf_GetShaderInfoLog_shader_not_4);
COMPILE_ASSERT(offsetof(GetShaderInfoLog, bucket_id) == 8,
               OffsetOf_GetShaderInfoLog_bucket_id_not_8);

struct GetShaderPrecisionFormat {
  typedef GetShaderPrecisionFormat ValueType;
  static const CommandId kCmdId = kGetShaderPrecisionFormat;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  struct Result {
    int32_t success;
    int32_t min_range;
    int32_t max_range;
    int32_t precision;
  };

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _shadertype,
            GLenum _precisiontype,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    shadertype = _shadertype;
    precisiontype = _precisiontype;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _shadertype,
            GLenum _precisiontype,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_shadertype, _precisiontype, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t shadertype;
  uint32_t precisiontype;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(GetShaderPrecisionFormat) == 20,
               Sizeof_GetShaderPrecisionFormat_is_not_20);
COMPILE_ASSERT(offsetof(GetShaderPrecisionFormat, header) == 0,
               OffsetOf_GetShaderPrecisionFormat_header_not_0);
COMPILE_ASSERT(offsetof(GetShaderPrecisionFormat, shadertype) == 4,
               OffsetOf_GetShaderPrecisionFormat_shadertype_not_4);
COMPILE_ASSERT(offsetof(GetShaderPrecisionFormat, precisiontype) == 8,
               OffsetOf_GetShaderPrecisionFormat_precisiontype_not_8);
COMPILE_ASSERT(offsetof(GetShaderPrecisionFormat, result_shm_id) == 12,
               OffsetOf_GetShaderPrecisionFormat_result_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetShaderPrecisionFormat, result_shm_offset) == 16,
               OffsetOf_GetShaderPrecisionFormat_result_shm_offset_not_16);
COMPILE_ASSERT(offsetof(GetShaderPrecisionFormat::Result, success) == 0,
               OffsetOf_GetShaderPrecisionFormat_Result_success_not_0);
COMPILE_ASSERT(offsetof(GetShaderPrecisionFormat::Result, min_range) == 4,
               OffsetOf_GetShaderPrecisionFormat_Result_min_range_not_4);
COMPILE_ASSERT(offsetof(GetShaderPrecisionFormat::Result, max_range) == 8,
               OffsetOf_GetShaderPrecisionFormat_Result_max_range_not_8);
COMPILE_ASSERT(offsetof(GetShaderPrecisionFormat::Result, precision) == 12,
               OffsetOf_GetShaderPrecisionFormat_Result_precision_not_12);

struct GetShaderSource {
  typedef GetShaderSource ValueType;
  static const CommandId kCmdId = kGetShaderSource;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _shader, uint32_t _bucket_id) {
    SetHeader();
    shader = _shader;
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, GLuint _shader, uint32_t _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_shader, _bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t shader;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(GetShaderSource) == 12, Sizeof_GetShaderSource_is_not_12);
COMPILE_ASSERT(offsetof(GetShaderSource, header) == 0,
               OffsetOf_GetShaderSource_header_not_0);
COMPILE_ASSERT(offsetof(GetShaderSource, shader) == 4,
               OffsetOf_GetShaderSource_shader_not_4);
COMPILE_ASSERT(offsetof(GetShaderSource, bucket_id) == 8,
               OffsetOf_GetShaderSource_bucket_id_not_8);

struct GetString {
  typedef GetString ValueType;
  static const CommandId kCmdId = kGetString;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _name, uint32_t _bucket_id) {
    SetHeader();
    name = _name;
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, GLenum _name, uint32_t _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_name, _bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t name;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(GetString) == 12, Sizeof_GetString_is_not_12);
COMPILE_ASSERT(offsetof(GetString, header) == 0,
               OffsetOf_GetString_header_not_0);
COMPILE_ASSERT(offsetof(GetString, name) == 4, OffsetOf_GetString_name_not_4);
COMPILE_ASSERT(offsetof(GetString, bucket_id) == 8,
               OffsetOf_GetString_bucket_id_not_8);

struct GetTexParameterfv {
  typedef GetTexParameterfv ValueType;
  static const CommandId kCmdId = kGetTexParameterfv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLfloat> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    target = _target;
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetTexParameterfv) == 20,
               Sizeof_GetTexParameterfv_is_not_20);
COMPILE_ASSERT(offsetof(GetTexParameterfv, header) == 0,
               OffsetOf_GetTexParameterfv_header_not_0);
COMPILE_ASSERT(offsetof(GetTexParameterfv, target) == 4,
               OffsetOf_GetTexParameterfv_target_not_4);
COMPILE_ASSERT(offsetof(GetTexParameterfv, pname) == 8,
               OffsetOf_GetTexParameterfv_pname_not_8);
COMPILE_ASSERT(offsetof(GetTexParameterfv, params_shm_id) == 12,
               OffsetOf_GetTexParameterfv_params_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetTexParameterfv, params_shm_offset) == 16,
               OffsetOf_GetTexParameterfv_params_shm_offset_not_16);

struct GetTexParameteriv {
  typedef GetTexParameteriv ValueType;
  static const CommandId kCmdId = kGetTexParameteriv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    target = _target;
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetTexParameteriv) == 20,
               Sizeof_GetTexParameteriv_is_not_20);
COMPILE_ASSERT(offsetof(GetTexParameteriv, header) == 0,
               OffsetOf_GetTexParameteriv_header_not_0);
COMPILE_ASSERT(offsetof(GetTexParameteriv, target) == 4,
               OffsetOf_GetTexParameteriv_target_not_4);
COMPILE_ASSERT(offsetof(GetTexParameteriv, pname) == 8,
               OffsetOf_GetTexParameteriv_pname_not_8);
COMPILE_ASSERT(offsetof(GetTexParameteriv, params_shm_id) == 12,
               OffsetOf_GetTexParameteriv_params_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetTexParameteriv, params_shm_offset) == 16,
               OffsetOf_GetTexParameteriv_params_shm_offset_not_16);

struct GetUniformfv {
  typedef GetUniformfv ValueType;
  static const CommandId kCmdId = kGetUniformfv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLfloat> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program,
            GLint _location,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    program = _program;
    location = _location;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _program,
            GLint _location,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_program, _location, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  int32_t location;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetUniformfv) == 20, Sizeof_GetUniformfv_is_not_20);
COMPILE_ASSERT(offsetof(GetUniformfv, header) == 0,
               OffsetOf_GetUniformfv_header_not_0);
COMPILE_ASSERT(offsetof(GetUniformfv, program) == 4,
               OffsetOf_GetUniformfv_program_not_4);
COMPILE_ASSERT(offsetof(GetUniformfv, location) == 8,
               OffsetOf_GetUniformfv_location_not_8);
COMPILE_ASSERT(offsetof(GetUniformfv, params_shm_id) == 12,
               OffsetOf_GetUniformfv_params_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetUniformfv, params_shm_offset) == 16,
               OffsetOf_GetUniformfv_params_shm_offset_not_16);

struct GetUniformiv {
  typedef GetUniformiv ValueType;
  static const CommandId kCmdId = kGetUniformiv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program,
            GLint _location,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    program = _program;
    location = _location;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _program,
            GLint _location,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_program, _location, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  int32_t location;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetUniformiv) == 20, Sizeof_GetUniformiv_is_not_20);
COMPILE_ASSERT(offsetof(GetUniformiv, header) == 0,
               OffsetOf_GetUniformiv_header_not_0);
COMPILE_ASSERT(offsetof(GetUniformiv, program) == 4,
               OffsetOf_GetUniformiv_program_not_4);
COMPILE_ASSERT(offsetof(GetUniformiv, location) == 8,
               OffsetOf_GetUniformiv_location_not_8);
COMPILE_ASSERT(offsetof(GetUniformiv, params_shm_id) == 12,
               OffsetOf_GetUniformiv_params_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetUniformiv, params_shm_offset) == 16,
               OffsetOf_GetUniformiv_params_shm_offset_not_16);

struct GetVertexAttribfv {
  typedef GetVertexAttribfv ValueType;
  static const CommandId kCmdId = kGetVertexAttribfv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLfloat> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _index,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    index = _index;
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _index,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_index, _pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t index;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetVertexAttribfv) == 20,
               Sizeof_GetVertexAttribfv_is_not_20);
COMPILE_ASSERT(offsetof(GetVertexAttribfv, header) == 0,
               OffsetOf_GetVertexAttribfv_header_not_0);
COMPILE_ASSERT(offsetof(GetVertexAttribfv, index) == 4,
               OffsetOf_GetVertexAttribfv_index_not_4);
COMPILE_ASSERT(offsetof(GetVertexAttribfv, pname) == 8,
               OffsetOf_GetVertexAttribfv_pname_not_8);
COMPILE_ASSERT(offsetof(GetVertexAttribfv, params_shm_id) == 12,
               OffsetOf_GetVertexAttribfv_params_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetVertexAttribfv, params_shm_offset) == 16,
               OffsetOf_GetVertexAttribfv_params_shm_offset_not_16);

struct GetVertexAttribiv {
  typedef GetVertexAttribiv ValueType;
  static const CommandId kCmdId = kGetVertexAttribiv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _index,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    SetHeader();
    index = _index;
    pname = _pname;
    params_shm_id = _params_shm_id;
    params_shm_offset = _params_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _index,
            GLenum _pname,
            uint32_t _params_shm_id,
            uint32_t _params_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_index, _pname, _params_shm_id, _params_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t index;
  uint32_t pname;
  uint32_t params_shm_id;
  uint32_t params_shm_offset;
};

COMPILE_ASSERT(sizeof(GetVertexAttribiv) == 20,
               Sizeof_GetVertexAttribiv_is_not_20);
COMPILE_ASSERT(offsetof(GetVertexAttribiv, header) == 0,
               OffsetOf_GetVertexAttribiv_header_not_0);
COMPILE_ASSERT(offsetof(GetVertexAttribiv, index) == 4,
               OffsetOf_GetVertexAttribiv_index_not_4);
COMPILE_ASSERT(offsetof(GetVertexAttribiv, pname) == 8,
               OffsetOf_GetVertexAttribiv_pname_not_8);
COMPILE_ASSERT(offsetof(GetVertexAttribiv, params_shm_id) == 12,
               OffsetOf_GetVertexAttribiv_params_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetVertexAttribiv, params_shm_offset) == 16,
               OffsetOf_GetVertexAttribiv_params_shm_offset_not_16);

struct GetVertexAttribPointerv {
  typedef GetVertexAttribPointerv ValueType;
  static const CommandId kCmdId = kGetVertexAttribPointerv;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef SizedResult<GLuint> Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _index,
            GLenum _pname,
            uint32_t _pointer_shm_id,
            uint32_t _pointer_shm_offset) {
    SetHeader();
    index = _index;
    pname = _pname;
    pointer_shm_id = _pointer_shm_id;
    pointer_shm_offset = _pointer_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _index,
            GLenum _pname,
            uint32_t _pointer_shm_id,
            uint32_t _pointer_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_index, _pname, _pointer_shm_id, _pointer_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t index;
  uint32_t pname;
  uint32_t pointer_shm_id;
  uint32_t pointer_shm_offset;
};

COMPILE_ASSERT(sizeof(GetVertexAttribPointerv) == 20,
               Sizeof_GetVertexAttribPointerv_is_not_20);
COMPILE_ASSERT(offsetof(GetVertexAttribPointerv, header) == 0,
               OffsetOf_GetVertexAttribPointerv_header_not_0);
COMPILE_ASSERT(offsetof(GetVertexAttribPointerv, index) == 4,
               OffsetOf_GetVertexAttribPointerv_index_not_4);
COMPILE_ASSERT(offsetof(GetVertexAttribPointerv, pname) == 8,
               OffsetOf_GetVertexAttribPointerv_pname_not_8);
COMPILE_ASSERT(offsetof(GetVertexAttribPointerv, pointer_shm_id) == 12,
               OffsetOf_GetVertexAttribPointerv_pointer_shm_id_not_12);
COMPILE_ASSERT(offsetof(GetVertexAttribPointerv, pointer_shm_offset) == 16,
               OffsetOf_GetVertexAttribPointerv_pointer_shm_offset_not_16);

struct Hint {
  typedef Hint ValueType;
  static const CommandId kCmdId = kHint;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target, GLenum _mode) {
    SetHeader();
    target = _target;
    mode = _mode;
  }

  void* Set(void* cmd, GLenum _target, GLenum _mode) {
    static_cast<ValueType*>(cmd)->Init(_target, _mode);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t mode;
};

COMPILE_ASSERT(sizeof(Hint) == 12, Sizeof_Hint_is_not_12);
COMPILE_ASSERT(offsetof(Hint, header) == 0, OffsetOf_Hint_header_not_0);
COMPILE_ASSERT(offsetof(Hint, target) == 4, OffsetOf_Hint_target_not_4);
COMPILE_ASSERT(offsetof(Hint, mode) == 8, OffsetOf_Hint_mode_not_8);

struct IsBuffer {
  typedef IsBuffer ValueType;
  static const CommandId kCmdId = kIsBuffer;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef uint32_t Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _buffer,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    buffer = _buffer;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _buffer,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_buffer, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t buffer;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(IsBuffer) == 16, Sizeof_IsBuffer_is_not_16);
COMPILE_ASSERT(offsetof(IsBuffer, header) == 0, OffsetOf_IsBuffer_header_not_0);
COMPILE_ASSERT(offsetof(IsBuffer, buffer) == 4, OffsetOf_IsBuffer_buffer_not_4);
COMPILE_ASSERT(offsetof(IsBuffer, result_shm_id) == 8,
               OffsetOf_IsBuffer_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(IsBuffer, result_shm_offset) == 12,
               OffsetOf_IsBuffer_result_shm_offset_not_12);

struct IsEnabled {
  typedef IsEnabled ValueType;
  static const CommandId kCmdId = kIsEnabled;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef uint32_t Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _cap, uint32_t _result_shm_id, uint32_t _result_shm_offset) {
    SetHeader();
    cap = _cap;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _cap,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_cap, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t cap;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(IsEnabled) == 16, Sizeof_IsEnabled_is_not_16);
COMPILE_ASSERT(offsetof(IsEnabled, header) == 0,
               OffsetOf_IsEnabled_header_not_0);
COMPILE_ASSERT(offsetof(IsEnabled, cap) == 4, OffsetOf_IsEnabled_cap_not_4);
COMPILE_ASSERT(offsetof(IsEnabled, result_shm_id) == 8,
               OffsetOf_IsEnabled_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(IsEnabled, result_shm_offset) == 12,
               OffsetOf_IsEnabled_result_shm_offset_not_12);

struct IsFramebuffer {
  typedef IsFramebuffer ValueType;
  static const CommandId kCmdId = kIsFramebuffer;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef uint32_t Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _framebuffer,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    framebuffer = _framebuffer;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _framebuffer,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_framebuffer, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t framebuffer;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(IsFramebuffer) == 16, Sizeof_IsFramebuffer_is_not_16);
COMPILE_ASSERT(offsetof(IsFramebuffer, header) == 0,
               OffsetOf_IsFramebuffer_header_not_0);
COMPILE_ASSERT(offsetof(IsFramebuffer, framebuffer) == 4,
               OffsetOf_IsFramebuffer_framebuffer_not_4);
COMPILE_ASSERT(offsetof(IsFramebuffer, result_shm_id) == 8,
               OffsetOf_IsFramebuffer_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(IsFramebuffer, result_shm_offset) == 12,
               OffsetOf_IsFramebuffer_result_shm_offset_not_12);

struct IsProgram {
  typedef IsProgram ValueType;
  static const CommandId kCmdId = kIsProgram;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef uint32_t Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    program = _program;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _program,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_program, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(IsProgram) == 16, Sizeof_IsProgram_is_not_16);
COMPILE_ASSERT(offsetof(IsProgram, header) == 0,
               OffsetOf_IsProgram_header_not_0);
COMPILE_ASSERT(offsetof(IsProgram, program) == 4,
               OffsetOf_IsProgram_program_not_4);
COMPILE_ASSERT(offsetof(IsProgram, result_shm_id) == 8,
               OffsetOf_IsProgram_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(IsProgram, result_shm_offset) == 12,
               OffsetOf_IsProgram_result_shm_offset_not_12);

struct IsRenderbuffer {
  typedef IsRenderbuffer ValueType;
  static const CommandId kCmdId = kIsRenderbuffer;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef uint32_t Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _renderbuffer,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    renderbuffer = _renderbuffer;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _renderbuffer,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_renderbuffer, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t renderbuffer;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(IsRenderbuffer) == 16, Sizeof_IsRenderbuffer_is_not_16);
COMPILE_ASSERT(offsetof(IsRenderbuffer, header) == 0,
               OffsetOf_IsRenderbuffer_header_not_0);
COMPILE_ASSERT(offsetof(IsRenderbuffer, renderbuffer) == 4,
               OffsetOf_IsRenderbuffer_renderbuffer_not_4);
COMPILE_ASSERT(offsetof(IsRenderbuffer, result_shm_id) == 8,
               OffsetOf_IsRenderbuffer_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(IsRenderbuffer, result_shm_offset) == 12,
               OffsetOf_IsRenderbuffer_result_shm_offset_not_12);

struct IsShader {
  typedef IsShader ValueType;
  static const CommandId kCmdId = kIsShader;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef uint32_t Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _shader,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    shader = _shader;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _shader,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_shader, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t shader;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(IsShader) == 16, Sizeof_IsShader_is_not_16);
COMPILE_ASSERT(offsetof(IsShader, header) == 0, OffsetOf_IsShader_header_not_0);
COMPILE_ASSERT(offsetof(IsShader, shader) == 4, OffsetOf_IsShader_shader_not_4);
COMPILE_ASSERT(offsetof(IsShader, result_shm_id) == 8,
               OffsetOf_IsShader_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(IsShader, result_shm_offset) == 12,
               OffsetOf_IsShader_result_shm_offset_not_12);

struct IsTexture {
  typedef IsTexture ValueType;
  static const CommandId kCmdId = kIsTexture;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef uint32_t Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _texture,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    texture = _texture;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _texture,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_texture, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t texture;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(IsTexture) == 16, Sizeof_IsTexture_is_not_16);
COMPILE_ASSERT(offsetof(IsTexture, header) == 0,
               OffsetOf_IsTexture_header_not_0);
COMPILE_ASSERT(offsetof(IsTexture, texture) == 4,
               OffsetOf_IsTexture_texture_not_4);
COMPILE_ASSERT(offsetof(IsTexture, result_shm_id) == 8,
               OffsetOf_IsTexture_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(IsTexture, result_shm_offset) == 12,
               OffsetOf_IsTexture_result_shm_offset_not_12);

struct LineWidth {
  typedef LineWidth ValueType;
  static const CommandId kCmdId = kLineWidth;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLfloat _width) {
    SetHeader();
    width = _width;
  }

  void* Set(void* cmd, GLfloat _width) {
    static_cast<ValueType*>(cmd)->Init(_width);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  float width;
};

COMPILE_ASSERT(sizeof(LineWidth) == 8, Sizeof_LineWidth_is_not_8);
COMPILE_ASSERT(offsetof(LineWidth, header) == 0,
               OffsetOf_LineWidth_header_not_0);
COMPILE_ASSERT(offsetof(LineWidth, width) == 4, OffsetOf_LineWidth_width_not_4);

struct LinkProgram {
  typedef LinkProgram ValueType;
  static const CommandId kCmdId = kLinkProgram;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program) {
    SetHeader();
    program = _program;
  }

  void* Set(void* cmd, GLuint _program) {
    static_cast<ValueType*>(cmd)->Init(_program);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
};

COMPILE_ASSERT(sizeof(LinkProgram) == 8, Sizeof_LinkProgram_is_not_8);
COMPILE_ASSERT(offsetof(LinkProgram, header) == 0,
               OffsetOf_LinkProgram_header_not_0);
COMPILE_ASSERT(offsetof(LinkProgram, program) == 4,
               OffsetOf_LinkProgram_program_not_4);

struct PixelStorei {
  typedef PixelStorei ValueType;
  static const CommandId kCmdId = kPixelStorei;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _pname, GLint _param) {
    SetHeader();
    pname = _pname;
    param = _param;
  }

  void* Set(void* cmd, GLenum _pname, GLint _param) {
    static_cast<ValueType*>(cmd)->Init(_pname, _param);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t pname;
  int32_t param;
};

COMPILE_ASSERT(sizeof(PixelStorei) == 12, Sizeof_PixelStorei_is_not_12);
COMPILE_ASSERT(offsetof(PixelStorei, header) == 0,
               OffsetOf_PixelStorei_header_not_0);
COMPILE_ASSERT(offsetof(PixelStorei, pname) == 4,
               OffsetOf_PixelStorei_pname_not_4);
COMPILE_ASSERT(offsetof(PixelStorei, param) == 8,
               OffsetOf_PixelStorei_param_not_8);

struct PolygonOffset {
  typedef PolygonOffset ValueType;
  static const CommandId kCmdId = kPolygonOffset;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLfloat _factor, GLfloat _units) {
    SetHeader();
    factor = _factor;
    units = _units;
  }

  void* Set(void* cmd, GLfloat _factor, GLfloat _units) {
    static_cast<ValueType*>(cmd)->Init(_factor, _units);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  float factor;
  float units;
};

COMPILE_ASSERT(sizeof(PolygonOffset) == 12, Sizeof_PolygonOffset_is_not_12);
COMPILE_ASSERT(offsetof(PolygonOffset, header) == 0,
               OffsetOf_PolygonOffset_header_not_0);
COMPILE_ASSERT(offsetof(PolygonOffset, factor) == 4,
               OffsetOf_PolygonOffset_factor_not_4);
COMPILE_ASSERT(offsetof(PolygonOffset, units) == 8,
               OffsetOf_PolygonOffset_units_not_8);

// ReadPixels has the result separated from the pixel buffer so that
// it is easier to specify the result going to some specific place
// that exactly fits the rectangle of pixels.
struct ReadPixels {
  typedef ReadPixels ValueType;
  static const CommandId kCmdId = kReadPixels;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef uint32_t Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _x,
            GLint _y,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLenum _type,
            uint32_t _pixels_shm_id,
            uint32_t _pixels_shm_offset,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset,
            GLboolean _async) {
    SetHeader();
    x = _x;
    y = _y;
    width = _width;
    height = _height;
    format = _format;
    type = _type;
    pixels_shm_id = _pixels_shm_id;
    pixels_shm_offset = _pixels_shm_offset;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
    async = _async;
  }

  void* Set(void* cmd,
            GLint _x,
            GLint _y,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLenum _type,
            uint32_t _pixels_shm_id,
            uint32_t _pixels_shm_offset,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset,
            GLboolean _async) {
    static_cast<ValueType*>(cmd)->Init(_x,
                                       _y,
                                       _width,
                                       _height,
                                       _format,
                                       _type,
                                       _pixels_shm_id,
                                       _pixels_shm_offset,
                                       _result_shm_id,
                                       _result_shm_offset,
                                       _async);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
  uint32_t format;
  uint32_t type;
  uint32_t pixels_shm_id;
  uint32_t pixels_shm_offset;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
  uint32_t async;
};

COMPILE_ASSERT(sizeof(ReadPixels) == 48, Sizeof_ReadPixels_is_not_48);
COMPILE_ASSERT(offsetof(ReadPixels, header) == 0,
               OffsetOf_ReadPixels_header_not_0);
COMPILE_ASSERT(offsetof(ReadPixels, x) == 4, OffsetOf_ReadPixels_x_not_4);
COMPILE_ASSERT(offsetof(ReadPixels, y) == 8, OffsetOf_ReadPixels_y_not_8);
COMPILE_ASSERT(offsetof(ReadPixels, width) == 12,
               OffsetOf_ReadPixels_width_not_12);
COMPILE_ASSERT(offsetof(ReadPixels, height) == 16,
               OffsetOf_ReadPixels_height_not_16);
COMPILE_ASSERT(offsetof(ReadPixels, format) == 20,
               OffsetOf_ReadPixels_format_not_20);
COMPILE_ASSERT(offsetof(ReadPixels, type) == 24,
               OffsetOf_ReadPixels_type_not_24);
COMPILE_ASSERT(offsetof(ReadPixels, pixels_shm_id) == 28,
               OffsetOf_ReadPixels_pixels_shm_id_not_28);
COMPILE_ASSERT(offsetof(ReadPixels, pixels_shm_offset) == 32,
               OffsetOf_ReadPixels_pixels_shm_offset_not_32);
COMPILE_ASSERT(offsetof(ReadPixels, result_shm_id) == 36,
               OffsetOf_ReadPixels_result_shm_id_not_36);
COMPILE_ASSERT(offsetof(ReadPixels, result_shm_offset) == 40,
               OffsetOf_ReadPixels_result_shm_offset_not_40);
COMPILE_ASSERT(offsetof(ReadPixels, async) == 44,
               OffsetOf_ReadPixels_async_not_44);

struct ReleaseShaderCompiler {
  typedef ReleaseShaderCompiler ValueType;
  static const CommandId kCmdId = kReleaseShaderCompiler;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init() { SetHeader(); }

  void* Set(void* cmd) {
    static_cast<ValueType*>(cmd)->Init();
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
};

COMPILE_ASSERT(sizeof(ReleaseShaderCompiler) == 4,
               Sizeof_ReleaseShaderCompiler_is_not_4);
COMPILE_ASSERT(offsetof(ReleaseShaderCompiler, header) == 0,
               OffsetOf_ReleaseShaderCompiler_header_not_0);

struct RenderbufferStorage {
  typedef RenderbufferStorage ValueType;
  static const CommandId kCmdId = kRenderbufferStorage;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLenum _internalformat,
            GLsizei _width,
            GLsizei _height) {
    SetHeader();
    target = _target;
    internalformat = _internalformat;
    width = _width;
    height = _height;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLenum _internalformat,
            GLsizei _width,
            GLsizei _height) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _internalformat, _width, _height);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t internalformat;
  int32_t width;
  int32_t height;
};

COMPILE_ASSERT(sizeof(RenderbufferStorage) == 20,
               Sizeof_RenderbufferStorage_is_not_20);
COMPILE_ASSERT(offsetof(RenderbufferStorage, header) == 0,
               OffsetOf_RenderbufferStorage_header_not_0);
COMPILE_ASSERT(offsetof(RenderbufferStorage, target) == 4,
               OffsetOf_RenderbufferStorage_target_not_4);
COMPILE_ASSERT(offsetof(RenderbufferStorage, internalformat) == 8,
               OffsetOf_RenderbufferStorage_internalformat_not_8);
COMPILE_ASSERT(offsetof(RenderbufferStorage, width) == 12,
               OffsetOf_RenderbufferStorage_width_not_12);
COMPILE_ASSERT(offsetof(RenderbufferStorage, height) == 16,
               OffsetOf_RenderbufferStorage_height_not_16);

struct SampleCoverage {
  typedef SampleCoverage ValueType;
  static const CommandId kCmdId = kSampleCoverage;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLclampf _value, GLboolean _invert) {
    SetHeader();
    value = _value;
    invert = _invert;
  }

  void* Set(void* cmd, GLclampf _value, GLboolean _invert) {
    static_cast<ValueType*>(cmd)->Init(_value, _invert);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  float value;
  uint32_t invert;
};

COMPILE_ASSERT(sizeof(SampleCoverage) == 12, Sizeof_SampleCoverage_is_not_12);
COMPILE_ASSERT(offsetof(SampleCoverage, header) == 0,
               OffsetOf_SampleCoverage_header_not_0);
COMPILE_ASSERT(offsetof(SampleCoverage, value) == 4,
               OffsetOf_SampleCoverage_value_not_4);
COMPILE_ASSERT(offsetof(SampleCoverage, invert) == 8,
               OffsetOf_SampleCoverage_invert_not_8);

struct Scissor {
  typedef Scissor ValueType;
  static const CommandId kCmdId = kScissor;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _x, GLint _y, GLsizei _width, GLsizei _height) {
    SetHeader();
    x = _x;
    y = _y;
    width = _width;
    height = _height;
  }

  void* Set(void* cmd, GLint _x, GLint _y, GLsizei _width, GLsizei _height) {
    static_cast<ValueType*>(cmd)->Init(_x, _y, _width, _height);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
};

COMPILE_ASSERT(sizeof(Scissor) == 20, Sizeof_Scissor_is_not_20);
COMPILE_ASSERT(offsetof(Scissor, header) == 0, OffsetOf_Scissor_header_not_0);
COMPILE_ASSERT(offsetof(Scissor, x) == 4, OffsetOf_Scissor_x_not_4);
COMPILE_ASSERT(offsetof(Scissor, y) == 8, OffsetOf_Scissor_y_not_8);
COMPILE_ASSERT(offsetof(Scissor, width) == 12, OffsetOf_Scissor_width_not_12);
COMPILE_ASSERT(offsetof(Scissor, height) == 16, OffsetOf_Scissor_height_not_16);

struct ShaderBinary {
  typedef ShaderBinary ValueType;
  static const CommandId kCmdId = kShaderBinary;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLsizei _n,
            uint32_t _shaders_shm_id,
            uint32_t _shaders_shm_offset,
            GLenum _binaryformat,
            uint32_t _binary_shm_id,
            uint32_t _binary_shm_offset,
            GLsizei _length) {
    SetHeader();
    n = _n;
    shaders_shm_id = _shaders_shm_id;
    shaders_shm_offset = _shaders_shm_offset;
    binaryformat = _binaryformat;
    binary_shm_id = _binary_shm_id;
    binary_shm_offset = _binary_shm_offset;
    length = _length;
  }

  void* Set(void* cmd,
            GLsizei _n,
            uint32_t _shaders_shm_id,
            uint32_t _shaders_shm_offset,
            GLenum _binaryformat,
            uint32_t _binary_shm_id,
            uint32_t _binary_shm_offset,
            GLsizei _length) {
    static_cast<ValueType*>(cmd)->Init(_n,
                                       _shaders_shm_id,
                                       _shaders_shm_offset,
                                       _binaryformat,
                                       _binary_shm_id,
                                       _binary_shm_offset,
                                       _length);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t n;
  uint32_t shaders_shm_id;
  uint32_t shaders_shm_offset;
  uint32_t binaryformat;
  uint32_t binary_shm_id;
  uint32_t binary_shm_offset;
  int32_t length;
};

COMPILE_ASSERT(sizeof(ShaderBinary) == 32, Sizeof_ShaderBinary_is_not_32);
COMPILE_ASSERT(offsetof(ShaderBinary, header) == 0,
               OffsetOf_ShaderBinary_header_not_0);
COMPILE_ASSERT(offsetof(ShaderBinary, n) == 4, OffsetOf_ShaderBinary_n_not_4);
COMPILE_ASSERT(offsetof(ShaderBinary, shaders_shm_id) == 8,
               OffsetOf_ShaderBinary_shaders_shm_id_not_8);
COMPILE_ASSERT(offsetof(ShaderBinary, shaders_shm_offset) == 12,
               OffsetOf_ShaderBinary_shaders_shm_offset_not_12);
COMPILE_ASSERT(offsetof(ShaderBinary, binaryformat) == 16,
               OffsetOf_ShaderBinary_binaryformat_not_16);
COMPILE_ASSERT(offsetof(ShaderBinary, binary_shm_id) == 20,
               OffsetOf_ShaderBinary_binary_shm_id_not_20);
COMPILE_ASSERT(offsetof(ShaderBinary, binary_shm_offset) == 24,
               OffsetOf_ShaderBinary_binary_shm_offset_not_24);
COMPILE_ASSERT(offsetof(ShaderBinary, length) == 28,
               OffsetOf_ShaderBinary_length_not_28);

struct ShaderSourceBucket {
  typedef ShaderSourceBucket ValueType;
  static const CommandId kCmdId = kShaderSourceBucket;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _shader, uint32_t _data_bucket_id) {
    SetHeader();
    shader = _shader;
    data_bucket_id = _data_bucket_id;
  }

  void* Set(void* cmd, GLuint _shader, uint32_t _data_bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_shader, _data_bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t shader;
  uint32_t data_bucket_id;
};

COMPILE_ASSERT(sizeof(ShaderSourceBucket) == 12,
               Sizeof_ShaderSourceBucket_is_not_12);
COMPILE_ASSERT(offsetof(ShaderSourceBucket, header) == 0,
               OffsetOf_ShaderSourceBucket_header_not_0);
COMPILE_ASSERT(offsetof(ShaderSourceBucket, shader) == 4,
               OffsetOf_ShaderSourceBucket_shader_not_4);
COMPILE_ASSERT(offsetof(ShaderSourceBucket, data_bucket_id) == 8,
               OffsetOf_ShaderSourceBucket_data_bucket_id_not_8);

struct StencilFunc {
  typedef StencilFunc ValueType;
  static const CommandId kCmdId = kStencilFunc;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _func, GLint _ref, GLuint _mask) {
    SetHeader();
    func = _func;
    ref = _ref;
    mask = _mask;
  }

  void* Set(void* cmd, GLenum _func, GLint _ref, GLuint _mask) {
    static_cast<ValueType*>(cmd)->Init(_func, _ref, _mask);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t func;
  int32_t ref;
  uint32_t mask;
};

COMPILE_ASSERT(sizeof(StencilFunc) == 16, Sizeof_StencilFunc_is_not_16);
COMPILE_ASSERT(offsetof(StencilFunc, header) == 0,
               OffsetOf_StencilFunc_header_not_0);
COMPILE_ASSERT(offsetof(StencilFunc, func) == 4,
               OffsetOf_StencilFunc_func_not_4);
COMPILE_ASSERT(offsetof(StencilFunc, ref) == 8, OffsetOf_StencilFunc_ref_not_8);
COMPILE_ASSERT(offsetof(StencilFunc, mask) == 12,
               OffsetOf_StencilFunc_mask_not_12);

struct StencilFuncSeparate {
  typedef StencilFuncSeparate ValueType;
  static const CommandId kCmdId = kStencilFuncSeparate;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _face, GLenum _func, GLint _ref, GLuint _mask) {
    SetHeader();
    face = _face;
    func = _func;
    ref = _ref;
    mask = _mask;
  }

  void* Set(void* cmd, GLenum _face, GLenum _func, GLint _ref, GLuint _mask) {
    static_cast<ValueType*>(cmd)->Init(_face, _func, _ref, _mask);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t face;
  uint32_t func;
  int32_t ref;
  uint32_t mask;
};

COMPILE_ASSERT(sizeof(StencilFuncSeparate) == 20,
               Sizeof_StencilFuncSeparate_is_not_20);
COMPILE_ASSERT(offsetof(StencilFuncSeparate, header) == 0,
               OffsetOf_StencilFuncSeparate_header_not_0);
COMPILE_ASSERT(offsetof(StencilFuncSeparate, face) == 4,
               OffsetOf_StencilFuncSeparate_face_not_4);
COMPILE_ASSERT(offsetof(StencilFuncSeparate, func) == 8,
               OffsetOf_StencilFuncSeparate_func_not_8);
COMPILE_ASSERT(offsetof(StencilFuncSeparate, ref) == 12,
               OffsetOf_StencilFuncSeparate_ref_not_12);
COMPILE_ASSERT(offsetof(StencilFuncSeparate, mask) == 16,
               OffsetOf_StencilFuncSeparate_mask_not_16);

struct StencilMask {
  typedef StencilMask ValueType;
  static const CommandId kCmdId = kStencilMask;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _mask) {
    SetHeader();
    mask = _mask;
  }

  void* Set(void* cmd, GLuint _mask) {
    static_cast<ValueType*>(cmd)->Init(_mask);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t mask;
};

COMPILE_ASSERT(sizeof(StencilMask) == 8, Sizeof_StencilMask_is_not_8);
COMPILE_ASSERT(offsetof(StencilMask, header) == 0,
               OffsetOf_StencilMask_header_not_0);
COMPILE_ASSERT(offsetof(StencilMask, mask) == 4,
               OffsetOf_StencilMask_mask_not_4);

struct StencilMaskSeparate {
  typedef StencilMaskSeparate ValueType;
  static const CommandId kCmdId = kStencilMaskSeparate;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _face, GLuint _mask) {
    SetHeader();
    face = _face;
    mask = _mask;
  }

  void* Set(void* cmd, GLenum _face, GLuint _mask) {
    static_cast<ValueType*>(cmd)->Init(_face, _mask);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t face;
  uint32_t mask;
};

COMPILE_ASSERT(sizeof(StencilMaskSeparate) == 12,
               Sizeof_StencilMaskSeparate_is_not_12);
COMPILE_ASSERT(offsetof(StencilMaskSeparate, header) == 0,
               OffsetOf_StencilMaskSeparate_header_not_0);
COMPILE_ASSERT(offsetof(StencilMaskSeparate, face) == 4,
               OffsetOf_StencilMaskSeparate_face_not_4);
COMPILE_ASSERT(offsetof(StencilMaskSeparate, mask) == 8,
               OffsetOf_StencilMaskSeparate_mask_not_8);

struct StencilOp {
  typedef StencilOp ValueType;
  static const CommandId kCmdId = kStencilOp;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _fail, GLenum _zfail, GLenum _zpass) {
    SetHeader();
    fail = _fail;
    zfail = _zfail;
    zpass = _zpass;
  }

  void* Set(void* cmd, GLenum _fail, GLenum _zfail, GLenum _zpass) {
    static_cast<ValueType*>(cmd)->Init(_fail, _zfail, _zpass);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t fail;
  uint32_t zfail;
  uint32_t zpass;
};

COMPILE_ASSERT(sizeof(StencilOp) == 16, Sizeof_StencilOp_is_not_16);
COMPILE_ASSERT(offsetof(StencilOp, header) == 0,
               OffsetOf_StencilOp_header_not_0);
COMPILE_ASSERT(offsetof(StencilOp, fail) == 4, OffsetOf_StencilOp_fail_not_4);
COMPILE_ASSERT(offsetof(StencilOp, zfail) == 8, OffsetOf_StencilOp_zfail_not_8);
COMPILE_ASSERT(offsetof(StencilOp, zpass) == 12,
               OffsetOf_StencilOp_zpass_not_12);

struct StencilOpSeparate {
  typedef StencilOpSeparate ValueType;
  static const CommandId kCmdId = kStencilOpSeparate;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _face, GLenum _fail, GLenum _zfail, GLenum _zpass) {
    SetHeader();
    face = _face;
    fail = _fail;
    zfail = _zfail;
    zpass = _zpass;
  }

  void* Set(void* cmd,
            GLenum _face,
            GLenum _fail,
            GLenum _zfail,
            GLenum _zpass) {
    static_cast<ValueType*>(cmd)->Init(_face, _fail, _zfail, _zpass);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t face;
  uint32_t fail;
  uint32_t zfail;
  uint32_t zpass;
};

COMPILE_ASSERT(sizeof(StencilOpSeparate) == 20,
               Sizeof_StencilOpSeparate_is_not_20);
COMPILE_ASSERT(offsetof(StencilOpSeparate, header) == 0,
               OffsetOf_StencilOpSeparate_header_not_0);
COMPILE_ASSERT(offsetof(StencilOpSeparate, face) == 4,
               OffsetOf_StencilOpSeparate_face_not_4);
COMPILE_ASSERT(offsetof(StencilOpSeparate, fail) == 8,
               OffsetOf_StencilOpSeparate_fail_not_8);
COMPILE_ASSERT(offsetof(StencilOpSeparate, zfail) == 12,
               OffsetOf_StencilOpSeparate_zfail_not_12);
COMPILE_ASSERT(offsetof(StencilOpSeparate, zpass) == 16,
               OffsetOf_StencilOpSeparate_zpass_not_16);

struct TexImage2D {
  typedef TexImage2D ValueType;
  static const CommandId kCmdId = kTexImage2D;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLint _level,
            GLint _internalformat,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLenum _type,
            uint32_t _pixels_shm_id,
            uint32_t _pixels_shm_offset) {
    SetHeader();
    target = _target;
    level = _level;
    internalformat = _internalformat;
    width = _width;
    height = _height;
    format = _format;
    type = _type;
    pixels_shm_id = _pixels_shm_id;
    pixels_shm_offset = _pixels_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLint _level,
            GLint _internalformat,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLenum _type,
            uint32_t _pixels_shm_id,
            uint32_t _pixels_shm_offset) {
    static_cast<ValueType*>(cmd)->Init(_target,
                                       _level,
                                       _internalformat,
                                       _width,
                                       _height,
                                       _format,
                                       _type,
                                       _pixels_shm_id,
                                       _pixels_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t level;
  int32_t internalformat;
  int32_t width;
  int32_t height;
  uint32_t format;
  uint32_t type;
  uint32_t pixels_shm_id;
  uint32_t pixels_shm_offset;
  static const int32_t border = 0;
};

COMPILE_ASSERT(sizeof(TexImage2D) == 40, Sizeof_TexImage2D_is_not_40);
COMPILE_ASSERT(offsetof(TexImage2D, header) == 0,
               OffsetOf_TexImage2D_header_not_0);
COMPILE_ASSERT(offsetof(TexImage2D, target) == 4,
               OffsetOf_TexImage2D_target_not_4);
COMPILE_ASSERT(offsetof(TexImage2D, level) == 8,
               OffsetOf_TexImage2D_level_not_8);
COMPILE_ASSERT(offsetof(TexImage2D, internalformat) == 12,
               OffsetOf_TexImage2D_internalformat_not_12);
COMPILE_ASSERT(offsetof(TexImage2D, width) == 16,
               OffsetOf_TexImage2D_width_not_16);
COMPILE_ASSERT(offsetof(TexImage2D, height) == 20,
               OffsetOf_TexImage2D_height_not_20);
COMPILE_ASSERT(offsetof(TexImage2D, format) == 24,
               OffsetOf_TexImage2D_format_not_24);
COMPILE_ASSERT(offsetof(TexImage2D, type) == 28,
               OffsetOf_TexImage2D_type_not_28);
COMPILE_ASSERT(offsetof(TexImage2D, pixels_shm_id) == 32,
               OffsetOf_TexImage2D_pixels_shm_id_not_32);
COMPILE_ASSERT(offsetof(TexImage2D, pixels_shm_offset) == 36,
               OffsetOf_TexImage2D_pixels_shm_offset_not_36);

struct TexParameterf {
  typedef TexParameterf ValueType;
  static const CommandId kCmdId = kTexParameterf;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target, GLenum _pname, GLfloat _param) {
    SetHeader();
    target = _target;
    pname = _pname;
    param = _param;
  }

  void* Set(void* cmd, GLenum _target, GLenum _pname, GLfloat _param) {
    static_cast<ValueType*>(cmd)->Init(_target, _pname, _param);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t pname;
  float param;
};

COMPILE_ASSERT(sizeof(TexParameterf) == 16, Sizeof_TexParameterf_is_not_16);
COMPILE_ASSERT(offsetof(TexParameterf, header) == 0,
               OffsetOf_TexParameterf_header_not_0);
COMPILE_ASSERT(offsetof(TexParameterf, target) == 4,
               OffsetOf_TexParameterf_target_not_4);
COMPILE_ASSERT(offsetof(TexParameterf, pname) == 8,
               OffsetOf_TexParameterf_pname_not_8);
COMPILE_ASSERT(offsetof(TexParameterf, param) == 12,
               OffsetOf_TexParameterf_param_not_12);

struct TexParameterfvImmediate {
  typedef TexParameterfvImmediate ValueType;
  static const CommandId kCmdId = kTexParameterfvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize() {
    return static_cast<uint32_t>(sizeof(GLfloat) * 1);  // NOLINT
  }

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize());  // NOLINT
  }

  void SetHeader() { header.SetCmdByTotalSize<ValueType>(ComputeSize()); }

  void Init(GLenum _target, GLenum _pname, const GLfloat* _params) {
    SetHeader();
    target = _target;
    pname = _pname;
    memcpy(ImmediateDataAddress(this), _params, ComputeDataSize());
  }

  void* Set(void* cmd, GLenum _target, GLenum _pname, const GLfloat* _params) {
    static_cast<ValueType*>(cmd)->Init(_target, _pname, _params);
    const uint32_t size = ComputeSize();
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t pname;
};

COMPILE_ASSERT(sizeof(TexParameterfvImmediate) == 12,
               Sizeof_TexParameterfvImmediate_is_not_12);
COMPILE_ASSERT(offsetof(TexParameterfvImmediate, header) == 0,
               OffsetOf_TexParameterfvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(TexParameterfvImmediate, target) == 4,
               OffsetOf_TexParameterfvImmediate_target_not_4);
COMPILE_ASSERT(offsetof(TexParameterfvImmediate, pname) == 8,
               OffsetOf_TexParameterfvImmediate_pname_not_8);

struct TexParameteri {
  typedef TexParameteri ValueType;
  static const CommandId kCmdId = kTexParameteri;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target, GLenum _pname, GLint _param) {
    SetHeader();
    target = _target;
    pname = _pname;
    param = _param;
  }

  void* Set(void* cmd, GLenum _target, GLenum _pname, GLint _param) {
    static_cast<ValueType*>(cmd)->Init(_target, _pname, _param);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t pname;
  int32_t param;
};

COMPILE_ASSERT(sizeof(TexParameteri) == 16, Sizeof_TexParameteri_is_not_16);
COMPILE_ASSERT(offsetof(TexParameteri, header) == 0,
               OffsetOf_TexParameteri_header_not_0);
COMPILE_ASSERT(offsetof(TexParameteri, target) == 4,
               OffsetOf_TexParameteri_target_not_4);
COMPILE_ASSERT(offsetof(TexParameteri, pname) == 8,
               OffsetOf_TexParameteri_pname_not_8);
COMPILE_ASSERT(offsetof(TexParameteri, param) == 12,
               OffsetOf_TexParameteri_param_not_12);

struct TexParameterivImmediate {
  typedef TexParameterivImmediate ValueType;
  static const CommandId kCmdId = kTexParameterivImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize() {
    return static_cast<uint32_t>(sizeof(GLint) * 1);  // NOLINT
  }

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize());  // NOLINT
  }

  void SetHeader() { header.SetCmdByTotalSize<ValueType>(ComputeSize()); }

  void Init(GLenum _target, GLenum _pname, const GLint* _params) {
    SetHeader();
    target = _target;
    pname = _pname;
    memcpy(ImmediateDataAddress(this), _params, ComputeDataSize());
  }

  void* Set(void* cmd, GLenum _target, GLenum _pname, const GLint* _params) {
    static_cast<ValueType*>(cmd)->Init(_target, _pname, _params);
    const uint32_t size = ComputeSize();
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t pname;
};

COMPILE_ASSERT(sizeof(TexParameterivImmediate) == 12,
               Sizeof_TexParameterivImmediate_is_not_12);
COMPILE_ASSERT(offsetof(TexParameterivImmediate, header) == 0,
               OffsetOf_TexParameterivImmediate_header_not_0);
COMPILE_ASSERT(offsetof(TexParameterivImmediate, target) == 4,
               OffsetOf_TexParameterivImmediate_target_not_4);
COMPILE_ASSERT(offsetof(TexParameterivImmediate, pname) == 8,
               OffsetOf_TexParameterivImmediate_pname_not_8);

struct TexSubImage2D {
  typedef TexSubImage2D ValueType;
  static const CommandId kCmdId = kTexSubImage2D;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLint _level,
            GLint _xoffset,
            GLint _yoffset,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLenum _type,
            uint32_t _pixels_shm_id,
            uint32_t _pixels_shm_offset,
            GLboolean _internal) {
    SetHeader();
    target = _target;
    level = _level;
    xoffset = _xoffset;
    yoffset = _yoffset;
    width = _width;
    height = _height;
    format = _format;
    type = _type;
    pixels_shm_id = _pixels_shm_id;
    pixels_shm_offset = _pixels_shm_offset;
    internal = _internal;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLint _level,
            GLint _xoffset,
            GLint _yoffset,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLenum _type,
            uint32_t _pixels_shm_id,
            uint32_t _pixels_shm_offset,
            GLboolean _internal) {
    static_cast<ValueType*>(cmd)->Init(_target,
                                       _level,
                                       _xoffset,
                                       _yoffset,
                                       _width,
                                       _height,
                                       _format,
                                       _type,
                                       _pixels_shm_id,
                                       _pixels_shm_offset,
                                       _internal);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t level;
  int32_t xoffset;
  int32_t yoffset;
  int32_t width;
  int32_t height;
  uint32_t format;
  uint32_t type;
  uint32_t pixels_shm_id;
  uint32_t pixels_shm_offset;
  uint32_t internal;
};

COMPILE_ASSERT(sizeof(TexSubImage2D) == 48, Sizeof_TexSubImage2D_is_not_48);
COMPILE_ASSERT(offsetof(TexSubImage2D, header) == 0,
               OffsetOf_TexSubImage2D_header_not_0);
COMPILE_ASSERT(offsetof(TexSubImage2D, target) == 4,
               OffsetOf_TexSubImage2D_target_not_4);
COMPILE_ASSERT(offsetof(TexSubImage2D, level) == 8,
               OffsetOf_TexSubImage2D_level_not_8);
COMPILE_ASSERT(offsetof(TexSubImage2D, xoffset) == 12,
               OffsetOf_TexSubImage2D_xoffset_not_12);
COMPILE_ASSERT(offsetof(TexSubImage2D, yoffset) == 16,
               OffsetOf_TexSubImage2D_yoffset_not_16);
COMPILE_ASSERT(offsetof(TexSubImage2D, width) == 20,
               OffsetOf_TexSubImage2D_width_not_20);
COMPILE_ASSERT(offsetof(TexSubImage2D, height) == 24,
               OffsetOf_TexSubImage2D_height_not_24);
COMPILE_ASSERT(offsetof(TexSubImage2D, format) == 28,
               OffsetOf_TexSubImage2D_format_not_28);
COMPILE_ASSERT(offsetof(TexSubImage2D, type) == 32,
               OffsetOf_TexSubImage2D_type_not_32);
COMPILE_ASSERT(offsetof(TexSubImage2D, pixels_shm_id) == 36,
               OffsetOf_TexSubImage2D_pixels_shm_id_not_36);
COMPILE_ASSERT(offsetof(TexSubImage2D, pixels_shm_offset) == 40,
               OffsetOf_TexSubImage2D_pixels_shm_offset_not_40);
COMPILE_ASSERT(offsetof(TexSubImage2D, internal) == 44,
               OffsetOf_TexSubImage2D_internal_not_44);

struct Uniform1f {
  typedef Uniform1f ValueType;
  static const CommandId kCmdId = kUniform1f;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _location, GLfloat _x) {
    SetHeader();
    location = _location;
    x = _x;
  }

  void* Set(void* cmd, GLint _location, GLfloat _x) {
    static_cast<ValueType*>(cmd)->Init(_location, _x);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t location;
  float x;
};

COMPILE_ASSERT(sizeof(Uniform1f) == 12, Sizeof_Uniform1f_is_not_12);
COMPILE_ASSERT(offsetof(Uniform1f, header) == 0,
               OffsetOf_Uniform1f_header_not_0);
COMPILE_ASSERT(offsetof(Uniform1f, location) == 4,
               OffsetOf_Uniform1f_location_not_4);
COMPILE_ASSERT(offsetof(Uniform1f, x) == 8, OffsetOf_Uniform1f_x_not_8);

struct Uniform1fvImmediate {
  typedef Uniform1fvImmediate ValueType;
  static const CommandId kCmdId = kUniform1fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLfloat) * 1 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLfloat* _v) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _v, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLfloat* _v) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _v);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
};

COMPILE_ASSERT(sizeof(Uniform1fvImmediate) == 12,
               Sizeof_Uniform1fvImmediate_is_not_12);
COMPILE_ASSERT(offsetof(Uniform1fvImmediate, header) == 0,
               OffsetOf_Uniform1fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(Uniform1fvImmediate, location) == 4,
               OffsetOf_Uniform1fvImmediate_location_not_4);
COMPILE_ASSERT(offsetof(Uniform1fvImmediate, count) == 8,
               OffsetOf_Uniform1fvImmediate_count_not_8);

struct Uniform1i {
  typedef Uniform1i ValueType;
  static const CommandId kCmdId = kUniform1i;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _location, GLint _x) {
    SetHeader();
    location = _location;
    x = _x;
  }

  void* Set(void* cmd, GLint _location, GLint _x) {
    static_cast<ValueType*>(cmd)->Init(_location, _x);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t x;
};

COMPILE_ASSERT(sizeof(Uniform1i) == 12, Sizeof_Uniform1i_is_not_12);
COMPILE_ASSERT(offsetof(Uniform1i, header) == 0,
               OffsetOf_Uniform1i_header_not_0);
COMPILE_ASSERT(offsetof(Uniform1i, location) == 4,
               OffsetOf_Uniform1i_location_not_4);
COMPILE_ASSERT(offsetof(Uniform1i, x) == 8, OffsetOf_Uniform1i_x_not_8);

struct Uniform1ivImmediate {
  typedef Uniform1ivImmediate ValueType;
  static const CommandId kCmdId = kUniform1ivImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLint) * 1 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLint* _v) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _v, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLint* _v) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _v);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
};

COMPILE_ASSERT(sizeof(Uniform1ivImmediate) == 12,
               Sizeof_Uniform1ivImmediate_is_not_12);
COMPILE_ASSERT(offsetof(Uniform1ivImmediate, header) == 0,
               OffsetOf_Uniform1ivImmediate_header_not_0);
COMPILE_ASSERT(offsetof(Uniform1ivImmediate, location) == 4,
               OffsetOf_Uniform1ivImmediate_location_not_4);
COMPILE_ASSERT(offsetof(Uniform1ivImmediate, count) == 8,
               OffsetOf_Uniform1ivImmediate_count_not_8);

struct Uniform2f {
  typedef Uniform2f ValueType;
  static const CommandId kCmdId = kUniform2f;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _location, GLfloat _x, GLfloat _y) {
    SetHeader();
    location = _location;
    x = _x;
    y = _y;
  }

  void* Set(void* cmd, GLint _location, GLfloat _x, GLfloat _y) {
    static_cast<ValueType*>(cmd)->Init(_location, _x, _y);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t location;
  float x;
  float y;
};

COMPILE_ASSERT(sizeof(Uniform2f) == 16, Sizeof_Uniform2f_is_not_16);
COMPILE_ASSERT(offsetof(Uniform2f, header) == 0,
               OffsetOf_Uniform2f_header_not_0);
COMPILE_ASSERT(offsetof(Uniform2f, location) == 4,
               OffsetOf_Uniform2f_location_not_4);
COMPILE_ASSERT(offsetof(Uniform2f, x) == 8, OffsetOf_Uniform2f_x_not_8);
COMPILE_ASSERT(offsetof(Uniform2f, y) == 12, OffsetOf_Uniform2f_y_not_12);

struct Uniform2fvImmediate {
  typedef Uniform2fvImmediate ValueType;
  static const CommandId kCmdId = kUniform2fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLfloat) * 2 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLfloat* _v) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _v, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLfloat* _v) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _v);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
};

COMPILE_ASSERT(sizeof(Uniform2fvImmediate) == 12,
               Sizeof_Uniform2fvImmediate_is_not_12);
COMPILE_ASSERT(offsetof(Uniform2fvImmediate, header) == 0,
               OffsetOf_Uniform2fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(Uniform2fvImmediate, location) == 4,
               OffsetOf_Uniform2fvImmediate_location_not_4);
COMPILE_ASSERT(offsetof(Uniform2fvImmediate, count) == 8,
               OffsetOf_Uniform2fvImmediate_count_not_8);

struct Uniform2i {
  typedef Uniform2i ValueType;
  static const CommandId kCmdId = kUniform2i;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _location, GLint _x, GLint _y) {
    SetHeader();
    location = _location;
    x = _x;
    y = _y;
  }

  void* Set(void* cmd, GLint _location, GLint _x, GLint _y) {
    static_cast<ValueType*>(cmd)->Init(_location, _x, _y);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t x;
  int32_t y;
};

COMPILE_ASSERT(sizeof(Uniform2i) == 16, Sizeof_Uniform2i_is_not_16);
COMPILE_ASSERT(offsetof(Uniform2i, header) == 0,
               OffsetOf_Uniform2i_header_not_0);
COMPILE_ASSERT(offsetof(Uniform2i, location) == 4,
               OffsetOf_Uniform2i_location_not_4);
COMPILE_ASSERT(offsetof(Uniform2i, x) == 8, OffsetOf_Uniform2i_x_not_8);
COMPILE_ASSERT(offsetof(Uniform2i, y) == 12, OffsetOf_Uniform2i_y_not_12);

struct Uniform2ivImmediate {
  typedef Uniform2ivImmediate ValueType;
  static const CommandId kCmdId = kUniform2ivImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLint) * 2 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLint* _v) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _v, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLint* _v) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _v);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
};

COMPILE_ASSERT(sizeof(Uniform2ivImmediate) == 12,
               Sizeof_Uniform2ivImmediate_is_not_12);
COMPILE_ASSERT(offsetof(Uniform2ivImmediate, header) == 0,
               OffsetOf_Uniform2ivImmediate_header_not_0);
COMPILE_ASSERT(offsetof(Uniform2ivImmediate, location) == 4,
               OffsetOf_Uniform2ivImmediate_location_not_4);
COMPILE_ASSERT(offsetof(Uniform2ivImmediate, count) == 8,
               OffsetOf_Uniform2ivImmediate_count_not_8);

struct Uniform3f {
  typedef Uniform3f ValueType;
  static const CommandId kCmdId = kUniform3f;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _location, GLfloat _x, GLfloat _y, GLfloat _z) {
    SetHeader();
    location = _location;
    x = _x;
    y = _y;
    z = _z;
  }

  void* Set(void* cmd, GLint _location, GLfloat _x, GLfloat _y, GLfloat _z) {
    static_cast<ValueType*>(cmd)->Init(_location, _x, _y, _z);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t location;
  float x;
  float y;
  float z;
};

COMPILE_ASSERT(sizeof(Uniform3f) == 20, Sizeof_Uniform3f_is_not_20);
COMPILE_ASSERT(offsetof(Uniform3f, header) == 0,
               OffsetOf_Uniform3f_header_not_0);
COMPILE_ASSERT(offsetof(Uniform3f, location) == 4,
               OffsetOf_Uniform3f_location_not_4);
COMPILE_ASSERT(offsetof(Uniform3f, x) == 8, OffsetOf_Uniform3f_x_not_8);
COMPILE_ASSERT(offsetof(Uniform3f, y) == 12, OffsetOf_Uniform3f_y_not_12);
COMPILE_ASSERT(offsetof(Uniform3f, z) == 16, OffsetOf_Uniform3f_z_not_16);

struct Uniform3fvImmediate {
  typedef Uniform3fvImmediate ValueType;
  static const CommandId kCmdId = kUniform3fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLfloat) * 3 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLfloat* _v) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _v, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLfloat* _v) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _v);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
};

COMPILE_ASSERT(sizeof(Uniform3fvImmediate) == 12,
               Sizeof_Uniform3fvImmediate_is_not_12);
COMPILE_ASSERT(offsetof(Uniform3fvImmediate, header) == 0,
               OffsetOf_Uniform3fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(Uniform3fvImmediate, location) == 4,
               OffsetOf_Uniform3fvImmediate_location_not_4);
COMPILE_ASSERT(offsetof(Uniform3fvImmediate, count) == 8,
               OffsetOf_Uniform3fvImmediate_count_not_8);

struct Uniform3i {
  typedef Uniform3i ValueType;
  static const CommandId kCmdId = kUniform3i;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _location, GLint _x, GLint _y, GLint _z) {
    SetHeader();
    location = _location;
    x = _x;
    y = _y;
    z = _z;
  }

  void* Set(void* cmd, GLint _location, GLint _x, GLint _y, GLint _z) {
    static_cast<ValueType*>(cmd)->Init(_location, _x, _y, _z);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t x;
  int32_t y;
  int32_t z;
};

COMPILE_ASSERT(sizeof(Uniform3i) == 20, Sizeof_Uniform3i_is_not_20);
COMPILE_ASSERT(offsetof(Uniform3i, header) == 0,
               OffsetOf_Uniform3i_header_not_0);
COMPILE_ASSERT(offsetof(Uniform3i, location) == 4,
               OffsetOf_Uniform3i_location_not_4);
COMPILE_ASSERT(offsetof(Uniform3i, x) == 8, OffsetOf_Uniform3i_x_not_8);
COMPILE_ASSERT(offsetof(Uniform3i, y) == 12, OffsetOf_Uniform3i_y_not_12);
COMPILE_ASSERT(offsetof(Uniform3i, z) == 16, OffsetOf_Uniform3i_z_not_16);

struct Uniform3ivImmediate {
  typedef Uniform3ivImmediate ValueType;
  static const CommandId kCmdId = kUniform3ivImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLint) * 3 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLint* _v) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _v, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLint* _v) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _v);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
};

COMPILE_ASSERT(sizeof(Uniform3ivImmediate) == 12,
               Sizeof_Uniform3ivImmediate_is_not_12);
COMPILE_ASSERT(offsetof(Uniform3ivImmediate, header) == 0,
               OffsetOf_Uniform3ivImmediate_header_not_0);
COMPILE_ASSERT(offsetof(Uniform3ivImmediate, location) == 4,
               OffsetOf_Uniform3ivImmediate_location_not_4);
COMPILE_ASSERT(offsetof(Uniform3ivImmediate, count) == 8,
               OffsetOf_Uniform3ivImmediate_count_not_8);

struct Uniform4f {
  typedef Uniform4f ValueType;
  static const CommandId kCmdId = kUniform4f;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _location, GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w) {
    SetHeader();
    location = _location;
    x = _x;
    y = _y;
    z = _z;
    w = _w;
  }

  void* Set(void* cmd,
            GLint _location,
            GLfloat _x,
            GLfloat _y,
            GLfloat _z,
            GLfloat _w) {
    static_cast<ValueType*>(cmd)->Init(_location, _x, _y, _z, _w);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t location;
  float x;
  float y;
  float z;
  float w;
};

COMPILE_ASSERT(sizeof(Uniform4f) == 24, Sizeof_Uniform4f_is_not_24);
COMPILE_ASSERT(offsetof(Uniform4f, header) == 0,
               OffsetOf_Uniform4f_header_not_0);
COMPILE_ASSERT(offsetof(Uniform4f, location) == 4,
               OffsetOf_Uniform4f_location_not_4);
COMPILE_ASSERT(offsetof(Uniform4f, x) == 8, OffsetOf_Uniform4f_x_not_8);
COMPILE_ASSERT(offsetof(Uniform4f, y) == 12, OffsetOf_Uniform4f_y_not_12);
COMPILE_ASSERT(offsetof(Uniform4f, z) == 16, OffsetOf_Uniform4f_z_not_16);
COMPILE_ASSERT(offsetof(Uniform4f, w) == 20, OffsetOf_Uniform4f_w_not_20);

struct Uniform4fvImmediate {
  typedef Uniform4fvImmediate ValueType;
  static const CommandId kCmdId = kUniform4fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLfloat) * 4 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLfloat* _v) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _v, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLfloat* _v) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _v);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
};

COMPILE_ASSERT(sizeof(Uniform4fvImmediate) == 12,
               Sizeof_Uniform4fvImmediate_is_not_12);
COMPILE_ASSERT(offsetof(Uniform4fvImmediate, header) == 0,
               OffsetOf_Uniform4fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(Uniform4fvImmediate, location) == 4,
               OffsetOf_Uniform4fvImmediate_location_not_4);
COMPILE_ASSERT(offsetof(Uniform4fvImmediate, count) == 8,
               OffsetOf_Uniform4fvImmediate_count_not_8);

struct Uniform4i {
  typedef Uniform4i ValueType;
  static const CommandId kCmdId = kUniform4i;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _location, GLint _x, GLint _y, GLint _z, GLint _w) {
    SetHeader();
    location = _location;
    x = _x;
    y = _y;
    z = _z;
    w = _w;
  }

  void* Set(void* cmd,
            GLint _location,
            GLint _x,
            GLint _y,
            GLint _z,
            GLint _w) {
    static_cast<ValueType*>(cmd)->Init(_location, _x, _y, _z, _w);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t w;
};

COMPILE_ASSERT(sizeof(Uniform4i) == 24, Sizeof_Uniform4i_is_not_24);
COMPILE_ASSERT(offsetof(Uniform4i, header) == 0,
               OffsetOf_Uniform4i_header_not_0);
COMPILE_ASSERT(offsetof(Uniform4i, location) == 4,
               OffsetOf_Uniform4i_location_not_4);
COMPILE_ASSERT(offsetof(Uniform4i, x) == 8, OffsetOf_Uniform4i_x_not_8);
COMPILE_ASSERT(offsetof(Uniform4i, y) == 12, OffsetOf_Uniform4i_y_not_12);
COMPILE_ASSERT(offsetof(Uniform4i, z) == 16, OffsetOf_Uniform4i_z_not_16);
COMPILE_ASSERT(offsetof(Uniform4i, w) == 20, OffsetOf_Uniform4i_w_not_20);

struct Uniform4ivImmediate {
  typedef Uniform4ivImmediate ValueType;
  static const CommandId kCmdId = kUniform4ivImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLint) * 4 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLint* _v) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _v, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLint* _v) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _v);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
};

COMPILE_ASSERT(sizeof(Uniform4ivImmediate) == 12,
               Sizeof_Uniform4ivImmediate_is_not_12);
COMPILE_ASSERT(offsetof(Uniform4ivImmediate, header) == 0,
               OffsetOf_Uniform4ivImmediate_header_not_0);
COMPILE_ASSERT(offsetof(Uniform4ivImmediate, location) == 4,
               OffsetOf_Uniform4ivImmediate_location_not_4);
COMPILE_ASSERT(offsetof(Uniform4ivImmediate, count) == 8,
               OffsetOf_Uniform4ivImmediate_count_not_8);

struct UniformMatrix2fvImmediate {
  typedef UniformMatrix2fvImmediate ValueType;
  static const CommandId kCmdId = kUniformMatrix2fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLfloat) * 4 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLfloat* _value) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _value, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLfloat* _value) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _value);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
  static const uint32_t transpose = false;
};

COMPILE_ASSERT(sizeof(UniformMatrix2fvImmediate) == 12,
               Sizeof_UniformMatrix2fvImmediate_is_not_12);
COMPILE_ASSERT(offsetof(UniformMatrix2fvImmediate, header) == 0,
               OffsetOf_UniformMatrix2fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(UniformMatrix2fvImmediate, location) == 4,
               OffsetOf_UniformMatrix2fvImmediate_location_not_4);
COMPILE_ASSERT(offsetof(UniformMatrix2fvImmediate, count) == 8,
               OffsetOf_UniformMatrix2fvImmediate_count_not_8);

struct UniformMatrix3fvImmediate {
  typedef UniformMatrix3fvImmediate ValueType;
  static const CommandId kCmdId = kUniformMatrix3fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLfloat) * 9 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLfloat* _value) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _value, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLfloat* _value) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _value);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
  static const uint32_t transpose = false;
};

COMPILE_ASSERT(sizeof(UniformMatrix3fvImmediate) == 12,
               Sizeof_UniformMatrix3fvImmediate_is_not_12);
COMPILE_ASSERT(offsetof(UniformMatrix3fvImmediate, header) == 0,
               OffsetOf_UniformMatrix3fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(UniformMatrix3fvImmediate, location) == 4,
               OffsetOf_UniformMatrix3fvImmediate_location_not_4);
COMPILE_ASSERT(offsetof(UniformMatrix3fvImmediate, count) == 8,
               OffsetOf_UniformMatrix3fvImmediate_count_not_8);

struct UniformMatrix4fvImmediate {
  typedef UniformMatrix4fvImmediate ValueType;
  static const CommandId kCmdId = kUniformMatrix4fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLfloat) * 16 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLint _location, GLsizei _count, const GLfloat* _value) {
    SetHeader(_count);
    location = _location;
    count = _count;
    memcpy(ImmediateDataAddress(this), _value, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLint _location, GLsizei _count, const GLfloat* _value) {
    static_cast<ValueType*>(cmd)->Init(_location, _count, _value);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t location;
  int32_t count;
  static const uint32_t transpose = false;
};

COMPILE_ASSERT(sizeof(UniformMatrix4fvImmediate) == 12,
               Sizeof_UniformMatrix4fvImmediate_is_not_12);
COMPILE_ASSERT(offsetof(UniformMatrix4fvImmediate, header) == 0,
               OffsetOf_UniformMatrix4fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(UniformMatrix4fvImmediate, location) == 4,
               OffsetOf_UniformMatrix4fvImmediate_location_not_4);
COMPILE_ASSERT(offsetof(UniformMatrix4fvImmediate, count) == 8,
               OffsetOf_UniformMatrix4fvImmediate_count_not_8);

struct UseProgram {
  typedef UseProgram ValueType;
  static const CommandId kCmdId = kUseProgram;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program) {
    SetHeader();
    program = _program;
  }

  void* Set(void* cmd, GLuint _program) {
    static_cast<ValueType*>(cmd)->Init(_program);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
};

COMPILE_ASSERT(sizeof(UseProgram) == 8, Sizeof_UseProgram_is_not_8);
COMPILE_ASSERT(offsetof(UseProgram, header) == 0,
               OffsetOf_UseProgram_header_not_0);
COMPILE_ASSERT(offsetof(UseProgram, program) == 4,
               OffsetOf_UseProgram_program_not_4);

struct ValidateProgram {
  typedef ValidateProgram ValueType;
  static const CommandId kCmdId = kValidateProgram;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program) {
    SetHeader();
    program = _program;
  }

  void* Set(void* cmd, GLuint _program) {
    static_cast<ValueType*>(cmd)->Init(_program);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
};

COMPILE_ASSERT(sizeof(ValidateProgram) == 8, Sizeof_ValidateProgram_is_not_8);
COMPILE_ASSERT(offsetof(ValidateProgram, header) == 0,
               OffsetOf_ValidateProgram_header_not_0);
COMPILE_ASSERT(offsetof(ValidateProgram, program) == 4,
               OffsetOf_ValidateProgram_program_not_4);

struct VertexAttrib1f {
  typedef VertexAttrib1f ValueType;
  static const CommandId kCmdId = kVertexAttrib1f;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _indx, GLfloat _x) {
    SetHeader();
    indx = _indx;
    x = _x;
  }

  void* Set(void* cmd, GLuint _indx, GLfloat _x) {
    static_cast<ValueType*>(cmd)->Init(_indx, _x);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t indx;
  float x;
};

COMPILE_ASSERT(sizeof(VertexAttrib1f) == 12, Sizeof_VertexAttrib1f_is_not_12);
COMPILE_ASSERT(offsetof(VertexAttrib1f, header) == 0,
               OffsetOf_VertexAttrib1f_header_not_0);
COMPILE_ASSERT(offsetof(VertexAttrib1f, indx) == 4,
               OffsetOf_VertexAttrib1f_indx_not_4);
COMPILE_ASSERT(offsetof(VertexAttrib1f, x) == 8,
               OffsetOf_VertexAttrib1f_x_not_8);

struct VertexAttrib1fvImmediate {
  typedef VertexAttrib1fvImmediate ValueType;
  static const CommandId kCmdId = kVertexAttrib1fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize() {
    return static_cast<uint32_t>(sizeof(GLfloat) * 1);  // NOLINT
  }

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize());  // NOLINT
  }

  void SetHeader() { header.SetCmdByTotalSize<ValueType>(ComputeSize()); }

  void Init(GLuint _indx, const GLfloat* _values) {
    SetHeader();
    indx = _indx;
    memcpy(ImmediateDataAddress(this), _values, ComputeDataSize());
  }

  void* Set(void* cmd, GLuint _indx, const GLfloat* _values) {
    static_cast<ValueType*>(cmd)->Init(_indx, _values);
    const uint32_t size = ComputeSize();
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t indx;
};

COMPILE_ASSERT(sizeof(VertexAttrib1fvImmediate) == 8,
               Sizeof_VertexAttrib1fvImmediate_is_not_8);
COMPILE_ASSERT(offsetof(VertexAttrib1fvImmediate, header) == 0,
               OffsetOf_VertexAttrib1fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(VertexAttrib1fvImmediate, indx) == 4,
               OffsetOf_VertexAttrib1fvImmediate_indx_not_4);

struct VertexAttrib2f {
  typedef VertexAttrib2f ValueType;
  static const CommandId kCmdId = kVertexAttrib2f;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _indx, GLfloat _x, GLfloat _y) {
    SetHeader();
    indx = _indx;
    x = _x;
    y = _y;
  }

  void* Set(void* cmd, GLuint _indx, GLfloat _x, GLfloat _y) {
    static_cast<ValueType*>(cmd)->Init(_indx, _x, _y);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t indx;
  float x;
  float y;
};

COMPILE_ASSERT(sizeof(VertexAttrib2f) == 16, Sizeof_VertexAttrib2f_is_not_16);
COMPILE_ASSERT(offsetof(VertexAttrib2f, header) == 0,
               OffsetOf_VertexAttrib2f_header_not_0);
COMPILE_ASSERT(offsetof(VertexAttrib2f, indx) == 4,
               OffsetOf_VertexAttrib2f_indx_not_4);
COMPILE_ASSERT(offsetof(VertexAttrib2f, x) == 8,
               OffsetOf_VertexAttrib2f_x_not_8);
COMPILE_ASSERT(offsetof(VertexAttrib2f, y) == 12,
               OffsetOf_VertexAttrib2f_y_not_12);

struct VertexAttrib2fvImmediate {
  typedef VertexAttrib2fvImmediate ValueType;
  static const CommandId kCmdId = kVertexAttrib2fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize() {
    return static_cast<uint32_t>(sizeof(GLfloat) * 2);  // NOLINT
  }

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize());  // NOLINT
  }

  void SetHeader() { header.SetCmdByTotalSize<ValueType>(ComputeSize()); }

  void Init(GLuint _indx, const GLfloat* _values) {
    SetHeader();
    indx = _indx;
    memcpy(ImmediateDataAddress(this), _values, ComputeDataSize());
  }

  void* Set(void* cmd, GLuint _indx, const GLfloat* _values) {
    static_cast<ValueType*>(cmd)->Init(_indx, _values);
    const uint32_t size = ComputeSize();
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t indx;
};

COMPILE_ASSERT(sizeof(VertexAttrib2fvImmediate) == 8,
               Sizeof_VertexAttrib2fvImmediate_is_not_8);
COMPILE_ASSERT(offsetof(VertexAttrib2fvImmediate, header) == 0,
               OffsetOf_VertexAttrib2fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(VertexAttrib2fvImmediate, indx) == 4,
               OffsetOf_VertexAttrib2fvImmediate_indx_not_4);

struct VertexAttrib3f {
  typedef VertexAttrib3f ValueType;
  static const CommandId kCmdId = kVertexAttrib3f;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _indx, GLfloat _x, GLfloat _y, GLfloat _z) {
    SetHeader();
    indx = _indx;
    x = _x;
    y = _y;
    z = _z;
  }

  void* Set(void* cmd, GLuint _indx, GLfloat _x, GLfloat _y, GLfloat _z) {
    static_cast<ValueType*>(cmd)->Init(_indx, _x, _y, _z);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t indx;
  float x;
  float y;
  float z;
};

COMPILE_ASSERT(sizeof(VertexAttrib3f) == 20, Sizeof_VertexAttrib3f_is_not_20);
COMPILE_ASSERT(offsetof(VertexAttrib3f, header) == 0,
               OffsetOf_VertexAttrib3f_header_not_0);
COMPILE_ASSERT(offsetof(VertexAttrib3f, indx) == 4,
               OffsetOf_VertexAttrib3f_indx_not_4);
COMPILE_ASSERT(offsetof(VertexAttrib3f, x) == 8,
               OffsetOf_VertexAttrib3f_x_not_8);
COMPILE_ASSERT(offsetof(VertexAttrib3f, y) == 12,
               OffsetOf_VertexAttrib3f_y_not_12);
COMPILE_ASSERT(offsetof(VertexAttrib3f, z) == 16,
               OffsetOf_VertexAttrib3f_z_not_16);

struct VertexAttrib3fvImmediate {
  typedef VertexAttrib3fvImmediate ValueType;
  static const CommandId kCmdId = kVertexAttrib3fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize() {
    return static_cast<uint32_t>(sizeof(GLfloat) * 3);  // NOLINT
  }

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize());  // NOLINT
  }

  void SetHeader() { header.SetCmdByTotalSize<ValueType>(ComputeSize()); }

  void Init(GLuint _indx, const GLfloat* _values) {
    SetHeader();
    indx = _indx;
    memcpy(ImmediateDataAddress(this), _values, ComputeDataSize());
  }

  void* Set(void* cmd, GLuint _indx, const GLfloat* _values) {
    static_cast<ValueType*>(cmd)->Init(_indx, _values);
    const uint32_t size = ComputeSize();
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t indx;
};

COMPILE_ASSERT(sizeof(VertexAttrib3fvImmediate) == 8,
               Sizeof_VertexAttrib3fvImmediate_is_not_8);
COMPILE_ASSERT(offsetof(VertexAttrib3fvImmediate, header) == 0,
               OffsetOf_VertexAttrib3fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(VertexAttrib3fvImmediate, indx) == 4,
               OffsetOf_VertexAttrib3fvImmediate_indx_not_4);

struct VertexAttrib4f {
  typedef VertexAttrib4f ValueType;
  static const CommandId kCmdId = kVertexAttrib4f;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _indx, GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w) {
    SetHeader();
    indx = _indx;
    x = _x;
    y = _y;
    z = _z;
    w = _w;
  }

  void* Set(void* cmd,
            GLuint _indx,
            GLfloat _x,
            GLfloat _y,
            GLfloat _z,
            GLfloat _w) {
    static_cast<ValueType*>(cmd)->Init(_indx, _x, _y, _z, _w);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t indx;
  float x;
  float y;
  float z;
  float w;
};

COMPILE_ASSERT(sizeof(VertexAttrib4f) == 24, Sizeof_VertexAttrib4f_is_not_24);
COMPILE_ASSERT(offsetof(VertexAttrib4f, header) == 0,
               OffsetOf_VertexAttrib4f_header_not_0);
COMPILE_ASSERT(offsetof(VertexAttrib4f, indx) == 4,
               OffsetOf_VertexAttrib4f_indx_not_4);
COMPILE_ASSERT(offsetof(VertexAttrib4f, x) == 8,
               OffsetOf_VertexAttrib4f_x_not_8);
COMPILE_ASSERT(offsetof(VertexAttrib4f, y) == 12,
               OffsetOf_VertexAttrib4f_y_not_12);
COMPILE_ASSERT(offsetof(VertexAttrib4f, z) == 16,
               OffsetOf_VertexAttrib4f_z_not_16);
COMPILE_ASSERT(offsetof(VertexAttrib4f, w) == 20,
               OffsetOf_VertexAttrib4f_w_not_20);

struct VertexAttrib4fvImmediate {
  typedef VertexAttrib4fvImmediate ValueType;
  static const CommandId kCmdId = kVertexAttrib4fvImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize() {
    return static_cast<uint32_t>(sizeof(GLfloat) * 4);  // NOLINT
  }

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize());  // NOLINT
  }

  void SetHeader() { header.SetCmdByTotalSize<ValueType>(ComputeSize()); }

  void Init(GLuint _indx, const GLfloat* _values) {
    SetHeader();
    indx = _indx;
    memcpy(ImmediateDataAddress(this), _values, ComputeDataSize());
  }

  void* Set(void* cmd, GLuint _indx, const GLfloat* _values) {
    static_cast<ValueType*>(cmd)->Init(_indx, _values);
    const uint32_t size = ComputeSize();
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t indx;
};

COMPILE_ASSERT(sizeof(VertexAttrib4fvImmediate) == 8,
               Sizeof_VertexAttrib4fvImmediate_is_not_8);
COMPILE_ASSERT(offsetof(VertexAttrib4fvImmediate, header) == 0,
               OffsetOf_VertexAttrib4fvImmediate_header_not_0);
COMPILE_ASSERT(offsetof(VertexAttrib4fvImmediate, indx) == 4,
               OffsetOf_VertexAttrib4fvImmediate_indx_not_4);

struct VertexAttribPointer {
  typedef VertexAttribPointer ValueType;
  static const CommandId kCmdId = kVertexAttribPointer;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _indx,
            GLint _size,
            GLenum _type,
            GLboolean _normalized,
            GLsizei _stride,
            GLuint _offset) {
    SetHeader();
    indx = _indx;
    size = _size;
    type = _type;
    normalized = _normalized;
    stride = _stride;
    offset = _offset;
  }

  void* Set(void* cmd,
            GLuint _indx,
            GLint _size,
            GLenum _type,
            GLboolean _normalized,
            GLsizei _stride,
            GLuint _offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_indx, _size, _type, _normalized, _stride, _offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t indx;
  int32_t size;
  uint32_t type;
  uint32_t normalized;
  int32_t stride;
  uint32_t offset;
};

COMPILE_ASSERT(sizeof(VertexAttribPointer) == 28,
               Sizeof_VertexAttribPointer_is_not_28);
COMPILE_ASSERT(offsetof(VertexAttribPointer, header) == 0,
               OffsetOf_VertexAttribPointer_header_not_0);
COMPILE_ASSERT(offsetof(VertexAttribPointer, indx) == 4,
               OffsetOf_VertexAttribPointer_indx_not_4);
COMPILE_ASSERT(offsetof(VertexAttribPointer, size) == 8,
               OffsetOf_VertexAttribPointer_size_not_8);
COMPILE_ASSERT(offsetof(VertexAttribPointer, type) == 12,
               OffsetOf_VertexAttribPointer_type_not_12);
COMPILE_ASSERT(offsetof(VertexAttribPointer, normalized) == 16,
               OffsetOf_VertexAttribPointer_normalized_not_16);
COMPILE_ASSERT(offsetof(VertexAttribPointer, stride) == 20,
               OffsetOf_VertexAttribPointer_stride_not_20);
COMPILE_ASSERT(offsetof(VertexAttribPointer, offset) == 24,
               OffsetOf_VertexAttribPointer_offset_not_24);

struct Viewport {
  typedef Viewport ValueType;
  static const CommandId kCmdId = kViewport;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _x, GLint _y, GLsizei _width, GLsizei _height) {
    SetHeader();
    x = _x;
    y = _y;
    width = _width;
    height = _height;
  }

  void* Set(void* cmd, GLint _x, GLint _y, GLsizei _width, GLsizei _height) {
    static_cast<ValueType*>(cmd)->Init(_x, _y, _width, _height);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
};

COMPILE_ASSERT(sizeof(Viewport) == 20, Sizeof_Viewport_is_not_20);
COMPILE_ASSERT(offsetof(Viewport, header) == 0, OffsetOf_Viewport_header_not_0);
COMPILE_ASSERT(offsetof(Viewport, x) == 4, OffsetOf_Viewport_x_not_4);
COMPILE_ASSERT(offsetof(Viewport, y) == 8, OffsetOf_Viewport_y_not_8);
COMPILE_ASSERT(offsetof(Viewport, width) == 12, OffsetOf_Viewport_width_not_12);
COMPILE_ASSERT(offsetof(Viewport, height) == 16,
               OffsetOf_Viewport_height_not_16);

struct BlitFramebufferCHROMIUM {
  typedef BlitFramebufferCHROMIUM ValueType;
  static const CommandId kCmdId = kBlitFramebufferCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _srcX0,
            GLint _srcY0,
            GLint _srcX1,
            GLint _srcY1,
            GLint _dstX0,
            GLint _dstY0,
            GLint _dstX1,
            GLint _dstY1,
            GLbitfield _mask,
            GLenum _filter) {
    SetHeader();
    srcX0 = _srcX0;
    srcY0 = _srcY0;
    srcX1 = _srcX1;
    srcY1 = _srcY1;
    dstX0 = _dstX0;
    dstY0 = _dstY0;
    dstX1 = _dstX1;
    dstY1 = _dstY1;
    mask = _mask;
    filter = _filter;
  }

  void* Set(void* cmd,
            GLint _srcX0,
            GLint _srcY0,
            GLint _srcX1,
            GLint _srcY1,
            GLint _dstX0,
            GLint _dstY0,
            GLint _dstX1,
            GLint _dstY1,
            GLbitfield _mask,
            GLenum _filter) {
    static_cast<ValueType*>(cmd)->Init(_srcX0,
                                       _srcY0,
                                       _srcX1,
                                       _srcY1,
                                       _dstX0,
                                       _dstY0,
                                       _dstX1,
                                       _dstY1,
                                       _mask,
                                       _filter);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t srcX0;
  int32_t srcY0;
  int32_t srcX1;
  int32_t srcY1;
  int32_t dstX0;
  int32_t dstY0;
  int32_t dstX1;
  int32_t dstY1;
  uint32_t mask;
  uint32_t filter;
};

COMPILE_ASSERT(sizeof(BlitFramebufferCHROMIUM) == 44,
               Sizeof_BlitFramebufferCHROMIUM_is_not_44);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, header) == 0,
               OffsetOf_BlitFramebufferCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, srcX0) == 4,
               OffsetOf_BlitFramebufferCHROMIUM_srcX0_not_4);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, srcY0) == 8,
               OffsetOf_BlitFramebufferCHROMIUM_srcY0_not_8);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, srcX1) == 12,
               OffsetOf_BlitFramebufferCHROMIUM_srcX1_not_12);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, srcY1) == 16,
               OffsetOf_BlitFramebufferCHROMIUM_srcY1_not_16);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, dstX0) == 20,
               OffsetOf_BlitFramebufferCHROMIUM_dstX0_not_20);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, dstY0) == 24,
               OffsetOf_BlitFramebufferCHROMIUM_dstY0_not_24);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, dstX1) == 28,
               OffsetOf_BlitFramebufferCHROMIUM_dstX1_not_28);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, dstY1) == 32,
               OffsetOf_BlitFramebufferCHROMIUM_dstY1_not_32);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, mask) == 36,
               OffsetOf_BlitFramebufferCHROMIUM_mask_not_36);
COMPILE_ASSERT(offsetof(BlitFramebufferCHROMIUM, filter) == 40,
               OffsetOf_BlitFramebufferCHROMIUM_filter_not_40);

// GL_CHROMIUM_framebuffer_multisample
struct RenderbufferStorageMultisampleCHROMIUM {
  typedef RenderbufferStorageMultisampleCHROMIUM ValueType;
  static const CommandId kCmdId = kRenderbufferStorageMultisampleCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLsizei _samples,
            GLenum _internalformat,
            GLsizei _width,
            GLsizei _height) {
    SetHeader();
    target = _target;
    samples = _samples;
    internalformat = _internalformat;
    width = _width;
    height = _height;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLsizei _samples,
            GLenum _internalformat,
            GLsizei _width,
            GLsizei _height) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _samples, _internalformat, _width, _height);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t samples;
  uint32_t internalformat;
  int32_t width;
  int32_t height;
};

COMPILE_ASSERT(sizeof(RenderbufferStorageMultisampleCHROMIUM) == 24,
               Sizeof_RenderbufferStorageMultisampleCHROMIUM_is_not_24);
COMPILE_ASSERT(offsetof(RenderbufferStorageMultisampleCHROMIUM, header) == 0,
               OffsetOf_RenderbufferStorageMultisampleCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(RenderbufferStorageMultisampleCHROMIUM, target) == 4,
               OffsetOf_RenderbufferStorageMultisampleCHROMIUM_target_not_4);
COMPILE_ASSERT(offsetof(RenderbufferStorageMultisampleCHROMIUM, samples) == 8,
               OffsetOf_RenderbufferStorageMultisampleCHROMIUM_samples_not_8);
COMPILE_ASSERT(
    offsetof(RenderbufferStorageMultisampleCHROMIUM, internalformat) == 12,
    OffsetOf_RenderbufferStorageMultisampleCHROMIUM_internalformat_not_12);
COMPILE_ASSERT(offsetof(RenderbufferStorageMultisampleCHROMIUM, width) == 16,
               OffsetOf_RenderbufferStorageMultisampleCHROMIUM_width_not_16);
COMPILE_ASSERT(offsetof(RenderbufferStorageMultisampleCHROMIUM, height) == 20,
               OffsetOf_RenderbufferStorageMultisampleCHROMIUM_height_not_20);

// GL_EXT_multisampled_render_to_texture
struct RenderbufferStorageMultisampleEXT {
  typedef RenderbufferStorageMultisampleEXT ValueType;
  static const CommandId kCmdId = kRenderbufferStorageMultisampleEXT;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLsizei _samples,
            GLenum _internalformat,
            GLsizei _width,
            GLsizei _height) {
    SetHeader();
    target = _target;
    samples = _samples;
    internalformat = _internalformat;
    width = _width;
    height = _height;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLsizei _samples,
            GLenum _internalformat,
            GLsizei _width,
            GLsizei _height) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _samples, _internalformat, _width, _height);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t samples;
  uint32_t internalformat;
  int32_t width;
  int32_t height;
};

COMPILE_ASSERT(sizeof(RenderbufferStorageMultisampleEXT) == 24,
               Sizeof_RenderbufferStorageMultisampleEXT_is_not_24);
COMPILE_ASSERT(offsetof(RenderbufferStorageMultisampleEXT, header) == 0,
               OffsetOf_RenderbufferStorageMultisampleEXT_header_not_0);
COMPILE_ASSERT(offsetof(RenderbufferStorageMultisampleEXT, target) == 4,
               OffsetOf_RenderbufferStorageMultisampleEXT_target_not_4);
COMPILE_ASSERT(offsetof(RenderbufferStorageMultisampleEXT, samples) == 8,
               OffsetOf_RenderbufferStorageMultisampleEXT_samples_not_8);
COMPILE_ASSERT(
    offsetof(RenderbufferStorageMultisampleEXT, internalformat) == 12,
    OffsetOf_RenderbufferStorageMultisampleEXT_internalformat_not_12);
COMPILE_ASSERT(offsetof(RenderbufferStorageMultisampleEXT, width) == 16,
               OffsetOf_RenderbufferStorageMultisampleEXT_width_not_16);
COMPILE_ASSERT(offsetof(RenderbufferStorageMultisampleEXT, height) == 20,
               OffsetOf_RenderbufferStorageMultisampleEXT_height_not_20);

struct FramebufferTexture2DMultisampleEXT {
  typedef FramebufferTexture2DMultisampleEXT ValueType;
  static const CommandId kCmdId = kFramebufferTexture2DMultisampleEXT;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLenum _attachment,
            GLenum _textarget,
            GLuint _texture,
            GLsizei _samples) {
    SetHeader();
    target = _target;
    attachment = _attachment;
    textarget = _textarget;
    texture = _texture;
    samples = _samples;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLenum _attachment,
            GLenum _textarget,
            GLuint _texture,
            GLsizei _samples) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _attachment, _textarget, _texture, _samples);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t attachment;
  uint32_t textarget;
  uint32_t texture;
  int32_t samples;
  static const int32_t level = 0;
};

COMPILE_ASSERT(sizeof(FramebufferTexture2DMultisampleEXT) == 24,
               Sizeof_FramebufferTexture2DMultisampleEXT_is_not_24);
COMPILE_ASSERT(offsetof(FramebufferTexture2DMultisampleEXT, header) == 0,
               OffsetOf_FramebufferTexture2DMultisampleEXT_header_not_0);
COMPILE_ASSERT(offsetof(FramebufferTexture2DMultisampleEXT, target) == 4,
               OffsetOf_FramebufferTexture2DMultisampleEXT_target_not_4);
COMPILE_ASSERT(offsetof(FramebufferTexture2DMultisampleEXT, attachment) == 8,
               OffsetOf_FramebufferTexture2DMultisampleEXT_attachment_not_8);
COMPILE_ASSERT(offsetof(FramebufferTexture2DMultisampleEXT, textarget) == 12,
               OffsetOf_FramebufferTexture2DMultisampleEXT_textarget_not_12);
COMPILE_ASSERT(offsetof(FramebufferTexture2DMultisampleEXT, texture) == 16,
               OffsetOf_FramebufferTexture2DMultisampleEXT_texture_not_16);
COMPILE_ASSERT(offsetof(FramebufferTexture2DMultisampleEXT, samples) == 20,
               OffsetOf_FramebufferTexture2DMultisampleEXT_samples_not_20);

struct TexStorage2DEXT {
  typedef TexStorage2DEXT ValueType;
  static const CommandId kCmdId = kTexStorage2DEXT;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLsizei _levels,
            GLenum _internalFormat,
            GLsizei _width,
            GLsizei _height) {
    SetHeader();
    target = _target;
    levels = _levels;
    internalFormat = _internalFormat;
    width = _width;
    height = _height;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLsizei _levels,
            GLenum _internalFormat,
            GLsizei _width,
            GLsizei _height) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _levels, _internalFormat, _width, _height);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t levels;
  uint32_t internalFormat;
  int32_t width;
  int32_t height;
};

COMPILE_ASSERT(sizeof(TexStorage2DEXT) == 24, Sizeof_TexStorage2DEXT_is_not_24);
COMPILE_ASSERT(offsetof(TexStorage2DEXT, header) == 0,
               OffsetOf_TexStorage2DEXT_header_not_0);
COMPILE_ASSERT(offsetof(TexStorage2DEXT, target) == 4,
               OffsetOf_TexStorage2DEXT_target_not_4);
COMPILE_ASSERT(offsetof(TexStorage2DEXT, levels) == 8,
               OffsetOf_TexStorage2DEXT_levels_not_8);
COMPILE_ASSERT(offsetof(TexStorage2DEXT, internalFormat) == 12,
               OffsetOf_TexStorage2DEXT_internalFormat_not_12);
COMPILE_ASSERT(offsetof(TexStorage2DEXT, width) == 16,
               OffsetOf_TexStorage2DEXT_width_not_16);
COMPILE_ASSERT(offsetof(TexStorage2DEXT, height) == 20,
               OffsetOf_TexStorage2DEXT_height_not_20);

struct GenQueriesEXTImmediate {
  typedef GenQueriesEXTImmediate ValueType;
  static const CommandId kCmdId = kGenQueriesEXTImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, GLuint* _queries) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _queries, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, GLuint* _queries) {
    static_cast<ValueType*>(cmd)->Init(_n, _queries);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(GenQueriesEXTImmediate) == 8,
               Sizeof_GenQueriesEXTImmediate_is_not_8);
COMPILE_ASSERT(offsetof(GenQueriesEXTImmediate, header) == 0,
               OffsetOf_GenQueriesEXTImmediate_header_not_0);
COMPILE_ASSERT(offsetof(GenQueriesEXTImmediate, n) == 4,
               OffsetOf_GenQueriesEXTImmediate_n_not_4);

struct DeleteQueriesEXTImmediate {
  typedef DeleteQueriesEXTImmediate ValueType;
  static const CommandId kCmdId = kDeleteQueriesEXTImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, const GLuint* _queries) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _queries, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, const GLuint* _queries) {
    static_cast<ValueType*>(cmd)->Init(_n, _queries);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(DeleteQueriesEXTImmediate) == 8,
               Sizeof_DeleteQueriesEXTImmediate_is_not_8);
COMPILE_ASSERT(offsetof(DeleteQueriesEXTImmediate, header) == 0,
               OffsetOf_DeleteQueriesEXTImmediate_header_not_0);
COMPILE_ASSERT(offsetof(DeleteQueriesEXTImmediate, n) == 4,
               OffsetOf_DeleteQueriesEXTImmediate_n_not_4);

struct BeginQueryEXT {
  typedef BeginQueryEXT ValueType;
  static const CommandId kCmdId = kBeginQueryEXT;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLuint _id,
            uint32_t _sync_data_shm_id,
            uint32_t _sync_data_shm_offset) {
    SetHeader();
    target = _target;
    id = _id;
    sync_data_shm_id = _sync_data_shm_id;
    sync_data_shm_offset = _sync_data_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLuint _id,
            uint32_t _sync_data_shm_id,
            uint32_t _sync_data_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _id, _sync_data_shm_id, _sync_data_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t id;
  uint32_t sync_data_shm_id;
  uint32_t sync_data_shm_offset;
};

COMPILE_ASSERT(sizeof(BeginQueryEXT) == 20, Sizeof_BeginQueryEXT_is_not_20);
COMPILE_ASSERT(offsetof(BeginQueryEXT, header) == 0,
               OffsetOf_BeginQueryEXT_header_not_0);
COMPILE_ASSERT(offsetof(BeginQueryEXT, target) == 4,
               OffsetOf_BeginQueryEXT_target_not_4);
COMPILE_ASSERT(offsetof(BeginQueryEXT, id) == 8,
               OffsetOf_BeginQueryEXT_id_not_8);
COMPILE_ASSERT(offsetof(BeginQueryEXT, sync_data_shm_id) == 12,
               OffsetOf_BeginQueryEXT_sync_data_shm_id_not_12);
COMPILE_ASSERT(offsetof(BeginQueryEXT, sync_data_shm_offset) == 16,
               OffsetOf_BeginQueryEXT_sync_data_shm_offset_not_16);

struct EndQueryEXT {
  typedef EndQueryEXT ValueType;
  static const CommandId kCmdId = kEndQueryEXT;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target, GLuint _submit_count) {
    SetHeader();
    target = _target;
    submit_count = _submit_count;
  }

  void* Set(void* cmd, GLenum _target, GLuint _submit_count) {
    static_cast<ValueType*>(cmd)->Init(_target, _submit_count);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t submit_count;
};

COMPILE_ASSERT(sizeof(EndQueryEXT) == 12, Sizeof_EndQueryEXT_is_not_12);
COMPILE_ASSERT(offsetof(EndQueryEXT, header) == 0,
               OffsetOf_EndQueryEXT_header_not_0);
COMPILE_ASSERT(offsetof(EndQueryEXT, target) == 4,
               OffsetOf_EndQueryEXT_target_not_4);
COMPILE_ASSERT(offsetof(EndQueryEXT, submit_count) == 8,
               OffsetOf_EndQueryEXT_submit_count_not_8);

struct InsertEventMarkerEXT {
  typedef InsertEventMarkerEXT ValueType;
  static const CommandId kCmdId = kInsertEventMarkerEXT;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _bucket_id) {
    SetHeader();
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, GLuint _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(InsertEventMarkerEXT) == 8,
               Sizeof_InsertEventMarkerEXT_is_not_8);
COMPILE_ASSERT(offsetof(InsertEventMarkerEXT, header) == 0,
               OffsetOf_InsertEventMarkerEXT_header_not_0);
COMPILE_ASSERT(offsetof(InsertEventMarkerEXT, bucket_id) == 4,
               OffsetOf_InsertEventMarkerEXT_bucket_id_not_4);

struct PushGroupMarkerEXT {
  typedef PushGroupMarkerEXT ValueType;
  static const CommandId kCmdId = kPushGroupMarkerEXT;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _bucket_id) {
    SetHeader();
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, GLuint _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(PushGroupMarkerEXT) == 8,
               Sizeof_PushGroupMarkerEXT_is_not_8);
COMPILE_ASSERT(offsetof(PushGroupMarkerEXT, header) == 0,
               OffsetOf_PushGroupMarkerEXT_header_not_0);
COMPILE_ASSERT(offsetof(PushGroupMarkerEXT, bucket_id) == 4,
               OffsetOf_PushGroupMarkerEXT_bucket_id_not_4);

struct PopGroupMarkerEXT {
  typedef PopGroupMarkerEXT ValueType;
  static const CommandId kCmdId = kPopGroupMarkerEXT;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init() { SetHeader(); }

  void* Set(void* cmd) {
    static_cast<ValueType*>(cmd)->Init();
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
};

COMPILE_ASSERT(sizeof(PopGroupMarkerEXT) == 4,
               Sizeof_PopGroupMarkerEXT_is_not_4);
COMPILE_ASSERT(offsetof(PopGroupMarkerEXT, header) == 0,
               OffsetOf_PopGroupMarkerEXT_header_not_0);

struct GenVertexArraysOESImmediate {
  typedef GenVertexArraysOESImmediate ValueType;
  static const CommandId kCmdId = kGenVertexArraysOESImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, GLuint* _arrays) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _arrays, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, GLuint* _arrays) {
    static_cast<ValueType*>(cmd)->Init(_n, _arrays);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(GenVertexArraysOESImmediate) == 8,
               Sizeof_GenVertexArraysOESImmediate_is_not_8);
COMPILE_ASSERT(offsetof(GenVertexArraysOESImmediate, header) == 0,
               OffsetOf_GenVertexArraysOESImmediate_header_not_0);
COMPILE_ASSERT(offsetof(GenVertexArraysOESImmediate, n) == 4,
               OffsetOf_GenVertexArraysOESImmediate_n_not_4);

struct DeleteVertexArraysOESImmediate {
  typedef DeleteVertexArraysOESImmediate ValueType;
  static const CommandId kCmdId = kDeleteVertexArraysOESImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(GLuint) * n);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei n) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(n));  // NOLINT
  }

  void SetHeader(GLsizei n) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(n));
  }

  void Init(GLsizei _n, const GLuint* _arrays) {
    SetHeader(_n);
    n = _n;
    memcpy(ImmediateDataAddress(this), _arrays, ComputeDataSize(_n));
  }

  void* Set(void* cmd, GLsizei _n, const GLuint* _arrays) {
    static_cast<ValueType*>(cmd)->Init(_n, _arrays);
    const uint32_t size = ComputeSize(_n);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t n;
};

COMPILE_ASSERT(sizeof(DeleteVertexArraysOESImmediate) == 8,
               Sizeof_DeleteVertexArraysOESImmediate_is_not_8);
COMPILE_ASSERT(offsetof(DeleteVertexArraysOESImmediate, header) == 0,
               OffsetOf_DeleteVertexArraysOESImmediate_header_not_0);
COMPILE_ASSERT(offsetof(DeleteVertexArraysOESImmediate, n) == 4,
               OffsetOf_DeleteVertexArraysOESImmediate_n_not_4);

struct IsVertexArrayOES {
  typedef IsVertexArrayOES ValueType;
  static const CommandId kCmdId = kIsVertexArrayOES;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef uint32_t Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _array,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    array = _array;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _array,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_array, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t array;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(IsVertexArrayOES) == 16,
               Sizeof_IsVertexArrayOES_is_not_16);
COMPILE_ASSERT(offsetof(IsVertexArrayOES, header) == 0,
               OffsetOf_IsVertexArrayOES_header_not_0);
COMPILE_ASSERT(offsetof(IsVertexArrayOES, array) == 4,
               OffsetOf_IsVertexArrayOES_array_not_4);
COMPILE_ASSERT(offsetof(IsVertexArrayOES, result_shm_id) == 8,
               OffsetOf_IsVertexArrayOES_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(IsVertexArrayOES, result_shm_offset) == 12,
               OffsetOf_IsVertexArrayOES_result_shm_offset_not_12);

struct BindVertexArrayOES {
  typedef BindVertexArrayOES ValueType;
  static const CommandId kCmdId = kBindVertexArrayOES;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _array) {
    SetHeader();
    array = _array;
  }

  void* Set(void* cmd, GLuint _array) {
    static_cast<ValueType*>(cmd)->Init(_array);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t array;
};

COMPILE_ASSERT(sizeof(BindVertexArrayOES) == 8,
               Sizeof_BindVertexArrayOES_is_not_8);
COMPILE_ASSERT(offsetof(BindVertexArrayOES, header) == 0,
               OffsetOf_BindVertexArrayOES_header_not_0);
COMPILE_ASSERT(offsetof(BindVertexArrayOES, array) == 4,
               OffsetOf_BindVertexArrayOES_array_not_4);

struct SwapBuffers {
  typedef SwapBuffers ValueType;
  static const CommandId kCmdId = kSwapBuffers;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init() { SetHeader(); }

  void* Set(void* cmd) {
    static_cast<ValueType*>(cmd)->Init();
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
};

COMPILE_ASSERT(sizeof(SwapBuffers) == 4, Sizeof_SwapBuffers_is_not_4);
COMPILE_ASSERT(offsetof(SwapBuffers, header) == 0,
               OffsetOf_SwapBuffers_header_not_0);

struct GetMaxValueInBufferCHROMIUM {
  typedef GetMaxValueInBufferCHROMIUM ValueType;
  static const CommandId kCmdId = kGetMaxValueInBufferCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef GLuint Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _buffer_id,
            GLsizei _count,
            GLenum _type,
            GLuint _offset,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    buffer_id = _buffer_id;
    count = _count;
    type = _type;
    offset = _offset;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _buffer_id,
            GLsizei _count,
            GLenum _type,
            GLuint _offset,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)->Init(
        _buffer_id, _count, _type, _offset, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t buffer_id;
  int32_t count;
  uint32_t type;
  uint32_t offset;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(GetMaxValueInBufferCHROMIUM) == 28,
               Sizeof_GetMaxValueInBufferCHROMIUM_is_not_28);
COMPILE_ASSERT(offsetof(GetMaxValueInBufferCHROMIUM, header) == 0,
               OffsetOf_GetMaxValueInBufferCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(GetMaxValueInBufferCHROMIUM, buffer_id) == 4,
               OffsetOf_GetMaxValueInBufferCHROMIUM_buffer_id_not_4);
COMPILE_ASSERT(offsetof(GetMaxValueInBufferCHROMIUM, count) == 8,
               OffsetOf_GetMaxValueInBufferCHROMIUM_count_not_8);
COMPILE_ASSERT(offsetof(GetMaxValueInBufferCHROMIUM, type) == 12,
               OffsetOf_GetMaxValueInBufferCHROMIUM_type_not_12);
COMPILE_ASSERT(offsetof(GetMaxValueInBufferCHROMIUM, offset) == 16,
               OffsetOf_GetMaxValueInBufferCHROMIUM_offset_not_16);
COMPILE_ASSERT(offsetof(GetMaxValueInBufferCHROMIUM, result_shm_id) == 20,
               OffsetOf_GetMaxValueInBufferCHROMIUM_result_shm_id_not_20);
COMPILE_ASSERT(offsetof(GetMaxValueInBufferCHROMIUM, result_shm_offset) == 24,
               OffsetOf_GetMaxValueInBufferCHROMIUM_result_shm_offset_not_24);

struct GenSharedIdsCHROMIUM {
  typedef GenSharedIdsCHROMIUM ValueType;
  static const CommandId kCmdId = kGenSharedIdsCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _namespace_id,
            GLuint _id_offset,
            GLsizei _n,
            uint32_t _ids_shm_id,
            uint32_t _ids_shm_offset) {
    SetHeader();
    namespace_id = _namespace_id;
    id_offset = _id_offset;
    n = _n;
    ids_shm_id = _ids_shm_id;
    ids_shm_offset = _ids_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _namespace_id,
            GLuint _id_offset,
            GLsizei _n,
            uint32_t _ids_shm_id,
            uint32_t _ids_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_namespace_id, _id_offset, _n, _ids_shm_id, _ids_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t namespace_id;
  uint32_t id_offset;
  int32_t n;
  uint32_t ids_shm_id;
  uint32_t ids_shm_offset;
};

COMPILE_ASSERT(sizeof(GenSharedIdsCHROMIUM) == 24,
               Sizeof_GenSharedIdsCHROMIUM_is_not_24);
COMPILE_ASSERT(offsetof(GenSharedIdsCHROMIUM, header) == 0,
               OffsetOf_GenSharedIdsCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(GenSharedIdsCHROMIUM, namespace_id) == 4,
               OffsetOf_GenSharedIdsCHROMIUM_namespace_id_not_4);
COMPILE_ASSERT(offsetof(GenSharedIdsCHROMIUM, id_offset) == 8,
               OffsetOf_GenSharedIdsCHROMIUM_id_offset_not_8);
COMPILE_ASSERT(offsetof(GenSharedIdsCHROMIUM, n) == 12,
               OffsetOf_GenSharedIdsCHROMIUM_n_not_12);
COMPILE_ASSERT(offsetof(GenSharedIdsCHROMIUM, ids_shm_id) == 16,
               OffsetOf_GenSharedIdsCHROMIUM_ids_shm_id_not_16);
COMPILE_ASSERT(offsetof(GenSharedIdsCHROMIUM, ids_shm_offset) == 20,
               OffsetOf_GenSharedIdsCHROMIUM_ids_shm_offset_not_20);

struct DeleteSharedIdsCHROMIUM {
  typedef DeleteSharedIdsCHROMIUM ValueType;
  static const CommandId kCmdId = kDeleteSharedIdsCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _namespace_id,
            GLsizei _n,
            uint32_t _ids_shm_id,
            uint32_t _ids_shm_offset) {
    SetHeader();
    namespace_id = _namespace_id;
    n = _n;
    ids_shm_id = _ids_shm_id;
    ids_shm_offset = _ids_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _namespace_id,
            GLsizei _n,
            uint32_t _ids_shm_id,
            uint32_t _ids_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_namespace_id, _n, _ids_shm_id, _ids_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t namespace_id;
  int32_t n;
  uint32_t ids_shm_id;
  uint32_t ids_shm_offset;
};

COMPILE_ASSERT(sizeof(DeleteSharedIdsCHROMIUM) == 20,
               Sizeof_DeleteSharedIdsCHROMIUM_is_not_20);
COMPILE_ASSERT(offsetof(DeleteSharedIdsCHROMIUM, header) == 0,
               OffsetOf_DeleteSharedIdsCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(DeleteSharedIdsCHROMIUM, namespace_id) == 4,
               OffsetOf_DeleteSharedIdsCHROMIUM_namespace_id_not_4);
COMPILE_ASSERT(offsetof(DeleteSharedIdsCHROMIUM, n) == 8,
               OffsetOf_DeleteSharedIdsCHROMIUM_n_not_8);
COMPILE_ASSERT(offsetof(DeleteSharedIdsCHROMIUM, ids_shm_id) == 12,
               OffsetOf_DeleteSharedIdsCHROMIUM_ids_shm_id_not_12);
COMPILE_ASSERT(offsetof(DeleteSharedIdsCHROMIUM, ids_shm_offset) == 16,
               OffsetOf_DeleteSharedIdsCHROMIUM_ids_shm_offset_not_16);

struct RegisterSharedIdsCHROMIUM {
  typedef RegisterSharedIdsCHROMIUM ValueType;
  static const CommandId kCmdId = kRegisterSharedIdsCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _namespace_id,
            GLsizei _n,
            uint32_t _ids_shm_id,
            uint32_t _ids_shm_offset) {
    SetHeader();
    namespace_id = _namespace_id;
    n = _n;
    ids_shm_id = _ids_shm_id;
    ids_shm_offset = _ids_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _namespace_id,
            GLsizei _n,
            uint32_t _ids_shm_id,
            uint32_t _ids_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_namespace_id, _n, _ids_shm_id, _ids_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t namespace_id;
  int32_t n;
  uint32_t ids_shm_id;
  uint32_t ids_shm_offset;
};

COMPILE_ASSERT(sizeof(RegisterSharedIdsCHROMIUM) == 20,
               Sizeof_RegisterSharedIdsCHROMIUM_is_not_20);
COMPILE_ASSERT(offsetof(RegisterSharedIdsCHROMIUM, header) == 0,
               OffsetOf_RegisterSharedIdsCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(RegisterSharedIdsCHROMIUM, namespace_id) == 4,
               OffsetOf_RegisterSharedIdsCHROMIUM_namespace_id_not_4);
COMPILE_ASSERT(offsetof(RegisterSharedIdsCHROMIUM, n) == 8,
               OffsetOf_RegisterSharedIdsCHROMIUM_n_not_8);
COMPILE_ASSERT(offsetof(RegisterSharedIdsCHROMIUM, ids_shm_id) == 12,
               OffsetOf_RegisterSharedIdsCHROMIUM_ids_shm_id_not_12);
COMPILE_ASSERT(offsetof(RegisterSharedIdsCHROMIUM, ids_shm_offset) == 16,
               OffsetOf_RegisterSharedIdsCHROMIUM_ids_shm_offset_not_16);

struct EnableFeatureCHROMIUM {
  typedef EnableFeatureCHROMIUM ValueType;
  static const CommandId kCmdId = kEnableFeatureCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  typedef GLint Result;

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _bucket_id,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    SetHeader();
    bucket_id = _bucket_id;
    result_shm_id = _result_shm_id;
    result_shm_offset = _result_shm_offset;
  }

  void* Set(void* cmd,
            GLuint _bucket_id,
            uint32_t _result_shm_id,
            uint32_t _result_shm_offset) {
    static_cast<ValueType*>(cmd)
        ->Init(_bucket_id, _result_shm_id, _result_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t bucket_id;
  uint32_t result_shm_id;
  uint32_t result_shm_offset;
};

COMPILE_ASSERT(sizeof(EnableFeatureCHROMIUM) == 16,
               Sizeof_EnableFeatureCHROMIUM_is_not_16);
COMPILE_ASSERT(offsetof(EnableFeatureCHROMIUM, header) == 0,
               OffsetOf_EnableFeatureCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(EnableFeatureCHROMIUM, bucket_id) == 4,
               OffsetOf_EnableFeatureCHROMIUM_bucket_id_not_4);
COMPILE_ASSERT(offsetof(EnableFeatureCHROMIUM, result_shm_id) == 8,
               OffsetOf_EnableFeatureCHROMIUM_result_shm_id_not_8);
COMPILE_ASSERT(offsetof(EnableFeatureCHROMIUM, result_shm_offset) == 12,
               OffsetOf_EnableFeatureCHROMIUM_result_shm_offset_not_12);

struct ResizeCHROMIUM {
  typedef ResizeCHROMIUM ValueType;
  static const CommandId kCmdId = kResizeCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _width, GLuint _height, GLfloat _scale_factor) {
    SetHeader();
    width = _width;
    height = _height;
    scale_factor = _scale_factor;
  }

  void* Set(void* cmd, GLuint _width, GLuint _height, GLfloat _scale_factor) {
    static_cast<ValueType*>(cmd)->Init(_width, _height, _scale_factor);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t width;
  uint32_t height;
  float scale_factor;
};

COMPILE_ASSERT(sizeof(ResizeCHROMIUM) == 16, Sizeof_ResizeCHROMIUM_is_not_16);
COMPILE_ASSERT(offsetof(ResizeCHROMIUM, header) == 0,
               OffsetOf_ResizeCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(ResizeCHROMIUM, width) == 4,
               OffsetOf_ResizeCHROMIUM_width_not_4);
COMPILE_ASSERT(offsetof(ResizeCHROMIUM, height) == 8,
               OffsetOf_ResizeCHROMIUM_height_not_8);
COMPILE_ASSERT(offsetof(ResizeCHROMIUM, scale_factor) == 12,
               OffsetOf_ResizeCHROMIUM_scale_factor_not_12);

struct GetRequestableExtensionsCHROMIUM {
  typedef GetRequestableExtensionsCHROMIUM ValueType;
  static const CommandId kCmdId = kGetRequestableExtensionsCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(uint32_t _bucket_id) {
    SetHeader();
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, uint32_t _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(GetRequestableExtensionsCHROMIUM) == 8,
               Sizeof_GetRequestableExtensionsCHROMIUM_is_not_8);
COMPILE_ASSERT(offsetof(GetRequestableExtensionsCHROMIUM, header) == 0,
               OffsetOf_GetRequestableExtensionsCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(GetRequestableExtensionsCHROMIUM, bucket_id) == 4,
               OffsetOf_GetRequestableExtensionsCHROMIUM_bucket_id_not_4);

struct RequestExtensionCHROMIUM {
  typedef RequestExtensionCHROMIUM ValueType;
  static const CommandId kCmdId = kRequestExtensionCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(uint32_t _bucket_id) {
    SetHeader();
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, uint32_t _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(RequestExtensionCHROMIUM) == 8,
               Sizeof_RequestExtensionCHROMIUM_is_not_8);
COMPILE_ASSERT(offsetof(RequestExtensionCHROMIUM, header) == 0,
               OffsetOf_RequestExtensionCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(RequestExtensionCHROMIUM, bucket_id) == 4,
               OffsetOf_RequestExtensionCHROMIUM_bucket_id_not_4);

struct GetMultipleIntegervCHROMIUM {
  typedef GetMultipleIntegervCHROMIUM ValueType;
  static const CommandId kCmdId = kGetMultipleIntegervCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(uint32_t _pnames_shm_id,
            uint32_t _pnames_shm_offset,
            GLuint _count,
            uint32_t _results_shm_id,
            uint32_t _results_shm_offset,
            GLsizeiptr _size) {
    SetHeader();
    pnames_shm_id = _pnames_shm_id;
    pnames_shm_offset = _pnames_shm_offset;
    count = _count;
    results_shm_id = _results_shm_id;
    results_shm_offset = _results_shm_offset;
    size = _size;
  }

  void* Set(void* cmd,
            uint32_t _pnames_shm_id,
            uint32_t _pnames_shm_offset,
            GLuint _count,
            uint32_t _results_shm_id,
            uint32_t _results_shm_offset,
            GLsizeiptr _size) {
    static_cast<ValueType*>(cmd)->Init(_pnames_shm_id,
                                       _pnames_shm_offset,
                                       _count,
                                       _results_shm_id,
                                       _results_shm_offset,
                                       _size);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t pnames_shm_id;
  uint32_t pnames_shm_offset;
  uint32_t count;
  uint32_t results_shm_id;
  uint32_t results_shm_offset;
  int32_t size;
};

COMPILE_ASSERT(sizeof(GetMultipleIntegervCHROMIUM) == 28,
               Sizeof_GetMultipleIntegervCHROMIUM_is_not_28);
COMPILE_ASSERT(offsetof(GetMultipleIntegervCHROMIUM, header) == 0,
               OffsetOf_GetMultipleIntegervCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(GetMultipleIntegervCHROMIUM, pnames_shm_id) == 4,
               OffsetOf_GetMultipleIntegervCHROMIUM_pnames_shm_id_not_4);
COMPILE_ASSERT(offsetof(GetMultipleIntegervCHROMIUM, pnames_shm_offset) == 8,
               OffsetOf_GetMultipleIntegervCHROMIUM_pnames_shm_offset_not_8);
COMPILE_ASSERT(offsetof(GetMultipleIntegervCHROMIUM, count) == 12,
               OffsetOf_GetMultipleIntegervCHROMIUM_count_not_12);
COMPILE_ASSERT(offsetof(GetMultipleIntegervCHROMIUM, results_shm_id) == 16,
               OffsetOf_GetMultipleIntegervCHROMIUM_results_shm_id_not_16);
COMPILE_ASSERT(offsetof(GetMultipleIntegervCHROMIUM, results_shm_offset) == 20,
               OffsetOf_GetMultipleIntegervCHROMIUM_results_shm_offset_not_20);
COMPILE_ASSERT(offsetof(GetMultipleIntegervCHROMIUM, size) == 24,
               OffsetOf_GetMultipleIntegervCHROMIUM_size_not_24);

struct GetProgramInfoCHROMIUM {
  typedef GetProgramInfoCHROMIUM ValueType;
  static const CommandId kCmdId = kGetProgramInfoCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  struct Result {
    uint32_t link_status;
    uint32_t num_attribs;
    uint32_t num_uniforms;
  };

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program, uint32_t _bucket_id) {
    SetHeader();
    program = _program;
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, GLuint _program, uint32_t _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_program, _bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(GetProgramInfoCHROMIUM) == 12,
               Sizeof_GetProgramInfoCHROMIUM_is_not_12);
COMPILE_ASSERT(offsetof(GetProgramInfoCHROMIUM, header) == 0,
               OffsetOf_GetProgramInfoCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(GetProgramInfoCHROMIUM, program) == 4,
               OffsetOf_GetProgramInfoCHROMIUM_program_not_4);
COMPILE_ASSERT(offsetof(GetProgramInfoCHROMIUM, bucket_id) == 8,
               OffsetOf_GetProgramInfoCHROMIUM_bucket_id_not_8);
COMPILE_ASSERT(offsetof(GetProgramInfoCHROMIUM::Result, link_status) == 0,
               OffsetOf_GetProgramInfoCHROMIUM_Result_link_status_not_0);
COMPILE_ASSERT(offsetof(GetProgramInfoCHROMIUM::Result, num_attribs) == 4,
               OffsetOf_GetProgramInfoCHROMIUM_Result_num_attribs_not_4);
COMPILE_ASSERT(offsetof(GetProgramInfoCHROMIUM::Result, num_uniforms) == 8,
               OffsetOf_GetProgramInfoCHROMIUM_Result_num_uniforms_not_8);

struct GetTranslatedShaderSourceANGLE {
  typedef GetTranslatedShaderSourceANGLE ValueType;
  static const CommandId kCmdId = kGetTranslatedShaderSourceANGLE;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _shader, uint32_t _bucket_id) {
    SetHeader();
    shader = _shader;
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, GLuint _shader, uint32_t _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_shader, _bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t shader;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(GetTranslatedShaderSourceANGLE) == 12,
               Sizeof_GetTranslatedShaderSourceANGLE_is_not_12);
COMPILE_ASSERT(offsetof(GetTranslatedShaderSourceANGLE, header) == 0,
               OffsetOf_GetTranslatedShaderSourceANGLE_header_not_0);
COMPILE_ASSERT(offsetof(GetTranslatedShaderSourceANGLE, shader) == 4,
               OffsetOf_GetTranslatedShaderSourceANGLE_shader_not_4);
COMPILE_ASSERT(offsetof(GetTranslatedShaderSourceANGLE, bucket_id) == 8,
               OffsetOf_GetTranslatedShaderSourceANGLE_bucket_id_not_8);

struct PostSubBufferCHROMIUM {
  typedef PostSubBufferCHROMIUM ValueType;
  static const CommandId kCmdId = kPostSubBufferCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _x, GLint _y, GLint _width, GLint _height) {
    SetHeader();
    x = _x;
    y = _y;
    width = _width;
    height = _height;
  }

  void* Set(void* cmd, GLint _x, GLint _y, GLint _width, GLint _height) {
    static_cast<ValueType*>(cmd)->Init(_x, _y, _width, _height);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
};

COMPILE_ASSERT(sizeof(PostSubBufferCHROMIUM) == 20,
               Sizeof_PostSubBufferCHROMIUM_is_not_20);
COMPILE_ASSERT(offsetof(PostSubBufferCHROMIUM, header) == 0,
               OffsetOf_PostSubBufferCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(PostSubBufferCHROMIUM, x) == 4,
               OffsetOf_PostSubBufferCHROMIUM_x_not_4);
COMPILE_ASSERT(offsetof(PostSubBufferCHROMIUM, y) == 8,
               OffsetOf_PostSubBufferCHROMIUM_y_not_8);
COMPILE_ASSERT(offsetof(PostSubBufferCHROMIUM, width) == 12,
               OffsetOf_PostSubBufferCHROMIUM_width_not_12);
COMPILE_ASSERT(offsetof(PostSubBufferCHROMIUM, height) == 16,
               OffsetOf_PostSubBufferCHROMIUM_height_not_16);

struct TexImageIOSurface2DCHROMIUM {
  typedef TexImageIOSurface2DCHROMIUM ValueType;
  static const CommandId kCmdId = kTexImageIOSurface2DCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLsizei _width,
            GLsizei _height,
            GLuint _ioSurfaceId,
            GLuint _plane) {
    SetHeader();
    target = _target;
    width = _width;
    height = _height;
    ioSurfaceId = _ioSurfaceId;
    plane = _plane;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLsizei _width,
            GLsizei _height,
            GLuint _ioSurfaceId,
            GLuint _plane) {
    static_cast<ValueType*>(cmd)
        ->Init(_target, _width, _height, _ioSurfaceId, _plane);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t width;
  int32_t height;
  uint32_t ioSurfaceId;
  uint32_t plane;
};

COMPILE_ASSERT(sizeof(TexImageIOSurface2DCHROMIUM) == 24,
               Sizeof_TexImageIOSurface2DCHROMIUM_is_not_24);
COMPILE_ASSERT(offsetof(TexImageIOSurface2DCHROMIUM, header) == 0,
               OffsetOf_TexImageIOSurface2DCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(TexImageIOSurface2DCHROMIUM, target) == 4,
               OffsetOf_TexImageIOSurface2DCHROMIUM_target_not_4);
COMPILE_ASSERT(offsetof(TexImageIOSurface2DCHROMIUM, width) == 8,
               OffsetOf_TexImageIOSurface2DCHROMIUM_width_not_8);
COMPILE_ASSERT(offsetof(TexImageIOSurface2DCHROMIUM, height) == 12,
               OffsetOf_TexImageIOSurface2DCHROMIUM_height_not_12);
COMPILE_ASSERT(offsetof(TexImageIOSurface2DCHROMIUM, ioSurfaceId) == 16,
               OffsetOf_TexImageIOSurface2DCHROMIUM_ioSurfaceId_not_16);
COMPILE_ASSERT(offsetof(TexImageIOSurface2DCHROMIUM, plane) == 20,
               OffsetOf_TexImageIOSurface2DCHROMIUM_plane_not_20);

struct CopyTextureCHROMIUM {
  typedef CopyTextureCHROMIUM ValueType;
  static const CommandId kCmdId = kCopyTextureCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLenum _source_id,
            GLenum _dest_id,
            GLint _level,
            GLint _internalformat,
            GLenum _dest_type) {
    SetHeader();
    target = _target;
    source_id = _source_id;
    dest_id = _dest_id;
    level = _level;
    internalformat = _internalformat;
    dest_type = _dest_type;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLenum _source_id,
            GLenum _dest_id,
            GLint _level,
            GLint _internalformat,
            GLenum _dest_type) {
    static_cast<ValueType*>(cmd)->Init(
        _target, _source_id, _dest_id, _level, _internalformat, _dest_type);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  uint32_t source_id;
  uint32_t dest_id;
  int32_t level;
  int32_t internalformat;
  uint32_t dest_type;
};

COMPILE_ASSERT(sizeof(CopyTextureCHROMIUM) == 28,
               Sizeof_CopyTextureCHROMIUM_is_not_28);
COMPILE_ASSERT(offsetof(CopyTextureCHROMIUM, header) == 0,
               OffsetOf_CopyTextureCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(CopyTextureCHROMIUM, target) == 4,
               OffsetOf_CopyTextureCHROMIUM_target_not_4);
COMPILE_ASSERT(offsetof(CopyTextureCHROMIUM, source_id) == 8,
               OffsetOf_CopyTextureCHROMIUM_source_id_not_8);
COMPILE_ASSERT(offsetof(CopyTextureCHROMIUM, dest_id) == 12,
               OffsetOf_CopyTextureCHROMIUM_dest_id_not_12);
COMPILE_ASSERT(offsetof(CopyTextureCHROMIUM, level) == 16,
               OffsetOf_CopyTextureCHROMIUM_level_not_16);
COMPILE_ASSERT(offsetof(CopyTextureCHROMIUM, internalformat) == 20,
               OffsetOf_CopyTextureCHROMIUM_internalformat_not_20);
COMPILE_ASSERT(offsetof(CopyTextureCHROMIUM, dest_type) == 24,
               OffsetOf_CopyTextureCHROMIUM_dest_type_not_24);

struct DrawArraysInstancedANGLE {
  typedef DrawArraysInstancedANGLE ValueType;
  static const CommandId kCmdId = kDrawArraysInstancedANGLE;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _mode, GLint _first, GLsizei _count, GLsizei _primcount) {
    SetHeader();
    mode = _mode;
    first = _first;
    count = _count;
    primcount = _primcount;
  }

  void* Set(void* cmd,
            GLenum _mode,
            GLint _first,
            GLsizei _count,
            GLsizei _primcount) {
    static_cast<ValueType*>(cmd)->Init(_mode, _first, _count, _primcount);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t mode;
  int32_t first;
  int32_t count;
  int32_t primcount;
};

COMPILE_ASSERT(sizeof(DrawArraysInstancedANGLE) == 20,
               Sizeof_DrawArraysInstancedANGLE_is_not_20);
COMPILE_ASSERT(offsetof(DrawArraysInstancedANGLE, header) == 0,
               OffsetOf_DrawArraysInstancedANGLE_header_not_0);
COMPILE_ASSERT(offsetof(DrawArraysInstancedANGLE, mode) == 4,
               OffsetOf_DrawArraysInstancedANGLE_mode_not_4);
COMPILE_ASSERT(offsetof(DrawArraysInstancedANGLE, first) == 8,
               OffsetOf_DrawArraysInstancedANGLE_first_not_8);
COMPILE_ASSERT(offsetof(DrawArraysInstancedANGLE, count) == 12,
               OffsetOf_DrawArraysInstancedANGLE_count_not_12);
COMPILE_ASSERT(offsetof(DrawArraysInstancedANGLE, primcount) == 16,
               OffsetOf_DrawArraysInstancedANGLE_primcount_not_16);

struct DrawElementsInstancedANGLE {
  typedef DrawElementsInstancedANGLE ValueType;
  static const CommandId kCmdId = kDrawElementsInstancedANGLE;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _mode,
            GLsizei _count,
            GLenum _type,
            GLuint _index_offset,
            GLsizei _primcount) {
    SetHeader();
    mode = _mode;
    count = _count;
    type = _type;
    index_offset = _index_offset;
    primcount = _primcount;
  }

  void* Set(void* cmd,
            GLenum _mode,
            GLsizei _count,
            GLenum _type,
            GLuint _index_offset,
            GLsizei _primcount) {
    static_cast<ValueType*>(cmd)
        ->Init(_mode, _count, _type, _index_offset, _primcount);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t mode;
  int32_t count;
  uint32_t type;
  uint32_t index_offset;
  int32_t primcount;
};

COMPILE_ASSERT(sizeof(DrawElementsInstancedANGLE) == 24,
               Sizeof_DrawElementsInstancedANGLE_is_not_24);
COMPILE_ASSERT(offsetof(DrawElementsInstancedANGLE, header) == 0,
               OffsetOf_DrawElementsInstancedANGLE_header_not_0);
COMPILE_ASSERT(offsetof(DrawElementsInstancedANGLE, mode) == 4,
               OffsetOf_DrawElementsInstancedANGLE_mode_not_4);
COMPILE_ASSERT(offsetof(DrawElementsInstancedANGLE, count) == 8,
               OffsetOf_DrawElementsInstancedANGLE_count_not_8);
COMPILE_ASSERT(offsetof(DrawElementsInstancedANGLE, type) == 12,
               OffsetOf_DrawElementsInstancedANGLE_type_not_12);
COMPILE_ASSERT(offsetof(DrawElementsInstancedANGLE, index_offset) == 16,
               OffsetOf_DrawElementsInstancedANGLE_index_offset_not_16);
COMPILE_ASSERT(offsetof(DrawElementsInstancedANGLE, primcount) == 20,
               OffsetOf_DrawElementsInstancedANGLE_primcount_not_20);

struct VertexAttribDivisorANGLE {
  typedef VertexAttribDivisorANGLE ValueType;
  static const CommandId kCmdId = kVertexAttribDivisorANGLE;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _index, GLuint _divisor) {
    SetHeader();
    index = _index;
    divisor = _divisor;
  }

  void* Set(void* cmd, GLuint _index, GLuint _divisor) {
    static_cast<ValueType*>(cmd)->Init(_index, _divisor);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t index;
  uint32_t divisor;
};

COMPILE_ASSERT(sizeof(VertexAttribDivisorANGLE) == 12,
               Sizeof_VertexAttribDivisorANGLE_is_not_12);
COMPILE_ASSERT(offsetof(VertexAttribDivisorANGLE, header) == 0,
               OffsetOf_VertexAttribDivisorANGLE_header_not_0);
COMPILE_ASSERT(offsetof(VertexAttribDivisorANGLE, index) == 4,
               OffsetOf_VertexAttribDivisorANGLE_index_not_4);
COMPILE_ASSERT(offsetof(VertexAttribDivisorANGLE, divisor) == 8,
               OffsetOf_VertexAttribDivisorANGLE_divisor_not_8);

struct ProduceTextureCHROMIUMImmediate {
  typedef ProduceTextureCHROMIUMImmediate ValueType;
  static const CommandId kCmdId = kProduceTextureCHROMIUMImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeDataSize() {
    return static_cast<uint32_t>(sizeof(GLbyte) * 64);  // NOLINT
  }

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize());  // NOLINT
  }

  void SetHeader() { header.SetCmdByTotalSize<ValueType>(ComputeSize()); }

  void Init(GLenum _target, const GLbyte* _mailbox) {
    SetHeader();
    target = _target;
    memcpy(ImmediateDataAddress(this), _mailbox, ComputeDataSize());
  }

  void* Set(void* cmd, GLenum _target, const GLbyte* _mailbox) {
    static_cast<ValueType*>(cmd)->Init(_target, _mailbox);
    const uint32_t size = ComputeSize();
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t target;
};

COMPILE_ASSERT(sizeof(ProduceTextureCHROMIUMImmediate) == 8,
               Sizeof_ProduceTextureCHROMIUMImmediate_is_not_8);
COMPILE_ASSERT(offsetof(ProduceTextureCHROMIUMImmediate, header) == 0,
               OffsetOf_ProduceTextureCHROMIUMImmediate_header_not_0);
COMPILE_ASSERT(offsetof(ProduceTextureCHROMIUMImmediate, target) == 4,
               OffsetOf_ProduceTextureCHROMIUMImmediate_target_not_4);

struct ProduceTextureDirectCHROMIUMImmediate {
  typedef ProduceTextureDirectCHROMIUMImmediate ValueType;
  static const CommandId kCmdId = kProduceTextureDirectCHROMIUMImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeDataSize() {
    return static_cast<uint32_t>(sizeof(GLbyte) * 64);  // NOLINT
  }

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize());  // NOLINT
  }

  void SetHeader() { header.SetCmdByTotalSize<ValueType>(ComputeSize()); }

  void Init(GLuint _texture, GLenum _target, const GLbyte* _mailbox) {
    SetHeader();
    texture = _texture;
    target = _target;
    memcpy(ImmediateDataAddress(this), _mailbox, ComputeDataSize());
  }

  void* Set(void* cmd,
            GLuint _texture,
            GLenum _target,
            const GLbyte* _mailbox) {
    static_cast<ValueType*>(cmd)->Init(_texture, _target, _mailbox);
    const uint32_t size = ComputeSize();
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t texture;
  uint32_t target;
};

COMPILE_ASSERT(sizeof(ProduceTextureDirectCHROMIUMImmediate) == 12,
               Sizeof_ProduceTextureDirectCHROMIUMImmediate_is_not_12);
COMPILE_ASSERT(offsetof(ProduceTextureDirectCHROMIUMImmediate, header) == 0,
               OffsetOf_ProduceTextureDirectCHROMIUMImmediate_header_not_0);
COMPILE_ASSERT(offsetof(ProduceTextureDirectCHROMIUMImmediate, texture) == 4,
               OffsetOf_ProduceTextureDirectCHROMIUMImmediate_texture_not_4);
COMPILE_ASSERT(offsetof(ProduceTextureDirectCHROMIUMImmediate, target) == 8,
               OffsetOf_ProduceTextureDirectCHROMIUMImmediate_target_not_8);

struct ConsumeTextureCHROMIUMImmediate {
  typedef ConsumeTextureCHROMIUMImmediate ValueType;
  static const CommandId kCmdId = kConsumeTextureCHROMIUMImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeDataSize() {
    return static_cast<uint32_t>(sizeof(GLbyte) * 64);  // NOLINT
  }

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize());  // NOLINT
  }

  void SetHeader() { header.SetCmdByTotalSize<ValueType>(ComputeSize()); }

  void Init(GLenum _target, const GLbyte* _mailbox) {
    SetHeader();
    target = _target;
    memcpy(ImmediateDataAddress(this), _mailbox, ComputeDataSize());
  }

  void* Set(void* cmd, GLenum _target, const GLbyte* _mailbox) {
    static_cast<ValueType*>(cmd)->Init(_target, _mailbox);
    const uint32_t size = ComputeSize();
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t target;
};

COMPILE_ASSERT(sizeof(ConsumeTextureCHROMIUMImmediate) == 8,
               Sizeof_ConsumeTextureCHROMIUMImmediate_is_not_8);
COMPILE_ASSERT(offsetof(ConsumeTextureCHROMIUMImmediate, header) == 0,
               OffsetOf_ConsumeTextureCHROMIUMImmediate_header_not_0);
COMPILE_ASSERT(offsetof(ConsumeTextureCHROMIUMImmediate, target) == 4,
               OffsetOf_ConsumeTextureCHROMIUMImmediate_target_not_4);

struct BindUniformLocationCHROMIUMBucket {
  typedef BindUniformLocationCHROMIUMBucket ValueType;
  static const CommandId kCmdId = kBindUniformLocationCHROMIUMBucket;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _program, GLint _location, uint32_t _name_bucket_id) {
    SetHeader();
    program = _program;
    location = _location;
    name_bucket_id = _name_bucket_id;
  }

  void* Set(void* cmd,
            GLuint _program,
            GLint _location,
            uint32_t _name_bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_program, _location, _name_bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t program;
  int32_t location;
  uint32_t name_bucket_id;
};

COMPILE_ASSERT(sizeof(BindUniformLocationCHROMIUMBucket) == 16,
               Sizeof_BindUniformLocationCHROMIUMBucket_is_not_16);
COMPILE_ASSERT(offsetof(BindUniformLocationCHROMIUMBucket, header) == 0,
               OffsetOf_BindUniformLocationCHROMIUMBucket_header_not_0);
COMPILE_ASSERT(offsetof(BindUniformLocationCHROMIUMBucket, program) == 4,
               OffsetOf_BindUniformLocationCHROMIUMBucket_program_not_4);
COMPILE_ASSERT(offsetof(BindUniformLocationCHROMIUMBucket, location) == 8,
               OffsetOf_BindUniformLocationCHROMIUMBucket_location_not_8);
COMPILE_ASSERT(
    offsetof(BindUniformLocationCHROMIUMBucket, name_bucket_id) == 12,
    OffsetOf_BindUniformLocationCHROMIUMBucket_name_bucket_id_not_12);

struct BindTexImage2DCHROMIUM {
  typedef BindTexImage2DCHROMIUM ValueType;
  static const CommandId kCmdId = kBindTexImage2DCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target, GLint _imageId) {
    SetHeader();
    target = _target;
    imageId = _imageId;
  }

  void* Set(void* cmd, GLenum _target, GLint _imageId) {
    static_cast<ValueType*>(cmd)->Init(_target, _imageId);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t imageId;
};

COMPILE_ASSERT(sizeof(BindTexImage2DCHROMIUM) == 12,
               Sizeof_BindTexImage2DCHROMIUM_is_not_12);
COMPILE_ASSERT(offsetof(BindTexImage2DCHROMIUM, header) == 0,
               OffsetOf_BindTexImage2DCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(BindTexImage2DCHROMIUM, target) == 4,
               OffsetOf_BindTexImage2DCHROMIUM_target_not_4);
COMPILE_ASSERT(offsetof(BindTexImage2DCHROMIUM, imageId) == 8,
               OffsetOf_BindTexImage2DCHROMIUM_imageId_not_8);

struct ReleaseTexImage2DCHROMIUM {
  typedef ReleaseTexImage2DCHROMIUM ValueType;
  static const CommandId kCmdId = kReleaseTexImage2DCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target, GLint _imageId) {
    SetHeader();
    target = _target;
    imageId = _imageId;
  }

  void* Set(void* cmd, GLenum _target, GLint _imageId) {
    static_cast<ValueType*>(cmd)->Init(_target, _imageId);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t imageId;
};

COMPILE_ASSERT(sizeof(ReleaseTexImage2DCHROMIUM) == 12,
               Sizeof_ReleaseTexImage2DCHROMIUM_is_not_12);
COMPILE_ASSERT(offsetof(ReleaseTexImage2DCHROMIUM, header) == 0,
               OffsetOf_ReleaseTexImage2DCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(ReleaseTexImage2DCHROMIUM, target) == 4,
               OffsetOf_ReleaseTexImage2DCHROMIUM_target_not_4);
COMPILE_ASSERT(offsetof(ReleaseTexImage2DCHROMIUM, imageId) == 8,
               OffsetOf_ReleaseTexImage2DCHROMIUM_imageId_not_8);

struct TraceBeginCHROMIUM {
  typedef TraceBeginCHROMIUM ValueType;
  static const CommandId kCmdId = kTraceBeginCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _bucket_id) {
    SetHeader();
    bucket_id = _bucket_id;
  }

  void* Set(void* cmd, GLuint _bucket_id) {
    static_cast<ValueType*>(cmd)->Init(_bucket_id);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t bucket_id;
};

COMPILE_ASSERT(sizeof(TraceBeginCHROMIUM) == 8,
               Sizeof_TraceBeginCHROMIUM_is_not_8);
COMPILE_ASSERT(offsetof(TraceBeginCHROMIUM, header) == 0,
               OffsetOf_TraceBeginCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(TraceBeginCHROMIUM, bucket_id) == 4,
               OffsetOf_TraceBeginCHROMIUM_bucket_id_not_4);

struct TraceEndCHROMIUM {
  typedef TraceEndCHROMIUM ValueType;
  static const CommandId kCmdId = kTraceEndCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init() { SetHeader(); }

  void* Set(void* cmd) {
    static_cast<ValueType*>(cmd)->Init();
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
};

COMPILE_ASSERT(sizeof(TraceEndCHROMIUM) == 4, Sizeof_TraceEndCHROMIUM_is_not_4);
COMPILE_ASSERT(offsetof(TraceEndCHROMIUM, header) == 0,
               OffsetOf_TraceEndCHROMIUM_header_not_0);

struct AsyncTexSubImage2DCHROMIUM {
  typedef AsyncTexSubImage2DCHROMIUM ValueType;
  static const CommandId kCmdId = kAsyncTexSubImage2DCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLint _level,
            GLint _xoffset,
            GLint _yoffset,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLenum _type,
            uint32_t _data_shm_id,
            uint32_t _data_shm_offset,
            uint32_t _async_upload_token,
            uint32_t _sync_data_shm_id,
            uint32_t _sync_data_shm_offset) {
    SetHeader();
    target = _target;
    level = _level;
    xoffset = _xoffset;
    yoffset = _yoffset;
    width = _width;
    height = _height;
    format = _format;
    type = _type;
    data_shm_id = _data_shm_id;
    data_shm_offset = _data_shm_offset;
    async_upload_token = _async_upload_token;
    sync_data_shm_id = _sync_data_shm_id;
    sync_data_shm_offset = _sync_data_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLint _level,
            GLint _xoffset,
            GLint _yoffset,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLenum _type,
            uint32_t _data_shm_id,
            uint32_t _data_shm_offset,
            uint32_t _async_upload_token,
            uint32_t _sync_data_shm_id,
            uint32_t _sync_data_shm_offset) {
    static_cast<ValueType*>(cmd)->Init(_target,
                                       _level,
                                       _xoffset,
                                       _yoffset,
                                       _width,
                                       _height,
                                       _format,
                                       _type,
                                       _data_shm_id,
                                       _data_shm_offset,
                                       _async_upload_token,
                                       _sync_data_shm_id,
                                       _sync_data_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t level;
  int32_t xoffset;
  int32_t yoffset;
  int32_t width;
  int32_t height;
  uint32_t format;
  uint32_t type;
  uint32_t data_shm_id;
  uint32_t data_shm_offset;
  uint32_t async_upload_token;
  uint32_t sync_data_shm_id;
  uint32_t sync_data_shm_offset;
};

COMPILE_ASSERT(sizeof(AsyncTexSubImage2DCHROMIUM) == 56,
               Sizeof_AsyncTexSubImage2DCHROMIUM_is_not_56);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, header) == 0,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, target) == 4,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_target_not_4);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, level) == 8,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_level_not_8);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, xoffset) == 12,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_xoffset_not_12);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, yoffset) == 16,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_yoffset_not_16);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, width) == 20,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_width_not_20);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, height) == 24,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_height_not_24);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, format) == 28,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_format_not_28);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, type) == 32,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_type_not_32);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, data_shm_id) == 36,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_data_shm_id_not_36);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, data_shm_offset) == 40,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_data_shm_offset_not_40);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, async_upload_token) == 44,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_async_upload_token_not_44);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, sync_data_shm_id) == 48,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_sync_data_shm_id_not_48);
COMPILE_ASSERT(offsetof(AsyncTexSubImage2DCHROMIUM, sync_data_shm_offset) == 52,
               OffsetOf_AsyncTexSubImage2DCHROMIUM_sync_data_shm_offset_not_52);

struct AsyncTexImage2DCHROMIUM {
  typedef AsyncTexImage2DCHROMIUM ValueType;
  static const CommandId kCmdId = kAsyncTexImage2DCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target,
            GLint _level,
            GLint _internalformat,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLenum _type,
            uint32_t _pixels_shm_id,
            uint32_t _pixels_shm_offset,
            uint32_t _async_upload_token,
            uint32_t _sync_data_shm_id,
            uint32_t _sync_data_shm_offset) {
    SetHeader();
    target = _target;
    level = _level;
    internalformat = _internalformat;
    width = _width;
    height = _height;
    format = _format;
    type = _type;
    pixels_shm_id = _pixels_shm_id;
    pixels_shm_offset = _pixels_shm_offset;
    async_upload_token = _async_upload_token;
    sync_data_shm_id = _sync_data_shm_id;
    sync_data_shm_offset = _sync_data_shm_offset;
  }

  void* Set(void* cmd,
            GLenum _target,
            GLint _level,
            GLint _internalformat,
            GLsizei _width,
            GLsizei _height,
            GLenum _format,
            GLenum _type,
            uint32_t _pixels_shm_id,
            uint32_t _pixels_shm_offset,
            uint32_t _async_upload_token,
            uint32_t _sync_data_shm_id,
            uint32_t _sync_data_shm_offset) {
    static_cast<ValueType*>(cmd)->Init(_target,
                                       _level,
                                       _internalformat,
                                       _width,
                                       _height,
                                       _format,
                                       _type,
                                       _pixels_shm_id,
                                       _pixels_shm_offset,
                                       _async_upload_token,
                                       _sync_data_shm_id,
                                       _sync_data_shm_offset);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t level;
  int32_t internalformat;
  int32_t width;
  int32_t height;
  uint32_t format;
  uint32_t type;
  uint32_t pixels_shm_id;
  uint32_t pixels_shm_offset;
  uint32_t async_upload_token;
  uint32_t sync_data_shm_id;
  uint32_t sync_data_shm_offset;
  static const int32_t border = 0;
};

COMPILE_ASSERT(sizeof(AsyncTexImage2DCHROMIUM) == 52,
               Sizeof_AsyncTexImage2DCHROMIUM_is_not_52);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, header) == 0,
               OffsetOf_AsyncTexImage2DCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, target) == 4,
               OffsetOf_AsyncTexImage2DCHROMIUM_target_not_4);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, level) == 8,
               OffsetOf_AsyncTexImage2DCHROMIUM_level_not_8);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, internalformat) == 12,
               OffsetOf_AsyncTexImage2DCHROMIUM_internalformat_not_12);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, width) == 16,
               OffsetOf_AsyncTexImage2DCHROMIUM_width_not_16);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, height) == 20,
               OffsetOf_AsyncTexImage2DCHROMIUM_height_not_20);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, format) == 24,
               OffsetOf_AsyncTexImage2DCHROMIUM_format_not_24);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, type) == 28,
               OffsetOf_AsyncTexImage2DCHROMIUM_type_not_28);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, pixels_shm_id) == 32,
               OffsetOf_AsyncTexImage2DCHROMIUM_pixels_shm_id_not_32);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, pixels_shm_offset) == 36,
               OffsetOf_AsyncTexImage2DCHROMIUM_pixels_shm_offset_not_36);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, async_upload_token) == 40,
               OffsetOf_AsyncTexImage2DCHROMIUM_async_upload_token_not_40);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, sync_data_shm_id) == 44,
               OffsetOf_AsyncTexImage2DCHROMIUM_sync_data_shm_id_not_44);
COMPILE_ASSERT(offsetof(AsyncTexImage2DCHROMIUM, sync_data_shm_offset) == 48,
               OffsetOf_AsyncTexImage2DCHROMIUM_sync_data_shm_offset_not_48);

struct WaitAsyncTexImage2DCHROMIUM {
  typedef WaitAsyncTexImage2DCHROMIUM ValueType;
  static const CommandId kCmdId = kWaitAsyncTexImage2DCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _target) {
    SetHeader();
    target = _target;
  }

  void* Set(void* cmd, GLenum _target) {
    static_cast<ValueType*>(cmd)->Init(_target);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t target;
};

COMPILE_ASSERT(sizeof(WaitAsyncTexImage2DCHROMIUM) == 8,
               Sizeof_WaitAsyncTexImage2DCHROMIUM_is_not_8);
COMPILE_ASSERT(offsetof(WaitAsyncTexImage2DCHROMIUM, header) == 0,
               OffsetOf_WaitAsyncTexImage2DCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(WaitAsyncTexImage2DCHROMIUM, target) == 4,
               OffsetOf_WaitAsyncTexImage2DCHROMIUM_target_not_4);

struct WaitAllAsyncTexImage2DCHROMIUM {
  typedef WaitAllAsyncTexImage2DCHROMIUM ValueType;
  static const CommandId kCmdId = kWaitAllAsyncTexImage2DCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init() { SetHeader(); }

  void* Set(void* cmd) {
    static_cast<ValueType*>(cmd)->Init();
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
};

COMPILE_ASSERT(sizeof(WaitAllAsyncTexImage2DCHROMIUM) == 4,
               Sizeof_WaitAllAsyncTexImage2DCHROMIUM_is_not_4);
COMPILE_ASSERT(offsetof(WaitAllAsyncTexImage2DCHROMIUM, header) == 0,
               OffsetOf_WaitAllAsyncTexImage2DCHROMIUM_header_not_0);

struct DiscardFramebufferEXTImmediate {
  typedef DiscardFramebufferEXTImmediate ValueType;
  static const CommandId kCmdId = kDiscardFramebufferEXTImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLenum) * 1 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLenum _target, GLsizei _count, const GLenum* _attachments) {
    SetHeader(_count);
    target = _target;
    count = _count;
    memcpy(ImmediateDataAddress(this), _attachments, ComputeDataSize(_count));
  }

  void* Set(void* cmd,
            GLenum _target,
            GLsizei _count,
            const GLenum* _attachments) {
    static_cast<ValueType*>(cmd)->Init(_target, _count, _attachments);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  uint32_t target;
  int32_t count;
};

COMPILE_ASSERT(sizeof(DiscardFramebufferEXTImmediate) == 12,
               Sizeof_DiscardFramebufferEXTImmediate_is_not_12);
COMPILE_ASSERT(offsetof(DiscardFramebufferEXTImmediate, header) == 0,
               OffsetOf_DiscardFramebufferEXTImmediate_header_not_0);
COMPILE_ASSERT(offsetof(DiscardFramebufferEXTImmediate, target) == 4,
               OffsetOf_DiscardFramebufferEXTImmediate_target_not_4);
COMPILE_ASSERT(offsetof(DiscardFramebufferEXTImmediate, count) == 8,
               OffsetOf_DiscardFramebufferEXTImmediate_count_not_8);

struct LoseContextCHROMIUM {
  typedef LoseContextCHROMIUM ValueType;
  static const CommandId kCmdId = kLoseContextCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLenum _current, GLenum _other) {
    SetHeader();
    current = _current;
    other = _other;
  }

  void* Set(void* cmd, GLenum _current, GLenum _other) {
    static_cast<ValueType*>(cmd)->Init(_current, _other);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t current;
  uint32_t other;
};

COMPILE_ASSERT(sizeof(LoseContextCHROMIUM) == 12,
               Sizeof_LoseContextCHROMIUM_is_not_12);
COMPILE_ASSERT(offsetof(LoseContextCHROMIUM, header) == 0,
               OffsetOf_LoseContextCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(LoseContextCHROMIUM, current) == 4,
               OffsetOf_LoseContextCHROMIUM_current_not_4);
COMPILE_ASSERT(offsetof(LoseContextCHROMIUM, other) == 8,
               OffsetOf_LoseContextCHROMIUM_other_not_8);

struct WaitSyncPointCHROMIUM {
  typedef WaitSyncPointCHROMIUM ValueType;
  static const CommandId kCmdId = kWaitSyncPointCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(1);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLuint _sync_point) {
    SetHeader();
    sync_point = _sync_point;
  }

  void* Set(void* cmd, GLuint _sync_point) {
    static_cast<ValueType*>(cmd)->Init(_sync_point);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  uint32_t sync_point;
};

COMPILE_ASSERT(sizeof(WaitSyncPointCHROMIUM) == 8,
               Sizeof_WaitSyncPointCHROMIUM_is_not_8);
COMPILE_ASSERT(offsetof(WaitSyncPointCHROMIUM, header) == 0,
               OffsetOf_WaitSyncPointCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(WaitSyncPointCHROMIUM, sync_point) == 4,
               OffsetOf_WaitSyncPointCHROMIUM_sync_point_not_4);

struct DrawBuffersEXTImmediate {
  typedef DrawBuffersEXTImmediate ValueType;
  static const CommandId kCmdId = kDrawBuffersEXTImmediate;
  static const cmd::ArgFlags kArgFlags = cmd::kAtLeastN;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeDataSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(GLenum) * 1 * count);  // NOLINT
  }

  static uint32_t ComputeSize(GLsizei count) {
    return static_cast<uint32_t>(sizeof(ValueType) +
                                 ComputeDataSize(count));  // NOLINT
  }

  void SetHeader(GLsizei count) {
    header.SetCmdByTotalSize<ValueType>(ComputeSize(count));
  }

  void Init(GLsizei _count, const GLenum* _bufs) {
    SetHeader(_count);
    count = _count;
    memcpy(ImmediateDataAddress(this), _bufs, ComputeDataSize(_count));
  }

  void* Set(void* cmd, GLsizei _count, const GLenum* _bufs) {
    static_cast<ValueType*>(cmd)->Init(_count, _bufs);
    const uint32_t size = ComputeSize(_count);
    return NextImmediateCmdAddressTotalSize<ValueType>(cmd, size);
  }

  gpu::CommandHeader header;
  int32_t count;
};

COMPILE_ASSERT(sizeof(DrawBuffersEXTImmediate) == 8,
               Sizeof_DrawBuffersEXTImmediate_is_not_8);
COMPILE_ASSERT(offsetof(DrawBuffersEXTImmediate, header) == 0,
               OffsetOf_DrawBuffersEXTImmediate_header_not_0);
COMPILE_ASSERT(offsetof(DrawBuffersEXTImmediate, count) == 4,
               OffsetOf_DrawBuffersEXTImmediate_count_not_4);

struct DiscardBackbufferCHROMIUM {
  typedef DiscardBackbufferCHROMIUM ValueType;
  static const CommandId kCmdId = kDiscardBackbufferCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init() { SetHeader(); }

  void* Set(void* cmd) {
    static_cast<ValueType*>(cmd)->Init();
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
};

COMPILE_ASSERT(sizeof(DiscardBackbufferCHROMIUM) == 4,
               Sizeof_DiscardBackbufferCHROMIUM_is_not_4);
COMPILE_ASSERT(offsetof(DiscardBackbufferCHROMIUM, header) == 0,
               OffsetOf_DiscardBackbufferCHROMIUM_header_not_0);

struct ScheduleOverlayPlaneCHROMIUM {
  typedef ScheduleOverlayPlaneCHROMIUM ValueType;
  static const CommandId kCmdId = kScheduleOverlayPlaneCHROMIUM;
  static const cmd::ArgFlags kArgFlags = cmd::kFixed;
  static const uint8 cmd_flags = CMD_FLAG_SET_TRACE_LEVEL(3);

  static uint32_t ComputeSize() {
    return static_cast<uint32_t>(sizeof(ValueType));  // NOLINT
  }

  void SetHeader() { header.SetCmd<ValueType>(); }

  void Init(GLint _plane_z_order,
            GLenum _plane_transform,
            GLuint _overlay_texture_id,
            GLint _bounds_x,
            GLint _bounds_y,
            GLint _bounds_width,
            GLint _bounds_height,
            GLfloat _uv_x,
            GLfloat _uv_y,
            GLfloat _uv_width,
            GLfloat _uv_height) {
    SetHeader();
    plane_z_order = _plane_z_order;
    plane_transform = _plane_transform;
    overlay_texture_id = _overlay_texture_id;
    bounds_x = _bounds_x;
    bounds_y = _bounds_y;
    bounds_width = _bounds_width;
    bounds_height = _bounds_height;
    uv_x = _uv_x;
    uv_y = _uv_y;
    uv_width = _uv_width;
    uv_height = _uv_height;
  }

  void* Set(void* cmd,
            GLint _plane_z_order,
            GLenum _plane_transform,
            GLuint _overlay_texture_id,
            GLint _bounds_x,
            GLint _bounds_y,
            GLint _bounds_width,
            GLint _bounds_height,
            GLfloat _uv_x,
            GLfloat _uv_y,
            GLfloat _uv_width,
            GLfloat _uv_height) {
    static_cast<ValueType*>(cmd)->Init(_plane_z_order,
                                       _plane_transform,
                                       _overlay_texture_id,
                                       _bounds_x,
                                       _bounds_y,
                                       _bounds_width,
                                       _bounds_height,
                                       _uv_x,
                                       _uv_y,
                                       _uv_width,
                                       _uv_height);
    return NextCmdAddress<ValueType>(cmd);
  }

  gpu::CommandHeader header;
  int32_t plane_z_order;
  uint32_t plane_transform;
  uint32_t overlay_texture_id;
  int32_t bounds_x;
  int32_t bounds_y;
  int32_t bounds_width;
  int32_t bounds_height;
  float uv_x;
  float uv_y;
  float uv_width;
  float uv_height;
};

COMPILE_ASSERT(sizeof(ScheduleOverlayPlaneCHROMIUM) == 48,
               Sizeof_ScheduleOverlayPlaneCHROMIUM_is_not_48);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, header) == 0,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_header_not_0);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, plane_z_order) == 4,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_plane_z_order_not_4);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, plane_transform) == 8,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_plane_transform_not_8);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, overlay_texture_id) == 12,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_overlay_texture_id_not_12);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, bounds_x) == 16,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_bounds_x_not_16);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, bounds_y) == 20,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_bounds_y_not_20);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, bounds_width) == 24,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_bounds_width_not_24);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, bounds_height) == 28,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_bounds_height_not_28);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, uv_x) == 32,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_uv_x_not_32);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, uv_y) == 36,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_uv_y_not_36);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, uv_width) == 40,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_uv_width_not_40);
COMPILE_ASSERT(offsetof(ScheduleOverlayPlaneCHROMIUM, uv_height) == 44,
               OffsetOf_ScheduleOverlayPlaneCHROMIUM_uv_height_not_44);

#endif  // GPU_COMMAND_BUFFER_COMMON_GLES2_CMD_FORMAT_AUTOGEN_H_
