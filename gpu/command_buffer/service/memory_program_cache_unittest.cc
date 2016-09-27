// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/memory_program_cache.h"

#include "base/bind.h"
#include "gpu/command_buffer/common/gles2_cmd_format.h"
#include "gpu/command_buffer/service/gl_utils.h"
#include "gpu/command_buffer/service/gpu_service_test.h"
#include "gpu/command_buffer/service/shader_manager.h"
#include "gpu/command_buffer/service/shader_translator.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_mock.h"

using ::testing::_;
using ::testing::ElementsAreArray;
using ::testing::Invoke;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;

namespace {
typedef gpu::gles2::ShaderTranslator::VariableMap VariableMap;
}  // anonymous namespace

namespace gpu {
namespace gles2 {

class ProgramBinaryEmulator {
 public:
  ProgramBinaryEmulator(GLsizei length,
                        GLenum format,
                        const char* binary)
      : length_(length),
        format_(format),
        binary_(binary) { }

  void GetProgramBinary(GLuint program,
                        GLsizei buffer_size,
                        GLsizei* length,
                        GLenum* format,
                        GLvoid* binary) {
    if (length) {
      *length = length_;
    }
    *format = format_;
    memcpy(binary, binary_, length_);
  }

  void ProgramBinary(GLuint program,
                     GLenum format,
                     const GLvoid* binary,
                     GLsizei length) {
    // format and length are verified by matcher
    EXPECT_EQ(0, memcmp(binary_, binary, length));
  }

  GLsizei length() const { return length_; }
  GLenum format() const { return format_; }
  const char* binary() const { return binary_; }

 private:
  GLsizei length_;
  GLenum format_;
  const char* binary_;
};

class MemoryProgramCacheTest : public GpuServiceTest {
 public:
  static const size_t kCacheSizeBytes = 1024;
  static const GLuint kVertexShaderClientId = 90;
  static const GLuint kVertexShaderServiceId = 100;
  static const GLuint kFragmentShaderClientId = 91;
  static const GLuint kFragmentShaderServiceId = 100;

  MemoryProgramCacheTest()
      : cache_(new MemoryProgramCache(kCacheSizeBytes)),
        vertex_shader_(NULL),
        fragment_shader_(NULL),
        shader_cache_count_(0) { }
  virtual ~MemoryProgramCacheTest() {
    shader_manager_.Destroy(false);
  }

  void ShaderCacheCb(const std::string& key, const std::string& shader) {
    shader_cache_count_++;
    shader_cache_shader_ = shader;
  }

  int32 shader_cache_count() { return shader_cache_count_; }
  const std::string& shader_cache_shader() { return shader_cache_shader_; }

