// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_interactiveform.h"

#include <utility>
#include <vector>

#include "constants/stream_dict_common.h"
#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/font/cpdf_fontencoding.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cfdf_document.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/parser/fpdf_parser_utility.h"
#include "core/fpdfdoc/cpdf_filespec.h"
#include "core/fpdfdoc/cpdf_formcontrol.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxge/cfx_substfont.h"
#include "core/fxge/fx_font.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

namespace {

const int nMaxRecursion = 32;

ByteString GenerateNewFontResourceName(const CPDF_Dictionary* pResDict,
                                       const ByteString& csPrefix) {
  static const char kDummyFontName[] = "ZiTi";
  ByteString csStr = csPrefix;
  if (csStr.IsEmpty())
    csStr = kDummyFontName;

  const size_t szCount = csStr.GetLength();
  size_t m = 0;
  ByteString csTmp;
  while (m < strlen(kDummyFontName) && m < szCount)
    csTmp += csStr[m++];
  while (m < strlen(kDummyFontName)) {
    csTmp += '0' + m % 10;
    m++;
  }

  const CPDF_Dictionary* pDict = pResDict->GetDictFor("Font");
  ASSERT(pDict);

  int num = 0;
  ByteString bsNum;
  while (true) {
    ByteString csKey = csTmp + bsNum;
    if (!pDict->KeyExist(csKey))
      return csKey;
    if (m < szCount)
      csTmp += csStr[m++];
    else
      bsNum = ByteString::Format("%d", num++);

    m++;
  }
  return csTmp;
}

void AddFont(CPDF_Dictionary*& pFormDict,
             CPDF_Document* pDocument,
             const CPDF_Font* pFont,
             ByteString* csNameTag);

void InitDict(CPDF_Dictionary*& pFormDict, CPDF_Document* pDocument) {
  if (!pDocument)
    return;

  if (!pFormDict) {
    pFormDict = pDocument->NewIndirect<CPDF_Dictionary>();
    pDocument->GetRoot()->SetFor("AcroForm",
                                 pFormDict->MakeReference(pDocument));
  }

  ByteString csDA;
  if (!pFormDict->KeyExist("DR")) {
    ByteString csBaseName;
    uint8_t charSet = CPDF_InteractiveForm::GetNativeCharSet();
    CPDF_Font* pFont = CPDF_InteractiveForm::AddStandardFont(
        pDocument, CFX_Font::kDefaultAnsiFontName);
    if (pFont)
      AddFont(pFormDict, pDocument, pFont, &csBaseName);

    if (charSet != FX_CHARSET_ANSI) {
      ByteString csFontName =
          CPDF_InteractiveForm::GetNativeFont(charSet, nullptr);
      if (!pFont || csFontName != CFX_Font::kDefaultAnsiFontName) {
        pFont = CPDF_InteractiveForm::AddNativeFont(pDocument);
        if (pFont) {
          csBaseName.clear();
          AddFont(pFormDict, pDocument, pFont, &csBaseName);
        }
      }
    }
    if (pFont)
      csDA = "/" + PDF_NameEncode(csBaseName) + " 0 Tf";
  }
  if (!csDA.IsEmpty())
    csDA += " ";

  csDA += "0 g";
  if (!pFormDict->KeyExist("DA"))
    pFormDict->SetNewFor<CPDF_String>("DA", csDA, false);
}

CPDF_Font* GetFont(CPDF_Dictionary* pFormDict,
                   CPDF_Document* pDocument,
                   const ByteString& csNameTag) {
  ByteString csAlias = PDF_NameDecode(csNameTag.AsStringView());
  if (!pFormDict || csAlias.IsEmpty())
    return nullptr;

  CPDF_Dictionary* pDR = pFormDict->GetDictFor("DR");
  if (!pDR)
    return nullptr;

  CPDF_Dictionary* pFonts = pDR->GetDictFor("Font");
  if (!pFonts)
    return nullptr;

  CPDF_Dictionary* pElement = pFonts->GetDictFor(csAlias);
  if (!pElement)
    return nullptr;

  if (pElement->GetStringFor("Type") == "Font")
    return pDocument->LoadFont(pElement);
  return nullptr;
}

CPDF_Font* GetNativeFont(CPDF_Dictionary* pFormDict,
                         CPDF_Document* pDocument,
                         uint8_t charSet,
                         ByteString* csNameTag) {
  if (!pFormDict)
    return nullptr;

  CPDF_Dictionary* pDR = pFormDict->GetDictFor("DR");
  if (!pDR)
    return nullptr;

  CPDF_Dictionary* pFonts = pDR->GetDictFor("Font");
  if (!pFonts)
    return nullptr;

  CPDF_DictionaryLocker locker(pFonts);
  for (const auto& it : locker) {
    const ByteString& csKey = it.first;
    if (!it.second)
      continue;

    CPDF_Dictionary* pElement = ToDictionary(it.second->GetDirect());
    if (!pElement)
      continue;
    if (pElement->GetStringFor("Type") != "Font")
      continue;
    CPDF_Font* pFind = pDocument->LoadFont(pElement);
    if (!pFind)
      continue;

    CFX_SubstFont* pSubst = pFind->GetSubstFont();
    if (!pSubst)
      continue;

    if (pSubst->m_Charset == static_cast<int>(charSet)) {
      *csNameTag = csKey;
      return pFind;
    }
  }
  return nullptr;
}

bool FindFont(CPDF_Dictionary* pFormDict,
              const CPDF_Font* pFont,
              ByteString* csNameTag) {
  if (!pFormDict || !pFont)
    return false;

  CPDF_Dictionary* pDR = pFormDict->GetDictFor("DR");
  if (!pDR)
    return false;

  CPDF_Dictionary* pFonts = pDR->GetDictFor("Font");
  if (!pFonts)
    return false;

  CPDF_DictionaryLocker locker(pFonts);
  for (const auto& it : locker) {
    const ByteString& csKey = it.first;
    if (!it.second)
      continue;
    CPDF_Dictionary* pElement = ToDictionary(it.second->GetDirect());
    if (!pElement)
      continue;
    if (pElement->GetStringFor("Type") != "Font")
      continue;
    if (pFont->GetFontDict() == pElement) {
      *csNameTag = csKey;
      return true;
    }
  }
  return false;
}

bool FindFont(CPDF_Dictionary* pFormDict,
              CPDF_Document* pDocument,
              ByteString csFontName,
              CPDF_Font*& pFont,
              ByteString* csNameTag) {
  if (!pFormDict)
    return false;

  CPDF_Dictionary* pDR = pFormDict->GetDictFor("DR");
  if (!pDR)
    return false;

  CPDF_Dictionary* pFonts = pDR->GetDictFor("Font");
  if (!pFonts)
    return false;

  if (csFontName.GetLength() > 0)
    csFontName.Remove(' ');

  CPDF_DictionaryLocker locker(pFonts);
  for (const auto& it : locker) {
    const ByteString& csKey = it.first;
    if (!it.second)
      continue;

    CPDF_Dictionary* pElement = ToDictionary(it.second->GetDirect());
    if (!pElement)
      continue;
    if (pElement->GetStringFor("Type") != "Font")
      continue;
    pFont = pDocument->LoadFont(pElement);
    if (!pFont)
      continue;

    ByteString csBaseFont = pFont->GetBaseFont();
    csBaseFont.Remove(' ');
    if (csBaseFont == csFontName) {
      *csNameTag = csKey;
      return true;
    }
  }
  return false;
}

void AddFont(CPDF_Dictionary*& pFormDict,
             CPDF_Document* pDocument,
             const CPDF_Font* pFont,
             ByteString* csNameTag) {
  if (!pFont)
    return;
  if (!pFormDict)
    InitDict(pFormDict, pDocument);

  ByteString csTag;
  if (FindFont(pFormDict, pFont, &csTag)) {
    *csNameTag = std::move(csTag);
    return;
  }
  if (!pFormDict)
    InitDict(pFormDict, pDocument);

  CPDF_Dictionary* pDR = pFormDict->GetDictFor("DR");
  if (!pDR)
    pDR = pFormDict->SetNewFor<CPDF_Dictionary>("DR");

  CPDF_Dictionary* pFonts = pDR->GetDictFor("Font");
  if (!pFonts)
    pFonts = pDR->SetNewFor<CPDF_Dictionary>("Font");

  if (csNameTag->IsEmpty())
    *csNameTag = pFont->GetBaseFont();

  csNameTag->Remove(' ');
  *csNameTag = GenerateNewFontResourceName(pDR, *csNameTag);
  pFonts->SetFor(*csNameTag, pFont->GetFontDict()->MakeReference(pDocument));
}

CPDF_Font* AddNativeFont(CPDF_Dictionary*& pFormDict,
                         CPDF_Document* pDocument,
                         uint8_t charSet,
                         ByteString* csNameTag) {
  if (!pFormDict)
    InitDict(pFormDict, pDocument);

  ByteString csTemp;
  CPDF_Font* pFont = GetNativeFont(pFormDict, pDocument, charSet, &csTemp);
  if (pFont) {
    *csNameTag = std::move(csTemp);
    return pFont;
  }
  ByteString csFontName = CPDF_InteractiveForm::GetNativeFont(charSet, nullptr);
  if (!csFontName.IsEmpty() &&
      FindFont(pFormDict, pDocument, csFontName, pFont, csNameTag)) {
    return pFont;
  }
  pFont = CPDF_InteractiveForm::AddNativeFont(charSet, pDocument);
  if (pFont)
    AddFont(pFormDict, pDocument, pFont, csNameTag);

  return pFont;
}

class CFieldNameExtractor {
 public:
  explicit CFieldNameExtractor(const WideString& full_name)
      : m_FullName(full_name) {
    m_pCur = m_FullName.c_str();
    m_pEnd = m_pCur + m_FullName.GetLength();
  }

