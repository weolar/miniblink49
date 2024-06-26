// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/xml/cfx_xmlnode.h"

#include <utility>
#include <vector>

#include "core/fxcrt/xml/cfx_xmlchardata.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmlinstruction.h"
#include "core/fxcrt/xml/cfx_xmltext.h"

CFX_XMLNode::CFX_XMLNode() = default;

CFX_XMLNode::~CFX_XMLNode() = default;

void CFX_XMLNode::DeleteChildren() {
  while (first_child_) {
    first_child_->parent_ = nullptr;
    first_child_->prev_sibling_ = nullptr;

    CFX_XMLNode* child = first_child_;
    first_child_ = child->next_sibling_;

    child->next_sibling_ = nullptr;
  }
  last_child_ = nullptr;
}

void CFX_XMLNode::AppendChild(CFX_XMLNode* pNode) {
  InsertChildNode(pNode, -1);
}

void CFX_XMLNode::InsertChildNode(CFX_XMLNode* pNode, int32_t index) {
  ASSERT(!pNode->parent_);

  pNode->parent_ = this;
  // No existing children, add node as first child.
  if (!first_child_) {
    ASSERT(!last_child_);

    first_child_ = pNode;
    last_child_ = first_child_;
    first_child_->prev_sibling_ = nullptr;
    first_child_->next_sibling_ = nullptr;
    return;
  }

  if (index == 0) {
    first_child_->prev_sibling_ = pNode;
    pNode->next_sibling_ = first_child_;
    pNode->prev_sibling_ = nullptr;
    first_child_ = pNode;
    return;
  }

  CFX_XMLNode* pFind;
  // Note, negative indexes, and indexes after the end of the list will result
  // in appending to the list.
  if (index < 0) {
    // Optimize the negative index case.
    pFind = last_child_;
  } else {
    pFind = first_child_;
    int32_t iCount = 0;
    while (++iCount != index && pFind->next_sibling_)
      pFind = pFind->next_sibling_;
  }

  pNode->prev_sibling_ = pFind;
  if (pFind->next_sibling_)
    pFind->next_sibling_->prev_sibling_ = pNode;
  pNode->next_sibling_ = pFind->next_sibling_;

  pFind->next_sibling_ = pNode;
  if (pFind == last_child_)
    last_child_ = pFind->next_sibling_;
}

void CFX_XMLNode::RemoveChildNode(CFX_XMLNode* pNode) {
  ASSERT(first_child_);
  ASSERT(pNode);

  if (pNode->GetParent() != this)
    return;

  if (first_child_ == pNode)
    first_child_ = pNode->next_sibling_;
  if (last_child_ == pNode)
    last_child_ = pNode->prev_sibling_;

  if (pNode->prev_sibling_)
    pNode->prev_sibling_->next_sibling_ = pNode->next_sibling_;
  if (pNode->next_sibling_)
    pNode->next_sibling_->prev_sibling_ = pNode->prev_sibling_;

  pNode->parent_ = nullptr;
  pNode->prev_sibling_ = nullptr;
  pNode->next_sibling_ = nullptr;
}

CFX_XMLNode* CFX_XMLNode::GetRoot() {
  CFX_XMLNode* pParent = this;
  while (pParent->parent_)
    pParent = pParent->parent_;

  return pParent;
}

WideString CFX_XMLNode::EncodeEntities(const WideString& value) {
  WideString ret = value;
  ret.Replace(L"&", L"&amp;");
  ret.Replace(L"<", L"&lt;");
  ret.Replace(L">", L"&gt;");
  ret.Replace(L"\'", L"&apos;");
  ret.Replace(L"\"", L"&quot;");
  return ret;
}
