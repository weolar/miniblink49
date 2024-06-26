// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_textparser.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "core/fxcrt/css/cfx_css.h"
#include "core/fxcrt/css/cfx_csscomputedstyle.h"
#include "core/fxcrt/css/cfx_cssstyleselector.h"
#include "core/fxcrt/css/cfx_cssstylesheet.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmlnode.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fgas/font/cfgas_fontmgr.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_fontmgr.h"
#include "xfa/fxfa/cxfa_textparsecontext.h"
#include "xfa/fxfa/cxfa_textprovider.h"
#include "xfa/fxfa/cxfa_texttabstopscontext.h"
#include "xfa/fxfa/parser/cxfa_font.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"
#include "xfa/fxfa/parser/cxfa_para.h"

namespace {

enum class TabStopStatus {
  Error,
  EOS,
  None,
  Alignment,
  StartLeader,
  Leader,
  Location,
};

WideString GetLowerCaseElementAttributeOrDefault(
    const CFX_XMLElement* pElement,
    const WideString& wsName,
    const WideString& wsDefaultValue) {
  WideString ws = pElement->GetAttribute(wsName);
  if (ws.IsEmpty())
    ws = wsDefaultValue;
  else
    ws.MakeLower();
  return ws;
}

}  // namespace

CXFA_TextParser::CXFA_TextParser()
    : m_bParsed(false), m_cssInitialized(false) {}

CXFA_TextParser::~CXFA_TextParser() {}

void CXFA_TextParser::Reset() {
  m_mapXMLNodeToParseContext.clear();
  m_bParsed = false;
}

void CXFA_TextParser::InitCSSData(CXFA_TextProvider* pTextProvider) {
  if (!pTextProvider)
    return;

  if (!m_pSelector) {
    m_pSelector = pdfium::MakeUnique<CFX_CSSStyleSelector>();

    CXFA_Font* font = pTextProvider->GetFontIfExists();
    m_pSelector->SetDefFontSize(font ? font->GetFontSize() : 10.0f);
  }

  if (m_cssInitialized)
    return;

  m_cssInitialized = true;
  auto uaSheet = LoadDefaultSheetStyle();
  m_pSelector->SetUAStyleSheet(std::move(uaSheet));
  m_pSelector->UpdateStyleIndex();
}

std::unique_ptr<CFX_CSSStyleSheet> CXFA_TextParser::LoadDefaultSheetStyle() {
  static const char kStyle[] =
      "html,body,ol,p,ul{display:block}"
      "li{display:list-item}"
      "ol,ul{padding-left:33px;margin:1.12em 0}"
      "ol{list-style-type:decimal}"
      "a{color:#0000ff;text-decoration:underline}"
      "b{font-weight:bolder}"
      "i{font-style:italic}"
      "sup{vertical-align:+15em;font-size:.66em}"
      "sub{vertical-align:-15em;font-size:.66em}";
  WideString ws = WideString::FromASCII(kStyle);
  auto sheet = pdfium::MakeUnique<CFX_CSSStyleSheet>();
  if (!sheet->LoadBuffer(ws.c_str(), ws.GetLength()))
    return nullptr;

  return sheet;
}

