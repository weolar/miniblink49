// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/feature_info.h"

#include <set>

#include "base/command_line.h"
#include "base/macros.h"
#include "base/metrics/histogram.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "gpu/command_buffer/service/gl_utils.h"
#include "gpu/command_buffer/service/gpu_switches.h"
#include "ui/gl/gl_fence.h"
#include "ui/gl/gl_implementation.h"

namespace gpu {
namespace gles2 {

namespace {

struct FormatInfo {
  GLenum format;
  const GLenum* types;
  size_t count;
};

class StringSet {
 public:
  StringSet() {}

  StringSet(const char* s) {
    Init(s);
  }

  StringSet(const std::string& str) {
    Init(str);
  }

  void Init(const char* s) {
    std::string str(s ? s : "");
    Init(str);
  }

  void Init(const std::string& str) {
    std::vector<std::string> tokens;
    Tokenize(str, " ", &tokens);
    string_set_.insert(tokens.begin(), tokens.end());
  }

  bool Contains(const char* s) {
    return string_set_.find(s) != string_set_.end();
  }

  bool Contains(const std::string& s) {
    return string_set_.find(s) != string_set_.end();
  }

 private:
  std::set<std::string> string_set_;
};

// Process a string of wordaround type IDs (seperated by ',') and set up
// the corresponding Workaround flags.
void StringToWorkarounds(
    const std::string& types, FeatureInfo::Workarounds* workarounds) {
  DCHECK(workarounds);
  std::vector<std::string> pieces;
  base::SplitString(types, ',', &pieces);
  for (size_t i = 0; i < pieces.size(); ++i) {
    int number = 0;
    bool succeed = base::StringToInt(pieces[i], &number);
    DCHECK(succeed);
    switch (number) {
#define GPU_OP(type, name)    \
  case gpu::type:             \
    workarounds->name = true; \
    break;
      GPU_DRIVER_BUG_WORKAROUNDS(GPU_OP)
#undef GPU_OP
      default:
        NOTIMPLEMENTED();
    }
  }
  if (workarounds->max_texture_size_limit_4096)
    workarounds->max_texture_size = 4096;
  if (workarounds->max_cube_map_texture_size_limit_4096)
    workarounds->max_cube_map_texture_size = 4096;
  if (workarounds->max_cube_map_texture_size_limit_1024)
    workarounds->max_cube_map_texture_size = 1024;
  if (workarounds->max_cube_map_texture_size_limit_512)
    workarounds->max_cube_map_texture_size = 512;

  if (workarounds->max_fragment_uniform_vectors_32)
    workarounds->max_fragment_uniform_vectors = 32;
  if (workarounds->max_varying_vectors_16)
    workarounds->max_varying_vectors = 16;
  if (workarounds->max_vertex_uniform_vectors_256)
    workarounds->max_vertex_uniform_vectors = 256;
}

}  // anonymous namespace.

FeatureInfo::FeatureFlags::FeatureFlags()
    : chromium_color_buffer_float_rgba(false),
      chromium_color_buffer_float_rgb(false),
      chromium_framebuffer_multisample(false),
      chromium_sync_query(false),
      use_core_framebuffer_multisample(false),
      multisampled_render_to_texture(false),
      use_img_for_multisampled_render_to_texture(false),
      oes_standard_derivatives(false),
      oes_egl_image_external(false),
      oes_depth24(false),
      oes_compressed_etc1_rgb8_texture(false),
      packed_depth24_stencil8(false),
      npot_ok(false),
      enable_texture_float_linear(false),
      enable_texture_half_float_linear(false),
      angle_translated_shader_source(false),
      angle_pack_reverse_row_order(false),
      arb_texture_rectangle(false),
      angle_instanced_arrays(false),
      occlusion_query_boolean(false),
      use_arb_occlusion_query2_for_occlusion_query_boolean(false),
      use_arb_occlusion_query_for_occlusion_query_boolean(false),
      native_vertex_array_object(false),
      ext_texture_format_bgra8888(false),
      enable_shader_name_hashing(false),
      enable_samplers(false),
      ext_draw_buffers(false),
      ext_frag_depth(false),
      ext_shader_texture_lod(false),
      use_async_readpixels(false),
      map_buffer_range(false),
      ext_discard_framebuffer(false),
      angle_depth_texture(false),
      is_angle(false),
      is_swiftshader(false),
      angle_texture_usage(false),
      ext_texture_storage(false) {
}

FeatureInfo::Workarounds::Workarounds() :
#define GPU_OP(type, name) name(false),
    GPU_DRIVER_BUG_WORKAROUNDS(GPU_OP)
#undef GPU_OP
    max_texture_size(0),
    max_cube_map_texture_size(0),
    max_fragment_uniform_vectors(0),
    max_varying_vectors(0),
    max_vertex_uniform_vectors(0) {
}

FeatureInfo::FeatureInfo() {
  InitializeBasicState(*CommandLine::ForCurrentProcess());
}

FeatureInfo::FeatureInfo(const CommandLine& command_line) {
  InitializeBasicState(command_line);
}

void FeatureInfo::InitializeBasicState(const CommandLine& command_line) {
  if (command_line.HasSwitch(switches::kGpuDriverBugWorkarounds)) {
    std::string types = command_line.GetSwitchValueASCII(
        switches::kGpuDriverBugWorkarounds);
    StringToWorkarounds(types, &workarounds_);
  }
  feature_flags_.enable_shader_name_hashing =
      !command_line.HasSwitch(switches::kDisableShaderNameHashing);

  feature_flags_.is_swiftshader =
      (command_line.GetSwitchValueASCII(switches::kUseGL) == "swiftshader");

  static const GLenum kAlphaTypes[] = {
      GL_UNSIGNED_BYTE,
  };
  static const GLenum kRGBTypes[] = {
      GL_UNSIGNED_BYTE,
      GL_UNSIGNED_SHORT_5_6_5,
  };
  static const GLenum kRGBATypes[] = {
      GL_UNSIGNED_BYTE,
      GL_UNSIGNED_SHORT_4_4_4_4,
      GL_UNSIGNED_SHORT_5_5_5_1,
  };
  static const GLenum kLuminanceTypes[] = {
      GL_UNSIGNED_BYTE,
  };
  static const GLenum kLuminanceAlphaTypes[] = {
      GL_UNSIGNED_BYTE,
  };
  static const FormatInfo kFormatTypes[] = {
    { GL_ALPHA, kAlphaTypes, arraysize(kAlphaTypes), },
    { GL_RGB, kRGBTypes, arraysize(kRGBTypes), },
    { GL_RGBA, kRGBATypes, arraysize(kRGBATypes), },
    { GL_LUMINANCE, kLuminanceTypes, arraysize(kLuminanceTypes), },
    { GL_LUMINANCE_ALPHA, kLuminanceAlphaTypes,
      arraysize(kLuminanceAlphaTypes), } ,
  };
  for (size_t ii = 0; ii < arraysize(kFormatTypes); ++ii) {
    const FormatInfo& info = kFormatTypes[ii];
    ValueValidator<GLenum>& validator = texture_format_validators_[info.format];
    for (size_t jj = 0; jj < info.count; ++jj) {
      validator.AddValue(info.types[jj]);
    }
  }
}

bool FeatureInfo::Initialize() {
  disallowed_features_ = DisallowedFeatures();
  InitializeFeatures();
  return true;
}

bool FeatureInfo::Initialize(const DisallowedFeatures& disallowed_features) {
  disallowed_features_ = disallowed_features;
  InitializeFeatures();
  return true;
}

void FeatureInfo::InitializeFeatures() {
  // Figure out what extensions to turn on.
  StringSet extensions(
      reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));