  void GetNext(const wchar_t*& pSubName, size_t& size) {
    pSubName = m_pCur;
    while (m_pCur < m_pEnd && m_pCur[0] != L'.')
      m_pCur++;

    size = static_cast<size_t>(m_pCur - pSubName);
    if (m_pCur < m_pEnd && m_pCur[0] == L'.')
      m_pCur++;
  }

 protected:
  WideString m_FullName;
  const wchar_t* m_pCur;
  const wchar_t* m_pEnd;
};

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
struct PDF_FONTDATA {
  bool bFind;
  LOGFONTA lf;
};

static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEXA* lpelfe,
                                      NEWTEXTMETRICEX* lpntme,
                                      DWORD FontType,
                                      LPARAM lParam) {
  if (FontType != 0x004 || strchr(lpelfe->elfLogFont.lfFaceName, '@'))
    return 1;

  PDF_FONTDATA* pData = (PDF_FONTDATA*)lParam;
  memcpy(&pData->lf, &lpelfe->elfLogFont, sizeof(LOGFONTA));
  pData->bFind = true;
  return 0;
}

bool RetrieveSpecificFont(LOGFONTA& lf) {
  PDF_FONTDATA fd;
  memset(&fd, 0, sizeof(PDF_FONTDATA));
  HDC hDC = ::GetDC(nullptr);
  EnumFontFamiliesExA(hDC, &lf, (FONTENUMPROCA)EnumFontFamExProc, (LPARAM)&fd,
                      0);
  ::ReleaseDC(nullptr, hDC);
  if (fd.bFind)
    memcpy(&lf, &fd.lf, sizeof(LOGFONTA));

  return fd.bFind;
}

