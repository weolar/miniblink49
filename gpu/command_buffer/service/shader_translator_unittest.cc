// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/shader_translator.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gpu {
namespace gles2 {

class ShaderTranslatorTest : public testing::Test {
 public:
  ShaderTranslatorTest() {
  }

  virtual ~ShaderTranslatorTest() {
  }

 protected:
  virtual void SetUp() {
    ShBuiltInResources resources;
    ShInitBuiltInResources(&resources);
    resources.MaxExpressionComplexity = 32;
    resources.MaxCallStackDepth = 32;

    vertex_translator_ = new ShaderTranslator();
    fragment_translator_ = new ShaderTranslator();

    ASSERT_TRUE(vertex_translator_->Init(
        SH_VERTEX_SHADER, SH_GLES2_SPEC, &resources,
        ShaderTranslatorInterface::kGlsl,
        SH_EMULATE_BUILT_IN_FUNCTIONS));
    ASSERT_TRUE(fragment_translator_->Init(
        SH_FRAGMENT_SHADER, SH_GLES2_SPEC, &resources,
        ShaderTranslatorInterface::kGlsl,
        static_cast<ShCompileOptions>(0)));
    // Post-init the results must be empty.
    // Vertex translator results.
    EXPECT_TRUE(vertex_translator_->translated_shader() == NULL);
    EXPECT_TRUE(vertex_translator_->info_log() == NULL);
    EXPECT_TRUE(vertex_translator_->attrib_map().empty());
    EXPECT_TRUE(vertex_translator_->uniform_map().empty());
    // Fragment translator results.
    EXPECT_TRUE(fragment_translator_->translated_shader() == NULL);
    EXPECT_TRUE(fragment_translator_->info_log() == NULL);
    EXPECT_TRUE(fragment_translator_->attrib_map().empty());
    EXPECT_TRUE(fragment_translator_->uniform_map().empty());
  }
  virtual void TearDown() {
    vertex_translator_ = NULL;
    fragment_translator_ = NULL;
  }

