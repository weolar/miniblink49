// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_THISPROXY_H_
#define XFA_FXFA_PARSER_CXFA_THISPROXY_H_

#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/parser/cxfa_object.h"

class CXFA_Node;

class CXFA_ThisProxy final : public CXFA_Object {
 public:
  CXFA_ThisProxy(CXFA_Node* pThisNode, CXFA_Node* pScriptNode);
  ~CXFA_ThisProxy() override;

  CXFA_Node* GetThisNode() const { return m_pThisNode.Get(); }
  CXFA_Node* GetScriptNode() const { return m_pScriptNode.Get(); }

 private:
  UnownedPtr<CXFA_Node> m_pThisNode;
  UnownedPtr<CXFA_Node> m_pScriptNode;
};

#endif  // XFA_FXFA_PARSER_CXFA_THISPROXY_H_
