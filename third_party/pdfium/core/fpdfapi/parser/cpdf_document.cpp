// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_document.h"

#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/font/cpdf_fontencoding.h"
#include "core/fpdfapi/page/cpdf_docpagedata.h"
#include "core/fpdfapi/page/cpdf_iccprofile.h"
#include "core/fpdfapi/page/cpdf_pagemodule.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_linearized_header.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_parser.h"
#include "core/fpdfapi/parser/cpdf_read_validator.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/render/cpdf_dibbase.h"
#include "core/fpdfapi/render/cpdf_docrenderdata.h"
#include "core/fxcodec/JBig2_DocumentContext.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxge/cfx_unicodeencoding.h"
#include "core/fxge/fx_font.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

namespace {

const int kMaxPageLevel = 1024;

void InsertWidthArrayImpl(std::vector<int> widths, CPDF_Array* pWidthArray) {
  size_t i;
  for (i = 1; i < widths.size(); i++) {
    if (widths[i] != widths[0])
      break;
  }
  if (i == widths.size()) {
    int first = pWidthArray->GetIntegerAt(pWidthArray->size() - 1);
    pWidthArray->AddNew<CPDF_Number>(first + static_cast<int>(widths.size()) -
                                     1);
    pWidthArray->AddNew<CPDF_Number>(widths[0]);
    return;
  }
  CPDF_Array* pWidthArray1 = pWidthArray->AddNew<CPDF_Array>();
  for (int w : widths)
    pWidthArray1->AddNew<CPDF_Number>(w);
}

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
void InsertWidthArray(HDC hDC, int start, int end, CPDF_Array* pWidthArray) {
  std::vector<int> widths(end - start + 1);
  GetCharWidth(hDC, start, end, widths.data());
  InsertWidthArrayImpl(std::move(widths), pWidthArray);
}

ByteString FPDF_GetPSNameFromTT(HDC hDC) {
  ByteString result;
  DWORD size = ::GetFontData(hDC, 'eman', 0, nullptr, 0);
  if (size != GDI_ERROR) {
    LPBYTE buffer = FX_Alloc(BYTE, size);
    ::GetFontData(hDC, 'eman', 0, buffer, size);
    result = GetNameFromTT(buffer, size, 6);
    FX_Free(buffer);
  }
  return result;
}
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

void InsertWidthArray1(CFX_Font* pFont,
                       CFX_UnicodeEncoding* pEncoding,
                       wchar_t start,
                       wchar_t end,
                       CPDF_Array* pWidthArray) {
  std::vector<int> widths(end - start + 1);
  for (size_t i = 0; i < widths.size(); ++i) {
    int glyph_index = pEncoding->GlyphFromCharCode(start + i);
    widths[i] = pFont->GetGlyphWidth(glyph_index);
  }
  InsertWidthArrayImpl(std::move(widths), pWidthArray);
}

int CountPages(CPDF_Dictionary* pPages,
               std::set<CPDF_Dictionary*>* visited_pages) {
  int count = pPages->GetIntegerFor("Count");
  if (count > 0 && count < CPDF_Document::kPageMaxNum)
    return count;
  CPDF_Array* pKidList = pPages->GetArrayFor("Kids");
  if (!pKidList)
    return 0;
  count = 0;
  for (size_t i = 0; i < pKidList->size(); i++) {
    CPDF_Dictionary* pKid = pKidList->GetDictAt(i);
    if (!pKid || pdfium::ContainsKey(*visited_pages, pKid))
      continue;
    if (pKid->KeyExist("Kids")) {
      // Use |visited_pages| to help detect circular references of pages.
      pdfium::ScopedSetInsertion<CPDF_Dictionary*> local_add(visited_pages,
                                                             pKid);
      count += CountPages(pKid, visited_pages);
    } else {
      // This page is a leaf node.
      count++;
    }
  }
  pPages->SetNewFor<CPDF_Number>("Count", count);
  return count;
}

int CalculateFlags(bool bold,
                   bool italic,
                   bool fixedPitch,
                   bool serif,
                   bool script,
                   bool symbolic) {
  int flags = 0;
  if (bold)
    flags |= FXFONT_BOLD;
  if (italic)
    flags |= FXFONT_ITALIC;
  if (fixedPitch)
    flags |= FXFONT_FIXED_PITCH;
  if (serif)
    flags |= FXFONT_SERIF;
  if (script)
    flags |= FXFONT_SCRIPT;
  if (symbolic)
    flags |= FXFONT_SYMBOLIC;
  else
    flags |= FXFONT_NONSYMBOLIC;
  return flags;
}

void ProcessNonbCJK(CPDF_Dictionary* pBaseDict,
                    bool bold,
                    bool italic,
                    ByteString basefont,
                    std::unique_ptr<CPDF_Array> pWidths) {
  if (bold && italic)
    basefont += ",BoldItalic";
  else if (bold)
    basefont += ",Bold";
  else if (italic)
    basefont += ",Italic";
  pBaseDict->SetNewFor<CPDF_Name>("Subtype", "TrueType");
  pBaseDict->SetNewFor<CPDF_Name>("BaseFont", basefont);
  pBaseDict->SetNewFor<CPDF_Number>("FirstChar", 32);
  pBaseDict->SetNewFor<CPDF_Number>("LastChar", 255);
  pBaseDict->SetFor("Widths", std::move(pWidths));
}

std::unique_ptr<CPDF_Dictionary> CalculateFontDesc(
    CPDF_Document* pDoc,
    ByteString basefont,
    int flags,
    int italicangle,
    int ascend,
    int descend,
    std::unique_ptr<CPDF_Array> bbox,
    int32_t stemV) {
  auto pFontDesc = pDoc->New<CPDF_Dictionary>();
  pFontDesc->SetNewFor<CPDF_Name>("Type", "FontDescriptor");
  pFontDesc->SetNewFor<CPDF_Name>("FontName", basefont);
  pFontDesc->SetNewFor<CPDF_Number>("Flags", flags);
  pFontDesc->SetFor("FontBBox", std::move(bbox));
  pFontDesc->SetNewFor<CPDF_Number>("ItalicAngle", italicangle);
  pFontDesc->SetNewFor<CPDF_Number>("Ascent", ascend);
  pFontDesc->SetNewFor<CPDF_Number>("Descent", descend);
  pFontDesc->SetNewFor<CPDF_Number>("StemV", stemV);
  return pFontDesc;
}

}  // namespace