  scoped_refptr<ShaderTranslator> vertex_translator_;
  scoped_refptr<ShaderTranslator> fragment_translator_;
};

TEST_F(ShaderTranslatorTest, ValidVertexShader) {
  const char* shader =
      "void main() {\n"
      "  gl_Position = vec4(1.0);\n"
      "}";

  // A valid shader should be successfully translated.
  EXPECT_TRUE(vertex_translator_->Translate(shader));
  // Info log must be NULL.
  EXPECT_TRUE(vertex_translator_->info_log() == NULL);
  // Translated shader must be valid and non-empty.
  ASSERT_TRUE(vertex_translator_->translated_shader() != NULL);
  EXPECT_GT(strlen(vertex_translator_->translated_shader()), 0u);
  // There should be no attributes or uniforms.
  EXPECT_TRUE(vertex_translator_->attrib_map().empty());
  EXPECT_TRUE(vertex_translator_->uniform_map().empty());
}

TEST_F(ShaderTranslatorTest, InvalidVertexShader) {
  const char* bad_shader = "foo-bar";
  const char* good_shader =
      "void main() {\n"
      "  gl_Position = vec4(1.0);\n"
      "}";

  // An invalid shader should fail.
  EXPECT_FALSE(vertex_translator_->Translate(bad_shader));
  // Info log must be valid and non-empty.
  ASSERT_TRUE(vertex_translator_->info_log() != NULL);
  EXPECT_GT(strlen(vertex_translator_->info_log()), 0u);
  // Translated shader must be NULL.
  EXPECT_TRUE(vertex_translator_->translated_shader() == NULL);
  // There should be no attributes or uniforms.
  EXPECT_TRUE(vertex_translator_->attrib_map().empty());
  EXPECT_TRUE(vertex_translator_->uniform_map().empty());

  // Try a good shader after bad.
  EXPECT_TRUE(vertex_translator_->Translate(good_shader));
  EXPECT_TRUE(vertex_translator_->info_log() == NULL);
  ASSERT_TRUE(vertex_translator_->translated_shader() != NULL);
  EXPECT_GT(strlen(vertex_translator_->translated_shader()), 0u);
}

TEST_F(ShaderTranslatorTest, ValidFragmentShader) {
  const char* shader =
      "void main() {\n"
      "  gl_FragColor = vec4(1.0);\n"
      "}";

  // A valid shader should be successfully translated.
  EXPECT_TRUE(fragment_translator_->Translate(shader));
  // Info log must be NULL.
  EXPECT_TRUE(fragment_translator_->info_log() == NULL);
  // Translated shader must be valid and non-empty.
  ASSERT_TRUE(fragment_translator_->translated_shader() != NULL);
  EXPECT_GT(strlen(fragment_translator_->translated_shader()), 0u);
  // There should be no attributes or uniforms.
  EXPECT_TRUE(fragment_translator_->attrib_map().empty());
  EXPECT_TRUE(fragment_translator_->uniform_map().empty());
}

TEST_F(ShaderTranslatorTest, InvalidFragmentShader) {
  const char* shader = "foo-bar";

  // An invalid shader should fail.
  EXPECT_FALSE(fragment_translator_->Translate(shader));
  // Info log must be valid and non-empty.
  ASSERT_TRUE(fragment_translator_->info_log() != NULL);
  EXPECT_GT(strlen(fragment_translator_->info_log()), 0u);
  // Translated shader must be NULL.
  EXPECT_TRUE(fragment_translator_->translated_shader() == NULL);
  // There should be no attributes or uniforms.
  EXPECT_TRUE(fragment_translator_->attrib_map().empty());
  EXPECT_TRUE(fragment_translator_->uniform_map().empty());
}

TEST_F(ShaderTranslatorTest, GetAttributes) {
  const char* shader =
      "attribute vec4 vPosition;\n"
      "void main() {\n"
      "  gl_Position = vPosition;\n"
      "}";

  EXPECT_TRUE(vertex_translator_->Translate(shader));
  // Info log must be NULL.
  EXPECT_TRUE(vertex_translator_->info_log() == NULL);
  // Translated shader must be valid and non-empty.
  ASSERT_TRUE(vertex_translator_->translated_shader() != NULL);
  EXPECT_GT(strlen(vertex_translator_->translated_shader()), 0u);
  // There should be no uniforms.
  EXPECT_TRUE(vertex_translator_->uniform_map().empty());
  // There should be one attribute with following characteristics:
  // name:vPosition type:SH_FLOAT_VEC4 size:1.
  const ShaderTranslator::VariableMap& attrib_map =
      vertex_translator_->attrib_map();
  EXPECT_EQ(1u, attrib_map.size());
  ShaderTranslator::VariableMap::const_iterator iter =
      attrib_map.find("vPosition");
  EXPECT_TRUE(iter != attrib_map.end());
  EXPECT_EQ(SH_FLOAT_VEC4, iter->second.type);
  EXPECT_EQ(1, iter->second.size);
  EXPECT_EQ("vPosition", iter->second.name);
}

TEST_F(ShaderTranslatorTest, GetUniforms) {
  const char* shader =
      "precision mediump float;\n"
      "struct Foo {\n"
      "  vec4 color[1];\n"
      "};\n"
      "struct Bar {\n"
      "  Foo foo;\n"
      "};\n"
      "uniform Bar bar[2];\n"
      "void main() {\n"
      "  gl_FragColor = bar[0].foo.color[0] + bar[1].foo.color[0];\n"
      "}";

  EXPECT_TRUE(fragment_translator_->Translate(shader));
  // Info log must be NULL.
  EXPECT_TRUE(fragment_translator_->info_log() == NULL);
  // Translated shader must be valid and non-empty.
  ASSERT_TRUE(fragment_translator_->translated_shader() != NULL);
  EXPECT_GT(strlen(fragment_translator_->translated_shader()), 0u);
  // There should be no attributes.
  EXPECT_TRUE(fragment_translator_->attrib_map().empty());
  // There should be two uniforms with following characteristics:
  // 1. name:bar[0].foo.color[0] type:SH_FLOAT_VEC4 size:1
  // 2. name:bar[1].foo.color[0] type:SH_FLOAT_VEC4 size:1
  const ShaderTranslator::VariableMap& uniform_map =
      fragment_translator_->uniform_map();
  EXPECT_EQ(2u, uniform_map.size());
  // First uniform.
  ShaderTranslator::VariableMap::const_iterator iter =
      uniform_map.find("bar[0].foo.color[0]");
  EXPECT_TRUE(iter != uniform_map.end());
  EXPECT_EQ(SH_FLOAT_VEC4, iter->second.type);
  EXPECT_EQ(1, iter->second.size);
  EXPECT_EQ("bar[0].foo.color[0]", iter->second.name);
  // Second uniform.
  iter = uniform_map.find("bar[1].foo.color[0]");
  EXPECT_TRUE(iter != uniform_map.end());
  EXPECT_EQ(SH_FLOAT_VEC4, iter->second.type);
  EXPECT_EQ(1, iter->second.size);
  EXPECT_EQ("bar[1].foo.color[0]", iter->second.name);
}

#if defined(OS_MACOSX)
TEST_F(ShaderTranslatorTest, BuiltInFunctionEmulation) {
  // This test might become invalid in the future when ANGLE Translator is no
  // longer emulate dot(float, float) in Mac, or the emulated function name is
  // no longer webgl_dot_emu.
  const char* shader =
      "void main() {\n"
      "  gl_Position = vec4(dot(1.0, 1.0), 1.0, 1.0, 1.0);\n"
      "}";

  EXPECT_TRUE(vertex_translator_->Translate(shader));
  // Info log must be NULL.
  EXPECT_TRUE(vertex_translator_->info_log() == NULL);
  // Translated shader must be valid and non-empty.
  ASSERT_TRUE(vertex_translator_->translated_shader() != NULL);
  EXPECT_TRUE(strstr(vertex_translator_->translated_shader(),
                     "webgl_dot_emu") != NULL);
}
#endif

TEST_F(ShaderTranslatorTest, OptionsString) {
  scoped_refptr<ShaderTranslator> translator_1 = new ShaderTranslator();
  scoped_refptr<ShaderTranslator> translator_2 = new ShaderTranslator();
  scoped_refptr<ShaderTranslator> translator_3 = new ShaderTranslator();

  ShBuiltInResources resources;
  ShInitBuiltInResources(&resources);

  ASSERT_TRUE(translator_1->Init(
      SH_VERTEX_SHADER, SH_GLES2_SPEC, &resources,
      ShaderTranslatorInterface::kGlsl,
      SH_EMULATE_BUILT_IN_FUNCTIONS));
  ASSERT_TRUE(translator_2->Init(
      SH_FRAGMENT_SHADER, SH_GLES2_SPEC, &resources,
      ShaderTranslatorInterface::kGlsl,
      static_cast<ShCompileOptions>(0)));
  resources.EXT_draw_buffers = 1;
  ASSERT_TRUE(translator_3->Init(
      SH_VERTEX_SHADER, SH_GLES2_SPEC, &resources,
      ShaderTranslatorInterface::kGlsl,
      SH_EMULATE_BUILT_IN_FUNCTIONS));

  std::string options_1(
      translator_1->GetStringForOptionsThatWouldAffectCompilation());
  std::string options_2(
      translator_1->GetStringForOptionsThatWouldAffectCompilation());
  std::string options_3(
      translator_2->GetStringForOptionsThatWouldAffectCompilation());
  std::string options_4(
      translator_3->GetStringForOptionsThatWouldAffectCompilation());

  EXPECT_EQ(options_1, options_2);
  EXPECT_NE(options_1, options_3);
  EXPECT_NE(options_1, options_4);
  EXPECT_NE(options_3, options_4);
}

}  // namespace gles2
}  // namespace gpu

