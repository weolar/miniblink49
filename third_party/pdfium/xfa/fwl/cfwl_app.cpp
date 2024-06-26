// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_app.h"

#include "third_party/base/ptr_util.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/cfwl_widgetmgr.h"
#include "xfa/fxfa/cxfa_fwladapterwidgetmgr.h"

CFWL_App::CFWL_App(CXFA_FFApp* pAdapter)
    : m_pAdapterNative(pAdapter),
      m_pWidgetMgr(pdfium::MakeUnique<CFWL_WidgetMgr>(pAdapter)),
      m_pNoteDriver(pdfium::MakeUnique<CFWL_NoteDriver>()) {
  ASSERT(m_pAdapterNative);
}

CFWL_App::~CFWL_App() = default;
