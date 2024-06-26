// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gles2_cmd_copy_texture_chromium.h"

#include <algorithm>

#include "base/basictypes.h"
#include "gpu/command_buffer/service/gl_utils.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "ui/gl/gl_implementation.h"

namespace {

const GLfloat kIdentityMatrix[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f };

enum VertexShaderId {
    VERTEX_SHADER_COPY_TEXTURE,
    VERTEX_SHADER_COPY_TEXTURE_FLIP_Y,
    NUM_VERTEX_SHADERS,
};

enum FragmentShaderId {
    FRAGMENT_SHADER_COPY_TEXTURE_2D,
    FRAGMENT_SHADER_COPY_TEXTURE_RECTANGLE_ARB,
    FRAGMENT_SHADER_COPY_TEXTURE_EXTERNAL_OES,
    FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_2D,
    FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_RECTANGLE_ARB,
    FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_EXTERNAL_OES,
    FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_2D,
    FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_RECTANGLE_ARB,
    FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_EXTERNAL_OES,
    NUM_FRAGMENT_SHADERS,
};

// Returns the correct vertex shader id to evaluate the copy operation for
// the CHROMIUM_flipy setting.
VertexShaderId GetVertexShaderId(bool flip_y)
{
    // bit 0: flip y
    static VertexShaderId shader_ids[] = {
        VERTEX_SHADER_COPY_TEXTURE,
        VERTEX_SHADER_COPY_TEXTURE_FLIP_Y,
    };

    unsigned index = flip_y ? 1 : 0;
    return shader_ids[index];
}

// Returns the correct fragment shader id to evaluate the copy operation for
// the premultiply alpha pixel store settings and target.
FragmentShaderId GetFragmentShaderId(bool premultiply_alpha,
    bool unpremultiply_alpha,
    GLenum target)
{
    // Only one alpha mode at a time makes sense.
    DCHECK(!premultiply_alpha || !unpremultiply_alpha);

    enum {
        SAMPLER_2D,
        SAMPLER_RECTANGLE_ARB,
        SAMPLER_EXTERNAL_OES,
        NUM_SAMPLERS
    };

    // bit 0: premultiply alpha
    // bit 1: unpremultiply alpha
    static FragmentShaderId shader_ids[][NUM_SAMPLERS] = {
        {
            FRAGMENT_SHADER_COPY_TEXTURE_2D,
            FRAGMENT_SHADER_COPY_TEXTURE_RECTANGLE_ARB,
            FRAGMENT_SHADER_COPY_TEXTURE_EXTERNAL_OES,
        },
        {
            FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_2D,
            FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_RECTANGLE_ARB,
            FRAGMENT_SHADER_COPY_TEXTURE_PREMULTIPLY_ALPHA_EXTERNAL_OES,
        },
        {
            FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_2D,
            FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_RECTANGLE_ARB,
            FRAGMENT_SHADER_COPY_TEXTURE_UNPREMULTIPLY_ALPHA_EXTERNAL_OES,
        },
        {
            FRAGMENT_SHADER_COPY_TEXTURE_2D,
            FRAGMENT_SHADER_COPY_TEXTURE_RECTANGLE_ARB,
            FRAGMENT_SHADER_COPY_TEXTURE_EXTERNAL_OES,
        }
    };

    unsigned index = (premultiply_alpha ? (1 << 0) : 0) | (unpremultiply_alpha ? (1 << 1) : 0);

    switch (target) {
    case GL_TEXTURE_2D:
        return shader_ids[index][SAMPLER_2D];
    case GL_TEXTURE_RECTANGLE_ARB:
        return shader_ids[index][SAMPLER_RECTANGLE_ARB];
    case GL_TEXTURE_EXTERNAL_OES:
        return shader_ids[index][SAMPLER_EXTERNAL_OES];
    default:
        break;
    }

    NOTREACHED();
    return shader_ids[0][SAMPLER_2D];
}

const char* kShaderPrecisionPreamble = "\
    #ifdef GL_ES\n\
    precision mediump float;\n\
    #define TexCoordPrecision mediump\n\
    #else\n\
    #define TexCoordPrecision\n\
    #endif\n";

std::string GetVertexShaderSource(bool flip_y)
{
    std::string source;

    // Preamble for core and compatibility mode.
    if (gfx::GetGLImplementation() == gfx::kGLImplementationDesktopGLCoreProfile) {
        source += std::string("\
        #version 150\n\
        #define ATTRIBUTE in\n\
        #define VARYING out\n");
    } else {
        source += std::string("\
        #define ATTRIBUTE attribute\n\
        #define VARYING varying\n");
    }

    // Preamble for texture precision.
    source += std::string(kShaderPrecisionPreamble);

    // Preamble to differentiate based on |flip_y|.
    if (flip_y) {
        source += std::string("#define SIGN -\n");
    } else {
        source += std::string("#define SIGN\n");
    }

    // Main shader source.
    source += std::string("\
      uniform vec2 u_vertex_translate;\n\
      uniform vec2 u_half_size;\n\
      ATTRIBUTE vec4 a_position;\n\
      VARYING TexCoordPrecision vec2 v_uv;\n\
      void main(void) {\n\
        gl_Position = a_position + vec4(u_vertex_translate, 0.0, 0.0);\n\
        v_uv = a_position.xy * vec2(u_half_size.s, SIGN u_half_size.t) +\n\
               vec2(u_half_size.s, u_half_size.t);\n\
      }\n");

    return source;
}

std::string GetFragmentShaderSource(bool premultiply_alpha,
    bool unpremultiply_alpha,
    GLenum target)
{
    // Only one alpha mode at a time makes sense.
    DCHECK(!premultiply_alpha || !unpremultiply_alpha);

    std::string source;

    // Preamble for core and compatibility mode.
    if (gfx::GetGLImplementation() == gfx::kGLImplementationDesktopGLCoreProfile) {
        source += std::string("\
        #version 150\n\
        out vec4 frag_color;\n\
        #define VARYING in\n\
        #define FRAGCOLOR frag_color\n\
        #define TextureLookup texture\n");
    } else {
        switch (target) {
        case GL_TEXTURE_2D:
            source += std::string("#define TextureLookup texture2D\n");
            break;
        case GL_TEXTURE_RECTANGLE_ARB:
            source += std::string("#define TextureLookup texture2DRect\n");
            break;
        case GL_TEXTURE_EXTERNAL_OES:
            source += std::string(
                "#extension GL_OES_EGL_image_external : require\n");
            source += std::string("#define TextureLookup texture2D\n");
            break;
        default:
            NOTREACHED();
            break;
        }
        source += std::string("\
        #define VARYING varying\n\
        #define FRAGCOLOR gl_FragColor\n");
    }

    // Preamble for sampler type.
    switch (target) {
    case GL_TEXTURE_2D:
        source += std::string("#define SamplerType sampler2D\n");
        break;
    case GL_TEXTURE_RECTANGLE_ARB:
        source += std::string("#define SamplerType sampler2DRect\n");
        break;
    case GL_TEXTURE_EXTERNAL_OES:
        source += std::string("#define SamplerType samplerExternalOES\n");
        break;
    default:
        NOTREACHED();
        break;
    }

    // Preamble for texture precision.
    source += std::string(kShaderPrecisionPreamble);

    // Main shader source.
    source += std::string("\
      uniform SamplerType u_sampler;\n\
      uniform mat4 u_tex_coord_transform;\n\
      VARYING TexCoordPrecision vec2 v_uv;\n\
      void main(void) {\n\
        TexCoordPrecision vec4 uv = u_tex_coord_transform * vec4(v_uv, 0, 1);\n\
        FRAGCOLOR = TextureLookup(u_sampler, uv.st);\n");

    // Post-processing to premultiply or un-premultiply alpha.
    if (premultiply_alpha) {
        source += std::string("        FRAGCOLOR.rgb *= FRAGCOLOR.a;\n");
    }
    if (unpremultiply_alpha) {
        source += std::string("\
        if (FRAGCOLOR.a > 0.0)\n\
          FRAGCOLOR.rgb /= FRAGCOLOR.a;\n");
    }

    // Main function end.
    source += std::string("      }\n");

    return source;
}

void CompileShader(GLuint shader, const char* shader_source)
{
    glShaderSource(shader, 1, &shader_source, 0);
    glCompileShader(shader);
#ifndef NDEBUG
    GLint compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if (GL_TRUE != compile_status)
        DLOG(ERROR) << "CopyTextureCHROMIUM: shader compilation failure.";
#endif
}

void DeleteShader(GLuint shader)
{
    if (shader)
        glDeleteShader(shader);
}

bool BindFramebufferTexture2D(GLenum target,
    GLuint texture_id,
    GLuint framebuffer)
{
    DCHECK(target == GL_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE_ARB);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(target, texture_id);
    // NVidia drivers require texture settings to be a certain way
    // or they won't report FRAMEBUFFER_COMPLETE.
    glTexParameterf(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target,
        texture_id, 0);

#ifndef NDEBUG
    GLenum fb_status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
    if (GL_FRAMEBUFFER_COMPLETE != fb_status) {
        DLOG(ERROR) << "CopyTextureCHROMIUM: Incomplete framebuffer.";
        return false;
    }
#endif
    return true;
}

void DoCopyTexImage2D(const gpu::gles2::GLES2Decoder* decoder,
    GLenum source_target,
    GLuint source_id,
    GLuint dest_id,
    GLenum dest_internal_format,
    GLsizei width,
    GLsizei height,
    GLuint framebuffer)
{
    DCHECK(source_target == GL_TEXTURE_2D || source_target == GL_TEXTURE_RECTANGLE_ARB);
    if (BindFramebufferTexture2D(source_target, source_id, framebuffer)) {
        glBindTexture(GL_TEXTURE_2D, dest_id);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glCopyTexImage2D(GL_TEXTURE_2D, 0 /* level */, dest_internal_format,
            0 /* x */, 0 /* y */, width, height, 0 /* border */);
    }

    decoder->RestoreTextureState(source_id);
    decoder->RestoreTextureState(dest_id);
    decoder->RestoreTextureUnitBindings(0);
    decoder->RestoreActiveTexture();
    decoder->RestoreFramebufferBindings();
}

void DoCopyTexSubImage2D(const gpu::gles2::GLES2Decoder* decoder,
    GLenum source_target,
    GLuint source_id,
    GLuint dest_id,
    GLint xoffset,
    GLint yoffset,
    GLint source_x,
    GLint source_y,
    GLsizei source_width,
    GLsizei source_height,
    GLuint framebuffer)
{
    DCHECK(source_target == GL_TEXTURE_2D || source_target == GL_TEXTURE_RECTANGLE_ARB);
    if (BindFramebufferTexture2D(source_target, source_id, framebuffer)) {
        glBindTexture(GL_TEXTURE_2D, dest_id);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0 /* level */, xoffset, yoffset,
            source_x, source_y, source_width, source_height);
    }

