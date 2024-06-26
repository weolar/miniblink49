// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_TEXTPARSER_H_
#define XFA_FXFA_CXFA_TEXTPARSER_H_

#include <map>
#include <memory>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/fx_dib.h"
#include "third_party/base/optional.h"
#include "xfa/fxfa/fxfa_basic.h"

class CFGAS_GEFont;
class CFX_CSSComputedStyle;
class CFX_CSSStyleSelector;
class CFX_CSSStyleSheet;
class CFX_XMLNode;
class CXFA_FFDoc;
class CXFA_TextParseContext;
class CXFA_TextProvider;
class CXFA_TextTabstopsContext;

class CXFA_TextParser {
 public:
  CXFA_TextParser();
  virtual ~CXFA_TextParser();

  void Reset();
  void DoParse(const CFX_XMLNode* pXMLContainer,
               CXFA_TextProvider* pTextProvider);

  RetainPtr<CFX_CSSComputedStyle> CreateRootStyle(
      CXFA_TextProvider* pTextProvider);
  RetainPtr<CFX_CSSComputedStyle> ComputeStyle(
      const CFX_XMLNode* pXMLNode,
      CFX_CSSComputedStyle* pParentStyle);

  bool IsParsed() const { return m_bParsed; }

  XFA_AttributeValue GetVAlign(CXFA_TextProvider* pTextProvider) const;

  float GetTabInterval(CFX_CSSComputedStyle* pStyle) const;
  int32_t CountTabs(CFX_CSSComputedStyle* pStyle) const;

  bool IsSpaceRun(CFX_CSSComputedStyle* pStyle) const;
  bool GetTabstops(CFX_CSSComputedStyle* pStyle,
                   CXFA_TextTabstopsContext* pTabstopContext);

  RetainPtr<CFGAS_GEFont> GetFont(CXFA_FFDoc* doc,
                                  CXFA_TextProvider* pTextProvider,
                                  CFX_CSSComputedStyle* pStyle) const;
  float GetFontSize(CXFA_TextProvider* pTextProvider,
                    CFX_CSSComputedStyle* pStyle) const;

  int32_t GetHorScale(CXFA_TextProvider* pTextProvider,
                      CFX_CSSComputedStyle* pStyle,
                      const CFX_XMLNode* pXMLNode) const;
  int32_t GetVerScale(CXFA_TextProvider* pTextProvider,
                      CFX_CSSComputedStyle* pStyle) const;

  void GetUnderline(CXFA_TextProvider* pTextProvider,
                    CFX_CSSComputedStyle* pStyle,
                    int32_t& iUnderline,
                    XFA_AttributeValue& iPeriod) const;
  void GetLinethrough(CXFA_TextProvider* pTextProvider,
                      CFX_CSSComputedStyle* pStyle,
                      int32_t& iLinethrough) const;
  FX_ARGB GetColor(CXFA_TextProvider* pTextProvider,
                   CFX_CSSComputedStyle* pStyle) const;
  float GetBaseline(CXFA_TextProvider* pTextProvider,
                    CFX_CSSComputedStyle* pStyle) const;
  float GetLineHeight(CXFA_TextProvider* pTextProvider,
                      CFX_CSSComputedStyle* pStyle,
                      bool bFirst,
                      float fVerScale) const;

  Optional<WideString> GetEmbeddedObj(const CXFA_TextProvider* pTextProvider,
                                      const CFX_XMLNode* pXMLNode);
  CXFA_TextParseContext* GetParseContextFromMap(const CFX_XMLNode* pXMLNode);

 protected:
  bool TagValidate(const WideString& str) const;

 private:
  class TagProvider {
   public:
    TagProvider();
    ~TagProvider();

    WideString GetTagName() { return m_wsTagName; }

    void SetTagName(const WideString& wsName) { m_wsTagName = wsName; }
    void SetAttribute(const WideString& wsAttr, const WideString& wsValue) {
      m_Attributes.insert({wsAttr, wsValue});
    }

    WideString GetAttribute(const WideString& wsAttr) {
      return m_Attributes[wsAttr];
    }

    bool m_bTagAvailable;
    bool m_bContent;

   private:
    WideString m_wsTagName;
    std::map<WideString, WideString> m_Attributes;
  };

  // static
  std::unique_ptr<TagProvider> ParseTagInfo(const CFX_XMLNode* pXMLNode);

  void InitCSSData(CXFA_TextProvider* pTextProvider);
  void ParseRichText(const CFX_XMLNode* pXMLNode,
                     CFX_CSSComputedStyle* pParentStyle);
  std::unique_ptr<CFX_CSSStyleSheet> LoadDefaultSheetStyle();
  RetainPtr<CFX_CSSComputedStyle> CreateStyle(
      CFX_CSSComputedStyle* pParentStyle);

  bool m_bParsed;
  bool m_cssInitialized;
  std::unique_ptr<CFX_CSSStyleSelector> m_pSelector;
  std::map<const CFX_XMLNode*, std::unique_ptr<CXFA_TextParseContext>>
      m_mapXMLNodeToParseContext;
};

#endif  // XFA_FXFA_CXFA_TEXTPARSER_H_
