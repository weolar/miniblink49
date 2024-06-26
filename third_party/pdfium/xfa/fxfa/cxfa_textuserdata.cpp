// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_textuserdata.h"

#include "core/fxcrt/css/cfx_css.h"
#include "core/fxcrt/css/cfx_csscomputedstyle.h"
#include "core/fxcrt/css/cfx_cssstyleselector.h"
#include "xfa/fxfa/cxfa_linkuserdata.h"

CXFA_TextUserData::CXFA_TextUserData(
    const RetainPtr<CFX_CSSComputedStyle>& pStyle)
    : m_pStyle(pStyle) {}

CXFA_TextUserData::CXFA_TextUserData(
    const RetainPtr<CFX_CSSComputedStyle>& pStyle,
    const RetainPtr<CXFA_LinkUserData>& pLinkData)
    : m_pStyle(pStyle), m_pLinkData(pLinkData) {}

CXFA_TextUserData::~CXFA_TextUserData() {}
