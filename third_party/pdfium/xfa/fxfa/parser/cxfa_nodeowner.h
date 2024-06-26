// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_NODEOWNER_H_
#define XFA_FXFA_PARSER_CXFA_NODEOWNER_H_

#include <memory>
#include <set>

class CXFA_Node;

class CXFA_NodeOwner {
 public:
  virtual ~CXFA_NodeOwner();

  CXFA_Node* AddOwnedNode(std::unique_ptr<CXFA_Node> node);
  void FreeOwnedNode(CXFA_Node* node);

 protected:
  CXFA_NodeOwner();

  std::set<std::unique_ptr<CXFA_Node>> nodes_;
};

#endif  // XFA_FXFA_PARSER_CXFA_NODEOWNER_H_
