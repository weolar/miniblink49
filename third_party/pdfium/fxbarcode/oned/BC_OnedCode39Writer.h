// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_ONED_BC_ONEDCODE39WRITER_H_
#define FXBARCODE_ONED_BC_ONEDCODE39WRITER_H_

#include "fxbarcode/BC_Library.h"
#include "fxbarcode/oned/BC_OneDimWriter.h"

class CBC_OnedCode39Writer final : public CBC_OneDimWriter {
 public:
  CBC_OnedCode39Writer();
  ~CBC_OnedCode39Writer() override;

  // CBC_OneDimWriter
  uint8_t* EncodeWithHint(const ByteString& contents,
                          BCFORMAT format,
                          int32_t& outWidth,
                          int32_t& outHeight,
                          int32_t hints) override;
  uint8_t* EncodeImpl(const ByteString& contents, int32_t& outLength) override;
  bool RenderResult(WideStringView contents,
                    uint8_t* code,
                    int32_t codeLength) override;
  bool CheckContentValidity(WideStringView contents) override;
  WideString FilterContents(WideStringView contents) override;
  WideString RenderTextContents(WideStringView contents) override;
  bool SetTextLocation(BC_TEXT_LOC location) override;
  bool SetWideNarrowRatio(int8_t ratio) override;

  bool encodedContents(WideStringView contents, WideString* result);

 private:
  void ToIntArray(int16_t a, int8_t* toReturn);
  char CalcCheckSum(const ByteString& contents);

  int8_t m_iWideNarrRatio = 3;
};

#endif  // FXBARCODE_ONED_BC_ONEDCODE39WRITER_H_
