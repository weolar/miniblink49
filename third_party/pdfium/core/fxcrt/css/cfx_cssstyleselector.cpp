// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_cssstyleselector.h"

#include <algorithm>
#include <utility>

#include "core/fxcrt/css/cfx_csscolorvalue.h"
#include "core/fxcrt/css/cfx_csscomputedstyle.h"
#include "core/fxcrt/css/cfx_csscustomproperty.h"
#include "core/fxcrt/css/cfx_cssdeclaration.h"
#include "core/fxcrt/css/cfx_cssenumvalue.h"
#include "core/fxcrt/css/cfx_csspropertyholder.h"
#include "core/fxcrt/css/cfx_cssselector.h"
#include "core/fxcrt/css/cfx_cssstylesheet.h"
#include "core/fxcrt/css/cfx_csssyntaxparser.h"
#include "core/fxcrt/css/cfx_cssvaluelist.h"
#include "third_party/base/logging.h"
#include "third_party/base/ptr_util.h"

CFX_CSSStyleSelector::CFX_CSSStyleSelector() : m_fDefFontSize(12.0f) {}

CFX_CSSStyleSelector::~CFX_CSSStyleSelector() {}

void CFX_CSSStyleSelector::SetDefFontSize(float fFontSize) {
  ASSERT(fFontSize > 0);
  m_fDefFontSize = fFontSize;
}

RetainPtr<CFX_CSSComputedStyle> CFX_CSSStyleSelector::CreateComputedStyle(
    CFX_CSSComputedStyle* pParentStyle) {
  auto pStyle = pdfium::MakeRetain<CFX_CSSComputedStyle>();
  if (pParentStyle)
    pStyle->m_InheritedData = pParentStyle->m_InheritedData;
  return pStyle;
}

void CFX_CSSStyleSelector::SetUAStyleSheet(
    std::unique_ptr<CFX_CSSStyleSheet> pSheet) {
  m_UAStyles = std::move(pSheet);
}

void CFX_CSSStyleSelector::UpdateStyleIndex() {
  m_UARules.Clear();
  m_UARules.AddRulesFrom(m_UAStyles.get());
}

std::vector<const CFX_CSSDeclaration*> CFX_CSSStyleSelector::MatchDeclarations(
    const WideString& tagname) {
  std::vector<const CFX_CSSDeclaration*> matchedDecls;
  if (m_UARules.CountSelectors() == 0 || tagname.IsEmpty())
    return matchedDecls;

  auto* rules = m_UARules.GetTagRuleData(tagname);
  if (!rules)
    return matchedDecls;

  for (const auto& d : *rules) {
    if (MatchSelector(tagname, d->pSelector))
      matchedDecls.push_back(d->pDeclaration);
  }
  return matchedDecls;
}

bool CFX_CSSStyleSelector::MatchSelector(const WideString& tagname,
                                         CFX_CSSSelector* pSel) {
  // TODO(dsinclair): The code only supports a single level of selector at this
  // point. None of the code using selectors required the complexity so lets
  // just say we don't support them to simplify the code for now.
  if (!pSel || pSel->GetNextSelector() ||
      pSel->GetType() == CFX_CSSSelectorType::Descendant) {
    return false;
  }
  return pSel->GetNameHash() == FX_HashCode_GetW(tagname.AsStringView(), true);
}

void CFX_CSSStyleSelector::ComputeStyle(
    const std::vector<const CFX_CSSDeclaration*>& declArray,
    const WideString& styleString,
    const WideString& alignString,
    CFX_CSSComputedStyle* pDest) {
  std::unique_ptr<CFX_CSSDeclaration> pDecl;
  if (!styleString.IsEmpty() || !alignString.IsEmpty()) {
    pDecl = pdfium::MakeUnique<CFX_CSSDeclaration>();

    if (!styleString.IsEmpty())
      AppendInlineStyle(pDecl.get(), styleString);
    if (!alignString.IsEmpty()) {
      pDecl->AddProperty(
          CFX_CSSData::GetPropertyByEnum(CFX_CSSProperty::TextAlign),
          alignString.AsStringView());
    }
  }
  ApplyDeclarations(declArray, pDecl.get(), pDest);
}

