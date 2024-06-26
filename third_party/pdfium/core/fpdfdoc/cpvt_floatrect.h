// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPVT_FLOATRECT_H_
#define CORE_FPDFDOC_CPVT_FLOATRECT_H_

#include "core/fxcrt/fx_coordinates.h"

class CPVT_FloatRect final : public CFX_FloatRect {
 public:
  CPVT_FloatRect() = default;

  CPVT_FloatRect(float other_left,
                 float other_top,
                 float other_right,
                 float other_bottom)
      : CFX_FloatRect(other_left, other_bottom, other_right, other_top) {}

  explicit CPVT_FloatRect(const CFX_FloatRect& rect)
      : CFX_FloatRect(rect.left, rect.bottom, rect.right, rect.top) {}

  float Height() const {
    if (top > bottom)
      return top - bottom;
    return bottom - top;
  }
};

#endif  // CORE_FPDFDOC_CPVT_FLOATRECT_H_
