// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_BINDINGS_H_
#define UI_GL_GL_BINDINGS_H_

#include <string>

// Includes the platform independent and platform dependent GL headers.
// Only include this in cc files. It pulls in system headers, including
// the X11 headers on linux, which define all kinds of macros that are
// liable to cause conflicts.
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "base/logging.h"
#include "base/threading/thread_local.h"
#include "build/build_config.h"
#include "ui/gl/gl_export.h"

// The standard OpenGL native extension headers are also included.
#if defined(OS_WIN)
#include <GL/wglext.h>
#elif defined(OS_MACOSX)
#include <OpenGL/OpenGL.h>
#elif defined(USE_X11)
#include <GL/glx.h>
#include <GL/glxext.h>

// Undefine some macros defined by X headers. This is why this file should only
// be included in .cc files.
#undef Bool
#undef None
#undef Status
#endif

// GLES2 defines not part of Desktop GL
// Shader Precision-Specified Types
#define GL_LOW_FLOAT 0x8DF0
#define GL_MEDIUM_FLOAT 0x8DF1
#define GL_HIGH_FLOAT 0x8DF2
#define GL_LOW_INT 0x8DF3
#define GL_MEDIUM_INT 0x8DF4
#define GL_HIGH_INT 0x8DF5
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS 0x8DFD
#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB
#define GL_MAX_VARYING_VECTORS 0x8DFC
#define GL_SHADER_BINARY_FORMATS 0x8DF8
#define GL_NUM_SHADER_BINARY_FORMATS 0x8DF9
#define GL_SHADER_COMPILER 0x8DFA
#define GL_RGB565 0x8D62
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES 0x8B8B
#define GL_RGB8_OES 0x8051
#define GL_RGBA8_OES 0x8058
#define GL_HALF_FLOAT_OES 0x8D61

// GL_OES_EGL_image_external
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#define GL_SAMPLER_EXTERNAL_OES 0x8D66
#define GL_TEXTURE_BINDING_EXTERNAL_OES 0x8D67
#define GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES 0x8D68

// GL_ANGLE_translated_shader_source
#define GL_TRANSLATED_SHADER_SOURCE_LENGTH_ANGLE 0x93A0

#define GL_UNPACK_COLORSPACE_CONVERSION_CHROMIUM 0x9243
#define GL_BIND_GENERATES_RESOURCE_CHROMIUM 0x9244

// GL_ANGLE_pack_reverse_row_order
#define GL_PACK_REVERSE_ROW_ORDER_ANGLE 0x93A4

// GL_ANGLE_texture_usage
#define GL_TEXTURE_USAGE_ANGLE 0x93A2
#define GL_FRAMEBUFFER_ATTACHMENT_ANGLE 0x93A3

// GL_EXT_texture_storage
#define GL_TEXTURE_IMMUTABLE_FORMAT_EXT 0x912F
#define GL_ALPHA8_EXT 0x803C
#define GL_LUMINANCE8_EXT 0x8040
#define GL_LUMINANCE8_ALPHA8_EXT 0x8045
#define GL_RGBA32F_EXT 0x8814
#define GL_RGB32F_EXT 0x8815
#define GL_ALPHA32F_EXT 0x8816
#define GL_LUMINANCE32F_EXT 0x8818
#define GL_LUMINANCE_ALPHA32F_EXT 0x8819
#define GL_RGBA16F_EXT 0x881A
#define GL_RGB16F_EXT 0x881B
#define GL_RG16F_EXT 0x822F
#define GL_R16F_EXT 0x822D
#define GL_ALPHA16F_EXT 0x881C
#define GL_LUMINANCE16F_EXT 0x881E
#define GL_LUMINANCE_ALPHA16F_EXT 0x881F
#define GL_R32F_EXT 0x822E
#define GL_RG32F_EXT 0x8230
#define GL_BGRA8_EXT 0x93A1

// GL_ANGLE_instanced_arrays
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE 0x88FE