void CFX_CSSStyleSelector::ApplyDeclarations(
    const std::vector<const CFX_CSSDeclaration*>& declArray,
    const CFX_CSSDeclaration* extraDecl,
    CFX_CSSComputedStyle* pComputedStyle) {
  std::vector<const CFX_CSSPropertyHolder*> importants;
  std::vector<const CFX_CSSPropertyHolder*> normals;
  std::vector<const CFX_CSSCustomProperty*> customs;

  for (auto* decl : declArray)
    ExtractValues(decl, &importants, &normals, &customs);

  if (extraDecl)
    ExtractValues(extraDecl, &importants, &normals, &customs);

  for (auto* prop : normals)
    ApplyProperty(prop->eProperty, prop->pValue, pComputedStyle);

  for (auto* prop : customs)
    pComputedStyle->AddCustomStyle(*prop);

  for (auto* prop : importants)
    ApplyProperty(prop->eProperty, prop->pValue, pComputedStyle);
}

void CFX_CSSStyleSelector::ExtractValues(
    const CFX_CSSDeclaration* decl,
    std::vector<const CFX_CSSPropertyHolder*>* importants,
    std::vector<const CFX_CSSPropertyHolder*>* normals,
    std::vector<const CFX_CSSCustomProperty*>* custom) {
  for (const auto& holder : *decl) {
    if (holder->bImportant)
      importants->push_back(holder.get());
    else
      normals->push_back(holder.get());
  }
  for (auto it = decl->custom_begin(); it != decl->custom_end(); it++)
    custom->push_back(it->get());
}

void CFX_CSSStyleSelector::AppendInlineStyle(CFX_CSSDeclaration* pDecl,
                                             const WideString& style) {
  ASSERT(pDecl);
  ASSERT(!style.IsEmpty());

  auto pSyntax = pdfium::MakeUnique<CFX_CSSSyntaxParser>(
      style.c_str(), style.GetLength(), 32, true);
  int32_t iLen2 = 0;
  const CFX_CSSData::Property* property = nullptr;
  WideString wsName;
  while (1) {
    CFX_CSSSyntaxStatus eStatus = pSyntax->DoSyntaxParse();
    if (eStatus == CFX_CSSSyntaxStatus::PropertyName) {
      WideStringView strValue = pSyntax->GetCurrentString();
      property = CFX_CSSData::GetPropertyByName(strValue);
      if (!property)
        wsName = WideString(strValue);
    } else if (eStatus == CFX_CSSSyntaxStatus::PropertyValue) {
      if (property || iLen2 > 0) {
        WideStringView strValue = pSyntax->GetCurrentString();
        if (!strValue.IsEmpty()) {
          if (property)
            pDecl->AddProperty(property, strValue);
          else if (iLen2 > 0)
            pDecl->AddProperty(wsName, WideString(strValue));
        }
      }
    } else {
      break;
    }
  }
}

