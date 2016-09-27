// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_BASE_H_
#define GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_BASE_H_

#include "gpu/command_buffer/common/gles2_cmd_format.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/service/buffer_manager.h"
#include "gpu/command_buffer/service/cmd_buffer_engine.h"
#include "gpu/command_buffer/service/context_group.h"
#include "gpu/command_buffer/service/framebuffer_manager.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder_mock.h"
#include "gpu/command_buffer/service/program_manager.h"
#include "gpu/command_buffer/service/query_manager.h"
#include "gpu/command_buffer/service/renderbuffer_manager.h"
#include "gpu/command_buffer/service/shader_manager.h"
#include "gpu/command_buffer/service/test_helper.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "gpu/command_buffer/service/vertex_array_manager.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_context_stub_with_extensions.h"
#include "ui/gl/gl_surface_stub.h"
#include "ui/gl/gl_mock.h"

namespace base {
class CommandLine;
}

namespace gpu {
namespace gles2 {

class MemoryTracker;

class GLES2DecoderTestBase : public ::testing::TestWithParam<bool> {
 public:
  GLES2DecoderTestBase();
  virtual ~GLES2DecoderTestBase();

  // Template to call glGenXXX functions.
  template <typename T>
  void GenHelper(GLuint client_id) {
    int8 buffer[sizeof(T) + sizeof(client_id)];
    T& cmd = *reinterpret_cast<T*>(&buffer);
    cmd.Init(1, &client_id);
    EXPECT_EQ(error::kNoError,
              ExecuteImmediateCmd(cmd, sizeof(client_id)));
  }

  // This template exists solely so we can specialize it for
  // certain commands.
  template <typename T, int id>
  void SpecializedSetup(bool valid) {
  }

  template <typename T>
  T* GetImmediateAs() {
    return reinterpret_cast<T*>(immediate_buffer_);
  }

  template <typename T, typename Command>
  T GetImmediateDataAs(Command* cmd) {
    return reinterpret_cast<T>(ImmediateDataAddress(cmd));
  }

  void ClearSharedMemory() {
    engine_->ClearSharedMemory();
  }

  virtual void SetUp() OVERRIDE;
  virtual void TearDown() OVERRIDE;

  template <typename T>
  error::Error ExecuteCmd(const T& cmd) {
    COMPILE_ASSERT(T::kArgFlags == cmd::kFixed, Cmd_kArgFlags_not_kFixed);
    return decoder_->DoCommand(cmd.kCmdId,
                               ComputeNumEntries(sizeof(cmd)) - 1,
                               &cmd);
  }

  template <typename T>
  error::Error ExecuteImmediateCmd(const T& cmd, size_t data_size) {
    COMPILE_ASSERT(T::kArgFlags == cmd::kAtLeastN, Cmd_kArgFlags_not_kAtLeastN);
    return decoder_->DoCommand(cmd.kCmdId,
                               ComputeNumEntries(sizeof(cmd) + data_size) - 1,
                               &cmd);
  }

  template <typename T>
  T GetSharedMemoryAs() {
    return reinterpret_cast<T>(shared_memory_address_);
  }

  template <typename T>
  T GetSharedMemoryAsWithOffset(uint32 offset) {
    void* ptr = reinterpret_cast<int8*>(shared_memory_address_) + offset;
    return reinterpret_cast<T>(ptr);
  }

  IdAllocatorInterface* GetIdAllocator(GLuint namespace_id) {
    return group_->GetIdAllocator(namespace_id);
  }

  Buffer* GetBuffer(GLuint service_id) {
    return group_->buffer_manager()->GetBuffer(service_id);
  }

  Framebuffer* GetFramebuffer(GLuint service_id) {
    return group_->framebuffer_manager()->GetFramebuffer(service_id);
  }

  Renderbuffer* GetRenderbuffer(
      GLuint service_id) {
    return group_->renderbuffer_manager()->GetRenderbuffer(service_id);
  }

  TextureRef* GetTexture(GLuint client_id) {
    return group_->texture_manager()->GetTexture(client_id);
  }

