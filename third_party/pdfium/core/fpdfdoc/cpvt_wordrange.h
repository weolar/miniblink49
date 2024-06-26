// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPVT_WORDRANGE_H_
#define CORE_FPDFDOC_CPVT_WORDRANGE_H_

#include <algorithm>
#include <utility>

#include "core/fpdfdoc/cpvt_wordplace.h"
#include "core/fxcrt/fx_system.h"

struct CPVT_WordRange {
  CPVT_WordRange() = default;

  CPVT_WordRange(const CPVT_WordPlace& begin, const CPVT_WordPlace& end)
      : BeginPos(begin), EndPos(end) {
    Normalize();
  }

  inline bool IsEmpty() const { return BeginPos == EndPos; }
  inline bool operator==(const CPVT_WordRange& wr) const {
    return wr.BeginPos == BeginPos && wr.EndPos == EndPos;
  }
  inline bool operator!=(const CPVT_WordRange& wr) const {
    return !(*this == wr);
  }

  void Normalize() {
    if (BeginPos > EndPos)
      std::swap(BeginPos, EndPos);
  }

  CPVT_WordPlace BeginPos;
  CPVT_WordPlace EndPos;
};

#endif  // CORE_FPDFDOC_CPVT_WORDRANGE_H_