 protected:
  virtual void SetUp() {
    GpuServiceTest::SetUp();

    vertex_shader_ = shader_manager_.CreateShader(kVertexShaderClientId,
                                                  kVertexShaderServiceId,
                                                  GL_VERTEX_SHADER);
    fragment_shader_ = shader_manager_.CreateShader(
        kFragmentShaderClientId,
        kFragmentShaderServiceId,
        GL_FRAGMENT_SHADER);
    ASSERT_TRUE(vertex_shader_ != NULL);
    ASSERT_TRUE(fragment_shader_ != NULL);
    typedef ShaderTranslatorInterface::VariableInfo VariableInfo;
    typedef ShaderTranslator::VariableMap VariableMap;
    VariableMap vertex_attrib_map;
    VariableMap vertex_uniform_map;
    VariableMap vertex_varying_map;
    VariableMap fragment_attrib_map;
    VariableMap fragment_uniform_map;
    VariableMap fragment_varying_map;

    vertex_attrib_map["a"] = VariableInfo(1, 34, SH_PRECISION_LOWP, 0, "a");
    vertex_uniform_map["a"] = VariableInfo(0, 10, SH_PRECISION_MEDIUMP, 1, "a");
    vertex_uniform_map["b"] = VariableInfo(2, 3114, SH_PRECISION_HIGHP, 1, "b");
    vertex_varying_map["c"] = VariableInfo(3, 2, SH_PRECISION_HIGHP, 1, "c");
    fragment_attrib_map["jjjbb"] =
        VariableInfo(463, 1114, SH_PRECISION_MEDIUMP, 0, "jjjbb");
    fragment_uniform_map["k"] =
        VariableInfo(10, 34413, SH_PRECISION_MEDIUMP, 1, "k");
    fragment_varying_map["c"] = VariableInfo(3, 2, SH_PRECISION_HIGHP, 1, "c");

    vertex_shader_->set_attrib_map(vertex_attrib_map);
    vertex_shader_->set_uniform_map(vertex_uniform_map);
    vertex_shader_->set_varying_map(vertex_varying_map);
    fragment_shader_->set_attrib_map(vertex_attrib_map);
    fragment_shader_->set_uniform_map(vertex_uniform_map);
    fragment_shader_->set_varying_map(vertex_varying_map);

    vertex_shader_->UpdateSource("bbbalsldkdkdkd");
    fragment_shader_->UpdateSource("bbbal   sldkdkdkas 134 ad");

    vertex_shader_->SetStatus(true, NULL, NULL);
    fragment_shader_->SetStatus(true, NULL, NULL);
  }

  void SetExpectationsForSaveLinkedProgram(
      const GLint program_id,
      ProgramBinaryEmulator* emulator) const {
    EXPECT_CALL(*gl_.get(),
                GetProgramiv(program_id, GL_PROGRAM_BINARY_LENGTH_OES, _))
        .WillOnce(SetArgPointee<2>(emulator->length()));
    EXPECT_CALL(*gl_.get(),
                GetProgramBinary(program_id, emulator->length(), _, _, _))
        .WillOnce(Invoke(emulator, &ProgramBinaryEmulator::GetProgramBinary));
  }

  void SetExpectationsForLoadLinkedProgram(
      const GLint program_id,
      ProgramBinaryEmulator* emulator) const {
    EXPECT_CALL(*gl_.get(),
                ProgramBinary(program_id,
                              emulator->format(),
                              _,
                              emulator->length()))
        .WillOnce(Invoke(emulator, &ProgramBinaryEmulator::ProgramBinary));
    EXPECT_CALL(*gl_.get(),
                GetProgramiv(program_id, GL_LINK_STATUS, _))
                .WillOnce(SetArgPointee<2>(GL_TRUE));
  }

  void SetExpectationsForLoadLinkedProgramFailure(
      const GLint program_id,
      ProgramBinaryEmulator* emulator) const {
    EXPECT_CALL(*gl_.get(),
                ProgramBinary(program_id,
                              emulator->format(),
                              _,
                              emulator->length()))
        .WillOnce(Invoke(emulator, &ProgramBinaryEmulator::ProgramBinary));
    EXPECT_CALL(*gl_.get(),
                GetProgramiv(program_id, GL_LINK_STATUS, _))
                .WillOnce(SetArgPointee<2>(GL_FALSE));
  }

  scoped_ptr<MemoryProgramCache> cache_;
  ShaderManager shader_manager_;
  Shader* vertex_shader_;
  Shader* fragment_shader_;
  int32 shader_cache_count_;
  std::string shader_cache_shader_;
};

TEST_F(MemoryProgramCacheTest, CacheSave) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator(kBinaryLength, kFormat, test_binary);

  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));

  EXPECT_EQ(ProgramCache::LINK_SUCCEEDED, cache_->GetLinkedProgramStatus(
      *vertex_shader_->signature_source(),
      NULL,
      *fragment_shader_->signature_source(),
      NULL,
      NULL));
  EXPECT_EQ(1, shader_cache_count());
}

