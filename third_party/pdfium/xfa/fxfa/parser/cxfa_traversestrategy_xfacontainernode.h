// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_XFACONTAINERNODE_H_
#define XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_XFACONTAINERNODE_H_

#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_nodeiteratortemplate.h"

class CXFA_TraverseStrategy_XFAContainerNode {
 public:
  static CXFA_Node* GetFirstChild(CXFA_Node* pTemplateNode) {
    return pTemplateNode->GetFirstContainerChild();
  }
  static CXFA_Node* GetNextSibling(CXFA_Node* pTemplateNode) {
    return pTemplateNode->GetNextContainerSibling();
  }
  static CXFA_Node* GetParent(CXFA_Node* pTemplateNode) {
    return pTemplateNode->GetContainerParent();
  }
};

using CXFA_ContainerIterator =
    CXFA_NodeIteratorTemplate<CXFA_Node,
                              CXFA_TraverseStrategy_XFAContainerNode>;

#endif  // XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_XFACONTAINERNODE_H_