  Shader* GetShader(GLuint client_id) {
    return group_->shader_manager()->GetShader(client_id);
  }

  Program* GetProgram(GLuint client_id) {
    return group_->program_manager()->GetProgram(client_id);
  }

  QueryManager::Query* GetQueryInfo(GLuint client_id) {
    return decoder_->GetQueryManager()->GetQuery(client_id);
  }

  // This name doesn't match the underlying function, but doing it this way
  // prevents the need to special-case the unit test generation
  VertexAttribManager* GetVertexArrayInfo(GLuint client_id) {
    return decoder_->GetVertexArrayManager()->GetVertexAttribManager(client_id);
  }

  ProgramManager* program_manager() {
    return group_->program_manager();
  }

  void DoCreateProgram(GLuint client_id, GLuint service_id);
  void DoCreateShader(GLenum shader_type, GLuint client_id, GLuint service_id);

  void SetBucketAsCString(uint32 bucket_id, const char* str);

  void set_memory_tracker(MemoryTracker* memory_tracker) {
    memory_tracker_ = memory_tracker;
  }

  struct InitState {
    InitState();

    std::string extensions;
    std::string gl_version;
    bool has_alpha;
    bool has_depth;
    bool has_stencil;
    bool request_alpha;
    bool request_depth;
    bool request_stencil;
    bool bind_generates_resource;
    bool lose_context_when_out_of_memory;
    bool use_native_vao;  // default is true.
  };

  void InitDecoder(const InitState& init);
  void InitDecoderWithCommandLine(const InitState& init,
                                  const base::CommandLine* command_line);

  void ResetDecoder();

  const ContextGroup& group() const {
    return *group_.get();
  }

  ::testing::StrictMock< ::gfx::MockGLInterface>* GetGLMock() const {
    return gl_.get();
  }

  GLES2Decoder* GetDecoder() const {
    return decoder_.get();
  }

  typedef TestHelper::AttribInfo AttribInfo;
  typedef TestHelper::UniformInfo UniformInfo;

  void SetupShader(
      AttribInfo* attribs, size_t num_attribs,
      UniformInfo* uniforms, size_t num_uniforms,
      GLuint client_id, GLuint service_id,
      GLuint vertex_shader_client_id, GLuint vertex_shader_service_id,
      GLuint fragment_shader_client_id, GLuint fragment_shader_service_id);

  void SetupInitCapabilitiesExpectations();
  void SetupInitStateExpectations();
  void ExpectEnableDisable(GLenum cap, bool enable);

  // Setups up a shader for testing glUniform.
  void SetupShaderForUniform(GLenum uniform_type);
  void SetupDefaultProgram();
  void SetupCubemapProgram();
  void SetupSamplerExternalProgram();
  void SetupTexture();

  // Note that the error is returned as GLint instead of GLenum.
  // This is because there is a mismatch in the types of GLenum and
  // the error values GL_NO_ERROR, GL_INVALID_ENUM, etc. GLenum is
  // typedef'd as unsigned int while the error values are defined as
  // integers. This is problematic for template functions such as
  // EXPECT_EQ that expect both types to be the same.
  GLint GetGLError();

  void DoBindBuffer(GLenum target, GLuint client_id, GLuint service_id);
  void DoBindFramebuffer(GLenum target, GLuint client_id, GLuint service_id);
  void DoBindRenderbuffer(GLenum target, GLuint client_id, GLuint service_id);
  void DoBindTexture(GLenum target, GLuint client_id, GLuint service_id);
  void DoBindVertexArrayOES(GLuint client_id, GLuint service_id);

  bool DoIsBuffer(GLuint client_id);
  bool DoIsFramebuffer(GLuint client_id);
  bool DoIsProgram(GLuint client_id);
  bool DoIsRenderbuffer(GLuint client_id);
  bool DoIsShader(GLuint client_id);
  bool DoIsTexture(GLuint client_id);

