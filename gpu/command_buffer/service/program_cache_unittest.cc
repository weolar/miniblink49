// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/program_cache.h"

#include "base/memory/scoped_ptr.h"
#include "gpu/command_buffer/service/mocks.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::Return;

namespace gpu {
namespace gles2 {

class NoBackendProgramCache : public ProgramCache {
 public:
  virtual ProgramLoadResult LoadLinkedProgram(
      GLuint /* program */,
      Shader* /* shader_a */,
      const ShaderTranslatorInterface* /* translator_a */,
      Shader* /* shader_b */,
      const ShaderTranslatorInterface* /* translator_b */,
      const LocationMap* /* bind_attrib_location_map */,
      const ShaderCacheCallback& /* callback */) OVERRIDE {
    return PROGRAM_LOAD_SUCCESS;
  }
  virtual void SaveLinkedProgram(
      GLuint /* program */,
      const Shader* /* shader_a */,
      const ShaderTranslatorInterface* /* translator_b */,
      const Shader* /* shader_b */,
      const ShaderTranslatorInterface* /* translator_b */,
      const LocationMap* /* bind_attrib_location_map */,
      const ShaderCacheCallback& /* callback */) OVERRIDE { }

  virtual void LoadProgram(const std::string& /* program */) OVERRIDE {}

  virtual void ClearBackend() OVERRIDE {}

  void SaySuccessfullyCached(const std::string& shader1,
                             const ShaderTranslatorInterface* translator_1,
                             const std::string& shader2,
                             const ShaderTranslatorInterface* translator_2,
                             std::map<std::string, GLint>* attrib_map) {
    char a_sha[kHashLength];
    char b_sha[kHashLength];
    ComputeShaderHash(shader1, translator_1, a_sha);
    ComputeShaderHash(shader2, translator_2, b_sha);

    char sha[kHashLength];
    ComputeProgramHash(a_sha,
                       b_sha,
                       attrib_map,
                       sha);
    const std::string shaString(sha, kHashLength);

    LinkedProgramCacheSuccess(shaString);
  }

  void ComputeShaderHash(const std::string& shader,
                         const ShaderTranslatorInterface* translator,
                         char* result) const {
    ProgramCache::ComputeShaderHash(shader, translator, result);
  }

  void ComputeProgramHash(const char* hashed_shader_0,
                          const char* hashed_shader_1,
                          const LocationMap* bind_attrib_location_map,
                          char* result) const {
    ProgramCache::ComputeProgramHash(hashed_shader_0,
                                     hashed_shader_1,
                                     bind_attrib_location_map,
                                     result);
  }

  void Evict(const std::string& program_hash) {
    ProgramCache::Evict(program_hash);
  }
};

class ProgramCacheTest : public testing::Test {
 public:
  ProgramCacheTest() :
    cache_(new NoBackendProgramCache()) { }