TEST_F(MemoryProgramCacheTest, LoadProgram) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator(kBinaryLength, kFormat, test_binary);

  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));

  EXPECT_EQ(ProgramCache::LINK_SUCCEEDED, cache_->GetLinkedProgramStatus(
      *vertex_shader_->signature_source(),
      NULL,
      *fragment_shader_->signature_source(),
      NULL,
      NULL));
  EXPECT_EQ(1, shader_cache_count());

  cache_->Clear();

  cache_->LoadProgram(shader_cache_shader());
  EXPECT_EQ(ProgramCache::LINK_SUCCEEDED, cache_->GetLinkedProgramStatus(
      *vertex_shader_->signature_source(),
      NULL,
      *fragment_shader_->signature_source(),
      NULL,
      NULL));
}

TEST_F(MemoryProgramCacheTest, CacheLoadMatchesSave) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator(kBinaryLength, kFormat, test_binary);

  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));
  EXPECT_EQ(1, shader_cache_count());

  VariableMap vertex_attrib_map = vertex_shader_->attrib_map();
  VariableMap vertex_uniform_map = vertex_shader_->uniform_map();
  VariableMap vertex_varying_map = vertex_shader_->varying_map();
  VariableMap fragment_attrib_map = fragment_shader_->attrib_map();
  VariableMap fragment_uniform_map = fragment_shader_->uniform_map();
  VariableMap fragment_varying_map = fragment_shader_->varying_map();

  vertex_shader_->set_attrib_map(VariableMap());
  vertex_shader_->set_uniform_map(VariableMap());
  vertex_shader_->set_varying_map(VariableMap());
  fragment_shader_->set_attrib_map(VariableMap());
  fragment_shader_->set_uniform_map(VariableMap());
  fragment_shader_->set_varying_map(VariableMap());

  SetExpectationsForLoadLinkedProgram(kProgramId, &emulator);

  EXPECT_EQ(ProgramCache::PROGRAM_LOAD_SUCCESS, cache_->LoadLinkedProgram(
      kProgramId,
      vertex_shader_,
      NULL,
      fragment_shader_,
      NULL,
      NULL,
      base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                 base::Unretained(this))));

  // apparently the hash_map implementation on android doesn't have the
  // equality operator
#if !defined(OS_ANDROID)
  EXPECT_EQ(vertex_attrib_map, vertex_shader_->attrib_map());
  EXPECT_EQ(vertex_uniform_map, vertex_shader_->uniform_map());
  EXPECT_EQ(vertex_varying_map, vertex_shader_->varying_map());
  EXPECT_EQ(fragment_attrib_map, fragment_shader_->attrib_map());
  EXPECT_EQ(fragment_uniform_map, fragment_shader_->uniform_map());
  EXPECT_EQ(fragment_varying_map, fragment_shader_->varying_map());
#endif
}

TEST_F(MemoryProgramCacheTest, LoadProgramMatchesSave) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator(kBinaryLength, kFormat, test_binary);

  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));
  EXPECT_EQ(1, shader_cache_count());

  VariableMap vertex_attrib_map = vertex_shader_->attrib_map();
  VariableMap vertex_uniform_map = vertex_shader_->uniform_map();
  VariableMap vertex_varying_map = vertex_shader_->varying_map();
  VariableMap fragment_attrib_map = fragment_shader_->attrib_map();
  VariableMap fragment_uniform_map = fragment_shader_->uniform_map();
  VariableMap fragment_varying_map = fragment_shader_->varying_map();

  vertex_shader_->set_attrib_map(VariableMap());
  vertex_shader_->set_uniform_map(VariableMap());
  vertex_shader_->set_varying_map(VariableMap());
  fragment_shader_->set_attrib_map(VariableMap());
  fragment_shader_->set_uniform_map(VariableMap());
  fragment_shader_->set_varying_map(VariableMap());

  SetExpectationsForLoadLinkedProgram(kProgramId, &emulator);

  cache_->Clear();
  cache_->LoadProgram(shader_cache_shader());

  EXPECT_EQ(ProgramCache::PROGRAM_LOAD_SUCCESS, cache_->LoadLinkedProgram(
      kProgramId,
      vertex_shader_,
      NULL,
      fragment_shader_,
      NULL,
      NULL,
      base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                 base::Unretained(this))));

  // apparently the hash_map implementation on android doesn't have the
  // equality operator
