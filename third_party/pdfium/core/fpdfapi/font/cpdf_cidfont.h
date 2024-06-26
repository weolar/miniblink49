// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FONT_CPDF_CIDFONT_H_
#define CORE_FPDFAPI_FONT_CPDF_CIDFONT_H_

#include <memory>
#include <vector>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"

enum CIDSet : uint8_t {
  CIDSET_UNKNOWN,
  CIDSET_GB1,
  CIDSET_CNS1,
  CIDSET_JAPAN1,
  CIDSET_KOREA1,
  CIDSET_UNICODE,
  CIDSET_NUM_SETS
};

class CFX_CTTGSUBTable;
class CPDF_Array;
class CPDF_CID2UnicodeMap;
class CPDF_CMap;
class CPDF_StreamAcc;

class CPDF_CIDFont final : public CPDF_Font {
 public:
  CPDF_CIDFont(CPDF_Document* pDocument, CPDF_Dictionary* pFontDict);
  ~CPDF_CIDFont() override;

  static float CIDTransformToFloat(uint8_t ch);

  // CPDF_Font:
  bool IsCIDFont() const override;
  const CPDF_CIDFont* AsCIDFont() const override;
  CPDF_CIDFont* AsCIDFont() override;
  int GlyphFromCharCode(uint32_t charcode, bool* pVertGlyph) override;
  uint32_t GetCharWidthF(uint32_t charcode) override;
  FX_RECT GetCharBBox(uint32_t charcode) override;
  uint32_t GetNextChar(ByteStringView pString, size_t* pOffset) const override;
  size_t CountChar(ByteStringView pString) const override;
  int AppendChar(char* str, uint32_t charcode) const override;
  bool IsVertWriting() const override;
  bool IsUnicodeCompatible() const override;
  bool Load() override;
  WideString UnicodeFromCharCode(uint32_t charcode) const override;
  uint32_t CharCodeFromUnicode(wchar_t Unicode) const override;

  uint16_t CIDFromCharCode(uint32_t charcode) const;
  const uint8_t* GetCIDTransform(uint16_t CID) const;
  short GetVertWidth(uint16_t CID) const;
  void GetVertOrigin(uint16_t CID, short& vx, short& vy) const;
  int GetCharSize(uint32_t charcode) const;

 private:
  void LoadGB2312();
  int GetGlyphIndex(uint32_t unicodeb, bool* pVertGlyph);
  int GetVerticalGlyph(int index, bool* pVertGlyph);
  void LoadMetricsArray(const CPDF_Array* pArray,
                        std::vector<uint32_t>* result,
                        int nElements);
  void LoadSubstFont();
  wchar_t GetUnicodeFromCharCode(uint32_t charcode) const;

  RetainPtr<CPDF_CMap> m_pCMap;
  UnownedPtr<const CPDF_CID2UnicodeMap> m_pCID2UnicodeMap;
  RetainPtr<CPDF_StreamAcc> m_pStreamAcc;
  std::unique_ptr<CFX_CTTGSUBTable> m_pTTGSUBTable;
  bool m_bType1 = false;
  bool m_bCIDIsGID = false;
  bool m_bAnsiWidthsFixed = false;
  bool m_bAdobeCourierStd = false;
  CIDSet m_Charset = CIDSET_UNKNOWN;
  uint16_t m_DefaultWidth = 1000;
  short m_DefaultVY = 880;
  short m_DefaultW1 = -1000;
  std::vector<uint32_t> m_WidthList;
  std::vector<uint32_t> m_VertMetrics;
  FX_RECT m_CharBBox[256];
};

#endif  // CORE_FPDFAPI_FONT_CPDF_CIDFONT_H_
