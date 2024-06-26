// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_metadata.h"

#include <memory>

#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmlparser.h"

namespace {

void CheckForSharedFormInternal(CFX_XMLElement* element,
                                std::vector<UnsupportedFeature>* unsupported) {
  WideString attr =
      element->GetAttribute(WideString::FromASCII("xmlns:adhocwf"));
  if (attr.EqualsASCII("http://ns.adobe.com/AcrobatAdhocWorkflow/1.0/")) {
    for (const auto* child = element->GetFirstChild(); child;
         child = child->GetNextSibling()) {
      if (child->GetType() != FX_XMLNODE_Element)
        continue;

      const auto* child_elem = static_cast<const CFX_XMLElement*>(child);
      if (!child_elem->GetName().EqualsASCII("adhocwf:workflowType"))
        continue;

      switch (child_elem->GetTextData().GetInteger()) {
        case 0:
          unsupported->push_back(UnsupportedFeature::kDocumentSharedFormEmail);
          break;
        case 1:
          unsupported->push_back(
              UnsupportedFeature::kDocumentSharedFormAcrobat);
          break;
        case 2:
          unsupported->push_back(
              UnsupportedFeature::kDocumentSharedFormFilesystem);
          break;
      }
      // We only care about the first one we find.
      break;
    }
  }

  for (auto* child = element->GetFirstChild(); child;
       child = child->GetNextSibling()) {
    CFX_XMLElement* pElement = ToXMLElement(child);
    if (pElement)
      CheckForSharedFormInternal(pElement, unsupported);
  }
}

}  // namespace

CPDF_Metadata::CPDF_Metadata(const CPDF_Stream* pStream) : stream_(pStream) {
  ASSERT(pStream);
}

CPDF_Metadata::~CPDF_Metadata() = default;

std::vector<UnsupportedFeature> CPDF_Metadata::CheckForSharedForm() const {
  auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(stream_.Get());
  pAcc->LoadAllDataFiltered();

  auto stream = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(pAcc->GetSpan());
  CFX_XMLParser parser(stream);
  std::unique_ptr<CFX_XMLDocument> doc = parser.Parse();
  if (!doc)
    return {};

  std::vector<UnsupportedFeature> unsupported;
  CheckForSharedFormInternal(doc->GetRoot(), &unsupported);
  return unsupported;
}
