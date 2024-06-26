// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_XFANODE_H_
#define XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_XFANODE_H_

#include "xfa/fxfa/parser/cxfa_nodeiteratortemplate.h"

class CXFA_TraverseStrategy_XFANode {
 public:
  static inline CXFA_Node* GetFirstChild(CXFA_Node* pTemplateNode) {
    return pTemplateNode->GetFirstChild();
  }
  static inline CXFA_Node* GetNextSibling(CXFA_Node* pTemplateNode) {
    return pTemplateNode->GetNextSibling();
  }
  static inline CXFA_Node* GetParent(CXFA_Node* pTemplateNode) {
    return pTemplateNode->GetParent();
  }
};

using CXFA_NodeIterator =
    CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFANode>;

#endif  // XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_XFANODE_H_