    decoder->RestoreTextureState(source_id);
    decoder->RestoreTextureState(dest_id);
    decoder->RestoreTextureUnitBindings(0);
    decoder->RestoreActiveTexture();
    decoder->RestoreFramebufferBindings();
}

} // namespace

namespace gpu {

CopyTextureCHROMIUMResourceManager::CopyTextureCHROMIUMResourceManager()
    : initialized_(false)
    , vertex_shaders_(NUM_VERTEX_SHADERS, 0u)
    , fragment_shaders_(NUM_FRAGMENT_SHADERS, 0u)
    , buffer_id_(0u)
    , framebuffer_(0u)
{
}

CopyTextureCHROMIUMResourceManager::~CopyTextureCHROMIUMResourceManager()
{
    // |buffer_id_| and |framebuffer_| can be not-null because when GPU context is
    // lost, this class can be deleted without releasing resources like
    // GLES2DecoderImpl.
}

void CopyTextureCHROMIUMResourceManager::Initialize(
    const gles2::GLES2Decoder* decoder)
{
    static_assert(
        kVertexPositionAttrib == 0u,
        "kVertexPositionAttrib must be 0");
    DCHECK(!buffer_id_);
    DCHECK(!framebuffer_);
    DCHECK(programs_.empty());

    // Initialize all of the GPU resources required to perform the copy.
    glGenBuffersARB(1, &buffer_id_);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id_);
    const GLfloat kQuadVertices[] = { -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f };
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);

    glGenFramebuffersEXT(1, &framebuffer_);

    decoder->RestoreBufferBindings();

    initialized_ = true;
}