bool RetrieveSpecificFont(uint8_t charSet,
                          uint8_t pitchAndFamily,
                          LPCSTR pcsFontName,
                          LOGFONTA& lf) {
  memset(&lf, 0, sizeof(LOGFONTA));
  lf.lfCharSet = charSet;
  lf.lfPitchAndFamily = pitchAndFamily;
  if (pcsFontName) {
    // TODO(dsinclair): Should this be strncpy?
    // NOLINTNEXTLINE(runtime/printf)
    strcpy(lf.lfFaceName, pcsFontName);
  }
  return RetrieveSpecificFont(lf);
}
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

}  // namespace

class CFieldTree {
 public:
  class Node {
   public:
    Node() : m_level(0) {}
    Node(const WideString& short_name, int level)
        : m_ShortName(short_name), m_level(level) {}
    ~Node() = default;

    void AddChildNode(std::unique_ptr<Node> pNode) {
      m_Children.push_back(std::move(pNode));
    }

    size_t GetChildrenCount() const { return m_Children.size(); }

    Node* GetChildAt(size_t i) { return m_Children[i].get(); }
    const Node* GetChildAt(size_t i) const { return m_Children[i].get(); }

    CPDF_FormField* GetFieldAtIndex(size_t index) {
      size_t nFieldsToGo = index;
      return GetFieldInternal(&nFieldsToGo);
    }

    size_t CountFields() const { return CountFieldsInternal(); }

    void SetField(std::unique_ptr<CPDF_FormField> pField) {
      m_pField = std::move(pField);
    }

    CPDF_FormField* GetField() const { return m_pField.get(); }
    WideString GetShortName() const { return m_ShortName; }
    int GetLevel() const { return m_level; }

   private:
    CPDF_FormField* GetFieldInternal(size_t* pFieldsToGo) {
      if (m_pField) {
        if (*pFieldsToGo == 0)
          return m_pField.get();

        --*pFieldsToGo;
      }
      for (size_t i = 0; i < GetChildrenCount(); ++i) {
        CPDF_FormField* pField = GetChildAt(i)->GetFieldInternal(pFieldsToGo);
        if (pField)
          return pField;
      }
      return nullptr;
    }

    size_t CountFieldsInternal() const {
      size_t count = 0;
      if (m_pField)
        ++count;

      for (size_t i = 0; i < GetChildrenCount(); ++i)
        count += GetChildAt(i)->CountFieldsInternal();
      return count;
    }

