// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_GEOMETRY_INSETS_F_H_
#define UI_GFX_GEOMETRY_INSETS_F_H_

#include <string>

#include "ui/gfx/gfx_export.h"

namespace gfx {

// A floating versin of gfx::Insets.
class GFX_EXPORT InsetsF {
public:
    InsetsF();
    InsetsF(float top, float left, float bottom, float right);
    ~InsetsF();

    float top() const { return top_; }
    float left() const { return left_; }
    float bottom() const { return bottom_; }
    float right() const { return right_; }

    // Returns the total width taken up by the insets, which is the sum of the
    // left and right insets.
    float width() const { return left_ + right_; }

    // Returns the total height taken up by the insets, which is the sum of the
    // top and bottom insets.
    float height() const { return top_ + bottom_; }

    // Returns true if the insets are empty.
    bool IsEmpty() const { return width() == 0.f && height() == 0.f; }

    void Set(float top, float left, float bottom, float right)
    {
        top_ = top;
        left_ = left;
        bottom_ = bottom;
        right_ = right;
    }

    bool operator==(const InsetsF& insets) const
    {
        return top_ == insets.top_ && left_ == insets.left_ && bottom_ == insets.bottom_ && right_ == insets.right_;
    }

    bool operator!=(const InsetsF& insets) const
    {
        return !(*this == insets);
    }

    void operator+=(const InsetsF& insets)
    {
        top_ += insets.top_;
        left_ += insets.left_;
        bottom_ += insets.bottom_;
        right_ += insets.right_;
    }

    InsetsF operator-() const
    {
        return InsetsF(-top_, -left_, -bottom_, -right_);
    }

    // Returns a string representation of the insets.
    std::string ToString() const;

private:
    float top_;
    float left_;
    float bottom_;
    float right_;
};

} // namespace gfx

#endif // UI_GFX_GEOMETRY_INSETS_F_H_
