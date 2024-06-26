// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_GEOMETRY_INSETS_H_
#define UI_GFX_GEOMETRY_INSETS_H_

#include <string>

#include "ui/gfx/geometry/insets_f.h"
#include "ui/gfx/gfx_export.h"

namespace gfx {

// An integer version of gfx::Insets.
class GFX_EXPORT Insets {
public:
    Insets();
    Insets(int top, int left, int bottom, int right);

    ~Insets();

    int top() const { return top_; }
    int left() const { return left_; }
    int bottom() const { return bottom_; }
    int right() const { return right_; }

    // Returns the total width taken up by the insets, which is the sum of the
    // left and right insets.
    int width() const { return left_ + right_; }

    // Returns the total height taken up by the insets, which is the sum of the
    // top and bottom insets.
    int height() const { return top_ + bottom_; }

    // Returns true if the insets are empty.
    bool IsEmpty() const { return width() == 0 && height() == 0; }

    void Set(int top, int left, int bottom, int right)
    {
        top_ = top;
        left_ = left;
        bottom_ = bottom;
        right_ = right;
    }

    bool operator==(const Insets& insets) const
    {
        return top_ == insets.top_ && left_ == insets.left_ && bottom_ == insets.bottom_ && right_ == insets.right_;
    }

    bool operator!=(const Insets& insets) const
    {
        return !(*this == insets);
    }

    void operator+=(const Insets& insets)
    {
        top_ += insets.top_;
        left_ += insets.left_;
        bottom_ += insets.bottom_;
        right_ += insets.right_;
    }

    Insets operator-() const
    {
        return Insets(-top_, -left_, -bottom_, -right_);
    }

    Insets Scale(float scale) const
    {
        return Scale(scale, scale);
    }

    Insets Scale(float x_scale, float y_scale) const
    {
        return Insets(static_cast<int>(top() * y_scale),
            static_cast<int>(left() * x_scale),
            static_cast<int>(bottom() * y_scale),
            static_cast<int>(right() * x_scale));
    }

    operator InsetsF() const
    {
        return InsetsF(static_cast<float>(top()), static_cast<float>(left()),
            static_cast<float>(bottom()), static_cast<float>(right()));
    }

    // Returns a string representation of the insets.
    std::string ToString() const;

private:
    int top_;
    int left_;
    int bottom_;
    int right_;
};

} // namespace gfx

#endif // UI_GFX_GEOMETRY_INSETS_H_
