// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFNUMERICEDIT_H_
#define XFA_FXFA_CXFA_FFNUMERICEDIT_H_

#include "core/fxcrt/fx_string.h"
#include "xfa/fxfa/cxfa_fftextedit.h"

class CFWL_Event;
class CFWL_Widget;

class CXFA_FFNumericEdit final : public CXFA_FFTextEdit {
 public:
  explicit CXFA_FFNumericEdit(CXFA_Node* pNode);
  ~CXFA_FFNumericEdit() override;

  // CXFA_FFTextEdit
  bool LoadWidget() override;
  void UpdateWidgetProperty() override;
  void OnProcessEvent(CFWL_Event* pEvent) override;

 private:
  bool OnValidate(CFWL_Widget* pWidget, WideString& wsText);
};

#endif  // XFA_FXFA_CXFA_FFNUMERICEDIT_H_
