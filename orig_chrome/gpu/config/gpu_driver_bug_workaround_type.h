// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_CONFIG_GPU_DRIVER_BUG_WORKAROUND_TYPE_H_
#define GPU_CONFIG_GPU_DRIVER_BUG_WORKAROUND_TYPE_H_

#include <string>

#include "gpu/gpu_export.h"

#define GPU_DRIVER_BUG_WORKAROUNDS(GPU_OP)                   \
    GPU_OP(AVOID_EGL_IMAGE_TARGET_TEXTURE_REUSE,             \
        avoid_egl_image_target_texture_reuse)                \
    GPU_OP(CLEAR_ALPHA_IN_READPIXELS,                        \
        clear_alpha_in_readpixels)                           \
    GPU_OP(CLEAR_UNIFORMS_BEFORE_FIRST_PROGRAM_USE,          \
        clear_uniforms_before_first_program_use)             \
    GPU_OP(COUNT_ALL_IN_VARYINGS_PACKING,                    \
        count_all_in_varyings_packing)                       \
    GPU_OP(DISABLE_ANGLE_INSTANCED_ARRAYS,                   \
        disable_angle_instanced_arrays)                      \
    GPU_OP(DISABLE_ASYNC_READPIXELS,                         \
        disable_async_readpixels)                            \
    GPU_OP(DISABLE_BLEND_EQUATION_ADVANCED,                  \
        disable_blend_equation_advanced)                     \
    GPU_OP(DISABLE_CHROMIUM_FRAMEBUFFER_MULTISAMPLE,         \
        disable_chromium_framebuffer_multisample)            \
    GPU_OP(DISABLE_D3D11,                                    \
        disable_d3d11)                                       \
    GPU_OP(DISABLE_DEPTH_TEXTURE,                            \
        disable_depth_texture)                               \
    GPU_OP(DISABLE_DISCARD_FRAMEBUFFER,                      \
        disable_discard_framebuffer)                         \
    GPU_OP(DISABLE_EXT_DRAW_BUFFERS,                         \
        disable_ext_draw_buffers)                            \
    GPU_OP(DISABLE_EXT_SRGB,                                 \
        disable_ext_srgb)                                    \
    GPU_OP(DISABLE_GL_PATH_RENDERING,                        \
        disable_gl_path_rendering)                           \
    GPU_OP(DISABLE_GL_RGB_FORMAT,                            \
        disable_gl_rgb_format)                               \
    GPU_OP(DISABLE_MSAA_ON_NON_WEBGL_CONTEXTS,               \
        disable_msaa_on_non_webgl_contexts)                  \
    GPU_OP(DISABLE_MULTIMONITOR_MULTISAMPLING,               \
        disable_multimonitor_multisampling)                  \
    GPU_OP(DISABLE_MULTISAMPLED_RENDER_TO_TEXTURE,           \
        disable_multisampled_render_to_texture)              \
    GPU_OP(DISABLE_OVERLAY_CA_LAYERS,                        \
        disable_overlay_ca_layers)                           \
    GPU_OP(DISABLE_POST_SUB_BUFFERS_FOR_ONSCREEN_SURFACES,   \
        disable_post_sub_buffers_for_onscreen_surfaces)      \
    GPU_OP(DISABLE_PROGRAM_CACHE,                            \
        disable_program_cache)                               \
    GPU_OP(DISABLE_TEXTURE_STORAGE,                          \
        disable_texture_storage)                             \
    GPU_OP(DISABLE_TIMESTAMP_QUERIES,                        \
        disable_timestamp_queries)                           \
    GPU_OP(ETC1_POWER_OF_TWO_ONLY,                           \
        etc1_power_of_two_only)                              \
    GPU_OP(EXIT_ON_CONTEXT_LOST,                             \
        exit_on_context_lost)                                \
    GPU_OP(FORCE_CUBE_COMPLETE,                              \
        force_cube_complete)                                 \
    GPU_OP(FORCE_CUBE_MAP_POSITIVE_X_ALLOCATION,             \
        force_cube_map_positive_x_allocation)                \
    GPU_OP(FORCE_DISCRETE_GPU,                               \
        force_discrete_gpu)                                  \
    GPU_OP(FORCE_GL_FINISH_AFTER_COMPOSITING,                \
        force_gl_finish_after_compositing)                   \
    GPU_OP(FORCE_INTEGRATED_GPU,                             \
        force_integrated_gpu)                                \
    GPU_OP(GL_BEGIN_GL_END_ON_FBO_CHANGE_TO_BACKBUFFER,      \
        gl_begin_gl_end_on_fbo_change_to_backbuffer)         \
    GPU_OP(GL_CLEAR_BROKEN,                                  \
        gl_clear_broken)                                     \
    GPU_OP(IGNORE_EGL_SYNC_FAILURES,                         \
        ignore_egl_sync_failures)                            \
    GPU_OP(INIT_GL_POSITION_IN_VERTEX_SHADER,                \
        init_gl_position_in_vertex_shader)                   \
    GPU_OP(INIT_TEXTURE_MAX_ANISOTROPY,                      \
        init_texture_max_anisotropy)                         \
    GPU_OP(INIT_VARYINGS_WITHOUT_STATIC_USE,                 \
        init_varyings_without_static_use)                    \
    GPU_OP(INIT_VERTEX_ATTRIBUTES,                           \
        init_vertex_attributes)                              \
    GPU_OP(MAX_COPY_TEXTURE_CHROMIUM_SIZE_1048576,           \
        max_copy_texture_chromium_size_1048576)              \
    GPU_OP(MAX_COPY_TEXTURE_CHROMIUM_SIZE_262144,            \
        max_copy_texture_chromium_size_262144)               \
    GPU_OP(MAX_CUBE_MAP_TEXTURE_SIZE_LIMIT_1024,             \
        max_cube_map_texture_size_limit_1024)                \
    GPU_OP(MAX_CUBE_MAP_TEXTURE_SIZE_LIMIT_4096,             \
        max_cube_map_texture_size_limit_4096)                \
    GPU_OP(MAX_CUBE_MAP_TEXTURE_SIZE_LIMIT_512,              \
        max_cube_map_texture_size_limit_512)                 \
    GPU_OP(MAX_FRAGMENT_UNIFORM_VECTORS_32,                  \
        max_fragment_uniform_vectors_32)                     \
    GPU_OP(MAX_TEXTURE_SIZE_LIMIT_4096,                      \
        max_texture_size_limit_4096)                         \
    GPU_OP(MAX_VARYING_VECTORS_16,                           \
        max_varying_vectors_16)                              \
    GPU_OP(MAX_VERTEX_UNIFORM_VECTORS_256,                   \
        max_vertex_uniform_vectors_256)                      \
    GPU_OP(NEEDS_GLSL_BUILT_IN_FUNCTION_EMULATION,           \
        needs_glsl_built_in_function_emulation)              \
    GPU_OP(NEEDS_OFFSCREEN_BUFFER_WORKAROUND,                \
        needs_offscreen_buffer_workaround)                   \
    GPU_OP(REGENERATE_STRUCT_NAMES,                          \
        regenerate_struct_names)                             \
    GPU_OP(REMOVE_POW_WITH_CONSTANT_EXPONENT,                \
        remove_pow_with_constant_exponent)                   \
    GPU_OP(RESTORE_SCISSOR_ON_FBO_CHANGE,                    \
        restore_scissor_on_fbo_change)                       \
    GPU_OP(REVERSE_POINT_SPRITE_COORD_ORIGIN,                \
        reverse_point_sprite_coord_origin)                   \
    GPU_OP(SCALARIZE_VEC_AND_MAT_CONSTRUCTOR_ARGS,           \
        scalarize_vec_and_mat_constructor_args)              \
    GPU_OP(SET_TEXTURE_FILTER_BEFORE_GENERATING_MIPMAP,      \
        set_texture_filter_before_generating_mipmap)         \
    GPU_OP(SIMULATE_OUT_OF_MEMORY_ON_LARGE_TEXTURES,         \
        simulate_out_of_memory_on_large_textures)            \
    GPU_OP(SWIZZLE_RGBA_FOR_ASYNC_READPIXELS,                \
        swizzle_rgba_for_async_readpixels)                   \
    GPU_OP(TEXSUBIMAGE_FASTER_THAN_TEXIMAGE,                 \
        texsubimage_faster_than_teximage)                    \
    GPU_OP(UNBIND_ATTACHMENTS_ON_BOUND_RENDER_FBO_DELETE,    \
        unbind_attachments_on_bound_render_fbo_delete)       \
    GPU_OP(UNBIND_FBO_ON_CONTEXT_SWITCH,                     \
        unbind_fbo_on_context_switch)                        \
    GPU_OP(UNFOLD_SHORT_CIRCUIT_AS_TERNARY_OPERATION,        \
        unfold_short_circuit_as_ternary_operation)           \
    GPU_OP(UNROLL_FOR_LOOP_WITH_SAMPLER_ARRAY_INDEX,         \
        unroll_for_loop_with_sampler_array_index)            \
    GPU_OP(USE_CLIENT_SIDE_ARRAYS_FOR_STREAM_BUFFERS,        \
        use_client_side_arrays_for_stream_buffers)           \
    GPU_OP(USE_CURRENT_PROGRAM_AFTER_SUCCESSFUL_LINK,        \
        use_current_program_after_successful_link)           \
    GPU_OP(USE_NON_ZERO_SIZE_FOR_CLIENT_SIDE_STREAM_BUFFERS, \
        use_non_zero_size_for_client_side_stream_buffers)    \
    GPU_OP(USE_VIRTUALIZED_GL_CONTEXTS,                      \
        use_virtualized_gl_contexts)                         \
    GPU_OP(VALIDATE_MULTISAMPLE_BUFFER_ALLOCATION,           \
        validate_multisample_buffer_allocation)              \
    GPU_OP(WAKE_UP_GPU_BEFORE_DRAWING,                       \
        wake_up_gpu_before_drawing)                          \
    GPU_OP(UNBIND_EGL_CONTEXT_TO_FLUSH_DRIVER_CACHES,        \
        unbind_egl_context_to_flush_driver_caches)

namespace gpu {

// Provides all types of GPU driver bug workarounds.
enum GpuDriverBugWorkaroundType {
#define GPU_OP(type, name) type,
    GPU_DRIVER_BUG_WORKAROUNDS(GPU_OP)
#undef GPU_OP
        NUMBER_OF_GPU_DRIVER_BUG_WORKAROUND_TYPES
};

GPU_EXPORT std::string GpuDriverBugWorkaroundTypeToString(
    GpuDriverBugWorkaroundType type);

} // namespace gpu

#endif // GPU_CONFIG_GPU_DRIVER_BUG_WORKAROUND_TYPE_H_
