// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/xml/cfx_xmlchardata.h"

#include "core/fxcrt/xml/cfx_xmldocument.h"

CFX_XMLCharData::CFX_XMLCharData(const WideString& wsCData)
    : CFX_XMLText(wsCData) {}

CFX_XMLCharData::~CFX_XMLCharData() = default;

FX_XMLNODETYPE CFX_XMLCharData::GetType() const {
  return FX_XMLNODE_CharData;
}

CFX_XMLNode* CFX_XMLCharData::Clone(CFX_XMLDocument* doc) {
  return doc->CreateNode<CFX_XMLCharData>(GetText());
}

void CFX_XMLCharData::Save(
    const RetainPtr<IFX_SeekableWriteStream>& pXMLStream) {
  pXMLStream->WriteString("<![CDATA[");
  pXMLStream->WriteString(GetText().ToUTF8().AsStringView());
  pXMLStream->WriteString("]]>");
}
