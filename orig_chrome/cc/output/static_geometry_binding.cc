// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/static_geometry_binding.h"

#include "gpu/command_buffer/client/gles2_interface.h"
#include "ui/gfx/geometry/rect_f.h"

namespace cc {

StaticGeometryBinding::StaticGeometryBinding(gpu::gles2::GLES2Interface* gl,
    const gfx::RectF& quad_vertex_rect)
    : gl_(gl)
    , quad_vertices_vbo_(0)
    , quad_elements_vbo_(0)
{
    GeometryBindingQuad quads[8];
    GeometryBindingQuadIndex quad_indices[8];

    static_assert(sizeof(GeometryBindingQuad) == 24 * sizeof(float),
        "struct Quad should be densely packed");
    static_assert(sizeof(GeometryBindingQuadIndex) == 6 * sizeof(uint16_t),
        "struct QuadIndex should be densely packed");

    for (size_t i = 0; i < 8; i++) {
        GeometryBindingVertex v0 = {
            { quad_vertex_rect.x(), quad_vertex_rect.bottom(), 0.0f },
            { 0.0f, 1.0f },
            i * 4.0f + 0.0f
        };
        GeometryBindingVertex v1 = {
            { quad_vertex_rect.x(), quad_vertex_rect.y(), 0.0f },
            { 0.0f, 0.0f },
            i * 4.0f + 1.0f
        };
        GeometryBindingVertex v2 = {
            { quad_vertex_rect.right(), quad_vertex_rect.y(), 0.0f },
            { 1.0f, 0.0f },
            i * 4.0f + 2.0f
        };
        GeometryBindingVertex v3 = {
            { quad_vertex_rect.right(), quad_vertex_rect.bottom(), 0.0f },
            { 1.0f, 1.0f },
            i * 4.0f + 3.0f
        };
        GeometryBindingQuad x(v0, v1, v2, v3);
        quads[i] = x;
        GeometryBindingQuadIndex y(
            static_cast<uint16>(0 + 4 * i), static_cast<uint16>(1 + 4 * i),
            static_cast<uint16>(2 + 4 * i), static_cast<uint16>(3 + 4 * i),
            static_cast<uint16>(0 + 4 * i), static_cast<uint16>(2 + 4 * i));
        quad_indices[i] = y;
    }

    gl_->GenBuffers(1, &quad_vertices_vbo_);
    gl_->GenBuffers(1, &quad_elements_vbo_);

    gl_->BindBuffer(GL_ARRAY_BUFFER, quad_vertices_vbo_);
    gl_->BufferData(GL_ARRAY_BUFFER, sizeof(GeometryBindingQuad) * 8, quads,
        GL_STATIC_DRAW);

    gl_->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_elements_vbo_);
    gl_->BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GeometryBindingQuadIndex) * 8,
        &quad_indices, GL_STATIC_DRAW);
}

StaticGeometryBinding::~StaticGeometryBinding()
{
    gl_->DeleteBuffers(1, &quad_vertices_vbo_);
    gl_->DeleteBuffers(1, &quad_elements_vbo_);
}

void StaticGeometryBinding::PrepareForDraw()
{
    SetupGLContext(gl_, quad_elements_vbo_, quad_vertices_vbo_);
}

} // namespace cc
