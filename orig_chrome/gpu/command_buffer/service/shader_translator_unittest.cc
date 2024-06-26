// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <GLES2/gl2.h>

#include "gpu/command_buffer/service/shader_translator.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_version_info.h"

namespace gpu {
namespace gles2 {

    class ShaderTranslatorTest : public testing::Test {
    public:
        ShaderTranslatorTest()
        {
            shader_output_language_ = ShaderTranslator::GetShaderOutputLanguageForContext(
                gfx::GLVersionInfo("2.0", "", ""));
        }

        ~ShaderTranslatorTest() override { }

    protected:
        void SetUp() override
        {
            ShBuiltInResources resources;
            ShInitBuiltInResources(&resources);
            resources.MaxExpressionComplexity = 32;
            resources.MaxCallStackDepth = 32;

            vertex_translator_ = new ShaderTranslator();
            fragment_translator_ = new ShaderTranslator();

            ASSERT_TRUE(vertex_translator_->Init(GL_VERTEX_SHADER, SH_GLES2_SPEC,
                &resources, shader_output_language_,
                SH_EMULATE_BUILT_IN_FUNCTIONS));
            ASSERT_TRUE(fragment_translator_->Init(GL_FRAGMENT_SHADER, SH_GLES2_SPEC,
                &resources, shader_output_language_,
                static_cast<ShCompileOptions>(0)));
        }
        void TearDown() override
        {
            vertex_translator_ = NULL;
            fragment_translator_ = NULL;
        }

        scoped_refptr<ShaderTranslator> vertex_translator_;
        scoped_refptr<ShaderTranslator> fragment_translator_;
        ShShaderOutput shader_output_language_;
    };

    TEST_F(ShaderTranslatorTest, ValidVertexShader)
    {
        const char* shader = "void main() {\n"
                             "  gl_Position = vec4(1.0);\n"
                             "}";

        // A valid shader should be successfully translated.
        std::string info_log, translated_source;
        int shader_version;
        AttributeMap attrib_map;
        UniformMap uniform_map;
        VaryingMap varying_map;
        NameMap name_map;
        EXPECT_TRUE(vertex_translator_->Translate(shader,
            &info_log,
            &translated_source,
            &shader_version,
            &attrib_map,
            &uniform_map,
            &varying_map,
            &name_map));
        // Info log must be NULL.
        EXPECT_TRUE(info_log.empty());
        // Translated shader must be valid and non-empty.
        ASSERT_FALSE(translated_source.empty());
        // There should be no attributes, uniforms, and only one built-in
        // varying: gl_Position.
        EXPECT_TRUE(attrib_map.empty());
        EXPECT_TRUE(uniform_map.empty());
        EXPECT_EQ(1u, varying_map.size());
        // There should be no name mapping.
        EXPECT_TRUE(name_map.empty());
    }

    TEST_F(ShaderTranslatorTest, InvalidVertexShader)
    {
        const char* bad_shader = "foo-bar";
        const char* good_shader = "void main() {\n"
                                  "  gl_Position = vec4(1.0);\n"
                                  "}";

        // An invalid shader should fail.
        std::string info_log, translated_source;
        int shader_version;
        AttributeMap attrib_map;
        UniformMap uniform_map;
        VaryingMap varying_map;
        NameMap name_map;
        EXPECT_FALSE(vertex_translator_->Translate(bad_shader,
            &info_log,
            &translated_source,
            &shader_version,
            &attrib_map,
            &uniform_map,
            &varying_map,
            &name_map));
        // Info log must be valid and non-empty.
        ASSERT_FALSE(info_log.empty());
        // Translated shader must be NULL.
        EXPECT_TRUE(translated_source.empty());
        // There should be no attributes, uniforms, varyings, or name mapping.
        EXPECT_TRUE(attrib_map.empty());
        EXPECT_TRUE(uniform_map.empty());
        EXPECT_TRUE(varying_map.empty());
        EXPECT_TRUE(name_map.empty());

        // Try a good shader after bad.
        info_log.clear();
        EXPECT_TRUE(vertex_translator_->Translate(good_shader,
            &info_log,
            &translated_source,
            &shader_version,
            &attrib_map,
            &uniform_map,
            &varying_map,
            &name_map));
        EXPECT_TRUE(info_log.empty());
        EXPECT_FALSE(translated_source.empty());
    }

