// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_CFX_FONTMGR_H_
#define CORE_FXGE_CFX_FONTMGR_H_

#include <map>
#include <memory>

#include "core/fxcrt/fx_memory.h"
#include "core/fxge/fx_font.h"
#include "third_party/base/optional.h"
#include "third_party/base/span.h"

class CFX_FontMapper;
class CFX_SubstFont;
class CTTFontDesc;
class SystemFontInfoIface;

class CFX_FontMgr {
 public:
  CFX_FontMgr();
  ~CFX_FontMgr();

  void InitFTLibrary();

  FXFT_Face GetCachedFace(const ByteString& face_name,
                          int weight,
                          bool bItalic,
                          uint8_t** pFontData);
  FXFT_Face AddCachedFace(const ByteString& face_name,
                          int weight,
                          bool bItalic,
                          std::unique_ptr<uint8_t, FxFreeDeleter> pData,
                          uint32_t size,
                          int face_index);
  FXFT_Face GetCachedTTCFace(int ttc_size,
                             uint32_t checksum,
                             int font_offset,
                             uint8_t** pFontData);
  FXFT_Face AddCachedTTCFace(int ttc_size,
                             uint32_t checksum,
                             std::unique_ptr<uint8_t, FxFreeDeleter> pData,
                             uint32_t size,
                             int font_offset);
  FXFT_Face GetFixedFace(pdfium::span<const uint8_t> span, int face_index);
  void ReleaseFace(FXFT_Face face);
  void SetSystemFontInfo(std::unique_ptr<SystemFontInfoIface> pFontInfo);
  FXFT_Face FindSubstFont(const ByteString& face_name,
                          bool bTrueType,
                          uint32_t flags,
                          int weight,
                          int italic_angle,
                          int CharsetCP,
                          CFX_SubstFont* pSubstFont);
  Optional<pdfium::span<const uint8_t>> GetBuiltinFont(size_t index);

  // Always present.
  CFX_FontMapper* GetBuiltinMapper() const { return m_pBuiltinMapper.get(); }

  FXFT_Library GetFTLibrary() const { return m_FTLibrary; }
  bool FTLibrarySupportsHinting() const { return m_FTLibrarySupportsHinting; }

 private:
  bool FreeTypeVersionSupportsHinting() const;
  bool SetLcdFilterMode() const;

  std::unique_ptr<CFX_FontMapper> m_pBuiltinMapper;
  std::map<ByteString, std::unique_ptr<CTTFontDesc>> m_FaceMap;
  FXFT_Library m_FTLibrary = nullptr;
  bool m_FTLibrarySupportsHinting = false;
};

#endif  // CORE_FXGE_CFX_FONTMGR_H_