  bool npot_ok = false;

  const char* renderer_str =
      reinterpret_cast<const char*>(glGetString(GL_RENDERER));
  if (renderer_str) {
    feature_flags_.is_angle = StartsWithASCII(renderer_str, "ANGLE", true);
  }

  bool is_es3 = false;
  const char* version_str =
      reinterpret_cast<const char*>(glGetString(GL_VERSION));
  if (version_str) {
    std::string lstr(StringToLowerASCII(std::string(version_str)));
    is_es3 = (lstr.substr(0, 12) == "opengl es 3.");
  }

  AddExtensionString("GL_ANGLE_translated_shader_source");
  AddExtensionString("GL_CHROMIUM_async_pixel_transfers");
  AddExtensionString("GL_CHROMIUM_bind_uniform_location");
  AddExtensionString("GL_CHROMIUM_command_buffer_query");
  AddExtensionString("GL_CHROMIUM_command_buffer_latency_query");
  AddExtensionString("GL_CHROMIUM_copy_texture");
  AddExtensionString("GL_CHROMIUM_get_error_query");
  AddExtensionString("GL_CHROMIUM_lose_context");
  AddExtensionString("GL_CHROMIUM_pixel_transfer_buffer_object");
  AddExtensionString("GL_CHROMIUM_rate_limit_offscreen_context");
  AddExtensionString("GL_CHROMIUM_resize");
  AddExtensionString("GL_CHROMIUM_resource_safe");
  AddExtensionString("GL_CHROMIUM_strict_attribs");
  AddExtensionString("GL_CHROMIUM_texture_mailbox");
  AddExtensionString("GL_EXT_debug_marker");

