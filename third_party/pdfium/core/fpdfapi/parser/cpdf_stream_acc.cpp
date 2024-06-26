// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_stream_acc.h"

#include <utility>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"

CPDF_StreamAcc::CPDF_StreamAcc(const CPDF_Stream* pStream)
    : m_pStream(pStream) {}

CPDF_StreamAcc::~CPDF_StreamAcc() = default;

void CPDF_StreamAcc::LoadAllData(bool bRawAccess,
                                 uint32_t estimated_size,
                                 bool bImageAcc) {
  if (bRawAccess) {
    ASSERT(!estimated_size);
    ASSERT(!bImageAcc);
  }

  if (!m_pStream)
    return;

  bool bProcessRawData = bRawAccess || !m_pStream->HasFilter();
  if (bProcessRawData)
    ProcessRawData();
  else
    ProcessFilteredData(estimated_size, bImageAcc);
}

void CPDF_StreamAcc::LoadAllDataFiltered() {
  LoadAllData(false, 0, false);
}

void CPDF_StreamAcc::LoadAllDataFilteredWithEstimatedSize(
    uint32_t estimated_size) {
  LoadAllData(false, estimated_size, false);
}

void CPDF_StreamAcc::LoadAllDataImageAcc(uint32_t estimated_size) {
  LoadAllData(false, estimated_size, true);
}

void CPDF_StreamAcc::LoadAllDataRaw() {
  LoadAllData(true, 0, false);
}

const CPDF_Dictionary* CPDF_StreamAcc::GetDict() const {
  return m_pStream ? m_pStream->GetDict() : nullptr;
}

uint8_t* CPDF_StreamAcc::GetData() const {
  if (m_pData.IsOwned())
    return m_pData.Get();
  return m_pStream ? m_pStream->GetInMemoryRawData() : nullptr;
}

uint32_t CPDF_StreamAcc::GetSize() const {
  if (m_pData.IsOwned())
    return m_dwSize;
  return (m_pStream && m_pStream->IsMemoryBased()) ? m_pStream->GetRawSize()
                                                   : 0;
}

std::unique_ptr<uint8_t, FxFreeDeleter> CPDF_StreamAcc::DetachData() {
  if (m_pData.IsOwned()) {
    std::unique_ptr<uint8_t, FxFreeDeleter> p = m_pData.ReleaseAndClear();
    m_dwSize = 0;
    return p;
  }
  std::unique_ptr<uint8_t, FxFreeDeleter> p(FX_Alloc(uint8_t, m_dwSize));
  memcpy(p.get(), m_pData.Get(), m_dwSize);
  return p;
}

void CPDF_StreamAcc::ProcessRawData() {
  uint32_t dwSrcSize = m_pStream->GetRawSize();
  if (dwSrcSize == 0)
    return;

  if (m_pStream->IsMemoryBased()) {
    m_pData = m_pStream->GetInMemoryRawData();
    m_dwSize = dwSrcSize;
    return;
  }

  std::unique_ptr<uint8_t, FxFreeDeleter> pData = ReadRawStream();
  if (!pData)
    return;

  m_pData = std::move(pData);
  m_dwSize = dwSrcSize;
}

void CPDF_StreamAcc::ProcessFilteredData(uint32_t estimated_size,
                                         bool bImageAcc) {
  uint32_t dwSrcSize = m_pStream->GetRawSize();
  if (dwSrcSize == 0)
    return;

  MaybeOwned<uint8_t, FxFreeDeleter> pSrcData;
  if (m_pStream->IsMemoryBased()) {
    pSrcData = m_pStream->GetInMemoryRawData();
  } else {
    std::unique_ptr<uint8_t, FxFreeDeleter> pTempSrcData = ReadRawStream();
    if (!pTempSrcData)
      return;

    pSrcData = std::move(pTempSrcData);
  }

  std::unique_ptr<uint8_t, FxFreeDeleter> pDecodedData;
  uint32_t dwDecodedSize = 0;
  if (!PDF_DataDecode({pSrcData.Get(), dwSrcSize}, m_pStream->GetDict(),
                      estimated_size, bImageAcc, &pDecodedData, &dwDecodedSize,
                      &m_ImageDecoder, &m_pImageParam)) {
    m_pData = std::move(pSrcData);
    m_dwSize = dwSrcSize;
    return;
  }

  if (pDecodedData) {
    ASSERT(pDecodedData.get() != pSrcData.Get());
    m_pData = std::move(pDecodedData);
    m_dwSize = dwDecodedSize;
  } else {
    m_pData = std::move(pSrcData);
    m_dwSize = dwSrcSize;
  }
}

std::unique_ptr<uint8_t, FxFreeDeleter> CPDF_StreamAcc::ReadRawStream() const {
  ASSERT(m_pStream);
  ASSERT(!m_pStream->IsMemoryBased());

  uint32_t dwSrcSize = m_pStream->GetRawSize();
  ASSERT(dwSrcSize);
  std::unique_ptr<uint8_t, FxFreeDeleter> pSrcData(
      FX_Alloc(uint8_t, dwSrcSize));
  if (!m_pStream->ReadRawData(0, pSrcData.get(), dwSrcSize))
    return nullptr;
  return pSrcData;
}