void CopyTextureCHROMIUMResourceManager::Destroy()
{
    if (!initialized_)
        return;

    glDeleteFramebuffersEXT(1, &framebuffer_);
    framebuffer_ = 0;

    std::for_each(vertex_shaders_.begin(), vertex_shaders_.end(), DeleteShader);
    std::for_each(
        fragment_shaders_.begin(), fragment_shaders_.end(), DeleteShader);

    for (ProgramMap::const_iterator it = programs_.begin(); it != programs_.end();
         ++it) {
        const ProgramInfo& info = it->second;
        glDeleteProgram(info.program);
    }

    glDeleteBuffersARB(1, &buffer_id_);
    buffer_id_ = 0;
}

void CopyTextureCHROMIUMResourceManager::DoCopyTexture(
    const gles2::GLES2Decoder* decoder,
    GLenum source_target,
    GLuint source_id,
    GLenum source_internal_format,
    GLuint dest_id,
    GLenum dest_internal_format,
    GLsizei width,
    GLsizei height,
    bool flip_y,
    bool premultiply_alpha,
    bool unpremultiply_alpha)
{
    bool premultiply_alpha_change = premultiply_alpha ^ unpremultiply_alpha;
    // GL_INVALID_OPERATION is generated if the currently bound framebuffer's
    // format does not contain a superset of the components required by the base
    // format of internalformat.
    // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glCopyTexImage2D.xml
    bool source_format_contain_superset_of_dest_format = (source_internal_format == dest_internal_format && source_internal_format != GL_BGRA_EXT) || (source_internal_format == GL_RGBA && dest_internal_format == GL_RGB);
    // GL_TEXTURE_RECTANGLE_ARB on FBO is supported by OpenGL, not GLES2,
    // so restrict this to GL_TEXTURE_2D.
    if (source_target == GL_TEXTURE_2D && !flip_y && !premultiply_alpha_change && source_format_contain_superset_of_dest_format) {
        DoCopyTexImage2D(decoder,
            source_target,
            source_id,
            dest_id,
            dest_internal_format,
            width,
            height,
            framebuffer_);
        return;
    }

    // Use kIdentityMatrix if no transform passed in.
    DoCopyTextureWithTransform(decoder, source_target, source_id, dest_id, width,
        height, flip_y, premultiply_alpha,
        unpremultiply_alpha, kIdentityMatrix);
}