  // OES_vertex_array_object is emulated if not present natively,
  // so the extension string is always exposed.
  AddExtensionString("GL_OES_vertex_array_object");

  if (!disallowed_features_.gpu_memory_manager)
    AddExtensionString("GL_CHROMIUM_gpu_memory_manager");

  if (extensions.Contains("GL_ANGLE_translated_shader_source")) {
    feature_flags_.angle_translated_shader_source = true;
  }

  // Check if we should allow GL_EXT_texture_compression_dxt1 and
  // GL_EXT_texture_compression_s3tc.
  bool enable_dxt1 = false;
  bool enable_dxt3 = false;
  bool enable_dxt5 = false;
  bool have_s3tc = extensions.Contains("GL_EXT_texture_compression_s3tc");
  bool have_dxt3 =
      have_s3tc || extensions.Contains("GL_ANGLE_texture_compression_dxt3");
  bool have_dxt5 =
      have_s3tc || extensions.Contains("GL_ANGLE_texture_compression_dxt5");

  if (extensions.Contains("GL_EXT_texture_compression_dxt1") || have_s3tc) {
    enable_dxt1 = true;
  }
  if (have_dxt3) {
    enable_dxt3 = true;
  }
  if (have_dxt5) {
    enable_dxt5 = true;
  }

  if (enable_dxt1) {
    AddExtensionString("GL_EXT_texture_compression_dxt1");
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
  }

  if (enable_dxt3) {
    // The difference between GL_EXT_texture_compression_s3tc and
    // GL_CHROMIUM_texture_compression_dxt3 is that the former
    // requires on the fly compression. The latter does not.
    AddExtensionString("GL_CHROMIUM_texture_compression_dxt3");
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
  }

  if (enable_dxt5) {
    // The difference between GL_EXT_texture_compression_s3tc and
    // GL_CHROMIUM_texture_compression_dxt5 is that the former
    // requires on the fly compression. The latter does not.
    AddExtensionString("GL_CHROMIUM_texture_compression_dxt5");
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
  }

  // Check if we should enable GL_EXT_texture_filter_anisotropic.
  if (extensions.Contains("GL_EXT_texture_filter_anisotropic")) {
    AddExtensionString("GL_EXT_texture_filter_anisotropic");
    validators_.texture_parameter.AddValue(
        GL_TEXTURE_MAX_ANISOTROPY_EXT);
    validators_.g_l_state.AddValue(
        GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);
  }

  // Check if we should support GL_OES_packed_depth_stencil and/or
  // GL_GOOGLE_depth_texture / GL_CHROMIUM_depth_texture.
  //
  // NOTE: GL_OES_depth_texture requires support for depth cubemaps.
  // GL_ARB_depth_texture requires other features that
  // GL_OES_packed_depth_stencil does not provide.
  //
  // Therefore we made up GL_GOOGLE_depth_texture / GL_CHROMIUM_depth_texture.
  //
  // GL_GOOGLE_depth_texture is legacy. As we exposed it into NaCl we can't
  // get rid of it.
  //
  bool enable_depth_texture = false;
  if (!workarounds_.disable_depth_texture &&
      (extensions.Contains("GL_ARB_depth_texture") ||
       extensions.Contains("GL_OES_depth_texture") ||
       extensions.Contains("GL_ANGLE_depth_texture") || is_es3)) {
    enable_depth_texture = true;
    feature_flags_.angle_depth_texture =
        extensions.Contains("GL_ANGLE_depth_texture");
  }

  if (enable_depth_texture) {
    AddExtensionString("GL_CHROMIUM_depth_texture");
    AddExtensionString("GL_GOOGLE_depth_texture");
    texture_format_validators_[GL_DEPTH_COMPONENT].AddValue(GL_UNSIGNED_SHORT);
    texture_format_validators_[GL_DEPTH_COMPONENT].AddValue(GL_UNSIGNED_INT);
    validators_.texture_internal_format.AddValue(GL_DEPTH_COMPONENT);
    validators_.texture_format.AddValue(GL_DEPTH_COMPONENT);
    validators_.pixel_type.AddValue(GL_UNSIGNED_SHORT);
    validators_.pixel_type.AddValue(GL_UNSIGNED_INT);
  }