#if !defined(OS_ANDROID)
  EXPECT_EQ(vertex_attrib_map, vertex_shader_->attrib_map());
  EXPECT_EQ(vertex_uniform_map, vertex_shader_->uniform_map());
  EXPECT_EQ(vertex_varying_map, vertex_shader_->varying_map());
  EXPECT_EQ(fragment_attrib_map, fragment_shader_->attrib_map());
  EXPECT_EQ(fragment_uniform_map, fragment_shader_->uniform_map());
  EXPECT_EQ(fragment_varying_map, fragment_shader_->varying_map());
#endif
}

TEST_F(MemoryProgramCacheTest, LoadFailOnLinkFalse) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator(kBinaryLength, kFormat, test_binary);

  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));

  SetExpectationsForLoadLinkedProgramFailure(kProgramId, &emulator);
  EXPECT_EQ(ProgramCache::PROGRAM_LOAD_FAILURE, cache_->LoadLinkedProgram(
      kProgramId,
      vertex_shader_,
      NULL,
      fragment_shader_,
      NULL,
      NULL,
      base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                 base::Unretained(this))));
}

TEST_F(MemoryProgramCacheTest, LoadFailOnDifferentSource) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator(kBinaryLength, kFormat, test_binary);

  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));

  const std::string vertex_orig_source =
      *vertex_shader_->signature_source();
  vertex_shader_->UpdateSource("different!");
  vertex_shader_->SetStatus(true, NULL, NULL);
  EXPECT_EQ(ProgramCache::PROGRAM_LOAD_FAILURE, cache_->LoadLinkedProgram(
      kProgramId,
      vertex_shader_,
      NULL,
      fragment_shader_,
      NULL,
      NULL,
      base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                 base::Unretained(this))));

  vertex_shader_->UpdateSource(vertex_orig_source.c_str());
  vertex_shader_->SetStatus(true, NULL, NULL);
  fragment_shader_->UpdateSource("different!");
  fragment_shader_->SetStatus(true, NULL, NULL);
  EXPECT_EQ(ProgramCache::PROGRAM_LOAD_FAILURE, cache_->LoadLinkedProgram(
      kProgramId,
      vertex_shader_,
      NULL,
      fragment_shader_,
      NULL,
      NULL,
      base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                 base::Unretained(this))));
}

TEST_F(MemoryProgramCacheTest, LoadFailOnDifferentMap) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator(kBinaryLength, kFormat, test_binary);

  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator);
  ProgramCache::LocationMap binding_map;
  binding_map["test"] = 512;
  cache_->SaveLinkedProgram(kProgramId,
                            vertex_shader_,
                            NULL,
                            fragment_shader_,
                            NULL,
                            &binding_map,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));

  binding_map["different!"] = 59;
  EXPECT_EQ(ProgramCache::PROGRAM_LOAD_FAILURE, cache_->LoadLinkedProgram(
      kProgramId,
      vertex_shader_,
      NULL,
      fragment_shader_,
      NULL,
      &binding_map,
      base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                 base::Unretained(this))));
  EXPECT_EQ(ProgramCache::PROGRAM_LOAD_FAILURE, cache_->LoadLinkedProgram(
      kProgramId,
      vertex_shader_,
      NULL,
      fragment_shader_,
      NULL,
      NULL,
      base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                 base::Unretained(this))));
}

