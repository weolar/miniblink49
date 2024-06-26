// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_LOADERCONTEXT_H_
#define XFA_FXFA_CXFA_LOADERCONTEXT_H_

#include <vector>

#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"

class CFX_CSSComputedStyle;
class CFX_XMLNode;
class CXFA_Node;

struct CXFA_LoaderContext {
  CXFA_LoaderContext();
  ~CXFA_LoaderContext();

  bool bSaveLineHeight = false;
  bool bFilterSpace = false;
  float fWidth = 0;
  float fHeight = 0;
  float fLastPos = 0;
  float fStartLineOffset = 0;
  int32_t iChar = 0;
  int32_t iTotalLines = -1;
  UnownedPtr<const CFX_XMLNode> pXMLNode;
  UnownedPtr<CXFA_Node> pNode;
  RetainPtr<CFX_CSSComputedStyle> pParentStyle;
  std::vector<float> lineHeights;
  std::vector<float> blocksHeight;
};

#endif  // XFA_FXFA_CXFA_LOADERCONTEXT_H_
