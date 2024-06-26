// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_image_io_surface.h"

#include <map>

#include "base/lazy_instance.h"
#include "base/mac/foundation_util.h"
#include "base/strings/stringize_macros.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/memory_allocator_dump.h"
#include "base/trace_event/memory_dump_manager.h"
#include "base/trace_event/process_memory_dump.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_helper.h"
#include "ui/gl/scoped_binders.h"

// Note that this must be included after gl_bindings.h to avoid conflicts.
#include <OpenGL/CGLIOSurface.h>
#include <Quartz/Quartz.h>
#include <stddef.h>

using gfx::BufferFormat;

namespace gl {
namespace {

using WidgetToLayerMap = std::map<gfx::AcceleratedWidget, CALayer*>;
base::LazyInstance<WidgetToLayerMap> g_widget_to_layer_map;

const char kGLSLVersion[] = "#version 110";

const char kTextureRectangleRequired[] =
    "#extension GL_ARB_texture_rectangle : require";

// clang-format off
const char kVertexShader[] =
STRINGIZE(
  attribute vec2 a_position;
  uniform vec2 a_texScale;
  varying vec2 v_texCoord;
  void main() {
    gl_Position = vec4(a_position.x, a_position.y, 0.0, 1.0);
    v_texCoord = (a_position + vec2(1.0, 1.0)) * 0.5 * a_texScale;
  }
);

const char kFragmentShader[] =
STRINGIZE(
  uniform sampler2DRect a_y_texture;
  uniform sampler2DRect a_uv_texture;
  varying vec2 v_texCoord;
  void main() {
    vec3 yuv_adj = vec3(-0.0625, -0.5, -0.5);
    mat3 yuv_matrix = mat3(vec3(1.164, 1.164, 1.164),
                           vec3(0.0, -.391, 2.018),
                           vec3(1.596, -.813, 0.0));
    vec3 yuv = vec3(
        texture2DRect(a_y_texture, v_texCoord).r,
        texture2DRect(a_uv_texture, v_texCoord * 0.5).rg);
    gl_FragColor = vec4(yuv_matrix * (yuv + yuv_adj), 1.0);
  }
);
// clang-format on

bool ValidInternalFormat(unsigned internalformat) {
  switch (internalformat) {
    case GL_RED:
    case GL_BGRA_EXT:
    case GL_RGB:
    case GL_RGB_YCBCR_420V_CHROMIUM:
    case GL_RGB_YCBCR_422_CHROMIUM:
    case GL_RGBA:
      return true;
    default:
      return false;
  }
}

bool ValidFormat(BufferFormat format) {
  switch (format) {
    case BufferFormat::R_8:
    case BufferFormat::BGRA_8888:
    case BufferFormat::RGBA_8888:
    case BufferFormat::UYVY_422:
    case BufferFormat::YUV_420_BIPLANAR:
      return true;
    case BufferFormat::ATC:
    case BufferFormat::ATCIA:
    case BufferFormat::DXT1:
    case BufferFormat::DXT5:
    case BufferFormat::ETC1:
    case BufferFormat::RGBA_4444:
    case BufferFormat::RGBX_8888:
    case BufferFormat::BGRX_8888:
    case BufferFormat::YUV_420:
      return false;
  }

  NOTREACHED();
  return false;
}

GLenum TextureFormat(BufferFormat format) {
  switch (format) {
    case BufferFormat::R_8:
      return GL_RED;
    case BufferFormat::BGRA_8888:
    case BufferFormat::RGBA_8888:
      return GL_RGBA;
    case BufferFormat::UYVY_422:
      return GL_RGB;
    case BufferFormat::YUV_420_BIPLANAR:
      return GL_RGB_YCBCR_420V_CHROMIUM;
    case BufferFormat::ATC:
    case BufferFormat::ATCIA:
    case BufferFormat::DXT1:
    case BufferFormat::DXT5:
    case BufferFormat::ETC1:
    case BufferFormat::RGBA_4444:
    case BufferFormat::RGBX_8888:
    case BufferFormat::BGRX_8888:
    case BufferFormat::YUV_420:
      NOTREACHED();
      return 0;
  }

  NOTREACHED();
  return 0;
}

GLenum DataFormat(BufferFormat format) {
  switch (format) {
    case BufferFormat::R_8:
      return GL_RED;
    case BufferFormat::BGRA_8888:
    case BufferFormat::RGBA_8888:
      return GL_BGRA;
    case BufferFormat::UYVY_422:
      return GL_YCBCR_422_APPLE;
    case BufferFormat::ATC:
    case BufferFormat::ATCIA:
    case BufferFormat::DXT1:
    case BufferFormat::DXT5:
    case BufferFormat::ETC1:
    case BufferFormat::RGBA_4444:
    case BufferFormat::RGBX_8888:
    case BufferFormat::BGRX_8888:
    case BufferFormat::YUV_420:
    case BufferFormat::YUV_420_BIPLANAR:
      NOTREACHED();
      return 0;
  }

  NOTREACHED();
  return 0;
}

GLenum DataType(BufferFormat format) {
  switch (format) {
    case BufferFormat::R_8:
      return GL_UNSIGNED_BYTE;
    case BufferFormat::BGRA_8888:
    case BufferFormat::RGBA_8888:
      return GL_UNSIGNED_INT_8_8_8_8_REV;
    case BufferFormat::UYVY_422:
      return GL_UNSIGNED_SHORT_8_8_APPLE;
      break;
    case BufferFormat::ATC:
    case BufferFormat::ATCIA:
    case BufferFormat::DXT1:
    case BufferFormat::DXT5:
    case BufferFormat::ETC1:
    case BufferFormat::RGBA_4444:
    case BufferFormat::RGBX_8888:
    case BufferFormat::BGRX_8888:
    case BufferFormat::YUV_420:
    case BufferFormat::YUV_420_BIPLANAR:
      NOTREACHED();
      return 0;
  }

  NOTREACHED();
  return 0;
}

}  // namespace

GLImageIOSurface::GLImageIOSurface(const gfx::Size& size,
                                   unsigned internalformat)
    : size_(size),
      internalformat_(internalformat),
      format_(BufferFormat::RGBA_8888) {}

GLImageIOSurface::~GLImageIOSurface() {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(!io_surface_);
}

bool GLImageIOSurface::Initialize(IOSurfaceRef io_surface,
                                  gfx::GenericSharedMemoryId io_surface_id,
                                  BufferFormat format) {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(!io_surface_);

  if (!ValidInternalFormat(internalformat_)) {
    LOG(ERROR) << "Invalid internalformat: " << internalformat_;
    return false;
  }

  if (!ValidFormat(format)) {
    LOG(ERROR) << "Invalid format: " << static_cast<int>(format);
    return false;
  }

  format_ = format;
  io_surface_.reset(io_surface, base::scoped_policy::RETAIN);
  io_surface_id_ = io_surface_id;
  return true;
}

void GLImageIOSurface::Destroy(bool have_context) {
  DCHECK(thread_checker_.CalledOnValidThread());
  if (have_context && framebuffer_) {
    glDeleteProgram(program_);
    glDeleteShader(vertex_shader_);
    glDeleteShader(fragment_shader_);
    glDeleteBuffersARB(1, &vertex_buffer_);
    glDeleteFramebuffersEXT(1, &framebuffer_);
    glDeleteTextures(2, yuv_textures_);
  }
  io_surface_.reset();
}

gfx::Size GLImageIOSurface::GetSize() {
  return size_;
}

unsigned GLImageIOSurface::GetInternalFormat() {
  return internalformat_;
}

bool GLImageIOSurface::BindTexImage(unsigned target) {
  DCHECK(thread_checker_.CalledOnValidThread());

  // YUV_420_BIPLANAR is not supported by BindTexImage.
  // CopyTexImage is supported by this format as that performs conversion to RGB
  // as part of the copy operation.
  if (format_ == BufferFormat::YUV_420_BIPLANAR)
    return false;

  if (target != GL_TEXTURE_RECTANGLE_ARB) {
    // This might be supported in the future. For now, perform strict
    // validation so we know what's going on.
    LOG(ERROR) << "IOSurface requires TEXTURE_RECTANGLE_ARB target";
    return false;
  }

  CGLContextObj cgl_context =
      static_cast<CGLContextObj>(gfx::GLContext::GetCurrent()->GetHandle());

  DCHECK(io_surface_);
  CGLError cgl_error =
      CGLTexImageIOSurface2D(cgl_context, target, TextureFormat(format_),
                             size_.width(), size_.height(), DataFormat(format_),
                             DataType(format_), io_surface_.get(), 0);
  if (cgl_error != kCGLNoError) {
    LOG(ERROR) << "Error in CGLTexImageIOSurface2D";
    return false;
  }

  return true;
}

bool GLImageIOSurface::CopyTexImage(unsigned target) {
  DCHECK(thread_checker_.CalledOnValidThread());

  if (format_ != BufferFormat::YUV_420_BIPLANAR)
    return false;
  if (target != GL_TEXTURE_RECTANGLE_ARB) {
    LOG(ERROR) << "YUV_420_BIPLANAR requires GL_TEXTURE_RECTANGLE_ARB target";
    return false;
  }

  if (!framebuffer_) {
    glGenFramebuffersEXT(1, &framebuffer_);
    vertex_buffer_ = gfx::GLHelper::SetupQuadVertexBuffer();
    vertex_shader_ = gfx::GLHelper::LoadShader(
        GL_VERTEX_SHADER,
        base::StringPrintf("%s\n%s", kGLSLVersion, kVertexShader).c_str());
    fragment_shader_ = gfx::GLHelper::LoadShader(
        GL_FRAGMENT_SHADER,
        base::StringPrintf("%s\n%s\n%s", kGLSLVersion,
                           kTextureRectangleRequired, kFragmentShader)
            .c_str());
    program_ = gfx::GLHelper::SetupProgram(vertex_shader_, fragment_shader_);
    gfx::ScopedUseProgram use_program(program_);

    size_location_ = glGetUniformLocation(program_, "a_texScale");
    DCHECK_NE(-1, size_location_);
    int y_sampler_location = glGetUniformLocation(program_, "a_y_texture");
    DCHECK_NE(-1, y_sampler_location);
    int uv_sampler_location = glGetUniformLocation(program_, "a_uv_texture");
    DCHECK_NE(-1, uv_sampler_location);

    glUniform1i(y_sampler_location, 0);
    glUniform1i(uv_sampler_location, 1);

    glGenTextures(2, yuv_textures_);
    DCHECK(yuv_textures_[0]);
    DCHECK(yuv_textures_[1]);
  }

  CGLContextObj cgl_context =
      static_cast<CGLContextObj>(gfx::GLContext::GetCurrent()->GetHandle());

  GLint target_texture = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE_ARB, &target_texture);
  DCHECK(target_texture);
  glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, size_.width(),
               size_.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

