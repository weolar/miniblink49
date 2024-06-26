// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_TEXTTABSTOPSCONTEXT_H_
#define XFA_FXFA_CXFA_TEXTTABSTOPSCONTEXT_H_

#include <stdint.h>

#include <vector>

struct XFA_TABSTOPS {
  uint32_t dwAlign;
  float fTabstops;

  bool operator<(const XFA_TABSTOPS& that) const {
    return fTabstops < that.fTabstops;
  }
};

class CXFA_TextTabstopsContext {
 public:
  CXFA_TextTabstopsContext();
  ~CXFA_TextTabstopsContext();

  void Append(uint32_t dwAlign, float fTabstops);
  void RemoveAll();
  void Reset();

  int32_t m_iTabIndex;
  bool m_bTabstops;
  float m_fTabWidth;
  float m_fLeft;
  std::vector<XFA_TABSTOPS> m_tabstops;
};

#endif  // XFA_FXFA_CXFA_TEXTTABSTOPSCONTEXT_H_
