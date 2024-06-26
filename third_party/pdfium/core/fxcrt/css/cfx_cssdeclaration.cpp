// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_cssdeclaration.h"

#include <utility>

#include "core/fxcrt/css/cfx_csscolorvalue.h"
#include "core/fxcrt/css/cfx_csscustomproperty.h"
#include "core/fxcrt/css/cfx_cssenumvalue.h"
#include "core/fxcrt/css/cfx_cssnumbervalue.h"
#include "core/fxcrt/css/cfx_csspropertyholder.h"
#include "core/fxcrt/css/cfx_cssstringvalue.h"
#include "core/fxcrt/css/cfx_cssvaluelist.h"
#include "core/fxcrt/css/cfx_cssvaluelistparser.h"
#include "core/fxcrt/fx_extension.h"
#include "third_party/base/logging.h"
#include "third_party/base/ptr_util.h"

namespace {

uint8_t Hex2Dec(uint8_t hexHigh, uint8_t hexLow) {
  return (FXSYS_HexCharToInt(hexHigh) << 4) + FXSYS_HexCharToInt(hexLow);
}

bool ParseCSSNumber(const wchar_t* pszValue,
                    int32_t iValueLen,
                    float& fValue,
                    CFX_CSSNumberType& eUnit) {
  ASSERT(pszValue);
  ASSERT(iValueLen > 0);

  int32_t iUsedLen = 0;
  fValue = FXSYS_wcstof(pszValue, iValueLen, &iUsedLen);
  if (iUsedLen <= 0)
    return false;

  iValueLen -= iUsedLen;
  pszValue += iUsedLen;
  eUnit = CFX_CSSNumberType::Number;
  if (iValueLen >= 1 && *pszValue == '%') {
    eUnit = CFX_CSSNumberType::Percent;
  } else if (iValueLen == 2) {
    const CFX_CSSData::LengthUnit* pUnit =
        CFX_CSSData::GetLengthUnitByName(WideStringView(pszValue, 2));
    if (pUnit)
      eUnit = pUnit->type;
  }
  return true;
}

}  // namespace

// static
bool CFX_CSSDeclaration::ParseCSSString(const wchar_t* pszValue,
                                        int32_t iValueLen,
                                        int32_t* iOffset,
                                        int32_t* iLength) {
  ASSERT(pszValue);
  ASSERT(iValueLen > 0);

  *iOffset = 0;
  *iLength = iValueLen;
  if (iValueLen >= 2) {
    wchar_t first = pszValue[0], last = pszValue[iValueLen - 1];
    if ((first == '\"' && last == '\"') || (first == '\'' && last == '\'')) {
      *iOffset = 1;
      *iLength -= 2;
    }
  }
  return iValueLen > 0;
}

// static.
bool CFX_CSSDeclaration::ParseCSSColor(const wchar_t* pszValue,
                                       int32_t iValueLen,
                                       FX_ARGB* dwColor) {
  ASSERT(pszValue);
  ASSERT(iValueLen > 0);
  ASSERT(dwColor);

  if (*pszValue == '#') {
    switch (iValueLen) {
      case 4: {
        uint8_t red = Hex2Dec((uint8_t)pszValue[1], (uint8_t)pszValue[1]);
        uint8_t green = Hex2Dec((uint8_t)pszValue[2], (uint8_t)pszValue[2]);
        uint8_t blue = Hex2Dec((uint8_t)pszValue[3], (uint8_t)pszValue[3]);
        *dwColor = ArgbEncode(255, red, green, blue);
        return true;
      }
      case 7: {
        uint8_t red = Hex2Dec((uint8_t)pszValue[1], (uint8_t)pszValue[2]);
        uint8_t green = Hex2Dec((uint8_t)pszValue[3], (uint8_t)pszValue[4]);
        uint8_t blue = Hex2Dec((uint8_t)pszValue[5], (uint8_t)pszValue[6]);
        *dwColor = ArgbEncode(255, red, green, blue);
        return true;
      }
      default:
        return false;
    }
  }

  if (iValueLen >= 10) {
    if (pszValue[iValueLen - 1] != ')' || FXSYS_wcsnicmp(L"rgb(", pszValue, 4))
      return false;

    uint8_t rgb[3] = {0};
    float fValue;
    CFX_CSSPrimitiveType eType;
    CFX_CSSValueListParser list(pszValue + 4, iValueLen - 5, ',');
    for (int32_t i = 0; i < 3; ++i) {
      if (!list.NextValue(&eType, &pszValue, &iValueLen))
        return false;
      if (eType != CFX_CSSPrimitiveType::Number)
        return false;
      CFX_CSSNumberType eNumType;
      if (!ParseCSSNumber(pszValue, iValueLen, fValue, eNumType))
        return false;

      rgb[i] = eNumType == CFX_CSSNumberType::Percent
                   ? FXSYS_round(fValue * 2.55f)
                   : FXSYS_round(fValue);
    }
    *dwColor = ArgbEncode(255, rgb[0], rgb[1], rgb[2]);
    return true;
  }

  const CFX_CSSData::Color* pColor =
      CFX_CSSData::GetColorByName(WideStringView(pszValue, iValueLen));
  if (!pColor)
    return false;

  *dwColor = pColor->value;
  return true;
}

