// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/shader.h"

#include <algorithm>

#include "base/basictypes.h"
#include "base/logging.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/size.h"

template <size_t size>
std::string StripLambda(const char (&shader)[size])
{
    // Must contain at least "[]() {}" and trailing null (included in size).
    static_assert(size >= 8,
        "String passed to StripLambda must be at least 8 characters");
    DCHECK_EQ(strncmp("[]() {", shader, 6), 0);
    DCHECK_EQ(shader[size - 2], '}');
    return std::string(shader + 6, shader + size - 2);
}

// Shaders are passed in with lambda syntax, which tricks clang-format into
// handling them correctly. StipLambda removes this.
#define SHADER0(Src) StripLambda(#Src)
#define VERTEX_SHADER(Head, Body) SetVertexTexCoordPrecision(Head + Body)
#define FRAGMENT_SHADER(Head, Body) \
    SetFragmentTexCoordPrecision(   \
        precision,                  \
        SetFragmentSamplerType(sampler, SetBlendModeFunctions(Head + Body)))

using gpu::gles2::GLES2Interface;

namespace cc {

namespace {

    static void GetProgramUniformLocations(GLES2Interface* context,
        unsigned program,
        size_t count,
        const char** uniforms,
        int* locations,
        int* base_uniform_index)
    {
        for (size_t i = 0; i < count; i++) {
            locations[i] = (*base_uniform_index)++;
            context->BindUniformLocationCHROMIUM(program, locations[i], uniforms[i]);
        }
    }

    static std::string SetFragmentTexCoordPrecision(
        TexCoordPrecision requested_precision,
        std::string shader_string)
    {
        switch (requested_precision) {
        case TEX_COORD_PRECISION_HIGH:
            DCHECK_NE(shader_string.find("TexCoordPrecision"), std::string::npos);
            return "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
                   "  #define TexCoordPrecision highp\n"
                   "#else\n"
                   "  #define TexCoordPrecision mediump\n"
                   "#endif\n"
                + shader_string;
        case TEX_COORD_PRECISION_MEDIUM:
            DCHECK_NE(shader_string.find("TexCoordPrecision"), std::string::npos);
            return "#define TexCoordPrecision mediump\n" + shader_string;
        case TEX_COORD_PRECISION_NA:
            DCHECK_EQ(shader_string.find("TexCoordPrecision"), std::string::npos);
            DCHECK_EQ(shader_string.find("texture2D"), std::string::npos);
            DCHECK_EQ(shader_string.find("texture2DRect"), std::string::npos);
            return shader_string;
        default:
            NOTREACHED();
            break;
        }
        return shader_string;
    }

    static std::string SetVertexTexCoordPrecision(
        const std::string& shader_string)
    {
        // We unconditionally use highp in the vertex shader since
        // we are unlikely to be vertex shader bound when drawing large quads.
        // Also, some vertex shaders mutate the texture coordinate in such a
        // way that the effective precision might be lower than expected.
        return "#define TexCoordPrecision highp\n" + shader_string;
    }

    TexCoordPrecision TexCoordPrecisionRequired(GLES2Interface* context,
        int* highp_threshold_cache,
        int highp_threshold_min,
        int x,
        int y)
    {
        if (*highp_threshold_cache == 0) {
            // Initialize range and precision with minimum spec values for when
            // GetShaderPrecisionFormat is a test stub.
            // TODO(brianderson): Implement better stubs of GetShaderPrecisionFormat
            // everywhere.
            GLint range[2] = { 14, 14 };
            GLint precision = 10;
            context->GetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_MEDIUM_FLOAT,
                range, &precision);
            *highp_threshold_cache = 1 << precision;
        }

        int highp_threshold = std::max(*highp_threshold_cache, highp_threshold_min);
        if (x > highp_threshold || y > highp_threshold)
            return TEX_COORD_PRECISION_HIGH;
        return TEX_COORD_PRECISION_MEDIUM;
    }

    static std::string SetFragmentSamplerType(SamplerType requested_type,
        std::string shader_string)
    {
        switch (requested_type) {
        case SAMPLER_TYPE_2D:
            DCHECK_NE(shader_string.find("SamplerType"), std::string::npos);
            DCHECK_NE(shader_string.find("TextureLookup"), std::string::npos);
            return "#define SamplerType sampler2D\n"
                   "#define TextureLookup texture2D\n"
                + shader_string;
        case SAMPLER_TYPE_2D_RECT:
            DCHECK_NE(shader_string.find("SamplerType"), std::string::npos);
            DCHECK_NE(shader_string.find("TextureLookup"), std::string::npos);
            return "#extension GL_ARB_texture_rectangle : require\n"
                   "#define SamplerType sampler2DRect\n"
                   "#define TextureLookup texture2DRect\n"
                + shader_string;
        case SAMPLER_TYPE_EXTERNAL_OES:
            DCHECK_NE(shader_string.find("SamplerType"), std::string::npos);
            DCHECK_NE(shader_string.find("TextureLookup"), std::string::npos);
            return "#extension GL_OES_EGL_image_external : require\n"
                   "#define SamplerType samplerExternalOES\n"
                   "#define TextureLookup texture2D\n"
                + shader_string;
        case SAMPLER_TYPE_NA:
            DCHECK_EQ(shader_string.find("SamplerType"), std::string::npos);
            DCHECK_EQ(shader_string.find("TextureLookup"), std::string::npos);
            return shader_string;
        default:
            NOTREACHED();
            break;
        }
        return shader_string;
    }

} // namespace

ShaderLocations::ShaderLocations()
{
}

TexCoordPrecision TexCoordPrecisionRequired(GLES2Interface* context,
    int* highp_threshold_cache,
    int highp_threshold_min,
    const gfx::Point& max_coordinate)
{
    return TexCoordPrecisionRequired(context,
        highp_threshold_cache,
        highp_threshold_min,
        max_coordinate.x(),
        max_coordinate.y());
}

TexCoordPrecision TexCoordPrecisionRequired(GLES2Interface* context,
    int* highp_threshold_cache,
    int highp_threshold_min,
    const gfx::Size& max_size)
{
    return TexCoordPrecisionRequired(context,
        highp_threshold_cache,
        highp_threshold_min,
        max_size.width(),
        max_size.height());
}

VertexShaderPosTex::VertexShaderPosTex()
    : matrix_location_(-1)
{
}

void VertexShaderPosTex::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "matrix",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    matrix_location_ = locations[0];
}

std::string VertexShaderPosTex::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderPosTex::GetShaderHead()
{
    return SHADER0([]() {
        attribute vec4 a_position;
        attribute TexCoordPrecision vec2 a_texCoord;
        uniform mat4 matrix;
        varying TexCoordPrecision vec2 v_texCoord;
    });
}

std::string VertexShaderPosTex::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            gl_Position = matrix * a_position;
            v_texCoord = a_texCoord;
        }
    });
}

VertexShaderPosTexYUVStretchOffset::VertexShaderPosTexYUVStretchOffset()
    : matrix_location_(-1)
    , ya_tex_scale_location_(-1)
    , ya_tex_offset_location_(-1)
    , uv_tex_scale_location_(-1)
    , uv_tex_offset_location_(-1)
{
}

void VertexShaderPosTexYUVStretchOffset::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "matrix",
        "yaTexScale",
        "yaTexOffset",
        "uvTexScale",
        "uvTexOffset",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    matrix_location_ = locations[0];
    ya_tex_scale_location_ = locations[1];
    ya_tex_offset_location_ = locations[2];
    uv_tex_scale_location_ = locations[3];
    uv_tex_offset_location_ = locations[4];
}

std::string VertexShaderPosTexYUVStretchOffset::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderPosTexYUVStretchOffset::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        attribute vec4 a_position;
        attribute TexCoordPrecision vec2 a_texCoord;
        uniform mat4 matrix;
        varying TexCoordPrecision vec2 v_yaTexCoord;
        varying TexCoordPrecision vec2 v_uvTexCoord;
        uniform TexCoordPrecision vec2 yaTexScale;
        uniform TexCoordPrecision vec2 yaTexOffset;
        uniform TexCoordPrecision vec2 uvTexScale;
        uniform TexCoordPrecision vec2 uvTexOffset;
    });
}

std::string VertexShaderPosTexYUVStretchOffset::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            gl_Position = matrix * a_position;
            v_yaTexCoord = a_texCoord * yaTexScale + yaTexOffset;
            v_uvTexCoord = a_texCoord * uvTexScale + uvTexOffset;
        }
    });
}