    std::vector<std::unique_ptr<Node>> m_Children;
    WideString m_ShortName;
    std::unique_ptr<CPDF_FormField> m_pField;
    const int m_level;
  };

  CFieldTree();
  ~CFieldTree();

  bool SetField(const WideString& full_name,
                std::unique_ptr<CPDF_FormField> pField);
  CPDF_FormField* GetField(const WideString& full_name);

  Node* FindNode(const WideString& full_name);
  Node* AddChild(Node* pParent, const WideString& short_name);

  Node* Lookup(Node* pParent, const WideString& short_name);

  Node m_Root;
};

CFieldTree::CFieldTree() = default;

CFieldTree::~CFieldTree() = default;

CFieldTree::Node* CFieldTree::AddChild(Node* pParent,
                                       const WideString& short_name) {
  if (!pParent)
    return nullptr;

  int level = pParent->GetLevel() + 1;
  if (level > nMaxRecursion)
    return nullptr;

  auto pNew = pdfium::MakeUnique<Node>(short_name, pParent->GetLevel() + 1);
  Node* pChild = pNew.get();
  pParent->AddChildNode(std::move(pNew));
  return pChild;
}

CFieldTree::Node* CFieldTree::Lookup(Node* pParent,
                                     const WideString& short_name) {
  if (!pParent)
    return nullptr;

  for (size_t i = 0; i < pParent->GetChildrenCount(); ++i) {
    Node* pNode = pParent->GetChildAt(i);
    if (pNode->GetShortName() == short_name)
      return pNode;
  }
  return nullptr;
}

bool CFieldTree::SetField(const WideString& full_name,
                          std::unique_ptr<CPDF_FormField> pField) {
  if (full_name.IsEmpty())
    return false;

  CFieldNameExtractor name_extractor(full_name);
  const wchar_t* pName;
  size_t nLength;
  name_extractor.GetNext(pName, nLength);
  Node* pNode = &m_Root;
  Node* pLast = nullptr;
  while (nLength > 0) {
    pLast = pNode;
    WideString name = WideString(pName, nLength);
    pNode = Lookup(pLast, name);
    if (!pNode)
      pNode = AddChild(pLast, name);
    if (!pNode)
      return false;

    name_extractor.GetNext(pName, nLength);
  }
  if (pNode == &m_Root)
    return false;

  pNode->SetField(std::move(pField));
  return true;
}

CPDF_FormField* CFieldTree::GetField(const WideString& full_name) {
  if (full_name.IsEmpty())
    return nullptr;

  CFieldNameExtractor name_extractor(full_name);
  const wchar_t* pName;
  size_t nLength;
  name_extractor.GetNext(pName, nLength);
  Node* pNode = &m_Root;
  Node* pLast = nullptr;
  while (nLength > 0 && pNode) {
    pLast = pNode;
    WideString name = WideString(pName, nLength);
    pNode = Lookup(pLast, name);
    name_extractor.GetNext(pName, nLength);
  }
  return pNode ? pNode->GetField() : nullptr;
}

CFieldTree::Node* CFieldTree::FindNode(const WideString& full_name) {
  if (full_name.IsEmpty())
    return nullptr;

  CFieldNameExtractor name_extractor(full_name);
  const wchar_t* pName;
  size_t nLength;
  name_extractor.GetNext(pName, nLength);
  Node* pNode = &m_Root;
  Node* pLast = nullptr;
  while (nLength > 0 && pNode) {
    pLast = pNode;
    WideString name = WideString(pName, nLength);
    pNode = Lookup(pLast, name);
    name_extractor.GetNext(pName, nLength);
  }
  return pNode;
}

CPDF_Font* AddNativeInteractiveFormFont(CPDF_Dictionary*& pFormDict,
                                        CPDF_Document* pDocument,
                                        ByteString* csNameTag) {
  uint8_t charSet = CPDF_InteractiveForm::GetNativeCharSet();
  return AddNativeFont(pFormDict, pDocument, charSet, csNameTag);
}

// static
uint8_t CPDF_InteractiveForm::GetNativeCharSet() {
  return FX_GetCharsetFromCodePage(FXSYS_GetACP());
}

CPDF_InteractiveForm::CPDF_InteractiveForm(CPDF_Document* pDocument)
    : m_pDocument(pDocument), m_pFieldTree(pdfium::MakeUnique<CFieldTree>()) {
  CPDF_Dictionary* pRoot = m_pDocument->GetRoot();
  if (!pRoot)
    return;

  m_pFormDict = pRoot->GetDictFor("AcroForm");
  if (!m_pFormDict)
    return;

  CPDF_Array* pFields = m_pFormDict->GetArrayFor("Fields");
  if (!pFields)
    return;

  for (size_t i = 0; i < pFields->size(); ++i)
    LoadField(pFields->GetDictAt(i), 0);
}