  if (extensions.Contains("GL_EXT_packed_depth_stencil") ||
      extensions.Contains("GL_OES_packed_depth_stencil") || is_es3) {
    AddExtensionString("GL_OES_packed_depth_stencil");
    feature_flags_.packed_depth24_stencil8 = true;
    if (enable_depth_texture) {
      texture_format_validators_[GL_DEPTH_STENCIL]
          .AddValue(GL_UNSIGNED_INT_24_8);
      validators_.texture_internal_format.AddValue(GL_DEPTH_STENCIL);
      validators_.texture_format.AddValue(GL_DEPTH_STENCIL);
      validators_.pixel_type.AddValue(GL_UNSIGNED_INT_24_8);
    }
    validators_.render_buffer_format.AddValue(GL_DEPTH24_STENCIL8);
  }

  if (extensions.Contains("GL_OES_vertex_array_object") ||
      extensions.Contains("GL_ARB_vertex_array_object") ||
      extensions.Contains("GL_APPLE_vertex_array_object")) {
    feature_flags_.native_vertex_array_object = true;
  }

  // If we're using client_side_arrays we have to emulate
  // vertex array objects since vertex array objects do not work
  // with client side arrays.
  if (workarounds_.use_client_side_arrays_for_stream_buffers) {
    feature_flags_.native_vertex_array_object = false;
  }

  if (extensions.Contains("GL_OES_element_index_uint") ||
      gfx::HasDesktopGLFeatures()) {
    AddExtensionString("GL_OES_element_index_uint");
    validators_.index_type.AddValue(GL_UNSIGNED_INT);
  }

  bool enable_texture_format_bgra8888 = false;
  bool enable_read_format_bgra = false;
  bool enable_render_buffer_bgra = false;

  // Check if we should allow GL_EXT_texture_format_BGRA8888
  if (extensions.Contains("GL_EXT_texture_format_BGRA8888") ||
      extensions.Contains("GL_APPLE_texture_format_BGRA8888") ||
      extensions.Contains("GL_EXT_bgra")) {
    enable_texture_format_bgra8888 = true;
  }

  if (extensions.Contains("GL_EXT_bgra")) {
    enable_render_buffer_bgra = true;
  }

  if (extensions.Contains("GL_EXT_read_format_bgra") ||
      extensions.Contains("GL_EXT_bgra")) {
    enable_read_format_bgra = true;
  }

  if (enable_texture_format_bgra8888) {
    feature_flags_.ext_texture_format_bgra8888 = true;
    AddExtensionString("GL_EXT_texture_format_BGRA8888");
    texture_format_validators_[GL_BGRA_EXT].AddValue(GL_UNSIGNED_BYTE);
    validators_.texture_internal_format.AddValue(GL_BGRA_EXT);
    validators_.texture_format.AddValue(GL_BGRA_EXT);
  }

  if (enable_read_format_bgra) {
    AddExtensionString("GL_EXT_read_format_bgra");
    validators_.read_pixel_format.AddValue(GL_BGRA_EXT);
  }

  if (enable_render_buffer_bgra) {
    AddExtensionString("GL_CHROMIUM_renderbuffer_format_BGRA8888");
    validators_.render_buffer_format.AddValue(GL_BGRA8_EXT);
  }

  if (extensions.Contains("GL_OES_rgb8_rgba8") || gfx::HasDesktopGLFeatures()) {
    AddExtensionString("GL_OES_rgb8_rgba8");
    validators_.render_buffer_format.AddValue(GL_RGB8_OES);
    validators_.render_buffer_format.AddValue(GL_RGBA8_OES);
  }

  // Check if we should allow GL_OES_texture_npot
  if (extensions.Contains("GL_ARB_texture_non_power_of_two") ||
      extensions.Contains("GL_OES_texture_npot")) {
    AddExtensionString("GL_OES_texture_npot");
    npot_ok = true;
  }

  // Check if we should allow GL_OES_texture_float, GL_OES_texture_half_float,
  // GL_OES_texture_float_linear, GL_OES_texture_half_float_linear
  bool enable_texture_float = false;
  bool enable_texture_float_linear = false;
  bool enable_texture_half_float = false;
  bool enable_texture_half_float_linear = false;

  bool may_enable_chromium_color_buffer_float = false;

  if (extensions.Contains("GL_ARB_texture_float")) {
    enable_texture_float = true;
    enable_texture_float_linear = true;
    enable_texture_half_float = true;
    enable_texture_half_float_linear = true;
    may_enable_chromium_color_buffer_float = true;
  } else {
    if (extensions.Contains("GL_OES_texture_float")) {
      enable_texture_float = true;
      if (extensions.Contains("GL_OES_texture_float_linear")) {
        enable_texture_float_linear = true;
      }
      if ((is_es3 && extensions.Contains("GL_EXT_color_buffer_float")) ||
          feature_flags_.is_angle) {
        may_enable_chromium_color_buffer_float = true;
      }
    }
    if (extensions.Contains("GL_OES_texture_half_float")) {
      enable_texture_half_float = true;
      if (extensions.Contains("GL_OES_texture_half_float_linear")) {
        enable_texture_half_float_linear = true;
      }
    }
  }