// GL_EXT_occlusion_query_boolean
#define GL_ANY_SAMPLES_PASSED_EXT 0x8C2F
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT 0x8D6A
#define GL_CURRENT_QUERY_EXT 0x8865
#define GL_QUERY_RESULT_EXT 0x8866
#define GL_QUERY_RESULT_AVAILABLE_EXT 0x8867

// GL_CHROMIUM_command_buffer_query
#define GL_COMMANDS_ISSUED_CHROMIUM 0x6004

/* GL_CHROMIUM_get_error_query */
#define GL_GET_ERROR_QUERY_CHROMIUM 0x6003

/* GL_CHROMIUM_command_buffer_latency_query */
#define GL_LATENCY_QUERY_CHROMIUM 0x6007

/* GL_CHROMIUM_async_pixel_transfers */
#define GL_ASYNC_PIXEL_PACK_COMPLETED_CHROMIUM 0x6006

// GL_CHROMIUM_sync_query
#define GL_COMMANDS_COMPLETED_CHROMIUM 0x84F7

// GL_CHROMIUM_gpu_memory_buffer_image
#define GL_READ_WRITE_CHROMIUM 0x78F2

// GL_CHROMIUM_yuv_420_image
#define GL_RGB_YUV_420_CHROMIUM 0x78FA

// GL_CHROMIUM_ycbcr_422_image
#define GL_RGB_YCBCR_422_CHROMIUM 0x78FB

// GL_CHROMIUM_schedule_overlay_plane
#define GL_OVERLAY_TRANSFORM_NONE_CHROMIUM 0x9245
#define GL_OVERLAY_TRANSFORM_FLIP_HORIZONTAL_CHROMIUM 0x9246
#define GL_OVERLAY_TRANSFORM_FLIP_VERTICAL_CHROMIUM 0x9247
#define GL_OVERLAY_TRANSFORM_ROTATE_90_CHROMIUM 0x9248
#define GL_OVERLAY_TRANSFORM_ROTATE_180_CHROMIUM 0x9249
#define GL_OVERLAY_TRANSFORM_ROTATE_270_CHROMIUM 0x924A

// GL_CHROMIUM_subscribe_uniforms
#define GL_SUBSCRIBED_VALUES_BUFFER_CHROMIUM 0x924B
#define GL_MOUSE_POSITION_CHROMIUM 0x924C

// GL_OES_texure_3D
#define GL_SAMPLER_3D_OES 0x8B5F

// GL_OES_depth24
#define GL_DEPTH_COMPONENT24_OES 0x81A6

// GL_OES_depth32
#define GL_DEPTH_COMPONENT32_OES 0x81A7

// GL_OES_packed_depth_stencil
#ifndef GL_DEPTH24_STENCIL8_OES
#define GL_DEPTH24_STENCIL8_OES 0x88F0
#endif

#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8 0x88F0
#endif

// GL_OES_compressed_ETC1_RGB8_texture
#define GL_ETC1_RGB8_OES 0x8D64

// GL_AMD_compressed_ATC_texture
#define GL_ATC_RGB_AMD 0x8C92
#define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD 0x8C93
#define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD 0x87EE

// GL_IMG_texture_compression_pvrtc
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03

// GL_OES_vertex_array_object
#define GL_VERTEX_ARRAY_BINDING_OES 0x85B5

// GL_CHROMIUM_pixel_transfer_buffer_object
#define GL_PIXEL_UNPACK_TRANSFER_BUFFER_CHROMIUM 0x78EC
#define GL_PIXEL_PACK_TRANSFER_BUFFER_CHROMIUM 0x78ED
#define GL_PIXEL_PACK_TRANSFER_BUFFER_BINDING_CHROMIUM 0x78EE
#define GL_PIXEL_UNPACK_TRANSFER_BUFFER_BINDING_CHROMIUM 0x78EF

/* GL_EXT_discard_framebuffer */
#ifndef GL_EXT_discard_framebuffer
#define GL_COLOR_EXT 0x1800
#define GL_DEPTH_EXT 0x1801
#define GL_STENCIL_EXT 0x1802
#endif

