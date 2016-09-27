// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/program_manager.h"

#include <algorithm>

#include "base/memory/scoped_ptr.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "gpu/command_buffer/common/gles2_cmd_format.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/service/common_decoder.h"
#include "gpu/command_buffer/service/feature_info.h"
#include "gpu/command_buffer/service/gpu_service_test.h"
#include "gpu/command_buffer/service/mocks.h"
#include "gpu/command_buffer/service/shader_manager.h"
#include "gpu/command_buffer/service/test_helper.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_mock.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::MatcherCast;
using ::testing::Pointee;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SetArrayArgument;
using ::testing::SetArgumentPointee;
using ::testing::StrEq;

namespace gpu {
namespace gles2 {

namespace {
const uint32 kMaxVaryingVectors = 8;

void ShaderCacheCb(const std::string& key, const std::string& shader) {}
}  // namespace anonymous

class ProgramManagerTest : public GpuServiceTest {
 public:
  ProgramManagerTest() : manager_(NULL, kMaxVaryingVectors) { }
  virtual ~ProgramManagerTest() {
    manager_.Destroy(false);
  }

 protected:
  ProgramManager manager_;
};

TEST_F(ProgramManagerTest, Basic) {
  const GLuint kClient1Id = 1;
  const GLuint kService1Id = 11;
  const GLuint kClient2Id = 2;
  // Check we can create program.
  manager_.CreateProgram(kClient1Id, kService1Id);
  // Check program got created.
  Program* program1 = manager_.GetProgram(kClient1Id);
  ASSERT_TRUE(program1 != NULL);
  GLuint client_id = 0;
  EXPECT_TRUE(manager_.GetClientId(program1->service_id(), &client_id));
  EXPECT_EQ(kClient1Id, client_id);
  // Check we get nothing for a non-existent program.
  EXPECT_TRUE(manager_.GetProgram(kClient2Id) == NULL);
}

TEST_F(ProgramManagerTest, Destroy) {
  const GLuint kClient1Id = 1;
  const GLuint kService1Id = 11;
  // Check we can create program.
  Program* program0 = manager_.CreateProgram(kClient1Id, kService1Id);
  ASSERT_TRUE(program0 != NULL);
  // Check program got created.
  Program* program1 = manager_.GetProgram(kClient1Id);
  ASSERT_EQ(program0, program1);
  EXPECT_CALL(*gl_, DeleteProgram(kService1Id))
      .Times(1)
      .RetiresOnSaturation();
  manager_.Destroy(true);
  // Check the resources were released.
  program1 = manager_.GetProgram(kClient1Id);
  ASSERT_TRUE(program1 == NULL);
}

TEST_F(ProgramManagerTest, DeleteBug) {
  ShaderManager shader_manager;
  const GLuint kClient1Id = 1;
  const GLuint kClient2Id = 2;
  const GLuint kService1Id = 11;
  const GLuint kService2Id = 12;
  // Check we can create program.
  scoped_refptr<Program> program1(
      manager_.CreateProgram(kClient1Id, kService1Id));
  scoped_refptr<Program> program2(
      manager_.CreateProgram(kClient2Id, kService2Id));
  // Check program got created.
  ASSERT_TRUE(program1.get());
  ASSERT_TRUE(program2.get());
  manager_.UseProgram(program1.get());
  manager_.MarkAsDeleted(&shader_manager, program1.get());
  //  Program will be deleted when last ref is released.
  EXPECT_CALL(*gl_, DeleteProgram(kService2Id))
      .Times(1)
      .RetiresOnSaturation();
  manager_.MarkAsDeleted(&shader_manager, program2.get());
  EXPECT_TRUE(manager_.IsOwned(program1.get()));
  EXPECT_FALSE(manager_.IsOwned(program2.get()));
}

TEST_F(ProgramManagerTest, Program) {
  const GLuint kClient1Id = 1;
  const GLuint kService1Id = 11;
  // Check we can create program.
  Program* program1 = manager_.CreateProgram(
      kClient1Id, kService1Id);
  ASSERT_TRUE(program1);
  EXPECT_EQ(kService1Id, program1->service_id());
  EXPECT_FALSE(program1->InUse());
  EXPECT_FALSE(program1->IsValid());
  EXPECT_FALSE(program1->IsDeleted());
  EXPECT_FALSE(program1->CanLink());
  EXPECT_TRUE(program1->log_info() == NULL);
}

class ProgramManagerWithShaderTest : public GpuServiceTest {
 public:
  ProgramManagerWithShaderTest()
      :  manager_(NULL, kMaxVaryingVectors), program_(NULL) {
  }

  virtual ~ProgramManagerWithShaderTest() {
    manager_.Destroy(false);
    shader_manager_.Destroy(false);
  }

  static const GLint kNumVertexAttribs = 16;

  static const GLuint kClientProgramId = 123;
  static const GLuint kServiceProgramId = 456;
  static const GLuint kVertexShaderClientId = 201;
  static const GLuint kFragmentShaderClientId = 202;
  static const GLuint kVertexShaderServiceId = 301;
  static const GLuint kFragmentShaderServiceId = 302;

  static const char* kAttrib1Name;
  static const char* kAttrib2Name;
  static const char* kAttrib3Name;
  static const GLint kAttrib1Size = 1;
  static const GLint kAttrib2Size = 1;
  static const GLint kAttrib3Size = 1;
  static const int kAttrib1Precision = SH_PRECISION_MEDIUMP;
  static const int kAttrib2Precision = SH_PRECISION_HIGHP;
  static const int kAttrib3Precision = SH_PRECISION_LOWP;
  static const int kAttribStaticUse = 0;
  static const GLint kAttrib1Location = 0;
  static const GLint kAttrib2Location = 1;
  static const GLint kAttrib3Location = 2;
  static const GLenum kAttrib1Type = GL_FLOAT_VEC4;
  static const GLenum kAttrib2Type = GL_FLOAT_VEC2;
  static const GLenum kAttrib3Type = GL_FLOAT_VEC3;
  static const GLint kInvalidAttribLocation = 30;
  static const GLint kBadAttribIndex = kNumVertexAttribs;

  static const char* kUniform1Name;
  static const char* kUniform2Name;
  static const char* kUniform3BadName;
  static const char* kUniform3GoodName;
  static const GLint kUniform1Size = 1;
  static const GLint kUniform2Size = 3;
  static const GLint kUniform3Size = 2;
  static const int kUniform1Precision = SH_PRECISION_LOWP;
  static const int kUniform2Precision = SH_PRECISION_MEDIUMP;
  static const int kUniform3Precision = SH_PRECISION_HIGHP;
  static const int kUniform1StaticUse = 1;
  static const int kUniform2StaticUse = 1;
  static const int kUniform3StaticUse = 1;
  static const GLint kUniform1FakeLocation = 0;  // These are hard coded
  static const GLint kUniform2FakeLocation = 1;  // to match
  static const GLint kUniform3FakeLocation = 2;  // ProgramManager.
  static const GLint kUniform1RealLocation = 11;
  static const GLint kUniform2RealLocation = 22;
  static const GLint kUniform3RealLocation = 33;
  static const GLint kUniform1DesiredLocation = -1;
  static const GLint kUniform2DesiredLocation = -1;
  static const GLint kUniform3DesiredLocation = -1;
  static const GLenum kUniform1Type = GL_FLOAT_VEC4;
  static const GLenum kUniform2Type = GL_INT_VEC2;
  static const GLenum kUniform3Type = GL_FLOAT_VEC3;
  static const GLint kInvalidUniformLocation = 30;
  static const GLint kBadUniformIndex = 1000;

  static const size_t kNumAttribs;
  static const size_t kNumUniforms;

 protected:
  typedef TestHelper::AttribInfo AttribInfo;
  typedef TestHelper::UniformInfo UniformInfo;

  typedef enum {
    kVarUniform,
    kVarVarying,
    kVarAttribute
  } VarCategory;

  typedef struct {
    int type;
    int size;
    int precision;
    int static_use;
    std::string name;
    VarCategory category;
  } VarInfo;

  virtual void SetUp() {
    GpuServiceTest::SetUp();

    SetupDefaultShaderExpectations();

    Shader* vertex_shader = shader_manager_.CreateShader(
        kVertexShaderClientId, kVertexShaderServiceId, GL_VERTEX_SHADER);
    Shader* fragment_shader =
        shader_manager_.CreateShader(
            kFragmentShaderClientId, kFragmentShaderServiceId,
            GL_FRAGMENT_SHADER);
    ASSERT_TRUE(vertex_shader != NULL);
    ASSERT_TRUE(fragment_shader != NULL);
    vertex_shader->SetStatus(true, NULL, NULL);
    fragment_shader->SetStatus(true, NULL, NULL);

    program_ = manager_.CreateProgram(
        kClientProgramId, kServiceProgramId);
    ASSERT_TRUE(program_ != NULL);

    program_->AttachShader(&shader_manager_, vertex_shader);
    program_->AttachShader(&shader_manager_, fragment_shader);
    program_->Link(NULL, NULL, NULL, Program::kCountOnlyStaticallyUsed,
                   base::Bind(&ShaderCacheCb));
  }

  void SetupShader(AttribInfo* attribs, size_t num_attribs,
                   UniformInfo* uniforms, size_t num_uniforms,
                   GLuint service_id) {
    TestHelper::SetupShader(
        gl_.get(), attribs, num_attribs, uniforms, num_uniforms, service_id);
  }

  void SetupDefaultShaderExpectations() {
    SetupShader(kAttribs, kNumAttribs, kUniforms, kNumUniforms,
                kServiceProgramId);
  }

  void SetupExpectationsForClearingUniforms(
      UniformInfo* uniforms, size_t num_uniforms) {
    TestHelper::SetupExpectationsForClearingUniforms(
        gl_.get(), uniforms, num_uniforms);
  }

