// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_textrenderer.h"

#include <algorithm>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/render/cpdf_charposlist.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_renderdevice.h"

namespace {

CFX_Font* GetFont(CPDF_Font* pFont, int32_t position) {
  return position == -1 ? pFont->GetFont() : pFont->GetFontFallback(position);
}

}  // namespace

// static
bool CPDF_TextRenderer::DrawTextPath(CFX_RenderDevice* pDevice,
                                     const std::vector<uint32_t>& charCodes,
                                     const std::vector<float>& charPos,
                                     CPDF_Font* pFont,
                                     float font_size,
                                     const CFX_Matrix* pText2User,
                                     const CFX_Matrix* pUser2Device,
                                     const CFX_GraphStateData* pGraphState,
                                     FX_ARGB fill_argb,
                                     FX_ARGB stroke_argb,
                                     CFX_PathData* pClippingPath,
                                     int nFlag) {
  CPDF_CharPosList CharPosList;
  CharPosList.Load(charCodes, charPos, pFont, font_size);
  if (CharPosList.m_nChars == 0)
    return true;

  bool bDraw = true;
  int32_t fontPosition = CharPosList.m_pCharPos[0].m_FallbackFontPosition;
  uint32_t startIndex = 0;
  for (uint32_t i = 0; i < CharPosList.m_nChars; i++) {
    int32_t curFontPosition = CharPosList.m_pCharPos[i].m_FallbackFontPosition;
    if (fontPosition == curFontPosition)
      continue;

    CFX_Font* font = GetFont(pFont, fontPosition);
    if (!pDevice->DrawTextPath(i - startIndex,
                               CharPosList.m_pCharPos + startIndex, font,
                               font_size, pText2User, pUser2Device, pGraphState,
                               fill_argb, stroke_argb, pClippingPath, nFlag)) {
      bDraw = false;
    }
    fontPosition = curFontPosition;
    startIndex = i;
  }
  CFX_Font* font = GetFont(pFont, fontPosition);
  if (!pDevice->DrawTextPath(CharPosList.m_nChars - startIndex,
                             CharPosList.m_pCharPos + startIndex, font,
                             font_size, pText2User, pUser2Device, pGraphState,
                             fill_argb, stroke_argb, pClippingPath, nFlag)) {
    bDraw = false;
  }
  return bDraw;
}

// static
void CPDF_TextRenderer::DrawTextString(CFX_RenderDevice* pDevice,
                                       float origin_x,
                                       float origin_y,
                                       CPDF_Font* pFont,
                                       float font_size,
                                       const CFX_Matrix& matrix,
                                       const ByteString& str,
                                       FX_ARGB fill_argb,
                                       const CFX_GraphStateData* pGraphState,
                                       const CPDF_RenderOptions* pOptions) {
  if (pFont->IsType3Font())
    return;

  int nChars = pFont->CountChar(str.AsStringView());
  if (nChars <= 0)
    return;

  size_t offset = 0;
  std::vector<uint32_t> codes;
  std::vector<float> positions;
  codes.resize(nChars);
  positions.resize(nChars - 1);
  float cur_pos = 0;
  for (int i = 0; i < nChars; i++) {
    codes[i] = pFont->GetNextChar(str.AsStringView(), &offset);
    if (i)
      positions[i - 1] = cur_pos;
    cur_pos += pFont->GetCharWidthF(codes[i]) * font_size / 1000;
  }
  CFX_Matrix new_matrix = matrix;
  new_matrix.e = origin_x;
  new_matrix.f = origin_y;
  DrawNormalText(pDevice, codes, positions, pFont, font_size, &new_matrix,
                 fill_argb, pOptions);
}

// static
bool CPDF_TextRenderer::DrawNormalText(CFX_RenderDevice* pDevice,
                                       const std::vector<uint32_t>& charCodes,
                                       const std::vector<float>& charPos,
                                       CPDF_Font* pFont,
                                       float font_size,
                                       const CFX_Matrix* pText2Device,
                                       FX_ARGB fill_argb,
                                       const CPDF_RenderOptions* pOptions) {
  CPDF_CharPosList CharPosList;
  CharPosList.Load(charCodes, charPos, pFont, font_size);
  if (CharPosList.m_nChars == 0)
    return true;
  int FXGE_flags = 0;
  if (pOptions) {
    if (pOptions->GetOptions().bClearType) {
      FXGE_flags |= FXTEXT_CLEARTYPE;
      if (pOptions->GetOptions().bBGRStripe)
        FXGE_flags |= FXTEXT_BGR_STRIPE;
    }
    if (pOptions->GetOptions().bNoTextSmooth)
      FXGE_flags |= FXTEXT_NOSMOOTH;
    if (pOptions->GetOptions().bPrintGraphicText)
      FXGE_flags |= FXTEXT_PRINTGRAPHICTEXT;
    if (pOptions->GetOptions().bNoNativeText)
      FXGE_flags |= FXTEXT_NO_NATIVETEXT;
    if (pOptions->GetOptions().bPrintImageText)
      FXGE_flags |= FXTEXT_PRINTIMAGETEXT;
  } else {
    FXGE_flags = FXTEXT_CLEARTYPE;
  }
  if (pFont->IsCIDFont())
    FXGE_flags |= FXFONT_CIDFONT;
  bool bDraw = true;
  int32_t fontPosition = CharPosList.m_pCharPos[0].m_FallbackFontPosition;
  uint32_t startIndex = 0;
  for (uint32_t i = 0; i < CharPosList.m_nChars; i++) {
    int32_t curFontPosition = CharPosList.m_pCharPos[i].m_FallbackFontPosition;
    if (fontPosition == curFontPosition)
      continue;

    CFX_Font* font = GetFont(pFont, fontPosition);
    if (!pDevice->DrawNormalText(
            i - startIndex, CharPosList.m_pCharPos + startIndex, font,
            font_size, pText2Device, fill_argb, FXGE_flags)) {
      bDraw = false;
    }
    fontPosition = curFontPosition;
    startIndex = i;
  }
  CFX_Font* font = GetFont(pFont, fontPosition);
  if (!pDevice->DrawNormalText(CharPosList.m_nChars - startIndex,
                               CharPosList.m_pCharPos + startIndex, font,
                               font_size, pText2Device, fill_argb,
                               FXGE_flags)) {
    bDraw = false;
  }
  return bDraw;
}