CPDF_InteractiveForm::~CPDF_InteractiveForm() = default;

bool CPDF_InteractiveForm::s_bUpdateAP = true;

bool CPDF_InteractiveForm::IsUpdateAPEnabled() {
  return s_bUpdateAP;
}

void CPDF_InteractiveForm::SetUpdateAP(bool bUpdateAP) {
  s_bUpdateAP = bUpdateAP;
}

CPDF_Font* CPDF_InteractiveForm::AddStandardFont(CPDF_Document* pDocument,
                                                 ByteString csFontName) {
  if (!pDocument || csFontName.IsEmpty())
    return nullptr;

  if (csFontName == "ZapfDingbats")
    return pDocument->AddStandardFont(csFontName.c_str(), nullptr);

  CPDF_FontEncoding encoding(PDFFONT_ENCODING_WINANSI);
  return pDocument->AddStandardFont(csFontName.c_str(), &encoding);
}

ByteString CPDF_InteractiveForm::GetNativeFont(uint8_t charSet,
                                               void* pLogFont) {
  ByteString csFontName;
#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  LOGFONTA lf = {};
  if (charSet == FX_CHARSET_ANSI) {
    csFontName = CFX_Font::kDefaultAnsiFontName;
    return csFontName;
  }
  bool bRet = false;
  const ByteString default_font_name =
      CFX_Font::GetDefaultFontNameByCharset(charSet);
  if (!default_font_name.IsEmpty()) {
    bRet = RetrieveSpecificFont(charSet, DEFAULT_PITCH | FF_DONTCARE,
                                default_font_name.c_str(), lf);
  }
  if (!bRet) {
    bRet = RetrieveSpecificFont(charSet, DEFAULT_PITCH | FF_DONTCARE,
                                CFX_Font::kUniversalDefaultFontName, lf);
  }
  if (!bRet) {
    bRet = RetrieveSpecificFont(charSet, DEFAULT_PITCH | FF_DONTCARE,
                                "Microsoft Sans Serif", lf);
  }
  if (!bRet) {
    bRet =
        RetrieveSpecificFont(charSet, DEFAULT_PITCH | FF_DONTCARE, nullptr, lf);
  }
  if (bRet) {
    if (pLogFont)
      memcpy(pLogFont, &lf, sizeof(LOGFONTA));

    csFontName = lf.lfFaceName;
    return csFontName;
  }
#endif
  return csFontName;
}

CPDF_Font* CPDF_InteractiveForm::AddNativeFont(uint8_t charSet,
                                               CPDF_Document* pDocument) {
  if (!pDocument)
    return nullptr;

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  LOGFONTA lf;
  ByteString csFontName = GetNativeFont(charSet, &lf);
  if (!csFontName.IsEmpty()) {
    if (csFontName == CFX_Font::kDefaultAnsiFontName)
      return AddStandardFont(pDocument, csFontName);
    return pDocument->AddWindowsFont(&lf);
  }
#endif
  return nullptr;
}

CPDF_Font* CPDF_InteractiveForm::AddNativeFont(CPDF_Document* pDocument) {
  return pDocument ? AddNativeFont(GetNativeCharSet(), pDocument) : nullptr;
}

size_t CPDF_InteractiveForm::CountFields(const WideString& csFieldName) const {
  if (csFieldName.IsEmpty())
    return m_pFieldTree->m_Root.CountFields();

  CFieldTree::Node* pNode = m_pFieldTree->FindNode(csFieldName);
  return pNode ? pNode->CountFields() : 0;
}

CPDF_FormField* CPDF_InteractiveForm::GetField(
    uint32_t index,
    const WideString& csFieldName) const {
  if (csFieldName.IsEmpty())
    return m_pFieldTree->m_Root.GetFieldAtIndex(index);

  CFieldTree::Node* pNode = m_pFieldTree->FindNode(csFieldName);
  return pNode ? pNode->GetFieldAtIndex(index) : nullptr;
}

CPDF_FormField* CPDF_InteractiveForm::GetFieldByDict(
    CPDF_Dictionary* pFieldDict) const {
  if (!pFieldDict)
    return nullptr;

  WideString csWName = FPDF_GetFullName(pFieldDict);
  return m_pFieldTree->GetField(csWName);
}

CPDF_FormControl* CPDF_InteractiveForm::GetControlAtPoint(
    CPDF_Page* pPage,
    const CFX_PointF& point,

    int* z_order) const {
  CPDF_Array* pAnnotList = pPage->GetDict()->GetArrayFor("Annots");
  if (!pAnnotList)
    return nullptr;

  for (size_t i = pAnnotList->size(); i > 0; --i) {
    size_t annot_index = i - 1;
    const CPDF_Dictionary* pAnnot = pAnnotList->GetDictAt(annot_index);
    if (!pAnnot)
      continue;

    const auto it = m_ControlMap.find(pAnnot);
    if (it == m_ControlMap.end())
      continue;

    CPDF_FormControl* pControl = it->second.get();
    if (!pControl->GetRect().Contains(point))
      continue;

    if (z_order)
      *z_order = static_cast<int>(annot_index);
    return pControl;
  }
  return nullptr;
}