  // Return true if link status matches expected_link_status
  bool LinkAsExpected(Program* program,
                      bool expected_link_status) {
    GLuint service_id = program->service_id();
    if (expected_link_status) {
      SetupShader(kAttribs, kNumAttribs, kUniforms, kNumUniforms,
                  service_id);
    }
    program->Link(NULL, NULL, NULL, Program::kCountOnlyStaticallyUsed,
                  base::Bind(&ShaderCacheCb));
    GLint link_status;
    program->GetProgramiv(GL_LINK_STATUS, &link_status);
    return (static_cast<bool>(link_status) == expected_link_status);
  }

  Program* SetupShaderVariableTest(const VarInfo* vertex_variables,
                                   size_t vertex_variable_size,
                                   const VarInfo* fragment_variables,
                                   size_t fragment_variable_size) {
    // Set up shader
    const GLuint kVShaderClientId = 1;
    const GLuint kVShaderServiceId = 11;
    const GLuint kFShaderClientId = 2;
    const GLuint kFShaderServiceId = 12;

    MockShaderTranslator vertex_shader_translator;
    ShaderTranslator::VariableMap vertex_attrib_map;
    ShaderTranslator::VariableMap vertex_uniform_map;
    ShaderTranslator::VariableMap vertex_varying_map;
    for (size_t ii = 0; ii < vertex_variable_size; ++ii) {
      ShaderTranslator::VariableMap* map = NULL;
      switch (vertex_variables[ii].category) {
        case kVarAttribute:
          map = &vertex_attrib_map;
          break;
        case kVarUniform:
          map = &vertex_uniform_map;
          break;
        case kVarVarying:
          map = &vertex_varying_map;
          break;
        default:
          NOTREACHED();
      }
      (*map)[vertex_variables[ii].name] =
          ShaderTranslator::VariableInfo(vertex_variables[ii].type,
                                         vertex_variables[ii].size,
                                         vertex_variables[ii].precision,
                                         vertex_variables[ii].static_use,
                                         vertex_variables[ii].name);
    }
    ShaderTranslator::NameMap vertex_name_map;
    EXPECT_CALL(vertex_shader_translator, attrib_map())
        .WillRepeatedly(ReturnRef(vertex_attrib_map));
    EXPECT_CALL(vertex_shader_translator, uniform_map())
        .WillRepeatedly(ReturnRef(vertex_uniform_map));
    EXPECT_CALL(vertex_shader_translator, varying_map())
        .WillRepeatedly(ReturnRef(vertex_varying_map));
    EXPECT_CALL(vertex_shader_translator, name_map())
      .WillRepeatedly(ReturnRef(vertex_name_map));

    MockShaderTranslator frag_shader_translator;
    ShaderTranslator::VariableMap frag_attrib_map;
    ShaderTranslator::VariableMap frag_uniform_map;
    ShaderTranslator::VariableMap frag_varying_map;
    for (size_t ii = 0; ii < fragment_variable_size; ++ii) {
      ShaderTranslator::VariableMap* map = NULL;
      switch (fragment_variables[ii].category) {
        case kVarAttribute:
          map = &frag_attrib_map;
          break;
        case kVarUniform:
          map = &frag_uniform_map;
          break;
        case kVarVarying:
          map = &frag_varying_map;
          break;
        default:
          NOTREACHED();
      }
      (*map)[fragment_variables[ii].name] =
          ShaderTranslator::VariableInfo(fragment_variables[ii].type,
                                         fragment_variables[ii].size,
                                         fragment_variables[ii].precision,
                                         fragment_variables[ii].static_use,
                                         fragment_variables[ii].name);
    }
    ShaderTranslator::NameMap frag_name_map;
    EXPECT_CALL(frag_shader_translator, attrib_map())
        .WillRepeatedly(ReturnRef(frag_attrib_map));
    EXPECT_CALL(frag_shader_translator, uniform_map())
        .WillRepeatedly(ReturnRef(frag_uniform_map));
    EXPECT_CALL(frag_shader_translator, varying_map())
        .WillRepeatedly(ReturnRef(frag_varying_map));
    EXPECT_CALL(frag_shader_translator, name_map())
      .WillRepeatedly(ReturnRef(frag_name_map));

    // Check we can create shader.
    Shader* vshader = shader_manager_.CreateShader(
        kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
    Shader* fshader = shader_manager_.CreateShader(
        kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
    // Check shader got created.
    EXPECT_TRUE(vshader != NULL && fshader != NULL);
    // Set Status
    vshader->SetStatus(true, "", &vertex_shader_translator);
    fshader->SetStatus(true, "", &frag_shader_translator);

    // Set up program
    const GLuint kClientProgramId = 6666;
    const GLuint kServiceProgramId = 8888;
    Program* program =
        manager_.CreateProgram(kClientProgramId, kServiceProgramId);
    EXPECT_TRUE(program != NULL);
    EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
    EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));
    return program;
  }

  static AttribInfo kAttribs[];
  static UniformInfo kUniforms[];