void CFX_CSSStyleSelector::ApplyProperty(CFX_CSSProperty eProperty,
                                         const RetainPtr<CFX_CSSValue>& pValue,
                                         CFX_CSSComputedStyle* pComputedStyle) {
  if (pValue->GetType() != CFX_CSSPrimitiveType::List) {
    CFX_CSSPrimitiveType eType = pValue->GetType();
    switch (eProperty) {
      case CFX_CSSProperty::Display:
        if (eType == CFX_CSSPrimitiveType::Enum) {
          pComputedStyle->m_NonInheritedData.m_eDisplay =
              ToDisplay(pValue.As<CFX_CSSEnumValue>()->Value());
        }
        break;
      case CFX_CSSProperty::FontSize: {
        float& fFontSize = pComputedStyle->m_InheritedData.m_fFontSize;
        if (eType == CFX_CSSPrimitiveType::Number) {
          fFontSize = pValue.As<CFX_CSSNumberValue>()->Apply(fFontSize);
        } else if (eType == CFX_CSSPrimitiveType::Enum) {
          fFontSize =
              ToFontSize(pValue.As<CFX_CSSEnumValue>()->Value(), fFontSize);
        }
      } break;
      case CFX_CSSProperty::LineHeight:
        if (eType == CFX_CSSPrimitiveType::Number) {
          RetainPtr<CFX_CSSNumberValue> v = pValue.As<CFX_CSSNumberValue>();
          if (v->Kind() == CFX_CSSNumberType::Number) {
            pComputedStyle->m_InheritedData.m_fLineHeight =
                v->Value() * pComputedStyle->m_InheritedData.m_fFontSize;
          } else {
            pComputedStyle->m_InheritedData.m_fLineHeight =
                v->Apply(pComputedStyle->m_InheritedData.m_fFontSize);
          }
        }
        break;
      case CFX_CSSProperty::TextAlign:
        if (eType == CFX_CSSPrimitiveType::Enum) {
          pComputedStyle->m_InheritedData.m_eTextAlign =
              ToTextAlign(pValue.As<CFX_CSSEnumValue>()->Value());
        }
        break;
      case CFX_CSSProperty::TextIndent:
        SetLengthWithPercent(pComputedStyle->m_InheritedData.m_TextIndent,
                             eType, pValue,
                             pComputedStyle->m_InheritedData.m_fFontSize);
        break;
      case CFX_CSSProperty::FontWeight:
        if (eType == CFX_CSSPrimitiveType::Enum) {
          pComputedStyle->m_InheritedData.m_wFontWeight =
              ToFontWeight(pValue.As<CFX_CSSEnumValue>()->Value());
        } else if (eType == CFX_CSSPrimitiveType::Number) {
          int32_t iValue =
              (int32_t)pValue.As<CFX_CSSNumberValue>()->Value() / 100;
          if (iValue >= 1 && iValue <= 9) {
            pComputedStyle->m_InheritedData.m_wFontWeight = iValue * 100;
          }
        }
        break;
      case CFX_CSSProperty::FontStyle:
        if (eType == CFX_CSSPrimitiveType::Enum) {
          pComputedStyle->m_InheritedData.m_eFontStyle =
              ToFontStyle(pValue.As<CFX_CSSEnumValue>()->Value());
        }
        break;
      case CFX_CSSProperty::Color:
        if (eType == CFX_CSSPrimitiveType::RGB) {
          pComputedStyle->m_InheritedData.m_dwFontColor =
              pValue.As<CFX_CSSColorValue>()->Value();
        }
        break;
      case CFX_CSSProperty::MarginLeft:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_MarginWidth.left, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasMargin = true;
        }
        break;
      case CFX_CSSProperty::MarginTop:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_MarginWidth.top, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasMargin = true;
        }
        break;
      case CFX_CSSProperty::MarginRight:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_MarginWidth.right, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasMargin = true;
        }
        break;
      case CFX_CSSProperty::MarginBottom:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_MarginWidth.bottom, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasMargin = true;
        }
        break;
      case CFX_CSSProperty::PaddingLeft:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_PaddingWidth.left, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasPadding = true;
        }
        break;
      case CFX_CSSProperty::PaddingTop:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_PaddingWidth.top, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasPadding = true;
        }
        break;
      case CFX_CSSProperty::PaddingRight:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_PaddingWidth.right, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasPadding = true;
        }
        break;
      case CFX_CSSProperty::PaddingBottom:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_PaddingWidth.bottom, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasPadding = true;
        }
        break;
      case CFX_CSSProperty::BorderLeftWidth:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_BorderWidth.left, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasBorder = true;
        }
        break;
      case CFX_CSSProperty::BorderTopWidth:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_BorderWidth.top, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasBorder = true;
        }
        break;
      case CFX_CSSProperty::BorderRightWidth:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_BorderWidth.right, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasBorder = true;
        }
        break;
      case CFX_CSSProperty::BorderBottomWidth:
        if (SetLengthWithPercent(
                pComputedStyle->m_NonInheritedData.m_BorderWidth.bottom, eType,
                pValue, pComputedStyle->m_InheritedData.m_fFontSize)) {
          pComputedStyle->m_NonInheritedData.m_bHasBorder = true;
        }
        break;
      case CFX_CSSProperty::VerticalAlign:
        if (eType == CFX_CSSPrimitiveType::Enum) {
          pComputedStyle->m_NonInheritedData.m_eVerticalAlign =
              ToVerticalAlign(pValue.As<CFX_CSSEnumValue>()->Value());
        } else if (eType == CFX_CSSPrimitiveType::Number) {
          pComputedStyle->m_NonInheritedData.m_eVerticalAlign =
              CFX_CSSVerticalAlign::Number;
          pComputedStyle->m_NonInheritedData.m_fVerticalAlign =
              pValue.As<CFX_CSSNumberValue>()->Apply(
                  pComputedStyle->m_InheritedData.m_fFontSize);
        }
        break;
      case CFX_CSSProperty::FontVariant:
        if (eType == CFX_CSSPrimitiveType::Enum) {
          pComputedStyle->m_InheritedData.m_eFontVariant =
              ToFontVariant(pValue.As<CFX_CSSEnumValue>()->Value());
        }
        break;
      case CFX_CSSProperty::LetterSpacing:
        if (eType == CFX_CSSPrimitiveType::Enum) {
          pComputedStyle->m_InheritedData.m_LetterSpacing.Set(
              CFX_CSSLengthUnit::Normal);
        } else if (eType == CFX_CSSPrimitiveType::Number) {
          if (pValue.As<CFX_CSSNumberValue>()->Kind() ==
              CFX_CSSNumberType::Percent) {
            break;
          }

          SetLengthWithPercent(pComputedStyle->m_InheritedData.m_LetterSpacing,
                               eType, pValue,
                               pComputedStyle->m_InheritedData.m_fFontSize);
        }
        break;
      case CFX_CSSProperty::WordSpacing:
        if (eType == CFX_CSSPrimitiveType::Enum) {
          pComputedStyle->m_InheritedData.m_WordSpacing.Set(
              CFX_CSSLengthUnit::Normal);
        } else if (eType == CFX_CSSPrimitiveType::Number) {
          if (pValue.As<CFX_CSSNumberValue>()->Kind() ==
              CFX_CSSNumberType::Percent) {
            break;
          }
          SetLengthWithPercent(pComputedStyle->m_InheritedData.m_WordSpacing,
                               eType, pValue,
                               pComputedStyle->m_InheritedData.m_fFontSize);
        }
        break;
      case CFX_CSSProperty::Top:
        SetLengthWithPercent(pComputedStyle->m_NonInheritedData.m_Top, eType,
                             pValue,
                             pComputedStyle->m_InheritedData.m_fFontSize);
        break;
      case CFX_CSSProperty::Bottom:
        SetLengthWithPercent(pComputedStyle->m_NonInheritedData.m_Bottom, eType,
                             pValue,
                             pComputedStyle->m_InheritedData.m_fFontSize);
        break;
      case CFX_CSSProperty::Left:
        SetLengthWithPercent(pComputedStyle->m_NonInheritedData.m_Left, eType,
                             pValue,
                             pComputedStyle->m_InheritedData.m_fFontSize);
        break;
      case CFX_CSSProperty::Right:
        SetLengthWithPercent(pComputedStyle->m_NonInheritedData.m_Right, eType,
                             pValue,
                             pComputedStyle->m_InheritedData.m_fFontSize);
        break;
      default:
        break;
    }
  } else if (pValue->GetType() == CFX_CSSPrimitiveType::List) {
    RetainPtr<CFX_CSSValueList> pList = pValue.As<CFX_CSSValueList>();
    int32_t iCount = pList->CountValues();
    if (iCount > 0) {
      switch (eProperty) {
        case CFX_CSSProperty::FontFamily:
          pComputedStyle->m_InheritedData.m_pFontFamily = pList;
          break;
        case CFX_CSSProperty::TextDecoration:
          pComputedStyle->m_NonInheritedData.m_dwTextDecoration =
              ToTextDecoration(pList);
          break;
        default:
          break;
      }
    }
  } else {
    NOTREACHED();
  }
}