RetainPtr<CFX_CSSComputedStyle> CXFA_TextParser::CreateRootStyle(
    CXFA_TextProvider* pTextProvider) {
  CXFA_Para* para = pTextProvider->GetParaIfExists();
  auto pStyle = m_pSelector->CreateComputedStyle(nullptr);
  float fLineHeight = 0;
  float fFontSize = 10;

  if (para) {
    fLineHeight = para->GetLineHeight();
    CFX_CSSLength indent;
    indent.Set(CFX_CSSLengthUnit::Point, para->GetTextIndent());
    pStyle->SetTextIndent(indent);
    CFX_CSSTextAlign hAlign = CFX_CSSTextAlign::Left;
    switch (para->GetHorizontalAlign()) {
      case XFA_AttributeValue::Center:
        hAlign = CFX_CSSTextAlign::Center;
        break;
      case XFA_AttributeValue::Right:
        hAlign = CFX_CSSTextAlign::Right;
        break;
      case XFA_AttributeValue::Justify:
        hAlign = CFX_CSSTextAlign::Justify;
        break;
      case XFA_AttributeValue::JustifyAll:
        hAlign = CFX_CSSTextAlign::JustifyAll;
        break;
      case XFA_AttributeValue::Left:
      case XFA_AttributeValue::Radix:
        break;
      default:
        NOTREACHED();
        break;
    }
    pStyle->SetTextAlign(hAlign);
    CFX_CSSRect rtMarginWidth;
    rtMarginWidth.left.Set(CFX_CSSLengthUnit::Point, para->GetMarginLeft());
    rtMarginWidth.top.Set(CFX_CSSLengthUnit::Point, para->GetSpaceAbove());
    rtMarginWidth.right.Set(CFX_CSSLengthUnit::Point, para->GetMarginRight());
    rtMarginWidth.bottom.Set(CFX_CSSLengthUnit::Point, para->GetSpaceBelow());
    pStyle->SetMarginWidth(rtMarginWidth);
  }

  CXFA_Font* font = pTextProvider->GetFontIfExists();
  if (font) {
    pStyle->SetColor(font->GetColor());
    pStyle->SetFontStyle(font->IsItalic() ? CFX_CSSFontStyle::Italic
                                          : CFX_CSSFontStyle::Normal);
    pStyle->SetFontWeight(font->IsBold() ? FXFONT_FW_BOLD : FXFONT_FW_NORMAL);
    pStyle->SetNumberVerticalAlign(-font->GetBaselineShift());
    fFontSize = font->GetFontSize();
    CFX_CSSLength letterSpacing;
    letterSpacing.Set(CFX_CSSLengthUnit::Point, font->GetLetterSpacing());
    pStyle->SetLetterSpacing(letterSpacing);
    uint32_t dwDecoration = 0;
    if (font->GetLineThrough() > 0)
      dwDecoration |= CFX_CSSTEXTDECORATION_LineThrough;
    if (font->GetUnderline() > 1)
      dwDecoration |= CFX_CSSTEXTDECORATION_Double;
    else if (font->GetUnderline() > 0)
      dwDecoration |= CFX_CSSTEXTDECORATION_Underline;

    pStyle->SetTextDecoration(dwDecoration);
  }
  pStyle->SetLineHeight(fLineHeight);
  pStyle->SetFontSize(fFontSize);
  return pStyle;
}

RetainPtr<CFX_CSSComputedStyle> CXFA_TextParser::CreateStyle(
    CFX_CSSComputedStyle* pParentStyle) {
  auto pNewStyle = m_pSelector->CreateComputedStyle(pParentStyle);
  ASSERT(pNewStyle);
  if (!pParentStyle)
    return pNewStyle;

  uint32_t dwDecoration = pParentStyle->GetTextDecoration();
  float fBaseLine = 0;
  if (pParentStyle->GetVerticalAlign() == CFX_CSSVerticalAlign::Number)
    fBaseLine = pParentStyle->GetNumberVerticalAlign();

  pNewStyle->SetTextDecoration(dwDecoration);
  pNewStyle->SetNumberVerticalAlign(fBaseLine);

  const CFX_CSSRect* pRect = pParentStyle->GetMarginWidth();
  if (pRect)
    pNewStyle->SetMarginWidth(*pRect);
  return pNewStyle;
}

RetainPtr<CFX_CSSComputedStyle> CXFA_TextParser::ComputeStyle(
    const CFX_XMLNode* pXMLNode,
    CFX_CSSComputedStyle* pParentStyle) {
  auto it = m_mapXMLNodeToParseContext.find(pXMLNode);
  if (it == m_mapXMLNodeToParseContext.end())
    return nullptr;

  CXFA_TextParseContext* pContext = it->second.get();
  if (!pContext)
    return nullptr;

  pContext->m_pParentStyle.Reset(pParentStyle);

  auto tagProvider = ParseTagInfo(pXMLNode);
  if (tagProvider->m_bContent)
    return nullptr;

  auto pStyle = CreateStyle(pParentStyle);
  m_pSelector->ComputeStyle(pContext->GetDecls(),
                            tagProvider->GetAttribute(L"style"),
                            tagProvider->GetAttribute(L"align"), pStyle.Get());
  return pStyle;
}

