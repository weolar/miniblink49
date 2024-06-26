// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_nodeowner.h"

#include <utility>

#include "third_party/base/stl_util.h"
#include "xfa/fxfa/parser/cxfa_node.h"

CXFA_NodeOwner::CXFA_NodeOwner() = default;

CXFA_NodeOwner::~CXFA_NodeOwner() = default;

CXFA_Node* CXFA_NodeOwner::AddOwnedNode(std::unique_ptr<CXFA_Node> node) {
  if (!node)
    return nullptr;

  CXFA_Node* ret = node.get();
  nodes_.insert(std::move(node));
  return ret;
}

void CXFA_NodeOwner::FreeOwnedNode(CXFA_Node* node) {
  if (!node)
    return;

  pdfium::FakeUniquePtr<CXFA_Node> search(node);
  auto it = nodes_.find(search);
  ASSERT(it != nodes_.end());
  nodes_.erase(it);
}
