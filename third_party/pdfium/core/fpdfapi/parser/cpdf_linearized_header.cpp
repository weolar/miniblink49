// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_linearized_header.h"

#include <algorithm>
#include <limits>
#include <utility>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_syntax_parser.h"
#include "third_party/base/ptr_util.h"

namespace {

constexpr FX_FILESIZE kLinearizedHeaderOffset = 9;
constexpr size_t kMaxInt = static_cast<size_t>(std::numeric_limits<int>::max());

template <class T>
bool IsValidNumericDictionaryValue(const CPDF_Dictionary* pDict,
                                   const char* key,
                                   T min_value,
                                   bool must_exist = true) {
  if (!pDict->KeyExist(key))
    return !must_exist;
  const CPDF_Number* pNum = ToNumber(pDict->GetObjectFor(key));
  if (!pNum || !pNum->IsInteger())
    return false;
  const int raw_value = pNum->GetInteger();
  if (!pdfium::base::IsValueInRangeForNumericType<T>(raw_value))
    return false;
  return static_cast<T>(raw_value) >= min_value;
}

bool IsLinearizedHeaderValid(const CPDF_LinearizedHeader* header,
                             FX_FILESIZE document_size) {
  ASSERT(header);
  return header->GetFileSize() == document_size &&
         header->GetFirstPageNo() < kMaxInt &&
         header->GetFirstPageNo() < header->GetPageCount() &&
         header->GetMainXRefTableFirstEntryOffset() < document_size &&
         header->GetFirstPageEndOffset() < document_size &&
         header->GetLastXRefOffset() < document_size &&
         header->GetHintStart() < document_size;
}

}  // namespace

// static
std::unique_ptr<CPDF_LinearizedHeader> CPDF_LinearizedHeader::Parse(
    CPDF_SyntaxParser* parser) {
  parser->SetPos(kLinearizedHeaderOffset);

  const auto pDict = ToDictionary(
      parser->GetIndirectObject(nullptr, CPDF_SyntaxParser::ParseType::kLoose));

  if (!pDict || !pDict->KeyExist("Linearized") ||
      !IsValidNumericDictionaryValue<FX_FILESIZE>(pDict.get(), "L", 1) ||
      !IsValidNumericDictionaryValue<uint32_t>(pDict.get(), "P", 0, false) ||
      !IsValidNumericDictionaryValue<FX_FILESIZE>(pDict.get(), "T", 1) ||
      !IsValidNumericDictionaryValue<uint32_t>(pDict.get(), "N", 1) ||
      !IsValidNumericDictionaryValue<FX_FILESIZE>(pDict.get(), "E", 1) ||
      !IsValidNumericDictionaryValue<uint32_t>(pDict.get(), "O", 1)) {
    return nullptr;
  }
  // Move parser to the start of the xref table for the documents first page.
  // (skpping endobj keyword)
  if (parser->GetNextWord(nullptr) != "endobj")
    return nullptr;

  auto result = pdfium::WrapUnique(
      new CPDF_LinearizedHeader(pDict.get(), parser->GetPos()));

  if (!IsLinearizedHeaderValid(result.get(), parser->GetDocumentSize()))
    return nullptr;

  return result;
}

CPDF_LinearizedHeader::CPDF_LinearizedHeader(const CPDF_Dictionary* pDict,
                                             FX_FILESIZE szLastXRefOffset)
    : m_szFileSize(pDict->GetIntegerFor("L")),
      m_dwFirstPageNo(pDict->GetIntegerFor("P")),
      m_szMainXRefTableFirstEntryOffset(pDict->GetIntegerFor("T")),
      m_PageCount(pDict->GetIntegerFor("N")),
      m_szFirstPageEndOffset(pDict->GetIntegerFor("E")),
      m_FirstPageObjNum(pDict->GetIntegerFor("O")),
      m_szLastXRefOffset(szLastXRefOffset) {
  const CPDF_Array* pHintStreamRange = pDict->GetArrayFor("H");
  const size_t nHintStreamSize =
      pHintStreamRange ? pHintStreamRange->size() : 0;
  if (nHintStreamSize == 2 || nHintStreamSize == 4) {
    m_szHintStart = std::max(pHintStreamRange->GetIntegerAt(0), 0);
    const FX_SAFE_UINT32 safe_hint_length = pHintStreamRange->GetIntegerAt(1);
    if (safe_hint_length.IsValid())
      m_HintLength = safe_hint_length.ValueOrDie();
  }
}

CPDF_LinearizedHeader::~CPDF_LinearizedHeader() {}

bool CPDF_LinearizedHeader::HasHintTable() const {
  return GetPageCount() > 1 && GetHintStart() > 0 && GetHintLength() > 0;
}
