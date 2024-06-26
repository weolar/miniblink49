// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_DEST_H_
#define CORE_FPDFDOC_CPDF_DEST_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_Document;
class CPDF_Array;

class CPDF_Dest {
 public:
  CPDF_Dest();
  CPDF_Dest(const CPDF_Dest& that);
  explicit CPDF_Dest(const CPDF_Array* pArray);
  ~CPDF_Dest();

  const CPDF_Array* GetArray() const { return m_pArray.Get(); }
  ByteString GetRemoteName() const;
  int GetDestPageIndex(CPDF_Document* pDoc) const;

  // Returns the zoom mode, as one of the PDFDEST_VIEW_* values in fpdf_doc.h.
  int GetZoomMode() const;

  unsigned long GetNumParams() const;
  float GetParam(int index) const;

  bool GetXYZ(bool* pHasX,
              bool* pHasY,
              bool* pHasZoom,
              float* pX,
              float* pY,
              float* pZoom) const;

 private:
  UnownedPtr<const CPDF_Array> const m_pArray;
};

#endif  // CORE_FPDFDOC_CPDF_DEST_H_
