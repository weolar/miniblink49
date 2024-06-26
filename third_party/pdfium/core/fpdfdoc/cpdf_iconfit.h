// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_ICONFIT_H_
#define CORE_FPDFDOC_CPDF_ICONFIT_H_

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_Dictionary;

class CPDF_IconFit {
 public:
  enum ScaleMethod { Always = 0, Bigger, Smaller, Never };

  explicit CPDF_IconFit(const CPDF_Dictionary* pDict);
  CPDF_IconFit(const CPDF_IconFit& that);
  ~CPDF_IconFit();

  ScaleMethod GetScaleMethod() const;
  bool IsProportionalScale() const;
  CFX_PointF GetIconBottomLeftPosition() const;
  bool GetFittingBounds() const;
  const CPDF_Dictionary* GetDict() const { return m_pDict.Get(); }

 private:
  UnownedPtr<const CPDF_Dictionary> const m_pDict;
};

#endif  // CORE_FPDFDOC_CPDF_ICONFIT_H_