VertexShaderPos::VertexShaderPos()
    : matrix_location_(-1)
{
}

void VertexShaderPos::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "matrix",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    matrix_location_ = locations[0];
}

std::string VertexShaderPos::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderPos::GetShaderHead()
{
    return SHADER0([]() {
        attribute vec4 a_position;
        uniform mat4 matrix;
    });
}

std::string VertexShaderPos::GetShaderBody()
{
    return SHADER0([]() {
        void main() { gl_Position = matrix * a_position; }
    });
}

VertexShaderPosTexTransform::VertexShaderPosTexTransform()
    : matrix_location_(-1)
    , tex_transform_location_(-1)
    , vertex_opacity_location_(-1)
{
}

void VertexShaderPosTexTransform::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "matrix",
        "texTransform",
        "opacity",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    matrix_location_ = locations[0];
    tex_transform_location_ = locations[1];
    vertex_opacity_location_ = locations[2];
}

std::string VertexShaderPosTexTransform::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderPosTexTransform::GetShaderHead()
{
    return SHADER0([]() {
        attribute vec4 a_position;
        attribute TexCoordPrecision vec2 a_texCoord;
        attribute float a_index;
        uniform mat4 matrix[8];
        uniform TexCoordPrecision vec4 texTransform[8];
        uniform float opacity[32];
        varying TexCoordPrecision vec2 v_texCoord;
        varying float v_alpha;
    });
}

std::string VertexShaderPosTexTransform::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            int quad_index = int(a_index * 0.25); // NOLINT
            gl_Position = matrix[quad_index] * a_position;
            TexCoordPrecision vec4 texTrans = texTransform[quad_index];
            v_texCoord = a_texCoord * texTrans.zw + texTrans.xy;
            v_alpha = opacity[int(a_index)]; // NOLINT
        }
    });
}

void VertexShaderPosTexTransform::FillLocations(
    ShaderLocations* locations) const
{
    locations->matrix = matrix_location();
    locations->tex_transform = tex_transform_location();
}

std::string VertexShaderPosTexIdentity::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderPosTexIdentity::GetShaderHead()
{
    return SHADER0([]() {
        attribute vec4 a_position;
        varying TexCoordPrecision vec2 v_texCoord;
    });
}

std::string VertexShaderPosTexIdentity::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            gl_Position = a_position;
            v_texCoord = (a_position.xy + vec2(1.0)) * 0.5;
        }
    });
}

VertexShaderQuad::VertexShaderQuad()
    : matrix_location_(-1)
    , quad_location_(-1)
{
}

void VertexShaderQuad::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "matrix",
        "quad",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    matrix_location_ = locations[0];
    quad_location_ = locations[1];
}

std::string VertexShaderQuad::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderQuad::GetShaderHead()
{
#if defined(OS_ANDROID)
    // TODO(epenner): Find the cause of this 'quad' uniform
    // being missing if we don't add dummy variables.
    // http://crbug.com/240602
    return SHADER0([]() {
        attribute TexCoordPrecision vec4 a_position;
        attribute float a_index;
        uniform mat4 matrix;
        uniform TexCoordPrecision vec2 quad[4];
        uniform TexCoordPrecision vec2 dummy_uniform;
        varying TexCoordPrecision vec2 dummy_varying;
    });
#else
    return SHADER0([]() {
        attribute TexCoordPrecision vec4 a_position;
        attribute float a_index;
        uniform mat4 matrix;
        uniform TexCoordPrecision vec2 quad[4];
    });
#endif
}

std::string VertexShaderQuad::GetShaderBody()
{
#if defined(OS_ANDROID)
    return SHADER0([]() {
        void main()
        {
            vec2 pos = quad[int(a_index)]; // NOLINT
            gl_Position = matrix * vec4(pos, a_position.z, a_position.w);
            dummy_varying = dummy_uniform;
        }
    });
#else
    return SHADER0([]() {
        void main()
        {
            vec2 pos = quad[int(a_index)]; // NOLINT
            gl_Position = matrix * vec4(pos, a_position.z, a_position.w);
        }
    });
#endif
}

VertexShaderQuadAA::VertexShaderQuadAA()
    : matrix_location_(-1)
    , viewport_location_(-1)
    , quad_location_(-1)
    , edge_location_(-1)
{
}

void VertexShaderQuadAA::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "matrix",
        "viewport",
        "quad",
        "edge",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    matrix_location_ = locations[0];
    viewport_location_ = locations[1];
    quad_location_ = locations[2];
    edge_location_ = locations[3];
}

std::string VertexShaderQuadAA::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderQuadAA::GetShaderHead()
{
    return SHADER0([]() {
        attribute TexCoordPrecision vec4 a_position;
        attribute float a_index;
        uniform mat4 matrix;
        uniform vec4 viewport;
        uniform TexCoordPrecision vec2 quad[4];
        uniform TexCoordPrecision vec3 edge[8];
        varying TexCoordPrecision vec4 edge_dist[2]; // 8 edge distances.
    });
}

std::string VertexShaderQuadAA::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec2 pos = quad[int(a_index)]; // NOLINT
            gl_Position = matrix * vec4(pos, a_position.z, a_position.w);
            vec2 ndc_pos = 0.5 * (1.0 + gl_Position.xy / gl_Position.w);
            vec3 screen_pos = vec3(viewport.xy + viewport.zw * ndc_pos, 1.0);
            edge_dist[0] = vec4(dot(edge[0], screen_pos), dot(edge[1], screen_pos),
                               dot(edge[2], screen_pos), dot(edge[3], screen_pos))
                * gl_Position.w;
            edge_dist[1] = vec4(dot(edge[4], screen_pos), dot(edge[5], screen_pos),
                               dot(edge[6], screen_pos), dot(edge[7], screen_pos))
                * gl_Position.w;
        }
    });
}

VertexShaderQuadTexTransformAA::VertexShaderQuadTexTransformAA()
    : matrix_location_(-1)
    , viewport_location_(-1)
    , quad_location_(-1)
    , edge_location_(-1)
    , tex_transform_location_(-1)
{
}

void VertexShaderQuadTexTransformAA::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "matrix",
        "viewport",
        "quad",
        "edge",
        "texTrans",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    matrix_location_ = locations[0];
    viewport_location_ = locations[1];
    quad_location_ = locations[2];
    edge_location_ = locations[3];
    tex_transform_location_ = locations[4];
}

std::string VertexShaderQuadTexTransformAA::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderQuadTexTransformAA::GetShaderHead()
{
    return SHADER0([]() {
        attribute TexCoordPrecision vec4 a_position;
        attribute float a_index;
        uniform mat4 matrix;
        uniform vec4 viewport;
        uniform TexCoordPrecision vec2 quad[4];
        uniform TexCoordPrecision vec3 edge[8];
        uniform TexCoordPrecision vec4 texTrans;
        varying TexCoordPrecision vec2 v_texCoord;
        varying TexCoordPrecision vec4 edge_dist[2]; // 8 edge distances.
    });
}

std::string VertexShaderQuadTexTransformAA::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec2 pos = quad[int(a_index)]; // NOLINT
            gl_Position = matrix * vec4(pos, a_position.z, a_position.w);
            vec2 ndc_pos = 0.5 * (1.0 + gl_Position.xy / gl_Position.w);
            vec3 screen_pos = vec3(viewport.xy + viewport.zw * ndc_pos, 1.0);
            edge_dist[0] = vec4(dot(edge[0], screen_pos), dot(edge[1], screen_pos),
                               dot(edge[2], screen_pos), dot(edge[3], screen_pos))
                * gl_Position.w;
            edge_dist[1] = vec4(dot(edge[4], screen_pos), dot(edge[5], screen_pos),
                               dot(edge[6], screen_pos), dot(edge[7], screen_pos))
                * gl_Position.w;
            v_texCoord = (pos.xy + vec2(0.5)) * texTrans.zw + texTrans.xy;
        }
    });
}

void VertexShaderQuadTexTransformAA::FillLocations(
    ShaderLocations* locations) const
{
    locations->quad = quad_location();
    locations->edge = edge_location();
    locations->viewport = viewport_location();
    locations->matrix = matrix_location();
    locations->tex_transform = tex_transform_location();
}

