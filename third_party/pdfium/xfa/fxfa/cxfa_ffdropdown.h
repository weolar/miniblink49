// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFDROPDOWN_H_
#define XFA_FXFA_CXFA_FFDROPDOWN_H_

#include "xfa/fxfa/cxfa_fffield.h"

class CXFA_FFDropDown : public CXFA_FFField {
 public:
  ~CXFA_FFDropDown() override;

  virtual void InsertItem(WideStringView wsLabel, int32_t nIndex) = 0;
  virtual void DeleteItem(int32_t nIndex) = 0;

 protected:
  explicit CXFA_FFDropDown(CXFA_Node* pNode);
};

#endif  // XFA_FXFA_CXFA_FFDROPDOWN_H_