  ProgramManager manager_;
  Program* program_;
  ShaderManager shader_manager_;
};

ProgramManagerWithShaderTest::AttribInfo
    ProgramManagerWithShaderTest::kAttribs[] = {
  { kAttrib1Name, kAttrib1Size, kAttrib1Type, kAttrib1Location, },
  { kAttrib2Name, kAttrib2Size, kAttrib2Type, kAttrib2Location, },
  { kAttrib3Name, kAttrib3Size, kAttrib3Type, kAttrib3Location, },
};

// GCC requires these declarations, but MSVC requires they not be present
#ifndef COMPILER_MSVC
const GLint ProgramManagerWithShaderTest::kNumVertexAttribs;
const GLuint ProgramManagerWithShaderTest::kClientProgramId;
const GLuint ProgramManagerWithShaderTest::kServiceProgramId;
const GLuint ProgramManagerWithShaderTest::kVertexShaderClientId;
const GLuint ProgramManagerWithShaderTest::kFragmentShaderClientId;
const GLuint ProgramManagerWithShaderTest::kVertexShaderServiceId;
const GLuint ProgramManagerWithShaderTest::kFragmentShaderServiceId;
const GLint ProgramManagerWithShaderTest::kAttrib1Size;
const GLint ProgramManagerWithShaderTest::kAttrib2Size;
const GLint ProgramManagerWithShaderTest::kAttrib3Size;
const GLint ProgramManagerWithShaderTest::kAttrib1Location;
const GLint ProgramManagerWithShaderTest::kAttrib2Location;
const GLint ProgramManagerWithShaderTest::kAttrib3Location;
const GLenum ProgramManagerWithShaderTest::kAttrib1Type;
const GLenum ProgramManagerWithShaderTest::kAttrib2Type;
const GLenum ProgramManagerWithShaderTest::kAttrib3Type;
const GLint ProgramManagerWithShaderTest::kInvalidAttribLocation;
const GLint ProgramManagerWithShaderTest::kBadAttribIndex;
const GLint ProgramManagerWithShaderTest::kUniform1Size;
const GLint ProgramManagerWithShaderTest::kUniform2Size;
const GLint ProgramManagerWithShaderTest::kUniform3Size;
const GLint ProgramManagerWithShaderTest::kUniform1FakeLocation;
const GLint ProgramManagerWithShaderTest::kUniform2FakeLocation;
const GLint ProgramManagerWithShaderTest::kUniform3FakeLocation;
const GLint ProgramManagerWithShaderTest::kUniform1RealLocation;
const GLint ProgramManagerWithShaderTest::kUniform2RealLocation;
const GLint ProgramManagerWithShaderTest::kUniform3RealLocation;
const GLint ProgramManagerWithShaderTest::kUniform1DesiredLocation;
const GLint ProgramManagerWithShaderTest::kUniform2DesiredLocation;
const GLint ProgramManagerWithShaderTest::kUniform3DesiredLocation;
const GLenum ProgramManagerWithShaderTest::kUniform1Type;
const GLenum ProgramManagerWithShaderTest::kUniform2Type;
const GLenum ProgramManagerWithShaderTest::kUniform3Type;
const GLint ProgramManagerWithShaderTest::kInvalidUniformLocation;
const GLint ProgramManagerWithShaderTest::kBadUniformIndex;
#endif

const size_t ProgramManagerWithShaderTest::kNumAttribs =
    arraysize(ProgramManagerWithShaderTest::kAttribs);

ProgramManagerWithShaderTest::UniformInfo
    ProgramManagerWithShaderTest::kUniforms[] = {
  { kUniform1Name,
    kUniform1Size,
    kUniform1Type,
    kUniform1FakeLocation,
    kUniform1RealLocation,
    kUniform1DesiredLocation,
    kUniform1Name,
  },
  { kUniform2Name,
    kUniform2Size,
    kUniform2Type,
    kUniform2FakeLocation,
    kUniform2RealLocation,
    kUniform2DesiredLocation,
    kUniform2Name,
  },
  { kUniform3BadName,
    kUniform3Size,
    kUniform3Type,
    kUniform3FakeLocation,
    kUniform3RealLocation,
    kUniform3DesiredLocation,
    kUniform3GoodName,
  },
};

const size_t ProgramManagerWithShaderTest::kNumUniforms =
    arraysize(ProgramManagerWithShaderTest::kUniforms);

const char* ProgramManagerWithShaderTest::kAttrib1Name = "attrib1";
const char* ProgramManagerWithShaderTest::kAttrib2Name = "attrib2";
const char* ProgramManagerWithShaderTest::kAttrib3Name = "attrib3";
const char* ProgramManagerWithShaderTest::kUniform1Name = "uniform1";
// Correctly has array spec.
const char* ProgramManagerWithShaderTest::kUniform2Name = "uniform2[0]";
// Incorrectly missing array spec.
const char* ProgramManagerWithShaderTest::kUniform3BadName = "uniform3";
const char* ProgramManagerWithShaderTest::kUniform3GoodName = "uniform3[0]";

TEST_F(ProgramManagerWithShaderTest, GetAttribInfos) {
  const Program* program = manager_.GetProgram(kClientProgramId);
  ASSERT_TRUE(program != NULL);
  const Program::AttribInfoVector& infos =
      program->GetAttribInfos();
  ASSERT_EQ(kNumAttribs, infos.size());
  for (size_t ii = 0; ii < kNumAttribs; ++ii) {
    const Program::VertexAttrib& info = infos[ii];
    const AttribInfo& expected = kAttribs[ii];
    EXPECT_EQ(expected.size, info.size);
    EXPECT_EQ(expected.type, info.type);
    EXPECT_EQ(expected.location, info.location);
    EXPECT_STREQ(expected.name, info.name.c_str());
  }
}

TEST_F(ProgramManagerWithShaderTest, GetAttribInfo) {
  const GLint kValidIndex = 1;
  const GLint kInvalidIndex = 1000;
  const Program* program = manager_.GetProgram(kClientProgramId);
  ASSERT_TRUE(program != NULL);
  const Program::VertexAttrib* info =
      program->GetAttribInfo(kValidIndex);
  ASSERT_TRUE(info != NULL);
  EXPECT_EQ(kAttrib2Size, info->size);
  EXPECT_EQ(kAttrib2Type, info->type);
  EXPECT_EQ(kAttrib2Location, info->location);
  EXPECT_STREQ(kAttrib2Name, info->name.c_str());
  EXPECT_TRUE(program->GetAttribInfo(kInvalidIndex) == NULL);
}

TEST_F(ProgramManagerWithShaderTest, GetAttribLocation) {
  const char* kInvalidName = "foo";
  const Program* program = manager_.GetProgram(kClientProgramId);
  ASSERT_TRUE(program != NULL);
  EXPECT_EQ(kAttrib2Location, program->GetAttribLocation(kAttrib2Name));
  EXPECT_EQ(-1, program->GetAttribLocation(kInvalidName));
}

TEST_F(ProgramManagerWithShaderTest, GetUniformInfo) {
  const GLint kInvalidIndex = 1000;
  const Program* program = manager_.GetProgram(kClientProgramId);
  ASSERT_TRUE(program != NULL);
  const Program::UniformInfo* info =
      program->GetUniformInfo(0);
  ASSERT_TRUE(info != NULL);
  EXPECT_EQ(kUniform1Size, info->size);
  EXPECT_EQ(kUniform1Type, info->type);
  EXPECT_EQ(kUniform1RealLocation, info->element_locations[0]);
  EXPECT_STREQ(kUniform1Name, info->name.c_str());
  info = program->GetUniformInfo(1);
  ASSERT_TRUE(info != NULL);
  EXPECT_EQ(kUniform2Size, info->size);
  EXPECT_EQ(kUniform2Type, info->type);
  EXPECT_EQ(kUniform2RealLocation, info->element_locations[0]);
  EXPECT_STREQ(kUniform2Name, info->name.c_str());
  info = program->GetUniformInfo(2);
  // We emulate certain OpenGL drivers by supplying the name without
  // the array spec. Our implementation should correctly add the required spec.
  ASSERT_TRUE(info != NULL);
  EXPECT_EQ(kUniform3Size, info->size);
  EXPECT_EQ(kUniform3Type, info->type);
  EXPECT_EQ(kUniform3RealLocation, info->element_locations[0]);
  EXPECT_STREQ(kUniform3GoodName, info->name.c_str());
  EXPECT_TRUE(program->GetUniformInfo(kInvalidIndex) == NULL);
}

TEST_F(ProgramManagerWithShaderTest, AttachDetachShader) {
  static const GLuint kClientProgramId = 124;
  static const GLuint kServiceProgramId = 457;
  Program* program = manager_.CreateProgram(
      kClientProgramId, kServiceProgramId);
  ASSERT_TRUE(program != NULL);
  EXPECT_FALSE(program->CanLink());
  const GLuint kVShaderClientId = 2001;
  const GLuint kFShaderClientId = 2002;
  const GLuint kVShaderServiceId = 3001;
  const GLuint kFShaderServiceId = 3002;
  Shader* vshader = shader_manager_.CreateShader(
      kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
  ASSERT_TRUE(vshader != NULL);
  vshader->SetStatus(true, "", NULL);
  Shader* fshader = shader_manager_.CreateShader(
      kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
  ASSERT_TRUE(fshader != NULL);
  fshader->SetStatus(true, "", NULL);
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_FALSE(program->CanLink());
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));
  EXPECT_TRUE(program->CanLink());
  program->DetachShader(&shader_manager_, vshader);
  EXPECT_FALSE(program->CanLink());
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_TRUE(program->CanLink());
  program->DetachShader(&shader_manager_, fshader);
  EXPECT_FALSE(program->CanLink());
  EXPECT_FALSE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_FALSE(program->CanLink());
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));
  EXPECT_TRUE(program->CanLink());
  vshader->SetStatus(false, "", NULL);
  EXPECT_FALSE(program->CanLink());
  vshader->SetStatus(true, "", NULL);
  EXPECT_TRUE(program->CanLink());
  fshader->SetStatus(false, "", NULL);
  EXPECT_FALSE(program->CanLink());
  fshader->SetStatus(true, "", NULL);
  EXPECT_TRUE(program->CanLink());
  EXPECT_TRUE(program->DetachShader(&shader_manager_, fshader));
  EXPECT_FALSE(program->DetachShader(&shader_manager_, fshader));
}

TEST_F(ProgramManagerWithShaderTest, GetUniformFakeLocation) {
  const Program* program = manager_.GetProgram(kClientProgramId);
  ASSERT_TRUE(program != NULL);
  // Emulate the situation that uniform3[1] isn't used and optimized out by
  // a driver, so it's location is -1.
  Program::UniformInfo* uniform = const_cast<Program::UniformInfo*>(
      program->GetUniformInfo(2));
  ASSERT_TRUE(uniform != NULL && kUniform3Size == 2);
  EXPECT_EQ(kUniform3Size, uniform->size);
  uniform->element_locations[1] = -1;
  EXPECT_EQ(kUniform1FakeLocation,
            program->GetUniformFakeLocation(kUniform1Name));
  EXPECT_EQ(kUniform2FakeLocation,
            program->GetUniformFakeLocation(kUniform2Name));
  EXPECT_EQ(kUniform3FakeLocation,
            program->GetUniformFakeLocation(kUniform3BadName));
  // Check we can get uniform2 as "uniform2" even though the name is
  // "uniform2[0]"
  EXPECT_EQ(kUniform2FakeLocation,
            program->GetUniformFakeLocation("uniform2"));
  // Check we can get uniform3 as "uniform3[0]" even though we simulated GL
  // returning "uniform3"
  EXPECT_EQ(kUniform3FakeLocation,
            program->GetUniformFakeLocation(kUniform3GoodName));
  // Check that we can get the locations of the array elements > 1
  EXPECT_EQ(ProgramManager::MakeFakeLocation(kUniform2FakeLocation, 1),
            program->GetUniformFakeLocation("uniform2[1]"));
  EXPECT_EQ(ProgramManager::MakeFakeLocation(kUniform2FakeLocation, 2),
            program->GetUniformFakeLocation("uniform2[2]"));
  EXPECT_EQ(-1, program->GetUniformFakeLocation("uniform2[3]"));
  EXPECT_EQ(-1, program->GetUniformFakeLocation("uniform3[1]"));
  EXPECT_EQ(-1, program->GetUniformFakeLocation("uniform3[2]"));
}

TEST_F(ProgramManagerWithShaderTest, GetUniformInfoByFakeLocation) {
  const GLint kInvalidLocation = 1234;
  const Program::UniformInfo* info;
  const Program* program = manager_.GetProgram(kClientProgramId);
  GLint real_location = -1;
  GLint array_index = -1;
  ASSERT_TRUE(program != NULL);
  info = program->GetUniformInfoByFakeLocation(
      kUniform2FakeLocation, &real_location, &array_index);
  EXPECT_EQ(kUniform2RealLocation, real_location);
  EXPECT_EQ(0, array_index);
  ASSERT_TRUE(info != NULL);
  EXPECT_EQ(kUniform2Type, info->type);
  real_location = -1;
  array_index = -1;
  info = program->GetUniformInfoByFakeLocation(
      kInvalidLocation, &real_location, &array_index);
  EXPECT_TRUE(info == NULL);
  EXPECT_EQ(-1, real_location);
  EXPECT_EQ(-1, array_index);
  GLint loc = program->GetUniformFakeLocation("uniform2[2]");
  info = program->GetUniformInfoByFakeLocation(
      loc, &real_location, &array_index);
  ASSERT_TRUE(info != NULL);
  EXPECT_EQ(kUniform2RealLocation + 2 * 2, real_location);
  EXPECT_EQ(2, array_index);
}

