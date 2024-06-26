// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_cssdata.h"

#include <algorithm>
#include <utility>

#include "core/fxcrt/css/cfx_cssstyleselector.h"
#include "core/fxcrt/css/cfx_cssvaluelistparser.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/fx_extension.h"

namespace {

#undef PROP
#define PROP(a, b, c, d) a, c, d

const CFX_CSSData::Property propertyTable[] = {
    {PROP(CFX_CSSProperty::BorderLeft,
          "border-left",
          0x04080036,
          CFX_CSSVALUETYPE_Shorthand)},
    {PROP(CFX_CSSProperty::Top,
          "top",
          0x0BEDAF33,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::Margin,
          "margin",
          0x0CB016BE,
          CFX_CSSVALUETYPE_List | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::TextIndent,
          "text-indent",
          0x169ADB74,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::Right,
          "right",
          0x193ADE3E,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::PaddingLeft,
          "padding-left",
          0x228CF02F,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::MarginLeft,
          "margin-left",
          0x297C5656,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeNumber |
              CFX_CSSVALUETYPE_MaybeEnum)},
    {PROP(CFX_CSSProperty::Border,
          "border",
          0x2A23349E,
          CFX_CSSVALUETYPE_Shorthand)},
    {PROP(CFX_CSSProperty::BorderTop,
          "border-top",
          0x2B866ADE,
          CFX_CSSVALUETYPE_Shorthand)},
    {PROP(CFX_CSSProperty::Bottom,
          "bottom",
          0x399F02B5,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::PaddingRight,
          "padding-right",
          0x3F616AC2,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::BorderBottom,
          "border-bottom",
          0x452CE780,
          CFX_CSSVALUETYPE_Shorthand)},
    {PROP(CFX_CSSProperty::FontFamily,
          "font-family",
          0x574686E6,
          CFX_CSSVALUETYPE_List | CFX_CSSVALUETYPE_MaybeString)},
    {PROP(CFX_CSSProperty::FontWeight,
          "font-weight",
          0x6692F60C,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::Color,
          "color",
          0x6E67921F,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeColor)},
    {PROP(CFX_CSSProperty::LetterSpacing,
          "letter-spacing",
          0x70536102,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::TextAlign,
          "text-align",
          0x7553F1BD,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum)},
    {PROP(CFX_CSSProperty::BorderRightWidth,
          "border-right-width",
          0x8F5A6036,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::VerticalAlign,
          "vertical-align",
          0x934A87D2,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::PaddingTop,
          "padding-top",
          0x959D22B7,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::FontVariant,
          "font-variant",
          0x9C785779,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum)},
    {PROP(CFX_CSSProperty::BorderWidth,
          "border-width",
          0xA8DE4FEB,
          CFX_CSSVALUETYPE_List | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::BorderBottomWidth,
          "border-bottom-width",
          0xAE41204D,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::BorderRight,
          "border-right",
          0xB78E9EA9,
          CFX_CSSVALUETYPE_Shorthand)},
    {PROP(CFX_CSSProperty::FontSize,
          "font-size",
          0xB93956DF,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::BorderSpacing,
          "border-spacing",
          0xC72030F0,
          CFX_CSSVALUETYPE_List | CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::FontStyle,
          "font-style",
          0xCB1950F5,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum)},
    {PROP(CFX_CSSProperty::Font,
          "font",
          0xCD308B77,
          CFX_CSSVALUETYPE_Shorthand)},
    {PROP(CFX_CSSProperty::LineHeight,
          "line-height",
          0xCFCACE2E,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::MarginRight,
          "margin-right",
          0xD13C58C9,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeNumber |
              CFX_CSSVALUETYPE_MaybeEnum)},
    {PROP(CFX_CSSProperty::BorderLeftWidth,
          "border-left-width",
          0xD1E93D83,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::Display,
          "display",
          0xD4224C36,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum)},
    {PROP(CFX_CSSProperty::PaddingBottom,
          "padding-bottom",
          0xE555B3B9,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::BorderTopWidth,
          "border-top-width",
          0xED2CB62B,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::WordSpacing,
          "word-spacing",
          0xEDA63BAE,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::Left,
          "left",
          0xF5AD782B,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeEnum |
              CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::TextDecoration,
          "text-decoration",
          0xF7C634BA,
          CFX_CSSVALUETYPE_List | CFX_CSSVALUETYPE_MaybeEnum)},
    {PROP(CFX_CSSProperty::Padding,
          "padding",
          0xF8C373F7,
          CFX_CSSVALUETYPE_List | CFX_CSSVALUETYPE_MaybeNumber)},
    {PROP(CFX_CSSProperty::MarginBottom,
          "margin-bottom",
          0xF93485A0,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeNumber |
              CFX_CSSVALUETYPE_MaybeEnum)},
    {PROP(CFX_CSSProperty::MarginTop,
          "margin-top",
          0xFE51DCFE,
          CFX_CSSVALUETYPE_Primitive | CFX_CSSVALUETYPE_MaybeNumber |
              CFX_CSSVALUETYPE_MaybeEnum)},
};

#undef PROP

#undef PVAL
#define PVAL(a, b, c) a, c

const CFX_CSSData::PropertyValue propertyValueTable[] = {
    {PVAL(CFX_CSSPropertyValue::Bolder, "bolder", 0x009F1058)},
    {PVAL(CFX_CSSPropertyValue::None, "none", 0x048B6670)},
    {PVAL(CFX_CSSPropertyValue::Dot, "dot", 0x0A48CB27)},
    {PVAL(CFX_CSSPropertyValue::Sub, "sub", 0x0BD37FAA)},
    {PVAL(CFX_CSSPropertyValue::Top, "top", 0x0BEDAF33)},
    {PVAL(CFX_CSSPropertyValue::Right, "right", 0x193ADE3E)},
    {PVAL(CFX_CSSPropertyValue::Normal, "normal", 0x247CF3E9)},
    {PVAL(CFX_CSSPropertyValue::Auto, "auto", 0x2B35B6D9)},
    {PVAL(CFX_CSSPropertyValue::Text, "text", 0x2D08AF85)},
    {PVAL(CFX_CSSPropertyValue::XSmall, "x-small", 0x2D2FCAFE)},
    {PVAL(CFX_CSSPropertyValue::Thin, "thin", 0x2D574D53)},
    {PVAL(CFX_CSSPropertyValue::Small, "small", 0x316A3739)},
    {PVAL(CFX_CSSPropertyValue::Bottom, "bottom", 0x399F02B5)},
    {PVAL(CFX_CSSPropertyValue::Underline, "underline", 0x3A0273A6)},
    {PVAL(CFX_CSSPropertyValue::Double, "double", 0x3D98515B)},
    {PVAL(CFX_CSSPropertyValue::Lighter, "lighter", 0x45BEB7AF)},
    {PVAL(CFX_CSSPropertyValue::Oblique, "oblique", 0x53EBDDB1)},
    {PVAL(CFX_CSSPropertyValue::Super, "super", 0x6A4F842F)},
    {PVAL(CFX_CSSPropertyValue::Center, "center", 0x6C51AFC1)},
    {PVAL(CFX_CSSPropertyValue::XxLarge, "xx-large", 0x70BB1508)},
    {PVAL(CFX_CSSPropertyValue::Smaller, "smaller", 0x849769F0)},
    {PVAL(CFX_CSSPropertyValue::Baseline, "baseline", 0x87436BA3)},
    {PVAL(CFX_CSSPropertyValue::Thick, "thick", 0x8CC35EB3)},
    {PVAL(CFX_CSSPropertyValue::Justify, "justify", 0x8D269CAE)},
    {PVAL(CFX_CSSPropertyValue::Middle, "middle", 0x947FA00F)},
    {PVAL(CFX_CSSPropertyValue::Medium, "medium", 0xA084A381)},
    {PVAL(CFX_CSSPropertyValue::ListItem, "list-item", 0xA32382B8)},
    {PVAL(CFX_CSSPropertyValue::XxSmall, "xx-small", 0xADE1FC76)},
    {PVAL(CFX_CSSPropertyValue::Bold, "bold", 0xB18313A1)},
    {PVAL(CFX_CSSPropertyValue::SmallCaps, "small-caps", 0xB299428D)},
    {PVAL(CFX_CSSPropertyValue::Inline, "inline", 0xC02D649F)},
    {PVAL(CFX_CSSPropertyValue::Overline, "overline", 0xC0EC9FA4)},
    {PVAL(CFX_CSSPropertyValue::TextBottom, "text-bottom", 0xC7D08D87)},
    {PVAL(CFX_CSSPropertyValue::Larger, "larger", 0xCD3C409D)},
    {PVAL(CFX_CSSPropertyValue::InlineTable, "inline-table", 0xD131F494)},
    {PVAL(CFX_CSSPropertyValue::InlineBlock, "inline-block", 0xD26A8BD7)},
    {PVAL(CFX_CSSPropertyValue::Blink, "blink", 0xDC36E390)},
    {PVAL(CFX_CSSPropertyValue::Block, "block", 0xDCD480AB)},
    {PVAL(CFX_CSSPropertyValue::Italic, "italic", 0xE31D5396)},
    {PVAL(CFX_CSSPropertyValue::LineThrough, "line-through", 0xE4C5A276)},
    {PVAL(CFX_CSSPropertyValue::XLarge, "x-large", 0xF008E390)},
    {PVAL(CFX_CSSPropertyValue::Large, "large", 0xF4434FCB)},
    {PVAL(CFX_CSSPropertyValue::Left, "left", 0xF5AD782B)},
    {PVAL(CFX_CSSPropertyValue::TextTop, "text-top", 0xFCB58D45)},
};

#undef PVAL

const CFX_CSSData::LengthUnit lengthUnitTable[] = {
    {L"cm", CFX_CSSNumberType::CentiMeters}, {L"em", CFX_CSSNumberType::EMS},
    {L"ex", CFX_CSSNumberType::EXS},         {L"in", CFX_CSSNumberType::Inches},
    {L"mm", CFX_CSSNumberType::MilliMeters}, {L"pc", CFX_CSSNumberType::Picas},
    {L"pt", CFX_CSSNumberType::Points},      {L"px", CFX_CSSNumberType::Pixels},
};

// 16 colours from CSS 2.0 + alternate spelling of grey/gray.
const CFX_CSSData::Color colorTable[] = {
    {L"aqua", 0xff00ffff},    {L"black", 0xff000000}, {L"blue", 0xff0000ff},
    {L"fuchsia", 0xffff00ff}, {L"gray", 0xff808080},  {L"green", 0xff008000},
    {L"grey", 0xff808080},    {L"lime", 0xff00ff00},  {L"maroon", 0xff800000},
    {L"navy", 0xff000080},    {L"olive", 0xff808000}, {L"orange", 0xffffa500},
    {L"purple", 0xff800080},  {L"red", 0xffff0000},   {L"silver", 0xffc0c0c0},
    {L"teal", 0xff008080},    {L"white", 0xffffffff}, {L"yellow", 0xffffff00},
};

}  // namespace