VertexShaderTile::VertexShaderTile()
    : matrix_location_(-1)
    , quad_location_(-1)
    , vertex_tex_transform_location_(-1)
{
}

void VertexShaderTile::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "matrix",
        "quad",
        "vertexTexTransform",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    matrix_location_ = locations[0];
    quad_location_ = locations[1];
    vertex_tex_transform_location_ = locations[2];
}

std::string VertexShaderTile::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderTile::GetShaderHead()
{
    return SHADER0([]() {
        attribute TexCoordPrecision vec4 a_position;
        attribute TexCoordPrecision vec2 a_texCoord;
        attribute float a_index;
        uniform mat4 matrix;
        uniform TexCoordPrecision vec2 quad[4];
        uniform TexCoordPrecision vec4 vertexTexTransform;
        varying TexCoordPrecision vec2 v_texCoord;
    });
}

std::string VertexShaderTile::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec2 pos = quad[int(a_index)]; // NOLINT
            gl_Position = matrix * vec4(pos, a_position.z, a_position.w);
            v_texCoord = a_texCoord * vertexTexTransform.zw + vertexTexTransform.xy;
        }
    });
}

VertexShaderTileAA::VertexShaderTileAA()
    : matrix_location_(-1)
    , viewport_location_(-1)
    , quad_location_(-1)
    , edge_location_(-1)
    , vertex_tex_transform_location_(-1)
{
}

void VertexShaderTileAA::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "matrix",
        "viewport",
        "quad",
        "edge",
        "vertexTexTransform",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    matrix_location_ = locations[0];
    viewport_location_ = locations[1];
    quad_location_ = locations[2];
    edge_location_ = locations[3];
    vertex_tex_transform_location_ = locations[4];
}

std::string VertexShaderTileAA::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderTileAA::GetShaderHead()
{
    return SHADER0([]() {
        attribute TexCoordPrecision vec4 a_position;
        attribute float a_index;
        uniform mat4 matrix;
        uniform vec4 viewport;
        uniform TexCoordPrecision vec2 quad[4];
        uniform TexCoordPrecision vec3 edge[8];
        uniform TexCoordPrecision vec4 vertexTexTransform;
        varying TexCoordPrecision vec2 v_texCoord;
        varying TexCoordPrecision vec4 edge_dist[2]; // 8 edge distances.
    });
}

std::string VertexShaderTileAA::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec2 pos = quad[int(a_index)]; // NOLINT
            gl_Position = matrix * vec4(pos, a_position.z, a_position.w);
            vec2 ndc_pos = 0.5 * (1.0 + gl_Position.xy / gl_Position.w);
            vec3 screen_pos = vec3(viewport.xy + viewport.zw * ndc_pos, 1.0);
            edge_dist[0] = vec4(dot(edge[0], screen_pos), dot(edge[1], screen_pos),
                               dot(edge[2], screen_pos), dot(edge[3], screen_pos))
                * gl_Position.w;
            edge_dist[1] = vec4(dot(edge[4], screen_pos), dot(edge[5], screen_pos),
                               dot(edge[6], screen_pos), dot(edge[7], screen_pos))
                * gl_Position.w;
            v_texCoord = pos.xy * vertexTexTransform.zw + vertexTexTransform.xy;
        }
    });
}

VertexShaderVideoTransform::VertexShaderVideoTransform()
    : matrix_location_(-1)
    , tex_matrix_location_(-1)
{
}

void VertexShaderVideoTransform::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "matrix",
        "texMatrix",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    matrix_location_ = locations[0];
    tex_matrix_location_ = locations[1];
}

std::string VertexShaderVideoTransform::GetShaderString() const
{
    return VERTEX_SHADER(GetShaderHead(), GetShaderBody());
}

std::string VertexShaderVideoTransform::GetShaderHead()
{
    return SHADER0([]() {
        attribute vec4 a_position;
        attribute TexCoordPrecision vec2 a_texCoord;
        uniform mat4 matrix;
        uniform TexCoordPrecision mat4 texMatrix;
        varying TexCoordPrecision vec2 v_texCoord;
    });
}

std::string VertexShaderVideoTransform::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            gl_Position = matrix * a_position;
            v_texCoord = vec2(texMatrix * vec4(a_texCoord.x, 1.0 - a_texCoord.y, 0.0, 1.0));
        }
    });
}

#define BLEND_MODE_UNIFORMS "s_backdropTexture",         \
                            "s_originalBackdropTexture", \
                            "backdropRect"
#define UNUSED_BLEND_MODE_UNIFORMS (!has_blend_mode() ? 3 : 0)
#define BLEND_MODE_SET_LOCATIONS(X, POS)                       \
    if (has_blend_mode()) {                                    \
        DCHECK_LT(static_cast<size_t>(POS) + 2, arraysize(X)); \
        backdrop_location_ = locations[POS];                   \
        original_backdrop_location_ = locations[POS + 1];      \
        backdrop_rect_location_ = locations[POS + 2];          \
    }

FragmentTexBlendMode::FragmentTexBlendMode()
    : backdrop_location_(-1)
    , original_backdrop_location_(-1)
    , backdrop_rect_location_(-1)
    , blend_mode_(BLEND_MODE_NONE)
    , mask_for_background_(false)
{
}

std::string FragmentTexBlendMode::SetBlendModeFunctions(
    const std::string& shader_string) const
{
    if (shader_string.find("ApplyBlendMode") == std::string::npos)
        return shader_string;

    if (!has_blend_mode()) {
        return "#define ApplyBlendMode(X, Y) (X)\n" + shader_string;
    }

    static const std::string kUniforms = SHADER0([]() {
        uniform sampler2D s_backdropTexture;
        uniform sampler2D s_originalBackdropTexture;
        uniform TexCoordPrecision vec4 backdropRect;
    });

    std::string mixFunction;
    if (mask_for_background()) {
        mixFunction = SHADER0([]() {
            vec4 MixBackdrop(TexCoordPrecision vec2 bgTexCoord, float mask)
            {
                vec4 backdrop = texture2D(s_backdropTexture, bgTexCoord);
                vec4 original_backdrop = texture2D(s_originalBackdropTexture, bgTexCoord);
                return mix(original_backdrop, backdrop, mask);
            }
        });
    } else {
        mixFunction = SHADER0([]() {
            vec4 MixBackdrop(TexCoordPrecision vec2 bgTexCoord, float mask)
            {
                return texture2D(s_backdropTexture, bgTexCoord);
            }
        });
    }

    static const std::string kFunctionApplyBlendMode = SHADER0([]() {
        vec4 GetBackdropColor(float mask)
        {
            TexCoordPrecision vec2 bgTexCoord = gl_FragCoord.xy - backdropRect.xy;
            bgTexCoord.x /= backdropRect.z;
            bgTexCoord.y /= backdropRect.w;
            return MixBackdrop(bgTexCoord, mask);
        }

        vec4 ApplyBlendMode(vec4 src, float mask)
        {
            vec4 dst = GetBackdropColor(mask);
            return Blend(src, dst);
        }
    });

    return "precision mediump float;" + GetHelperFunctions() + GetBlendFunction() + kUniforms + mixFunction + kFunctionApplyBlendMode + shader_string;
}