void CXFA_TextParser::DoParse(const CFX_XMLNode* pXMLContainer,
                              CXFA_TextProvider* pTextProvider) {
  if (!pXMLContainer || !pTextProvider || m_bParsed)
    return;

  m_bParsed = true;
  InitCSSData(pTextProvider);
  auto pRootStyle = CreateRootStyle(pTextProvider);
  ParseRichText(pXMLContainer, pRootStyle.Get());
}

void CXFA_TextParser::ParseRichText(const CFX_XMLNode* pXMLNode,
                                    CFX_CSSComputedStyle* pParentStyle) {
  if (!pXMLNode)
    return;

  auto tagProvider = ParseTagInfo(pXMLNode);
  if (!tagProvider->m_bTagAvailable)
    return;

  RetainPtr<CFX_CSSComputedStyle> pNewStyle;
  if (!(tagProvider->GetTagName().EqualsASCII("body") &&
        tagProvider->GetTagName().EqualsASCII("html"))) {
    auto pTextContext = pdfium::MakeUnique<CXFA_TextParseContext>();
    CFX_CSSDisplay eDisplay = CFX_CSSDisplay::Inline;
    if (!tagProvider->m_bContent) {
      auto declArray =
          m_pSelector->MatchDeclarations(tagProvider->GetTagName());
      pNewStyle = CreateStyle(pParentStyle);
      m_pSelector->ComputeStyle(declArray, tagProvider->GetAttribute(L"style"),
                                tagProvider->GetAttribute(L"align"),
                                pNewStyle.Get());

      if (!declArray.empty())
        pTextContext->SetDecls(std::move(declArray));

      eDisplay = pNewStyle->GetDisplay();
    }
    pTextContext->SetDisplay(eDisplay);
    m_mapXMLNodeToParseContext[pXMLNode] = std::move(pTextContext);
  }

  for (CFX_XMLNode* pXMLChild = pXMLNode->GetFirstChild(); pXMLChild;
       pXMLChild = pXMLChild->GetNextSibling()) {
    ParseRichText(pXMLChild, pNewStyle.Get());
  }
}

bool CXFA_TextParser::TagValidate(const WideString& wsName) const {
  static const uint32_t s_XFATagName[] = {
      0x61,        // a
      0x62,        // b
      0x69,        // i
      0x70,        // p
      0x0001f714,  // br
      0x00022a55,  // li
      0x000239bb,  // ol
      0x00025881,  // ul
      0x0bd37faa,  // sub
      0x0bd37fb8,  // sup
      0xa73e3af2,  // span
      0xb182eaae,  // body
      0xdb8ac455,  // html
  };
  static const int32_t s_iCount = FX_ArraySize(s_XFATagName);

  return std::binary_search(s_XFATagName, s_XFATagName + s_iCount,
                            FX_HashCode_GetW(wsName.AsStringView(), true));
}

// static
std::unique_ptr<CXFA_TextParser::TagProvider> CXFA_TextParser::ParseTagInfo(
    const CFX_XMLNode* pXMLNode) {
  auto tagProvider = pdfium::MakeUnique<TagProvider>();
  const CFX_XMLElement* pXMLElement = ToXMLElement(pXMLNode);
  if (pXMLElement) {
    WideString wsName = pXMLElement->GetLocalTagName();
    tagProvider->SetTagName(wsName);
    tagProvider->m_bTagAvailable = TagValidate(wsName);
    WideString wsValue = pXMLElement->GetAttribute(L"style");
    if (!wsValue.IsEmpty())
      tagProvider->SetAttribute(L"style", wsValue);

    return tagProvider;
  }
  if (pXMLNode->GetType() == FX_XMLNODE_Text) {
    tagProvider->m_bTagAvailable = true;
    tagProvider->m_bContent = true;
  }
  return tagProvider;
}

XFA_AttributeValue CXFA_TextParser::GetVAlign(
    CXFA_TextProvider* pTextProvider) const {
  CXFA_Para* para = pTextProvider->GetParaIfExists();
  return para ? para->GetVerticalAlign() : XFA_AttributeValue::Top;
}

