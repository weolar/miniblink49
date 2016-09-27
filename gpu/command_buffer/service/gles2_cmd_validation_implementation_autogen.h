// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

#ifndef GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_VALIDATION_IMPLEMENTATION_AUTOGEN_H_
#define GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_VALIDATION_IMPLEMENTATION_AUTOGEN_H_

static const GLenum valid_attachment_table[] = {
    GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT,
};

static const GLenum valid_backbuffer_attachment_table[] = {
    GL_COLOR_EXT, GL_DEPTH_EXT, GL_STENCIL_EXT,
};

static const GLenum valid_blit_filter_table[] = {
    GL_NEAREST, GL_LINEAR,
};

static const GLenum valid_buffer_parameter_table[] = {
    GL_BUFFER_SIZE, GL_BUFFER_USAGE,
};

static const GLenum valid_buffer_target_table[] = {
    GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER,
};

static const GLenum valid_buffer_usage_table[] = {
    GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW,
};

static const GLenum valid_capability_table[] = {
    GL_BLEND,           GL_CULL_FACE,           GL_DEPTH_TEST,
    GL_DITHER,          GL_POLYGON_OFFSET_FILL, GL_SAMPLE_ALPHA_TO_COVERAGE,
    GL_SAMPLE_COVERAGE, GL_SCISSOR_TEST,        GL_STENCIL_TEST,
};

static const GLenum valid_cmp_function_table[] = {
    GL_NEVER,   GL_LESS,     GL_EQUAL,  GL_LEQUAL,
    GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS,
};

static const GLenum valid_draw_mode_table[] = {
    GL_POINTS,         GL_LINE_STRIP,   GL_LINE_LOOP, GL_LINES,
    GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES,
};

static const GLenum valid_dst_blend_factor_table[] = {
    GL_ZERO,           GL_ONE,
    GL_SRC_COLOR,      GL_ONE_MINUS_SRC_COLOR,
    GL_DST_COLOR,      GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,      GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,      GL_ONE_MINUS_DST_ALPHA,
    GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA,
};

static const GLenum valid_equation_table[] = {
    GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT,
};

static const GLenum valid_face_mode_table[] = {
    GL_CW, GL_CCW,
};

static const GLenum valid_face_type_table[] = {
    GL_FRONT, GL_BACK, GL_FRONT_AND_BACK,
};

static const GLenum valid_frame_buffer_parameter_table[] = {
    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
    GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
    GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE,
};

static const GLenum valid_frame_buffer_target_table[] = {
    GL_FRAMEBUFFER,
};