// Some GL drivers incorrectly return gl_DepthRange and possibly other uniforms
// that start with "gl_". Our implementation catches these and does not allow
// them back to client.
TEST_F(ProgramManagerWithShaderTest, GLDriverReturnsGLUnderscoreUniform) {
  static const char* kUniform2Name = "gl_longNameWeCanCheckFor";
  static ProgramManagerWithShaderTest::UniformInfo kUniforms[] = {
    { kUniform1Name,
      kUniform1Size,
      kUniform1Type,
      kUniform1FakeLocation,
      kUniform1RealLocation,
      kUniform1DesiredLocation,
      kUniform1Name,
    },
    { kUniform2Name,
      kUniform2Size,
      kUniform2Type,
      kUniform2FakeLocation,
      kUniform2RealLocation,
      kUniform2DesiredLocation,
      kUniform2Name,
    },
    { kUniform3BadName,
      kUniform3Size,
      kUniform3Type,
      kUniform3FakeLocation,
      kUniform3RealLocation,
      kUniform3DesiredLocation,
      kUniform3GoodName,
    },
  };
  const size_t kNumUniforms = arraysize(kUniforms);
  static const GLuint kClientProgramId = 1234;
  static const GLuint kServiceProgramId = 5679;
  const GLuint kVShaderClientId = 2001;
  const GLuint kFShaderClientId = 2002;
  const GLuint kVShaderServiceId = 3001;
  const GLuint kFShaderServiceId = 3002;
  SetupShader(
      kAttribs, kNumAttribs, kUniforms, kNumUniforms, kServiceProgramId);
  Shader* vshader = shader_manager_.CreateShader(
      kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
  ASSERT_TRUE(vshader != NULL);
  vshader->SetStatus(true, "", NULL);
  Shader* fshader = shader_manager_.CreateShader(
      kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
  ASSERT_TRUE(fshader != NULL);
  fshader->SetStatus(true, "", NULL);
  Program* program =
      manager_.CreateProgram(kClientProgramId, kServiceProgramId);
  ASSERT_TRUE(program != NULL);
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));
  program->Link(NULL, NULL, NULL, Program::kCountOnlyStaticallyUsed,
                base::Bind(&ShaderCacheCb));
  GLint value = 0;
  program->GetProgramiv(GL_ACTIVE_ATTRIBUTES, &value);
  EXPECT_EQ(3, value);
  // Check that we skipped the "gl_" uniform.
  program->GetProgramiv(GL_ACTIVE_UNIFORMS, &value);
  EXPECT_EQ(2, value);
  // Check that our max length adds room for the array spec and is not as long
  // as the "gl_" uniform we skipped.
  // +4u is to account for "gl_" and NULL terminator.
  program->GetProgramiv(GL_ACTIVE_UNIFORM_MAX_LENGTH, &value);
  EXPECT_EQ(strlen(kUniform3BadName) + 4u, static_cast<size_t>(value));
}

// Test the bug comparing similar array names is fixed.
TEST_F(ProgramManagerWithShaderTest, SimilarArrayNames) {
  static const char* kUniform2Name = "u_nameLong[0]";
  static const char* kUniform3Name = "u_name[0]";
  static const GLint kUniform2Size = 2;
  static const GLint kUniform3Size = 2;
  static ProgramManagerWithShaderTest::UniformInfo kUniforms[] = {
    { kUniform1Name,
      kUniform1Size,
      kUniform1Type,
      kUniform1FakeLocation,
      kUniform1RealLocation,
      kUniform1DesiredLocation,
      kUniform1Name,
    },
    { kUniform2Name,
      kUniform2Size,
      kUniform2Type,
      kUniform2FakeLocation,
      kUniform2RealLocation,
      kUniform2DesiredLocation,
      kUniform2Name,
    },
    { kUniform3Name,
      kUniform3Size,
      kUniform3Type,
      kUniform3FakeLocation,
      kUniform3RealLocation,
      kUniform3DesiredLocation,
      kUniform3Name,
    },
  };
  const size_t kNumUniforms = arraysize(kUniforms);
  static const GLuint kClientProgramId = 1234;
  static const GLuint kServiceProgramId = 5679;
  const GLuint kVShaderClientId = 2001;
  const GLuint kFShaderClientId = 2002;
  const GLuint kVShaderServiceId = 3001;
  const GLuint kFShaderServiceId = 3002;
  SetupShader(
      kAttribs, kNumAttribs, kUniforms, kNumUniforms, kServiceProgramId);
  Shader* vshader = shader_manager_.CreateShader(
      kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
  ASSERT_TRUE(vshader != NULL);
  vshader->SetStatus(true, "", NULL);
  Shader* fshader = shader_manager_.CreateShader(
      kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
  ASSERT_TRUE(fshader != NULL);
  fshader->SetStatus(true, "", NULL);
  Program* program =
      manager_.CreateProgram(kClientProgramId, kServiceProgramId);
  ASSERT_TRUE(program != NULL);
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));
  program->Link(NULL, NULL, NULL, Program::kCountOnlyStaticallyUsed,
                base::Bind(&ShaderCacheCb));

  // Check that we get the correct locations.
  EXPECT_EQ(kUniform2FakeLocation,
            program->GetUniformFakeLocation(kUniform2Name));
  EXPECT_EQ(kUniform3FakeLocation,
            program->GetUniformFakeLocation(kUniform3Name));
}

// Some GL drivers incorrectly return the wrong type. For example they return
// GL_FLOAT_VEC2 when they should return GL_FLOAT_MAT2. Check we handle this.
TEST_F(ProgramManagerWithShaderTest, GLDriverReturnsWrongTypeInfo) {
  static GLenum kAttrib2BadType = GL_FLOAT_VEC2;
  static GLenum kAttrib2GoodType = GL_FLOAT_MAT2;
  static GLenum kUniform2BadType = GL_FLOAT_VEC3;
  static GLenum kUniform2GoodType = GL_FLOAT_MAT3;
  MockShaderTranslator shader_translator;
  ShaderTranslator::VariableMap attrib_map;
  ShaderTranslator::VariableMap uniform_map;
  ShaderTranslator::VariableMap varying_map;
  attrib_map[kAttrib1Name] = ShaderTranslatorInterface::VariableInfo(
      kAttrib1Type, kAttrib1Size, kAttrib1Precision,
      kAttribStaticUse, kAttrib1Name);
  attrib_map[kAttrib2Name] = ShaderTranslatorInterface::VariableInfo(
      kAttrib2GoodType, kAttrib2Size, kAttrib2Precision,
      kAttribStaticUse, kAttrib2Name);
  attrib_map[kAttrib3Name] = ShaderTranslatorInterface::VariableInfo(
      kAttrib3Type, kAttrib3Size, kAttrib3Precision,
      kAttribStaticUse, kAttrib3Name);
  uniform_map[kUniform1Name] = ShaderTranslatorInterface::VariableInfo(
      kUniform1Type, kUniform1Size, kUniform1Precision,
      kUniform1StaticUse, kUniform1Name);
  uniform_map[kUniform2Name] = ShaderTranslatorInterface::VariableInfo(
      kUniform2GoodType, kUniform2Size, kUniform2Precision,
      kUniform2StaticUse, kUniform2Name);
  uniform_map[kUniform3GoodName] = ShaderTranslatorInterface::VariableInfo(
      kUniform3Type, kUniform3Size, kUniform3Precision,
      kUniform3StaticUse, kUniform3GoodName);
  EXPECT_CALL(shader_translator, attrib_map())
      .WillRepeatedly(ReturnRef(attrib_map));
  EXPECT_CALL(shader_translator, uniform_map())
      .WillRepeatedly(ReturnRef(uniform_map));
  EXPECT_CALL(shader_translator, varying_map())
      .WillRepeatedly(ReturnRef(varying_map));
  ShaderTranslator::NameMap name_map;
  EXPECT_CALL(shader_translator, name_map())
      .WillRepeatedly(ReturnRef(name_map));
  const GLuint kVShaderClientId = 2001;
  const GLuint kFShaderClientId = 2002;
  const GLuint kVShaderServiceId = 3001;
  const GLuint kFShaderServiceId = 3002;
  Shader* vshader = shader_manager_.CreateShader(
      kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
  ASSERT_TRUE(vshader != NULL);
  vshader->SetStatus(true, "", &shader_translator);
  Shader* fshader = shader_manager_.CreateShader(
      kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
  ASSERT_TRUE(fshader != NULL);
  fshader->SetStatus(true, "", &shader_translator);
  static ProgramManagerWithShaderTest::AttribInfo kAttribs[] = {
    { kAttrib1Name, kAttrib1Size, kAttrib1Type, kAttrib1Location, },
    { kAttrib2Name, kAttrib2Size, kAttrib2BadType, kAttrib2Location, },
    { kAttrib3Name, kAttrib3Size, kAttrib3Type, kAttrib3Location, },
  };
  static ProgramManagerWithShaderTest::UniformInfo kUniforms[] = {
    { kUniform1Name,
      kUniform1Size,
      kUniform1Type,
      kUniform1FakeLocation,
      kUniform1RealLocation,
      kUniform1DesiredLocation,
      kUniform1Name,
    },
    { kUniform2Name,
      kUniform2Size,
      kUniform2BadType,
      kUniform2FakeLocation,
      kUniform2RealLocation,
      kUniform2DesiredLocation,
      kUniform2Name,
    },
    { kUniform3BadName,
      kUniform3Size,
      kUniform3Type,
      kUniform3FakeLocation,
      kUniform3RealLocation,
      kUniform3DesiredLocation,
      kUniform3GoodName,
    },
  };
  const size_t kNumAttribs= arraysize(kAttribs);
  const size_t kNumUniforms = arraysize(kUniforms);
  static const GLuint kClientProgramId = 1234;
  static const GLuint kServiceProgramId = 5679;
  SetupShader(kAttribs, kNumAttribs, kUniforms, kNumUniforms,
              kServiceProgramId);
  Program* program = manager_.CreateProgram(
      kClientProgramId, kServiceProgramId);
  ASSERT_TRUE(program!= NULL);
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));
  program->Link(NULL, NULL, NULL, Program::kCountOnlyStaticallyUsed,
                base::Bind(&ShaderCacheCb));
  // Check that we got the good type, not the bad.
  // Check Attribs
  for (unsigned index = 0; index < kNumAttribs; ++index) {
    const Program::VertexAttrib* attrib_info =
        program->GetAttribInfo(index);
    ASSERT_TRUE(attrib_info != NULL);
    ShaderTranslator::VariableMap::const_iterator it = attrib_map.find(
        attrib_info->name);
    ASSERT_TRUE(it != attrib_map.end());
    EXPECT_EQ(it->first, attrib_info->name);
    EXPECT_EQ(static_cast<GLenum>(it->second.type), attrib_info->type);
    EXPECT_EQ(it->second.size, attrib_info->size);
    EXPECT_EQ(it->second.name, attrib_info->name);
  }
  // Check Uniforms
  for (unsigned index = 0; index < kNumUniforms; ++index) {
    const Program::UniformInfo* uniform_info =
        program->GetUniformInfo(index);
    ASSERT_TRUE(uniform_info != NULL);
    ShaderTranslator::VariableMap::const_iterator it = uniform_map.find(
        uniform_info->name);
    ASSERT_TRUE(it != uniform_map.end());
    EXPECT_EQ(it->first, uniform_info->name);
    EXPECT_EQ(static_cast<GLenum>(it->second.type), uniform_info->type);
    EXPECT_EQ(it->second.size, uniform_info->size);
    EXPECT_EQ(it->second.name, uniform_info->name);
  }
}

