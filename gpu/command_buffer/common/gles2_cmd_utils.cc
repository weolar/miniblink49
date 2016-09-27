// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is here so other GLES2 related files can have a common set of
// includes where appropriate.

#include <sstream>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2extchromium.h>

#include "gpu/command_buffer/common/gles2_cmd_format.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"

namespace gpu {
namespace gles2 {

namespace gl_error_bit {
enum GLErrorBit {
  kNoError = 0,
  kInvalidEnum = (1 << 0),
  kInvalidValue = (1 << 1),
  kInvalidOperation = (1 << 2),
  kOutOfMemory = (1 << 3),
  kInvalidFrameBufferOperation = (1 << 4)
};
}

int GLES2Util::GLGetNumValuesReturned(int id) const {
  switch (id) {
    // -- glGetBooleanv, glGetFloatv, glGetIntergerv
    case GL_ACTIVE_TEXTURE:
      return 1;
    case GL_ALIASED_LINE_WIDTH_RANGE:
      return 2;
    case GL_ALIASED_POINT_SIZE_RANGE:
      return 2;
    case GL_ALPHA_BITS:
      return 1;
    case GL_ARRAY_BUFFER_BINDING:
      return 1;
    case GL_BLEND:
      return 1;
    case GL_BLEND_COLOR:
      return 4;
    case GL_BLEND_DST_ALPHA:
      return 1;
    case GL_BLEND_DST_RGB:
      return 1;
    case GL_BLEND_EQUATION_ALPHA:
      return 1;
    case GL_BLEND_EQUATION_RGB:
      return 1;
    case GL_BLEND_SRC_ALPHA:
      return 1;
    case GL_BLEND_SRC_RGB:
      return 1;
    case GL_BLUE_BITS:
      return 1;
    case GL_COLOR_CLEAR_VALUE:
      return 4;
    case GL_COLOR_WRITEMASK:
      return 4;
    case GL_COMPRESSED_TEXTURE_FORMATS:
      return num_compressed_texture_formats_;
    case GL_CULL_FACE:
      return 1;
    case GL_CULL_FACE_MODE:
      return 1;
    case GL_CURRENT_PROGRAM:
      return 1;
    case GL_DEPTH_BITS:
      return 1;
    case GL_DEPTH_CLEAR_VALUE:
      return 1;
    case GL_DEPTH_FUNC:
      return 1;
    case GL_DEPTH_RANGE:
      return 2;
    case GL_DEPTH_TEST:
      return 1;
    case GL_DEPTH_WRITEMASK:
      return 1;
    case GL_DITHER:
      return 1;
    case GL_ELEMENT_ARRAY_BUFFER_BINDING:
      return 1;
    case GL_FRAMEBUFFER_BINDING:
      return 1;
    case GL_FRONT_FACE:
      return 1;
    case GL_GENERATE_MIPMAP_HINT:
      return 1;
    case GL_GREEN_BITS:
      return 1;
    case GL_IMPLEMENTATION_COLOR_READ_FORMAT:
      return 1;
    case GL_IMPLEMENTATION_COLOR_READ_TYPE:
      return 1;
    case GL_LINE_WIDTH:
      return 1;
    case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
      return 1;
    case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
      return 1;
    case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
      return 1;
    case GL_MAX_RENDERBUFFER_SIZE:
      return 1;
    case GL_MAX_TEXTURE_IMAGE_UNITS:
      return 1;
    case GL_MAX_TEXTURE_SIZE:
      return 1;
    case GL_MAX_VARYING_VECTORS:
      return 1;
    case GL_MAX_VERTEX_ATTRIBS:
      return 1;
    case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
      return 1;
    case GL_MAX_VERTEX_UNIFORM_VECTORS:
      return 1;
    case GL_MAX_VIEWPORT_DIMS:
      return 2;
    case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
      return 1;
    case GL_NUM_SHADER_BINARY_FORMATS:
      return 1;
    case GL_PACK_ALIGNMENT:
      return 1;
    case GL_PACK_REVERSE_ROW_ORDER_ANGLE:
      return 1;
    case GL_POLYGON_OFFSET_FACTOR:
      return 1;
    case GL_POLYGON_OFFSET_FILL:
      return 1;
    case GL_POLYGON_OFFSET_UNITS:
      return 1;
    case GL_RED_BITS:
      return 1;
    case GL_RENDERBUFFER_BINDING:
      return 1;
    case GL_SAMPLE_BUFFERS:
      return 1;
    case GL_SAMPLE_COVERAGE_INVERT:
      return 1;
    case GL_SAMPLE_COVERAGE_VALUE:
      return 1;
    case GL_SAMPLES:
      return 1;
    case GL_SCISSOR_BOX:
      return 4;
    case GL_SCISSOR_TEST:
      return 1;
    case GL_SHADER_BINARY_FORMATS:
      return num_shader_binary_formats_;
    case GL_SHADER_COMPILER:
      return 1;
    case GL_STENCIL_BACK_FAIL:
      return 1;
    case GL_STENCIL_BACK_FUNC:
      return 1;
    case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
      return 1;
    case GL_STENCIL_BACK_PASS_DEPTH_PASS:
      return 1;
    case GL_STENCIL_BACK_REF:
      return 1;
    case GL_STENCIL_BACK_VALUE_MASK:
      return 1;
    case GL_STENCIL_BACK_WRITEMASK:
      return 1;
    case GL_STENCIL_BITS:
      return 1;
    case GL_STENCIL_CLEAR_VALUE:
      return 1;
    case GL_STENCIL_FAIL:
      return 1;
    case GL_STENCIL_FUNC:
      return 1;
    case GL_STENCIL_PASS_DEPTH_FAIL:
      return 1;
    case GL_STENCIL_PASS_DEPTH_PASS:
      return 1;
    case GL_STENCIL_REF:
      return 1;
    case GL_STENCIL_TEST:
      return 1;
    case GL_STENCIL_VALUE_MASK:
      return 1;
    case GL_STENCIL_WRITEMASK:
      return 1;
    case GL_SUBPIXEL_BITS:
      return 1;
    case GL_TEXTURE_BINDING_2D:
      return 1;
    case GL_TEXTURE_BINDING_CUBE_MAP:
      return 1;
    case GL_TEXTURE_BINDING_EXTERNAL_OES:
      return 1;
    case GL_TEXTURE_BINDING_RECTANGLE_ARB:
      return 1;
    case GL_TEXTURE_IMMUTABLE_FORMAT_EXT:
      return 1;
    case GL_UNPACK_ALIGNMENT:
      return 1;
    case GL_VIEWPORT:
      return 4;
    // -- glGetBooleanv, glGetFloatv, glGetIntergerv with
    //    GL_CHROMIUM_framebuffer_multisample
    case GL_MAX_SAMPLES_EXT:
      return 1;
    case GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT:
      return 1;

    // -- glGetBufferParameteriv
    case GL_BUFFER_SIZE:
      return 1;
    case GL_BUFFER_USAGE:
      return 1;

    // -- glGetFramebufferAttachmentParameteriv
    case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:
      return 1;
    case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME:
      return 1;
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL:
      return 1;
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE:
      return 1;
    // -- glGetFramebufferAttachmentParameteriv with
    //    GL_EXT_multisampled_render_to_texture
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT:
      return 1;

    // -- glGetProgramiv
    case GL_DELETE_STATUS:
      return 1;
    case GL_LINK_STATUS:
      return 1;
    case GL_VALIDATE_STATUS:
      return 1;
    case GL_INFO_LOG_LENGTH:
      return 1;
    case GL_ATTACHED_SHADERS:
      return 1;
    case GL_ACTIVE_ATTRIBUTES:
      return 1;
    case GL_ACTIVE_ATTRIBUTE_MAX_LENGTH:
      return 1;
    case GL_ACTIVE_UNIFORMS:
      return 1;
    case GL_ACTIVE_UNIFORM_MAX_LENGTH:
      return 1;


    // -- glGetRenderbufferAttachmentParameteriv
    case GL_RENDERBUFFER_WIDTH:
      return 1;
    case GL_RENDERBUFFER_HEIGHT:
      return 1;
    case GL_RENDERBUFFER_INTERNAL_FORMAT:
      return 1;
    case GL_RENDERBUFFER_RED_SIZE:
      return 1;
    case GL_RENDERBUFFER_GREEN_SIZE:
      return 1;
    case GL_RENDERBUFFER_BLUE_SIZE:
      return 1;
    case GL_RENDERBUFFER_ALPHA_SIZE:
      return 1;
    case GL_RENDERBUFFER_DEPTH_SIZE:
      return 1;
    case GL_RENDERBUFFER_STENCIL_SIZE:
      return 1;
    // -- glGetRenderbufferAttachmentParameteriv with
    //    GL_EXT_multisampled_render_to_texture
    case GL_RENDERBUFFER_SAMPLES_EXT:
      return 1;

    // -- glGetShaderiv
    case GL_SHADER_TYPE:
      return 1;
    // Already defined under glGetFramebufferAttachemntParameteriv.
    // case GL_DELETE_STATUS:
    //   return 1;
    case GL_COMPILE_STATUS:
      return 1;
    // Already defined under glGetFramebufferAttachemntParameteriv.
    // case GL_INFO_LOG_LENGTH:
    //   return 1;
    case GL_SHADER_SOURCE_LENGTH:
      return 1;
    case GL_TRANSLATED_SHADER_SOURCE_LENGTH_ANGLE:
      return 1;

    // -- glGetTexParameterfv, glGetTexParameteriv
    case GL_TEXTURE_MAG_FILTER:
      return 1;
    case GL_TEXTURE_MIN_FILTER:
      return 1;
    case GL_TEXTURE_WRAP_S:
      return 1;
    case GL_TEXTURE_WRAP_T:
      return 1;
    case GL_TEXTURE_MAX_ANISOTROPY_EXT:
      return 1;

    // -- glGetVertexAttribfv, glGetVertexAttribiv
    case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
      return 1;
    case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
      return 1;
    case GL_VERTEX_ATTRIB_ARRAY_SIZE:
      return 1;
    case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
      return 1;
    case GL_VERTEX_ATTRIB_ARRAY_TYPE:
      return 1;
    case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
      return 1;
    case GL_CURRENT_VERTEX_ATTRIB:
      return 4;

    // -- glHint with GL_OES_standard_derivatives
    case GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES:
      return 1;

    // Chromium internal bind_generates_resource query
    case GL_BIND_GENERATES_RESOURCE_CHROMIUM:
      return 1;

    // bad enum
    default:
      return 0;
  }
}

namespace {

// Return the number of elements per group of a specified format.
int ElementsPerGroup(int format, int type) {
  switch (type) {
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_INT_24_8_OES:
       return 1;
    default:
       break;
    }

    switch (format) {
    case GL_RGB:
       return 3;
    case GL_LUMINANCE_ALPHA:
       return 2;
    case GL_RGBA:
    case GL_BGRA_EXT:
       return 4;
    case GL_ALPHA:
    case GL_LUMINANCE:
    case GL_DEPTH_COMPONENT:
    case GL_DEPTH_COMPONENT24_OES:
    case GL_DEPTH_COMPONENT32_OES:
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH24_STENCIL8_OES:
    case GL_DEPTH_STENCIL_OES:
       return 1;
    default:
       return 0;
  }
}

// Return the number of bytes per element, based on the element type.
int BytesPerElement(int type) {
  switch (type) {
    case GL_FLOAT:
    case GL_UNSIGNED_INT_24_8_OES:
    case GL_UNSIGNED_INT:
      return 4;
    case GL_HALF_FLOAT_OES:
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
       return 2;
    case GL_UNSIGNED_BYTE:
    case GL_BYTE:
       return 1;
    default:
       return 0;
  }
}

}  // anonymous namespace

uint32 GLES2Util::ComputeImageGroupSize(int format, int type) {
  return BytesPerElement(type) * ElementsPerGroup(format, type);
}

bool GLES2Util::ComputeImagePaddedRowSize(
        int width, int format, int type, int unpack_alignment,
        uint32* padded_row_size) {
  uint32 bytes_per_group = ComputeImageGroupSize(format, type);
  uint32 unpadded_row_size;
  if (!SafeMultiplyUint32(width, bytes_per_group, &unpadded_row_size)) {
    return false;
  }
  uint32 temp;
  if (!SafeAddUint32(unpadded_row_size, unpack_alignment - 1, &temp)) {
      return false;
  }
  *padded_row_size = (temp / unpack_alignment) * unpack_alignment;
  return true;
}

// Returns the amount of data glTexImage2D or glTexSubImage2D will access.
bool GLES2Util::ComputeImageDataSizes(
    int width, int height, int format, int type, int unpack_alignment,
    uint32* size, uint32* ret_unpadded_row_size, uint32* ret_padded_row_size) {
  uint32 bytes_per_group = ComputeImageGroupSize(format, type);
  uint32 row_size;
  if (!SafeMultiplyUint32(width, bytes_per_group, &row_size)) {
    return false;
  }
  if (height > 1) {
    uint32 temp;
    if (!SafeAddUint32(row_size, unpack_alignment - 1, &temp)) {
      return false;
    }
    uint32 padded_row_size = (temp / unpack_alignment) * unpack_alignment;
    uint32 size_of_all_but_last_row;
    if (!SafeMultiplyUint32((height - 1), padded_row_size,
                            &size_of_all_but_last_row)) {
      return false;
    }
    if (!SafeAddUint32(size_of_all_but_last_row, row_size, size)) {
      return false;
    }
    if (ret_padded_row_size) {
      *ret_padded_row_size = padded_row_size;
    }
  } else {
    if (!SafeMultiplyUint32(height, row_size, size)) {
      return false;
    }
    if (ret_padded_row_size) {
      *ret_padded_row_size = row_size;
    }
  }
  if (ret_unpadded_row_size) {
    *ret_unpadded_row_size = row_size;
  }

  return true;
}

size_t GLES2Util::RenderbufferBytesPerPixel(int format) {
  switch (format) {
    case GL_STENCIL_INDEX8:
      return 1;
    case GL_RGBA4:
    case GL_RGB565:
    case GL_RGB5_A1:
    case GL_DEPTH_COMPONENT16:
      return 2;
    case GL_RGB:
    case GL_RGBA:
    case GL_DEPTH24_STENCIL8_OES:
    case GL_RGB8_OES:
    case GL_RGBA8_OES:
    case GL_DEPTH_COMPONENT24_OES:
      return 4;
    default:
      return 0;
  }
}

uint32 GLES2Util::GetGLDataTypeSizeForUniforms(int type) {
  switch (type) {
    case GL_FLOAT:
      return sizeof(GLfloat);              // NOLINT
    case GL_FLOAT_VEC2:
      return sizeof(GLfloat) * 2;          // NOLINT
    case GL_FLOAT_VEC3:
      return sizeof(GLfloat) * 3;          // NOLINT
    case GL_FLOAT_VEC4:
      return sizeof(GLfloat) * 4;          // NOLINT
    case GL_INT:
      return sizeof(GLint);                // NOLINT
    case GL_INT_VEC2:
      return sizeof(GLint) * 2;            // NOLINT
    case GL_INT_VEC3:
      return sizeof(GLint) * 3;            // NOLINT
    case GL_INT_VEC4:
      return sizeof(GLint) * 4;            // NOLINT
    case GL_BOOL:
      return sizeof(GLint);                // NOLINT
    case GL_BOOL_VEC2:
      return sizeof(GLint) * 2;            // NOLINT
    case GL_BOOL_VEC3:
      return sizeof(GLint) * 3;            // NOLINT
    case GL_BOOL_VEC4:
      return sizeof(GLint) * 4;            // NOLINT
    case GL_FLOAT_MAT2:
      return sizeof(GLfloat) * 2 * 2;      // NOLINT
    case GL_FLOAT_MAT3:
      return sizeof(GLfloat) * 3 * 3;      // NOLINT
    case GL_FLOAT_MAT4:
      return sizeof(GLfloat) * 4 * 4;      // NOLINT
    case GL_SAMPLER_2D:
      return sizeof(GLint);                // NOLINT
    case GL_SAMPLER_2D_RECT_ARB:
      return sizeof(GLint);                // NOLINT
    case GL_SAMPLER_CUBE:
      return sizeof(GLint);                // NOLINT
    case GL_SAMPLER_EXTERNAL_OES:
      return sizeof(GLint);                // NOLINT
    default:
      return 0;
  }
}

size_t GLES2Util::GetGLTypeSizeForTexturesAndBuffers(uint32 type) {
  switch (type) {
    case GL_BYTE:
      return sizeof(GLbyte);  // NOLINT
    case GL_UNSIGNED_BYTE:
      return sizeof(GLubyte);  // NOLINT
    case GL_SHORT:
      return sizeof(GLshort);  // NOLINT
    case GL_UNSIGNED_SHORT:
      return sizeof(GLushort);  // NOLINT
    case GL_INT:
      return sizeof(GLint);  // NOLINT
    case GL_UNSIGNED_INT:
      return sizeof(GLuint);  // NOLINT
    case GL_FLOAT:
      return sizeof(GLfloat);  // NOLINT
    case GL_FIXED:
      return sizeof(GLfixed);  // NOLINT
    default:
      return 0;
  }
}

uint32 GLES2Util::GLErrorToErrorBit(uint32 error) {
  switch (error) {
    case GL_INVALID_ENUM:
      return gl_error_bit::kInvalidEnum;
    case GL_INVALID_VALUE:
      return gl_error_bit::kInvalidValue;
    case GL_INVALID_OPERATION:
      return gl_error_bit::kInvalidOperation;
    case GL_OUT_OF_MEMORY:
      return gl_error_bit::kOutOfMemory;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return gl_error_bit::kInvalidFrameBufferOperation;
    default:
      NOTREACHED();
      return gl_error_bit::kNoError;
  }
}

uint32 GLES2Util::GLErrorBitToGLError(uint32 error_bit) {
  switch (error_bit) {
    case gl_error_bit::kInvalidEnum:
      return GL_INVALID_ENUM;
    case gl_error_bit::kInvalidValue:
      return GL_INVALID_VALUE;
    case gl_error_bit::kInvalidOperation:
      return GL_INVALID_OPERATION;
    case gl_error_bit::kOutOfMemory:
      return GL_OUT_OF_MEMORY;
    case gl_error_bit::kInvalidFrameBufferOperation:
      return GL_INVALID_FRAMEBUFFER_OPERATION;
    default:
      NOTREACHED();
      return GL_NO_ERROR;
  }
}

uint32 GLES2Util::IndexToGLFaceTarget(int index) {
  static uint32 faces[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
  };
  return faces[index];
}

uint32 GLES2Util::GetPreferredGLReadPixelsFormat(uint32 internal_format) {
  switch (internal_format) {
    case GL_RGB16F_EXT:
    case GL_RGB32F_EXT:
      return GL_RGB;
    case GL_RGBA16F_EXT:
    case GL_RGBA32F_EXT:
      return GL_RGBA;
    default:
      return GL_RGBA;
  }
}

uint32 GLES2Util::GetPreferredGLReadPixelsType(
    uint32 internal_format, uint32 texture_type) {
  switch (internal_format) {
    case GL_RGBA32F_EXT:
    case GL_RGB32F_EXT:
      return GL_FLOAT;
    case GL_RGBA16F_EXT:
    case GL_RGB16F_EXT:
      return GL_HALF_FLOAT_OES;
    case GL_RGBA:
    case GL_RGB:
      // Unsized internal format, check the type
      switch (texture_type) {
        case GL_FLOAT:
        case GL_HALF_FLOAT_OES:
          return GL_FLOAT;
        default:
          return GL_UNSIGNED_BYTE;
      }
    default:
      return GL_UNSIGNED_BYTE;
  }
}

uint32 GLES2Util::GetChannelsForFormat(int format) {
  switch (format) {
    case GL_ALPHA:
    case GL_ALPHA16F_EXT:
    case GL_ALPHA32F_EXT:
      return kAlpha;
    case GL_LUMINANCE:
      return kRGB;
    case GL_LUMINANCE_ALPHA:
      return kRGBA;
    case GL_RGB:
    case GL_RGB8_OES:
    case GL_RGB565:
    case GL_RGB16F_EXT:
    case GL_RGB32F_EXT:
      return kRGB;
    case GL_BGRA_EXT:
    case GL_BGRA8_EXT:
    case GL_RGBA16F_EXT:
    case GL_RGBA32F_EXT:
    case GL_RGBA:
    case GL_RGBA8_OES:
    case GL_RGBA4:
    case GL_RGB5_A1:
      return kRGBA;
    case GL_DEPTH_COMPONENT32_OES:
    case GL_DEPTH_COMPONENT24_OES:
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT:
      return kDepth;
    case GL_STENCIL_INDEX8:
      return kStencil;
    case GL_DEPTH_STENCIL_OES:
    case GL_DEPTH24_STENCIL8_OES:
      return kDepth | kStencil;
    default:
      return 0x0000;
  }
}

uint32 GLES2Util::GetChannelsNeededForAttachmentType(
    int type, uint32 max_color_attachments) {
  switch (type) {
    case GL_DEPTH_ATTACHMENT:
      return kDepth;
    case GL_STENCIL_ATTACHMENT:
      return kStencil;
    default:
      if (type >= GL_COLOR_ATTACHMENT0 &&
          type < static_cast<int>(
              GL_COLOR_ATTACHMENT0 + max_color_attachments)) {
        return kRGBA;
      }
      return 0x0000;
  }
}

std::string GLES2Util::GetStringEnum(uint32 value) {
  const EnumToString* entry = enum_to_string_table_;
  const EnumToString* end = entry + enum_to_string_table_len_;
  for (;entry < end; ++entry) {
    if (value == entry->value) {
      return entry->name;
    }
  }
  std::stringstream ss;
  ss.fill('0');
  ss.width(value < 0x10000 ? 4 : 8);
  ss << std::hex << value;
  return "0x" + ss.str();
}

std::string GLES2Util::GetStringError(uint32 value) {
  static EnumToString string_table[] = {
    { GL_NONE, "GL_NONE" },
  };
  return GLES2Util::GetQualifiedEnumString(
      string_table, arraysize(string_table), value);
}

std::string GLES2Util::GetStringBool(uint32 value) {
  return value ? "GL_TRUE" : "GL_FALSE";
}

std::string GLES2Util::GetQualifiedEnumString(
    const EnumToString* table, size_t count, uint32 value) {
  for (const EnumToString* end = table + count; table < end; ++table) {
    if (table->value == value) {
      return table->name;
    }
  }
  return GetStringEnum(value);
}

bool GLES2Util::ParseUniformName(
    const std::string& name,
    size_t* array_pos,
    int* element_index,
    bool* getting_array) {
  bool getting_array_location = false;
  size_t open_pos = std::string::npos;
  int index = 0;
  if (name[name.size() - 1] == ']') {
    if (name.size() < 3) {
      return false;
    }
    open_pos = name.find_last_of('[');
    if (open_pos == std::string::npos ||
        open_pos >= name.size() - 2) {
      return false;
    }
    size_t last = name.size() - 1;
    for (size_t pos = open_pos + 1; pos < last; ++pos) {
      int8 digit = name[pos] - '0';
      if (digit < 0 || digit > 9) {
        return false;
      }
      index = index * 10 + digit;
    }
    getting_array_location = true;
  }
  *getting_array = getting_array_location;
  *element_index = index;
  *array_pos = open_pos;
  return true;
}

namespace {

// From <EGL/egl.h>.
const int32 kAlphaSize       = 0x3021;  // EGL_ALPHA_SIZE
const int32 kBlueSize        = 0x3022;  // EGL_BLUE_SIZE
const int32 kGreenSize       = 0x3023;  // EGL_GREEN_SIZE
const int32 kRedSize         = 0x3024;  // EGL_RED_SIZE
const int32 kDepthSize       = 0x3025;  // EGL_DEPTH_SIZE
const int32 kStencilSize     = 0x3026;  // EGL_STENCIL_SIZE
const int32 kSamples         = 0x3031;  // EGL_SAMPLES
const int32 kSampleBuffers   = 0x3032;  // EGL_SAMPLE_BUFFERS
const int32 kNone            = 0x3038;  // EGL_NONE
const int32 kSwapBehavior    = 0x3093;  // EGL_SWAP_BEHAVIOR
const int32 kBufferPreserved = 0x3094;  // EGL_BUFFER_PRESERVED
const int32 kBufferDestroyed = 0x3095;  // EGL_BUFFER_DESTROYED

// Chromium only.
const int32 kShareResources        = 0x10000;
const int32 kBindGeneratesResource = 0x10001;
const int32 kFailIfMajorPerfCaveat = 0x10002;
const int32 kLoseContextWhenOutOfMemory = 0x10003;

#ifdef TENCENT_CHANGES
const int32 kShareSysContext = 0x10004;
#endif

}  // namespace

ContextCreationAttribHelper::ContextCreationAttribHelper()
    : alpha_size_(-1),
      blue_size_(-1),
      green_size_(-1),
      red_size_(-1),
      depth_size_(-1),
      stencil_size_(-1),
      samples_(-1),
      sample_buffers_(-1),
      buffer_preserved_(true),
      share_resources_(false),
      bind_generates_resource_(true),
      fail_if_major_perf_caveat_(false),
#ifdef TENCENT_CHANGES
      lose_context_when_out_of_memory_(false),
      share_sys_context_(false) {}
#else
      lose_context_when_out_of_memory_(false) {}
#endif

void ContextCreationAttribHelper::Serialize(std::vector<int32>* attribs) {
  if (alpha_size_ != -1) {
    attribs->push_back(kAlphaSize);
    attribs->push_back(alpha_size_);
  }
  if (blue_size_ != -1) {
    attribs->push_back(kBlueSize);
    attribs->push_back(blue_size_);
  }
  if (green_size_ != -1) {
    attribs->push_back(kGreenSize);
    attribs->push_back(green_size_);
  }
  if (red_size_ != -1) {
    attribs->push_back(kRedSize);
    attribs->push_back(red_size_);
  }
  if (depth_size_ != -1) {
    attribs->push_back(kDepthSize);
    attribs->push_back(depth_size_);
  }
  if (stencil_size_ != -1) {
    attribs->push_back(kStencilSize);
    attribs->push_back(stencil_size_);
  }
  if (samples_ != -1) {
    attribs->push_back(kSamples);
    attribs->push_back(samples_);
  }
  if (sample_buffers_ != -1) {
    attribs->push_back(kSampleBuffers);
    attribs->push_back(sample_buffers_);
  }
  attribs->push_back(kSwapBehavior);
  attribs->push_back(buffer_preserved_ ? kBufferPreserved : kBufferDestroyed);
  attribs->push_back(kShareResources);
  attribs->push_back(share_resources_ ? 1 : 0);
  attribs->push_back(kBindGeneratesResource);
  attribs->push_back(bind_generates_resource_ ? 1 : 0);
  attribs->push_back(kFailIfMajorPerfCaveat);
  attribs->push_back(fail_if_major_perf_caveat_ ? 1 : 0);
  attribs->push_back(kLoseContextWhenOutOfMemory);
  attribs->push_back(lose_context_when_out_of_memory_ ? 1 : 0);
#ifdef TENCENT_CHANGES
  attribs->push_back(kShareSysContext);
  attribs->push_back(share_sys_context_ ? 1 : 0);
#endif
  attribs->push_back(kNone);
}

bool ContextCreationAttribHelper::Parse(const std::vector<int32>& attribs) {
  for (size_t i = 0; i < attribs.size(); i += 2) {
    const int32 attrib = attribs[i];
    if (i + 1 >= attribs.size()) {
      if (attrib == kNone) {
        return true;
      }

      DLOG(ERROR) << "Missing value after context creation attribute: "
                  << attrib;
      return false;
    }

    const int32 value = attribs[i+1];
    switch (attrib) {
      case kAlphaSize:
        alpha_size_ = value;
        break;
      case kBlueSize:
        blue_size_ = value;
        break;
      case kGreenSize:
        green_size_ = value;
        break;
      case kRedSize:
        red_size_ = value;
        break;
      case kDepthSize:
        depth_size_ = value;
        break;
      case kStencilSize:
        stencil_size_ = value;
        break;
      case kSamples:
        samples_ = value;
        break;
      case kSampleBuffers:
        sample_buffers_ = value;
        break;
      case kSwapBehavior:
        buffer_preserved_ = value == kBufferPreserved;
        break;
      case kShareResources:
        share_resources_ = value != 0;
        break;
      case kBindGeneratesResource:
        bind_generates_resource_ = value != 0;
        break;
      case kFailIfMajorPerfCaveat:
        fail_if_major_perf_caveat_ = value != 0;
        break;
      case kLoseContextWhenOutOfMemory:
        lose_context_when_out_of_memory_ = value != 0;
        break;
#ifdef TENCENT_CHANGES
      case kShareSysContext:
        share_sys_context_ = value != 0;
#endif
      case kNone:
        // Terminate list, even if more attributes.
        return true;
      default:
        DLOG(ERROR) << "Invalid context creation attribute: " << attrib;
        return false;
    }
  }

  return true;
}

#include "gpu/command_buffer/common/gles2_cmd_utils_implementation_autogen.h"

}  // namespace gles2
}  // namespace gpu

