// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/dynamic_geometry_binding.h"

#include "gpu/command_buffer/client/gles2_interface.h"
#include "ui/gfx/geometry/quad_f.h"
#include "ui/gfx/geometry/rect_f.h"

namespace cc {

DynamicGeometryBinding::DynamicGeometryBinding(gpu::gles2::GLES2Interface* gl)
    : gl_(gl)
    , quad_vertices_vbo_(0)
    , quad_elements_vbo_(0)
{
    GeometryBindingQuad quads[1];
    GeometryBindingQuadIndex quad_indices[1];

    static_assert(sizeof(GeometryBindingQuad) == 24 * sizeof(float),
        "struct Quad should be densely packed");
    static_assert(sizeof(GeometryBindingQuadIndex) == 6 * sizeof(uint16_t),
        "struct QuadIndex should be densely packed");

    gl_->GenBuffers(1, &quad_vertices_vbo_);
    gl_->GenBuffers(1, &quad_elements_vbo_);

    gl_->BindBuffer(GL_ARRAY_BUFFER, quad_vertices_vbo_);
    gl_->BufferData(GL_ARRAY_BUFFER, sizeof(GeometryBindingQuad) * 1, quads,
        GL_DYNAMIC_DRAW);

    gl_->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_elements_vbo_);
    gl_->BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GeometryBindingQuadIndex) * 1,
        &quad_indices, GL_DYNAMIC_DRAW);
}

void DynamicGeometryBinding::InitializeCustomQuad(const gfx::QuadF& quad)
{
    float uv[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
    InitializeCustomQuadWithUVs(quad, uv);
}

void DynamicGeometryBinding::InitializeCustomQuadWithUVs(const gfx::QuadF& quad,
    const float uv[8])
{
    GeometryBindingVertex v0 = {
        { quad.p1().x(), quad.p1().y(), 0.0f }, { uv[0], uv[1] }, 0.0f
    };
    GeometryBindingVertex v1 = {
        { quad.p2().x(), quad.p2().y(), 0.0f }, { uv[2], uv[3] }, 1.0f
    };
    GeometryBindingVertex v2 = {
        { quad.p3().x(), quad.p3().y(), 0.0f }, { uv[4], uv[5] }, 2.0f
    };
    GeometryBindingVertex v3 = {
        { quad.p4().x(), quad.p4().y(), 0.0f }, { uv[6], uv[7] }, 3.0f
    };

    GeometryBindingQuad local_quad = { v0, v1, v2, v3 };
    GeometryBindingQuadIndex quad_index(
        static_cast<uint16>(0), static_cast<uint16>(1), static_cast<uint16>(2),
        static_cast<uint16>(3), static_cast<uint16>(0), static_cast<uint16>(2));

    gl_->BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GeometryBindingQuad),
        &local_quad);
    gl_->BufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
        sizeof(GeometryBindingQuadIndex), &quad_index);
}

void DynamicGeometryBinding::PrepareForDraw()
{
    SetupGLContext(gl_, quad_elements_vbo_, quad_vertices_vbo_);
}

} // namespace cc