TEST_F(ProgramManagerWithShaderTest, ProgramInfoUseCount) {
  static const GLuint kClientProgramId = 124;
  static const GLuint kServiceProgramId = 457;
  Program* program = manager_.CreateProgram(
      kClientProgramId, kServiceProgramId);
  ASSERT_TRUE(program != NULL);
  EXPECT_FALSE(program->CanLink());
  const GLuint kVShaderClientId = 2001;
  const GLuint kFShaderClientId = 2002;
  const GLuint kVShaderServiceId = 3001;
  const GLuint kFShaderServiceId = 3002;
  Shader* vshader = shader_manager_.CreateShader(
      kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
  ASSERT_TRUE(vshader != NULL);
  vshader->SetStatus(true, "", NULL);
  Shader* fshader = shader_manager_.CreateShader(
      kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
  ASSERT_TRUE(fshader != NULL);
  fshader->SetStatus(true, "", NULL);
  EXPECT_FALSE(vshader->InUse());
  EXPECT_FALSE(fshader->InUse());
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_TRUE(vshader->InUse());
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));
  EXPECT_TRUE(fshader->InUse());
  EXPECT_TRUE(program->CanLink());
  EXPECT_FALSE(program->InUse());
  EXPECT_FALSE(program->IsDeleted());
  manager_.UseProgram(program);
  EXPECT_TRUE(program->InUse());
  manager_.UseProgram(program);
  EXPECT_TRUE(program->InUse());
  manager_.MarkAsDeleted(&shader_manager_, program);
  EXPECT_TRUE(program->IsDeleted());
  Program* info2 = manager_.GetProgram(kClientProgramId);
  EXPECT_EQ(program, info2);
  manager_.UnuseProgram(&shader_manager_, program);
  EXPECT_TRUE(program->InUse());
  // this should delete the info.
  EXPECT_CALL(*gl_, DeleteProgram(kServiceProgramId))
      .Times(1)
      .RetiresOnSaturation();
  manager_.UnuseProgram(&shader_manager_, program);
  info2 = manager_.GetProgram(kClientProgramId);
  EXPECT_TRUE(info2 == NULL);
  EXPECT_FALSE(vshader->InUse());
  EXPECT_FALSE(fshader->InUse());
}

TEST_F(ProgramManagerWithShaderTest, ProgramInfoUseCount2) {
  static const GLuint kClientProgramId = 124;
  static const GLuint kServiceProgramId = 457;
  Program* program = manager_.CreateProgram(
      kClientProgramId, kServiceProgramId);
  ASSERT_TRUE(program != NULL);
  EXPECT_FALSE(program->CanLink());
  const GLuint kVShaderClientId = 2001;
  const GLuint kFShaderClientId = 2002;
  const GLuint kVShaderServiceId = 3001;
  const GLuint kFShaderServiceId = 3002;
  Shader* vshader = shader_manager_.CreateShader(
      kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
  ASSERT_TRUE(vshader != NULL);
  vshader->SetStatus(true, "", NULL);
  Shader* fshader = shader_manager_.CreateShader(
      kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
  ASSERT_TRUE(fshader != NULL);
  fshader->SetStatus(true, "", NULL);
  EXPECT_FALSE(vshader->InUse());
  EXPECT_FALSE(fshader->InUse());
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_TRUE(vshader->InUse());
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));
  EXPECT_TRUE(fshader->InUse());
  EXPECT_TRUE(program->CanLink());
  EXPECT_FALSE(program->InUse());
  EXPECT_FALSE(program->IsDeleted());
  manager_.UseProgram(program);
  EXPECT_TRUE(program->InUse());
  manager_.UseProgram(program);
  EXPECT_TRUE(program->InUse());
  manager_.UnuseProgram(&shader_manager_, program);
  EXPECT_TRUE(program->InUse());
  manager_.UnuseProgram(&shader_manager_, program);
  EXPECT_FALSE(program->InUse());
  Program* info2 = manager_.GetProgram(kClientProgramId);
  EXPECT_EQ(program, info2);
  // this should delete the program.
  EXPECT_CALL(*gl_, DeleteProgram(kServiceProgramId))
      .Times(1)
      .RetiresOnSaturation();
  manager_.MarkAsDeleted(&shader_manager_, program);
  info2 = manager_.GetProgram(kClientProgramId);
  EXPECT_TRUE(info2 == NULL);
  EXPECT_FALSE(vshader->InUse());
  EXPECT_FALSE(fshader->InUse());
}

TEST_F(ProgramManagerWithShaderTest, ProgramInfoGetProgramInfo) {
  CommonDecoder::Bucket bucket;
  const Program* program = manager_.GetProgram(kClientProgramId);
  ASSERT_TRUE(program != NULL);
  program->GetProgramInfo(&manager_, &bucket);
  ProgramInfoHeader* header =
      bucket.GetDataAs<ProgramInfoHeader*>(0, sizeof(ProgramInfoHeader));
  ASSERT_TRUE(header != NULL);
  EXPECT_EQ(1u, header->link_status);
  EXPECT_EQ(arraysize(kAttribs), header->num_attribs);
  EXPECT_EQ(arraysize(kUniforms), header->num_uniforms);
  const ProgramInput* inputs = bucket.GetDataAs<const ProgramInput*>(
      sizeof(*header),
      sizeof(ProgramInput) * (header->num_attribs + header->num_uniforms));
  ASSERT_TRUE(inputs != NULL);
  const ProgramInput* input = inputs;
  // TODO(gman): Don't assume these are in order.
  for (uint32 ii = 0; ii < header->num_attribs; ++ii) {
    const AttribInfo& expected = kAttribs[ii];
    EXPECT_EQ(expected.size, input->size);
    EXPECT_EQ(expected.type, input->type);
    const int32* location = bucket.GetDataAs<const int32*>(
        input->location_offset, sizeof(int32));
    ASSERT_TRUE(location != NULL);
    EXPECT_EQ(expected.location, *location);
    const char* name_buf = bucket.GetDataAs<const char*>(
        input->name_offset, input->name_length);
    ASSERT_TRUE(name_buf != NULL);
    std::string name(name_buf, input->name_length);
    EXPECT_STREQ(expected.name, name.c_str());
    ++input;
  }
  // TODO(gman): Don't assume these are in order.
  for (uint32 ii = 0; ii < header->num_uniforms; ++ii) {
    const UniformInfo& expected = kUniforms[ii];
    EXPECT_EQ(expected.size, input->size);
    EXPECT_EQ(expected.type, input->type);
    const int32* locations = bucket.GetDataAs<const int32*>(
        input->location_offset, sizeof(int32) * input->size);
    ASSERT_TRUE(locations != NULL);
    for (int32 jj = 0; jj < input->size; ++jj) {
      EXPECT_EQ(
          ProgramManager::MakeFakeLocation(expected.fake_location, jj),
          locations[jj]);
    }
    const char* name_buf = bucket.GetDataAs<const char*>(
        input->name_offset, input->name_length);
    ASSERT_TRUE(name_buf != NULL);
    std::string name(name_buf, input->name_length);
    EXPECT_STREQ(expected.good_name, name.c_str());
    ++input;
  }
  EXPECT_EQ(header->num_attribs + header->num_uniforms,
            static_cast<uint32>(input - inputs));
}

// Some drivers optimize out unused uniform array elements, so their
// location would be -1.
TEST_F(ProgramManagerWithShaderTest, UnusedUniformArrayElements) {
  CommonDecoder::Bucket bucket;
  const Program* program = manager_.GetProgram(kClientProgramId);
  ASSERT_TRUE(program != NULL);
  // Emulate the situation that only the first element has a valid location.
  // TODO(zmo): Don't assume these are in order.
  for (size_t ii = 0; ii < arraysize(kUniforms); ++ii) {
    Program::UniformInfo* uniform = const_cast<Program::UniformInfo*>(
        program->GetUniformInfo(ii));
    ASSERT_TRUE(uniform != NULL);
    EXPECT_EQ(static_cast<size_t>(kUniforms[ii].size),
              uniform->element_locations.size());
    for (GLsizei jj = 1; jj < uniform->size; ++jj)
      uniform->element_locations[jj] = -1;
  }
  program->GetProgramInfo(&manager_, &bucket);
  ProgramInfoHeader* header =
      bucket.GetDataAs<ProgramInfoHeader*>(0, sizeof(ProgramInfoHeader));
  ASSERT_TRUE(header != NULL);
  EXPECT_EQ(1u, header->link_status);
  EXPECT_EQ(arraysize(kAttribs), header->num_attribs);
  EXPECT_EQ(arraysize(kUniforms), header->num_uniforms);
  const ProgramInput* inputs = bucket.GetDataAs<const ProgramInput*>(
      sizeof(*header),
      sizeof(ProgramInput) * (header->num_attribs + header->num_uniforms));
  ASSERT_TRUE(inputs != NULL);
  const ProgramInput* input = inputs + header->num_attribs;
  for (uint32 ii = 0; ii < header->num_uniforms; ++ii) {
    const UniformInfo& expected = kUniforms[ii];
    EXPECT_EQ(expected.size, input->size);
    const int32* locations = bucket.GetDataAs<const int32*>(
        input->location_offset, sizeof(int32) * input->size);
    ASSERT_TRUE(locations != NULL);
    EXPECT_EQ(
        ProgramManager::MakeFakeLocation(expected.fake_location, 0),
        locations[0]);
    for (int32 jj = 1; jj < input->size; ++jj)
      EXPECT_EQ(-1, locations[jj]);
    ++input;
  }
}

