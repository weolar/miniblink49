// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FORMFILLER_CFFL_TEXTOBJECT_H_
#define FPDFSDK_FORMFILLER_CFFL_TEXTOBJECT_H_

#include <memory>

#include "fpdfsdk/formfiller/cffl_formfiller.h"

// Class to implement common functionality for CFFL_FormFiller sub-classes with
// text fields.
class CFFL_TextObject : public CFFL_FormFiller {
 public:
  // CFFL_FormFiller:
  CPWL_Wnd* ResetPDFWindow(CPDFSDK_PageView* pPageView,
                           bool bRestoreValue) override;

 protected:
  CFFL_TextObject(CPDFSDK_FormFillEnvironment* pApp, CPDFSDK_Widget* pWidget);
  ~CFFL_TextObject() override;

  CBA_FontMap* MaybeCreateFontMap();

 private:
  std::unique_ptr<CBA_FontMap> m_pFontMap;
};

#endif  // FPDFSDK_FORMFILLER_CFFL_TEXTOBJECT_H_
