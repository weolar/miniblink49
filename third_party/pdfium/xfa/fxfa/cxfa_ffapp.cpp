// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_ffapp.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "third_party/base/ptr_util.h"
#include "xfa/fgas/font/cfgas_fontmgr.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_widgetmgr.h"
#include "xfa/fwl/ifwl_adaptertimermgr.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffwidgethandler.h"
#include "xfa/fxfa/cxfa_fontmgr.h"
#include "xfa/fxfa/cxfa_fwladapterwidgetmgr.h"
#include "xfa/fxfa/cxfa_fwltheme.h"

namespace {

bool g_skipFontLoadForTesting = false;

}  // namespace

// static
void CXFA_FFApp::SkipFontLoadForTesting(bool skip) {
  g_skipFontLoadForTesting = skip;
}

CXFA_FFApp::CXFA_FFApp(IXFA_AppProvider* pProvider) : m_pProvider(pProvider) {
  // Ensure fully initialized before making an app based on |this|.
  m_pFWLApp = pdfium::MakeUnique<CFWL_App>(this);
}

CXFA_FFApp::~CXFA_FFApp() {}

CFGAS_FontMgr* CXFA_FFApp::GetFDEFontMgr() {
  if (!m_pFDEFontMgr) {
    m_pFDEFontMgr = pdfium::MakeUnique<CFGAS_FontMgr>();
    if (!g_skipFontLoadForTesting) {
      if (!m_pFDEFontMgr->EnumFonts())
        m_pFDEFontMgr = nullptr;
    }
  }
  return m_pFDEFontMgr.get();
}

CXFA_FWLTheme* CXFA_FFApp::GetFWLTheme(CXFA_FFDoc* doc) {
  if (!m_pFWLTheme) {
    auto fwl_theme = pdfium::MakeUnique<CXFA_FWLTheme>(this);
    if (fwl_theme->LoadCalendarFont(doc))
      m_pFWLTheme = std::move(fwl_theme);
  }
  return m_pFWLTheme.get();
}

CXFA_FWLAdapterWidgetMgr* CXFA_FFApp::GetFWLAdapterWidgetMgr() {
  if (!m_pAdapterWidgetMgr)
    m_pAdapterWidgetMgr = pdfium::MakeUnique<CXFA_FWLAdapterWidgetMgr>();
  return m_pAdapterWidgetMgr.get();
}

std::unique_ptr<IFWL_AdapterTimerMgr> CXFA_FFApp::NewTimerMgr() const {
  return m_pProvider->NewTimerMgr();
}

void CXFA_FFApp::ClearEventTargets() {
  m_pFWLApp->GetNoteDriver()->ClearEventTargets();
}
