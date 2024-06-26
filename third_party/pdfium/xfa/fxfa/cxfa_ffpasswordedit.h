// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFPASSWORDEDIT_H_
#define XFA_FXFA_CXFA_FFPASSWORDEDIT_H_

#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/cxfa_fftextedit.h"

class CXFA_PasswordEdit;

class CXFA_FFPasswordEdit final : public CXFA_FFTextEdit {
 public:
  CXFA_FFPasswordEdit(CXFA_Node* pNode, CXFA_PasswordEdit* password_node);
  ~CXFA_FFPasswordEdit() override;

  // CXFA_FFTextEdit
  bool LoadWidget() override;
  void UpdateWidgetProperty() override;

 private:
  UnownedPtr<CXFA_PasswordEdit> password_node_;
};

#endif  // XFA_FXFA_CXFA_FFPASSWORDEDIT_H_
