// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXGRAPHICS_CXFA_GECOLOR_H_
#define XFA_FXGRAPHICS_CXFA_GECOLOR_H_

#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/fx_dib.h"

class CXFA_GEPattern;
class CXFA_GEShading;

class CXFA_GEColor {
 public:
  enum Type { Invalid, Solid, Pattern, Shading };

  CXFA_GEColor();
  explicit CXFA_GEColor(const FX_ARGB argb);
  explicit CXFA_GEColor(CXFA_GEShading* shading);
  CXFA_GEColor(CXFA_GEPattern* pattern, const FX_ARGB argb);
  CXFA_GEColor(const CXFA_GEColor& that);
  ~CXFA_GEColor();

  Type GetType() const { return m_type; }
  FX_ARGB GetArgb() const {
    ASSERT(m_type == Solid || m_type == Pattern);
    return m_argb;
  }
  CXFA_GEPattern* GetPattern() const {
    ASSERT(m_type == Pattern);
    return m_pPattern.Get();
  }
  CXFA_GEShading* GetShading() const {
    ASSERT(m_type == Shading);
    return m_pShading.Get();
  }

  CXFA_GEColor& operator=(const CXFA_GEColor& that);

 private:
  Type m_type = Invalid;
  FX_ARGB m_argb = 0;
  UnownedPtr<CXFA_GEPattern> m_pPattern;
  UnownedPtr<CXFA_GEShading> m_pShading;
};

#endif  // XFA_FXGRAPHICS_CXFA_GECOLOR_H_
