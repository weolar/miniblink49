// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPVT_WORDPLACE_H_
#define CORE_FPDFDOC_CPVT_WORDPLACE_H_

#include "core/fxcrt/fx_system.h"

struct CPVT_WordPlace {
  CPVT_WordPlace() : nSecIndex(-1), nLineIndex(-1), nWordIndex(-1) {}

  CPVT_WordPlace(int32_t other_nSecIndex,
                 int32_t other_nLineIndex,
                 int32_t other_nWordIndex)
      : nSecIndex(other_nSecIndex),
        nLineIndex(other_nLineIndex),
        nWordIndex(other_nWordIndex) {}

  void Reset() {
    nSecIndex = -1;
    nLineIndex = -1;
    nWordIndex = -1;
  }

  void AdvanceSection() {
    nSecIndex++;
    nLineIndex = 0;
    nWordIndex = -1;
  }

  inline bool operator==(const CPVT_WordPlace& wp) const {
    return wp.nSecIndex == nSecIndex && wp.nLineIndex == nLineIndex &&
           wp.nWordIndex == nWordIndex;
  }
  inline bool operator!=(const CPVT_WordPlace& wp) const {
    return !(*this == wp);
  }
  inline bool operator<(const CPVT_WordPlace& wp) const {
    if (nSecIndex != wp.nSecIndex)
      return nSecIndex < wp.nSecIndex;
    if (nLineIndex != wp.nLineIndex)
      return nLineIndex < wp.nLineIndex;
    return nWordIndex < wp.nWordIndex;
  }
  inline bool operator>(const CPVT_WordPlace& wp) const {
    if (nSecIndex != wp.nSecIndex)
      return nSecIndex > wp.nSecIndex;
    if (nLineIndex != wp.nLineIndex)
      return nLineIndex > wp.nLineIndex;
    return nWordIndex > wp.nWordIndex;
  }
  inline bool operator<=(const CPVT_WordPlace& wp) const {
    return *this < wp || *this == wp;
  }
  inline bool operator>=(const CPVT_WordPlace& wp) const {
    return *this > wp || *this == wp;
  }

  inline int32_t LineCmp(const CPVT_WordPlace& wp) const {
    if (nSecIndex != wp.nSecIndex)
      return nSecIndex - wp.nSecIndex;
    return nLineIndex - wp.nLineIndex;
  }

  int32_t nSecIndex;
  int32_t nLineIndex;
  int32_t nWordIndex;
};

#endif  // CORE_FPDFDOC_CPVT_WORDPLACE_H_