    TEST_F(ShaderTranslatorTest, ValidFragmentShader)
    {
        const char* shader = "void main() {\n"
                             "  gl_FragColor = vec4(1.0);\n"
                             "}";

        // A valid shader should be successfully translated.
        std::string info_log, translated_source;
        int shader_version;
        AttributeMap attrib_map;
        UniformMap uniform_map;
        VaryingMap varying_map;
        NameMap name_map;
        EXPECT_TRUE(fragment_translator_->Translate(shader,
            &info_log,
            &translated_source,
            &shader_version,
            &attrib_map,
            &uniform_map,
            &varying_map,
            &name_map));
        // Info log must be NULL.
        EXPECT_TRUE(info_log.empty());
        // Translated shader must be valid and non-empty.
        ASSERT_FALSE(translated_source.empty());
        // There should be no attributes, uniforms, varyings, or name mapping.
        EXPECT_TRUE(attrib_map.empty());
        EXPECT_TRUE(uniform_map.empty());
        EXPECT_TRUE(varying_map.empty());
        EXPECT_TRUE(name_map.empty());
    }

    TEST_F(ShaderTranslatorTest, InvalidFragmentShader)
    {
        const char* shader = "foo-bar";

        std::string info_log, translated_source;
        int shader_version;
        AttributeMap attrib_map;
        UniformMap uniform_map;
        VaryingMap varying_map;
        NameMap name_map;
        // An invalid shader should fail.
        EXPECT_FALSE(fragment_translator_->Translate(shader,
            &info_log,
            &translated_source,
            &shader_version,
            &attrib_map,
            &uniform_map,
            &varying_map,
            &name_map));
        // Info log must be valid and non-empty.
        EXPECT_FALSE(info_log.empty());
        // Translated shader must be NULL.
        EXPECT_TRUE(translated_source.empty());
        // There should be no attributes or uniforms.
        EXPECT_TRUE(attrib_map.empty());
        EXPECT_TRUE(uniform_map.empty());
        EXPECT_TRUE(varying_map.empty());
        EXPECT_TRUE(name_map.empty());
    }

    TEST_F(ShaderTranslatorTest, GetAttributes)
    {
        const char* shader = "attribute vec4 vPosition;\n"
                             "void main() {\n"
                             "  gl_Position = vPosition;\n"
                             "}";

        std::string info_log, translated_source;
        int shader_version;
        AttributeMap attrib_map;
        UniformMap uniform_map;
        VaryingMap varying_map;
        NameMap name_map;
        EXPECT_TRUE(vertex_translator_->Translate(shader,
            &info_log,
            &translated_source,
            &shader_version,
            &attrib_map,
            &uniform_map,
            &varying_map,
            &name_map));
        // Info log must be NULL.
        EXPECT_TRUE(info_log.empty());
        // Translated shader must be valid and non-empty.
        EXPECT_FALSE(translated_source.empty());
        // There should be no uniforms.
        EXPECT_TRUE(uniform_map.empty());
        // There should be one attribute with following characteristics:
        // name:vPosition type:GL_FLOAT_VEC4 size:0.
        EXPECT_EQ(1u, attrib_map.size());
        AttributeMap::const_iterator iter = attrib_map.find("vPosition");
        EXPECT_TRUE(iter != attrib_map.end());
        EXPECT_EQ(static_cast<GLenum>(GL_FLOAT_VEC4), iter->second.type);
        EXPECT_EQ(0u, iter->second.arraySize);
        EXPECT_EQ("vPosition", iter->second.name);
    }

