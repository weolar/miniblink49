// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_CFX_FONTMAPPER_H_
#define CORE_FXGE_CFX_FONTMAPPER_H_

#include <memory>
#include <utility>
#include <vector>

#include "core/fxge/fx_font.h"

class CFX_FontMgr;
class CFX_SubstFont;
class SystemFontInfoIface;

class CFX_FontMapper {
 public:
  explicit CFX_FontMapper(CFX_FontMgr* mgr);
  ~CFX_FontMapper();

  void SetSystemFontInfo(std::unique_ptr<SystemFontInfoIface> pFontInfo);
  SystemFontInfoIface* GetSystemFontInfo() { return m_pFontInfo.get(); }
  void AddInstalledFont(const ByteString& name, int charset);
  void LoadInstalledFonts();

  FXFT_Face FindSubstFont(const ByteString& face_name,
                          bool bTrueType,
                          uint32_t flags,
                          int weight,
                          int italic_angle,
                          int CharsetCP,
                          CFX_SubstFont* pSubstFont);

  bool IsBuiltinFace(const FXFT_Face face) const;
  int GetFaceSize() const;
  ByteString GetFaceName(int index) const { return m_FaceArray[index].name; }

  std::vector<ByteString> m_InstalledTTFonts;
  std::vector<std::pair<ByteString, ByteString>> m_LocalizedTTFonts;

 private:
  static const size_t MM_FACE_COUNT = 2;
  static const size_t FOXIT_FACE_COUNT = 14;

  ByteString GetPSNameFromTT(void* hFont);
  ByteString MatchInstalledFonts(const ByteString& norm_name);
  FXFT_Face UseInternalSubst(CFX_SubstFont* pSubstFont,
                             int iBaseFont,
                             int italic_angle,
                             int weight,
                             int picthfamily);
  FXFT_Face GetCachedTTCFace(void* hFont,
                             const uint32_t tableTTCF,
                             uint32_t ttc_size,
                             uint32_t font_size);
  FXFT_Face GetCachedFace(void* hFont,
                          ByteString SubstName,
                          int weight,
                          bool bItalic,
                          uint32_t font_size);

  struct FaceData {
    ByteString name;
    uint32_t charset;
  };

  bool m_bListLoaded;
  ByteString m_LastFamily;
  std::vector<FaceData> m_FaceArray;
  std::unique_ptr<SystemFontInfoIface> m_pFontInfo;
  UnownedPtr<CFX_FontMgr> const m_pFontMgr;
  FXFT_Face m_MMFaces[MM_FACE_COUNT];
  FXFT_Face m_FoxitFaces[FOXIT_FACE_COUNT];
};

#endif  // CORE_FXGE_CFX_FONTMAPPER_H_
