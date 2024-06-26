// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cfdf_document.h"

#include <memory>
#include <sstream>
#include <utility>

#include "core/fpdfapi/edit/cpdf_creator.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_syntax_parser.h"
#include "core/fpdfapi/parser/fpdf_parser_utility.h"
#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/span.h"

CFDF_Document::CFDF_Document() = default;

CFDF_Document::~CFDF_Document() = default;

std::unique_ptr<CFDF_Document> CFDF_Document::CreateNewDoc() {
  auto pDoc = pdfium::MakeUnique<CFDF_Document>();
  pDoc->m_pRootDict = pDoc->NewIndirect<CPDF_Dictionary>();
  pDoc->m_pRootDict->SetNewFor<CPDF_Dictionary>("FDF");
  return pDoc;
}

std::unique_ptr<CFDF_Document> CFDF_Document::ParseMemory(
    pdfium::span<const uint8_t> span) {
  auto pDoc = pdfium::MakeUnique<CFDF_Document>();
  pDoc->ParseStream(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(span));
  return pDoc->m_pRootDict ? std::move(pDoc) : nullptr;
}

void CFDF_Document::ParseStream(
    const RetainPtr<IFX_SeekableReadStream>& pFile) {
  m_pFile = pFile;
  CPDF_SyntaxParser parser(m_pFile);
  while (1) {
    bool bNumber;
    ByteString word = parser.GetNextWord(&bNumber);
    if (bNumber) {
      uint32_t objnum = FXSYS_atoui(word.c_str());
      if (!objnum)
        break;

      word = parser.GetNextWord(&bNumber);
      if (!bNumber)
        break;

      word = parser.GetNextWord(nullptr);
      if (word != "obj")
        break;

      std::unique_ptr<CPDF_Object> pObj = parser.GetObjectBody(this);
      if (!pObj)
        break;

      ReplaceIndirectObjectIfHigherGeneration(objnum, std::move(pObj));
      word = parser.GetNextWord(nullptr);
      if (word != "endobj")
        break;
    } else {
      if (word != "trailer")
        break;

      std::unique_ptr<CPDF_Dictionary> pMainDict =
          ToDictionary(parser.GetObjectBody(this));
      if (pMainDict)
        m_pRootDict = pMainDict->GetDictFor("Root");

      break;
    }
  }
}

ByteString CFDF_Document::WriteToString() const {
  if (!m_pRootDict)
    return ByteString();

  std::ostringstream buf;
  buf << "%FDF-1.2\r\n";
  for (const auto& pair : *this)
    buf << pair.first << " 0 obj\r\n"
        << pair.second.get() << "\r\nendobj\r\n\r\n";

  buf << "trailer\r\n<</Root " << m_pRootDict->GetObjNum()
      << " 0 R>>\r\n%%EOF\r\n";

  return ByteString(buf);
}