CPDF_FormControl* CPDF_InteractiveForm::GetControlByDict(
    const CPDF_Dictionary* pWidgetDict) const {
  const auto it = m_ControlMap.find(pWidgetDict);
  return it != m_ControlMap.end() ? it->second.get() : nullptr;
}

bool CPDF_InteractiveForm::NeedConstructAP() const {
  return m_pFormDict && m_pFormDict->GetBooleanFor("NeedAppearances", false);
}

int CPDF_InteractiveForm::CountFieldsInCalculationOrder() {
  if (!m_pFormDict)
    return 0;

  CPDF_Array* pArray = m_pFormDict->GetArrayFor("CO");
  return pArray ? pArray->size() : 0;
}

CPDF_FormField* CPDF_InteractiveForm::GetFieldInCalculationOrder(int index) {
  if (!m_pFormDict || index < 0)
    return nullptr;

  CPDF_Array* pArray = m_pFormDict->GetArrayFor("CO");
  if (!pArray)
    return nullptr;

  CPDF_Dictionary* pElement = ToDictionary(pArray->GetDirectObjectAt(index));
  return pElement ? GetFieldByDict(pElement) : nullptr;
}

int CPDF_InteractiveForm::FindFieldInCalculationOrder(
    const CPDF_FormField* pField) {
  if (!m_pFormDict || !pField)
    return -1;

  CPDF_Array* pArray = m_pFormDict->GetArrayFor("CO");
  if (!pArray)
    return -1;

  for (size_t i = 0; i < pArray->size(); i++) {
    CPDF_Object* pElement = pArray->GetDirectObjectAt(i);
    if (pElement == pField->GetDict())
      return i;
  }
  return -1;
}

CPDF_Font* CPDF_InteractiveForm::GetFormFont(ByteString csNameTag) const {
  return GetFont(m_pFormDict.Get(), m_pDocument.Get(), csNameTag);
}

CPDF_DefaultAppearance CPDF_InteractiveForm::GetDefaultAppearance() const {
  if (!m_pFormDict)
    return CPDF_DefaultAppearance();
  return CPDF_DefaultAppearance(m_pFormDict->GetStringFor("DA"));
}

int CPDF_InteractiveForm::GetFormAlignment() const {
  return m_pFormDict ? m_pFormDict->GetIntegerFor("Q", 0) : 0;
}

void CPDF_InteractiveForm::ResetForm(const std::vector<CPDF_FormField*>& fields,
                                     bool bIncludeOrExclude,
                                     NotificationOption notify) {
  size_t nCount = m_pFieldTree->m_Root.CountFields();
  for (size_t i = 0; i < nCount; ++i) {
    CPDF_FormField* pField = m_pFieldTree->m_Root.GetFieldAtIndex(i);
    if (!pField)
      continue;

    if (bIncludeOrExclude == pdfium::ContainsValue(fields, pField))
      pField->ResetField(notify);
  }
  if (notify == NotificationOption::kNotify && m_pFormNotify)
    m_pFormNotify->AfterFormReset(this);
}

void CPDF_InteractiveForm::ResetForm(NotificationOption notify) {
  size_t nCount = m_pFieldTree->m_Root.CountFields();
  for (size_t i = 0; i < nCount; ++i) {
    CPDF_FormField* pField = m_pFieldTree->m_Root.GetFieldAtIndex(i);
    if (!pField)
      continue;

    pField->ResetField(notify);
  }
  if (notify == NotificationOption::kNotify && m_pFormNotify)
    m_pFormNotify->AfterFormReset(this);
}

const std::vector<UnownedPtr<CPDF_FormControl>>&
CPDF_InteractiveForm::GetControlsForField(const CPDF_FormField* pField) {
  return m_ControlLists[pField];
}

void CPDF_InteractiveForm::LoadField(CPDF_Dictionary* pFieldDict, int nLevel) {
  if (nLevel > nMaxRecursion)
    return;
  if (!pFieldDict)
    return;

  uint32_t dwParentObjNum = pFieldDict->GetObjNum();
  CPDF_Array* pKids = pFieldDict->GetArrayFor("Kids");
  if (!pKids) {
    AddTerminalField(pFieldDict);
    return;
  }

  CPDF_Dictionary* pFirstKid = pKids->GetDictAt(0);
  if (!pFirstKid)
    return;

  if (pFirstKid->KeyExist("T") || pFirstKid->KeyExist("Kids")) {
    for (size_t i = 0; i < pKids->size(); i++) {
      CPDF_Dictionary* pChildDict = pKids->GetDictAt(i);
      if (pChildDict) {
        if (pChildDict->GetObjNum() != dwParentObjNum)
          LoadField(pChildDict, nLevel + 1);
      }
    }
  } else {
    AddTerminalField(pFieldDict);
  }
}