  if (enable_texture_float) {
    texture_format_validators_[GL_ALPHA].AddValue(GL_FLOAT);
    texture_format_validators_[GL_RGB].AddValue(GL_FLOAT);
    texture_format_validators_[GL_RGBA].AddValue(GL_FLOAT);
    texture_format_validators_[GL_LUMINANCE].AddValue(GL_FLOAT);
    texture_format_validators_[GL_LUMINANCE_ALPHA].AddValue(GL_FLOAT);
    validators_.pixel_type.AddValue(GL_FLOAT);
    validators_.read_pixel_type.AddValue(GL_FLOAT);
    AddExtensionString("GL_OES_texture_float");
    if (enable_texture_float_linear) {
      AddExtensionString("GL_OES_texture_float_linear");
    }
  }

  if (enable_texture_half_float) {
    texture_format_validators_[GL_ALPHA].AddValue(GL_HALF_FLOAT_OES);
    texture_format_validators_[GL_RGB].AddValue(GL_HALF_FLOAT_OES);
    texture_format_validators_[GL_RGBA].AddValue(GL_HALF_FLOAT_OES);
    texture_format_validators_[GL_LUMINANCE].AddValue(GL_HALF_FLOAT_OES);
    texture_format_validators_[GL_LUMINANCE_ALPHA].AddValue(GL_HALF_FLOAT_OES);
    validators_.pixel_type.AddValue(GL_HALF_FLOAT_OES);
    validators_.read_pixel_type.AddValue(GL_HALF_FLOAT_OES);
    AddExtensionString("GL_OES_texture_half_float");
    if (enable_texture_half_float_linear) {
      AddExtensionString("GL_OES_texture_half_float_linear");
    }
  }

  if (may_enable_chromium_color_buffer_float) {
    COMPILE_ASSERT(GL_RGBA32F_ARB == GL_RGBA32F &&
                   GL_RGBA32F_EXT == GL_RGBA32F &&
                   GL_RGB32F_ARB == GL_RGB32F &&
                   GL_RGB32F_EXT == GL_RGB32F,
                   sized_float_internal_format_variations_must_match);
    // We don't check extension support beyond ARB_texture_float on desktop GL,
    // and format support varies between GL configurations. For example, spec
    // prior to OpenGL 3.0 mandates framebuffer support only for one
    // implementation-chosen format, and ES3.0 EXT_color_buffer_float does not
    // support rendering to RGB32F. Check for framebuffer completeness with
    // formats that the extensions expose, and only enable an extension when a
    // framebuffer created with its texture format is reported as complete.
    GLint fb_binding = 0;
    GLint tex_binding = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fb_binding);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &tex_binding);

    GLuint tex_id = 0;
    GLuint fb_id = 0;
    GLsizei width = 16;

    glGenTextures(1, &tex_id);
    glGenFramebuffersEXT(1, &fb_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    // Nearest filter needed for framebuffer completeness on some drivers.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, width, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glBindFramebufferEXT(GL_FRAMEBUFFER, fb_id);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_TEXTURE_2D, tex_id, 0);
    GLenum statusRGBA = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, width, 0, GL_RGB,
                 GL_FLOAT, NULL);
    GLenum statusRGB = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
    glDeleteFramebuffersEXT(1, &fb_id);
    glDeleteTextures(1, &tex_id);