float CXFA_TextParser::GetTabInterval(CFX_CSSComputedStyle* pStyle) const {
  WideString wsValue;
  if (pStyle && pStyle->GetCustomStyle(L"tab-interval", &wsValue))
    return CXFA_Measurement(wsValue.AsStringView()).ToUnit(XFA_Unit::Pt);
  return 36;
}

int32_t CXFA_TextParser::CountTabs(CFX_CSSComputedStyle* pStyle) const {
  WideString wsValue;
  if (pStyle && pStyle->GetCustomStyle(L"xfa-tab-count", &wsValue))
    return wsValue.GetInteger();
  return 0;
}

bool CXFA_TextParser::IsSpaceRun(CFX_CSSComputedStyle* pStyle) const {
  WideString wsValue;
  return pStyle && pStyle->GetCustomStyle(L"xfa-spacerun", &wsValue) &&
         wsValue.EqualsASCIINoCase("yes");
}

RetainPtr<CFGAS_GEFont> CXFA_TextParser::GetFont(
    CXFA_FFDoc* doc,
    CXFA_TextProvider* pTextProvider,
    CFX_CSSComputedStyle* pStyle) const {
  WideString wsFamily = L"Courier";
  uint32_t dwStyle = 0;
  CXFA_Font* font = pTextProvider->GetFontIfExists();
  if (font) {
    wsFamily = font->GetTypeface();
    if (font->IsBold())
      dwStyle |= FXFONT_BOLD;
    if (font->IsItalic())
      dwStyle |= FXFONT_BOLD;
  }

  if (pStyle) {
    int32_t iCount = pStyle->CountFontFamilies();
    if (iCount > 0)
      wsFamily = pStyle->GetFontFamily(iCount - 1).AsStringView();

    dwStyle = 0;
    if (pStyle->GetFontWeight() > FXFONT_FW_NORMAL)
      dwStyle |= FXFONT_BOLD;
    if (pStyle->GetFontStyle() == CFX_CSSFontStyle::Italic)
      dwStyle |= FXFONT_ITALIC;
  }

  CXFA_FontMgr* pFontMgr = doc->GetApp()->GetXFAFontMgr();
  return pFontMgr->GetFont(doc, wsFamily.AsStringView(), dwStyle);
}

float CXFA_TextParser::GetFontSize(CXFA_TextProvider* pTextProvider,
                                   CFX_CSSComputedStyle* pStyle) const {
  if (pStyle)
    return pStyle->GetFontSize();

  CXFA_Font* font = pTextProvider->GetFontIfExists();
  return font ? font->GetFontSize() : 10;
}

int32_t CXFA_TextParser::GetHorScale(CXFA_TextProvider* pTextProvider,
                                     CFX_CSSComputedStyle* pStyle,
                                     const CFX_XMLNode* pXMLNode) const {
  if (pStyle) {
    WideString wsValue;
    if (pStyle->GetCustomStyle(L"xfa-font-horizontal-scale", &wsValue))
      return wsValue.GetInteger();

    while (pXMLNode) {
      auto it = m_mapXMLNodeToParseContext.find(pXMLNode);
      if (it != m_mapXMLNodeToParseContext.end()) {
        CXFA_TextParseContext* pContext = it->second.get();
        if (pContext && pContext->m_pParentStyle &&
            pContext->m_pParentStyle->GetCustomStyle(
                L"xfa-font-horizontal-scale", &wsValue)) {
          return wsValue.GetInteger();
        }
      }
      pXMLNode = pXMLNode->GetParent();
    }
  }

  CXFA_Font* font = pTextProvider->GetFontIfExists();
  return font ? static_cast<int32_t>(font->GetHorizontalScale()) : 100;
}

int32_t CXFA_TextParser::GetVerScale(CXFA_TextProvider* pTextProvider,
                                     CFX_CSSComputedStyle* pStyle) const {
  if (pStyle) {
    WideString wsValue;
    if (pStyle->GetCustomStyle(L"xfa-font-vertical-scale", &wsValue))
      return wsValue.GetInteger();
  }

  CXFA_Font* font = pTextProvider->GetFontIfExists();
  return font ? static_cast<int32_t>(font->GetVerticalScale()) : 100;
}

