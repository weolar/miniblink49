// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/shader_translator.h"

#include <string.h>
#include <algorithm>

#include "base/at_exit.h"
#include "base/debug/trace_event.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"

namespace {

using gpu::gles2::ShaderTranslator;

class ShaderTranslatorInitializer {
 public:
  ShaderTranslatorInitializer() {
    TRACE_EVENT0("gpu", "ShInitialize");
    CHECK(ShInitialize());
  }

  ~ShaderTranslatorInitializer() {
    TRACE_EVENT0("gpu", "ShFinalize");
    ShFinalize();
  }
};

base::LazyInstance<ShaderTranslatorInitializer> g_translator_initializer =
    LAZY_INSTANCE_INITIALIZER;

#if !defined(ANGLE_SH_VERSION) || ANGLE_SH_VERSION < 108
typedef int ANGLEGetInfoType;
#else
typedef size_t ANGLEGetInfoType;
#endif

void GetVariableInfo(ShHandle compiler, ShShaderInfo var_type,
                     ShaderTranslator::VariableMap* var_map) {
  ANGLEGetInfoType name_len = 0, mapped_name_len = 0;
  switch (var_type) {
    case SH_ACTIVE_ATTRIBUTES:
      ShGetInfo(compiler, SH_ACTIVE_ATTRIBUTE_MAX_LENGTH, &name_len);
      break;
    case SH_ACTIVE_UNIFORMS:
      ShGetInfo(compiler, SH_ACTIVE_UNIFORM_MAX_LENGTH, &name_len);
      break;
    case SH_VARYINGS:
      ShGetInfo(compiler, SH_VARYING_MAX_LENGTH, &name_len);
      break;
    default: NOTREACHED();
  }
  ShGetInfo(compiler, SH_MAPPED_NAME_MAX_LENGTH, &mapped_name_len);
  if (name_len <= 1 || mapped_name_len <= 1) return;
  scoped_ptr<char[]> name(new char[name_len]);
  scoped_ptr<char[]> mapped_name(new char[mapped_name_len]);

  ANGLEGetInfoType num_vars = 0;
  ShGetInfo(compiler, var_type, &num_vars);
  for (ANGLEGetInfoType i = 0; i < num_vars; ++i) {
    ANGLEGetInfoType len = 0;
    int size = 0;
    ShDataType type = SH_NONE;
    ShPrecisionType precision = SH_PRECISION_UNDEFINED;
    int static_use = 0;

    ShGetVariableInfo(compiler, var_type, i,
                      &len, &size, &type, &precision, &static_use,
                      name.get(), mapped_name.get());

    // In theory we should CHECK(len <= name_len - 1) here, but ANGLE needs
    // to handle long struct field name mapping before we can do this.
    // Also, we should modify the ANGLE interface to also return a length
    // for mapped_name.
    std::string name_string(name.get(), std::min(len, name_len - 1));
    mapped_name.get()[mapped_name_len - 1] = '\0';

    ShaderTranslator::VariableInfo info(
        type, size, precision, static_use, name_string);
    (*var_map)[mapped_name.get()] = info;
  }
}

void GetNameHashingInfo(
    ShHandle compiler, ShaderTranslator::NameMap* name_map) {
  ANGLEGetInfoType hashed_names_count = 0;
  ShGetInfo(compiler, SH_HASHED_NAMES_COUNT, &hashed_names_count);
  if (hashed_names_count == 0)
    return;

  ANGLEGetInfoType name_max_len = 0, hashed_name_max_len = 0;
  ShGetInfo(compiler, SH_NAME_MAX_LENGTH, &name_max_len);
  ShGetInfo(compiler, SH_HASHED_NAME_MAX_LENGTH, &hashed_name_max_len);

  scoped_ptr<char[]> name(new char[name_max_len]);
  scoped_ptr<char[]> hashed_name(new char[hashed_name_max_len]);

  for (ANGLEGetInfoType i = 0; i < hashed_names_count; ++i) {
    ShGetNameHashingEntry(compiler, i, name.get(), hashed_name.get());
    (*name_map)[hashed_name.get()] = name.get();
  }
}

}  // namespace

