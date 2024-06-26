// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_cssexttextbuf.h"

CFX_CSSExtTextBuf::CFX_CSSExtTextBuf()
    : m_pExtBuffer(nullptr), m_iDatLen(0), m_iDatPos(0) {}

CFX_CSSExtTextBuf::~CFX_CSSExtTextBuf() {}

void CFX_CSSExtTextBuf::AttachBuffer(const wchar_t* pBuffer, int32_t iBufLen) {
  m_pExtBuffer = pBuffer;
  m_iDatLen = iBufLen;
}
