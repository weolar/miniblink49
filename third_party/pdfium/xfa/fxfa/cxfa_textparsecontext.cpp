// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_textparsecontext.h"

#include "core/fxcrt/css/cfx_csscomputedstyle.h"
#include "core/fxcrt/css/cfx_cssdeclaration.h"
#include "core/fxcrt/css/cfx_cssstyleselector.h"

CXFA_TextParseContext::CXFA_TextParseContext()
    : m_pParentStyle(nullptr), m_eDisplay(CFX_CSSDisplay::None) {}

CXFA_TextParseContext::~CXFA_TextParseContext() {}
