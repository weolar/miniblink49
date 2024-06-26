// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_LAYOUTCONTEXT_H_
#define XFA_FXFA_PARSER_CXFA_LAYOUTCONTEXT_H_

#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "third_party/base/optional.h"

class CXFA_ItemLayoutProcessor;
class CXFA_Node;

class CXFA_LayoutContext {
 public:
  CXFA_LayoutContext();
  ~CXFA_LayoutContext();

  Optional<float> m_fCurColumnWidth;
  UnownedPtr<std::vector<float>> m_prgSpecifiedColumnWidths;
  UnownedPtr<CXFA_ItemLayoutProcessor> m_pOverflowProcessor;
  UnownedPtr<CXFA_Node> m_pOverflowNode;
};

#endif  // XFA_FXFA_PARSER_CXFA_LAYOUTCONTEXT_H_
