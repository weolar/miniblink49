// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSTEXTBUF_H_
#define CORE_FXCRT_CSS_CFX_CSSTEXTBUF_H_

#include "core/fxcrt/fx_system.h"

class CFX_CSSTextBuf {
 public:
  CFX_CSSTextBuf();
  ~CFX_CSSTextBuf();

  void InitWithSize(int32_t iAllocSize);
  void AppendChar(wchar_t wch);

  void Clear() { m_iDatLen = 0; }

  int32_t TrimEnd();

  int32_t GetLength() const { return m_iDatLen; }
  const wchar_t* GetBuffer() const { return m_pBuffer; }

 protected:
  void ExpandBuf(int32_t iDesiredSize);

  wchar_t* m_pBuffer;
  int32_t m_iBufLen;
  int32_t m_iDatLen;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSTEXTBUF_H_
