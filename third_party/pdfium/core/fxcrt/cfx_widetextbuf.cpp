// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/cfx_widetextbuf.h"

size_t CFX_WideTextBuf::GetLength() const {
  return m_DataSize / sizeof(wchar_t);
}

void CFX_WideTextBuf::AppendChar(wchar_t ch) {
  ExpandBuf(sizeof(wchar_t));
  *reinterpret_cast<wchar_t*>(m_pBuffer.get() + m_DataSize) = ch;
  m_DataSize += sizeof(wchar_t);
}

CFX_WideTextBuf& CFX_WideTextBuf::operator<<(ByteStringView ascii) {
  ExpandBuf(ascii.GetLength() * sizeof(wchar_t));
  for (uint8_t ch : ascii) {
    *reinterpret_cast<wchar_t*>(m_pBuffer.get() + m_DataSize) = ch;
    m_DataSize += sizeof(wchar_t);
  }
  return *this;
}

CFX_WideTextBuf& CFX_WideTextBuf::operator<<(WideStringView str) {
  AppendBlock(str.unterminated_c_str(), str.GetLength() * sizeof(wchar_t));
  return *this;
}

CFX_WideTextBuf& CFX_WideTextBuf::operator<<(const WideString& str) {
  AppendBlock(str.c_str(), str.GetLength() * sizeof(wchar_t));
  return *this;
}

CFX_WideTextBuf& CFX_WideTextBuf::operator<<(int i) {
  char buf[32];
  FXSYS_itoa(i, buf, 10);
  size_t len = strlen(buf);
  ExpandBuf(len * sizeof(wchar_t));
  wchar_t* str = reinterpret_cast<wchar_t*>(m_pBuffer.get() + m_DataSize);
  for (size_t j = 0; j < len; j++) {
    *str++ = buf[j];
  }
  m_DataSize += len * sizeof(wchar_t);
  return *this;
}

CFX_WideTextBuf& CFX_WideTextBuf::operator<<(double f) {
  char buf[32];
  size_t len = FloatToString((float)f, buf);
  ExpandBuf(len * sizeof(wchar_t));
  wchar_t* str = reinterpret_cast<wchar_t*>(m_pBuffer.get() + m_DataSize);
  for (size_t i = 0; i < len; i++) {
    *str++ = buf[i];
  }
  m_DataSize += len * sizeof(wchar_t);
  return *this;
}

CFX_WideTextBuf& CFX_WideTextBuf::operator<<(const wchar_t* lpsz) {
  AppendBlock(lpsz, wcslen(lpsz) * sizeof(wchar_t));
  return *this;
}

CFX_WideTextBuf& CFX_WideTextBuf::operator<<(const CFX_WideTextBuf& buf) {
  AppendBlock(buf.m_pBuffer.get(), buf.m_DataSize);
  return *this;
}