  CGLError cgl_error = kCGLNoError;
  {
    DCHECK(io_surface_);

    gfx::ScopedActiveTexture active_texture0(GL_TEXTURE0);
    gfx::ScopedTextureBinder texture_y_binder(GL_TEXTURE_RECTANGLE_ARB,
                                              yuv_textures_[0]);
    cgl_error = CGLTexImageIOSurface2D(
        cgl_context, GL_TEXTURE_RECTANGLE_ARB, GL_RED, size_.width(),
        size_.height(), GL_RED, GL_UNSIGNED_BYTE, io_surface_.get(), 0);
    if (cgl_error != kCGLNoError) {
      LOG(ERROR) << "Error in CGLTexImageIOSurface2D for the Y plane. "
                 << cgl_error;
      return false;
    }
    {
      gfx::ScopedActiveTexture active_texture1(GL_TEXTURE1);
      gfx::ScopedTextureBinder texture_uv_binder(GL_TEXTURE_RECTANGLE_ARB,
                                                 yuv_textures_[1]);
      cgl_error = CGLTexImageIOSurface2D(
          cgl_context, GL_TEXTURE_RECTANGLE_ARB, GL_RG, size_.width() / 2,
          size_.height() / 2, GL_RG, GL_UNSIGNED_BYTE, io_surface_.get(), 1);
      if (cgl_error != kCGLNoError) {
        LOG(ERROR) << "Error in CGLTexImageIOSurface2D for the UV plane. "
                   << cgl_error;
        return false;
      }

      gfx::ScopedFrameBufferBinder framebuffer_binder(framebuffer_);
      gfx::ScopedViewport viewport(0, 0, size_.width(), size_.height());
      glViewport(0, 0, size_.width(), size_.height());
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_RECTANGLE_ARB, target_texture, 0);
      DCHECK_EQ(static_cast<GLenum>(GL_FRAMEBUFFER_COMPLETE),
                glCheckFramebufferStatusEXT(GL_FRAMEBUFFER));

      gfx::ScopedUseProgram use_program(program_);
      glUniform2f(size_location_, size_.width(), size_.height());

      gfx::GLHelper::DrawQuad(vertex_buffer_);
      // Detach the output texture from the fbo.
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_RECTANGLE_ARB, 0, 0);
    }
  }
  return true;
}