CFX_CSSDisplay CFX_CSSStyleSelector::ToDisplay(CFX_CSSPropertyValue eValue) {
  switch (eValue) {
    case CFX_CSSPropertyValue::Block:
      return CFX_CSSDisplay::Block;
    case CFX_CSSPropertyValue::None:
      return CFX_CSSDisplay::None;
    case CFX_CSSPropertyValue::ListItem:
      return CFX_CSSDisplay::ListItem;
    case CFX_CSSPropertyValue::InlineTable:
      return CFX_CSSDisplay::InlineTable;
    case CFX_CSSPropertyValue::InlineBlock:
      return CFX_CSSDisplay::InlineBlock;
    case CFX_CSSPropertyValue::Inline:
    default:
      return CFX_CSSDisplay::Inline;
  }
}

CFX_CSSTextAlign CFX_CSSStyleSelector::ToTextAlign(
    CFX_CSSPropertyValue eValue) {
  switch (eValue) {
    case CFX_CSSPropertyValue::Center:
      return CFX_CSSTextAlign::Center;
    case CFX_CSSPropertyValue::Right:
      return CFX_CSSTextAlign::Right;
    case CFX_CSSPropertyValue::Justify:
      return CFX_CSSTextAlign::Justify;
    case CFX_CSSPropertyValue::Left:
    default:
      return CFX_CSSTextAlign::Left;
  }
}