    glBindFramebufferEXT(GL_FRAMEBUFFER, static_cast<GLuint>(fb_binding));
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(tex_binding));

    DCHECK(glGetError() == GL_NO_ERROR);

    if (statusRGBA == GL_FRAMEBUFFER_COMPLETE) {
      validators_.texture_internal_format.AddValue(GL_RGBA32F);
      feature_flags_.chromium_color_buffer_float_rgba = true;
      AddExtensionString("GL_CHROMIUM_color_buffer_float_rgba");
    }
    if (statusRGB == GL_FRAMEBUFFER_COMPLETE) {
      validators_.texture_internal_format.AddValue(GL_RGB32F);
      feature_flags_.chromium_color_buffer_float_rgb = true;
      AddExtensionString("GL_CHROMIUM_color_buffer_float_rgb");
    }
  }

  // Check for multisample support
  if (!workarounds_.disable_multisampling) {
    bool ext_has_multisample =
        extensions.Contains("GL_EXT_framebuffer_multisample") || is_es3;
    if (feature_flags_.is_angle) {
      ext_has_multisample |=
          extensions.Contains("GL_ANGLE_framebuffer_multisample");
    }
    feature_flags_.use_core_framebuffer_multisample = is_es3;
    if (ext_has_multisample) {
      feature_flags_.chromium_framebuffer_multisample = true;
      validators_.frame_buffer_target.AddValue(GL_READ_FRAMEBUFFER_EXT);
      validators_.frame_buffer_target.AddValue(GL_DRAW_FRAMEBUFFER_EXT);
      validators_.g_l_state.AddValue(GL_READ_FRAMEBUFFER_BINDING_EXT);
      validators_.g_l_state.AddValue(GL_MAX_SAMPLES_EXT);
      validators_.render_buffer_parameter.AddValue(GL_RENDERBUFFER_SAMPLES_EXT);
      AddExtensionString("GL_CHROMIUM_framebuffer_multisample");
    }
    if (extensions.Contains("GL_EXT_multisampled_render_to_texture")) {
      feature_flags_.multisampled_render_to_texture = true;
    } else if (extensions.Contains("GL_IMG_multisampled_render_to_texture")) {
      feature_flags_.multisampled_render_to_texture = true;
      feature_flags_.use_img_for_multisampled_render_to_texture = true;
    }
    if (feature_flags_.multisampled_render_to_texture) {
      validators_.render_buffer_parameter.AddValue(
          GL_RENDERBUFFER_SAMPLES_EXT);
      validators_.g_l_state.AddValue(GL_MAX_SAMPLES_EXT);
      validators_.frame_buffer_parameter.AddValue(
          GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT);
      AddExtensionString("GL_EXT_multisampled_render_to_texture");
    }
  }

  if (extensions.Contains("GL_OES_depth24") || gfx::HasDesktopGLFeatures() ||
      is_es3) {
    AddExtensionString("GL_OES_depth24");
    feature_flags_.oes_depth24 = true;
    validators_.render_buffer_format.AddValue(GL_DEPTH_COMPONENT24);
  }

  if (!workarounds_.disable_oes_standard_derivatives &&
      (extensions.Contains("GL_OES_standard_derivatives") ||
       gfx::HasDesktopGLFeatures())) {
    AddExtensionString("GL_OES_standard_derivatives");
    feature_flags_.oes_standard_derivatives = true;
    validators_.hint_target.AddValue(GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES);
    validators_.g_l_state.AddValue(GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES);
  }

  if (extensions.Contains("GL_OES_EGL_image_external")) {
    AddExtensionString("GL_OES_EGL_image_external");
    feature_flags_.oes_egl_image_external = true;
    validators_.texture_bind_target.AddValue(GL_TEXTURE_EXTERNAL_OES);
    validators_.get_tex_param_target.AddValue(GL_TEXTURE_EXTERNAL_OES);
    validators_.texture_parameter.AddValue(GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES);
    validators_.g_l_state.AddValue(GL_TEXTURE_BINDING_EXTERNAL_OES);
  }

  if (extensions.Contains("GL_OES_compressed_ETC1_RGB8_texture")) {
    AddExtensionString("GL_OES_compressed_ETC1_RGB8_texture");
    feature_flags_.oes_compressed_etc1_rgb8_texture = true;
    validators_.compressed_texture_format.AddValue(GL_ETC1_RGB8_OES);
  }

  if (extensions.Contains("GL_AMD_compressed_ATC_texture")) {
    AddExtensionString("GL_AMD_compressed_ATC_texture");
    validators_.compressed_texture_format.AddValue(
        GL_ATC_RGB_AMD);
    validators_.compressed_texture_format.AddValue(
        GL_ATC_RGBA_EXPLICIT_ALPHA_AMD);
    validators_.compressed_texture_format.AddValue(
        GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD);
  }

  if (extensions.Contains("GL_IMG_texture_compression_pvrtc")) {
    AddExtensionString("GL_IMG_texture_compression_pvrtc");
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG);
  }

  // Ideally we would only expose this extension on Mac OS X, to
  // support GL_CHROMIUM_iosurface and the compositor. We don't want
  // applications to start using it; they should use ordinary non-
  // power-of-two textures. However, for unit testing purposes we
  // expose it on all supported platforms.
  if (extensions.Contains("GL_ARB_texture_rectangle")) {
    AddExtensionString("GL_ARB_texture_rectangle");
    feature_flags_.arb_texture_rectangle = true;
    validators_.texture_bind_target.AddValue(GL_TEXTURE_RECTANGLE_ARB);
    // For the moment we don't add this enum to the texture_target
    // validator. This implies that the only way to get image data into a
    // rectangular texture is via glTexImageIOSurface2DCHROMIUM, which is
    // just fine since again we don't want applications depending on this
    // extension.
    validators_.get_tex_param_target.AddValue(GL_TEXTURE_RECTANGLE_ARB);
    validators_.g_l_state.AddValue(GL_TEXTURE_BINDING_RECTANGLE_ARB);
  }