TEST_F(ProgramManagerWithShaderTest, BindAttribLocationConflicts) {
  // Set up shader
  const GLuint kVShaderClientId = 1;
  const GLuint kVShaderServiceId = 11;
  const GLuint kFShaderClientId = 2;
  const GLuint kFShaderServiceId = 12;
  MockShaderTranslator shader_translator;
  ShaderTranslator::VariableMap attrib_map;
  for (uint32 ii = 0; ii < kNumAttribs; ++ii) {
    attrib_map[kAttribs[ii].name] = ShaderTranslatorInterface::VariableInfo(
        kAttribs[ii].type,
        kAttribs[ii].size,
        SH_PRECISION_MEDIUMP,
        kAttribStaticUse,
        kAttribs[ii].name);
  }
  ShaderTranslator::VariableMap uniform_map;
  ShaderTranslator::VariableMap varying_map;
  EXPECT_CALL(shader_translator, attrib_map())
      .WillRepeatedly(ReturnRef(attrib_map));
  EXPECT_CALL(shader_translator, uniform_map())
      .WillRepeatedly(ReturnRef(uniform_map));
  EXPECT_CALL(shader_translator, varying_map())
      .WillRepeatedly(ReturnRef(varying_map));
  ShaderTranslator::NameMap name_map;
  EXPECT_CALL(shader_translator, name_map())
      .WillRepeatedly(ReturnRef(name_map));
  // Check we can create shader.
  Shader* vshader = shader_manager_.CreateShader(
      kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
  Shader* fshader = shader_manager_.CreateShader(
      kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
  // Check shader got created.
  ASSERT_TRUE(vshader != NULL && fshader != NULL);
  // Set Status
  vshader->SetStatus(true, "", &shader_translator);
  // Check attrib infos got copied.
  for (ShaderTranslator::VariableMap::const_iterator it = attrib_map.begin();
       it != attrib_map.end(); ++it) {
    const Shader::VariableInfo* variable_info =
        vshader->GetAttribInfo(it->first);
    ASSERT_TRUE(variable_info != NULL);
    EXPECT_EQ(it->second.type, variable_info->type);
    EXPECT_EQ(it->second.size, variable_info->size);
    EXPECT_EQ(it->second.precision, variable_info->precision);
    EXPECT_EQ(it->second.static_use, variable_info->static_use);
    EXPECT_EQ(it->second.name, variable_info->name);
  }
  fshader->SetStatus(true, "", NULL);

  // Set up program
  const GLuint kClientProgramId = 6666;
  const GLuint kServiceProgramId = 8888;
  Program* program =
      manager_.CreateProgram(kClientProgramId, kServiceProgramId);
  ASSERT_TRUE(program != NULL);
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));

  EXPECT_FALSE(program->DetectAttribLocationBindingConflicts());
  EXPECT_TRUE(LinkAsExpected(program, true));

  program->SetAttribLocationBinding(kAttrib1Name, 0);
  EXPECT_FALSE(program->DetectAttribLocationBindingConflicts());
  EXPECT_TRUE(LinkAsExpected(program, true));

  program->SetAttribLocationBinding("xxx", 0);
  EXPECT_FALSE(program->DetectAttribLocationBindingConflicts());
  EXPECT_TRUE(LinkAsExpected(program, true));

  program->SetAttribLocationBinding(kAttrib2Name, 1);
  EXPECT_FALSE(program->DetectAttribLocationBindingConflicts());
  EXPECT_TRUE(LinkAsExpected(program, true));

  program->SetAttribLocationBinding(kAttrib2Name, 0);
  EXPECT_TRUE(program->DetectAttribLocationBindingConflicts());
  EXPECT_TRUE(LinkAsExpected(program, false));
}

TEST_F(ProgramManagerWithShaderTest, UniformsPrecisionMismatch) {
  // Set up shader
  const GLuint kVShaderClientId = 1;
  const GLuint kVShaderServiceId = 11;
  const GLuint kFShaderClientId = 2;
  const GLuint kFShaderServiceId = 12;

  MockShaderTranslator vertex_shader_translator;
  ShaderTranslator::VariableMap vertex_attrib_map;
  ShaderTranslator::VariableMap vertex_uniform_map;
  vertex_uniform_map["a"] = ShaderTranslator::VariableInfo(
      1, 3, SH_PRECISION_MEDIUMP, 1, "a");
  ShaderTranslator::VariableMap vertex_varying_map;
  ShaderTranslator::NameMap vertex_name_map;
  EXPECT_CALL(vertex_shader_translator, attrib_map())
      .WillRepeatedly(ReturnRef(vertex_attrib_map));
  EXPECT_CALL(vertex_shader_translator, uniform_map())
      .WillRepeatedly(ReturnRef(vertex_uniform_map));
  EXPECT_CALL(vertex_shader_translator, varying_map())
      .WillRepeatedly(ReturnRef(vertex_varying_map));
  EXPECT_CALL(vertex_shader_translator, name_map())
    .WillRepeatedly(ReturnRef(vertex_name_map));

  MockShaderTranslator frag_shader_translator;
  ShaderTranslator::VariableMap frag_attrib_map;
  ShaderTranslator::VariableMap frag_uniform_map;
  frag_uniform_map["a"] = ShaderTranslator::VariableInfo(
      1, 3, SH_PRECISION_LOWP, 1, "a");
  ShaderTranslator::VariableMap frag_varying_map;
  ShaderTranslator::NameMap frag_name_map;
  EXPECT_CALL(frag_shader_translator, attrib_map())
      .WillRepeatedly(ReturnRef(frag_attrib_map));
  EXPECT_CALL(frag_shader_translator, uniform_map())
      .WillRepeatedly(ReturnRef(frag_uniform_map));
  EXPECT_CALL(frag_shader_translator, varying_map())
      .WillRepeatedly(ReturnRef(frag_varying_map));
  EXPECT_CALL(frag_shader_translator, name_map())
    .WillRepeatedly(ReturnRef(frag_name_map));

  // Check we can create shader.
  Shader* vshader = shader_manager_.CreateShader(
      kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
  Shader* fshader = shader_manager_.CreateShader(
      kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
  // Check shader got created.
  ASSERT_TRUE(vshader != NULL && fshader != NULL);
  // Set Status
  vshader->SetStatus(true, "", &vertex_shader_translator);
  fshader->SetStatus(true, "", &frag_shader_translator);

  // Set up program
  const GLuint kClientProgramId = 6666;
  const GLuint kServiceProgramId = 8888;
  Program* program =
      manager_.CreateProgram(kClientProgramId, kServiceProgramId);
  ASSERT_TRUE(program != NULL);
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));

  std::string conflicting_name;

  EXPECT_TRUE(program->DetectUniformsMismatch(&conflicting_name));
  EXPECT_EQ("a", conflicting_name);
  EXPECT_TRUE(LinkAsExpected(program, false));
}

// If a varying has different type in the vertex and fragment
// shader, linking should fail.
TEST_F(ProgramManagerWithShaderTest, VaryingTypeMismatch) {
  const VarInfo kVertexVarying =
      { SH_FLOAT_VEC3, 1, SH_PRECISION_MEDIUMP, 1, "a", kVarVarying };
  const VarInfo kFragmentVarying =
      { SH_FLOAT_VEC4, 1, SH_PRECISION_MEDIUMP, 1, "a", kVarVarying };
  Program* program = SetupShaderVariableTest(
      &kVertexVarying, 1, &kFragmentVarying, 1);

  std::string conflicting_name;

  EXPECT_TRUE(program->DetectVaryingsMismatch(&conflicting_name));
  EXPECT_EQ("a", conflicting_name);
  EXPECT_TRUE(LinkAsExpected(program, false));
}

// If a varying has different array size in the vertex and fragment
// shader, linking should fail.
TEST_F(ProgramManagerWithShaderTest, VaryingArraySizeMismatch) {
  const VarInfo kVertexVarying =
      { SH_FLOAT, 2, SH_PRECISION_MEDIUMP, 1, "a", kVarVarying };
  const VarInfo kFragmentVarying =
      { SH_FLOAT, 3, SH_PRECISION_MEDIUMP, 1, "a", kVarVarying };
  Program* program = SetupShaderVariableTest(
      &kVertexVarying, 1, &kFragmentVarying, 1);

  std::string conflicting_name;

  EXPECT_TRUE(program->DetectVaryingsMismatch(&conflicting_name));
  EXPECT_EQ("a", conflicting_name);
  EXPECT_TRUE(LinkAsExpected(program, false));
}

// If a varying has different precision in the vertex and fragment
// shader, linking should succeed.
TEST_F(ProgramManagerWithShaderTest, VaryingPrecisionMismatch) {
  const VarInfo kVertexVarying =
      { SH_FLOAT, 2, SH_PRECISION_HIGHP, 1, "a", kVarVarying };
  const VarInfo kFragmentVarying =
      { SH_FLOAT, 2, SH_PRECISION_MEDIUMP, 1, "a", kVarVarying };
  Program* program = SetupShaderVariableTest(
      &kVertexVarying, 1, &kFragmentVarying, 1);

  std::string conflicting_name;

  EXPECT_FALSE(program->DetectVaryingsMismatch(&conflicting_name));
  EXPECT_TRUE(conflicting_name.empty());
  EXPECT_TRUE(LinkAsExpected(program, true));
}

// If a varying is statically used in fragment shader but not
// declared in vertex shader, link should fail.
TEST_F(ProgramManagerWithShaderTest, VaryingMissing) {
  const VarInfo kFragmentVarying =
      { SH_FLOAT, 3, SH_PRECISION_MEDIUMP, 1, "a", kVarVarying };
  Program* program = SetupShaderVariableTest(
      NULL, 0, &kFragmentVarying, 1);

  std::string conflicting_name;

  EXPECT_TRUE(program->DetectVaryingsMismatch(&conflicting_name));
  EXPECT_EQ("a", conflicting_name);
  EXPECT_TRUE(LinkAsExpected(program, false));
}

