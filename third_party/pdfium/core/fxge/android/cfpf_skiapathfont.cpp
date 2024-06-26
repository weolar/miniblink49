// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/android/cfpf_skiapathfont.h"


CFPF_SkiaPathFont::CFPF_SkiaPathFont(const ByteString& path,
                                     const char* pFamily,
                                     uint32_t dwStyle,
                                     int32_t iFaceIndex,
                                     uint32_t dwCharsets,
                                     int32_t iGlyphNum)
    : m_bsPath(path),
      m_bsFamily(pFamily),
      m_dwStyle(dwStyle),
      m_iFaceIndex(iFaceIndex),
      m_dwCharsets(dwCharsets),
      m_iGlyphNum(iGlyphNum) {}

CFPF_SkiaPathFont::~CFPF_SkiaPathFont() = default;