CFX_CSSDeclaration::CFX_CSSDeclaration() {}

CFX_CSSDeclaration::~CFX_CSSDeclaration() {}

RetainPtr<CFX_CSSValue> CFX_CSSDeclaration::GetProperty(
    CFX_CSSProperty eProperty,
    bool* bImportant) const {
  for (const auto& p : properties_) {
    if (p->eProperty == eProperty) {
      *bImportant = p->bImportant;
      return p->pValue;
    }
  }
  return nullptr;
}

void CFX_CSSDeclaration::AddPropertyHolder(CFX_CSSProperty eProperty,
                                           RetainPtr<CFX_CSSValue> pValue,
                                           bool bImportant) {
  auto pHolder = pdfium::MakeUnique<CFX_CSSPropertyHolder>();
  pHolder->bImportant = bImportant;
  pHolder->eProperty = eProperty;
  pHolder->pValue = pValue;
  properties_.push_back(std::move(pHolder));
}

void CFX_CSSDeclaration::AddProperty(const CFX_CSSData::Property* property,
                                     WideStringView value) {
  ASSERT(!value.IsEmpty());

  const wchar_t* pszValue = value.unterminated_c_str();
  int32_t iValueLen = value.GetLength();
  bool bImportant = false;
  if (iValueLen >= 10 && pszValue[iValueLen - 10] == '!' &&
      FXSYS_wcsnicmp(L"important", pszValue + iValueLen - 9, 9) == 0) {
    if ((iValueLen -= 10) == 0)
      return;

    bImportant = true;
  }
  const uint32_t dwType = property->dwType;
  switch (dwType & 0x0F) {
    case CFX_CSSVALUETYPE_Primitive: {
      static constexpr CFX_CSSVALUETYPE kValueGuessOrder[] = {
          CFX_CSSVALUETYPE_MaybeNumber, CFX_CSSVALUETYPE_MaybeEnum,
          CFX_CSSVALUETYPE_MaybeColor, CFX_CSSVALUETYPE_MaybeString,
      };
      for (uint32_t guess : kValueGuessOrder) {
        const uint32_t dwMatch = dwType & guess;
        if (dwMatch == 0)
          continue;

        RetainPtr<CFX_CSSValue> pCSSValue;
        switch (dwMatch) {
          case CFX_CSSVALUETYPE_MaybeNumber:
            pCSSValue = ParseNumber(pszValue, iValueLen);
            break;
          case CFX_CSSVALUETYPE_MaybeEnum:
            pCSSValue = ParseEnum(pszValue, iValueLen);
            break;
          case CFX_CSSVALUETYPE_MaybeColor:
            pCSSValue = ParseColor(pszValue, iValueLen);
            break;
          case CFX_CSSVALUETYPE_MaybeString:
            pCSSValue = ParseString(pszValue, iValueLen);
            break;
          default:
            break;
        }
        if (pCSSValue) {
          AddPropertyHolder(property->eName, pCSSValue, bImportant);
          return;
        }

        if ((dwType & ~guess) == CFX_CSSVALUETYPE_Primitive)
          return;
      }
      break;
    }
    case CFX_CSSVALUETYPE_Shorthand: {
      RetainPtr<CFX_CSSValue> pWidth;
      switch (property->eName) {
        case CFX_CSSProperty::Font:
          ParseFontProperty(pszValue, iValueLen, bImportant);
          return;
        case CFX_CSSProperty::Border:
          if (ParseBorderProperty(pszValue, iValueLen, pWidth)) {
            AddPropertyHolder(CFX_CSSProperty::BorderLeftWidth, pWidth,
                              bImportant);
            AddPropertyHolder(CFX_CSSProperty::BorderTopWidth, pWidth,
                              bImportant);
            AddPropertyHolder(CFX_CSSProperty::BorderRightWidth, pWidth,
                              bImportant);
            AddPropertyHolder(CFX_CSSProperty::BorderBottomWidth, pWidth,
                              bImportant);
            return;
          }
          break;
        case CFX_CSSProperty::BorderLeft:
          if (ParseBorderProperty(pszValue, iValueLen, pWidth)) {
            AddPropertyHolder(CFX_CSSProperty::BorderLeftWidth, pWidth,
                              bImportant);
            return;
          }
          break;
        case CFX_CSSProperty::BorderTop:
          if (ParseBorderProperty(pszValue, iValueLen, pWidth)) {
            AddPropertyHolder(CFX_CSSProperty::BorderTopWidth, pWidth,
                              bImportant);
            return;
          }
          break;
        case CFX_CSSProperty::BorderRight:
          if (ParseBorderProperty(pszValue, iValueLen, pWidth)) {
            AddPropertyHolder(CFX_CSSProperty::BorderRightWidth, pWidth,
                              bImportant);
            return;
          }
          break;
        case CFX_CSSProperty::BorderBottom:
          if (ParseBorderProperty(pszValue, iValueLen, pWidth)) {
            AddPropertyHolder(CFX_CSSProperty::BorderBottomWidth, pWidth,
                              bImportant);
            return;
          }
          break;
        default:
          break;
      }
    } break;
    case CFX_CSSVALUETYPE_List:
      ParseValueListProperty(property, pszValue, iValueLen, bImportant);
      return;
    default:
      NOTREACHED();
      break;
  }
}

