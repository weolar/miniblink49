// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_ANDROID_CFPF_SKIAPATHFONT_H_
#define CORE_FXGE_ANDROID_CFPF_SKIAPATHFONT_H_

#include "core/fxcrt/bytestring.h"
#include "core/fxcrt/fx_system.h"

class CFPF_SkiaPathFont {
 public:
  CFPF_SkiaPathFont(const ByteString& path,
                    const char* pFamily,
                    uint32_t dwStyle,
                    int32_t iFaceIndex,
                    uint32_t dwCharsets,
                    int32_t iGlyphNum);
  ~CFPF_SkiaPathFont();

  const char* path() const { return m_bsPath.c_str(); }
  const char* family() const { return m_bsFamily.c_str(); }
  uint32_t style() const { return m_dwStyle; }
  int32_t face_index() const { return m_iFaceIndex; }
  uint32_t charsets() const { return m_dwCharsets; }
  int32_t glyph_num() const { return m_iGlyphNum; }

 private:
  const ByteString m_bsPath;
  const ByteString m_bsFamily;
  const uint32_t m_dwStyle;
  const int32_t m_iFaceIndex;
  const uint32_t m_dwCharsets;
  const int32_t m_iGlyphNum;
};

#endif  // CORE_FXGE_ANDROID_CFPF_SKIAPATHFONT_H_
