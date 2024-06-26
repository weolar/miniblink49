// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_base_FloatBox_h
#define mc_base_FloatBox_h

#include "third_party/WebKit/Source/platform/geometry/FloatPoint3D.h"

#include <iosfwd>
#include <string>

// #include "ui/gfx/geometry/point3_f.h"
// #include "ui/gfx/geometry/vector3d_f.h"

namespace mc {

// A 3d version of gfx::RectF, with the positive z-axis pointed towards
// the camera.
class FloatBox {
public:
    FloatBox()
        : width_(0.f),
        height_(0.f),
        depth_(0.f) {}

    FloatBox(float width, float height, float depth)
        : width_(width < 0 ? 0 : width),
        height_(height < 0 ? 0 : height),
        depth_(depth < 0 ? 0 : depth) {}

    FloatBox(float x, float y, float z, float width, float height, float depth)
        : origin_(x, y, z),
        width_(width < 0 ? 0 : width),
        height_(height < 0 ? 0 : height),
        depth_(depth < 0 ? 0 : depth) {}

    FloatBox(const blink::FloatPoint3D& origin, float width, float height, float depth)
        : origin_(origin),
        width_(width < 0 ? 0 : width),
        height_(height < 0 ? 0 : height),
        depth_(depth < 0 ? 0 : depth) {}

    ~FloatBox() {}

    // Scales all three axes by the given scale.
    void Scale(float scale) {
        Scale(scale, scale, scale);
    }

    // Scales each axis by the corresponding given scale.
    void Scale(float x_scale, float y_scale, float z_scale) {
        origin_.scale(x_scale, y_scale, z_scale);
        set_size(width_ * x_scale, height_ * y_scale, depth_ * z_scale);
    }

    // Moves the box by the specified distance in each dimension.
    void operator+=(const blink::FloatPoint3D& offset) {
        origin_ += offset;
    }

    // Returns true if the box has no interior points.
    bool IsEmpty() const;

    // Computes the union of this box with the given box. The union is the
    // smallest box that contains both boxes.
    void Union(const FloatBox& box);

    std::string ToString() const;

    float x() const {
        return origin_.x();
    }
    void set_x(float x) {
        origin_.setX(x);
    }

    float y() const {
        return origin_.y();
    }
    void set_y(float y) {
        origin_.setY(y);
    }

    float z() const {
        return origin_.z();
    }
    void set_z(float z) {
        origin_.setZ(z);
    }

    float width() const {
        return width_;
    }
    void set_width(float width) {
        width_ = width < 0 ? 0 : width;
    }

    float height() const {
        return height_;
    }
    void set_height(float height) {
        height_ = height < 0 ? 0 : height;
    }

    float depth() const {
        return depth_;
    }
    void set_depth(float depth) {
        depth_ = depth < 0 ? 0 : depth;
    }

    float right() const {
        return x() + width();
    }
    float bottom() const {
        return y() + height();
    }
    float front() const {
        return z() + depth();
    }

    void set_size(float width, float height, float depth) {
        width_ = width < 0 ? 0 : width;
        height_ = height < 0 ? 0 : height;
        depth_ = depth < 0 ? 0 : depth;
    }

    const blink::FloatPoint3D& origin() const {
        return origin_;
    }
    void set_origin(const blink::FloatPoint3D& origin) {
        origin_ = origin;
    }

    // Expands |this| to contain the given point, if necessary. Please note, even
    // if |this| is empty, after the function |this| will continue to contain its
    // |origin_|.
    void ExpandTo(const blink::FloatPoint3D& point);

    // Expands |this| to contain the given box, if necessary. Please note, even
    // if |this| is empty, after the function |this| will continue to contain its
    // |origin_|.
    void ExpandTo(const FloatBox& box);

private:
    // Expands the box to contain the two given points. It is required that each
    // component of |min| is less than or equal to the corresponding component in
    // |max|. Precisely, what this function does is ensure that after the function
    // completes, |this| contains origin_, min, max, and origin_ + (width_,
    // height_, depth_), even if the box is empty. Emptiness checks are handled in
    // the public function Union.
    void ExpandTo(const blink::FloatPoint3D& min, const blink::FloatPoint3D& max);

    blink::FloatPoint3D origin_;
    float width_;
    float height_;
    float depth_;
};

FloatBox UnionBoxes(const FloatBox& a, const FloatBox& b);

inline FloatBox ScaleBox(const FloatBox& b,
    float x_scale,
    float y_scale,
    float z_scale) {
    return FloatBox(b.x() * x_scale,
        b.y() * y_scale,
        b.z() * z_scale,
        b.width() * x_scale,
        b.height() * y_scale,
        b.depth() * z_scale);
}

inline FloatBox ScaleBox(const FloatBox& b, float scale) {
    return ScaleBox(b, scale, scale, scale);
}

inline bool operator==(const FloatBox& a, const FloatBox& b) {
    return a.origin() == b.origin() && a.width() == b.width() &&
        a.height() == b.height() && a.depth() == b.depth();
}

inline bool operator!=(const FloatBox& a, const FloatBox& b) {
    return !(a == b);
}

inline FloatBox operator+(const FloatBox& b, const blink::FloatPoint3D& v) {
    return FloatBox(b.x() + v.x(),
        b.y() + v.y(),
        b.z() + v.z(),
        b.width(),
        b.height(),
        b.depth());
}

// This is declared here for use in gtest-based unit tests but is defined in
// the gfx_test_support target. Depend on that to use this in your unit test.
// This should not be used in production code - call ToString() instead.
//void PrintTo(const FloatBox& box, ::std::ostream* os);

}  // namespace mc

#endif  // mc_base_FloatBox_h