CPDF_Document::CPDF_Document()
    : m_pDocPage(pdfium::MakeUnique<CPDF_DocPageData>(this)),
      m_pDocRender(pdfium::MakeUnique<CPDF_DocRenderData>(this)),
      m_StockFontClearer(this) {}

CPDF_Document::~CPDF_Document() = default;

std::unique_ptr<CPDF_Object> CPDF_Document::ParseIndirectObject(
    uint32_t objnum) {
  return m_pParser ? m_pParser->ParseIndirectObject(objnum) : nullptr;
}

bool CPDF_Document::TryInit() {
  SetLastObjNum(m_pParser->GetLastObjNum());

  CPDF_Object* pRootObj = GetOrParseIndirectObject(m_pParser->GetRootObjNum());
  if (pRootObj)
    m_pRootDict = pRootObj->GetDict();

  LoadPages();
  return GetRoot() && GetPageCount() > 0;
}

CPDF_Parser::Error CPDF_Document::LoadDoc(
    const RetainPtr<IFX_SeekableReadStream>& pFileAccess,
    const char* password) {
  if (!m_pParser)
    SetParser(pdfium::MakeUnique<CPDF_Parser>(this));

  return HandleLoadResult(m_pParser->StartParse(pFileAccess, password));
}

CPDF_Parser::Error CPDF_Document::LoadLinearizedDoc(
    const RetainPtr<CPDF_ReadValidator>& validator,
    const char* password) {
  if (!m_pParser)
    SetParser(pdfium::MakeUnique<CPDF_Parser>(this));

  return HandleLoadResult(m_pParser->StartLinearizedParse(validator, password));
}

void CPDF_Document::LoadPages() {
  const CPDF_LinearizedHeader* linearized_header =
      m_pParser->GetLinearizedHeader();
  if (!linearized_header) {
    m_PageList.resize(RetrievePageCount());
    return;
  }

  m_PageList.resize(linearized_header->GetPageCount());
  ASSERT(linearized_header->GetFirstPageNo() < m_PageList.size());
  m_PageList[linearized_header->GetFirstPageNo()] =
      linearized_header->GetFirstPageObjNum();
}

