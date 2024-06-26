// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/base/FloatBox.h"

#include <algorithm>

// #include "base/logging.h"
// #include "base/strings/stringprintf.h"

namespace mc {

// std::string FloatBox::ToString() const {
//   return base::StringPrintf("%s %fx%fx%f",
//                             origin().ToString().c_str(),
//                             width_,
//                             height_,
//                             depth_);
// }

bool FloatBox::IsEmpty() const {
  return (width_ == 0 && height_ == 0) ||
         (width_ == 0 && depth_ == 0) ||
         (height_ == 0 && depth_ == 0);
}

void FloatBox::ExpandTo(const blink::FloatPoint3D& min, const blink::FloatPoint3D& max) {
//   DCHECK_LE(min.x(), max.x());
//   DCHECK_LE(min.y(), max.y());
//   DCHECK_LE(min.z(), max.z());

  float min_x = std::min(x(), min.x());
  float min_y = std::min(y(), min.y());
  float min_z = std::min(z(), min.z());
  float max_x = std::max(right(), max.x());
  float max_y = std::max(bottom(), max.y());
  float max_z = std::max(front(), max.z());

  origin_ = blink::FloatPoint3D(min_x, min_y, min_z);
  width_ = max_x - min_x;
  height_ = max_y - min_y;
  depth_ = max_z - min_z;
}

void FloatBox::Union(const FloatBox& box) {
  if (IsEmpty()) {
    *this = box;
    return;
  }
  if (box.IsEmpty())
    return;
  ExpandTo(box);
}

void FloatBox::ExpandTo(const blink::FloatPoint3D& point) {
  ExpandTo(point, point);
}

void FloatBox::ExpandTo(const FloatBox& box) {
  ExpandTo(box.origin(), blink::FloatPoint3D(box.right(), box.bottom(), box.front()));
}

FloatBox UnionBoxes(const FloatBox& a, const FloatBox& b) {
  FloatBox result = a;
  result.Union(b);
  return result;
}

}  // namespace gfx
