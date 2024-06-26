// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CFX_UTF8ENCODER_H_
#define CORE_FXCRT_CFX_UTF8ENCODER_H_

#include <vector>

#include "core/fxcrt/fx_string.h"

class CFX_UTF8Encoder {
 public:
  CFX_UTF8Encoder();
  ~CFX_UTF8Encoder();

  void Input(wchar_t unicodeAsWchar);

  // The data returned by GetResult() is invalidated when this is modified by
  // appending any data.
  ByteStringView GetResult() const {
    return ByteStringView(m_Buffer.data(), m_Buffer.size());
  }

 private:
  std::vector<uint8_t> m_Buffer;
};

#endif  // CORE_FXCRT_CFX_UTF8ENCODER_H_