CPDF_Dictionary* CPDF_Document::TraversePDFPages(int iPage,
                                                 int* nPagesToGo,
                                                 size_t level) {
  if (*nPagesToGo < 0 || m_bReachedMaxPageLevel)
    return nullptr;

  CPDF_Dictionary* pPages = m_pTreeTraversal[level].first;
  CPDF_Array* pKidList = pPages->GetArrayFor("Kids");
  if (!pKidList) {
    m_pTreeTraversal.pop_back();
    if (*nPagesToGo != 1)
      return nullptr;
    m_PageList[iPage] = pPages->GetObjNum();
    return pPages;
  }
  if (level >= kMaxPageLevel) {
    m_pTreeTraversal.pop_back();
    m_bReachedMaxPageLevel = true;
    return nullptr;
  }
  CPDF_Dictionary* page = nullptr;
  for (size_t i = m_pTreeTraversal[level].second; i < pKidList->size(); i++) {
    if (*nPagesToGo == 0)
      break;
    pKidList->ConvertToIndirectObjectAt(i, this);
    CPDF_Dictionary* pKid = pKidList->GetDictAt(i);
    if (!pKid) {
      (*nPagesToGo)--;
      m_pTreeTraversal[level].second++;
      continue;
    }
    if (pKid == pPages) {
      m_pTreeTraversal[level].second++;
      continue;
    }
    if (!pKid->KeyExist("Kids")) {
      m_PageList[iPage - (*nPagesToGo) + 1] = pKid->GetObjNum();
      (*nPagesToGo)--;
      m_pTreeTraversal[level].second++;
      if (*nPagesToGo == 0) {
        page = pKid;
        break;
      }
    } else {
      // If the vector has size level+1, the child is not in yet
      if (m_pTreeTraversal.size() == level + 1)
        m_pTreeTraversal.push_back(std::make_pair(pKid, 0));
      // Now m_pTreeTraversal[level+1] should exist and be equal to pKid.
      CPDF_Dictionary* pageKid = TraversePDFPages(iPage, nPagesToGo, level + 1);
      // Check if child was completely processed, i.e. it popped itself out
      if (m_pTreeTraversal.size() == level + 1)
        m_pTreeTraversal[level].second++;
      // If child did not finish, no pages to go, or max level reached, end
      if (m_pTreeTraversal.size() != level + 1 || *nPagesToGo == 0 ||
          m_bReachedMaxPageLevel) {
        page = pageKid;
        break;
      }
    }
  }
  if (m_pTreeTraversal[level].second == pKidList->size())
    m_pTreeTraversal.pop_back();
  return page;
}

void CPDF_Document::ResetTraversal() {
  m_iNextPageToTraverse = 0;
  m_bReachedMaxPageLevel = false;
  m_pTreeTraversal.clear();
}

void CPDF_Document::SetParser(std::unique_ptr<CPDF_Parser> pParser) {
  ASSERT(!m_pParser);
  m_pParser = std::move(pParser);
}

CPDF_Parser::Error CPDF_Document::HandleLoadResult(CPDF_Parser::Error error) {
  if (error == CPDF_Parser::SUCCESS)
    m_bHasValidCrossReferenceTable = !m_pParser->xref_table_rebuilt();
  return error;
}

const CPDF_Dictionary* CPDF_Document::GetPagesDict() const {
  const CPDF_Dictionary* pRoot = GetRoot();
  return pRoot ? pRoot->GetDictFor("Pages") : nullptr;
}

CPDF_Dictionary* CPDF_Document::GetPagesDict() {
  return const_cast<CPDF_Dictionary*>(
      static_cast<const CPDF_Document*>(this)->GetPagesDict());
}

bool CPDF_Document::IsPageLoaded(int iPage) const {
  return !!m_PageList[iPage];
}

CPDF_Dictionary* CPDF_Document::GetPageDictionary(int iPage) {
  if (!pdfium::IndexInBounds(m_PageList, iPage))
    return nullptr;

  const uint32_t objnum = m_PageList[iPage];
  if (objnum) {
    CPDF_Dictionary* result = ToDictionary(GetOrParseIndirectObject(objnum));
    if (result)
      return result;
  }

  CPDF_Dictionary* pPages = GetPagesDict();
  if (!pPages)
    return nullptr;

  if (m_pTreeTraversal.empty()) {
    ResetTraversal();
    m_pTreeTraversal.push_back(std::make_pair(pPages, 0));
  }
  int nPagesToGo = iPage - m_iNextPageToTraverse + 1;
  CPDF_Dictionary* pPage = TraversePDFPages(iPage, &nPagesToGo, 0);
  m_iNextPageToTraverse = iPage + 1;
  return pPage;
}

void CPDF_Document::SetPageObjNum(int iPage, uint32_t objNum) {
  m_PageList[iPage] = objNum;
}