#if defined(OS_MACOSX)
  AddExtensionString("GL_CHROMIUM_iosurface");
#endif

  // TODO(gman): Add support for these extensions.
  //     GL_OES_depth32

  feature_flags_.enable_texture_float_linear |= enable_texture_float_linear;
  feature_flags_.enable_texture_half_float_linear |=
      enable_texture_half_float_linear;
  feature_flags_.npot_ok |= npot_ok;

  if (extensions.Contains("GL_ANGLE_pack_reverse_row_order")) {
    AddExtensionString("GL_ANGLE_pack_reverse_row_order");
    feature_flags_.angle_pack_reverse_row_order = true;
    validators_.pixel_store.AddValue(GL_PACK_REVERSE_ROW_ORDER_ANGLE);
    validators_.g_l_state.AddValue(GL_PACK_REVERSE_ROW_ORDER_ANGLE);
  }

  if (extensions.Contains("GL_ANGLE_texture_usage")) {
    feature_flags_.angle_texture_usage = true;
    AddExtensionString("GL_ANGLE_texture_usage");
    validators_.texture_parameter.AddValue(GL_TEXTURE_USAGE_ANGLE);
  }

  if (extensions.Contains("GL_EXT_texture_storage")) {
    feature_flags_.ext_texture_storage = true;
    AddExtensionString("GL_EXT_texture_storage");
    validators_.texture_parameter.AddValue(GL_TEXTURE_IMMUTABLE_FORMAT_EXT);
    if (enable_texture_format_bgra8888)
        validators_.texture_internal_format_storage.AddValue(GL_BGRA8_EXT);
    if (enable_texture_float) {
        validators_.texture_internal_format_storage.AddValue(GL_RGBA32F_EXT);
        validators_.texture_internal_format_storage.AddValue(GL_RGB32F_EXT);
        validators_.texture_internal_format_storage.AddValue(GL_ALPHA32F_EXT);
        validators_.texture_internal_format_storage.AddValue(
            GL_LUMINANCE32F_EXT);
        validators_.texture_internal_format_storage.AddValue(
            GL_LUMINANCE_ALPHA32F_EXT);
    }
    if (enable_texture_half_float) {
        validators_.texture_internal_format_storage.AddValue(GL_RGBA16F_EXT);
        validators_.texture_internal_format_storage.AddValue(GL_RGB16F_EXT);
        validators_.texture_internal_format_storage.AddValue(GL_ALPHA16F_EXT);
        validators_.texture_internal_format_storage.AddValue(
            GL_LUMINANCE16F_EXT);
        validators_.texture_internal_format_storage.AddValue(
            GL_LUMINANCE_ALPHA16F_EXT);
    }
  }

  bool have_ext_occlusion_query_boolean =
      extensions.Contains("GL_EXT_occlusion_query_boolean");
  bool have_arb_occlusion_query2 =
      extensions.Contains("GL_ARB_occlusion_query2");
  bool have_arb_occlusion_query =
      extensions.Contains("GL_ARB_occlusion_query");

  if (!workarounds_.disable_ext_occlusion_query &&
      (have_ext_occlusion_query_boolean ||
       have_arb_occlusion_query2 ||
       have_arb_occlusion_query)) {
    AddExtensionString("GL_EXT_occlusion_query_boolean");
    feature_flags_.occlusion_query_boolean = true;
    feature_flags_.use_arb_occlusion_query2_for_occlusion_query_boolean =
        !have_ext_occlusion_query_boolean && have_arb_occlusion_query2;
    feature_flags_.use_arb_occlusion_query_for_occlusion_query_boolean =
        !have_ext_occlusion_query_boolean && have_arb_occlusion_query &&
        !have_arb_occlusion_query2;
  }

  if (!workarounds_.disable_angle_instanced_arrays &&
      (extensions.Contains("GL_ANGLE_instanced_arrays") ||
       (extensions.Contains("GL_ARB_instanced_arrays") &&
        extensions.Contains("GL_ARB_draw_instanced")) ||
       is_es3)) {
    AddExtensionString("GL_ANGLE_instanced_arrays");
    feature_flags_.angle_instanced_arrays = true;
    validators_.vertex_attribute.AddValue(GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE);
  }

  if (!workarounds_.disable_ext_draw_buffers &&
      (extensions.Contains("GL_ARB_draw_buffers") ||
       extensions.Contains("GL_EXT_draw_buffers"))) {
    AddExtensionString("GL_EXT_draw_buffers");
    feature_flags_.ext_draw_buffers = true;

    GLint max_color_attachments = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_color_attachments);
    for (GLenum i = GL_COLOR_ATTACHMENT1_EXT;
         i < static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + max_color_attachments);
         ++i) {
      validators_.attachment.AddValue(i);
    }

    validators_.g_l_state.AddValue(GL_MAX_COLOR_ATTACHMENTS_EXT);
    validators_.g_l_state.AddValue(GL_MAX_DRAW_BUFFERS_ARB);
    GLint max_draw_buffers = 0;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &max_draw_buffers);
    for (GLenum i = GL_DRAW_BUFFER0_ARB;
         i < static_cast<GLenum>(GL_DRAW_BUFFER0_ARB + max_draw_buffers);
         ++i) {
      validators_.g_l_state.AddValue(i);
    }
  }

  if (extensions.Contains("GL_EXT_blend_minmax") ||
      gfx::HasDesktopGLFeatures()) {
    AddExtensionString("GL_EXT_blend_minmax");
    validators_.equation.AddValue(GL_MIN_EXT);
    validators_.equation.AddValue(GL_MAX_EXT);
  }

  if (extensions.Contains("GL_EXT_frag_depth") || gfx::HasDesktopGLFeatures()) {
    AddExtensionString("GL_EXT_frag_depth");
    feature_flags_.ext_frag_depth = true;
  }

  if (extensions.Contains("GL_EXT_shader_texture_lod") ||
      gfx::HasDesktopGLFeatures()) {
    AddExtensionString("GL_EXT_shader_texture_lod");
    feature_flags_.ext_shader_texture_lod = true;
  }