std::string FragmentTexBlendMode::GetHelperFunctions() const
{
    static const std::string kFunctionHardLight = SHADER0([]() {
        vec3 hardLight(vec4 src, vec4 dst)
        {
            vec3 result;
            result.r = (2.0 * src.r <= src.a)
                ? (2.0 * src.r * dst.r)
                : (src.a * dst.a - 2.0 * (dst.a - dst.r) * (src.a - src.r));
            result.g = (2.0 * src.g <= src.a)
                ? (2.0 * src.g * dst.g)
                : (src.a * dst.a - 2.0 * (dst.a - dst.g) * (src.a - src.g));
            result.b = (2.0 * src.b <= src.a)
                ? (2.0 * src.b * dst.b)
                : (src.a * dst.a - 2.0 * (dst.a - dst.b) * (src.a - src.b));
            result.rgb += src.rgb * (1.0 - dst.a) + dst.rgb * (1.0 - src.a);
            return result;
        }
    });

    static const std::string kFunctionColorDodgeComponent = SHADER0([]() {
        float getColorDodgeComponent(float srcc, float srca, float dstc,
            float dsta)
        {
            if (0.0 == dstc)
                return srcc * (1.0 - dsta);
            float d = srca - srcc;
            if (0.0 == d)
                return srca * dsta + srcc * (1.0 - dsta) + dstc * (1.0 - srca);
            d = min(dsta, dstc * srca / d);
            return d * srca + srcc * (1.0 - dsta) + dstc * (1.0 - srca);
        }
    });

    static const std::string kFunctionColorBurnComponent = SHADER0([]() {
        float getColorBurnComponent(float srcc, float srca, float dstc,
            float dsta)
        {
            if (dsta == dstc)
                return srca * dsta + srcc * (1.0 - dsta) + dstc * (1.0 - srca);
            if (0.0 == srcc)
                return dstc * (1.0 - srca);
            float d = max(0.0, dsta - (dsta - dstc) * srca / srcc);
            return srca * d + srcc * (1.0 - dsta) + dstc * (1.0 - srca);
        }
    });

    static const std::string kFunctionSoftLightComponentPosDstAlpha = SHADER0([]() {
        float getSoftLightComponent(float srcc, float srca, float dstc,
            float dsta)
        {
            if (2.0 * srcc <= srca) {
                return (dstc * dstc * (srca - 2.0 * srcc)) / dsta + (1.0 - dsta) * srcc + dstc * (-srca + 2.0 * srcc + 1.0);
            } else if (4.0 * dstc <= dsta) {
                float DSqd = dstc * dstc;
                float DCub = DSqd * dstc;
                float DaSqd = dsta * dsta;
                float DaCub = DaSqd * dsta;
                return (-DaCub * srcc + DaSqd * (srcc - dstc * (3.0 * srca - 6.0 * srcc - 1.0)) + 12.0 * dsta * DSqd * (srca - 2.0 * srcc) - 16.0 * DCub * (srca - 2.0 * srcc)) / DaSqd;
            } else {
                return -sqrt(dsta * dstc) * (srca - 2.0 * srcc) - dsta * srcc + dstc * (srca - 2.0 * srcc + 1.0) + srcc;
            }
        }
    });

    static const std::string kFunctionLum = SHADER0([]() {
        float luminance(vec3 color) { return dot(vec3(0.3, 0.59, 0.11), color); }

        vec3 set_luminance(vec3 hueSat, float alpha, vec3 lumColor)
        {
            float diff = luminance(lumColor - hueSat);
            vec3 outColor = hueSat + diff;
            float outLum = luminance(outColor);
            float minComp = min(min(outColor.r, outColor.g), outColor.b);
            float maxComp = max(max(outColor.r, outColor.g), outColor.b);
            if (minComp < 0.0 && outLum != minComp) {
                outColor = outLum + ((outColor - vec3(outLum, outLum, outLum)) * outLum) / (outLum - minComp);
            }
            if (maxComp > alpha && maxComp != outLum) {
                outColor = outLum + ((outColor - vec3(outLum, outLum, outLum)) * (alpha - outLum)) / (maxComp - outLum);
            }
            return outColor;
        }
    });

    static const std::string kFunctionSat = SHADER0([]() {
        float saturation(vec3 color)
        {
            return max(max(color.r, color.g), color.b) - min(min(color.r, color.g), color.b);
        }

        vec3 set_saturation_helper(float minComp, float midComp, float maxComp,
            float sat)
        {
            if (minComp < maxComp) {
                vec3 result;
                result.r = 0.0;
                result.g = sat * (midComp - minComp) / (maxComp - minComp);
                result.b = sat;
                return result;
            } else {
                return vec3(0, 0, 0);
            }
        }

        vec3 set_saturation(vec3 hueLumColor, vec3 satColor)
        {
            float sat = saturation(satColor);
            if (hueLumColor.r <= hueLumColor.g) {
                if (hueLumColor.g <= hueLumColor.b) {
                    hueLumColor.rgb = set_saturation_helper(hueLumColor.r, hueLumColor.g,
                        hueLumColor.b, sat);
                } else if (hueLumColor.r <= hueLumColor.b) {
                    hueLumColor.rbg = set_saturation_helper(hueLumColor.r, hueLumColor.b,
                        hueLumColor.g, sat);
                } else {
                    hueLumColor.brg = set_saturation_helper(hueLumColor.b, hueLumColor.r,
                        hueLumColor.g, sat);
                }
            } else if (hueLumColor.r <= hueLumColor.b) {
                hueLumColor.grb = set_saturation_helper(hueLumColor.g, hueLumColor.r,
                    hueLumColor.b, sat);
            } else if (hueLumColor.g <= hueLumColor.b) {
                hueLumColor.gbr = set_saturation_helper(hueLumColor.g, hueLumColor.b,
                    hueLumColor.r, sat);
            } else {
                hueLumColor.bgr = set_saturation_helper(hueLumColor.b, hueLumColor.g,
                    hueLumColor.r, sat);
            }
            return hueLumColor;
        }
    });

    switch (blend_mode_) {
    case BLEND_MODE_OVERLAY:
    case BLEND_MODE_HARD_LIGHT:
        return kFunctionHardLight;
    case BLEND_MODE_COLOR_DODGE:
        return kFunctionColorDodgeComponent;
    case BLEND_MODE_COLOR_BURN:
        return kFunctionColorBurnComponent;
    case BLEND_MODE_SOFT_LIGHT:
        return kFunctionSoftLightComponentPosDstAlpha;
    case BLEND_MODE_HUE:
    case BLEND_MODE_SATURATION:
        return kFunctionLum + kFunctionSat;
    case BLEND_MODE_COLOR:
    case BLEND_MODE_LUMINOSITY:
        return kFunctionLum;
    default:
        return std::string();
    }
}

std::string FragmentTexBlendMode::GetBlendFunction() const
{
    return "vec4 Blend(vec4 src, vec4 dst) {"
           "    vec4 result;"
           "    result.a = src.a + (1.0 - src.a) * dst.a;"
        + GetBlendFunctionBodyForRGB() + "    return result;"
                                         "}";
}

std::string FragmentTexBlendMode::GetBlendFunctionBodyForRGB() const
{
    switch (blend_mode_) {
    case BLEND_MODE_NORMAL:
        return "result.rgb = src.rgb + dst.rgb * (1.0 - src.a);";
    case BLEND_MODE_SCREEN:
        return "result.rgb = src.rgb + (1.0 - src.rgb) * dst.rgb;";
    case BLEND_MODE_LIGHTEN:
        return "result.rgb = max((1.0 - src.a) * dst.rgb + src.rgb,"
               "                 (1.0 - dst.a) * src.rgb + dst.rgb);";
    case BLEND_MODE_OVERLAY:
        return "result.rgb = hardLight(dst, src);";
    case BLEND_MODE_DARKEN:
        return "result.rgb = min((1.0 - src.a) * dst.rgb + src.rgb,"
               "                 (1.0 - dst.a) * src.rgb + dst.rgb);";
    case BLEND_MODE_COLOR_DODGE:
        return "result.r = getColorDodgeComponent(src.r, src.a, dst.r, dst.a);"
               "result.g = getColorDodgeComponent(src.g, src.a, dst.g, dst.a);"
               "result.b = getColorDodgeComponent(src.b, src.a, dst.b, dst.a);";
    case BLEND_MODE_COLOR_BURN:
        return "result.r = getColorBurnComponent(src.r, src.a, dst.r, dst.a);"
               "result.g = getColorBurnComponent(src.g, src.a, dst.g, dst.a);"
               "result.b = getColorBurnComponent(src.b, src.a, dst.b, dst.a);";
    case BLEND_MODE_HARD_LIGHT:
        return "result.rgb = hardLight(src, dst);";
    case BLEND_MODE_SOFT_LIGHT:
        return "if (0.0 == dst.a) {"
               "  result.rgb = src.rgb;"
               "} else {"
               "  result.r = getSoftLightComponent(src.r, src.a, dst.r, dst.a);"
               "  result.g = getSoftLightComponent(src.g, src.a, dst.g, dst.a);"
               "  result.b = getSoftLightComponent(src.b, src.a, dst.b, dst.a);"
               "}";
    case BLEND_MODE_DIFFERENCE:
        return "result.rgb = src.rgb + dst.rgb -"
               "    2.0 * min(src.rgb * dst.a, dst.rgb * src.a);";
    case BLEND_MODE_EXCLUSION:
        return "result.rgb = dst.rgb + src.rgb - 2.0 * dst.rgb * src.rgb;";
    case BLEND_MODE_MULTIPLY:
        return "result.rgb = (1.0 - src.a) * dst.rgb +"
               "    (1.0 - dst.a) * src.rgb + src.rgb * dst.rgb;";
    case BLEND_MODE_HUE:
        return "vec4 dstSrcAlpha = dst * src.a;"
               "result.rgb ="
               "    set_luminance(set_saturation(src.rgb * dst.a,"
               "                                 dstSrcAlpha.rgb),"
               "                  dstSrcAlpha.a,"
               "                  dstSrcAlpha.rgb);"
               "result.rgb += (1.0 - src.a) * dst.rgb + (1.0 - dst.a) * src.rgb;";
    case BLEND_MODE_SATURATION:
        return "vec4 dstSrcAlpha = dst * src.a;"
               "result.rgb = set_luminance(set_saturation(dstSrcAlpha.rgb,"
               "                                          src.rgb * dst.a),"
               "                           dstSrcAlpha.a,"
               "                           dstSrcAlpha.rgb);"
               "result.rgb += (1.0 - src.a) * dst.rgb + (1.0 - dst.a) * src.rgb;";
    case BLEND_MODE_COLOR:
        return "vec4 srcDstAlpha = src * dst.a;"
               "result.rgb = set_luminance(srcDstAlpha.rgb,"
               "                           srcDstAlpha.a,"
               "                           dst.rgb * src.a);"
               "result.rgb += (1.0 - src.a) * dst.rgb + (1.0 - dst.a) * src.rgb;";
    case BLEND_MODE_LUMINOSITY:
        return "vec4 srcDstAlpha = src * dst.a;"
               "result.rgb = set_luminance(dst.rgb * src.a,"
               "                           srcDstAlpha.a,"
               "                           srcDstAlpha.rgb);"
               "result.rgb += (1.0 - src.a) * dst.rgb + (1.0 - dst.a) * src.rgb;";
    case BLEND_MODE_NONE:
        NOTREACHED();
    }
    return "result = vec4(1.0, 0.0, 0.0, 1.0);";
}