  void DoDeleteBuffer(GLuint client_id, GLuint service_id);
  void DoDeleteFramebuffer(
      GLuint client_id, GLuint service_id,
      bool reset_draw, GLenum draw_target, GLuint draw_id,
      bool reset_read, GLenum read_target, GLuint read_id);
  void DoDeleteProgram(GLuint client_id, GLuint service_id);
  void DoDeleteRenderbuffer(GLuint client_id, GLuint service_id);
  void DoDeleteShader(GLuint client_id, GLuint service_id);
  void DoDeleteTexture(GLuint client_id, GLuint service_id);

  void DoCompressedTexImage2D(
      GLenum target, GLint level, GLenum format,
      GLsizei width, GLsizei height, GLint border,
      GLsizei size, uint32 bucket_id);
  void DoTexImage2D(
      GLenum target, GLint level, GLenum internal_format,
      GLsizei width, GLsizei height, GLint border,
      GLenum format, GLenum type,
      uint32 shared_memory_id, uint32 shared_memory_offset);
  void DoTexImage2DConvertInternalFormat(
      GLenum target, GLint level, GLenum requested_internal_format,
      GLsizei width, GLsizei height, GLint border,
      GLenum format, GLenum type,
      uint32 shared_memory_id, uint32 shared_memory_offset,
      GLenum expected_internal_format);
  void DoRenderbufferStorage(
      GLenum target, GLenum internal_format, GLenum actual_format,
      GLsizei width, GLsizei height, GLenum error);
  void DoFramebufferRenderbuffer(
      GLenum target,
      GLenum attachment,
      GLenum renderbuffer_target,
      GLuint renderbuffer_client_id,
      GLuint renderbuffer_service_id,
      GLenum error);
  void DoFramebufferTexture2D(
      GLenum target, GLenum attachment, GLenum tex_target,
      GLuint texture_client_id, GLuint texture_service_id,
      GLint level, GLenum error);
  void DoVertexAttribPointer(
      GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset);
  void DoVertexAttribDivisorANGLE(GLuint index, GLuint divisor);

  void DoEnableDisable(GLenum cap, bool enable);

  void DoEnableVertexAttribArray(GLint index);

  void DoBufferData(GLenum target, GLsizei size);

  void DoBufferSubData(
      GLenum target, GLint offset, GLsizei size, const void* data);

  void SetupVertexBuffer();
  void SetupAllNeededVertexBuffers();

  void SetupIndexBuffer();

  void DeleteVertexBuffer();

  void DeleteIndexBuffer();

  void SetupClearTextureExpectations(
      GLuint service_id,
      GLuint old_service_id,
      GLenum bind_target,
      GLenum target,
      GLint level,
      GLenum internal_format,
      GLenum format,
      GLenum type,
      GLsizei width,
      GLsizei height);

  void SetupExpectationsForRestoreClearState(
      GLclampf restore_red,
      GLclampf restore_green,
      GLclampf restore_blue,
      GLclampf restore_alpha,
      GLuint restore_stencil,
      GLclampf restore_depth,
      bool restore_scissor_test);

  void SetupExpectationsForFramebufferClearing(
      GLenum target,
      GLuint clear_bits,
      GLclampf restore_red,
      GLclampf restore_green,
      GLclampf restore_blue,
      GLclampf restore_alpha,
      GLuint restore_stencil,
      GLclampf restore_depth,
      bool restore_scissor_test);

  void SetupExpectationsForFramebufferClearingMulti(
      GLuint read_framebuffer_service_id,
      GLuint draw_framebuffer_service_id,
      GLenum target,
      GLuint clear_bits,
      GLclampf restore_red,
      GLclampf restore_green,
      GLclampf restore_blue,
      GLclampf restore_alpha,
      GLuint restore_stencil,
      GLclampf restore_depth,
      bool restore_scissor_test);

  void SetupExpectationsForDepthMask(bool mask);
  void SetupExpectationsForEnableDisable(GLenum cap, bool enable);
  void SetupExpectationsForColorMask(bool red,
                                     bool green,
                                     bool blue,
                                     bool alpha);
  void SetupExpectationsForStencilMask(uint32 front_mask,
                                       uint32 back_mask);