const CFX_CSSData::Property* CFX_CSSData::GetPropertyByName(
    WideStringView name) {
  if (name.IsEmpty())
    return nullptr;

  uint32_t hash = FX_HashCode_GetW(name, true);
  auto* result =
      std::lower_bound(std::begin(propertyTable), std::end(propertyTable), hash,
                       [](const CFX_CSSData::Property& iter,
                          const uint32_t& hash) { return iter.dwHash < hash; });

  if (result != std::end(propertyTable) && result->dwHash == hash)
    return result;
  return nullptr;
}

const CFX_CSSData::Property* CFX_CSSData::GetPropertyByEnum(
    CFX_CSSProperty property) {
  return &propertyTable[static_cast<uint8_t>(property)];
}

const CFX_CSSData::PropertyValue* CFX_CSSData::GetPropertyValueByName(
    WideStringView wsName) {
  if (wsName.IsEmpty())
    return nullptr;

  uint32_t hash = FX_HashCode_GetW(wsName, true);
  auto* result = std::lower_bound(
      std::begin(propertyValueTable), std::end(propertyValueTable), hash,
      [](const PropertyValue& iter, const uint32_t& hash) {
        return iter.dwHash < hash;
      });

  if (result != std::end(propertyValueTable) && result->dwHash == hash)
    return result;
  return nullptr;
}

const CFX_CSSData::LengthUnit* CFX_CSSData::GetLengthUnitByName(
    WideStringView wsName) {
  if (wsName.IsEmpty() || wsName.GetLength() != 2)
    return nullptr;

  WideString lowerName = WideString(wsName);
  lowerName.MakeLower();

  for (auto* iter = std::begin(lengthUnitTable);
       iter != std::end(lengthUnitTable); ++iter) {
    if (lowerName.Compare(iter->value) == 0)
      return iter;
  }

  return nullptr;
}

const CFX_CSSData::Color* CFX_CSSData::GetColorByName(WideStringView wsName) {
  if (wsName.IsEmpty())
    return nullptr;

  WideString lowerName = WideString(wsName);
  lowerName.MakeLower();

  for (auto* iter = std::begin(colorTable); iter != std::end(colorTable);
       ++iter) {
    if (lowerName.Compare(iter->name) == 0)
      return iter;
  }
  return nullptr;
}