void CXFA_TextParser::GetUnderline(CXFA_TextProvider* pTextProvider,
                                   CFX_CSSComputedStyle* pStyle,
                                   int32_t& iUnderline,
                                   XFA_AttributeValue& iPeriod) const {
  iUnderline = 0;
  iPeriod = XFA_AttributeValue::All;
  CXFA_Font* font = pTextProvider->GetFontIfExists();
  if (!pStyle) {
    if (font) {
      iUnderline = font->GetUnderline();
      iPeriod = font->GetUnderlinePeriod();
    }
    return;
  }

  uint32_t dwDecoration = pStyle->GetTextDecoration();
  if (dwDecoration & CFX_CSSTEXTDECORATION_Double)
    iUnderline = 2;
  else if (dwDecoration & CFX_CSSTEXTDECORATION_Underline)
    iUnderline = 1;

  WideString wsValue;
  if (pStyle->GetCustomStyle(L"underlinePeriod", &wsValue)) {
    if (wsValue.EqualsASCII("word"))
      iPeriod = XFA_AttributeValue::Word;
  } else if (font) {
    iPeriod = font->GetUnderlinePeriod();
  }
}

void CXFA_TextParser::GetLinethrough(CXFA_TextProvider* pTextProvider,
                                     CFX_CSSComputedStyle* pStyle,
                                     int32_t& iLinethrough) const {
  iLinethrough = 0;
  if (pStyle) {
    uint32_t dwDecoration = pStyle->GetTextDecoration();
    if (dwDecoration & CFX_CSSTEXTDECORATION_LineThrough)
      iLinethrough = 1;
    return;
  }

  CXFA_Font* font = pTextProvider->GetFontIfExists();
  if (font)
    iLinethrough = font->GetLineThrough();
}

FX_ARGB CXFA_TextParser::GetColor(CXFA_TextProvider* pTextProvider,
                                  CFX_CSSComputedStyle* pStyle) const {
  if (pStyle)
    return pStyle->GetColor();

  CXFA_Font* font = pTextProvider->GetFontIfExists();
  return font ? font->GetColor() : 0xFF000000;
}

float CXFA_TextParser::GetBaseline(CXFA_TextProvider* pTextProvider,
                                   CFX_CSSComputedStyle* pStyle) const {
  if (pStyle) {
    if (pStyle->GetVerticalAlign() == CFX_CSSVerticalAlign::Number)
      return pStyle->GetNumberVerticalAlign();
  } else {
    CXFA_Font* font = pTextProvider->GetFontIfExists();
    if (font)
      return font->GetBaselineShift();
  }
  return 0;
}

float CXFA_TextParser::GetLineHeight(CXFA_TextProvider* pTextProvider,
                                     CFX_CSSComputedStyle* pStyle,
                                     bool bFirst,
                                     float fVerScale) const {
  float fLineHeight = 0;
  if (pStyle) {
    fLineHeight = pStyle->GetLineHeight();
  } else {
    CXFA_Para* para = pTextProvider->GetParaIfExists();
    if (para)
      fLineHeight = para->GetLineHeight();
  }

  if (bFirst) {
    float fFontSize = GetFontSize(pTextProvider, pStyle);
    if (fLineHeight < 0.1f)
      fLineHeight = fFontSize;
    else
      fLineHeight = std::min(fLineHeight, fFontSize);
  } else if (fLineHeight < 0.1f) {
    fLineHeight = GetFontSize(pTextProvider, pStyle) * 1.2f;
  }
  fLineHeight *= fVerScale;
  return fLineHeight;
}

