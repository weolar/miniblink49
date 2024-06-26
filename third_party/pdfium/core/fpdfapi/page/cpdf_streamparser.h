// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_STREAMPARSER_H_
#define CORE_FPDFAPI_PAGE_CPDF_STREAMPARSER_H_

#include <memory>
#include <utility>

#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fxcrt/string_pool_template.h"
#include "core/fxcrt/weak_ptr.h"
#include "third_party/base/span.h"

class CPDF_Dictionary;

class CPDF_StreamParser {
 public:
  enum SyntaxType { EndOfData, Number, Keyword, Name, Others };

  explicit CPDF_StreamParser(pdfium::span<const uint8_t> span);
  CPDF_StreamParser(pdfium::span<const uint8_t> span,
                    const WeakPtr<ByteStringPool>& pPool);
  ~CPDF_StreamParser();

  SyntaxType ParseNextElement();
  ByteStringView GetWord() const {
    return ByteStringView(m_WordBuffer, m_WordSize);
  }
  uint32_t GetPos() const { return m_Pos; }
  void SetPos(uint32_t pos) { m_Pos = pos; }
  std::unique_ptr<CPDF_Object> GetObject() { return std::move(m_pLastObj); }
  std::unique_ptr<CPDF_Object> ReadNextObject(bool bAllowNestedArray,
                                              bool bInArray,
                                              uint32_t dwRecursionLevel);
  std::unique_ptr<CPDF_Stream> ReadInlineStream(
      CPDF_Document* pDoc,
      std::unique_ptr<CPDF_Dictionary> pDict,
      const CPDF_Object* pCSObj);

 private:
  friend class cpdf_streamparser_ReadHexString_Test;
  static const uint32_t kMaxWordLength = 255;

  void GetNextWord(bool& bIsNumber);
  ByteString ReadString();
  ByteString ReadHexString();
  bool PositionIsInBounds() const;

  uint32_t m_Pos = 0;       // Current byte position within m_pBuf.
  uint32_t m_WordSize = 0;  // Current byte position within m_WordBuffer.
  WeakPtr<ByteStringPool> m_pPool;
  std::unique_ptr<CPDF_Object> m_pLastObj;
  pdfium::span<const uint8_t> m_pBuf;
  uint8_t m_WordBuffer[kMaxWordLength + 1];  // Include space for NUL.
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_STREAMPARSER_H_