FragmentTexAlphaBinding::FragmentTexAlphaBinding()
    : sampler_location_(-1)
    , alpha_location_(-1)
{
}

void FragmentTexAlphaBinding::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
        "alpha",
        BLEND_MODE_UNIFORMS,
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms) - UNUSED_BLEND_MODE_UNIFORMS,
        uniforms,
        locations,
        base_uniform_index);
    sampler_location_ = locations[0];
    alpha_location_ = locations[1];
    BLEND_MODE_SET_LOCATIONS(locations, 2);
}

FragmentTexColorMatrixAlphaBinding::FragmentTexColorMatrixAlphaBinding()
    : sampler_location_(-1)
    , alpha_location_(-1)
    , color_matrix_location_(-1)
    , color_offset_location_(-1)
{
}

void FragmentTexColorMatrixAlphaBinding::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
        "alpha",
        "colorMatrix",
        "colorOffset",
        BLEND_MODE_UNIFORMS,
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms) - UNUSED_BLEND_MODE_UNIFORMS,
        uniforms,
        locations,
        base_uniform_index);
    sampler_location_ = locations[0];
    alpha_location_ = locations[1];
    color_matrix_location_ = locations[2];
    color_offset_location_ = locations[3];
    BLEND_MODE_SET_LOCATIONS(locations, 4);
}

FragmentTexOpaqueBinding::FragmentTexOpaqueBinding()
    : sampler_location_(-1)
{
}

void FragmentTexOpaqueBinding::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    sampler_location_ = locations[0];
}

std::string FragmentShaderRGBATexAlpha::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexAlpha::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        uniform SamplerType s_texture;
        uniform float alpha;
    });
}

std::string FragmentShaderRGBATexAlpha::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            gl_FragColor = ApplyBlendMode(texColor * alpha, 0.0);
        }
    });
}

void FragmentShaderRGBATexAlpha::FillLocations(
    ShaderLocations* locations) const
{
    locations->sampler = sampler_location();
    locations->alpha = alpha_location();
    locations->backdrop = backdrop_location();
    locations->backdrop_rect = backdrop_rect_location();
}

std::string FragmentShaderRGBATexColorMatrixAlpha::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexColorMatrixAlpha::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        uniform SamplerType s_texture;
        uniform float alpha;
        uniform mat4 colorMatrix;
        uniform vec4 colorOffset;
    });
}

std::string FragmentShaderRGBATexColorMatrixAlpha::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            float nonZeroAlpha = max(texColor.a, 0.00001);
            texColor = vec4(texColor.rgb / nonZeroAlpha, nonZeroAlpha);
            texColor = colorMatrix * texColor + colorOffset;
            texColor.rgb *= texColor.a;
            texColor = clamp(texColor, 0.0, 1.0);
            gl_FragColor = ApplyBlendMode(texColor * alpha, 0.0);
        }
    });
}

void FragmentShaderRGBATexColorMatrixAlpha::FillLocations(
    ShaderLocations* locations) const
{
    locations->sampler = sampler_location();
    locations->alpha = alpha_location();
    locations->color_matrix = color_matrix_location();
    locations->color_offset = color_offset_location();
    locations->backdrop = backdrop_location();
    locations->backdrop_rect = backdrop_rect_location();
}

std::string FragmentShaderRGBATexVaryingAlpha::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexVaryingAlpha::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        varying float v_alpha;
        uniform SamplerType s_texture;
    });
}

std::string FragmentShaderRGBATexVaryingAlpha::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            gl_FragColor = texColor * v_alpha;
        }
    });
}

std::string FragmentShaderRGBATexPremultiplyAlpha::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexPremultiplyAlpha::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        varying float v_alpha;
        uniform SamplerType s_texture;
    });
}

std::string FragmentShaderRGBATexPremultiplyAlpha::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            texColor.rgb *= texColor.a;
            gl_FragColor = texColor * v_alpha;
        }
    });
}

FragmentTexBackgroundBinding::FragmentTexBackgroundBinding()
    : background_color_location_(-1)
    , sampler_location_(-1)
{
}

void FragmentTexBackgroundBinding::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
        "background_color",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);

    sampler_location_ = locations[0];
    DCHECK_NE(sampler_location_, -1);

    background_color_location_ = locations[1];
    DCHECK_NE(background_color_location_, -1);
}

std::string FragmentShaderTexBackgroundVaryingAlpha::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderTexBackgroundVaryingAlpha::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        varying float v_alpha;
        uniform vec4 background_color;
        uniform SamplerType s_texture;
    });
}

std::string FragmentShaderTexBackgroundVaryingAlpha::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            texColor += background_color * (1.0 - texColor.a);
            gl_FragColor = texColor * v_alpha;
        }
    });
}

std::string FragmentShaderTexBackgroundPremultiplyAlpha::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderTexBackgroundPremultiplyAlpha::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        varying float v_alpha;
        uniform vec4 background_color;
        uniform SamplerType s_texture;
    });
}

std::string FragmentShaderTexBackgroundPremultiplyAlpha::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            texColor.rgb *= texColor.a;
            texColor += background_color * (1.0 - texColor.a);
            gl_FragColor = texColor * v_alpha;
        }
    });
}

std::string FragmentShaderRGBATexOpaque::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexOpaque::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        uniform SamplerType s_texture;
    });
}

std::string FragmentShaderRGBATexOpaque::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            gl_FragColor = vec4(texColor.rgb, 1.0);
        }
    });
}

std::string FragmentShaderRGBATex::GetShaderString(TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATex::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        uniform SamplerType s_texture;
    });
}

std::string FragmentShaderRGBATex::GetShaderBody()
{
    return SHADER0([]() {
        void main() { gl_FragColor = TextureLookup(s_texture, v_texCoord); }
    });
}

std::string FragmentShaderRGBATexSwizzleAlpha::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexSwizzleAlpha::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        uniform SamplerType s_texture;
        uniform float alpha;
    });
}

std::string FragmentShaderRGBATexSwizzleAlpha::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            gl_FragColor = vec4(texColor.z, texColor.y, texColor.x, texColor.w) * alpha;
        }
    });
}

std::string FragmentShaderRGBATexSwizzleOpaque::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexSwizzleOpaque::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        uniform SamplerType s_texture;
    });
}