void CopyTextureCHROMIUMResourceManager::DoCopySubTexture(
    const gles2::GLES2Decoder* decoder,
    GLenum source_target,
    GLuint source_id,
    GLenum source_internal_format,
    GLuint dest_id,
    GLenum dest_internal_format,
    GLint xoffset,
    GLint yoffset,
    GLint x,
    GLint y,
    GLsizei width,
    GLsizei height,
    GLsizei dest_width,
    GLsizei dest_height,
    GLsizei source_width,
    GLsizei source_height,
    bool flip_y,
    bool premultiply_alpha,
    bool unpremultiply_alpha)
{
    bool premultiply_alpha_change = premultiply_alpha ^ unpremultiply_alpha;
    // GL_INVALID_OPERATION is generated if the currently bound framebuffer's
    // format does not contain a superset of the components required by the base
    // format of internalformat.
    // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glCopyTexImage2D.xml
    bool source_format_contain_superset_of_dest_format = (source_internal_format == dest_internal_format && source_internal_format != GL_BGRA_EXT) || (source_internal_format == GL_RGBA && dest_internal_format == GL_RGB);
    // GL_TEXTURE_RECTANGLE_ARB on FBO is supported by OpenGL, not GLES2,
    // so restrict this to GL_TEXTURE_2D.
    if (source_target == GL_TEXTURE_2D && !flip_y && !premultiply_alpha_change && source_format_contain_superset_of_dest_format) {
        DoCopyTexSubImage2D(decoder, source_target, source_id, dest_id, xoffset,
            yoffset, x, y, width, height, framebuffer_);
        return;
    }

    DoCopyTextureInternal(decoder, source_target, source_id, dest_id, xoffset,
        yoffset, x, y, width, height, dest_width, dest_height,
        source_width, source_height, flip_y, premultiply_alpha,
        unpremultiply_alpha, kIdentityMatrix);
}