#if !defined(OS_MACOSX)
  if (workarounds_.disable_egl_khr_fence_sync) {
    gfx::g_driver_egl.ext.b_EGL_KHR_fence_sync = false;
  }
  if (workarounds_.disable_egl_khr_wait_sync) {
    gfx::g_driver_egl.ext.b_EGL_KHR_wait_sync = false;
  }
#endif
  if (workarounds_.disable_arb_sync)
    gfx::g_driver_gl.ext.b_GL_ARB_sync = false;
  bool ui_gl_fence_works = gfx::GLFence::IsSupported();
  UMA_HISTOGRAM_BOOLEAN("GPU.FenceSupport", ui_gl_fence_works);

  feature_flags_.map_buffer_range =
      is_es3 || extensions.Contains("GL_ARB_map_buffer_range");

  // Really it's part of core OpenGL 2.1 and up, but let's assume the
  // extension is still advertised.
  bool has_pixel_buffers =
      is_es3 || extensions.Contains("GL_ARB_pixel_buffer_object");

  // We will use either glMapBuffer() or glMapBufferRange() for async readbacks.
  if (has_pixel_buffers && ui_gl_fence_works &&
      !workarounds_.disable_async_readpixels) {
    feature_flags_.use_async_readpixels = true;
  }

  if (is_es3 || extensions.Contains("GL_ARB_sampler_objects")) {
    feature_flags_.enable_samplers = true;
    // TODO(dsinclair): Add AddExtensionString("GL_CHROMIUM_sampler_objects")
    // when available.
  }

  if ((is_es3 || extensions.Contains("GL_EXT_discard_framebuffer")) &&
      !workarounds_.disable_ext_discard_framebuffer) {
    // DiscardFramebufferEXT is automatically bound to InvalidateFramebuffer.
    AddExtensionString("GL_EXT_discard_framebuffer");
    feature_flags_.ext_discard_framebuffer = true;
  }

  if (ui_gl_fence_works) {
    AddExtensionString("GL_CHROMIUM_sync_query");
    feature_flags_.chromium_sync_query = true;
  }
}

void FeatureInfo::AddExtensionString(const std::string& str) {
  size_t pos = extensions_.find(str);
  while (pos != std::string::npos &&
         pos + str.length() < extensions_.length() &&
         extensions_.substr(pos + str.length(), 1) != " ") {
    // This extension name is a substring of another.
    pos = extensions_.find(str, pos + str.length());
  }
  if (pos == std::string::npos) {
    extensions_ += (extensions_.empty() ? "" : " ") + str;
  }
}

FeatureInfo::~FeatureInfo() {
}

}  // namespace gles2
}  // namespace gpu