// GL_EXT_sRGB
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT 0x8210

// GL_ARB_get_program_binary
#define PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
// GL_OES_get_program_binary
#define GL_PROGRAM_BINARY_LENGTH_OES 0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS_OES 0x87FE
#define GL_PROGRAM_BINARY_FORMATS_OES 0x87FF

#ifndef GL_EXT_multisampled_render_to_texture
#define GL_RENDERBUFFER_SAMPLES_EXT 0x8CAB
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT 0x8D56
#define GL_MAX_SAMPLES_EXT 0x8D57
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT 0x8D6C
#endif

#ifndef GL_IMG_multisampled_render_to_texture
#define GL_RENDERBUFFER_SAMPLES_IMG 0x9133
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_IMG 0x9134
#define GL_MAX_SAMPLES_IMG 0x9135
#define GL_TEXTURE_SAMPLES_IMG 0x9136
#endif

#ifndef GL_CHROMIUM_path_rendering
#define GL_CHROMIUM_path_rendering 1
// These match the corresponding values in NV_path_rendering
// extension, eg tokens with CHROMIUM replaced with NV.
#define GL_PATH_MODELVIEW_MATRIX_CHROMIUM 0x0BA6
#define GL_PATH_PROJECTION_MATRIX_CHROMIUM 0x0BA7
#define GL_PATH_MODELVIEW_CHROMIUM 0x1700
#define GL_PATH_PROJECTION_CHROMIUM 0x1701
#define GL_FLAT_CHROMIUM 0x1D00
#define GL_CLOSE_PATH_CHROMIUM 0x00
#define GL_MOVE_TO_CHROMIUM 0x02
#define GL_LINE_TO_CHROMIUM 0x04
#define GL_QUADRATIC_CURVE_TO_CHROMIUM 0x0A
#define GL_CUBIC_CURVE_TO_CHROMIUM 0x0C
#define GL_CONIC_CURVE_TO_CHROMIUM 0x1A
#define GL_EYE_LINEAR_CHROMIUM 0x2400
#define GL_OBJECT_LINEAR_CHROMIUM 0x2401
#define GL_CONSTANT_CHROMIUM 0x8576
#define GL_PATH_STROKE_WIDTH_CHROMIUM 0x9075
#define GL_PATH_END_CAPS_CHROMIUM 0x9076
#define GL_PATH_JOIN_STYLE_CHROMIUM 0x9079
#define GL_PATH_MITER_LIMIT_CHROMIUM 0x907a
#define GL_PATH_STROKE_BOUND_CHROMIUM 0x9086
#define GL_COUNT_UP_CHROMIUM 0x9088
#define GL_COUNT_DOWN_CHROMIUM 0x9089
#define GL_CONVEX_HULL_CHROMIUM 0x908B
#define GL_BOUNDING_BOX_CHROMIUM 0x908D
#define GL_TRANSLATE_X_CHROMIUM 0x908E
#define GL_TRANSLATE_Y_CHROMIUM 0x908F
#define GL_TRANSLATE_2D_CHROMIUM 0x9090
#define GL_TRANSLATE_3D_CHROMIUM 0x9091
#define GL_AFFINE_2D_CHROMIUM 0x9092
#define GL_AFFINE_3D_CHROMIUM 0x9094
#define GL_TRANSPOSE_AFFINE_2D_CHROMIUM 0x9096
#define GL_TRANSPOSE_AFFINE_3D_CHROMIUM 0x9098
#define GL_BOUNDING_BOX_OF_BOUNDING_BOXES_CHROMIUM 0x909C
#define GL_SQUARE_CHROMIUM 0x90a3
#define GL_ROUND_CHROMIUM 0x90a4
#define GL_BEVEL_CHROMIUM 0x90a6
#define GL_MITER_REVERT_CHROMIUM 0x90a7
#define GL_PATH_STENCIL_FUNC_CHROMIUM 0x90B7
#define GL_PATH_STENCIL_REF_CHROMIUM 0x90B8
#define GL_PATH_STENCIL_VALUE_MASK_CHROMIUM 0x90B9
#endif

