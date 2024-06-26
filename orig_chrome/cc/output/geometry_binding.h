// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_GEOMETRY_BINDING_H_
#define CC_OUTPUT_GEOMETRY_BINDING_H_

#include "base/basictypes.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "third_party/khronos/GLES2/gl2ext.h"
#include "ui/gfx/geometry/rect_f.h"

namespace gfx {
class QuadF;
class Quad;
class QuadIndex;
class PointF;
}

namespace cc {

struct GeometryBindingVertex {
    float a_position[3];
    float a_texCoord[2];
    // Index of the vertex, divide by 4 to have the matrix for this quad.
    float a_index;
};

struct GeometryBindingQuad {
    GeometryBindingQuad();
    GeometryBindingQuad(const GeometryBindingVertex& vert0,
        const GeometryBindingVertex& vert1,
        const GeometryBindingVertex& vert2,
        const GeometryBindingVertex& vert3);
    GeometryBindingVertex v0, v1, v2, v3;
};

struct GeometryBindingQuadIndex {
    GeometryBindingQuadIndex();
    GeometryBindingQuadIndex(uint16 index0,
        uint16 index1,
        uint16 index2,
        uint16 index3,
        uint16 index4,
        uint16 index5);

    uint16 data[6];
};

class DrawQuad;
class DrawPolygon;

struct GeometryBinding {
    // All layer shaders share the same attribute locations for the vertex
    // positions and texture coordinates. This allows switching shaders without
    // rebinding attribute arrays.
    static int PositionAttribLocation() { return 0; }
    static int TexCoordAttribLocation() { return 1; }
    static int TriangleIndexAttribLocation() { return 2; }
};

void SetupGLContext(gpu::gles2::GLES2Interface* gl,
    GLuint quad_elements_vbo,
    GLuint quad_vertices_vbo);

} // namespace cc

#endif // CC_OUTPUT_GEOMETRY_BINDING_H_
