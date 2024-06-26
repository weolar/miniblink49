// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2006-2007 Jeremias Maerki.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fxbarcode/datamatrix/BC_EncoderContext.h"

#include <utility>

#include "fxbarcode/common/BC_CommonBitMatrix.h"
#include "fxbarcode/datamatrix/BC_Encoder.h"
#include "fxbarcode/datamatrix/BC_SymbolInfo.h"

CBC_EncoderContext::CBC_EncoderContext(const WideString& msg) {
  ByteString dststr = msg.ToUTF8();
  size_t c = dststr.GetLength();
  WideString sb;
  sb.Reserve(c);
  for (size_t i = 0; i < c; i++) {
    wchar_t ch = static_cast<wchar_t>(dststr[i] & 0xff);
    if (ch == '?' && dststr[i] != '?') {
      m_bHasCharactersOutsideISO88591Encoding = true;
    }
    sb += ch;
  }
  m_msg = std::move(sb);
  m_codewords.Reserve(m_msg.GetLength());
}

CBC_EncoderContext::~CBC_EncoderContext() = default;

void CBC_EncoderContext::setSkipAtEnd(int32_t count) {
  m_skipAtEnd = count;
}
wchar_t CBC_EncoderContext::getCurrentChar() {
  return m_msg[m_pos];
}
wchar_t CBC_EncoderContext::getCurrent() {
  return m_msg[m_pos];
}

void CBC_EncoderContext::writeCodewords(const WideString& codewords) {
  m_codewords += codewords;
}

void CBC_EncoderContext::writeCodeword(wchar_t codeword) {
  m_codewords += codeword;
}

size_t CBC_EncoderContext::getCodewordCount() {
  return m_codewords.GetLength();
}

void CBC_EncoderContext::SignalEncoderChange(
    CBC_HighLevelEncoder::Encoding encoding) {
  m_newEncoding = encoding;
}

void CBC_EncoderContext::ResetEncoderSignal() {
  m_newEncoding = CBC_HighLevelEncoder::Encoding::UNKNOWN;
}

bool CBC_EncoderContext::hasMoreCharacters() {
  return m_pos < getTotalMessageCharCount();
}

size_t CBC_EncoderContext::getRemainingCharacters() {
  return getTotalMessageCharCount() - m_pos;
}

bool CBC_EncoderContext::UpdateSymbolInfo() {
  return UpdateSymbolInfo(getCodewordCount());
}

bool CBC_EncoderContext::UpdateSymbolInfo(size_t len) {
  if (!m_symbolInfo || len > m_symbolInfo->dataCapacity()) {
    m_symbolInfo = CBC_SymbolInfo::Lookup(len, m_bAllowRectangular);
    if (!m_symbolInfo)
      return false;
  }
  return true;
}

void CBC_EncoderContext::resetSymbolInfo() {
  m_bAllowRectangular = true;
}

size_t CBC_EncoderContext::getTotalMessageCharCount() {
  return m_msg.GetLength() - m_skipAtEnd;
}