int CPDF_Document::FindPageIndex(const CPDF_Dictionary* pNode,
                                 uint32_t* skip_count,
                                 uint32_t objnum,
                                 int* index,
                                 int level) const {
  if (!pNode->KeyExist("Kids")) {
    if (objnum == pNode->GetObjNum())
      return *index;

    if (*skip_count)
      (*skip_count)--;

    (*index)++;
    return -1;
  }

  const CPDF_Array* pKidList = pNode->GetArrayFor("Kids");
  if (!pKidList)
    return -1;

  if (level >= kMaxPageLevel)
    return -1;

  size_t count = pNode->GetIntegerFor("Count");
  if (count <= *skip_count) {
    (*skip_count) -= count;
    (*index) += count;
    return -1;
  }

  if (count && count == pKidList->size()) {
    for (size_t i = 0; i < count; i++) {
      const CPDF_Reference* pKid = ToReference(pKidList->GetObjectAt(i));
      if (pKid && pKid->GetRefObjNum() == objnum)
        return static_cast<int>(*index + i);
    }
  }

  for (size_t i = 0; i < pKidList->size(); i++) {
    const CPDF_Dictionary* pKid = pKidList->GetDictAt(i);
    if (!pKid || pKid == pNode)
      continue;

    int found_index = FindPageIndex(pKid, skip_count, objnum, index, level + 1);
    if (found_index >= 0)
      return found_index;
  }
  return -1;
}

int CPDF_Document::GetPageIndex(uint32_t objnum) {
  uint32_t nPages = m_PageList.size();
  uint32_t skip_count = 0;
  bool bSkipped = false;
  for (uint32_t i = 0; i < nPages; i++) {
    if (m_PageList[i] == objnum)
      return i;

    if (!bSkipped && m_PageList[i] == 0) {
      skip_count = i;
      bSkipped = true;
    }
  }
  const CPDF_Dictionary* pPages = GetPagesDict();
  if (!pPages)
    return -1;

  int start_index = 0;
  int found_index = FindPageIndex(pPages, &skip_count, objnum, &start_index, 0);

  // Corrupt page tree may yield out-of-range results.
  if (!pdfium::IndexInBounds(m_PageList, found_index))
    return -1;

  m_PageList[found_index] = objnum;
  return found_index;
}

int CPDF_Document::GetPageCount() const {
  return pdfium::CollectionSize<int>(m_PageList);
}

int CPDF_Document::RetrievePageCount() {
  CPDF_Dictionary* pPages = GetPagesDict();
  if (!pPages)
    return 0;

  if (!pPages->KeyExist("Kids"))
    return 1;

  std::set<CPDF_Dictionary*> visited_pages;
  visited_pages.insert(pPages);
  return CountPages(pPages, &visited_pages);
}

uint32_t CPDF_Document::GetUserPermissions() const {
  if (m_pParser)
    return m_pParser->GetPermissions();

  return m_pExtension ? m_pExtension->GetUserPermissions() : 0;
}

CPDF_Font* CPDF_Document::LoadFont(CPDF_Dictionary* pFontDict) {
  ASSERT(pFontDict);
  return m_pDocPage->GetFont(pFontDict);
}

RetainPtr<CPDF_StreamAcc> CPDF_Document::LoadFontFile(
    const CPDF_Stream* pStream) {
  return m_pDocPage->GetFontFileStreamAcc(pStream);
}

CPDF_ColorSpace* CPDF_Document::LoadColorSpace(
    const CPDF_Object* pCSObj,
    const CPDF_Dictionary* pResources) {
  return m_pDocPage->GetColorSpace(pCSObj, pResources);
}

CPDF_Pattern* CPDF_Document::LoadPattern(CPDF_Object* pPatternObj,
                                         bool bShading,
                                         const CFX_Matrix& matrix) {
  return m_pDocPage->GetPattern(pPatternObj, bShading, matrix);
}

RetainPtr<CPDF_IccProfile> CPDF_Document::LoadIccProfile(
    const CPDF_Stream* pStream) {
  return m_pDocPage->GetIccProfile(pStream);
}

RetainPtr<CPDF_Image> CPDF_Document::LoadImageFromPageData(
    uint32_t dwStreamObjNum) {
  ASSERT(dwStreamObjNum);
  return m_pDocPage->GetImage(dwStreamObjNum);
}

void CPDF_Document::CreateNewDoc() {
  ASSERT(!m_pRootDict);
  ASSERT(!m_pInfoDict);
  m_pRootDict = NewIndirect<CPDF_Dictionary>();
  m_pRootDict->SetNewFor<CPDF_Name>("Type", "Catalog");

  CPDF_Dictionary* pPages = NewIndirect<CPDF_Dictionary>();
  pPages->SetNewFor<CPDF_Name>("Type", "Pages");
  pPages->SetNewFor<CPDF_Number>("Count", 0);
  pPages->SetNewFor<CPDF_Array>("Kids");
  m_pRootDict->SetFor("Pages", pPages->MakeReference(this));
  m_pInfoDict = NewIndirect<CPDF_Dictionary>();
}