// If a varying is declared but not statically used in fragment
// shader, even if it's not declared in vertex shader, link should
// succeed.
TEST_F(ProgramManagerWithShaderTest, InactiveVarying) {
  const VarInfo kFragmentVarying =
      { SH_FLOAT, 3, SH_PRECISION_MEDIUMP, 0, "a", kVarVarying };
  Program* program = SetupShaderVariableTest(
      NULL, 0, &kFragmentVarying, 1);

  std::string conflicting_name;

  EXPECT_FALSE(program->DetectVaryingsMismatch(&conflicting_name));
  EXPECT_TRUE(conflicting_name.empty());
  EXPECT_TRUE(LinkAsExpected(program, true));
}

// Uniforms and attributes are both global variables, thus sharing
// the same namespace. Any name conflicts should cause link
// failure.
TEST_F(ProgramManagerWithShaderTest, AttribUniformNameConflict) {
  const VarInfo kVertexAttribute =
      { SH_FLOAT_VEC4, 1, SH_PRECISION_MEDIUMP, 1, "a", kVarAttribute };
  const VarInfo kFragmentUniform =
      { SH_FLOAT_VEC4, 1, SH_PRECISION_MEDIUMP, 1, "a", kVarUniform };
  Program* program = SetupShaderVariableTest(
      &kVertexAttribute, 1, &kFragmentUniform, 1);

  std::string conflicting_name;

  EXPECT_TRUE(program->DetectGlobalNameConflicts(&conflicting_name));
  EXPECT_EQ("a", conflicting_name);
  EXPECT_TRUE(LinkAsExpected(program, false));
}

// Varyings go over 8 rows.
TEST_F(ProgramManagerWithShaderTest, TooManyVaryings) {
  const VarInfo kVertexVaryings[] = {
      { SH_FLOAT_VEC4, 4, SH_PRECISION_MEDIUMP, 1, "a", kVarVarying },
      { SH_FLOAT_VEC4, 5, SH_PRECISION_MEDIUMP, 1, "b", kVarVarying }
  };
  const VarInfo kFragmentVaryings[] = {
      { SH_FLOAT_VEC4, 4, SH_PRECISION_MEDIUMP, 1, "a", kVarVarying },
      { SH_FLOAT_VEC4, 5, SH_PRECISION_MEDIUMP, 1, "b", kVarVarying }
  };
  Program* program = SetupShaderVariableTest(
      kVertexVaryings, 2, kFragmentVaryings, 2);

  EXPECT_FALSE(
      program->CheckVaryingsPacking(Program::kCountOnlyStaticallyUsed));
  EXPECT_TRUE(LinkAsExpected(program, false));
}

// Varyings go over 8 rows but some are inactive
TEST_F(ProgramManagerWithShaderTest, TooManyInactiveVaryings) {
  const VarInfo kVertexVaryings[] = {
      { SH_FLOAT_VEC4, 4, SH_PRECISION_MEDIUMP, 1, "a", kVarVarying },
      { SH_FLOAT_VEC4, 5, SH_PRECISION_MEDIUMP, 1, "b", kVarVarying }
  };
  const VarInfo kFragmentVaryings[] = {
      { SH_FLOAT_VEC4, 4, SH_PRECISION_MEDIUMP, 0, "a", kVarVarying },
      { SH_FLOAT_VEC4, 5, SH_PRECISION_MEDIUMP, 1, "b", kVarVarying }
  };
  Program* program = SetupShaderVariableTest(
      kVertexVaryings, 2, kFragmentVaryings, 2);

  EXPECT_TRUE(
      program->CheckVaryingsPacking(Program::kCountOnlyStaticallyUsed));
  EXPECT_TRUE(LinkAsExpected(program, true));
}

// Varyings go over 8 rows but some are inactive.
// However, we still fail the check if kCountAll option is used.
TEST_F(ProgramManagerWithShaderTest, CountAllVaryingsInPacking) {
  const VarInfo kVertexVaryings[] = {
      { SH_FLOAT_VEC4, 4, SH_PRECISION_MEDIUMP, 1, "a", kVarVarying },
      { SH_FLOAT_VEC4, 5, SH_PRECISION_MEDIUMP, 1, "b", kVarVarying }
  };
  const VarInfo kFragmentVaryings[] = {
      { SH_FLOAT_VEC4, 4, SH_PRECISION_MEDIUMP, 0, "a", kVarVarying },
      { SH_FLOAT_VEC4, 5, SH_PRECISION_MEDIUMP, 1, "b", kVarVarying }
  };
  Program* program = SetupShaderVariableTest(
      kVertexVaryings, 2, kFragmentVaryings, 2);

  EXPECT_FALSE(program->CheckVaryingsPacking(Program::kCountAll));
}

TEST_F(ProgramManagerWithShaderTest, ClearWithSamplerTypes) {
  const GLuint kVShaderClientId = 2001;
  const GLuint kFShaderClientId = 2002;
  const GLuint kVShaderServiceId = 3001;
  const GLuint kFShaderServiceId = 3002;
  Shader* vshader = shader_manager_.CreateShader(
      kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
  ASSERT_TRUE(vshader != NULL);
  vshader->SetStatus(true, NULL, NULL);
  Shader* fshader = shader_manager_.CreateShader(
      kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
  ASSERT_TRUE(fshader != NULL);
  fshader->SetStatus(true, NULL, NULL);
  static const GLuint kClientProgramId = 1234;
  static const GLuint kServiceProgramId = 5679;
  Program* program = manager_.CreateProgram(
      kClientProgramId, kServiceProgramId);
  ASSERT_TRUE(program != NULL);
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));

  static const GLenum kSamplerTypes[] = {
    GL_SAMPLER_2D,
    GL_SAMPLER_CUBE,
    GL_SAMPLER_EXTERNAL_OES,
    GL_SAMPLER_3D_OES,
    GL_SAMPLER_2D_RECT_ARB,
  };
  const size_t kNumSamplerTypes = arraysize(kSamplerTypes);
  for (size_t ii = 0; ii < kNumSamplerTypes; ++ii) {
    static ProgramManagerWithShaderTest::AttribInfo kAttribs[] = {
      { kAttrib1Name, kAttrib1Size, kAttrib1Type, kAttrib1Location, },
      { kAttrib2Name, kAttrib2Size, kAttrib2Type, kAttrib2Location, },
      { kAttrib3Name, kAttrib3Size, kAttrib3Type, kAttrib3Location, },
    };
    ProgramManagerWithShaderTest::UniformInfo kUniforms[] = {
      { kUniform1Name,
        kUniform1Size,
        kUniform1Type,
        kUniform1FakeLocation,
        kUniform1RealLocation,
        kUniform1DesiredLocation,
        kUniform1Name,
      },
      { kUniform2Name,
        kUniform2Size,
        kSamplerTypes[ii],
        kUniform2FakeLocation,
        kUniform2RealLocation,
        kUniform2DesiredLocation,
        kUniform2Name,
      },
      { kUniform3BadName,
        kUniform3Size,
        kUniform3Type,
        kUniform3FakeLocation,
        kUniform3RealLocation,
        kUniform3DesiredLocation,
        kUniform3GoodName,
      },
    };
    const size_t kNumAttribs = arraysize(kAttribs);
    const size_t kNumUniforms = arraysize(kUniforms);
    SetupShader(kAttribs, kNumAttribs, kUniforms, kNumUniforms,
                kServiceProgramId);
    program->Link(NULL, NULL, NULL, Program::kCountOnlyStaticallyUsed,
                  base::Bind(&ShaderCacheCb));
    SetupExpectationsForClearingUniforms(kUniforms, kNumUniforms);
    manager_.ClearUniforms(program);
  }
}

TEST_F(ProgramManagerWithShaderTest, BindUniformLocation) {
  const GLuint kVShaderClientId = 2001;
  const GLuint kFShaderClientId = 2002;
  const GLuint kVShaderServiceId = 3001;
  const GLuint kFShaderServiceId = 3002;

  const GLint kUniform1DesiredLocation = 10;
  const GLint kUniform2DesiredLocation = -1;
  const GLint kUniform3DesiredLocation = 5;

  Shader* vshader = shader_manager_.CreateShader(
      kVShaderClientId, kVShaderServiceId, GL_VERTEX_SHADER);
  ASSERT_TRUE(vshader != NULL);
  vshader->SetStatus(true, NULL, NULL);
  Shader* fshader = shader_manager_.CreateShader(
      kFShaderClientId, kFShaderServiceId, GL_FRAGMENT_SHADER);
  ASSERT_TRUE(fshader != NULL);
  fshader->SetStatus(true, NULL, NULL);
  static const GLuint kClientProgramId = 1234;
  static const GLuint kServiceProgramId = 5679;
  Program* program = manager_.CreateProgram(
      kClientProgramId, kServiceProgramId);
  ASSERT_TRUE(program != NULL);
  EXPECT_TRUE(program->AttachShader(&shader_manager_, vshader));
  EXPECT_TRUE(program->AttachShader(&shader_manager_, fshader));
  EXPECT_TRUE(program->SetUniformLocationBinding(
      kUniform1Name, kUniform1DesiredLocation));
  EXPECT_TRUE(program->SetUniformLocationBinding(
      kUniform3BadName, kUniform3DesiredLocation));

  static ProgramManagerWithShaderTest::AttribInfo kAttribs[] = {
    { kAttrib1Name, kAttrib1Size, kAttrib1Type, kAttrib1Location, },
    { kAttrib2Name, kAttrib2Size, kAttrib2Type, kAttrib2Location, },
    { kAttrib3Name, kAttrib3Size, kAttrib3Type, kAttrib3Location, },
  };
  ProgramManagerWithShaderTest::UniformInfo kUniforms[] = {
    { kUniform1Name,
      kUniform1Size,
      kUniform1Type,
      kUniform1FakeLocation,
      kUniform1RealLocation,
      kUniform1DesiredLocation,
      kUniform1Name,
    },
    { kUniform2Name,
      kUniform2Size,
      kUniform2Type,
      kUniform2FakeLocation,
      kUniform2RealLocation,
      kUniform2DesiredLocation,
      kUniform2Name,
    },
    { kUniform3BadName,
      kUniform3Size,
      kUniform3Type,
      kUniform3FakeLocation,
      kUniform3RealLocation,
      kUniform3DesiredLocation,
      kUniform3GoodName,
    },
  };

  const size_t kNumAttribs = arraysize(kAttribs);
  const size_t kNumUniforms = arraysize(kUniforms);
  SetupShader(kAttribs, kNumAttribs, kUniforms, kNumUniforms,
              kServiceProgramId);
  program->Link(NULL, NULL, NULL, Program::kCountOnlyStaticallyUsed,
                base::Bind(&ShaderCacheCb));

  EXPECT_EQ(kUniform1DesiredLocation,
            program->GetUniformFakeLocation(kUniform1Name));
  EXPECT_EQ(kUniform3DesiredLocation,
            program->GetUniformFakeLocation(kUniform3BadName));
  EXPECT_EQ(kUniform3DesiredLocation,
            program->GetUniformFakeLocation(kUniform3GoodName));
}