uint16_t CFX_CSSStyleSelector::ToFontWeight(CFX_CSSPropertyValue eValue) {
  switch (eValue) {
    case CFX_CSSPropertyValue::Bold:
      return 700;
    case CFX_CSSPropertyValue::Bolder:
      return 900;
    case CFX_CSSPropertyValue::Lighter:
      return 200;
    case CFX_CSSPropertyValue::Normal:
    default:
      return 400;
  }
}

CFX_CSSFontStyle CFX_CSSStyleSelector::ToFontStyle(
    CFX_CSSPropertyValue eValue) {
  switch (eValue) {
    case CFX_CSSPropertyValue::Italic:
    case CFX_CSSPropertyValue::Oblique:
      return CFX_CSSFontStyle::Italic;
    default:
      return CFX_CSSFontStyle::Normal;
  }
}

bool CFX_CSSStyleSelector::SetLengthWithPercent(
    CFX_CSSLength& width,
    CFX_CSSPrimitiveType eType,
    const RetainPtr<CFX_CSSValue>& pValue,
    float fFontSize) {
  if (eType == CFX_CSSPrimitiveType::Number) {
    RetainPtr<CFX_CSSNumberValue> v = pValue.As<CFX_CSSNumberValue>();
    if (v->Kind() == CFX_CSSNumberType::Percent) {
      width.Set(CFX_CSSLengthUnit::Percent,
                pValue.As<CFX_CSSNumberValue>()->Value() / 100.0f);
      return width.NonZero();
    }

    float fValue = v->Apply(fFontSize);
    width.Set(CFX_CSSLengthUnit::Point, fValue);
    return width.NonZero();
  } else if (eType == CFX_CSSPrimitiveType::Enum) {
    switch (pValue.As<CFX_CSSEnumValue>()->Value()) {
      case CFX_CSSPropertyValue::Auto:
        width.Set(CFX_CSSLengthUnit::Auto);
        return true;
      case CFX_CSSPropertyValue::None:
        width.Set(CFX_CSSLengthUnit::None);
        return true;
      case CFX_CSSPropertyValue::Thin:
        width.Set(CFX_CSSLengthUnit::Point, 2);
        return true;
      case CFX_CSSPropertyValue::Medium:
        width.Set(CFX_CSSLengthUnit::Point, 3);
        return true;
      case CFX_CSSPropertyValue::Thick:
        width.Set(CFX_CSSLengthUnit::Point, 4);
        return true;
      default:
        return false;
    }
  }
  return false;
}

