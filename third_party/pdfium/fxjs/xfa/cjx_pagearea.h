// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_PAGEAREA_H_
#define FXJS_XFA_CJX_PAGEAREA_H_

#include "fxjs/xfa/cjx_container.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_PageArea;

class CJX_PageArea final : public CJX_Container {
 public:
  explicit CJX_PageArea(CXFA_PageArea* node);
  ~CJX_PageArea() override;
};

#endif  // FXJS_XFA_CJX_PAGEAREA_H_
