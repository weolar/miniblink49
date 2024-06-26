// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/shader_translator.h"

#include <GLES2/gl2.h>
#include <algorithm>
#include <string.h>

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/trace_event/trace_event.h"
#include "gpu/command_buffer/service/gpu_switches.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_version_info.h"

namespace gpu {
namespace gles2 {

    namespace {

        class ShaderTranslatorInitializer {
        public:
            ShaderTranslatorInitializer()
            {
                TRACE_EVENT0("gpu", "ShInitialize");
                CHECK(ShInitialize());
            }

            ~ShaderTranslatorInitializer()
            {
                TRACE_EVENT0("gpu", "ShFinalize");
                ShFinalize();
            }
        };

        base::LazyInstance<ShaderTranslatorInitializer> g_translator_initializer = LAZY_INSTANCE_INITIALIZER;

        void GetAttributes(ShHandle compiler, AttributeMap* var_map)
        {
            if (!var_map)
                return;
            var_map->clear();
            const std::vector<sh::Attribute>* attribs = ShGetAttributes(compiler);
            if (attribs) {
                for (size_t ii = 0; ii < attribs->size(); ++ii)
                    (*var_map)[(*attribs)[ii].mappedName] = (*attribs)[ii];
            }
        }

        void GetUniforms(ShHandle compiler, UniformMap* var_map)
        {
            if (!var_map)
                return;
            var_map->clear();
            const std::vector<sh::Uniform>* uniforms = ShGetUniforms(compiler);
            if (uniforms) {
                for (size_t ii = 0; ii < uniforms->size(); ++ii)
                    (*var_map)[(*uniforms)[ii].mappedName] = (*uniforms)[ii];
            }
        }

        void GetVaryings(ShHandle compiler, VaryingMap* var_map)
        {
            if (!var_map)
                return;
            var_map->clear();
            const std::vector<sh::Varying>* varyings = ShGetVaryings(compiler);
            if (varyings) {
                for (size_t ii = 0; ii < varyings->size(); ++ii)
                    (*var_map)[(*varyings)[ii].mappedName] = (*varyings)[ii];
            }
        }

        void GetInterfaceBlocks(ShHandle compiler, InterfaceBlockMap* var_map)
        {
            if (!var_map)
                return;
            var_map->clear();
            const std::vector<sh::InterfaceBlock>* interface_blocks = ShGetInterfaceBlocks(compiler);
            if (interface_blocks) {
                for (const auto& block : *interface_blocks) {
                    (*var_map)[block.mappedName] = block;
                }
            }
        }

        void GetNameHashingInfo(ShHandle compiler, NameMap* name_map)
        {
            if (!name_map)
                return;
            name_map->clear();

            typedef std::map<std::string, std::string> NameMapANGLE;
            const NameMapANGLE* angle_map = ShGetNameHashingMap(compiler);
            DCHECK(angle_map);

            for (NameMapANGLE::const_iterator iter = angle_map->begin();
                 iter != angle_map->end(); ++iter) {
                // Note that in ANGLE, the map is (original_name, hash);
                // here, we want (hash, original_name).
                (*name_map)[iter->second] = iter->first;
            }
        }

    } // namespace

    ShShaderOutput ShaderTranslator::GetShaderOutputLanguageForContext(
        const gfx::GLVersionInfo& version_info)
    {
        if (version_info.is_es) {
            return SH_ESSL_OUTPUT;
        }

        // Determine the GLSL version based on OpenGL specification.

        unsigned context_version = version_info.major_version * 100 + version_info.minor_version * 10;
        if (context_version >= 450) {
            // OpenGL specs from 4.2 on specify that the core profile is "also
            // guaranteed to support all previous versions of the OpenGL Shading
            // Language back to version 1.40". For simplicity, we assume future
            // specs do not unspecify this. If they did, they could unspecify
            // glGetStringi(GL_SHADING_LANGUAGE_VERSION, k), too.
            // Since current context >= 4.5, use GLSL 4.50 core.
            return SH_GLSL_450_CORE_OUTPUT;
        } else if (context_version == 440) {
            return SH_GLSL_440_CORE_OUTPUT;
        } else if (context_version == 430) {
            return SH_GLSL_430_CORE_OUTPUT;
        } else if (context_version == 420) {
            return SH_GLSL_420_CORE_OUTPUT;
        } else if (context_version == 410) {
            return SH_GLSL_410_CORE_OUTPUT;
        } else if (context_version == 400) {
            return SH_GLSL_400_CORE_OUTPUT;
        } else if (context_version == 330) {
            return SH_GLSL_330_CORE_OUTPUT;
        } else if (context_version == 320) {
            return SH_GLSL_150_CORE_OUTPUT;
        } else if (context_version == 310) {
            return SH_GLSL_140_OUTPUT;
        } else if (context_version == 300) {
            return SH_GLSL_130_OUTPUT;
        }

        // Before OpenGL 3.0 we use compatibility profile. Also for future
        // specs between OpenGL 3.3 and OpenGL 4.0, at the time of writing,
        // we use compatibility profile.
        return SH_GLSL_COMPATIBILITY_OUTPUT;
    }

