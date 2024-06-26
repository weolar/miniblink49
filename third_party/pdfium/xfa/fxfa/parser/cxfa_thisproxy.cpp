// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_thisproxy.h"

#include "fxjs/xfa/cjx_object.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_node.h"

CXFA_ThisProxy::CXFA_ThisProxy(CXFA_Node* pThisNode, CXFA_Node* pScriptNode)
    : CXFA_Object(pThisNode->GetDocument(),
                  XFA_ObjectType::ThisProxy,
                  XFA_Element::Object,
                  pdfium::MakeUnique<CJX_Object>(this)),
      m_pThisNode(pThisNode),
      m_pScriptNode(pScriptNode) {}

CXFA_ThisProxy::~CXFA_ThisProxy() = default;