    TEST_F(ShaderTranslatorTest, GetUniforms)
    {
        const char* shader = "precision mediump float;\n"
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

        std::string info_log, translated_source;
        int shader_version;
        AttributeMap attrib_map;
        UniformMap uniform_map;
        VaryingMap varying_map;
        NameMap name_map;
        EXPECT_TRUE(fragment_translator_->Translate(shader,
            &info_log,
            &translated_source,
            &shader_version,
            &attrib_map,
            &uniform_map,
            &varying_map,
            &name_map));
        // Info log must be NULL.
        EXPECT_TRUE(info_log.empty());
        // Translated shader must be valid and non-empty.
        EXPECT_FALSE(translated_source.empty());
        // There should be no attributes.
        EXPECT_TRUE(attrib_map.empty());
        // There should be two uniforms with following characteristics:
        // 1. name:bar[0].foo.color[0] type:GL_FLOAT_VEC4 size:1
        // 2. name:bar[1].foo.color[0] type:GL_FLOAT_VEC4 size:1
        // However, there will be only one entry "bar" in the map.
        EXPECT_EQ(1u, uniform_map.size());
        UniformMap::const_iterator iter = uniform_map.find("bar");
        EXPECT_TRUE(iter != uniform_map.end());
        // First uniform.
        const sh::ShaderVariable* info;
        std::string original_name;
        EXPECT_TRUE(iter->second.findInfoByMappedName(
            "bar[0].foo.color[0]", &info, &original_name));
        EXPECT_EQ(static_cast<GLenum>(GL_FLOAT_VEC4), info->type);
        EXPECT_EQ(1u, info->arraySize);
        EXPECT_STREQ("color", info->name.c_str());
        EXPECT_STREQ("bar[0].foo.color[0]", original_name.c_str());
        // Second uniform.
        EXPECT_TRUE(iter->second.findInfoByMappedName(
            "bar[1].foo.color[0]", &info, &original_name));
        EXPECT_EQ(static_cast<GLenum>(GL_FLOAT_VEC4), info->type);
        EXPECT_EQ(1u, info->arraySize);
        EXPECT_STREQ("color", info->name.c_str());
        EXPECT_STREQ("bar[1].foo.color[0]", original_name.c_str());
    }