    ShaderTranslator::DestructionObserver::DestructionObserver()
    {
    }

    ShaderTranslator::DestructionObserver::~DestructionObserver()
    {
    }

    ShaderTranslator::ShaderTranslator()
        : compiler_(NULL)
        , driver_bug_workarounds_(static_cast<ShCompileOptions>(0))
    {
    }

    bool ShaderTranslator::Init(GLenum shader_type,
        ShShaderSpec shader_spec,
        const ShBuiltInResources* resources,
        ShShaderOutput shader_output_language,
        ShCompileOptions driver_bug_workarounds)
    {
        // Make sure Init is called only once.
        DCHECK(compiler_ == NULL);
        DCHECK(shader_type == GL_FRAGMENT_SHADER || shader_type == GL_VERTEX_SHADER);
        DCHECK(shader_spec == SH_GLES2_SPEC || shader_spec == SH_WEBGL_SPEC || shader_spec == SH_GLES3_SPEC || shader_spec == SH_WEBGL2_SPEC);
        DCHECK(resources != NULL);

        g_translator_initializer.Get();

        {
            TRACE_EVENT0("gpu", "ShConstructCompiler");
            compiler_ = ShConstructCompiler(shader_type, shader_spec,
                shader_output_language, resources);
        }
        driver_bug_workarounds_ = driver_bug_workarounds;
        return compiler_ != NULL;
    }

    int ShaderTranslator::GetCompileOptions() const
    {
        int compile_options = SH_OBJECT_CODE | SH_VARIABLES | SH_ENFORCE_PACKING_RESTRICTIONS | SH_LIMIT_EXPRESSION_COMPLEXITY | SH_LIMIT_CALL_STACK_DEPTH | SH_CLAMP_INDIRECT_ARRAY_BOUNDS;

        //   if (base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kGLShaderIntermOutput))
        //     compile_options |= SH_INTERMEDIATE_TREE;

        compile_options |= driver_bug_workarounds_;

        return compile_options;
    }

    bool ShaderTranslator::Translate(const std::string& shader_source,
        std::string* info_log,
        std::string* translated_source,
        int* shader_version,
        AttributeMap* attrib_map,
        UniformMap* uniform_map,
        VaryingMap* varying_map,
        InterfaceBlockMap* interface_block_map,
        NameMap* name_map) const
    {
        // Make sure this instance is initialized.
        DCHECK(compiler_ != NULL);

        bool success = false;
        {
            TRACE_EVENT0("gpu", "ShCompile");
            const char* const shader_strings[] = { shader_source.c_str() };
            success = ShCompile(
                compiler_, shader_strings, 1, GetCompileOptions());
        }
        if (success) {
            // Get translated shader.
            if (translated_source) {
                *translated_source = ShGetObjectCode(compiler_);
            }
            // Get shader version.
            *shader_version = ShGetShaderVersion(compiler_);
            // Get info for attribs, uniforms, and varyings.
            GetAttributes(compiler_, attrib_map);
            GetUniforms(compiler_, uniform_map);
            GetVaryings(compiler_, varying_map);
            GetInterfaceBlocks(compiler_, interface_block_map);
            // Get info for name hashing.
            GetNameHashingInfo(compiler_, name_map);
        }

        // Get info log.
        if (info_log) {
            *info_log = ShGetInfoLog(compiler_);
        }

        // We don't need results in the compiler anymore.
        ShClearResults(compiler_);

        return success;
    }

    std::string ShaderTranslator::GetStringForOptionsThatWouldAffectCompilation()
        const
    {
        DCHECK(compiler_ != NULL);
        return std::string(":CompileOptions:" + base::IntToString(GetCompileOptions())) + ShGetBuiltInResourcesString(compiler_);
    }

    void ShaderTranslator::AddDestructionObserver(
        DestructionObserver* observer)
    {
        destruction_observers_.AddObserver(observer);
    }

    void ShaderTranslator::RemoveDestructionObserver(
        DestructionObserver* observer)
    {
        destruction_observers_.RemoveObserver(observer);
    }

    ShaderTranslator::~ShaderTranslator()
    {
        FOR_EACH_OBSERVER(DestructionObserver,
            destruction_observers_,
            OnDestruct(this));

        if (compiler_ != NULL)
            ShDestruct(compiler_);
    }

} // namespace gles2
} // namespace gpu