bool GLImageIOSurface::CopyTexSubImage(unsigned target,
                                       const gfx::Point& offset,
                                       const gfx::Rect& rect) {
  return false;
}

bool GLImageIOSurface::ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
                                            int z_order,
                                            gfx::OverlayTransform transform,
                                            const gfx::Rect& bounds_rect,
                                            const gfx::RectF& crop_rect) {
  NOTREACHED();
  return false;
}

void GLImageIOSurface::OnMemoryDump(base::trace_event::ProcessMemoryDump* pmd,
                                    uint64_t process_tracing_id,
                                    const std::string& dump_name) {
  // IOSurfaceGetAllocSize will return 0 if io_surface_ is invalid. In this case
  // we log 0 for consistency with other GLImage memory dump functions.
  size_t size_bytes = IOSurfaceGetAllocSize(io_surface_);

  base::trace_event::MemoryAllocatorDump* dump =
      pmd->CreateAllocatorDump(dump_name);
  dump->AddScalar(base::trace_event::MemoryAllocatorDump::kNameSize,
                  base::trace_event::MemoryAllocatorDump::kUnitsBytes,
                  static_cast<uint64_t>(size_bytes));

  auto guid =
      GetGenericSharedMemoryGUIDForTracing(process_tracing_id, io_surface_id_);
  pmd->CreateSharedGlobalAllocatorDump(guid);
  pmd->AddOwnershipEdge(dump->guid(), guid);
}

base::ScopedCFTypeRef<IOSurfaceRef> GLImageIOSurface::io_surface() {
  return io_surface_;
}

// static
void GLImageIOSurface::SetLayerForWidget(gfx::AcceleratedWidget widget,
                                         CALayer* layer) {
  if (layer)
    g_widget_to_layer_map.Pointer()->insert(std::make_pair(widget, layer));
  else
    g_widget_to_layer_map.Pointer()->erase(widget);
}

// static
unsigned GLImageIOSurface::GetInternalFormatForTesting(
    gfx::BufferFormat format) {
  DCHECK(ValidFormat(format));
  return TextureFormat(format);
}
}  // namespace gl