static const GLenum valid_g_l_state_table[] = {
    GL_ACTIVE_TEXTURE,
    GL_ALIASED_LINE_WIDTH_RANGE,
    GL_ALIASED_POINT_SIZE_RANGE,
    GL_ALPHA_BITS,
    GL_ARRAY_BUFFER_BINDING,
    GL_BLUE_BITS,
    GL_COMPRESSED_TEXTURE_FORMATS,
    GL_CURRENT_PROGRAM,
    GL_DEPTH_BITS,
    GL_DEPTH_RANGE,
    GL_ELEMENT_ARRAY_BUFFER_BINDING,
    GL_FRAMEBUFFER_BINDING,
    GL_GENERATE_MIPMAP_HINT,
    GL_GREEN_BITS,
    GL_IMPLEMENTATION_COLOR_READ_FORMAT,
    GL_IMPLEMENTATION_COLOR_READ_TYPE,
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE,
    GL_MAX_FRAGMENT_UNIFORM_VECTORS,
    GL_MAX_RENDERBUFFER_SIZE,
    GL_MAX_TEXTURE_IMAGE_UNITS,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_VARYING_VECTORS,
    GL_MAX_VERTEX_ATTRIBS,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    GL_MAX_VERTEX_UNIFORM_VECTORS,
    GL_MAX_VIEWPORT_DIMS,
    GL_NUM_COMPRESSED_TEXTURE_FORMATS,
    GL_NUM_SHADER_BINARY_FORMATS,
    GL_PACK_ALIGNMENT,
    GL_RED_BITS,
    GL_RENDERBUFFER_BINDING,
    GL_SAMPLE_BUFFERS,
    GL_SAMPLE_COVERAGE_INVERT,
    GL_SAMPLE_COVERAGE_VALUE,
    GL_SAMPLES,
    GL_SCISSOR_BOX,
    GL_SHADER_BINARY_FORMATS,
    GL_SHADER_COMPILER,
    GL_SUBPIXEL_BITS,
    GL_STENCIL_BITS,
    GL_TEXTURE_BINDING_2D,
    GL_TEXTURE_BINDING_CUBE_MAP,
    GL_UNPACK_ALIGNMENT,
    GL_UNPACK_FLIP_Y_CHROMIUM,
    GL_UNPACK_PREMULTIPLY_ALPHA_CHROMIUM,
    GL_UNPACK_UNPREMULTIPLY_ALPHA_CHROMIUM,
    GL_BIND_GENERATES_RESOURCE_CHROMIUM,
    GL_VERTEX_ARRAY_BINDING_OES,
    GL_VIEWPORT,
    GL_BLEND_COLOR,
    GL_BLEND_EQUATION_RGB,
    GL_BLEND_EQUATION_ALPHA,
    GL_BLEND_SRC_RGB,
    GL_BLEND_DST_RGB,
    GL_BLEND_SRC_ALPHA,
    GL_BLEND_DST_ALPHA,
    GL_COLOR_CLEAR_VALUE,
    GL_DEPTH_CLEAR_VALUE,
    GL_STENCIL_CLEAR_VALUE,
    GL_COLOR_WRITEMASK,
    GL_CULL_FACE_MODE,
    GL_DEPTH_FUNC,
    GL_DEPTH_WRITEMASK,
    GL_FRONT_FACE,
    GL_LINE_WIDTH,
    GL_POLYGON_OFFSET_FACTOR,
    GL_POLYGON_OFFSET_UNITS,
    GL_STENCIL_FUNC,
    GL_STENCIL_REF,
    GL_STENCIL_VALUE_MASK,
    GL_STENCIL_BACK_FUNC,
    GL_STENCIL_BACK_REF,
    GL_STENCIL_BACK_VALUE_MASK,
    GL_STENCIL_WRITEMASK,
    GL_STENCIL_BACK_WRITEMASK,
    GL_STENCIL_FAIL,
    GL_STENCIL_PASS_DEPTH_FAIL,
    GL_STENCIL_PASS_DEPTH_PASS,
    GL_STENCIL_BACK_FAIL,
    GL_STENCIL_BACK_PASS_DEPTH_FAIL,
    GL_STENCIL_BACK_PASS_DEPTH_PASS,
    GL_BLEND,
    GL_CULL_FACE,
    GL_DEPTH_TEST,
    GL_DITHER,
    GL_POLYGON_OFFSET_FILL,
    GL_SAMPLE_ALPHA_TO_COVERAGE,
    GL_SAMPLE_COVERAGE,
    GL_SCISSOR_TEST,
    GL_STENCIL_TEST,
};

static const GLenum valid_get_max_index_type_table[] = {
    GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT,
};

static const GLenum valid_get_tex_param_target_table[] = {
    GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP,
};

static const GLenum valid_hint_mode_table[] = {
    GL_FASTEST, GL_NICEST, GL_DONT_CARE,
};

static const GLenum valid_hint_target_table[] = {
    GL_GENERATE_MIPMAP_HINT,
};

static const GLenum valid_index_type_table[] = {
    GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT,
};

static const GLenum valid_pixel_store_table[] = {
    GL_PACK_ALIGNMENT,
    GL_UNPACK_ALIGNMENT,
    GL_UNPACK_FLIP_Y_CHROMIUM,
    GL_UNPACK_PREMULTIPLY_ALPHA_CHROMIUM,
    GL_UNPACK_UNPREMULTIPLY_ALPHA_CHROMIUM,
};

static const GLint valid_pixel_store_alignment_table[] = {
    1, 2, 4, 8,
};

static const GLenum valid_pixel_type_table[] = {
    GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4,
    GL_UNSIGNED_SHORT_5_5_5_1,
};

