// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_ENCODERCONTEXT_H_
#define FXBARCODE_DATAMATRIX_BC_ENCODERCONTEXT_H_

#include "core/fxcrt/unowned_ptr.h"
#include "core/fxcrt/widestring.h"
#include "fxbarcode/datamatrix/BC_HighLevelEncoder.h"

class CBC_SymbolInfo;

class CBC_EncoderContext {
 public:
  explicit CBC_EncoderContext(const WideString& msg);
  ~CBC_EncoderContext();

  void setSkipAtEnd(int32_t count);
  wchar_t getCurrentChar();
  wchar_t getCurrent();
  void writeCodewords(const WideString& codewords);
  void writeCodeword(wchar_t codeword);
  size_t getCodewordCount();
  void SignalEncoderChange(CBC_HighLevelEncoder::Encoding encoding);
  void ResetEncoderSignal();
  bool hasMoreCharacters();
  size_t getRemainingCharacters();
  bool UpdateSymbolInfo();
  bool UpdateSymbolInfo(size_t len);
  void resetSymbolInfo();

  bool HasCharactersOutsideISO88591Encoding() const {
    return m_bHasCharactersOutsideISO88591Encoding;
  }

  WideString m_msg;
  WideString m_codewords;
  size_t m_pos = 0;
  CBC_HighLevelEncoder::Encoding m_newEncoding =
      CBC_HighLevelEncoder::Encoding::UNKNOWN;
  UnownedPtr<const CBC_SymbolInfo> m_symbolInfo;

 private:
  size_t getTotalMessageCharCount();

  bool m_bAllowRectangular = false;  // Force square when false.
  bool m_bHasCharactersOutsideISO88591Encoding = false;
  size_t m_skipAtEnd = 0;
};

#endif  // FXBARCODE_DATAMATRIX_BC_ENCODERCONTEXT_H_
