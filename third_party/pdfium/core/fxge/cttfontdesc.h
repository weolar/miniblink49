// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_CTTFONTDESC_H_
#define CORE_FXGE_CTTFONTDESC_H_

#include <memory>

#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxge/fx_font.h"

class CTTFontDesc {
 public:
  enum ReleaseStatus {
    kNotAppropriate,  // ReleaseFace() not appropriate for given object.
    kReleased,
    kNotReleased  // Object still alive.
  };

  explicit CTTFontDesc(std::unique_ptr<uint8_t, FxFreeDeleter> pData);
  ~CTTFontDesc();

  void SetFace(size_t index, FXFT_Face face);

  void AddRef();

  // May not decrement refcount, depending on the value of |face|.
  ReleaseStatus ReleaseFace(FXFT_Face face);

  uint8_t* FontData() const { return m_pFontData.get(); }
  FXFT_Face GetFace(size_t index) const;

 private:
  int m_RefCount = 1;
  std::unique_ptr<uint8_t, FxFreeDeleter> const m_pFontData;
  FXFT_Face m_TTCFaces[16];
};

#endif  // CORE_FXGE_CTTFONTDESC_H_