namespace gpu {
namespace gles2 {

ShaderTranslator::DestructionObserver::DestructionObserver() {
}

ShaderTranslator::DestructionObserver::~DestructionObserver() {
}

ShaderTranslator::ShaderTranslator()
    : compiler_(NULL),
      implementation_is_glsl_es_(false),
      driver_bug_workarounds_(static_cast<ShCompileOptions>(0)) {
}

bool ShaderTranslator::Init(
    ShShaderType shader_type,
    ShShaderSpec shader_spec,
    const ShBuiltInResources* resources,
    ShaderTranslatorInterface::GlslImplementationType glsl_implementation_type,
    ShCompileOptions driver_bug_workarounds) {
  // Make sure Init is called only once.
  DCHECK(compiler_ == NULL);
  DCHECK(shader_type == SH_FRAGMENT_SHADER || shader_type == SH_VERTEX_SHADER);
  DCHECK(shader_spec == SH_GLES2_SPEC || shader_spec == SH_WEBGL_SPEC);
  DCHECK(resources != NULL);

  g_translator_initializer.Get();

  ShShaderOutput shader_output =
      (glsl_implementation_type == kGlslES ? SH_ESSL_OUTPUT : SH_GLSL_OUTPUT);

  {
    TRACE_EVENT0("gpu", "ShConstructCompiler");
    compiler_ = ShConstructCompiler(
        shader_type, shader_spec, shader_output, resources);
  }
  compiler_options_ = *resources;
  implementation_is_glsl_es_ = (glsl_implementation_type == kGlslES);
  driver_bug_workarounds_ = driver_bug_workarounds;
  return compiler_ != NULL;
}

int ShaderTranslator::GetCompileOptions() const {
  int compile_options =
      SH_OBJECT_CODE | SH_VARIABLES | SH_ENFORCE_PACKING_RESTRICTIONS |
      SH_LIMIT_EXPRESSION_COMPLEXITY | SH_LIMIT_CALL_STACK_DEPTH |
      SH_CLAMP_INDIRECT_ARRAY_BOUNDS;

  compile_options |= driver_bug_workarounds_;

  return compile_options;
}

bool ShaderTranslator::Translate(const char* shader) {
  // Make sure this instance is initialized.
  DCHECK(compiler_ != NULL);
  DCHECK(shader != NULL);
  ClearResults();

  bool success = false;
  {
    TRACE_EVENT0("gpu", "ShCompile");
    success = !!ShCompile(compiler_, &shader, 1, GetCompileOptions());
  }
  if (success) {
    // Get translated shader.
    ANGLEGetInfoType obj_code_len = 0;
    ShGetInfo(compiler_, SH_OBJECT_CODE_LENGTH, &obj_code_len);
    if (obj_code_len > 1) {
      translated_shader_.reset(new char[obj_code_len]);
      ShGetObjectCode(compiler_, translated_shader_.get());
    }
    // Get info for attribs and uniforms.
    GetVariableInfo(compiler_, SH_ACTIVE_ATTRIBUTES, &attrib_map_);
    GetVariableInfo(compiler_, SH_ACTIVE_UNIFORMS, &uniform_map_);
    GetVariableInfo(compiler_, SH_VARYINGS, &varying_map_);
    // Get info for name hashing.
    GetNameHashingInfo(compiler_, &name_map_);
  }

  // Get info log.
  ANGLEGetInfoType info_log_len = 0;
  ShGetInfo(compiler_, SH_INFO_LOG_LENGTH, &info_log_len);
  if (info_log_len > 1) {
    info_log_.reset(new char[info_log_len]);
    ShGetInfoLog(compiler_, info_log_.get());
  } else {
    info_log_.reset();
  }

  return success;
}

std::string ShaderTranslator::GetStringForOptionsThatWouldAffectCompilation()
    const {
#if ANGLE_SH_VERSION >= 124
  DCHECK(compiler_ != NULL);

  ANGLEGetInfoType resource_len = 0;
  ShGetInfo(compiler_, SH_RESOURCES_STRING_LENGTH, &resource_len);
  DCHECK(resource_len > 1);
  scoped_ptr<char[]> resource_str(new char[resource_len]);

  ShGetBuiltInResourcesString(compiler_, resource_len, resource_str.get());

  return std::string(":CompileOptions:" +
         base::IntToString(GetCompileOptions())) +
         std::string(resource_str.get());
#else
#if ANGLE_SH_VERSION >= 123
  const size_t kNumIntFields = 21;
#elif ANGLE_SH_VERSION >= 122
  const size_t kNumIntFields = 20;
#else
  const size_t kNumIntFields = 16;
#endif
  const size_t kNumEnumFields = 1;
  const size_t kNumFunctionPointerFields = 1;
  struct MustMatchShBuiltInResource {
    typedef khronos_uint64_t (*FunctionPointer)(const char*, size_t);
    enum Enum {
      kFirst,
    };
    int int_fields[kNumIntFields];
    FunctionPointer pointer_fields[kNumFunctionPointerFields];
    Enum enum_fields[kNumEnumFields];
  };
  // If this assert fails most likely that means something below needs updating.
  COMPILE_ASSERT(
      sizeof(ShBuiltInResources) == sizeof(MustMatchShBuiltInResource),
      Fields_Have_Changed_In_ShBuiltInResource_So_Update_Below);

  return std::string(
      ":CompileOptions:" +
      base::IntToString(GetCompileOptions()) +
      ":MaxVertexAttribs:" +
      base::IntToString(compiler_options_.MaxVertexAttribs) +
      ":MaxVertexUniformVectors:" +
      base::IntToString(compiler_options_.MaxVertexUniformVectors) +
      ":MaxVaryingVectors:" +
      base::IntToString(compiler_options_.MaxVaryingVectors) +
      ":MaxVertexTextureImageUnits:" +
      base::IntToString(compiler_options_.MaxVertexTextureImageUnits) +
      ":MaxCombinedTextureImageUnits:" +
      base::IntToString(compiler_options_.MaxCombinedTextureImageUnits) +
      ":MaxTextureImageUnits:" +
      base::IntToString(compiler_options_.MaxTextureImageUnits) +
      ":MaxFragmentUniformVectors:" +
      base::IntToString(compiler_options_.MaxFragmentUniformVectors) +
      ":MaxDrawBuffers:" +
      base::IntToString(compiler_options_.MaxDrawBuffers) +
      ":OES_standard_derivatives:" +
      base::IntToString(compiler_options_.OES_standard_derivatives) +
      ":OES_EGL_image_external:" +
      base::IntToString(compiler_options_.OES_EGL_image_external) +
      ":ARB_texture_rectangle:" +
      base::IntToString(compiler_options_.ARB_texture_rectangle) +
      ":EXT_draw_buffers:" +
      base::IntToString(compiler_options_.EXT_draw_buffers) +
      ":FragmentPrecisionHigh:" +
      base::IntToString(compiler_options_.FragmentPrecisionHigh) +
      ":MaxExpressionComplexity:" +
      base::IntToString(compiler_options_.MaxExpressionComplexity) +
      ":MaxCallStackDepth:" +
      base::IntToString(compiler_options_.MaxCallStackDepth) +
      ":EXT_frag_depth:" +
#if ANGLE_SH_VERSION >= 122
      base::IntToString(compiler_options_.EXT_frag_depth) +
#if ANGLE_SH_VERSION >= 123
      ":EXT_shader_texture_lod:" +
      base::IntToString(compiler_options_.EXT_shader_texture_lod) +
#endif
      ":MaxVertexOutputVectors:" +
      base::IntToString(compiler_options_.MaxVertexOutputVectors) +
      ":MaxFragmentInputVectors:" +
      base::IntToString(compiler_options_.MaxFragmentInputVectors) +
      ":MinProgramTexelOffset:" +
      base::IntToString(compiler_options_.MinProgramTexelOffset) +
      ":MaxProgramTexelOffset:" +
      base::IntToString(compiler_options_.MaxProgramTexelOffset));
#else   // ANGLE_SH_VERSION < 122
      base::IntToString(compiler_options_.EXT_frag_depth));
#endif
#endif
}

const char* ShaderTranslator::translated_shader() const {
  return translated_shader_.get();
}

const char* ShaderTranslator::info_log() const {
  return info_log_.get();
}

const ShaderTranslatorInterface::VariableMap&
ShaderTranslator::attrib_map() const {
  return attrib_map_;
}

const ShaderTranslatorInterface::VariableMap&
ShaderTranslator::uniform_map() const {
  return uniform_map_;
}

const ShaderTranslatorInterface::VariableMap&
ShaderTranslator::varying_map() const {
  return varying_map_;
}

const ShaderTranslatorInterface::NameMap&
ShaderTranslator::name_map() const {
  return name_map_;
}

void ShaderTranslator::AddDestructionObserver(
    DestructionObserver* observer) {
  destruction_observers_.AddObserver(observer);
}

void ShaderTranslator::RemoveDestructionObserver(
    DestructionObserver* observer) {
  destruction_observers_.RemoveObserver(observer);
}

ShaderTranslator::~ShaderTranslator() {
  FOR_EACH_OBSERVER(DestructionObserver,
                    destruction_observers_,
                    OnDestruct(this));

  if (compiler_ != NULL)
    ShDestruct(compiler_);
}

void ShaderTranslator::ClearResults() {
  translated_shader_.reset();
  info_log_.reset();
  attrib_map_.clear();
  uniform_map_.clear();
  varying_map_.clear();
  name_map_.clear();
}

}  // namespace gles2
}  // namespace gpu