std::string FragmentShaderRGBATexSwizzleOpaque::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            gl_FragColor = vec4(texColor.z, texColor.y, texColor.x, 1.0);
        }
    });
}

FragmentShaderRGBATexAlphaAA::FragmentShaderRGBATexAlphaAA()
    : sampler_location_(-1)
    , alpha_location_(-1)
{
}

void FragmentShaderRGBATexAlphaAA::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
        "alpha",
        BLEND_MODE_UNIFORMS,
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms) - UNUSED_BLEND_MODE_UNIFORMS,
        uniforms,
        locations,
        base_uniform_index);
    sampler_location_ = locations[0];
    alpha_location_ = locations[1];
    BLEND_MODE_SET_LOCATIONS(locations, 2);
}

std::string FragmentShaderRGBATexAlphaAA::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexAlphaAA::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        uniform SamplerType s_texture;
        uniform float alpha;
        varying TexCoordPrecision vec2 v_texCoord;
        varying TexCoordPrecision vec4 edge_dist[2]; // 8 edge distances.
    });
}

std::string FragmentShaderRGBATexAlphaAA::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            vec4 d4 = min(edge_dist[0], edge_dist[1]);
            vec2 d2 = min(d4.xz, d4.yw);
            float aa = clamp(gl_FragCoord.w * min(d2.x, d2.y), 0.0, 1.0);
            gl_FragColor = ApplyBlendMode(texColor * alpha * aa, 0.0);
        }
    });
}

void FragmentShaderRGBATexAlphaAA::FillLocations(
    ShaderLocations* locations) const
{
    locations->sampler = sampler_location();
    locations->alpha = alpha_location();
    locations->backdrop = backdrop_location();
    locations->backdrop_rect = backdrop_rect_location();
}

FragmentTexClampAlphaAABinding::FragmentTexClampAlphaAABinding()
    : sampler_location_(-1)
    , alpha_location_(-1)
    , fragment_tex_transform_location_(-1)
{
}

void FragmentTexClampAlphaAABinding::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
        "alpha",
        "fragmentTexTransform",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    sampler_location_ = locations[0];
    alpha_location_ = locations[1];
    fragment_tex_transform_location_ = locations[2];
}

std::string FragmentShaderRGBATexClampAlphaAA::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexClampAlphaAA::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        uniform SamplerType s_texture;
        uniform float alpha;
        uniform TexCoordPrecision vec4 fragmentTexTransform;
        varying TexCoordPrecision vec2 v_texCoord;
        varying TexCoordPrecision vec4 edge_dist[2]; // 8 edge distances.
    });
}

std::string FragmentShaderRGBATexClampAlphaAA::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            TexCoordPrecision vec2 texCoord = clamp(v_texCoord, 0.0, 1.0) * fragmentTexTransform.zw + fragmentTexTransform.xy;
            vec4 texColor = TextureLookup(s_texture, texCoord);
            vec4 d4 = min(edge_dist[0], edge_dist[1]);
            vec2 d2 = min(d4.xz, d4.yw);
            float aa = clamp(gl_FragCoord.w * min(d2.x, d2.y), 0.0, 1.0);
            gl_FragColor = texColor * alpha * aa;
        }
    });
}

std::string FragmentShaderRGBATexClampSwizzleAlphaAA::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexClampSwizzleAlphaAA::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        uniform SamplerType s_texture;
        uniform float alpha;
        uniform TexCoordPrecision vec4 fragmentTexTransform;
        varying TexCoordPrecision vec2 v_texCoord;
        varying TexCoordPrecision vec4 edge_dist[2]; // 8 edge distances.
    });
}

std::string FragmentShaderRGBATexClampSwizzleAlphaAA::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            TexCoordPrecision vec2 texCoord = clamp(v_texCoord, 0.0, 1.0) * fragmentTexTransform.zw + fragmentTexTransform.xy;
            vec4 texColor = TextureLookup(s_texture, texCoord);
            vec4 d4 = min(edge_dist[0], edge_dist[1]);
            vec2 d2 = min(d4.xz, d4.yw);
            float aa = clamp(gl_FragCoord.w * min(d2.x, d2.y), 0.0, 1.0);
            gl_FragColor = vec4(texColor.z, texColor.y, texColor.x, texColor.w) * alpha * aa;
        }
    });
}

FragmentShaderRGBATexAlphaMask::FragmentShaderRGBATexAlphaMask()
    : sampler_location_(-1)
    , mask_sampler_location_(-1)
    , alpha_location_(-1)
    , mask_tex_coord_scale_location_(-1)
{
}

void FragmentShaderRGBATexAlphaMask::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
        "s_mask",
        "alpha",
        "maskTexCoordScale",
        "maskTexCoordOffset",
        BLEND_MODE_UNIFORMS,
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms) - UNUSED_BLEND_MODE_UNIFORMS,
        uniforms,
        locations,
        base_uniform_index);
    sampler_location_ = locations[0];
    mask_sampler_location_ = locations[1];
    alpha_location_ = locations[2];
    mask_tex_coord_scale_location_ = locations[3];
    mask_tex_coord_offset_location_ = locations[4];
    BLEND_MODE_SET_LOCATIONS(locations, 5);
}

std::string FragmentShaderRGBATexAlphaMask::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexAlphaMask::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        uniform sampler2D s_texture;
        uniform SamplerType s_mask;
        uniform TexCoordPrecision vec2 maskTexCoordScale;
        uniform TexCoordPrecision vec2 maskTexCoordOffset;
        uniform float alpha;
    });
}

std::string FragmentShaderRGBATexAlphaMask::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = texture2D(s_texture, v_texCoord);
            TexCoordPrecision vec2 maskTexCoord = vec2(maskTexCoordOffset.x + v_texCoord.x * maskTexCoordScale.x,
                maskTexCoordOffset.y + v_texCoord.y * maskTexCoordScale.y);
            vec4 maskColor = TextureLookup(s_mask, maskTexCoord);
            gl_FragColor = ApplyBlendMode(
                texColor * alpha * maskColor.w, maskColor.w);
        }
    });
}

void FragmentShaderRGBATexAlphaMask::FillLocations(
    ShaderLocations* locations) const
{
    locations->sampler = sampler_location();
    locations->mask_sampler = mask_sampler_location();
    locations->mask_tex_coord_scale = mask_tex_coord_scale_location();
    locations->mask_tex_coord_offset = mask_tex_coord_offset_location();
    locations->alpha = alpha_location();
    locations->backdrop = backdrop_location();
    locations->backdrop_rect = backdrop_rect_location();
    if (mask_for_background())
        locations->original_backdrop = original_backdrop_location();
}

FragmentShaderRGBATexAlphaMaskAA::FragmentShaderRGBATexAlphaMaskAA()
    : sampler_location_(-1)
    , mask_sampler_location_(-1)
    , alpha_location_(-1)
    , mask_tex_coord_scale_location_(-1)
    , mask_tex_coord_offset_location_(-1)
{
}

void FragmentShaderRGBATexAlphaMaskAA::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
        "s_mask",
        "alpha",
        "maskTexCoordScale",
        "maskTexCoordOffset",
        BLEND_MODE_UNIFORMS,
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms) - UNUSED_BLEND_MODE_UNIFORMS,
        uniforms,
        locations,
        base_uniform_index);
    sampler_location_ = locations[0];
    mask_sampler_location_ = locations[1];
    alpha_location_ = locations[2];
    mask_tex_coord_scale_location_ = locations[3];
    mask_tex_coord_offset_location_ = locations[4];
    BLEND_MODE_SET_LOCATIONS(locations, 5);
}

std::string FragmentShaderRGBATexAlphaMaskAA::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexAlphaMaskAA::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        uniform sampler2D s_texture;
        uniform SamplerType s_mask;
        uniform TexCoordPrecision vec2 maskTexCoordScale;
        uniform TexCoordPrecision vec2 maskTexCoordOffset;
        uniform float alpha;
        varying TexCoordPrecision vec2 v_texCoord;
        varying TexCoordPrecision vec4 edge_dist[2]; // 8 edge distances.
    });
}