static const GLenum valid_program_parameter_table[] = {
    GL_DELETE_STATUS,               GL_LINK_STATUS,
    GL_VALIDATE_STATUS,             GL_INFO_LOG_LENGTH,
    GL_ATTACHED_SHADERS,            GL_ACTIVE_ATTRIBUTES,
    GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, GL_ACTIVE_UNIFORMS,
    GL_ACTIVE_UNIFORM_MAX_LENGTH,
};

static const GLenum valid_query_object_parameter_table[] = {
    GL_QUERY_RESULT_EXT, GL_QUERY_RESULT_AVAILABLE_EXT,
};

static const GLenum valid_query_parameter_table[] = {
    GL_CURRENT_QUERY_EXT,
};

static const GLenum valid_query_target_table[] = {
    GL_ANY_SAMPLES_PASSED_EXT,
    GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT,
    GL_COMMANDS_ISSUED_CHROMIUM,
    GL_LATENCY_QUERY_CHROMIUM,
    GL_ASYNC_PIXEL_UNPACK_COMPLETED_CHROMIUM,
    GL_ASYNC_PIXEL_PACK_COMPLETED_CHROMIUM,
    GL_COMMANDS_COMPLETED_CHROMIUM,
};

static const GLenum valid_read_pixel_format_table[] = {
    GL_ALPHA, GL_RGB, GL_RGBA,
};

static const GLenum valid_read_pixel_type_table[] = {
    GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4,
    GL_UNSIGNED_SHORT_5_5_5_1,
};

static const GLenum valid_render_buffer_format_table[] = {
    GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT16, GL_STENCIL_INDEX8,
};

static const GLenum valid_render_buffer_parameter_table[] = {
    GL_RENDERBUFFER_RED_SIZE,        GL_RENDERBUFFER_GREEN_SIZE,
    GL_RENDERBUFFER_BLUE_SIZE,       GL_RENDERBUFFER_ALPHA_SIZE,
    GL_RENDERBUFFER_DEPTH_SIZE,      GL_RENDERBUFFER_STENCIL_SIZE,
    GL_RENDERBUFFER_WIDTH,           GL_RENDERBUFFER_HEIGHT,
    GL_RENDERBUFFER_INTERNAL_FORMAT,
};

static const GLenum valid_render_buffer_target_table[] = {
    GL_RENDERBUFFER,
};

static const GLenum valid_reset_status_table[] = {
    GL_GUILTY_CONTEXT_RESET_ARB, GL_INNOCENT_CONTEXT_RESET_ARB,
    GL_UNKNOWN_CONTEXT_RESET_ARB,
};

static const GLenum valid_shader_parameter_table[] = {
    GL_SHADER_TYPE,          GL_DELETE_STATUS,
    GL_COMPILE_STATUS,       GL_INFO_LOG_LENGTH,
    GL_SHADER_SOURCE_LENGTH, GL_TRANSLATED_SHADER_SOURCE_LENGTH_ANGLE,
};

static const GLenum valid_shader_precision_table[] = {
    GL_LOW_FLOAT, GL_MEDIUM_FLOAT, GL_HIGH_FLOAT,
    GL_LOW_INT,   GL_MEDIUM_INT,   GL_HIGH_INT,
};

static const GLenum valid_shader_type_table[] = {
    GL_VERTEX_SHADER, GL_FRAGMENT_SHADER,
};

static const GLenum valid_src_blend_factor_table[] = {
    GL_ZERO,               GL_ONE,
    GL_SRC_COLOR,          GL_ONE_MINUS_SRC_COLOR,
    GL_DST_COLOR,          GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,          GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,          GL_ONE_MINUS_DST_ALPHA,
    GL_CONSTANT_COLOR,     GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,     GL_ONE_MINUS_CONSTANT_ALPHA,
    GL_SRC_ALPHA_SATURATE,
};

static const GLenum valid_stencil_op_table[] = {
    GL_KEEP,      GL_ZERO, GL_REPLACE,   GL_INCR,
    GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, GL_INVERT,
};

static const GLenum valid_string_type_table[] = {
    GL_VENDOR,                   GL_RENDERER,   GL_VERSION,
    GL_SHADING_LANGUAGE_VERSION, GL_EXTENSIONS,
};

static const GLenum valid_texture_bind_target_table[] = {
    GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP,
};