    TEST_F(ShaderTranslatorTest, OptionsString)
    {
        scoped_refptr<ShaderTranslator> translator_1 = new ShaderTranslator();
        scoped_refptr<ShaderTranslator> translator_2 = new ShaderTranslator();
        scoped_refptr<ShaderTranslator> translator_3 = new ShaderTranslator();

        ShBuiltInResources resources;
        ShInitBuiltInResources(&resources);

        ASSERT_TRUE(translator_1->Init(GL_VERTEX_SHADER, SH_GLES2_SPEC, &resources,
            SH_GLSL_150_CORE_OUTPUT,
            SH_EMULATE_BUILT_IN_FUNCTIONS));
        ASSERT_TRUE(translator_2->Init(GL_FRAGMENT_SHADER, SH_GLES2_SPEC, &resources,
            SH_GLSL_150_CORE_OUTPUT,
            static_cast<ShCompileOptions>(0)));
        resources.EXT_draw_buffers = 1;
        ASSERT_TRUE(translator_3->Init(GL_VERTEX_SHADER, SH_GLES2_SPEC, &resources,
            SH_GLSL_150_CORE_OUTPUT,
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

    class ShaderTranslatorOutputVersionTest
        : public testing::TestWithParam<testing::tuple<const char*, const char*>> {
    };

    TEST_P(ShaderTranslatorOutputVersionTest, HasCorrectOutputGLSLVersion)
    {
        // Test that translating to a shader targeting certain OpenGL context version
        // (version string in test param tuple index 0) produces a GLSL shader that
        // contains correct version string for that context (version directive
        // in test param tuple index 1).

        const char* kShader = "attribute vec4 vPosition;\n"
                              "void main() {\n"
                              "  gl_Position = vPosition;\n"
                              "}";

        gfx::GLVersionInfo output_context_version(testing::get<0>(GetParam()), "",
            "");

        scoped_refptr<ShaderTranslator> translator = new ShaderTranslator();
        ShBuiltInResources resources;
        ShInitBuiltInResources(&resources);
        ShCompileOptions compile_options = SH_OBJECT_CODE;
        ShShaderOutput shader_output_language = ShaderTranslator::GetShaderOutputLanguageForContext(
            output_context_version);
        ASSERT_TRUE(translator->Init(GL_VERTEX_SHADER, SH_GLES2_SPEC, &resources,
            shader_output_language, compile_options));

        std::string translated_source;
        int shader_version;
        EXPECT_TRUE(translator->Translate(kShader, nullptr, &translated_source,
            &shader_version, nullptr, nullptr, nullptr,
            nullptr));

        std::string expected_version_directive = testing::get<1>(GetParam());
        if (expected_version_directive.empty()) {
            EXPECT_TRUE(translated_source.find("#version") == std::string::npos)
                << "Translation was:\n"
                << translated_source;
        } else {
            EXPECT_TRUE(translated_source.find(expected_version_directive) != std::string::npos)
                << "Translation was:\n"
                << translated_source;
        }
    }

    // For some compilers, using make_tuple("a", "bb") would end up
    // instantiating make_tuple<char[1], char[2]>. This does not work.
    namespace {
        testing::tuple<const char*, const char*> make_gl_glsl_tuple(
            const char* gl_version,
            const char* glsl_version_directive)
        {
            return testing::make_tuple(gl_version, glsl_version_directive);
        }
    }

    // Test data for the above test. OpenGL specifications specify a
    // certain version of GLSL to be guaranteed to be supported. Test
    // that ShaderTranslator produces a GLSL shader with the exact
    // specified GLSL version for each known OpenGL version.
    INSTANTIATE_TEST_CASE_P(
        KnownOpenGLContexts,
        ShaderTranslatorOutputVersionTest,
        testing::Values(make_gl_glsl_tuple("4.5", "#version 450\n"),
            make_gl_glsl_tuple("4.4", "#version 440\n"),
            make_gl_glsl_tuple("4.3", "#version 430\n"),
            make_gl_glsl_tuple("4.2", "#version 420\n"),
            make_gl_glsl_tuple("4.1", "#version 410\n"),
            make_gl_glsl_tuple("4.0", "#version 400\n"),
            make_gl_glsl_tuple("3.3", "#version 330\n"),
            make_gl_glsl_tuple("3.2", "#version 150\n"),
            make_gl_glsl_tuple("3.1", "#version 140\n"),
            make_gl_glsl_tuple("3.0", "#version 130\n")));

    // Test data for the above test. Check that early OpenGL contexts get
    // GLSL compatibility profile shader, e.g. shader has no #version
    // directive. Also check that future version 3.3+ OpenGL contexts get
    // similar shader. We do not expect that future 3.3+ specs contain
    // the "all eariler GLSL versions" clause, since 3.3 did not contain
    // it either.
    INSTANTIATE_TEST_CASE_P(OldOrUnknownOpenGLContexts,
        ShaderTranslatorOutputVersionTest,
        testing::Values(make_gl_glsl_tuple("3.4", ""),
            make_gl_glsl_tuple("2.0", "")));

    // Test data for the above test. Cases for the future OpenGL versions. The
    // code assumes that the future OpenGL specs specify the clause that all
    // earlier GLSL versions are supported. We select the highest GLSL
    // version known at the time of writing.
    INSTANTIATE_TEST_CASE_P(
        BackwardsCompatibleFutureOpenGLContexts,
        ShaderTranslatorOutputVersionTest,
        testing::Values(make_gl_glsl_tuple("5.0", "#version 450\n"),
            make_gl_glsl_tuple("4.6", "#version 450\n")));

    // Test data for the above test. Check that for the OpenGL ES output
    // contexts, the shader is such that GLSL 1.0 is used. The translator
    // selects GLSL 1.0 by not output any version at the moment, though we
    // do not know if that would be correct for the future OpenGL ES specs.
    INSTANTIATE_TEST_CASE_P(OpenGLESContexts,
        ShaderTranslatorOutputVersionTest,
        testing::Values(make_gl_glsl_tuple("opengl es 2.0", ""),
            make_gl_glsl_tuple("opengl es 3.0", ""),
            make_gl_glsl_tuple("opengl es 3.1", ""),
            make_gl_glsl_tuple("opengl es 3.2",
                "")));

} // namespace gles2
} // namespace gpu