#ifndef GL_KHR_blend_equation_advanced
#define GL_KHR_blend_equation_advanced 1
#define GL_COLORBURN_KHR 0x929A
#define GL_COLORDODGE_KHR 0x9299
#define GL_DARKEN_KHR 0x9297
#define GL_DIFFERENCE_KHR 0x929E
#define GL_EXCLUSION_KHR 0x92A0
#define GL_HARDLIGHT_KHR 0x929B
#define GL_HSL_COLOR_KHR 0x92AF
#define GL_HSL_HUE_KHR 0x92AD
#define GL_HSL_LUMINOSITY_KHR 0x92B0
#define GL_HSL_SATURATION_KHR 0x92AE
#define GL_LIGHTEN_KHR 0x9298
#define GL_MULTIPLY_KHR 0x9294
#define GL_OVERLAY_KHR 0x9296
#define GL_SCREEN_KHR 0x9295
#define GL_SOFTLIGHT_KHR 0x929C
#endif /* GL_KHR_blend_equation_advanced */

#ifndef GL_KHR_blend_equation_advanced_coherent
#define GL_KHR_blend_equation_advanced_coherent 1
#define GL_BLEND_ADVANCED_COHERENT_KHR 0x9285
#endif /* GL_KHR_blend_equation_advanced_coherent */

#ifndef GL_EXT_disjoint_timer_query
#define GL_EXT_disjoint_timer_query 1
#define GL_QUERY_COUNTER_BITS_EXT 0x8864
#define GL_TIME_ELAPSED_EXT 0x88BF
#define GL_TIMESTAMP_EXT 0x8E28
#define GL_GPU_DISJOINT_EXT 0x8FBB
#endif

#ifndef GL_KHR_robustness
#define GL_KHR_robustness 1
#define GL_CONTEXT_ROBUST_ACCESS_KHR 0x90F3
#define GL_LOSE_CONTEXT_ON_RESET_KHR 0x8252
#define GL_GUILTY_CONTEXT_RESET_KHR 0x8253
#define GL_INNOCENT_CONTEXT_RESET_KHR 0x8254
#define GL_UNKNOWN_CONTEXT_RESET_KHR 0x8255
#define GL_RESET_NOTIFICATION_STRATEGY_KHR 0x8256
#define GL_NO_RESET_NOTIFICATION_KHR 0x8261
#define GL_CONTEXT_LOST_KHR 0x0507
#endif /* GL_KHR_robustness */

#ifndef GL_EXT_texture_rg
#define GL_EXT_texture_rg 1
#define GL_RED_EXT 0x1903
#define GL_RG_EXT 0x8227
#define GL_R8_EXT 0x8229
#define GL_RG8_EXT 0x822B
#endif /* GL_EXT_texture_rg */

// This is from NV_path_rendering, but the Mesa GL header is not up-to-date with
// the most recent
// version of the extension. This definition could be removed once glext.h
// r27498 or later is
// imported.
#ifndef GL_FRAGMENT_INPUT_NV
#define GL_FRAGMENT_INPUT_NV 0x936D
#endif

#define GL_GLEXT_PROTOTYPES 1

#if defined(OS_WIN)
#define GL_BINDING_CALL WINAPI
#else
#define GL_BINDING_CALL
#endif

#define GL_SERVICE_LOG(args) DLOG(INFO) << args;
#if defined(NDEBUG)
#define GL_SERVICE_LOG_CODE_BLOCK(code)
#else
#define GL_SERVICE_LOG_CODE_BLOCK(code) code
#endif

// Forward declare OSMesa types.
typedef struct osmesa_context* OSMesaContext;
typedef void (*OSMESAproc)();

// Forward declare EGL types.
typedef uint64 EGLuint64CHROMIUM;

#include "gl_bindings_autogen_gl.h"
#include "gl_bindings_autogen_osmesa.h"

