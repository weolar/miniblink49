// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_CFX_SUBSTFONT_H_
#define CORE_FXGE_CFX_SUBSTFONT_H_

#include "core/fxcrt/fx_string.h"

class CFX_SubstFont {
 public:
  CFX_SubstFont();

  ByteString m_Family;
  int m_Charset;
  int m_Weight;
  int m_ItalicAngle;
  int m_WeightCJK;
  bool m_bSubstCJK;
  bool m_bItalicCJK;

#ifdef PDF_ENABLE_XFA
  bool m_bFlagItalic;
#endif  // PDF_ENABLE_XFA
  bool m_bFlagMM;
};

#endif  // CORE_FXGE_CFX_SUBSTFONT_H_