Optional<WideString> CXFA_TextParser::GetEmbeddedObj(
    const CXFA_TextProvider* pTextProvider,
    const CFX_XMLNode* pXMLNode) {
  if (!pXMLNode)
    return {};

  const CFX_XMLElement* pElement = ToXMLElement(pXMLNode);
  if (!pElement)
    return {};

  WideString wsAttr = pElement->GetAttribute(L"xfa:embed");
  if (wsAttr.IsEmpty())
    return {};

  if (wsAttr[0] == L'#')
    wsAttr.Delete(0);

  WideString ws =
      GetLowerCaseElementAttributeOrDefault(pElement, L"xfa:embedType", L"som");
  if (!ws.EqualsASCII("uri"))
    return {};

  ws = GetLowerCaseElementAttributeOrDefault(pElement, L"xfa:embedMode",
                                             L"formatted");
  if (!(ws.EqualsASCII("raw") || ws.EqualsASCII("formatted")))
    return {};

  return pTextProvider->GetEmbeddedObj(wsAttr);
}

CXFA_TextParseContext* CXFA_TextParser::GetParseContextFromMap(
    const CFX_XMLNode* pXMLNode) {
  auto it = m_mapXMLNodeToParseContext.find(pXMLNode);
  return it != m_mapXMLNodeToParseContext.end() ? it->second.get() : nullptr;
}

bool CXFA_TextParser::GetTabstops(CFX_CSSComputedStyle* pStyle,
                                  CXFA_TextTabstopsContext* pTabstopContext) {
  if (!pStyle || !pTabstopContext)
    return false;

  WideString wsValue;
  if (!pStyle->GetCustomStyle(L"xfa-tab-stops", &wsValue) &&
      !pStyle->GetCustomStyle(L"tab-stops", &wsValue)) {
    return false;
  }

  int32_t iLength = wsValue.GetLength();
  const wchar_t* pTabStops = wsValue.c_str();
  int32_t iCur = 0;
  int32_t iLast = 0;
  WideString wsAlign;
  TabStopStatus eStatus = TabStopStatus::None;
  wchar_t ch;
  while (iCur < iLength) {
    ch = pTabStops[iCur];
    switch (eStatus) {
      case TabStopStatus::None:
        if (ch <= ' ') {
          iCur++;
        } else {
          eStatus = TabStopStatus::Alignment;
          iLast = iCur;
        }
        break;
      case TabStopStatus::Alignment:
        if (ch == ' ') {
          wsAlign = WideStringView(pTabStops + iLast, iCur - iLast);
          eStatus = TabStopStatus::StartLeader;
          iCur++;
          while (iCur < iLength && pTabStops[iCur] <= ' ')
            iCur++;
          iLast = iCur;
        } else {
          iCur++;
        }
        break;
      case TabStopStatus::StartLeader:
        if (ch != 'l') {
          eStatus = TabStopStatus::Location;
        } else {
          int32_t iCount = 0;
          while (iCur < iLength) {
            ch = pTabStops[iCur];
            iCur++;
            if (ch == '(') {
              iCount++;
            } else if (ch == ')') {
              iCount--;
              if (iCount == 0)
                break;
            }
          }
          while (iCur < iLength && pTabStops[iCur] <= ' ')
            iCur++;

          iLast = iCur;
          eStatus = TabStopStatus::Location;
        }
        break;
      case TabStopStatus::Location:
        if (ch == ' ') {
          uint32_t dwHashCode = FX_HashCode_GetW(wsAlign.AsStringView(), true);
          CXFA_Measurement ms(WideStringView(pTabStops + iLast, iCur - iLast));
          float fPos = ms.ToUnit(XFA_Unit::Pt);
          pTabstopContext->Append(dwHashCode, fPos);
          wsAlign.clear();
          eStatus = TabStopStatus::None;
        }
        iCur++;
        break;
      default:
        break;
    }
  }

  if (!wsAlign.IsEmpty()) {
    uint32_t dwHashCode = FX_HashCode_GetW(wsAlign.AsStringView(), true);
    CXFA_Measurement ms(WideStringView(pTabStops + iLast, iCur - iLast));
    float fPos = ms.ToUnit(XFA_Unit::Pt);
    pTabstopContext->Append(dwHashCode, fPos);
  }
  return true;
}

CXFA_TextParser::TagProvider::TagProvider()
    : m_bTagAvailable(false), m_bContent(false) {}

CXFA_TextParser::TagProvider::~TagProvider() {}