  void SetupExpectationsForApplyingDirtyState(
      bool framebuffer_is_rgb,
      bool framebuffer_has_depth,
      bool framebuffer_has_stencil,
      GLuint color_bits,  // NOTE! bits are 0x1000, 0x0100, 0x0010, and 0x0001
      bool depth_mask,
      bool depth_enabled,
      GLuint front_stencil_mask,
      GLuint back_stencil_mask,
      bool stencil_enabled);

  void SetupExpectationsForApplyingDefaultDirtyState();

  void AddExpectationsForSimulatedAttrib0WithError(
      GLsizei num_vertices, GLuint buffer_id, GLenum error);

  void AddExpectationsForSimulatedAttrib0(
      GLsizei num_vertices, GLuint buffer_id);

  void AddExpectationsForGenVertexArraysOES();
  void AddExpectationsForDeleteVertexArraysOES();
  void AddExpectationsForDeleteBoundVertexArraysOES();
  void AddExpectationsForBindVertexArrayOES();
  void AddExpectationsForRestoreAttribState(GLuint attrib);

  GLvoid* BufferOffset(unsigned i) {
    return static_cast<int8 *>(NULL)+(i);
  }

  template <typename Command, typename Result>
  bool IsObjectHelper(GLuint client_id) {
    Result* result = static_cast<Result*>(shared_memory_address_);
    Command cmd;
    cmd.Init(client_id, kSharedMemoryId, kSharedMemoryOffset);
    EXPECT_EQ(error::kNoError, ExecuteCmd(cmd));
    bool isObject = static_cast<bool>(*result);
    EXPECT_EQ(GL_NO_ERROR, GetGLError());
    return isObject;
  }

 protected:
  static const int kBackBufferWidth = 128;
  static const int kBackBufferHeight = 64;

  static const GLint kMaxTextureSize = 2048;
  static const GLint kMaxCubeMapTextureSize = 256;
  static const GLint kNumVertexAttribs = 16;
  static const GLint kNumTextureUnits = 8;
  static const GLint kMaxTextureImageUnits = 8;
  static const GLint kMaxVertexTextureImageUnits = 2;
  static const GLint kMaxFragmentUniformVectors = 16;
  static const GLint kMaxVaryingVectors = 8;
  static const GLint kMaxVertexUniformVectors = 128;
  static const GLint kMaxViewportWidth = 8192;
  static const GLint kMaxViewportHeight = 8192;

  static const GLint kViewportX = 0;
  static const GLint kViewportY = 0;
  static const GLint kViewportWidth = kBackBufferWidth;
  static const GLint kViewportHeight = kBackBufferHeight;

  static const GLuint kServiceAttrib0BufferId = 801;
  static const GLuint kServiceFixedAttribBufferId = 802;

  static const GLuint kServiceBufferId = 301;
  static const GLuint kServiceFramebufferId = 302;
  static const GLuint kServiceRenderbufferId = 303;
  static const GLuint kServiceTextureId = 304;
  static const GLuint kServiceProgramId = 305;
  static const GLuint kServiceShaderId = 306;
  static const GLuint kServiceElementBufferId = 308;
  static const GLuint kServiceQueryId = 309;
  static const GLuint kServiceVertexArrayId = 310;

  static const int32 kSharedMemoryId = 401;
  static const size_t kSharedBufferSize = 2048;
  static const uint32 kSharedMemoryOffset = 132;
  static const int32 kInvalidSharedMemoryId = 402;
  static const uint32 kInvalidSharedMemoryOffset = kSharedBufferSize + 1;
  static const uint32 kInitialResult = 0xBDBDBDBDu;
  static const uint8 kInitialMemoryValue = 0xBDu;

  static const uint32 kNewClientId = 501;
  static const uint32 kNewServiceId = 502;
  static const uint32 kInvalidClientId = 601;

  static const GLuint kServiceVertexShaderId = 321;
  static const GLuint kServiceFragmentShaderId = 322;

  static const GLuint kServiceCopyTextureChromiumShaderId = 701;
  static const GLuint kServiceCopyTextureChromiumProgramId = 721;

