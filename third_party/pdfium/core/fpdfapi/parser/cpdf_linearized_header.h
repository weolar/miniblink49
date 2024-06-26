// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_LINEARIZED_HEADER_H_
#define CORE_FPDFAPI_PARSER_CPDF_LINEARIZED_HEADER_H_

#include <memory>

#include "core/fxcrt/fx_stream.h"

class CPDF_Dictionary;
class CPDF_Object;
class CPDF_SyntaxParser;

class CPDF_LinearizedHeader {
 public:
  ~CPDF_LinearizedHeader();
  static std::unique_ptr<CPDF_LinearizedHeader> Parse(
      CPDF_SyntaxParser* parser);

  // Will only return values > 0.
  FX_FILESIZE GetFileSize() const { return m_szFileSize; }
  uint32_t GetFirstPageNo() const { return m_dwFirstPageNo; }
  // Will only return values > 0.
  FX_FILESIZE GetMainXRefTableFirstEntryOffset() const {
    return m_szMainXRefTableFirstEntryOffset;
  }
  uint32_t GetPageCount() const { return m_PageCount; }
  // Will only return values > 0.
  FX_FILESIZE GetFirstPageEndOffset() const { return m_szFirstPageEndOffset; }
  // Will only return values > 0.
  uint32_t GetFirstPageObjNum() const { return m_FirstPageObjNum; }
  // Will only return values > 0.
  FX_FILESIZE GetLastXRefOffset() const { return m_szLastXRefOffset; }

  bool HasHintTable() const;
  // Will only return values > 0.
  FX_FILESIZE GetHintStart() const { return m_szHintStart; }
  uint32_t GetHintLength() const { return m_HintLength; }

 protected:
  CPDF_LinearizedHeader(const CPDF_Dictionary* pDict,
                        FX_FILESIZE szLastXRefOffset);

 private:
  const FX_FILESIZE m_szFileSize;
  const uint32_t m_dwFirstPageNo;
  const FX_FILESIZE m_szMainXRefTableFirstEntryOffset;
  const uint32_t m_PageCount;
  const FX_FILESIZE m_szFirstPageEndOffset;
  const uint32_t m_FirstPageObjNum;
  const FX_FILESIZE m_szLastXRefOffset;
  FX_FILESIZE m_szHintStart = 0;
  uint32_t m_HintLength = 0;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_LINEARIZED_HEADER_H_
