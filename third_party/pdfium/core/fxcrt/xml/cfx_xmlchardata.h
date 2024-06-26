// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_XML_CFX_XMLCHARDATA_H_
#define CORE_FXCRT_XML_CFX_XMLCHARDATA_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/xml/cfx_xmltext.h"

class CFX_XMLDocument;

class CFX_XMLCharData final : public CFX_XMLText {
 public:
  explicit CFX_XMLCharData(const WideString& wsCData);
  ~CFX_XMLCharData() override;

  // CFX_XMLNode
  FX_XMLNODETYPE GetType() const override;
  CFX_XMLNode* Clone(CFX_XMLDocument* doc) override;
  void Save(const RetainPtr<IFX_SeekableWriteStream>& pXMLStream) override;
};

inline CFX_XMLCharData* ToXMLCharData(CFX_XMLNode* pNode) {
  return pNode && pNode->GetType() == FX_XMLNODE_CharData
             ? static_cast<CFX_XMLCharData*>(pNode)
             : nullptr;
}

#endif  // CORE_FXCRT_XML_CFX_XMLCHARDATA_H_