class ProgramManagerWithCacheTest : public GpuServiceTest {
 public:
  static const GLuint kClientProgramId = 1;
  static const GLuint kServiceProgramId = 10;
  static const GLuint kVertexShaderClientId = 2;
  static const GLuint kFragmentShaderClientId = 20;
  static const GLuint kVertexShaderServiceId = 3;
  static const GLuint kFragmentShaderServiceId = 30;

  ProgramManagerWithCacheTest()
      : cache_(new MockProgramCache()),
        manager_(cache_.get(), kMaxVaryingVectors),
        vertex_shader_(NULL),
        fragment_shader_(NULL),
        program_(NULL) {
  }
  virtual ~ProgramManagerWithCacheTest() {
    manager_.Destroy(false);
    shader_manager_.Destroy(false);
  }

 protected:
  virtual void SetUp() {
    GpuServiceTest::SetUp();

    vertex_shader_ = shader_manager_.CreateShader(
       kVertexShaderClientId, kVertexShaderServiceId, GL_VERTEX_SHADER);
    fragment_shader_ = shader_manager_.CreateShader(
       kFragmentShaderClientId, kFragmentShaderServiceId, GL_FRAGMENT_SHADER);
    ASSERT_TRUE(vertex_shader_ != NULL);
    ASSERT_TRUE(fragment_shader_ != NULL);
    vertex_shader_->UpdateSource("lka asjf bjajsdfj");
    fragment_shader_->UpdateSource("lka asjf a   fasgag 3rdsf3 bjajsdfj");

    program_ = manager_.CreateProgram(
        kClientProgramId, kServiceProgramId);
    ASSERT_TRUE(program_ != NULL);

    program_->AttachShader(&shader_manager_, vertex_shader_);
    program_->AttachShader(&shader_manager_, fragment_shader_);
  }

  void SetShadersCompiled() {
    vertex_shader_->SetStatus(true, NULL, NULL);
    fragment_shader_->SetStatus(true, NULL, NULL);
  }

  void SetProgramCached() {
    cache_->LinkedProgramCacheSuccess(
        vertex_shader_->source()->c_str(),
        NULL,
        fragment_shader_->source()->c_str(),
        NULL,
        &program_->bind_attrib_location_map());
  }

  void SetExpectationsForProgramCached() {
    SetExpectationsForProgramCached(program_,
                                    vertex_shader_,
                                    fragment_shader_);
  }

  void SetExpectationsForProgramCached(
      Program* program,
      Shader* vertex_shader,
      Shader* fragment_shader) {
    EXPECT_CALL(*cache_.get(), SaveLinkedProgram(
        program->service_id(),
        vertex_shader,
        NULL,
        fragment_shader,
        NULL,
        &program->bind_attrib_location_map(),
        _)).Times(1);
  }

  void SetExpectationsForNotCachingProgram() {
    SetExpectationsForNotCachingProgram(program_,
                                        vertex_shader_,
                                        fragment_shader_);
  }

  void SetExpectationsForNotCachingProgram(
      Program* program,
      Shader* vertex_shader,
      Shader* fragment_shader) {
    EXPECT_CALL(*cache_.get(), SaveLinkedProgram(
        program->service_id(),
        vertex_shader,
        NULL,
        fragment_shader,
        NULL,
        &program->bind_attrib_location_map(),
        _)).Times(0);
  }

  void SetExpectationsForProgramLoad(ProgramCache::ProgramLoadResult result) {
    SetExpectationsForProgramLoad(kServiceProgramId,
                                  program_,
                                  vertex_shader_,
                                  fragment_shader_,
                                  result);
  }

  void SetExpectationsForProgramLoad(
      GLuint service_program_id,
      Program* program,
      Shader* vertex_shader,
      Shader* fragment_shader,
      ProgramCache::ProgramLoadResult result) {
    EXPECT_CALL(*cache_.get(),
                LoadLinkedProgram(service_program_id,
                                  vertex_shader,
                                  NULL,
                                  fragment_shader,
                                  NULL,
                                  &program->bind_attrib_location_map(),
                                  _))
        .WillOnce(Return(result));
  }

  void SetExpectationsForProgramLoadSuccess() {
    SetExpectationsForProgramLoadSuccess(kServiceProgramId);
  }

  void SetExpectationsForProgramLoadSuccess(GLuint service_program_id) {
    TestHelper::SetupProgramSuccessExpectations(gl_.get(),
                                                NULL,
                                                0,
                                                NULL,
                                                0,
                                                service_program_id);
  }

  void SetExpectationsForProgramLink() {
    SetExpectationsForProgramLink(kServiceProgramId);
  }

  void SetExpectationsForProgramLink(GLuint service_program_id) {
    TestHelper::SetupShader(gl_.get(), NULL, 0, NULL, 0, service_program_id);
    if (gfx::g_driver_gl.ext.b_GL_ARB_get_program_binary) {
      EXPECT_CALL(*gl_.get(),
                  ProgramParameteri(service_program_id,
                                    PROGRAM_BINARY_RETRIEVABLE_HINT,
                                    GL_TRUE)).Times(1);
    }
  }

  void SetExpectationsForSuccessCompile(
      const Shader* shader) {
    const GLuint shader_id = shader->service_id();
    const char* src = shader->source()->c_str();
    EXPECT_CALL(*gl_.get(),
                ShaderSource(shader_id, 1, Pointee(src), NULL)).Times(1);
    EXPECT_CALL(*gl_.get(), CompileShader(shader_id)).Times(1);
    EXPECT_CALL(*gl_.get(), GetShaderiv(shader_id, GL_COMPILE_STATUS, _))
      .WillOnce(SetArgumentPointee<2>(GL_TRUE));
  }

  void SetExpectationsForNoCompile(const Shader* shader) {
    const GLuint shader_id = shader->service_id();
    const char* src = shader->source()->c_str();
    EXPECT_CALL(*gl_.get(),
                ShaderSource(shader_id, 1, Pointee(src), NULL)).Times(0);
    EXPECT_CALL(*gl_.get(), CompileShader(shader_id)).Times(0);
    EXPECT_CALL(*gl_.get(), GetShaderiv(shader_id, GL_COMPILE_STATUS, _))
        .Times(0);
  }

  void SetExpectationsForErrorCompile(const Shader* shader) {
    const GLuint shader_id = shader->service_id();
    const char* src = shader->source()->c_str();
    EXPECT_CALL(*gl_.get(),
                ShaderSource(shader_id, 1, Pointee(src), NULL)).Times(1);
    EXPECT_CALL(*gl_.get(), CompileShader(shader_id)).Times(1);
    EXPECT_CALL(*gl_.get(), GetShaderiv(shader_id, GL_COMPILE_STATUS, _))
      .WillOnce(SetArgumentPointee<2>(GL_FALSE));
    EXPECT_CALL(*gl_.get(), GetShaderiv(shader_id, GL_INFO_LOG_LENGTH, _))
      .WillOnce(SetArgumentPointee<2>(0));
    EXPECT_CALL(*gl_.get(), GetShaderInfoLog(shader_id, 0, _, _))
      .Times(1);
  }

  scoped_ptr<MockProgramCache> cache_;
  ProgramManager manager_;

  Shader* vertex_shader_;
  Shader* fragment_shader_;
  Program* program_;
  ShaderManager shader_manager_;
};

// GCC requires these declarations, but MSVC requires they not be present
#ifndef COMPILER_MSVC
const GLuint ProgramManagerWithCacheTest::kClientProgramId;
const GLuint ProgramManagerWithCacheTest::kServiceProgramId;
const GLuint ProgramManagerWithCacheTest::kVertexShaderClientId;
const GLuint ProgramManagerWithCacheTest::kFragmentShaderClientId;
const GLuint ProgramManagerWithCacheTest::kVertexShaderServiceId;
const GLuint ProgramManagerWithCacheTest::kFragmentShaderServiceId;
#endif

TEST_F(ProgramManagerWithCacheTest, CacheProgramOnSuccessfulLink) {
  SetShadersCompiled();
  SetExpectationsForProgramLink();
  SetExpectationsForProgramCached();
  EXPECT_TRUE(program_->Link(NULL, NULL, NULL,
      Program::kCountOnlyStaticallyUsed, base::Bind(&ShaderCacheCb)));
}

TEST_F(ProgramManagerWithCacheTest, LoadProgramOnProgramCacheHit) {
  SetShadersCompiled();
  SetProgramCached();

  SetExpectationsForNoCompile(vertex_shader_);
  SetExpectationsForNoCompile(fragment_shader_);
  SetExpectationsForProgramLoad(ProgramCache::PROGRAM_LOAD_SUCCESS);
  SetExpectationsForNotCachingProgram();
  SetExpectationsForProgramLoadSuccess();

  EXPECT_TRUE(program_->Link(NULL, NULL, NULL,
      Program::kCountOnlyStaticallyUsed, base::Bind(&ShaderCacheCb)));
}

}  // namespace gles2
}  // namespace gpu