TEST_F(MemoryProgramCacheTest, MemoryProgramCacheEviction) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator1(kBinaryLength, kFormat, test_binary);


  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator1);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));

  const int kEvictingProgramId = 11;
  const GLuint kEvictingBinaryLength = kCacheSizeBytes - kBinaryLength + 1;

  // save old source and modify for new program
  const std::string old_source =
      *fragment_shader_->signature_source();
  fragment_shader_->UpdateSource("al sdfkjdk");
  fragment_shader_->SetStatus(true, NULL, NULL);

  scoped_ptr<char[]> bigTestBinary =
      scoped_ptr<char[]>(new char[kEvictingBinaryLength]);
  for (size_t i = 0; i < kEvictingBinaryLength; ++i) {
    bigTestBinary[i] = i % 250;
  }
  ProgramBinaryEmulator emulator2(kEvictingBinaryLength,
                                  kFormat,
                                  bigTestBinary.get());

  SetExpectationsForSaveLinkedProgram(kEvictingProgramId, &emulator2);
  cache_->SaveLinkedProgram(kEvictingProgramId,
                            vertex_shader_,
                            NULL,
                            fragment_shader_,
                            NULL,
                            NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));

  EXPECT_EQ(ProgramCache::LINK_SUCCEEDED, cache_->GetLinkedProgramStatus(
      *vertex_shader_->signature_source(),
      NULL,
      *fragment_shader_->signature_source(),
      NULL,
      NULL));
  EXPECT_EQ(ProgramCache::LINK_UNKNOWN, cache_->GetLinkedProgramStatus(
      old_source,
      NULL,
      *fragment_shader_->signature_source(),
      NULL,
      NULL));
}

TEST_F(MemoryProgramCacheTest, SaveCorrectProgram) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator1(kBinaryLength, kFormat, test_binary);

  vertex_shader_->UpdateSource("different!");
  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator1);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));

  EXPECT_EQ(ProgramCache::LINK_SUCCEEDED, cache_->GetLinkedProgramStatus(
      *vertex_shader_->signature_source(),
      NULL,
      *fragment_shader_->signature_source(),
      NULL,
      NULL));
}

TEST_F(MemoryProgramCacheTest, LoadCorrectProgram) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator(kBinaryLength, kFormat, test_binary);

  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));

  EXPECT_EQ(ProgramCache::LINK_SUCCEEDED, cache_->GetLinkedProgramStatus(
      *vertex_shader_->signature_source(),
      NULL,
      *fragment_shader_->signature_source(),
      NULL,
      NULL));

  SetExpectationsForLoadLinkedProgram(kProgramId, &emulator);

  fragment_shader_->UpdateSource("different!");
  EXPECT_EQ(ProgramCache::PROGRAM_LOAD_SUCCESS, cache_->LoadLinkedProgram(
      kProgramId,
      vertex_shader_,
      NULL,
      fragment_shader_,
      NULL,
      NULL,
      base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                 base::Unretained(this))));
}

TEST_F(MemoryProgramCacheTest, OverwriteOnNewSave) {
  const GLenum kFormat = 1;
  const int kProgramId = 10;
  const int kBinaryLength = 20;
  char test_binary[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary[i] = i;
  }
  ProgramBinaryEmulator emulator(kBinaryLength, kFormat, test_binary);

  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));


  char test_binary2[kBinaryLength];
  for (int i = 0; i < kBinaryLength; ++i) {
    test_binary2[i] = (i*2) % 250;
  }
  ProgramBinaryEmulator emulator2(kBinaryLength, kFormat, test_binary2);
  SetExpectationsForSaveLinkedProgram(kProgramId, &emulator2);
  cache_->SaveLinkedProgram(kProgramId, vertex_shader_, NULL,
                            fragment_shader_, NULL, NULL,
                            base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                                       base::Unretained(this)));

  SetExpectationsForLoadLinkedProgram(kProgramId, &emulator2);
  EXPECT_EQ(ProgramCache::PROGRAM_LOAD_SUCCESS, cache_->LoadLinkedProgram(
      kProgramId,
      vertex_shader_,
      NULL,
      fragment_shader_,
      NULL,
      NULL,
      base::Bind(&MemoryProgramCacheTest::ShaderCacheCb,
                 base::Unretained(this))));
}

}  // namespace gles2
}  // namespace gpu
