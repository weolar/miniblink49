// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_CONTENT_H_
#define FXJS_XFA_CJX_CONTENT_H_

#include "fxjs/xfa/cjx_object.h"

class CXFA_Content;

class CJX_Content : public CJX_Object {
 public:
  explicit CJX_Content(CXFA_Object* obj);
  ~CJX_Content() override;
};

#endif  // FXJS_XFA_CJX_CONTENT_H_