float CFX_CSSStyleSelector::ToFontSize(CFX_CSSPropertyValue eValue,
                                       float fCurFontSize) {
  switch (eValue) {
    case CFX_CSSPropertyValue::XxSmall:
      return m_fDefFontSize / 1.2f / 1.2f / 1.2f;
    case CFX_CSSPropertyValue::XSmall:
      return m_fDefFontSize / 1.2f / 1.2f;
    case CFX_CSSPropertyValue::Small:
      return m_fDefFontSize / 1.2f;
    case CFX_CSSPropertyValue::Medium:
      return m_fDefFontSize;
    case CFX_CSSPropertyValue::Large:
      return m_fDefFontSize * 1.2f;
    case CFX_CSSPropertyValue::XLarge:
      return m_fDefFontSize * 1.2f * 1.2f;
    case CFX_CSSPropertyValue::XxLarge:
      return m_fDefFontSize * 1.2f * 1.2f * 1.2f;
    case CFX_CSSPropertyValue::Larger:
      return fCurFontSize * 1.2f;
    case CFX_CSSPropertyValue::Smaller:
      return fCurFontSize / 1.2f;
    default:
      return fCurFontSize;
  }
}

CFX_CSSVerticalAlign CFX_CSSStyleSelector::ToVerticalAlign(
    CFX_CSSPropertyValue eValue) {
  switch (eValue) {
    case CFX_CSSPropertyValue::Middle:
      return CFX_CSSVerticalAlign::Middle;
    case CFX_CSSPropertyValue::Bottom:
      return CFX_CSSVerticalAlign::Bottom;
    case CFX_CSSPropertyValue::Super:
      return CFX_CSSVerticalAlign::Super;
    case CFX_CSSPropertyValue::Sub:
      return CFX_CSSVerticalAlign::Sub;
    case CFX_CSSPropertyValue::Top:
      return CFX_CSSVerticalAlign::Top;
    case CFX_CSSPropertyValue::TextTop:
      return CFX_CSSVerticalAlign::TextTop;
    case CFX_CSSPropertyValue::TextBottom:
      return CFX_CSSVerticalAlign::TextBottom;
    case CFX_CSSPropertyValue::Baseline:
    default:
      return CFX_CSSVerticalAlign::Baseline;
  }
}

uint32_t CFX_CSSStyleSelector::ToTextDecoration(
    const RetainPtr<CFX_CSSValueList>& pValue) {
  uint32_t dwDecoration = 0;
  for (int32_t i = pValue->CountValues() - 1; i >= 0; --i) {
    const RetainPtr<CFX_CSSValue> pVal = pValue->GetValue(i);
    if (pVal->GetType() != CFX_CSSPrimitiveType::Enum)
      continue;

    switch (pVal.As<CFX_CSSEnumValue>()->Value()) {
      case CFX_CSSPropertyValue::Underline:
        dwDecoration |= CFX_CSSTEXTDECORATION_Underline;
        break;
      case CFX_CSSPropertyValue::LineThrough:
        dwDecoration |= CFX_CSSTEXTDECORATION_LineThrough;
        break;
      case CFX_CSSPropertyValue::Overline:
        dwDecoration |= CFX_CSSTEXTDECORATION_Overline;
        break;
      case CFX_CSSPropertyValue::Blink:
        dwDecoration |= CFX_CSSTEXTDECORATION_Blink;
        break;
      case CFX_CSSPropertyValue::Double:
        dwDecoration |= CFX_CSSTEXTDECORATION_Double;
        break;
      default:
        break;
    }
  }
  return dwDecoration;
}

CFX_CSSFontVariant CFX_CSSStyleSelector::ToFontVariant(
    CFX_CSSPropertyValue eValue) {
  return eValue == CFX_CSSPropertyValue::SmallCaps
             ? CFX_CSSFontVariant::SmallCaps
             : CFX_CSSFontVariant::Normal;
}