CPDF_Dictionary* CPDF_Document::CreateNewPage(int iPage) {
  CPDF_Dictionary* pDict = NewIndirect<CPDF_Dictionary>();
  pDict->SetNewFor<CPDF_Name>("Type", "Page");
  uint32_t dwObjNum = pDict->GetObjNum();
  if (!InsertNewPage(iPage, pDict)) {
    DeleteIndirectObject(dwObjNum);
    return nullptr;
  }
  return pDict;
}

bool CPDF_Document::InsertDeletePDFPage(CPDF_Dictionary* pPages,
                                        int nPagesToGo,
                                        CPDF_Dictionary* pPageDict,
                                        bool bInsert,
                                        std::set<CPDF_Dictionary*>* pVisited) {
  CPDF_Array* pKidList = pPages->GetArrayFor("Kids");
  if (!pKidList)
    return false;

  for (size_t i = 0; i < pKidList->size(); i++) {
    CPDF_Dictionary* pKid = pKidList->GetDictAt(i);
    if (pKid->GetStringFor("Type") == "Page") {
      if (nPagesToGo != 0) {
        nPagesToGo--;
        continue;
      }
      if (bInsert) {
        pKidList->InsertAt(i, pPageDict->MakeReference(this));
        pPageDict->SetFor("Parent", pPages->MakeReference(this));
      } else {
        pKidList->RemoveAt(i);
      }
      pPages->SetNewFor<CPDF_Number>(
          "Count", pPages->GetIntegerFor("Count") + (bInsert ? 1 : -1));
      ResetTraversal();
      break;
    }
    int nPages = pKid->GetIntegerFor("Count");
    if (nPagesToGo >= nPages) {
      nPagesToGo -= nPages;
      continue;
    }
    if (pdfium::ContainsKey(*pVisited, pKid))
      return false;

    pdfium::ScopedSetInsertion<CPDF_Dictionary*> insertion(pVisited, pKid);
    if (!InsertDeletePDFPage(pKid, nPagesToGo, pPageDict, bInsert, pVisited))
      return false;

    pPages->SetNewFor<CPDF_Number>(
        "Count", pPages->GetIntegerFor("Count") + (bInsert ? 1 : -1));
    break;
  }
  return true;
}

bool CPDF_Document::InsertNewPage(int iPage, CPDF_Dictionary* pPageDict) {
  CPDF_Dictionary* pRoot = GetRoot();
  CPDF_Dictionary* pPages = pRoot ? pRoot->GetDictFor("Pages") : nullptr;
  if (!pPages)
    return false;

  int nPages = GetPageCount();
  if (iPage < 0 || iPage > nPages)
    return false;

  if (iPage == nPages) {
    CPDF_Array* pPagesList = pPages->GetArrayFor("Kids");
    if (!pPagesList)
      pPagesList = pPages->SetNewFor<CPDF_Array>("Kids");
    pPagesList->Add(pPageDict->MakeReference(this));
    pPages->SetNewFor<CPDF_Number>("Count", nPages + 1);
    pPageDict->SetFor("Parent", pPages->MakeReference(this));
    ResetTraversal();
  } else {
    std::set<CPDF_Dictionary*> stack = {pPages};
    if (!InsertDeletePDFPage(pPages, iPage, pPageDict, true, &stack))
      return false;
  }
  m_PageList.insert(m_PageList.begin() + iPage, pPageDict->GetObjNum());
  return true;
}

CPDF_Dictionary* CPDF_Document::GetInfo() {
  if (m_pInfoDict)
    return m_pInfoDict.Get();

  if (!m_pParser || !m_pParser->GetInfoObjNum())
    return nullptr;

  CPDF_Reference ref(this, m_pParser->GetInfoObjNum());
  m_pInfoDict = ToDictionary(ref.GetDirect());
  return m_pInfoDict.Get();
}

void CPDF_Document::DeletePage(int iPage) {
  CPDF_Dictionary* pPages = GetPagesDict();
  if (!pPages)
    return;

  int nPages = pPages->GetIntegerFor("Count");
  if (iPage < 0 || iPage >= nPages)
    return;

  std::set<CPDF_Dictionary*> stack = {pPages};
  if (!InsertDeletePDFPage(pPages, iPage, nullptr, false, &stack))
    return;

  m_PageList.erase(m_PageList.begin() + iPage);
}

CPDF_Font* CPDF_Document::AddStandardFont(const char* font,
                                          CPDF_FontEncoding* pEncoding) {
  ByteString name(font);
  if (PDF_GetStandardFontName(&name) < 0)
    return nullptr;
  return GetPageData()->GetStandardFont(name, pEncoding);
}

