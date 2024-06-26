// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_STREAM_ACC_H_
#define CORE_FPDFAPI_PARSER_CPDF_STREAM_ACC_H_

#include <memory>

#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/maybe_owned.h"
#include "core/fxcrt/retain_ptr.h"
#include "third_party/base/span.h"

class CPDF_Dictionary;

class CPDF_StreamAcc final : public Retainable {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  CPDF_StreamAcc(const CPDF_StreamAcc&) = delete;
  CPDF_StreamAcc& operator=(const CPDF_StreamAcc&) = delete;

  void LoadAllDataFiltered();
  void LoadAllDataFilteredWithEstimatedSize(uint32_t estimated_size);
  void LoadAllDataImageAcc(uint32_t estimated_size);
  void LoadAllDataRaw();

  const CPDF_Stream* GetStream() const { return m_pStream.Get(); }
  const CPDF_Dictionary* GetDict() const;

  uint8_t* GetData() const;
  uint32_t GetSize() const;
  pdfium::span<uint8_t> GetSpan() const {
    return pdfium::make_span(GetData(), GetSize());
  }
  ByteString GetImageDecoder() const { return m_ImageDecoder; }
  const CPDF_Dictionary* GetImageParam() const { return m_pImageParam.Get(); }
  std::unique_ptr<uint8_t, FxFreeDeleter> DetachData();

 private:
  explicit CPDF_StreamAcc(const CPDF_Stream* pStream);
  ~CPDF_StreamAcc() override;

  void LoadAllData(bool bRawAccess, uint32_t estimated_size, bool bImageAcc);

  void ProcessRawData();
  void ProcessFilteredData(uint32_t estimated_size, bool bImageAcc);

  // Reads the raw data from |m_pStream|, or return nullptr on failure.
  std::unique_ptr<uint8_t, FxFreeDeleter> ReadRawStream() const;

  MaybeOwned<uint8_t, FxFreeDeleter> m_pData;
  uint32_t m_dwSize = 0;
  ByteString m_ImageDecoder;
  UnownedPtr<const CPDF_Dictionary> m_pImageParam;
  UnownedPtr<const CPDF_Stream> const m_pStream;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_STREAM_ACC_H_