void CFX_CSSDeclaration::AddProperty(const WideString& prop,
                                     const WideString& value) {
  custom_properties_.push_back(
      pdfium::MakeUnique<CFX_CSSCustomProperty>(prop, value));
}

RetainPtr<CFX_CSSValue> CFX_CSSDeclaration::ParseNumber(const wchar_t* pszValue,
                                                        int32_t iValueLen) {
  float fValue;
  CFX_CSSNumberType eUnit;
  if (!ParseCSSNumber(pszValue, iValueLen, fValue, eUnit))
    return nullptr;
  return pdfium::MakeRetain<CFX_CSSNumberValue>(eUnit, fValue);
}

RetainPtr<CFX_CSSValue> CFX_CSSDeclaration::ParseEnum(const wchar_t* pszValue,
                                                      int32_t iValueLen) {
  const CFX_CSSData::PropertyValue* pValue =
      CFX_CSSData::GetPropertyValueByName(WideStringView(pszValue, iValueLen));
  return pValue ? pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName) : nullptr;
}

RetainPtr<CFX_CSSValue> CFX_CSSDeclaration::ParseColor(const wchar_t* pszValue,
                                                       int32_t iValueLen) {
  FX_ARGB dwColor;
  if (!ParseCSSColor(pszValue, iValueLen, &dwColor))
    return nullptr;
  return pdfium::MakeRetain<CFX_CSSColorValue>(dwColor);
}

RetainPtr<CFX_CSSValue> CFX_CSSDeclaration::ParseString(const wchar_t* pszValue,
                                                        int32_t iValueLen) {
  int32_t iOffset;
  if (!ParseCSSString(pszValue, iValueLen, &iOffset, &iValueLen))
    return nullptr;

  if (iValueLen <= 0)
    return nullptr;

  return pdfium::MakeRetain<CFX_CSSStringValue>(
      WideString(pszValue + iOffset, iValueLen));
}