static const GLenum valid_texture_format_table[] = {
    GL_ALPHA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA,
};

static const GLenum valid_texture_internal_format_table[] = {
    GL_ALPHA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA,
};

static const GLenum valid_texture_internal_format_storage_table[] = {
    GL_RGB565,         GL_RGBA4,                 GL_RGB5_A1,  GL_ALPHA8_EXT,
    GL_LUMINANCE8_EXT, GL_LUMINANCE8_ALPHA8_EXT, GL_RGB8_OES, GL_RGBA8_OES,
};

static const GLenum valid_texture_mag_filter_mode_table[] = {
    GL_NEAREST, GL_LINEAR,
};

static const GLenum valid_texture_min_filter_mode_table[] = {
    GL_NEAREST,                GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,  GL_LINEAR_MIPMAP_LINEAR,
};

static const GLenum valid_texture_parameter_table[] = {
    GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_POOL_CHROMIUM,
    GL_TEXTURE_WRAP_S,     GL_TEXTURE_WRAP_T,
};

static const GLenum valid_texture_pool_table[] = {
    GL_TEXTURE_POOL_MANAGED_CHROMIUM, GL_TEXTURE_POOL_UNMANAGED_CHROMIUM,
};

static const GLenum valid_texture_target_table[] = {
    GL_TEXTURE_2D,                  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

static const GLenum valid_texture_usage_table[] = {
    GL_NONE, GL_FRAMEBUFFER_ATTACHMENT_ANGLE,
};

static const GLenum valid_texture_wrap_mode_table[] = {
    GL_CLAMP_TO_EDGE, GL_MIRRORED_REPEAT, GL_REPEAT,
};

static const GLint valid_vertex_attrib_size_table[] = {
    1, 2, 3, 4,
};

static const GLenum valid_vertex_attrib_type_table[] = {
    GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_FLOAT,
};

static const GLenum valid_vertex_attribute_table[] = {
    GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,
    GL_VERTEX_ATTRIB_ARRAY_ENABLED,    GL_VERTEX_ATTRIB_ARRAY_SIZE,
    GL_VERTEX_ATTRIB_ARRAY_STRIDE,     GL_VERTEX_ATTRIB_ARRAY_TYPE,
    GL_CURRENT_VERTEX_ATTRIB,
};

static const GLenum valid_vertex_pointer_table[] = {
    GL_VERTEX_ATTRIB_ARRAY_POINTER,
};

Validators::Validators()
    : attachment(valid_attachment_table, arraysize(valid_attachment_table)),
      backbuffer_attachment(valid_backbuffer_attachment_table,
                            arraysize(valid_backbuffer_attachment_table)),
      blit_filter(valid_blit_filter_table, arraysize(valid_blit_filter_table)),
      buffer_parameter(valid_buffer_parameter_table,
                       arraysize(valid_buffer_parameter_table)),
      buffer_target(valid_buffer_target_table,
                    arraysize(valid_buffer_target_table)),
      buffer_usage(valid_buffer_usage_table,
                   arraysize(valid_buffer_usage_table)),
      capability(valid_capability_table, arraysize(valid_capability_table)),
      cmp_function(valid_cmp_function_table,
                   arraysize(valid_cmp_function_table)),
      compressed_texture_format(),
      draw_mode(valid_draw_mode_table, arraysize(valid_draw_mode_table)),
      dst_blend_factor(valid_dst_blend_factor_table,
                       arraysize(valid_dst_blend_factor_table)),
      equation(valid_equation_table, arraysize(valid_equation_table)),
      face_mode(valid_face_mode_table, arraysize(valid_face_mode_table)),
      face_type(valid_face_type_table, arraysize(valid_face_type_table)),
      frame_buffer_parameter(valid_frame_buffer_parameter_table,
                             arraysize(valid_frame_buffer_parameter_table)),
      frame_buffer_target(valid_frame_buffer_target_table,
                          arraysize(valid_frame_buffer_target_table)),
      g_l_state(valid_g_l_state_table, arraysize(valid_g_l_state_table)),
      get_max_index_type(valid_get_max_index_type_table,
                         arraysize(valid_get_max_index_type_table)),
      get_tex_param_target(valid_get_tex_param_target_table,
                           arraysize(valid_get_tex_param_target_table)),
      hint_mode(valid_hint_mode_table, arraysize(valid_hint_mode_table)),
      hint_target(valid_hint_target_table, arraysize(valid_hint_target_table)),
      index_type(valid_index_type_table, arraysize(valid_index_type_table)),
      pixel_store(valid_pixel_store_table, arraysize(valid_pixel_store_table)),
      pixel_store_alignment(valid_pixel_store_alignment_table,
                            arraysize(valid_pixel_store_alignment_table)),
      pixel_type(valid_pixel_type_table, arraysize(valid_pixel_type_table)),
      program_parameter(valid_program_parameter_table,
                        arraysize(valid_program_parameter_table)),
      query_object_parameter(valid_query_object_parameter_table,
                             arraysize(valid_query_object_parameter_table)),
      query_parameter(valid_query_parameter_table,
                      arraysize(valid_query_parameter_table)),
      query_target(valid_query_target_table,
                   arraysize(valid_query_target_table)),
      read_pixel_format(valid_read_pixel_format_table,
                        arraysize(valid_read_pixel_format_table)),
      read_pixel_type(valid_read_pixel_type_table,
                      arraysize(valid_read_pixel_type_table)),
      render_buffer_format(valid_render_buffer_format_table,
                           arraysize(valid_render_buffer_format_table)),
      render_buffer_parameter(valid_render_buffer_parameter_table,
                              arraysize(valid_render_buffer_parameter_table)),
      render_buffer_target(valid_render_buffer_target_table,
                           arraysize(valid_render_buffer_target_table)),
      reset_status(valid_reset_status_table,
                   arraysize(valid_reset_status_table)),
      shader_binary_format(),
      shader_parameter(valid_shader_parameter_table,
                       arraysize(valid_shader_parameter_table)),
      shader_precision(valid_shader_precision_table,
                       arraysize(valid_shader_precision_table)),
      shader_type(valid_shader_type_table, arraysize(valid_shader_type_table)),
      src_blend_factor(valid_src_blend_factor_table,
                       arraysize(valid_src_blend_factor_table)),
      stencil_op(valid_stencil_op_table, arraysize(valid_stencil_op_table)),
      string_type(valid_string_type_table, arraysize(valid_string_type_table)),
      texture_bind_target(valid_texture_bind_target_table,
                          arraysize(valid_texture_bind_target_table)),
      texture_format(valid_texture_format_table,
                     arraysize(valid_texture_format_table)),
      texture_internal_format(valid_texture_internal_format_table,
                              arraysize(valid_texture_internal_format_table)),
      texture_internal_format_storage(
          valid_texture_internal_format_storage_table,
          arraysize(valid_texture_internal_format_storage_table)),
      texture_mag_filter_mode(valid_texture_mag_filter_mode_table,
                              arraysize(valid_texture_mag_filter_mode_table)),
      texture_min_filter_mode(valid_texture_min_filter_mode_table,
                              arraysize(valid_texture_min_filter_mode_table)),
      texture_parameter(valid_texture_parameter_table,
                        arraysize(valid_texture_parameter_table)),
      texture_pool(valid_texture_pool_table,
                   arraysize(valid_texture_pool_table)),
      texture_target(valid_texture_target_table,
                     arraysize(valid_texture_target_table)),
      texture_usage(valid_texture_usage_table,
                    arraysize(valid_texture_usage_table)),
      texture_wrap_mode(valid_texture_wrap_mode_table,
                        arraysize(valid_texture_wrap_mode_table)),
      vertex_attrib_size(valid_vertex_attrib_size_table,
                         arraysize(valid_vertex_attrib_size_table)),
      vertex_attrib_type(valid_vertex_attrib_type_table,
                         arraysize(valid_vertex_attrib_type_table)),
      vertex_attribute(valid_vertex_attribute_table,
                       arraysize(valid_vertex_attribute_table)),
      vertex_pointer(valid_vertex_pointer_table,
                     arraysize(valid_vertex_pointer_table)) {
}

#endif  // GPU_COMMAND_BUFFER_SERVICE_GLES2_CMD_VALIDATION_IMPLEMENTATION_AUTOGEN_H_