#if defined(OS_WIN)
#include "gl_bindings_autogen_egl.h"
#include "gl_bindings_autogen_wgl.h"
#elif defined(USE_X11)
#include "gl_bindings_autogen_egl.h"
#include "gl_bindings_autogen_glx.h"
#elif defined(USE_OZONE)
#include "gl_bindings_autogen_egl.h"
#elif defined(OS_ANDROID)
#include "gl_bindings_autogen_egl.h"
#endif

namespace gfx {

struct GL_EXPORT DriverGL {
    void InitializeStaticBindings();
    void InitializeCustomDynamicBindings(GLContext* context);
    void InitializeDebugBindings();
    void InitializeNullDrawBindings();
    // TODO(danakj): Remove this when all test suites are using null-draw.
    bool HasInitializedNullDrawBindings();
    bool SetNullDrawBindingsEnabled(bool enabled);
    void ClearBindings();

    ProcsGL fn;
    ProcsGL orig_fn;
    ProcsGL debug_fn;
    ExtensionsGL ext;
    bool null_draw_bindings_enabled;

private:
    void InitializeDynamicBindings(GLContext* context);
};

struct GL_EXPORT DriverOSMESA {
    void InitializeStaticBindings();
    void InitializeExtensionBindings();
    void InitializeDebugBindings();
    void ClearBindings();

    ProcsOSMESA fn;
    ProcsOSMESA debug_fn;
    ExtensionsOSMESA ext;

private:
    static std::string GetPlatformExtensions();
};

#if defined(OS_WIN)
struct GL_EXPORT DriverWGL {
    void InitializeStaticBindings();
    void InitializeExtensionBindings();
    void InitializeDebugBindings();
    void ClearBindings();

    ProcsWGL fn;
    ProcsWGL debug_fn;
    ExtensionsWGL ext;

private:
    static std::string GetPlatformExtensions();
};
#endif

#if defined(OS_WIN) || defined(USE_X11) || defined(OS_ANDROID) || defined(USE_OZONE)
struct GL_EXPORT DriverEGL {
    void InitializeStaticBindings();
    void InitializeExtensionBindings();
    void InitializeDebugBindings();
    void ClearBindings();

    ProcsEGL fn;
    ProcsEGL debug_fn;
    ExtensionsEGL ext;

    static std::string GetPlatformExtensions();
    static std::string GetClientExtensions();
};
#endif

#if defined(USE_X11)
struct GL_EXPORT DriverGLX {
    void InitializeStaticBindings();
    void InitializeExtensionBindings();
    void InitializeDebugBindings();
    void ClearBindings();

    ProcsGLX fn;
    ProcsGLX debug_fn;
    ExtensionsGLX ext;

private:
    static std::string GetPlatformExtensions();
};
#endif

// This #define is here to support autogenerated code.
#define g_current_gl_context g_current_gl_context_tls->Get()
GL_EXPORT extern base::ThreadLocalPointer<GLApi>* g_current_gl_context_tls;

GL_EXPORT extern OSMESAApi* g_current_osmesa_context;
GL_EXPORT extern DriverGL g_driver_gl;
GL_EXPORT extern DriverOSMESA g_driver_osmesa;

#if defined(OS_WIN)

GL_EXPORT extern EGLApi* g_current_egl_context;
GL_EXPORT extern WGLApi* g_current_wgl_context;
GL_EXPORT extern DriverEGL g_driver_egl;
GL_EXPORT extern DriverWGL g_driver_wgl;

#elif defined(USE_X11)

GL_EXPORT extern EGLApi* g_current_egl_context;
GL_EXPORT extern GLXApi* g_current_glx_context;
GL_EXPORT extern DriverEGL g_driver_egl;
GL_EXPORT extern DriverGLX g_driver_glx;

#elif defined(USE_OZONE)

GL_EXPORT extern EGLApi* g_current_egl_context;
GL_EXPORT extern DriverEGL g_driver_egl;

#elif defined(OS_ANDROID)

GL_EXPORT extern EGLApi* g_current_egl_context;
GL_EXPORT extern DriverEGL g_driver_egl;

#endif

} // namespace gfx

#endif // UI_GL_GL_BINDINGS_H_
