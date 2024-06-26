// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_defaultappearance.h"

#include <algorithm>
#include <vector>

#include "core/fpdfapi/parser/cpdf_simple_parser.h"
#include "core/fpdfapi/parser/fpdf_parser_utility.h"
#include "core/fxge/cfx_color.h"

namespace {

// Find the token and its |nParams| parameters from the start of data,
// and move the current position to the start of those parameters.
bool FindTagParamFromStart(CPDF_SimpleParser* parser,
                           ByteStringView token,
                           int nParams) {
  nParams++;

  std::vector<uint32_t> pBuf(nParams);
  int buf_index = 0;
  int buf_count = 0;

  parser->SetCurPos(0);
  while (1) {
    pBuf[buf_index++] = parser->GetCurPos();
    if (buf_index == nParams)
      buf_index = 0;

    buf_count++;
    if (buf_count > nParams)
      buf_count = nParams;

    ByteStringView word = parser->GetWord();
    if (word.IsEmpty())
      return false;

    if (word == token) {
      if (buf_count < nParams)
        continue;

      parser->SetCurPos(pBuf[buf_index]);
      return true;
    }
  }
  return false;
}

}  // namespace

Optional<ByteString> CPDF_DefaultAppearance::GetFont(float* fFontSize) {
  *fFontSize = 0.0f;
  if (m_csDA.IsEmpty())
    return {};

  ByteString csFontNameTag;
  CPDF_SimpleParser syntax(m_csDA.AsStringView().span());
  if (FindTagParamFromStart(&syntax, "Tf", 2)) {
    csFontNameTag = ByteString(syntax.GetWord());
    csFontNameTag.Delete(0, 1);
    *fFontSize = StringToFloat(syntax.GetWord());
  }
  return {PDF_NameDecode(csFontNameTag.AsStringView())};
}

Optional<CFX_Color::Type> CPDF_DefaultAppearance::GetColor(float fc[4]) {
  for (int c = 0; c < 4; c++)
    fc[c] = 0;

  if (m_csDA.IsEmpty())
    return {};

  CPDF_SimpleParser syntax(m_csDA.AsStringView().span());
  if (FindTagParamFromStart(&syntax, "g", 1)) {
    fc[0] = StringToFloat(syntax.GetWord());
    return {CFX_Color::kGray};
  }
  if (FindTagParamFromStart(&syntax, "rg", 3)) {
    fc[0] = StringToFloat(syntax.GetWord());
    fc[1] = StringToFloat(syntax.GetWord());
    fc[2] = StringToFloat(syntax.GetWord());
    return {CFX_Color::kRGB};
  }
  if (FindTagParamFromStart(&syntax, "k", 4)) {
    fc[0] = StringToFloat(syntax.GetWord());
    fc[1] = StringToFloat(syntax.GetWord());
    fc[2] = StringToFloat(syntax.GetWord());
    fc[3] = StringToFloat(syntax.GetWord());
    return {CFX_Color::kCMYK};
  }

  return {};
}

std::pair<Optional<CFX_Color::Type>, FX_ARGB>
CPDF_DefaultAppearance::GetColor() {
  float values[4];
  Optional<CFX_Color::Type> type = GetColor(values);
  if (!type)
    return {type, 0};

  if (*type == CFX_Color::kGray) {
    int g = static_cast<int>(values[0] * 255 + 0.5f);
    return {type, ArgbEncode(255, g, g, g)};
  }
  if (*type == CFX_Color::kRGB) {
    int r = static_cast<int>(values[0] * 255 + 0.5f);
    int g = static_cast<int>(values[1] * 255 + 0.5f);
    int b = static_cast<int>(values[2] * 255 + 0.5f);
    return {type, ArgbEncode(255, r, g, b)};
  }
  if (*type == CFX_Color::kCMYK) {
    float r = 1.0f - std::min(1.0f, values[0] + values[3]);
    float g = 1.0f - std::min(1.0f, values[1] + values[3]);
    float b = 1.0f - std::min(1.0f, values[2] + values[3]);
    return {type, ArgbEncode(255, static_cast<int>(r * 255 + 0.5f),
                             static_cast<int>(g * 255 + 0.5f),
                             static_cast<int>(b * 255 + 0.5f))};
  }
  NOTREACHED();
  return {{}, 0};
}

bool CPDF_DefaultAppearance::FindTagParamFromStartForTesting(
    CPDF_SimpleParser* parser,
    ByteStringView token,
    int nParams) {
  return FindTagParamFromStart(parser, token, nParams);
}