  static const GLuint kServiceCopyTextureChromiumTextureBufferId = 751;
  static const GLuint kServiceCopyTextureChromiumVertexBufferId = 752;
  static const GLuint kServiceCopyTextureChromiumFBOId = 753;
  static const GLuint kServiceCopyTextureChromiumPositionAttrib = 761;
  static const GLuint kServiceCopyTextureChromiumTexAttrib = 762;
  static const GLuint kServiceCopyTextureChromiumSamplerLocation = 763;

  static const GLsizei kNumVertices = 100;
  static const GLsizei kNumIndices = 10;
  static const int kValidIndexRangeStart = 1;
  static const int kValidIndexRangeCount = 7;
  static const int kInvalidIndexRangeStart = 0;
  static const int kInvalidIndexRangeCount = 7;
  static const int kOutOfRangeIndexRangeEnd = 10;
  static const GLuint kMaxValidIndex = 7;

  static const GLint kMaxAttribLength = 10;
  static const char* kAttrib1Name;
  static const char* kAttrib2Name;
  static const char* kAttrib3Name;
  static const GLint kAttrib1Size = 1;
  static const GLint kAttrib2Size = 1;
  static const GLint kAttrib3Size = 1;
  static const GLint kAttrib1Location = 0;
  static const GLint kAttrib2Location = 1;
  static const GLint kAttrib3Location = 2;
  static const GLenum kAttrib1Type = GL_FLOAT_VEC4;
  static const GLenum kAttrib2Type = GL_FLOAT_VEC2;
  static const GLenum kAttrib3Type = GL_FLOAT_VEC3;
  static const GLint kInvalidAttribLocation = 30;
  static const GLint kBadAttribIndex = kNumVertexAttribs;

  static const GLint kMaxUniformLength = 12;
  static const char* kUniform1Name;
  static const char* kUniform2Name;
  static const char* kUniform3Name;
  static const GLint kUniform1Size = 1;
  static const GLint kUniform2Size = 3;
  static const GLint kUniform3Size = 2;
  static const GLint kUniform1RealLocation = 3;
  static const GLint kUniform2RealLocation = 10;
  static const GLint kUniform2ElementRealLocation = 12;
  static const GLint kUniform3RealLocation = 20;
  static const GLint kUniform1FakeLocation = 0;               // These are
  static const GLint kUniform2FakeLocation = 1;               // hardcoded
  static const GLint kUniform2ElementFakeLocation = 0x10001;  // to match
  static const GLint kUniform3FakeLocation = 2;               // ProgramManager.
  static const GLint kUniform1DesiredLocation = -1;
  static const GLint kUniform2DesiredLocation = -1;
  static const GLint kUniform3DesiredLocation = -1;
  static const GLenum kUniform1Type = GL_SAMPLER_2D;
  static const GLenum kUniform2Type = GL_INT_VEC2;
  static const GLenum kUniform3Type = GL_FLOAT_VEC3;
  static const GLenum kUniformSamplerExternalType = GL_SAMPLER_EXTERNAL_OES;
  static const GLenum kUniformCubemapType = GL_SAMPLER_CUBE;
  static const GLint kInvalidUniformLocation = 30;
  static const GLint kBadUniformIndex = 1000;

  // Use StrictMock to make 100% sure we know how GL will be called.
  scoped_ptr< ::testing::StrictMock< ::gfx::MockGLInterface> > gl_;
  scoped_refptr<gfx::GLSurfaceStub> surface_;
  scoped_refptr<gfx::GLContextStubWithExtensions> context_;
  scoped_ptr<MockGLES2Decoder> mock_decoder_;
  scoped_ptr<GLES2Decoder> decoder_;
  MemoryTracker* memory_tracker_;

  GLuint client_buffer_id_;
  GLuint client_framebuffer_id_;
  GLuint client_program_id_;
  GLuint client_renderbuffer_id_;
  GLuint client_shader_id_;
  GLuint client_texture_id_;
  GLuint client_element_buffer_id_;
  GLuint client_vertex_shader_id_;
  GLuint client_fragment_shader_id_;
  GLuint client_query_id_;
  GLuint client_vertexarray_id_;