void CFX_CSSDeclaration::ParseValueListProperty(
    const CFX_CSSData::Property* pProperty,
    const wchar_t* pszValue,
    int32_t iValueLen,
    bool bImportant) {
  wchar_t separator =
      (pProperty->eName == CFX_CSSProperty::FontFamily) ? ',' : ' ';
  CFX_CSSValueListParser parser(pszValue, iValueLen, separator);

  const uint32_t dwType = pProperty->dwType;
  CFX_CSSPrimitiveType eType;
  std::vector<RetainPtr<CFX_CSSValue>> list;
  while (parser.NextValue(&eType, &pszValue, &iValueLen)) {
    switch (eType) {
      case CFX_CSSPrimitiveType::Number:
        if (dwType & CFX_CSSVALUETYPE_MaybeNumber) {
          float fValue;
          CFX_CSSNumberType eNumType;
          if (ParseCSSNumber(pszValue, iValueLen, fValue, eNumType))
            list.push_back(
                pdfium::MakeRetain<CFX_CSSNumberValue>(eNumType, fValue));
        }
        break;
      case CFX_CSSPrimitiveType::String:
        if (dwType & CFX_CSSVALUETYPE_MaybeColor) {
          FX_ARGB dwColor;
          if (ParseCSSColor(pszValue, iValueLen, &dwColor)) {
            list.push_back(pdfium::MakeRetain<CFX_CSSColorValue>(dwColor));
            continue;
          }
        }
        if (dwType & CFX_CSSVALUETYPE_MaybeEnum) {
          const CFX_CSSData::PropertyValue* pValue =
              CFX_CSSData::GetPropertyValueByName(
                  WideStringView(pszValue, iValueLen));
          if (pValue) {
            list.push_back(pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName));
            continue;
          }
        }
        if (dwType & CFX_CSSVALUETYPE_MaybeString) {
          list.push_back(pdfium::MakeRetain<CFX_CSSStringValue>(
              WideString(pszValue, iValueLen)));
        }
        break;
      case CFX_CSSPrimitiveType::RGB:
        if (dwType & CFX_CSSVALUETYPE_MaybeColor) {
          FX_ARGB dwColor;
          if (ParseCSSColor(pszValue, iValueLen, &dwColor)) {
            list.push_back(pdfium::MakeRetain<CFX_CSSColorValue>(dwColor));
          }
        }
        break;
      default:
        break;
    }
  }
  if (list.empty())
    return;

  switch (pProperty->eName) {
    case CFX_CSSProperty::BorderWidth:
      Add4ValuesProperty(list, bImportant, CFX_CSSProperty::BorderLeftWidth,
                         CFX_CSSProperty::BorderTopWidth,
                         CFX_CSSProperty::BorderRightWidth,
                         CFX_CSSProperty::BorderBottomWidth);
      return;
    case CFX_CSSProperty::Margin:
      Add4ValuesProperty(list, bImportant, CFX_CSSProperty::MarginLeft,
                         CFX_CSSProperty::MarginTop,
                         CFX_CSSProperty::MarginRight,
                         CFX_CSSProperty::MarginBottom);
      return;
    case CFX_CSSProperty::Padding:
      Add4ValuesProperty(list, bImportant, CFX_CSSProperty::PaddingLeft,
                         CFX_CSSProperty::PaddingTop,
                         CFX_CSSProperty::PaddingRight,
                         CFX_CSSProperty::PaddingBottom);
      return;
    default: {
      auto pList = pdfium::MakeRetain<CFX_CSSValueList>(list);
      AddPropertyHolder(pProperty->eName, pList, bImportant);
      return;
    }
  }
}