std::string FragmentShaderRGBATexAlphaMaskAA::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = texture2D(s_texture, v_texCoord);
            TexCoordPrecision vec2 maskTexCoord = vec2(maskTexCoordOffset.x + v_texCoord.x * maskTexCoordScale.x,
                maskTexCoordOffset.y + v_texCoord.y * maskTexCoordScale.y);
            vec4 maskColor = TextureLookup(s_mask, maskTexCoord);
            vec4 d4 = min(edge_dist[0], edge_dist[1]);
            vec2 d2 = min(d4.xz, d4.yw);
            float aa = clamp(gl_FragCoord.w * min(d2.x, d2.y), 0.0, 1.0);
            gl_FragColor = ApplyBlendMode(
                texColor * alpha * maskColor.w * aa, maskColor.w);
        }
    });
}

void FragmentShaderRGBATexAlphaMaskAA::FillLocations(
    ShaderLocations* locations) const
{
    locations->sampler = sampler_location();
    locations->mask_sampler = mask_sampler_location();
    locations->mask_tex_coord_scale = mask_tex_coord_scale_location();
    locations->mask_tex_coord_offset = mask_tex_coord_offset_location();
    locations->alpha = alpha_location();
    locations->backdrop = backdrop_location();
    locations->backdrop_rect = backdrop_rect_location();
    if (mask_for_background())
        locations->original_backdrop = original_backdrop_location();
}

FragmentShaderRGBATexAlphaMaskColorMatrixAA::
    FragmentShaderRGBATexAlphaMaskColorMatrixAA()
    : sampler_location_(-1)
    , mask_sampler_location_(-1)
    , alpha_location_(-1)
    , mask_tex_coord_scale_location_(-1)
    , color_matrix_location_(-1)
    , color_offset_location_(-1)
{
}

void FragmentShaderRGBATexAlphaMaskColorMatrixAA::Init(
    GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
        "s_mask",
        "alpha",
        "maskTexCoordScale",
        "maskTexCoordOffset",
        "colorMatrix",
        "colorOffset",
        BLEND_MODE_UNIFORMS,
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms) - UNUSED_BLEND_MODE_UNIFORMS,
        uniforms,
        locations,
        base_uniform_index);
    sampler_location_ = locations[0];
    mask_sampler_location_ = locations[1];
    alpha_location_ = locations[2];
    mask_tex_coord_scale_location_ = locations[3];
    mask_tex_coord_offset_location_ = locations[4];
    color_matrix_location_ = locations[5];
    color_offset_location_ = locations[6];
    BLEND_MODE_SET_LOCATIONS(locations, 7);
}

std::string FragmentShaderRGBATexAlphaMaskColorMatrixAA::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexAlphaMaskColorMatrixAA::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        uniform sampler2D s_texture;
        uniform SamplerType s_mask;
        uniform vec2 maskTexCoordScale;
        uniform vec2 maskTexCoordOffset;
        uniform mat4 colorMatrix;
        uniform vec4 colorOffset;
        uniform float alpha;
        varying TexCoordPrecision vec2 v_texCoord;
        varying TexCoordPrecision vec4 edge_dist[2]; // 8 edge distances.
    });
}

std::string FragmentShaderRGBATexAlphaMaskColorMatrixAA::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = texture2D(s_texture, v_texCoord);
            float nonZeroAlpha = max(texColor.a, 0.00001);
            texColor = vec4(texColor.rgb / nonZeroAlpha, nonZeroAlpha);
            texColor = colorMatrix * texColor + colorOffset;
            texColor.rgb *= texColor.a;
            texColor = clamp(texColor, 0.0, 1.0);
            TexCoordPrecision vec2 maskTexCoord = vec2(maskTexCoordOffset.x + v_texCoord.x * maskTexCoordScale.x,
                maskTexCoordOffset.y + v_texCoord.y * maskTexCoordScale.y);
            vec4 maskColor = TextureLookup(s_mask, maskTexCoord);
            vec4 d4 = min(edge_dist[0], edge_dist[1]);
            vec2 d2 = min(d4.xz, d4.yw);
            float aa = clamp(gl_FragCoord.w * min(d2.x, d2.y), 0.0, 1.0);
            gl_FragColor = ApplyBlendMode(
                texColor * alpha * maskColor.w * aa, maskColor.w);
        }
    });
}

void FragmentShaderRGBATexAlphaMaskColorMatrixAA::FillLocations(
    ShaderLocations* locations) const
{
    locations->sampler = sampler_location();
    locations->alpha = alpha_location();
    locations->mask_sampler = mask_sampler_location();
    locations->mask_tex_coord_scale = mask_tex_coord_scale_location();
    locations->mask_tex_coord_offset = mask_tex_coord_offset_location();
    locations->color_matrix = color_matrix_location();
    locations->color_offset = color_offset_location();
    locations->backdrop = backdrop_location();
    locations->backdrop_rect = backdrop_rect_location();
    if (mask_for_background())
        locations->original_backdrop = original_backdrop_location();
}

FragmentShaderRGBATexAlphaColorMatrixAA::
    FragmentShaderRGBATexAlphaColorMatrixAA()
    : sampler_location_(-1)
    , alpha_location_(-1)
    , color_matrix_location_(-1)
    , color_offset_location_(-1)
{
}

void FragmentShaderRGBATexAlphaColorMatrixAA::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
        "alpha",
        "colorMatrix",
        "colorOffset",
        BLEND_MODE_UNIFORMS,
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms) - UNUSED_BLEND_MODE_UNIFORMS,
        uniforms,
        locations,
        base_uniform_index);
    sampler_location_ = locations[0];
    alpha_location_ = locations[1];
    color_matrix_location_ = locations[2];
    color_offset_location_ = locations[3];
    BLEND_MODE_SET_LOCATIONS(locations, 4);
}

std::string FragmentShaderRGBATexAlphaColorMatrixAA::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexAlphaColorMatrixAA::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        uniform SamplerType s_texture;
        uniform float alpha;
        uniform mat4 colorMatrix;
        uniform vec4 colorOffset;
        varying TexCoordPrecision vec2 v_texCoord;
        varying TexCoordPrecision vec4 edge_dist[2]; // 8 edge distances.
    });
}

std::string FragmentShaderRGBATexAlphaColorMatrixAA::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = TextureLookup(s_texture, v_texCoord);
            float nonZeroAlpha = max(texColor.a, 0.00001);
            texColor = vec4(texColor.rgb / nonZeroAlpha, nonZeroAlpha);
            texColor = colorMatrix * texColor + colorOffset;
            texColor.rgb *= texColor.a;
            texColor = clamp(texColor, 0.0, 1.0);
            vec4 d4 = min(edge_dist[0], edge_dist[1]);
            vec2 d2 = min(d4.xz, d4.yw);
            float aa = clamp(gl_FragCoord.w * min(d2.x, d2.y), 0.0, 1.0);
            gl_FragColor = ApplyBlendMode(texColor * alpha * aa, 0.0);
        }
    });
}

void FragmentShaderRGBATexAlphaColorMatrixAA::FillLocations(
    ShaderLocations* locations) const
{
    locations->sampler = sampler_location();
    locations->alpha = alpha_location();
    locations->color_matrix = color_matrix_location();
    locations->color_offset = color_offset_location();
    locations->backdrop = backdrop_location();
    locations->backdrop_rect = backdrop_rect_location();
}

FragmentShaderRGBATexAlphaMaskColorMatrix::
    FragmentShaderRGBATexAlphaMaskColorMatrix()
    : sampler_location_(-1)
    , mask_sampler_location_(-1)
    , alpha_location_(-1)
    , mask_tex_coord_scale_location_(-1)
{
}

void FragmentShaderRGBATexAlphaMaskColorMatrix::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "s_texture",
        "s_mask",
        "alpha",
        "maskTexCoordScale",
        "maskTexCoordOffset",
        "colorMatrix",
        "colorOffset",
        BLEND_MODE_UNIFORMS,
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms) - UNUSED_BLEND_MODE_UNIFORMS,
        uniforms,
        locations,
        base_uniform_index);
    sampler_location_ = locations[0];
    mask_sampler_location_ = locations[1];
    alpha_location_ = locations[2];
    mask_tex_coord_scale_location_ = locations[3];
    mask_tex_coord_offset_location_ = locations[4];
    color_matrix_location_ = locations[5];
    color_offset_location_ = locations[6];
    BLEND_MODE_SET_LOCATIONS(locations, 7);
}

std::string FragmentShaderRGBATexAlphaMaskColorMatrix::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderRGBATexAlphaMaskColorMatrix::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        varying TexCoordPrecision vec2 v_texCoord;
        uniform sampler2D s_texture;
        uniform SamplerType s_mask;
        uniform vec2 maskTexCoordScale;
        uniform vec2 maskTexCoordOffset;
        uniform mat4 colorMatrix;
        uniform vec4 colorOffset;
        uniform float alpha;
    });
}

