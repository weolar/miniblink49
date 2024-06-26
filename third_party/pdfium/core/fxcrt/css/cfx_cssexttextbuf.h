// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSEXTTEXTBUF_H_
#define CORE_FXCRT_CSS_CFX_CSSEXTTEXTBUF_H_

#include "core/fxcrt/fx_system.h"

class CFX_CSSExtTextBuf {
 public:
  CFX_CSSExtTextBuf();
  ~CFX_CSSExtTextBuf();

  void AttachBuffer(const wchar_t* pBuffer, int32_t iBufLen);

  bool IsEOF() const { return m_iDatPos >= m_iDatLen; }

  wchar_t GetChar() const { return m_pExtBuffer[m_iDatPos]; }
  wchar_t GetNextChar() const {
    return (m_iDatPos + 1 >= m_iDatLen) ? 0 : m_pExtBuffer[m_iDatPos + 1];
  }

  void MoveNext() { m_iDatPos++; }

 protected:
  const wchar_t* m_pExtBuffer;
  int32_t m_iDatLen;
  int32_t m_iDatPos;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSEXTTEXTBUF_H_