  uint32 shared_memory_id_;
  uint32 shared_memory_offset_;
  void* shared_memory_address_;
  void* shared_memory_base_;

  int8 immediate_buffer_[256];

  const bool ignore_cached_state_for_test_;
  bool cached_color_mask_red_;
  bool cached_color_mask_green_;
  bool cached_color_mask_blue_;
  bool cached_color_mask_alpha_;
  bool cached_depth_mask_;
  uint32 cached_stencil_front_mask_;
  uint32 cached_stencil_back_mask_;

  struct EnableFlags {
    EnableFlags();
    bool cached_blend;
    bool cached_cull_face;
    bool cached_depth_test;
    bool cached_dither;
    bool cached_polygon_offset_fill;
    bool cached_sample_alpha_to_coverage;
    bool cached_sample_coverage;
    bool cached_scissor_test;
    bool cached_stencil_test;
  };

  EnableFlags enable_flags_;

 private:
  class MockCommandBufferEngine : public CommandBufferEngine {
   public:
    MockCommandBufferEngine();

    virtual ~MockCommandBufferEngine();

    virtual scoped_refptr<gpu::Buffer> GetSharedMemoryBuffer(int32 shm_id)
        OVERRIDE;

    void ClearSharedMemory() {
      memset(valid_buffer_->memory(), kInitialMemoryValue, kSharedBufferSize);
    }

    virtual void set_token(int32 token) OVERRIDE;

    virtual bool SetGetBuffer(int32 /* transfer_buffer_id */) OVERRIDE;

    // Overridden from CommandBufferEngine.
    virtual bool SetGetOffset(int32 offset) OVERRIDE;

    // Overridden from CommandBufferEngine.
    virtual int32 GetGetOffset() OVERRIDE;

   private:
    scoped_refptr<gpu::Buffer> valid_buffer_;
    scoped_refptr<gpu::Buffer> invalid_buffer_;
  };

  // MockGLStates is used to track GL states and emulate driver
  // behaviors on top of MockGLInterface.
  class MockGLStates {
   public:
    MockGLStates()
        : bound_array_buffer_object_(0),
          bound_vertex_array_object_(0) {
    }

    ~MockGLStates() {
    }

    void OnBindArrayBuffer(GLuint id) {
      bound_array_buffer_object_ = id;
    }

    void OnBindVertexArrayOES(GLuint id) {
      bound_vertex_array_object_ = id;
    }

    void OnVertexAttribNullPointer() {
      // When a vertex array object is bound, some drivers (AMD Linux,
      // Qualcomm, etc.) have a bug where it incorrectly generates an
      // GL_INVALID_OPERATION on glVertexAttribPointer() if pointer
      // is NULL, no buffer is bound on GL_ARRAY_BUFFER.
      // Make sure we don't trigger this bug.
      if (bound_vertex_array_object_ != 0)
        EXPECT_TRUE(bound_array_buffer_object_ != 0);
    }

   private:
    GLuint bound_array_buffer_object_;
    GLuint bound_vertex_array_object_;
  };  // class MockGLStates

  void AddExpectationsForVertexAttribManager();
  void SetupMockGLBehaviors();

  scoped_ptr< ::testing::StrictMock<MockCommandBufferEngine> > engine_;
  scoped_refptr<ContextGroup> group_;
  MockGLStates gl_states_;
};

class GLES2DecoderWithShaderTestBase : public GLES2DecoderTestBase {
 public:
  GLES2DecoderWithShaderTestBase()
      : GLES2DecoderTestBase() {
  }

 protected:
  virtual void SetUp() OVERRIDE;
  virtual void TearDown() OVERRIDE;

};

// SpecializedSetup specializations that are needed in multiple unittest files.
template <>
void GLES2DecoderTestBase::SpecializedSetup<cmds::LinkProgram, 0>(bool valid);

}  // namespace gles2
}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_DECODER_UNITTEST_BASE_H_
