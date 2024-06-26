// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/edit/cpdf_flateencoder.h"

#include <memory>
#include <utility>

#include "constants/stream_dict_common.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"

CPDF_FlateEncoder::CPDF_FlateEncoder(const CPDF_Stream* pStream,
                                     bool bFlateEncode)
    : m_pAcc(pdfium::MakeRetain<CPDF_StreamAcc>(pStream)), m_dwSize(0) {
  m_pAcc->LoadAllDataRaw();

  bool bHasFilter = pStream && pStream->HasFilter();
  if (bHasFilter && !bFlateEncode) {
    auto pDestAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pStream);
    pDestAcc->LoadAllDataFiltered();

    m_dwSize = pDestAcc->GetSize();
    m_pData = pDestAcc->DetachData();
    m_pClonedDict = ToDictionary(pStream->GetDict()->Clone());
    m_pClonedDict->RemoveFor("Filter");
    ASSERT(!m_pDict);
    return;
  }
  if (bHasFilter || !bFlateEncode) {
    m_pData = m_pAcc->GetData();
    m_dwSize = m_pAcc->GetSize();
    m_pDict = pStream->GetDict();
    ASSERT(!m_pClonedDict);
    return;
  }

  // TODO(thestig): Move to Init() and check return value.
  std::unique_ptr<uint8_t, FxFreeDeleter> buffer;
  ::FlateEncode(m_pAcc->GetSpan(), &buffer, &m_dwSize);

  m_pData = std::move(buffer);
  m_pClonedDict = ToDictionary(pStream->GetDict()->Clone());
  m_pClonedDict->SetNewFor<CPDF_Number>("Length", static_cast<int>(m_dwSize));
  m_pClonedDict->SetNewFor<CPDF_Name>("Filter", "FlateDecode");
  m_pClonedDict->RemoveFor(pdfium::stream::kDecodeParms);
  ASSERT(!m_pDict);
}

CPDF_FlateEncoder::~CPDF_FlateEncoder() {}

void CPDF_FlateEncoder::CloneDict() {
  if (m_pClonedDict) {
    ASSERT(!m_pDict);
    return;
  }

  m_pClonedDict = ToDictionary(m_pDict->Clone());
  ASSERT(m_pClonedDict);
  m_pDict.Release();
}

CPDF_Dictionary* CPDF_FlateEncoder::GetClonedDict() {
  ASSERT(!m_pDict);
  return m_pClonedDict.get();
}

const CPDF_Dictionary* CPDF_FlateEncoder::GetDict() const {
  if (m_pClonedDict) {
    ASSERT(!m_pDict);
    return m_pClonedDict.get();
  }

  return m_pDict.Get();
}