bool CPDF_InteractiveForm::HasXFAForm() const {
  return m_pFormDict && m_pFormDict->GetArrayFor("XFA");
}

void CPDF_InteractiveForm::FixPageFields(CPDF_Page* pPage) {
  CPDF_Dictionary* pPageDict = pPage->GetDict();
  if (!pPageDict)
    return;

  CPDF_Array* pAnnots = pPageDict->GetArrayFor("Annots");
  if (!pAnnots)
    return;

  for (size_t i = 0; i < pAnnots->size(); i++) {
    CPDF_Dictionary* pAnnot = pAnnots->GetDictAt(i);
    if (pAnnot && pAnnot->GetStringFor("Subtype") == "Widget")
      LoadField(pAnnot, 0);
  }
}

void CPDF_InteractiveForm::AddTerminalField(CPDF_Dictionary* pFieldDict) {
  if (!pFieldDict->KeyExist("FT")) {
    // Key "FT" is required for terminal fields, it is also inheritable.
    CPDF_Dictionary* pParentDict = pFieldDict->GetDictFor("Parent");
    if (!pParentDict || !pParentDict->KeyExist("FT"))
      return;
  }

  CPDF_Dictionary* pDict = pFieldDict;
  WideString csWName = FPDF_GetFullName(pFieldDict);
  if (csWName.IsEmpty())
    return;

  CPDF_FormField* pField = nullptr;
  pField = m_pFieldTree->GetField(csWName);
  if (!pField) {
    CPDF_Dictionary* pParent = pFieldDict;
    if (!pFieldDict->KeyExist("T") &&
        pFieldDict->GetStringFor("Subtype") == "Widget") {
      pParent = pFieldDict->GetDictFor("Parent");
      if (!pParent)
        pParent = pFieldDict;
    }

    if (pParent && pParent != pFieldDict && !pParent->KeyExist("FT")) {
      if (pFieldDict->KeyExist("FT")) {
        CPDF_Object* pFTValue = pFieldDict->GetDirectObjectFor("FT");
        if (pFTValue)
          pParent->SetFor("FT", pFTValue->Clone());
      }

      if (pFieldDict->KeyExist("Ff")) {
        CPDF_Object* pFfValue = pFieldDict->GetDirectObjectFor("Ff");
        if (pFfValue)
          pParent->SetFor("Ff", pFfValue->Clone());
      }
    }

    auto newField = pdfium::MakeUnique<CPDF_FormField>(this, pParent);
    pField = newField.get();
    CPDF_Object* pTObj = pDict->GetObjectFor("T");
    if (ToReference(pTObj)) {
      std::unique_ptr<CPDF_Object> pClone = pTObj->CloneDirectObject();
      if (pClone)
        pDict->SetFor("T", std::move(pClone));
      else
        pDict->SetNewFor<CPDF_Name>("T", ByteString());
    }
    if (!m_pFieldTree->SetField(csWName, std::move(newField)))
      return;
  }

  CPDF_Array* pKids = pFieldDict->GetArrayFor("Kids");
  if (pKids) {
    for (size_t i = 0; i < pKids->size(); i++) {
      CPDF_Dictionary* pKid = pKids->GetDictAt(i);
      if (!pKid)
        continue;
      if (pKid->GetStringFor("Subtype") != "Widget")
        continue;

      AddControl(pField, pKid);
    }
  } else {
    if (pFieldDict->GetStringFor("Subtype") == "Widget")
      AddControl(pField, pFieldDict);
  }
}

CPDF_FormControl* CPDF_InteractiveForm::AddControl(
    CPDF_FormField* pField,
    CPDF_Dictionary* pWidgetDict) {
  const auto it = m_ControlMap.find(pWidgetDict);
  if (it != m_ControlMap.end())
    return it->second.get();

  auto pNew = pdfium::MakeUnique<CPDF_FormControl>(pField, pWidgetDict);
  CPDF_FormControl* pControl = pNew.get();
  m_ControlMap[pWidgetDict] = std::move(pNew);
  m_ControlLists[pField].emplace_back(pControl);
  return pControl;
}

