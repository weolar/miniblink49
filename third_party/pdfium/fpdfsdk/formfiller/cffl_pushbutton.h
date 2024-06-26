// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FORMFILLER_CFFL_PUSHBUTTON_H_
#define FPDFSDK_FORMFILLER_CFFL_PUSHBUTTON_H_

#include <memory>

#include "fpdfsdk/formfiller/cffl_button.h"

class CFFL_PushButton final : public CFFL_Button {
 public:
  CFFL_PushButton(CPDFSDK_FormFillEnvironment* pApp, CPDFSDK_Widget* pWidget);
  ~CFFL_PushButton() override;

  // CFFL_Button:
  std::unique_ptr<CPWL_Wnd> NewPWLWindow(
      const CPWL_Wnd::CreateParams& cp,
      std::unique_ptr<CPWL_Wnd::PrivateData> pAttachedData) override;
};

#endif  // FPDFSDK_FORMFILLER_CFFL_PUSHBUTTON_H_