void CopyTextureCHROMIUMResourceManager::DoCopyTextureWithTransform(
    const gles2::GLES2Decoder* decoder,
    GLenum source_target,
    GLuint source_id,
    GLuint dest_id,
    GLsizei width,
    GLsizei height,
    bool flip_y,
    bool premultiply_alpha,
    bool unpremultiply_alpha,
    const GLfloat transform_matrix[16])
{
    GLsizei dest_width = width;
    GLsizei dest_height = height;
    DoCopyTextureInternal(decoder, source_target, source_id, dest_id, 0, 0, 0, 0,
        width, height, dest_width, dest_height, width, height,
        flip_y, premultiply_alpha, unpremultiply_alpha,
        transform_matrix);
}

void CopyTextureCHROMIUMResourceManager::DoCopyTextureInternal(
    const gles2::GLES2Decoder* decoder,
    GLenum source_target,
    GLuint source_id,
    GLuint dest_id,
    GLint xoffset,
    GLint yoffset,
    GLint x,
    GLint y,
    GLsizei width,
    GLsizei height,
    GLsizei dest_width,
    GLsizei dest_height,
    GLsizei source_width,
    GLsizei source_height,
    bool flip_y,
    bool premultiply_alpha,
    bool unpremultiply_alpha,
    const GLfloat transform_matrix[16])
{
    DCHECK(source_target == GL_TEXTURE_2D || source_target == GL_TEXTURE_RECTANGLE_ARB || source_target == GL_TEXTURE_EXTERNAL_OES);
    DCHECK_GE(xoffset, 0);
    DCHECK_LE(xoffset + width, dest_width);
    DCHECK_GE(yoffset, 0);
    DCHECK_LE(yoffset + height, dest_height);
    if (!initialized_) {
        DLOG(ERROR) << "CopyTextureCHROMIUM: Uninitialized manager.";
        return;
    }

    VertexShaderId vertex_shader_id = GetVertexShaderId(flip_y);
    DCHECK_LT(static_cast<size_t>(vertex_shader_id), vertex_shaders_.size());
    FragmentShaderId fragment_shader_id = GetFragmentShaderId(
        premultiply_alpha, unpremultiply_alpha, source_target);
    DCHECK_LT(static_cast<size_t>(fragment_shader_id), fragment_shaders_.size());

    ProgramMapKey key(vertex_shader_id, fragment_shader_id);
    ProgramInfo* info = &programs_[key];
    // Create program if necessary.
    if (!info->program) {
        info->program = glCreateProgram();
        GLuint* vertex_shader = &vertex_shaders_[vertex_shader_id];
        if (!*vertex_shader) {
            *vertex_shader = glCreateShader(GL_VERTEX_SHADER);
            std::string source = GetVertexShaderSource(flip_y);
            CompileShader(*vertex_shader, source.c_str());
        }
        glAttachShader(info->program, *vertex_shader);
        GLuint* fragment_shader = &fragment_shaders_[fragment_shader_id];
        if (!*fragment_shader) {
            *fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
            std::string source = GetFragmentShaderSource(
                premultiply_alpha, unpremultiply_alpha, source_target);
            CompileShader(*fragment_shader, source.c_str());
        }
        glAttachShader(info->program, *fragment_shader);
        glBindAttribLocation(info->program, kVertexPositionAttrib, "a_position");
        glLinkProgram(info->program);
#ifndef NDEBUG
        GLint linked;
        glGetProgramiv(info->program, GL_LINK_STATUS, &linked);
        if (!linked)
            DLOG(ERROR) << "CopyTextureCHROMIUM: program link failure.";
#endif
        info->vertex_translate_handle = glGetUniformLocation(info->program,
            "u_vertex_translate");
        info->tex_coord_transform_handle = glGetUniformLocation(info->program, "u_tex_coord_transform");
        info->half_size_handle = glGetUniformLocation(info->program, "u_half_size");
        info->sampler_handle = glGetUniformLocation(info->program, "u_sampler");
    }
    glUseProgram(info->program);

    glUniformMatrix4fv(info->tex_coord_transform_handle, 1, GL_FALSE,
        transform_matrix);

    GLint x_translate = xoffset - x;
    GLint y_translate = yoffset - y;
    if (!x_translate && !y_translate) {
        glUniform2f(info->vertex_translate_handle, 0.0f, 0.0f);
    } else {
        // transform offsets from ([0, dest_width], [0, dest_height]) coord.
        // to ([-1, 1], [-1, 1]) coord.
        GLfloat x_translate_on_vertex = ((2.f * x_translate) / dest_width);
        GLfloat y_translate_on_vertex = ((2.f * y_translate) / dest_height);

        // Pass translation to the shader program.
        glUniform2f(info->vertex_translate_handle, x_translate_on_vertex,
            y_translate_on_vertex);
    }
    if (source_target == GL_TEXTURE_RECTANGLE_ARB)
        glUniform2f(info->half_size_handle, source_width / 2.0f,
            source_height / 2.0f);
    else
        glUniform2f(info->half_size_handle, 0.5f, 0.5f);

    if (BindFramebufferTexture2D(GL_TEXTURE_2D, dest_id, framebuffer_)) {
#ifndef NDEBUG
        // glValidateProgram of MACOSX validates FBO unlike other platforms, so
        // glValidateProgram must be called after FBO binding. crbug.com/463439
        glValidateProgram(info->program);
        GLint validation_status;
        glGetProgramiv(info->program, GL_VALIDATE_STATUS, &validation_status);
        if (GL_TRUE != validation_status) {
            DLOG(ERROR) << "CopyTextureCHROMIUM: Invalid shader.";
            return;
        }
#endif

        if (gfx::GetGLImplementation() != gfx::kGLImplementationDesktopGLCoreProfile) {
            decoder->ClearAllAttributes();
            glEnableVertexAttribArray(kVertexPositionAttrib);
        }

        glBindBuffer(GL_ARRAY_BUFFER, buffer_id_);
        glVertexAttribPointer(kVertexPositionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glUniform1i(info->sampler_handle, 0);

        glBindTexture(source_target, source_id);
        glTexParameterf(source_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(source_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(source_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(source_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_CULL_FACE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_FALSE);
        glDisable(GL_BLEND);

        bool need_scissor = xoffset || yoffset || width != dest_width || height != dest_height;
        if (need_scissor) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(xoffset, yoffset, width, height);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }
        glViewport(0, 0, dest_width, dest_height);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    decoder->RestoreAllAttributes();
    decoder->RestoreTextureState(source_id);
    decoder->RestoreTextureState(dest_id);
    decoder->RestoreTextureUnitBindings(0);
    decoder->RestoreActiveTexture();
    decoder->RestoreProgramBindings();
    decoder->RestoreBufferBindings();
    decoder->RestoreFramebufferBindings();
    decoder->RestoreGlobalState();
}

} // namespace gpu
