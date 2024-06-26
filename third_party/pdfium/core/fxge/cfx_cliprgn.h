// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_CFX_CLIPRGN_H_
#define CORE_FXGE_CFX_CLIPRGN_H_

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/retain_ptr.h"

class CFX_DIBitmap;

class CFX_ClipRgn {
 public:
  enum ClipType { RectI, MaskF };

  CFX_ClipRgn(int device_width, int device_height);
  CFX_ClipRgn(const CFX_ClipRgn& src);
  ~CFX_ClipRgn();

  ClipType GetType() const { return m_Type; }
  const FX_RECT& GetBox() const { return m_Box; }
  RetainPtr<CFX_DIBitmap> GetMask() const { return m_Mask; }

  void IntersectRect(const FX_RECT& rect);
  void IntersectMaskF(int left, int top, const RetainPtr<CFX_DIBitmap>& Mask);

 private:
  void IntersectMaskRect(FX_RECT rect,
                         FX_RECT mask_box,
                         const RetainPtr<CFX_DIBitmap>& Mask);

  ClipType m_Type;
  FX_RECT m_Box;
  RetainPtr<CFX_DIBitmap> m_Mask;
};

#endif  // CORE_FXGE_CFX_CLIPRGN_H_
