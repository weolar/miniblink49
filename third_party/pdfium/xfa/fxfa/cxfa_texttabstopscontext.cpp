// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_texttabstopscontext.h"

#include <algorithm>

CXFA_TextTabstopsContext::CXFA_TextTabstopsContext()
    : m_iTabIndex(-1), m_bTabstops(false), m_fTabWidth(0), m_fLeft(0) {}

CXFA_TextTabstopsContext::~CXFA_TextTabstopsContext() {}

void CXFA_TextTabstopsContext::Append(uint32_t dwAlign, float fTabstops) {
  XFA_TABSTOPS tabstop;
  tabstop.dwAlign = dwAlign;
  tabstop.fTabstops = fTabstops;

  auto it = std::lower_bound(m_tabstops.begin(), m_tabstops.end(), tabstop);
  m_tabstops.insert(it, tabstop);
}

void CXFA_TextTabstopsContext::RemoveAll() {
  m_tabstops.clear();
}

void CXFA_TextTabstopsContext::Reset() {
  m_iTabIndex = -1;
  m_bTabstops = false;
  m_fTabWidth = 0;
  m_fLeft = 0;
}
