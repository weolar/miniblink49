// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/formfiller/cffl_textobject.h"
#include "third_party/base/ptr_util.h"

CPWL_Wnd* CFFL_TextObject::ResetPDFWindow(CPDFSDK_PageView* pPageView,
                                          bool bRestoreValue) {
  if (bRestoreValue)
    SaveState(pPageView);

  DestroyPDFWindow(pPageView);
  if (bRestoreValue)
    RestoreState(pPageView);

  CPWL_Wnd::ObservedPtr pRet(GetPDFWindow(pPageView, !bRestoreValue));
  m_pWidget->UpdateField();  // May invoke JS, invalidating |pRet|.
  return pRet.Get();
}

CFFL_TextObject::CFFL_TextObject(CPDFSDK_FormFillEnvironment* pApp,
                                 CPDFSDK_Widget* pWidget)
    : CFFL_FormFiller(pApp, pWidget) {}

CFFL_TextObject::~CFFL_TextObject() {
  // Destroy view classes before this object's members are destroyed since
  // the view classes have pointers to m_pFontMap that would be left dangling.
  DestroyWindows();
}

CBA_FontMap* CFFL_TextObject::MaybeCreateFontMap() {
  if (!m_pFontMap) {
    m_pFontMap =
        pdfium::MakeUnique<CBA_FontMap>(m_pWidget.Get(), GetSystemHandler());
  }
  return m_pFontMap.get();
}