bool CPDF_InteractiveForm::CheckRequiredFields(
    const std::vector<CPDF_FormField*>* fields,
    bool bIncludeOrExclude) const {
  size_t nCount = m_pFieldTree->m_Root.CountFields();
  for (size_t i = 0; i < nCount; ++i) {
    CPDF_FormField* pField = m_pFieldTree->m_Root.GetFieldAtIndex(i);
    if (!pField)
      continue;

    int32_t iType = pField->GetType();
    if (iType == CPDF_FormField::kPushButton ||
        iType == CPDF_FormField::kCheckBox ||
        iType == CPDF_FormField::kListBox) {
      continue;
    }
    if (pField->IsNoExport())
      continue;

    bool bFind = true;
    if (fields)
      bFind = pdfium::ContainsValue(*fields, pField);
    if (bIncludeOrExclude == bFind) {
      const CPDF_Dictionary* pFieldDict = pField->GetDict();
      if (pField->IsRequired() && pFieldDict->GetStringFor("V").IsEmpty())
        return false;
    }
  }
  return true;
}

std::unique_ptr<CFDF_Document> CPDF_InteractiveForm::ExportToFDF(
    const WideString& pdf_path,
    bool bSimpleFileSpec) const {
  std::vector<CPDF_FormField*> fields;
  size_t nCount = m_pFieldTree->m_Root.CountFields();
  for (size_t i = 0; i < nCount; ++i)
    fields.push_back(m_pFieldTree->m_Root.GetFieldAtIndex(i));
  return ExportToFDF(pdf_path, fields, true, bSimpleFileSpec);
}

std::unique_ptr<CFDF_Document> CPDF_InteractiveForm::ExportToFDF(
    const WideString& pdf_path,
    const std::vector<CPDF_FormField*>& fields,
    bool bIncludeOrExclude,
    bool bSimpleFileSpec) const {
  std::unique_ptr<CFDF_Document> pDoc = CFDF_Document::CreateNewDoc();
  if (!pDoc)
    return nullptr;

  CPDF_Dictionary* pMainDict = pDoc->GetRoot()->GetDictFor("FDF");
  if (!pdf_path.IsEmpty()) {
    if (bSimpleFileSpec) {
      WideString wsFilePath = CPDF_FileSpec::EncodeFileName(pdf_path);
      pMainDict->SetNewFor<CPDF_String>(pdfium::stream::kF,
                                        wsFilePath.ToDefANSI(), false);
      pMainDict->SetNewFor<CPDF_String>("UF", wsFilePath);
    } else {
      auto pNewDict = pDoc->New<CPDF_Dictionary>();
      pNewDict->SetNewFor<CPDF_Name>("Type", "Filespec");
      CPDF_FileSpec filespec(pNewDict.get());
      filespec.SetFileName(pdf_path);
      pMainDict->SetFor("F", std::move(pNewDict));
    }
  }

  CPDF_Array* pFields = pMainDict->SetNewFor<CPDF_Array>("Fields");
  size_t nCount = m_pFieldTree->m_Root.CountFields();
  for (size_t i = 0; i < nCount; ++i) {
    CPDF_FormField* pField = m_pFieldTree->m_Root.GetFieldAtIndex(i);
    if (!pField || pField->GetType() == CPDF_FormField::kPushButton)
      continue;

    uint32_t dwFlags = pField->GetFieldFlags();
    if (dwFlags & 0x04)
      continue;

    if (bIncludeOrExclude != pdfium::ContainsValue(fields, pField))
      continue;

    if ((dwFlags & 0x02) != 0 &&
        pField->GetDict()->GetStringFor("V").IsEmpty()) {
      continue;
    }

    WideString fullname = FPDF_GetFullName(pField->GetFieldDict());
    auto pFieldDict = pDoc->New<CPDF_Dictionary>();
    pFieldDict->SetNewFor<CPDF_String>("T", fullname);
    if (pField->GetType() == CPDF_FormField::kCheckBox ||
        pField->GetType() == CPDF_FormField::kRadioButton) {
      WideString csExport = pField->GetCheckValue(false);
      ByteString csBExport = PDF_EncodeText(csExport);
      CPDF_Object* pOpt = FPDF_GetFieldAttr(pField->GetDict(), "Opt");
      if (pOpt)
        pFieldDict->SetNewFor<CPDF_String>("V", csBExport, false);
      else
        pFieldDict->SetNewFor<CPDF_Name>("V", csBExport);
    } else {
      CPDF_Object* pV = FPDF_GetFieldAttr(pField->GetDict(), "V");
      if (pV)
        pFieldDict->SetFor("V", pV->CloneDirectObject());
    }
    pFields->Add(std::move(pFieldDict));
  }
  return pDoc;
}

void CPDF_InteractiveForm::SetFormNotify(IPDF_FormNotify* pNotify) {
  m_pFormNotify = pNotify;
}
