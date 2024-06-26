// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CFDF_DOCUMENT_H_
#define CORE_FPDFAPI_PARSER_CFDF_DOCUMENT_H_

#include <memory>

#include "core/fpdfapi/parser/cpdf_indirect_object_holder.h"
#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fxcrt/unowned_ptr.h"
#include "third_party/base/span.h"

class CPDF_Dictionary;
class IFX_SeekableReadStream;

class CFDF_Document final : public CPDF_IndirectObjectHolder {
 public:
  static std::unique_ptr<CFDF_Document> CreateNewDoc();
  static std::unique_ptr<CFDF_Document> ParseMemory(
      pdfium::span<const uint8_t> span);

  CFDF_Document();
  ~CFDF_Document() override;

  ByteString WriteToString() const;
  CPDF_Dictionary* GetRoot() const { return m_pRootDict.Get(); }

 private:
  void ParseStream(const RetainPtr<IFX_SeekableReadStream>& pFile);

  UnownedPtr<CPDF_Dictionary> m_pRootDict;
  RetainPtr<IFX_SeekableReadStream> m_pFile;
};

#endif  // CORE_FPDFAPI_PARSER_CFDF_DOCUMENT_H_