 protected:
  scoped_ptr<NoBackendProgramCache> cache_;
};

TEST_F(ProgramCacheTest, LinkStatusSave) {
  const std::string shader1 = "abcd1234";
  const std::string shader2 = "abcda sda b1~#4 bbbbb1234";
  {
    std::string shader_a = shader1;
    std::string shader_b = shader2;
    EXPECT_EQ(ProgramCache::LINK_UNKNOWN,
              cache_->GetLinkedProgramStatus(
                  shader_a, NULL, shader_b, NULL, NULL));
    cache_->SaySuccessfullyCached(shader_a, NULL, shader_b, NULL, NULL);

    shader_a.clear();
    shader_b.clear();
  }
  // make sure it was copied
  EXPECT_EQ(ProgramCache::LINK_SUCCEEDED,
            cache_->GetLinkedProgramStatus(
                shader1, NULL, shader2, NULL, NULL));
}

TEST_F(ProgramCacheTest, LinkUnknownOnFragmentSourceChange) {
  const std::string shader1 = "abcd1234";
  std::string shader2 = "abcda sda b1~#4 bbbbb1234";
  cache_->SaySuccessfullyCached(shader1, NULL, shader2, NULL, NULL);

  shader2 = "different!";
  EXPECT_EQ(ProgramCache::LINK_UNKNOWN,
            cache_->GetLinkedProgramStatus(shader1, NULL, shader2, NULL, NULL));
}

TEST_F(ProgramCacheTest, LinkUnknownOnVertexSourceChange) {
  std::string shader1 = "abcd1234";
  const std::string shader2 = "abcda sda b1~#4 bbbbb1234";
  cache_->SaySuccessfullyCached(shader1, NULL, shader2, NULL, NULL);

  shader1 = "different!";
  EXPECT_EQ(ProgramCache::LINK_UNKNOWN,
            cache_->GetLinkedProgramStatus(shader1, NULL, shader2, NULL, NULL));
}

TEST_F(ProgramCacheTest, StatusEviction) {
  const std::string shader1 = "abcd1234";
  const std::string shader2 = "abcda sda b1~#4 bbbbb1234";
  cache_->SaySuccessfullyCached(shader1, NULL, shader2, NULL, NULL);
  char a_sha[ProgramCache::kHashLength];
  char b_sha[ProgramCache::kHashLength];
  cache_->ComputeShaderHash(shader1, NULL, a_sha);
  cache_->ComputeShaderHash(shader2, NULL, b_sha);

  char sha[ProgramCache::kHashLength];
  cache_->ComputeProgramHash(a_sha,
                             b_sha,
                             NULL,
                             sha);
  cache_->Evict(std::string(sha, ProgramCache::kHashLength));
  EXPECT_EQ(ProgramCache::LINK_UNKNOWN,
            cache_->GetLinkedProgramStatus(shader1, NULL, shader2, NULL, NULL));
}

TEST_F(ProgramCacheTest, EvictionWithReusedShader) {
  const std::string shader1 = "abcd1234";
  const std::string shader2 = "abcda sda b1~#4 bbbbb1234";
  const std::string shader3 = "asbjbbjj239a";
  cache_->SaySuccessfullyCached(shader1, NULL, shader2, NULL, NULL);
  cache_->SaySuccessfullyCached(shader1, NULL, shader3, NULL, NULL);

  char a_sha[ProgramCache::kHashLength];
  char b_sha[ProgramCache::kHashLength];
  char c_sha[ProgramCache::kHashLength];
  cache_->ComputeShaderHash(shader1, NULL, a_sha);
  cache_->ComputeShaderHash(shader2, NULL, b_sha);
  cache_->ComputeShaderHash(shader3, NULL, c_sha);

  char sha[ProgramCache::kHashLength];
  cache_->ComputeProgramHash(a_sha,
                             b_sha,
                             NULL,
                             sha);
  cache_->Evict(std::string(sha, ProgramCache::kHashLength));
  EXPECT_EQ(ProgramCache::LINK_UNKNOWN,
            cache_->GetLinkedProgramStatus(shader1, NULL, shader2, NULL, NULL));
  EXPECT_EQ(ProgramCache::LINK_SUCCEEDED,
            cache_->GetLinkedProgramStatus(shader1, NULL, shader3, NULL, NULL));


  cache_->ComputeProgramHash(a_sha,
                             c_sha,
                             NULL,
                             sha);
  cache_->Evict(std::string(sha, ProgramCache::kHashLength));
  EXPECT_EQ(ProgramCache::LINK_UNKNOWN,
            cache_->GetLinkedProgramStatus(shader1, NULL, shader2, NULL, NULL));
  EXPECT_EQ(ProgramCache::LINK_UNKNOWN,
            cache_->GetLinkedProgramStatus(shader1, NULL, shader3, NULL, NULL));
}

TEST_F(ProgramCacheTest, StatusClear) {
  const std::string shader1 = "abcd1234";
  const std::string shader2 = "abcda sda b1~#4 bbbbb1234";
  const std::string shader3 = "asbjbbjj239a";
  cache_->SaySuccessfullyCached(shader1, NULL, shader2, NULL, NULL);
  cache_->SaySuccessfullyCached(shader1, NULL, shader3, NULL, NULL);
  cache_->Clear();
  EXPECT_EQ(ProgramCache::LINK_UNKNOWN,
            cache_->GetLinkedProgramStatus(shader1, NULL, shader2, NULL, NULL));
  EXPECT_EQ(ProgramCache::LINK_UNKNOWN,
            cache_->GetLinkedProgramStatus(shader1, NULL, shader3, NULL, NULL));
}

}  // namespace gles2
}  // namespace gpu
