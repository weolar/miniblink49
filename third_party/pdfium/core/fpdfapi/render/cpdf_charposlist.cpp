// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_charposlist.h"

#include "core/fpdfapi/font/cpdf_cidfont.h"
#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fxge/cfx_renderdevice.h"

CPDF_CharPosList::CPDF_CharPosList() = default;

CPDF_CharPosList::~CPDF_CharPosList() {
  FX_Free(m_pCharPos);
}

void CPDF_CharPosList::Load(const std::vector<uint32_t>& charCodes,
                            const std::vector<float>& charPos,
                            CPDF_Font* pFont,
                            float FontSize) {
  m_pCharPos = FX_Alloc(FXTEXT_CHARPOS, charCodes.size());
  m_nChars = 0;
  CPDF_CIDFont* pCIDFont = pFont->AsCIDFont();
  bool bVertWriting = pCIDFont && pCIDFont->IsVertWriting();
  for (size_t iChar = 0; iChar < charCodes.size(); ++iChar) {
    uint32_t CharCode = charCodes[iChar];
    if (CharCode == static_cast<uint32_t>(-1))
      continue;

    bool bVert = false;
    FXTEXT_CHARPOS& charpos = m_pCharPos[m_nChars++];
    if (pCIDFont)
      charpos.m_bFontStyle = true;
    WideString unicode = pFont->UnicodeFromCharCode(CharCode);
    charpos.m_Unicode = !unicode.IsEmpty() ? unicode[0] : CharCode;
    charpos.m_GlyphIndex = pFont->GlyphFromCharCode(CharCode, &bVert);
    uint32_t GlyphID = charpos.m_GlyphIndex;
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
    charpos.m_ExtGID = pFont->GlyphFromCharCodeExt(CharCode);
    GlyphID = charpos.m_ExtGID != static_cast<uint32_t>(-1)
                  ? charpos.m_ExtGID
                  : charpos.m_GlyphIndex;
#endif
    CFX_Font* pCurrentFont;
    if (GlyphID != static_cast<uint32_t>(-1)) {
      charpos.m_FallbackFontPosition = -1;
      pCurrentFont = pFont->GetFont();
    } else {
      charpos.m_FallbackFontPosition =
          pFont->FallbackFontFromCharcode(CharCode);
      charpos.m_GlyphIndex = pFont->FallbackGlyphFromCharcode(
          charpos.m_FallbackFontPosition, CharCode);
      pCurrentFont = pFont->GetFontFallback(charpos.m_FallbackFontPosition);
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
      charpos.m_ExtGID = charpos.m_GlyphIndex;
#endif
    }

    if (!pFont->IsEmbedded() && !pFont->IsCIDFont())
      charpos.m_FontCharWidth = pFont->GetCharWidthF(CharCode);
    else
      charpos.m_FontCharWidth = 0;

    charpos.m_Origin = CFX_PointF(iChar > 0 ? charPos[iChar - 1] : 0, 0);
    charpos.m_bGlyphAdjust = false;

    float scalingFactor = 1.0f;
    if (!pFont->IsEmbedded() && pFont->HasFontWidths() && !bVertWriting &&
        !pCurrentFont->GetSubstFont()->m_bFlagMM) {
      uint32_t pdfGlyphWidth = pFont->GetCharWidthF(CharCode);
      uint32_t ftGlyphWidth =
          pCurrentFont ? pCurrentFont->GetGlyphWidth(charpos.m_GlyphIndex) : 0;
      if (ftGlyphWidth && pdfGlyphWidth > ftGlyphWidth + 1) {
        // Move the initial x position by half of the excess (transformed to
        // text space coordinates).
        charpos.m_Origin.x +=
            (pdfGlyphWidth - ftGlyphWidth) * FontSize / 2000.0f;
      } else if (pdfGlyphWidth && ftGlyphWidth &&
                 pdfGlyphWidth < ftGlyphWidth) {
        scalingFactor = static_cast<float>(pdfGlyphWidth) / ftGlyphWidth;
        charpos.m_AdjustMatrix[0] = scalingFactor;
        charpos.m_AdjustMatrix[1] = 0.0f;
        charpos.m_AdjustMatrix[2] = 0.0f;
        charpos.m_AdjustMatrix[3] = 1.0f;
        charpos.m_bGlyphAdjust = true;
      }
    }
    if (!pCIDFont)
      continue;

    uint16_t CID = pCIDFont->CIDFromCharCode(CharCode);
    if (bVertWriting) {
      charpos.m_Origin = CFX_PointF(0, charpos.m_Origin.x);

      short vx;
      short vy;
      pCIDFont->GetVertOrigin(CID, vx, vy);
      charpos.m_Origin.x -= FontSize * vx / 1000;
      charpos.m_Origin.y -= FontSize * vy / 1000;
    }

    const uint8_t* pTransform = pCIDFont->GetCIDTransform(CID);
    if (pTransform && !bVert) {
      charpos.m_AdjustMatrix[0] =
          pCIDFont->CIDTransformToFloat(pTransform[0]) * scalingFactor;
      charpos.m_AdjustMatrix[1] =
          pCIDFont->CIDTransformToFloat(pTransform[1]) * scalingFactor;
      charpos.m_AdjustMatrix[2] = pCIDFont->CIDTransformToFloat(pTransform[2]);
      charpos.m_AdjustMatrix[3] = pCIDFont->CIDTransformToFloat(pTransform[3]);
      charpos.m_Origin.x +=
          pCIDFont->CIDTransformToFloat(pTransform[4]) * FontSize;
      charpos.m_Origin.y +=
          pCIDFont->CIDTransformToFloat(pTransform[5]) * FontSize;
      charpos.m_bGlyphAdjust = true;
    }
  }
}