void CFX_CSSDeclaration::Add4ValuesProperty(
    const std::vector<RetainPtr<CFX_CSSValue>>& list,
    bool bImportant,
    CFX_CSSProperty eLeft,
    CFX_CSSProperty eTop,
    CFX_CSSProperty eRight,
    CFX_CSSProperty eBottom) {
  switch (list.size()) {
    case 1:
      AddPropertyHolder(eLeft, list[0], bImportant);
      AddPropertyHolder(eTop, list[0], bImportant);
      AddPropertyHolder(eRight, list[0], bImportant);
      AddPropertyHolder(eBottom, list[0], bImportant);
      return;
    case 2:
      AddPropertyHolder(eLeft, list[1], bImportant);
      AddPropertyHolder(eTop, list[0], bImportant);
      AddPropertyHolder(eRight, list[1], bImportant);
      AddPropertyHolder(eBottom, list[0], bImportant);
      return;
    case 3:
      AddPropertyHolder(eLeft, list[1], bImportant);
      AddPropertyHolder(eTop, list[0], bImportant);
      AddPropertyHolder(eRight, list[1], bImportant);
      AddPropertyHolder(eBottom, list[2], bImportant);
      return;
    case 4:
      AddPropertyHolder(eLeft, list[3], bImportant);
      AddPropertyHolder(eTop, list[0], bImportant);
      AddPropertyHolder(eRight, list[1], bImportant);
      AddPropertyHolder(eBottom, list[2], bImportant);
      return;
    default:
      break;
  }
}

bool CFX_CSSDeclaration::ParseBorderProperty(
    const wchar_t* pszValue,
    int32_t iValueLen,
    RetainPtr<CFX_CSSValue>& pWidth) const {
  pWidth.Reset(nullptr);

  CFX_CSSValueListParser parser(pszValue, iValueLen, ' ');
  CFX_CSSPrimitiveType eType;
  while (parser.NextValue(&eType, &pszValue, &iValueLen)) {
    switch (eType) {
      case CFX_CSSPrimitiveType::Number: {
        if (pWidth)
          continue;

        float fValue;
        CFX_CSSNumberType eNumType;
        if (ParseCSSNumber(pszValue, iValueLen, fValue, eNumType))
          pWidth = pdfium::MakeRetain<CFX_CSSNumberValue>(eNumType, fValue);
        break;
      }
      case CFX_CSSPrimitiveType::String: {
        const CFX_CSSData::Color* pColorItem =
            CFX_CSSData::GetColorByName(WideStringView(pszValue, iValueLen));
        if (pColorItem)
          continue;

        const CFX_CSSData::PropertyValue* pValue =
            CFX_CSSData::GetPropertyValueByName(
                WideStringView(pszValue, iValueLen));
        if (!pValue)
          continue;

        switch (pValue->eName) {
          case CFX_CSSPropertyValue::Thin:
          case CFX_CSSPropertyValue::Thick:
          case CFX_CSSPropertyValue::Medium:
            if (!pWidth)
              pWidth = pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName);
            break;
          default:
            break;
        }
        break;
      }
      default:
        break;
    }
  }
  if (!pWidth)
    pWidth =
        pdfium::MakeRetain<CFX_CSSNumberValue>(CFX_CSSNumberType::Number, 0.0f);

  return true;
}