std::string FragmentShaderRGBATexAlphaMaskColorMatrix::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 texColor = texture2D(s_texture, v_texCoord);
            float nonZeroAlpha = max(texColor.a, 0.00001);
            texColor = vec4(texColor.rgb / nonZeroAlpha, nonZeroAlpha);
            texColor = colorMatrix * texColor + colorOffset;
            texColor.rgb *= texColor.a;
            texColor = clamp(texColor, 0.0, 1.0);
            TexCoordPrecision vec2 maskTexCoord = vec2(maskTexCoordOffset.x + v_texCoord.x * maskTexCoordScale.x,
                maskTexCoordOffset.y + v_texCoord.y * maskTexCoordScale.y);
            vec4 maskColor = TextureLookup(s_mask, maskTexCoord);
            gl_FragColor = ApplyBlendMode(
                texColor * alpha * maskColor.w, maskColor.w);
        }
    });
}

void FragmentShaderRGBATexAlphaMaskColorMatrix::FillLocations(
    ShaderLocations* locations) const
{
    locations->sampler = sampler_location();
    locations->mask_sampler = mask_sampler_location();
    locations->mask_tex_coord_scale = mask_tex_coord_scale_location();
    locations->mask_tex_coord_offset = mask_tex_coord_offset_location();
    locations->alpha = alpha_location();
    locations->color_matrix = color_matrix_location();
    locations->color_offset = color_offset_location();
    locations->backdrop = backdrop_location();
    locations->backdrop_rect = backdrop_rect_location();
    if (mask_for_background())
        locations->original_backdrop = original_backdrop_location();
}

FragmentShaderYUVVideo::FragmentShaderYUVVideo()
    : y_texture_location_(-1)
    , u_texture_location_(-1)
    , v_texture_location_(-1)
    , alpha_location_(-1)
    , yuv_matrix_location_(-1)
    , yuv_adj_location_(-1)
    , ya_clamp_rect_location_(-1)
    , uv_clamp_rect_location_(-1)
{
}

void FragmentShaderYUVVideo::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = { "y_texture",
        "u_texture",
        "v_texture",
        "alpha",
        "yuv_matrix",
        "yuv_adj",
        "ya_clamp_rect",
        "uv_clamp_rect" };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    y_texture_location_ = locations[0];
    u_texture_location_ = locations[1];
    v_texture_location_ = locations[2];
    alpha_location_ = locations[3];
    yuv_matrix_location_ = locations[4];
    yuv_adj_location_ = locations[5];
    ya_clamp_rect_location_ = locations[6];
    uv_clamp_rect_location_ = locations[7];
}

std::string FragmentShaderYUVVideo::GetShaderString(TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderYUVVideo::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        precision mediump int;
        varying TexCoordPrecision vec2 v_yaTexCoord;
        varying TexCoordPrecision vec2 v_uvTexCoord;
        uniform SamplerType y_texture;
        uniform SamplerType u_texture;
        uniform SamplerType v_texture;
        uniform float alpha;
        uniform vec3 yuv_adj;
        uniform mat3 yuv_matrix;
        uniform vec4 ya_clamp_rect;
        uniform vec4 uv_clamp_rect;
    });
}

std::string FragmentShaderYUVVideo::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec2 ya_clamped = max(ya_clamp_rect.xy, min(ya_clamp_rect.zw, v_yaTexCoord));
            float y_raw = TextureLookup(y_texture, ya_clamped).x;
            vec2 uv_clamped = max(uv_clamp_rect.xy, min(uv_clamp_rect.zw, v_uvTexCoord));
            float u_unsigned = TextureLookup(u_texture, uv_clamped).x;
            float v_unsigned = TextureLookup(v_texture, uv_clamped).x;
            vec3 yuv = vec3(y_raw, u_unsigned, v_unsigned) + yuv_adj;
            vec3 rgb = yuv_matrix * yuv;
            gl_FragColor = vec4(rgb, 1.0) * alpha;
        }
    });
}

FragmentShaderYUVAVideo::FragmentShaderYUVAVideo()
    : y_texture_location_(-1)
    , u_texture_location_(-1)
    , v_texture_location_(-1)
    , a_texture_location_(-1)
    , alpha_location_(-1)
    , yuv_matrix_location_(-1)
    , yuv_adj_location_(-1)
{
}

void FragmentShaderYUVAVideo::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "y_texture",
        "u_texture",
        "v_texture",
        "a_texture",
        "alpha",
        "cc_matrix",
        "yuv_adj",
        "ya_clamp_rect",
        "uv_clamp_rect",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    y_texture_location_ = locations[0];
    u_texture_location_ = locations[1];
    v_texture_location_ = locations[2];
    a_texture_location_ = locations[3];
    alpha_location_ = locations[4];
    yuv_matrix_location_ = locations[5];
    yuv_adj_location_ = locations[6];
    ya_clamp_rect_location_ = locations[7];
    uv_clamp_rect_location_ = locations[8];
}

std::string FragmentShaderYUVAVideo::GetShaderString(
    TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderYUVAVideo::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        precision mediump int;
        varying TexCoordPrecision vec2 v_yaTexCoord;
        varying TexCoordPrecision vec2 v_uvTexCoord;
        uniform SamplerType y_texture;
        uniform SamplerType u_texture;
        uniform SamplerType v_texture;
        uniform SamplerType a_texture;
        uniform float alpha;
        uniform vec3 yuv_adj;
        uniform mat3 yuv_matrix;
        uniform vec4 ya_clamp_rect;
        uniform vec4 uv_clamp_rect;
    });
}

std::string FragmentShaderYUVAVideo::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec2 ya_clamped = max(ya_clamp_rect.xy, min(ya_clamp_rect.zw, v_yaTexCoord));
            float y_raw = TextureLookup(y_texture, ya_clamped).x;
            vec2 uv_clamped = max(uv_clamp_rect.xy, min(uv_clamp_rect.zw, v_uvTexCoord));
            float u_unsigned = TextureLookup(u_texture, uv_clamped).x;
            float v_unsigned = TextureLookup(v_texture, uv_clamped).x;
            float a_raw = TextureLookup(a_texture, ya_clamped).x;
            vec3 yuv = vec3(y_raw, u_unsigned, v_unsigned) + yuv_adj;
            vec3 rgb = yuv_matrix * yuv;
            gl_FragColor = vec4(rgb, 1.0) * (alpha * a_raw);
        }
    });
}

FragmentShaderColor::FragmentShaderColor()
    : color_location_(-1)
{
}

void FragmentShaderColor::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "color",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    color_location_ = locations[0];
}

std::string FragmentShaderColor::GetShaderString(TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderColor::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        uniform vec4 color;
    });
}

std::string FragmentShaderColor::GetShaderBody()
{
    return SHADER0([]() {
        void main() { gl_FragColor = color; }
    });
}

FragmentShaderColorAA::FragmentShaderColorAA()
    : color_location_(-1)
{
}

void FragmentShaderColorAA::Init(GLES2Interface* context,
    unsigned program,
    int* base_uniform_index)
{
    static const char* uniforms[] = {
        "color",
    };
    int locations[arraysize(uniforms)];

    GetProgramUniformLocations(context,
        program,
        arraysize(uniforms),
        uniforms,
        locations,
        base_uniform_index);
    color_location_ = locations[0];
}

std::string FragmentShaderColorAA::GetShaderString(TexCoordPrecision precision,
    SamplerType sampler) const
{
    return FRAGMENT_SHADER(GetShaderHead(), GetShaderBody());
}

std::string FragmentShaderColorAA::GetShaderHead()
{
    return SHADER0([]() {
        precision mediump float;
        uniform vec4 color;
        varying vec4 edge_dist[2]; // 8 edge distances.
    });
}

std::string FragmentShaderColorAA::GetShaderBody()
{
    return SHADER0([]() {
        void main()
        {
            vec4 d4 = min(edge_dist[0], edge_dist[1]);
            vec2 d2 = min(d4.xz, d4.yw);
            float aa = clamp(gl_FragCoord.w * min(d2.x, d2.y), 0.0, 1.0);
            gl_FragColor = color * aa;
        }
    });
}

} // namespace cc
