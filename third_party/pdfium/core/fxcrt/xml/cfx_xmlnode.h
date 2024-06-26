// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_XML_CFX_XMLNODE_H_
#define CORE_FXCRT_XML_CFX_XMLNODE_H_

#include "core/fxcrt/fx_stream.h"
#include "core/fxcrt/retain_ptr.h"

enum FX_XMLNODETYPE {
  FX_XMLNODE_Instruction = 0,
  FX_XMLNODE_Element,
  FX_XMLNODE_Text,
  FX_XMLNODE_CharData,
};

class CFX_XMLDocument;

class CFX_XMLNode {
 public:
  CFX_XMLNode();
  virtual ~CFX_XMLNode();

  virtual FX_XMLNODETYPE GetType() const = 0;
  virtual CFX_XMLNode* Clone(CFX_XMLDocument* doc) = 0;
  virtual void Save(const RetainPtr<IFX_SeekableWriteStream>& pXMLStream) = 0;

  CFX_XMLNode* GetRoot();
  CFX_XMLNode* GetParent() const { return parent_; }
  CFX_XMLNode* GetFirstChild() const { return first_child_; }
  CFX_XMLNode* GetNextSibling() const { return next_sibling_; }

  void AppendChild(CFX_XMLNode* pNode);
  void InsertChildNode(CFX_XMLNode* pNode, int32_t index);
  void RemoveChildNode(CFX_XMLNode* pNode);
  void DeleteChildren();

  CFX_XMLNode* GetLastChildForTesting() const { return last_child_; }
  CFX_XMLNode* GetPrevSiblingForTesting() const { return prev_sibling_; }

 protected:
  WideString EncodeEntities(const WideString& value);

 private:
  // The nodes are owned by the XML document. We do not know what order the
  // nodes will be destroyed in so they can not be UnownedPtrs.
  CFX_XMLNode* parent_ = nullptr;
  CFX_XMLNode* first_child_ = nullptr;
  CFX_XMLNode* last_child_ = nullptr;
  CFX_XMLNode* next_sibling_ = nullptr;
  CFX_XMLNode* prev_sibling_ = nullptr;
};

#endif  // CORE_FXCRT_XML_CFX_XMLNODE_H_
