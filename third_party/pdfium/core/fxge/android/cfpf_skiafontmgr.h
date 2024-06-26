// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_ANDROID_CFPF_SKIAFONTMGR_H_
#define CORE_FXGE_ANDROID_CFPF_SKIAFONTMGR_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxge/fx_font.h"

class CFPF_SkiaFont;
class CFPF_SkiaPathFont;

class CFPF_SkiaFontMgr {
 public:
  CFPF_SkiaFontMgr();
  ~CFPF_SkiaFontMgr();

  void LoadSystemFonts();
  CFPF_SkiaFont* CreateFont(ByteStringView bsFamilyname,
                            uint8_t uCharset,
                            uint32_t dwStyle);

  bool InitFTLibrary();
  FXFT_Face GetFontFace(ByteStringView bsFile, int32_t iFaceIndex);

 private:
  void ScanPath(const ByteString& path);
  void ScanFile(const ByteString& file);
  std::unique_ptr<CFPF_SkiaPathFont> ReportFace(FXFT_Face face,
                                                const ByteString& file);

  bool m_bLoaded = false;
  FXFT_Library m_FTLibrary = nullptr;
  std::vector<std::unique_ptr<CFPF_SkiaPathFont>> m_FontFaces;
  std::map<uint32_t, std::unique_ptr<CFPF_SkiaFont>> m_FamilyFonts;
};

#endif  // CORE_FXGE_ANDROID_CFPF_SKIAFONTMGR_H_