size_t CPDF_Document::CalculateEncodingDict(int charset,
                                            CPDF_Dictionary* pBaseDict) {
  size_t i;
  for (i = 0; i < FX_ArraySize(g_FX_CharsetUnicodes); ++i) {
    if (g_FX_CharsetUnicodes[i].m_Charset == charset)
      break;
  }
  if (i == FX_ArraySize(g_FX_CharsetUnicodes))
    return i;

  CPDF_Dictionary* pEncodingDict = NewIndirect<CPDF_Dictionary>();
  pEncodingDict->SetNewFor<CPDF_Name>("BaseEncoding", "WinAnsiEncoding");

  CPDF_Array* pArray = pEncodingDict->SetNewFor<CPDF_Array>("Differences");
  pArray->AddNew<CPDF_Number>(128);

  const uint16_t* pUnicodes = g_FX_CharsetUnicodes[i].m_pUnicodes;
  for (int j = 0; j < 128; j++) {
    ByteString name = PDF_AdobeNameFromUnicode(pUnicodes[j]);
    pArray->AddNew<CPDF_Name>(name.IsEmpty() ? ".notdef" : name);
  }
  pBaseDict->SetFor("Encoding", pEncodingDict->MakeReference(this));
  return i;
}

CPDF_Dictionary* CPDF_Document::ProcessbCJK(
    CPDF_Dictionary* pBaseDict,
    int charset,
    ByteString basefont,
    std::function<void(wchar_t, wchar_t, CPDF_Array*)> Insert) {
  CPDF_Dictionary* pFontDict = NewIndirect<CPDF_Dictionary>();
  ByteString cmap;
  ByteString ordering;
  int supplement = 0;
  CPDF_Array* pWidthArray = pFontDict->SetNewFor<CPDF_Array>("W");
  switch (charset) {
    case FX_CHARSET_ChineseTraditional:
      cmap = "ETenms-B5-H";
      ordering = "CNS1";
      supplement = 4;
      pWidthArray->AddNew<CPDF_Number>(1);
      Insert(0x20, 0x7e, pWidthArray);
      break;
    case FX_CHARSET_ChineseSimplified:
      cmap = "GBK-EUC-H";
      ordering = "GB1";
      supplement = 2;
      pWidthArray->AddNew<CPDF_Number>(7716);
      Insert(0x20, 0x20, pWidthArray);
      pWidthArray->AddNew<CPDF_Number>(814);
      Insert(0x21, 0x7e, pWidthArray);
      break;
    case FX_CHARSET_Hangul:
      cmap = "KSCms-UHC-H";
      ordering = "Korea1";
      supplement = 2;
      pWidthArray->AddNew<CPDF_Number>(1);
      Insert(0x20, 0x7e, pWidthArray);
      break;
    case FX_CHARSET_ShiftJIS:
      cmap = "90ms-RKSJ-H";
      ordering = "Japan1";
      supplement = 5;
      pWidthArray->AddNew<CPDF_Number>(231);
      Insert(0x20, 0x7d, pWidthArray);
      pWidthArray->AddNew<CPDF_Number>(326);
      Insert(0xa0, 0xa0, pWidthArray);
      pWidthArray->AddNew<CPDF_Number>(327);
      Insert(0xa1, 0xdf, pWidthArray);
      pWidthArray->AddNew<CPDF_Number>(631);
      Insert(0x7e, 0x7e, pWidthArray);
      break;
  }
  pBaseDict->SetNewFor<CPDF_Name>("Subtype", "Type0");
  pBaseDict->SetNewFor<CPDF_Name>("BaseFont", basefont);
  pBaseDict->SetNewFor<CPDF_Name>("Encoding", cmap);
  pFontDict->SetNewFor<CPDF_Name>("Type", "Font");
  pFontDict->SetNewFor<CPDF_Name>("Subtype", "CIDFontType2");
  pFontDict->SetNewFor<CPDF_Name>("BaseFont", basefont);

  CPDF_Dictionary* pCIDSysInfo =
      pFontDict->SetNewFor<CPDF_Dictionary>("CIDSystemInfo");
  pCIDSysInfo->SetNewFor<CPDF_String>("Registry", "Adobe", false);
  pCIDSysInfo->SetNewFor<CPDF_String>("Ordering", ordering, false);
  pCIDSysInfo->SetNewFor<CPDF_Number>("Supplement", supplement);

  CPDF_Array* pArray = pBaseDict->SetNewFor<CPDF_Array>("DescendantFonts");
  pArray->Add(pFontDict->MakeReference(this));
  return pFontDict;
}

