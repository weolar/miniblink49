// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_DYNAMIC_GEOMETRY_BINDING_H_
#define CC_OUTPUT_DYNAMIC_GEOMETRY_BINDING_H_

#include "cc/output/geometry_binding.h"

namespace cc {

class DynamicGeometryBinding {
public:
    explicit DynamicGeometryBinding(gpu::gles2::GLES2Interface* gl);
    void PrepareForDraw();
    void InitializeCustomQuad(const gfx::QuadF& quad);
    void InitializeCustomQuadWithUVs(const gfx::QuadF& quad, const float uv[8]);

private:
    gpu::gles2::GLES2Interface* gl_;

    GLuint quad_vertices_vbo_;
    GLuint quad_elements_vbo_;

    DISALLOW_COPY_AND_ASSIGN(DynamicGeometryBinding);
};

} // namespace cc

#endif // CC_OUTPUT_DYNAMIC_GEOMETRY_BINDING_H_
