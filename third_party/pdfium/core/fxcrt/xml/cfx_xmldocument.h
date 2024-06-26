// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FXCRT_XML_CFX_XMLDOCUMENT_H_
#define CORE_FXCRT_XML_CFX_XMLDOCUMENT_H_

#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "third_party/base/ptr_util.h"

class CFX_XMLNode;

class CFX_XMLDocument {
 public:
  CFX_XMLDocument();
  ~CFX_XMLDocument();

  CFX_XMLElement* GetRoot() const { return root_.Get(); }

  template <typename T, typename... Args>
  T* CreateNode(Args&&... args) {
    nodes_.push_back(pdfium::MakeUnique<T>(std::forward<Args>(args)...));
    return static_cast<T*>(nodes_.back().get());
  }

  // Transfers ownership of entries in |nodes_| from |other| to |this|.
  // This is used in CJX_Node::loadXML to transfer ownership of the newly
  // created nodes to the top-level XML doc for the PDF, after parsing an XML
  // blob.
  void AppendNodesFrom(CFX_XMLDocument* other);

 private:
  std::vector<std::unique_ptr<CFX_XMLNode>> nodes_;
  UnownedPtr<CFX_XMLElement> root_;
};

#endif  // CORE_FXCRT_XML_CFX_XMLDOCUMENT_H_