void CFX_CSSDeclaration::ParseFontProperty(const wchar_t* pszValue,
                                           int32_t iValueLen,
                                           bool bImportant) {
  CFX_CSSValueListParser parser(pszValue, iValueLen, '/');
  RetainPtr<CFX_CSSValue> pStyle;
  RetainPtr<CFX_CSSValue> pVariant;
  RetainPtr<CFX_CSSValue> pWeight;
  RetainPtr<CFX_CSSValue> pFontSize;
  RetainPtr<CFX_CSSValue> pLineHeight;
  std::vector<RetainPtr<CFX_CSSValue>> familyList;
  CFX_CSSPrimitiveType eType;
  while (parser.NextValue(&eType, &pszValue, &iValueLen)) {
    switch (eType) {
      case CFX_CSSPrimitiveType::String: {
        const CFX_CSSData::PropertyValue* pValue =
            CFX_CSSData::GetPropertyValueByName(
                WideStringView(pszValue, iValueLen));
        if (pValue) {
          switch (pValue->eName) {
            case CFX_CSSPropertyValue::XxSmall:
            case CFX_CSSPropertyValue::XSmall:
            case CFX_CSSPropertyValue::Small:
            case CFX_CSSPropertyValue::Medium:
            case CFX_CSSPropertyValue::Large:
            case CFX_CSSPropertyValue::XLarge:
            case CFX_CSSPropertyValue::XxLarge:
            case CFX_CSSPropertyValue::Smaller:
            case CFX_CSSPropertyValue::Larger:
              if (!pFontSize)
                pFontSize = pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName);
              continue;
            case CFX_CSSPropertyValue::Bold:
            case CFX_CSSPropertyValue::Bolder:
            case CFX_CSSPropertyValue::Lighter:
              if (!pWeight)
                pWeight = pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName);
              continue;
            case CFX_CSSPropertyValue::Italic:
            case CFX_CSSPropertyValue::Oblique:
              if (!pStyle)
                pStyle = pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName);
              continue;
            case CFX_CSSPropertyValue::SmallCaps:
              if (!pVariant)
                pVariant = pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName);
              continue;
            case CFX_CSSPropertyValue::Normal:
              if (!pStyle)
                pStyle = pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName);
              else if (!pVariant)
                pVariant = pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName);
              else if (!pWeight)
                pWeight = pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName);
              else if (!pFontSize)
                pFontSize = pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName);
              else if (!pLineHeight)
                pLineHeight =
                    pdfium::MakeRetain<CFX_CSSEnumValue>(pValue->eName);
              continue;
            default:
              break;
          }
        }
        if (pFontSize) {
          familyList.push_back(pdfium::MakeRetain<CFX_CSSStringValue>(
              WideString(pszValue, iValueLen)));
        }
        parser.UseCommaSeparator();
        break;
      }
      case CFX_CSSPrimitiveType::Number: {
        float fValue;
        CFX_CSSNumberType eNumType;
        if (!ParseCSSNumber(pszValue, iValueLen, fValue, eNumType))
          break;
        if (eType == CFX_CSSPrimitiveType::Number) {
          switch ((int32_t)fValue) {
            case 100:
            case 200:
            case 300:
            case 400:
            case 500:
            case 600:
            case 700:
            case 800:
            case 900:
              if (!pWeight)
                pWeight = pdfium::MakeRetain<CFX_CSSNumberValue>(
                    CFX_CSSNumberType::Number, fValue);
              continue;
          }
        }
        if (!pFontSize)
          pFontSize = pdfium::MakeRetain<CFX_CSSNumberValue>(eNumType, fValue);
        else if (!pLineHeight)
          pLineHeight =
              pdfium::MakeRetain<CFX_CSSNumberValue>(eNumType, fValue);
        break;
      }
      default:
        break;
    }
  }

  if (!pStyle) {
    pStyle = pdfium::MakeRetain<CFX_CSSEnumValue>(CFX_CSSPropertyValue::Normal);
  }
  if (!pVariant) {
    pVariant =
        pdfium::MakeRetain<CFX_CSSEnumValue>(CFX_CSSPropertyValue::Normal);
  }
  if (!pWeight) {
    pWeight =
        pdfium::MakeRetain<CFX_CSSEnumValue>(CFX_CSSPropertyValue::Normal);
  }
  if (!pFontSize) {
    pFontSize =
        pdfium::MakeRetain<CFX_CSSEnumValue>(CFX_CSSPropertyValue::Medium);
  }
  if (!pLineHeight) {
    pLineHeight =
        pdfium::MakeRetain<CFX_CSSEnumValue>(CFX_CSSPropertyValue::Normal);
  }

  AddPropertyHolder(CFX_CSSProperty::FontStyle, pStyle, bImportant);
  AddPropertyHolder(CFX_CSSProperty::FontVariant, pVariant, bImportant);
  AddPropertyHolder(CFX_CSSProperty::FontWeight, pWeight, bImportant);
  AddPropertyHolder(CFX_CSSProperty::FontSize, pFontSize, bImportant);
  AddPropertyHolder(CFX_CSSProperty::LineHeight, pLineHeight, bImportant);
  if (!familyList.empty()) {
    auto pList = pdfium::MakeRetain<CFX_CSSValueList>(familyList);
    AddPropertyHolder(CFX_CSSProperty::FontFamily, pList, bImportant);
  }
}

size_t CFX_CSSDeclaration::PropertyCountForTesting() const {
  return properties_.size();
}
