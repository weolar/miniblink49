// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_CFX_UNICODEENCODINGEX_H_
#define CORE_FXGE_CFX_UNICODEENCODINGEX_H_

#include <memory>

#include "core/fxcrt/fx_system.h"
#include "core/fxge/cfx_unicodeencoding.h"

class CFX_UnicodeEncodingEx final : public CFX_UnicodeEncoding {
 public:
  CFX_UnicodeEncodingEx(CFX_Font* pFont, uint32_t EncodingID);
  ~CFX_UnicodeEncodingEx() override;

  // CFX_UnicodeEncoding:
  uint32_t GlyphFromCharCode(uint32_t charcode) override;

  uint32_t CharCodeFromUnicode(wchar_t Unicode) const;

 private:
  uint32_t m_nEncodingID;
};

std::unique_ptr<CFX_UnicodeEncodingEx> FX_CreateFontEncodingEx(CFX_Font* pFont);

#endif  // CORE_FXGE_CFX_UNICODEENCODINGEX_H_