CPDF_Font* CPDF_Document::AddFont(CFX_Font* pFont, int charset) {
  if (!pFont)
    return nullptr;

  const bool bCJK = FX_CharSetIsCJK(charset);
  ByteString basefont = pFont->GetFamilyName();
  basefont.Replace(" ", "");
  int flags =
      CalculateFlags(pFont->IsBold(), pFont->IsItalic(), pFont->IsFixedWidth(),
                     false, false, charset == FX_CHARSET_Symbol);

  CPDF_Dictionary* pBaseDict = NewIndirect<CPDF_Dictionary>();
  pBaseDict->SetNewFor<CPDF_Name>("Type", "Font");
  auto pEncoding = pdfium::MakeUnique<CFX_UnicodeEncoding>(pFont);
  CPDF_Dictionary* pFontDict = pBaseDict;
  if (!bCJK) {
    auto pWidths = pdfium::MakeUnique<CPDF_Array>();
    for (int charcode = 32; charcode < 128; charcode++) {
      int glyph_index = pEncoding->GlyphFromCharCode(charcode);
      int char_width = pFont->GetGlyphWidth(glyph_index);
      pWidths->AddNew<CPDF_Number>(char_width);
    }
    if (charset == FX_CHARSET_ANSI || charset == FX_CHARSET_Default ||
        charset == FX_CHARSET_Symbol) {
      pBaseDict->SetNewFor<CPDF_Name>("Encoding", "WinAnsiEncoding");
      for (int charcode = 128; charcode <= 255; charcode++) {
        int glyph_index = pEncoding->GlyphFromCharCode(charcode);
        int char_width = pFont->GetGlyphWidth(glyph_index);
        pWidths->AddNew<CPDF_Number>(char_width);
      }
    } else {
      size_t i = CalculateEncodingDict(charset, pBaseDict);
      if (i < FX_ArraySize(g_FX_CharsetUnicodes)) {
        const uint16_t* pUnicodes = g_FX_CharsetUnicodes[i].m_pUnicodes;
        for (int j = 0; j < 128; j++) {
          int glyph_index = pEncoding->GlyphFromCharCode(pUnicodes[j]);
          int char_width = pFont->GetGlyphWidth(glyph_index);
          pWidths->AddNew<CPDF_Number>(char_width);
        }
      }
    }
    ProcessNonbCJK(pBaseDict, pFont->IsBold(), pFont->IsItalic(), basefont,
                   std::move(pWidths));
  } else {
    pFontDict = ProcessbCJK(
        pBaseDict, charset, basefont,
        [pFont, &pEncoding](wchar_t start, wchar_t end, CPDF_Array* widthArr) {
          InsertWidthArray1(pFont, pEncoding.get(), start, end, widthArr);
        });
  }
  int italicangle =
      pFont->GetSubstFont() ? pFont->GetSubstFont()->m_ItalicAngle : 0;
  FX_RECT bbox;
  pFont->GetBBox(&bbox);
  auto pBBox = pdfium::MakeUnique<CPDF_Array>();
  pBBox->AddNew<CPDF_Number>(bbox.left);
  pBBox->AddNew<CPDF_Number>(bbox.bottom);
  pBBox->AddNew<CPDF_Number>(bbox.right);
  pBBox->AddNew<CPDF_Number>(bbox.top);
  int32_t nStemV = 0;
  if (pFont->GetSubstFont()) {
    nStemV = pFont->GetSubstFont()->m_Weight / 5;
  } else {
    static const char stem_chars[] = {'i', 'I', '!', '1'};
    const size_t count = FX_ArraySize(stem_chars);
    uint32_t glyph = pEncoding->GlyphFromCharCode(stem_chars[0]);
    nStemV = pFont->GetGlyphWidth(glyph);
    for (size_t i = 1; i < count; i++) {
      glyph = pEncoding->GlyphFromCharCode(stem_chars[i]);
      int width = pFont->GetGlyphWidth(glyph);
      if (width > 0 && width < nStemV)
        nStemV = width;
    }
  }
  CPDF_Dictionary* pFontDesc = ToDictionary(AddIndirectObject(
      CalculateFontDesc(this, basefont, flags, italicangle, pFont->GetAscent(),
                        pFont->GetDescent(), std::move(pBBox), nStemV)));
  pFontDict->SetFor("FontDescriptor", pFontDesc->MakeReference(this));
  return LoadFont(pBaseDict);
}

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
CPDF_Font* CPDF_Document::AddWindowsFont(LOGFONTA* pLogFont) {
  pLogFont->lfHeight = -1000;
  pLogFont->lfWidth = 0;
  HGDIOBJ hFont = CreateFontIndirectA(pLogFont);
  HDC hDC = CreateCompatibleDC(nullptr);
  hFont = SelectObject(hDC, hFont);
  int tm_size = GetOutlineTextMetrics(hDC, 0, nullptr);
  if (tm_size == 0) {
    hFont = SelectObject(hDC, hFont);
    DeleteObject(hFont);
    DeleteDC(hDC);
    return nullptr;
  }

  LPBYTE tm_buf = FX_Alloc(BYTE, tm_size);
  OUTLINETEXTMETRIC* ptm = reinterpret_cast<OUTLINETEXTMETRIC*>(tm_buf);
  GetOutlineTextMetrics(hDC, tm_size, ptm);
  int flags = CalculateFlags(false, pLogFont->lfItalic != 0,
                             (pLogFont->lfPitchAndFamily & 3) == FIXED_PITCH,
                             (pLogFont->lfPitchAndFamily & 0xf8) == FF_ROMAN,
                             (pLogFont->lfPitchAndFamily & 0xf8) == FF_SCRIPT,
                             pLogFont->lfCharSet == FX_CHARSET_Symbol);

  const bool bCJK = FX_CharSetIsCJK(pLogFont->lfCharSet);
  ByteString basefont;
  if (bCJK)
    basefont = FPDF_GetPSNameFromTT(hDC);

  if (basefont.IsEmpty())
    basefont = pLogFont->lfFaceName;

  int italicangle = ptm->otmItalicAngle / 10;
  int ascend = ptm->otmrcFontBox.top;
  int descend = ptm->otmrcFontBox.bottom;
  int capheight = ptm->otmsCapEmHeight;
  int bbox[4] = {ptm->otmrcFontBox.left, ptm->otmrcFontBox.bottom,
                 ptm->otmrcFontBox.right, ptm->otmrcFontBox.top};
  FX_Free(tm_buf);
  basefont.Replace(" ", "");
  CPDF_Dictionary* pBaseDict = NewIndirect<CPDF_Dictionary>();
  pBaseDict->SetNewFor<CPDF_Name>("Type", "Font");
  CPDF_Dictionary* pFontDict = pBaseDict;
  if (!bCJK) {
    if (pLogFont->lfCharSet == FX_CHARSET_ANSI ||
        pLogFont->lfCharSet == FX_CHARSET_Default ||
        pLogFont->lfCharSet == FX_CHARSET_Symbol) {
      pBaseDict->SetNewFor<CPDF_Name>("Encoding", "WinAnsiEncoding");
    } else {
      CalculateEncodingDict(pLogFont->lfCharSet, pBaseDict);
    }
    int char_widths[224];
    GetCharWidth(hDC, 32, 255, char_widths);
    auto pWidths = pdfium::MakeUnique<CPDF_Array>();
    for (size_t i = 0; i < 224; i++)
      pWidths->AddNew<CPDF_Number>(char_widths[i]);
    ProcessNonbCJK(pBaseDict, pLogFont->lfWeight > FW_MEDIUM,
                   pLogFont->lfItalic != 0, basefont, std::move(pWidths));
  } else {
    pFontDict =
        ProcessbCJK(pBaseDict, pLogFont->lfCharSet, basefont,
                    [&hDC](wchar_t start, wchar_t end, CPDF_Array* widthArr) {
                      InsertWidthArray(hDC, start, end, widthArr);
                    });
  }
  auto pBBox = pdfium::MakeUnique<CPDF_Array>();
  for (int i = 0; i < 4; i++)
    pBBox->AddNew<CPDF_Number>(bbox[i]);
  std::unique_ptr<CPDF_Dictionary> pFontDesc =
      CalculateFontDesc(this, basefont, flags, italicangle, ascend, descend,
                        std::move(pBBox), pLogFont->lfWeight / 5);
  pFontDesc->SetNewFor<CPDF_Number>("CapHeight", capheight);
  pFontDict->SetFor(
      "FontDescriptor",
      AddIndirectObject(std::move(pFontDesc))->MakeReference(this));
  hFont = SelectObject(hDC, hFont);
  DeleteObject(hFont);
  DeleteDC(hDC);
  return LoadFont(pBaseDict);
}
#endif  //  _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

CPDF_Document::StockFontClearer::StockFontClearer(CPDF_Document* pDoc)
    : m_pDoc(pDoc) {}

CPDF_Document::StockFontClearer::~StockFontClearer() {
  CPDF_ModuleMgr::Get()->GetPageModule()->ClearStockFont(m_pDoc.Get());
}
