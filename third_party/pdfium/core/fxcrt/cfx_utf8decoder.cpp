// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/cfx_utf8decoder.h"

CFX_UTF8Decoder::CFX_UTF8Decoder() = default;

CFX_UTF8Decoder::~CFX_UTF8Decoder() = default;

void CFX_UTF8Decoder::AppendCodePoint(uint32_t ch) {
  m_Buffer.AppendChar(static_cast<wchar_t>(ch));
}

void CFX_UTF8Decoder::Input(uint8_t byte) {
  if (byte < 0x80) {
    m_PendingBytes = 0;
    m_Buffer.AppendChar(byte);
  } else if (byte < 0xc0) {
    if (m_PendingBytes == 0) {
      return;
    }
    m_PendingBytes--;
    m_PendingChar |= (byte & 0x3f) << (m_PendingBytes * 6);
    if (m_PendingBytes == 0) {
      AppendCodePoint(m_PendingChar);
    }
  } else if (byte < 0xe0) {
    m_PendingBytes = 1;
    m_PendingChar = (byte & 0x1f) << 6;
  } else if (byte < 0xf0) {
    m_PendingBytes = 2;
    m_PendingChar = (byte & 0x0f) << 12;
  } else if (byte < 0xf8) {
    m_PendingBytes = 3;
    m_PendingChar = (byte & 0x07) << 18;
  } else if (byte < 0xfc) {
    m_PendingBytes = 4;
    m_PendingChar = (byte & 0x03) << 24;
  } else if (byte < 0xfe) {
    m_PendingBytes = 5;
    m_PendingChar = (byte & 0x01) << 30;
  } else {
    m_PendingBytes = 0;
  }
}
