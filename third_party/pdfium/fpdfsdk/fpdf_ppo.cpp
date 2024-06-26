// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_ppo.h"

#include <algorithm>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "constants/page_object.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "public/cpp/fpdf_scopers.h"
#include "third_party/base/ptr_util.h"

namespace {

// Struct that stores sub page origin and scale information.  When importing
// more than one pages onto the same page, most likely the pages will need to be
// scaled down, and scale is in range of (0, 1) exclusive.
struct NupPageSettings {
  CFX_PointF subPageStartPoint;
  float scale;
};

// Calculates the N-up parameters.  When importing multiple pages into one page.
// The space of output page is evenly divided along the X axis and Y axis based
// on the input |numPagesOnXAxis| and |numPagesOnYAxis|.
class NupState {
 public:
  NupState(const CFX_SizeF& pagesize,
           unsigned int numPagesOnXAxis,
           unsigned int numPagesOnYAxis);

  // Calculate sub page origin and scale with the source page of |pagesize| and
  // new page of |m_subPageSize|.
  NupPageSettings CalculateNewPagePosition(const CFX_SizeF& pagesize);

 private:
  // Helper function to get the subX, subY pair based on |m_subPageIndex|.
  // The space of output page is evenly divided into slots along x and y axis.
  // subX and subY are 0-based indices that indicate which allocation slot to
  // use.
  std::pair<size_t, size_t> ConvertPageOrder() const;

  // Given the |subX| and |subY| subpage position within a page, and a source
  // page with dimensions of |pagesize|, calculate the sub page's origin and
  // scale.
  NupPageSettings CalculatePageEdit(size_t subX,
                                    size_t subY,
                                    const CFX_SizeF& pagesize) const;

  const CFX_SizeF m_destPageSize;
  const size_t m_numPagesOnXAxis;
  const size_t m_numPagesOnYAxis;
  const size_t m_numPagesPerSheet;
  CFX_SizeF m_subPageSize;

  // A 0-based index, in range of [0, m_numPagesPerSheet - 1).
  size_t m_subPageIndex = 0;
};

NupState::NupState(const CFX_SizeF& pagesize,
                   unsigned int numPagesOnXAxis,
                   unsigned int numPagesOnYAxis)
    : m_destPageSize(pagesize),
      m_numPagesOnXAxis(numPagesOnXAxis),
      m_numPagesOnYAxis(numPagesOnYAxis),
      m_numPagesPerSheet(numPagesOnXAxis * numPagesOnYAxis) {
  ASSERT(m_numPagesOnXAxis > 0);
  ASSERT(m_numPagesOnYAxis > 0);
  ASSERT(m_destPageSize.width > 0);
  ASSERT(m_destPageSize.height > 0);

  m_subPageSize.width = m_destPageSize.width / m_numPagesOnXAxis;
  m_subPageSize.height = m_destPageSize.height / m_numPagesOnYAxis;
}

std::pair<size_t, size_t> NupState::ConvertPageOrder() const {
  size_t subX = m_subPageIndex % m_numPagesOnXAxis;
  size_t subY = m_subPageIndex / m_numPagesOnXAxis;

  // Y Axis, pages start from the top of the output page.
  subY = m_numPagesOnYAxis - subY - 1;

  return {subX, subY};
}

NupPageSettings NupState::CalculatePageEdit(size_t subX,
                                            size_t subY,
                                            const CFX_SizeF& pagesize) const {
  NupPageSettings settings;
  settings.subPageStartPoint.x = subX * m_subPageSize.width;
  settings.subPageStartPoint.y = subY * m_subPageSize.height;

  const float xScale = m_subPageSize.width / pagesize.width;
  const float yScale = m_subPageSize.height / pagesize.height;
  settings.scale = std::min(xScale, yScale);

  float subWidth = pagesize.width * settings.scale;
  float subHeight = pagesize.height * settings.scale;
  if (xScale > yScale)
    settings.subPageStartPoint.x += (m_subPageSize.width - subWidth) / 2;
  else
    settings.subPageStartPoint.y += (m_subPageSize.height - subHeight) / 2;
  return settings;
}

NupPageSettings NupState::CalculateNewPagePosition(const CFX_SizeF& pagesize) {
  if (m_subPageIndex >= m_numPagesPerSheet)
    m_subPageIndex = 0;

  size_t subX;
  size_t subY;
  std::tie(subX, subY) = ConvertPageOrder();
  ++m_subPageIndex;
  return CalculatePageEdit(subX, subY, pagesize);
}

const CPDF_Object* PageDictGetInheritableTag(const CPDF_Dictionary* pDict,
                                             const ByteString& bsSrcTag) {
  if (!pDict || bsSrcTag.IsEmpty())
    return nullptr;
  if (!pDict->KeyExist(pdfium::page_object::kParent) ||
      !pDict->KeyExist(pdfium::page_object::kType)) {
    return nullptr;
  }

  const CPDF_Object* pType =
      pDict->GetObjectFor(pdfium::page_object::kType)->GetDirect();
  if (!ToName(pType))
    return nullptr;
  if (pType->GetString().Compare("Page"))
    return nullptr;

  const CPDF_Dictionary* pp = ToDictionary(
      pDict->GetObjectFor(pdfium::page_object::kParent)->GetDirect());
  if (!pp)
    return nullptr;

  if (pDict->KeyExist(bsSrcTag))
    return pDict->GetObjectFor(bsSrcTag);

  while (pp) {
    if (pp->KeyExist(bsSrcTag))
      return pp->GetObjectFor(bsSrcTag);
    if (!pp->KeyExist(pdfium::page_object::kParent))
      break;
    pp = ToDictionary(
        pp->GetObjectFor(pdfium::page_object::kParent)->GetDirect());
  }
  return nullptr;
}

CFX_FloatRect GetMediaBox(const CPDF_Dictionary* pPageDict) {
  const CPDF_Object* pMediaBox =
      PageDictGetInheritableTag(pPageDict, pdfium::page_object::kMediaBox);
  const CPDF_Array* pArray = ToArray(pMediaBox->GetDirect());
  if (!pArray)
    return CFX_FloatRect();
  return pArray->GetRect();
}

CFX_FloatRect GetCropBox(const CPDF_Dictionary* pPageDict) {
  if (pPageDict->KeyExist(pdfium::page_object::kCropBox))
    return pPageDict->GetRectFor(pdfium::page_object::kCropBox);
  return GetMediaBox(pPageDict);
}

const CPDF_Object* GetPageOrganizerPageContent(
    const CPDF_Dictionary* pPageDict) {
  return pPageDict
             ? pPageDict->GetDirectObjectFor(pdfium::page_object::kContents)
             : nullptr;
}

bool CopyInheritable(CPDF_Dictionary* pDestPageDict,
                     const CPDF_Dictionary* pSrcPageDict,
                     const ByteString& key) {
  if (pDestPageDict->KeyExist(key))
    return true;

  const CPDF_Object* pInheritable =
      PageDictGetInheritableTag(pSrcPageDict, key);
  if (!pInheritable)
    return false;

  pDestPageDict->SetFor(key, pInheritable->Clone());
  return true;
}

bool ParsePageRangeString(const ByteString& bsPageRange,
                          uint32_t nCount,
                          std::vector<uint32_t>* pageArray) {
  ByteString bsStrippedPageRange = bsPageRange;
  bsStrippedPageRange.Remove(' ');
  size_t nLength = bsStrippedPageRange.GetLength();
  if (nLength == 0)
    return true;

  static const ByteString cbCompareString("0123456789-,");
  for (size_t i = 0; i < nLength; ++i) {
    if (!cbCompareString.Contains(bsStrippedPageRange[i]))
      return false;
  }

  ByteString cbMidRange;
  size_t nStringFrom = 0;
  size_t nStringTo = 0;
  while (nStringTo < nLength) {
    nStringTo = bsStrippedPageRange.Find(',', nStringFrom).value_or(nLength);
    cbMidRange = bsStrippedPageRange.Mid(nStringFrom, nStringTo - nStringFrom);
    Optional<size_t> nDashPosition = cbMidRange.Find('-');
    if (nDashPosition) {
      size_t nMid = nDashPosition.value();
      uint32_t nStartPageNum = pdfium::base::checked_cast<uint32_t>(
          atoi(cbMidRange.Left(nMid).c_str()));
      if (nStartPageNum == 0)
        return false;

      ++nMid;
      size_t nEnd = cbMidRange.GetLength() - nMid;
      if (nEnd == 0)
        return false;

      uint32_t nEndPageNum = pdfium::base::checked_cast<uint32_t>(
          atoi(cbMidRange.Mid(nMid, nEnd).c_str()));
      if (nStartPageNum < 0 || nStartPageNum > nEndPageNum ||
          nEndPageNum > nCount) {
        return false;
      }
      for (uint32_t i = nStartPageNum; i <= nEndPageNum; ++i) {
        pageArray->push_back(i);
      }
    } else {
      uint32_t nPageNum =
          pdfium::base::checked_cast<uint32_t>(atoi(cbMidRange.c_str()));
      if (nPageNum <= 0 || nPageNum > nCount)
        return false;
      pageArray->push_back(nPageNum);
    }
    nStringFrom = nStringTo + 1;
  }
  return true;
}

std::vector<uint32_t> GetPageNumbers(const CPDF_Document& doc,
                                     const ByteString& bsPageRange) {
  std::vector<uint32_t> page_numbers;
  uint32_t nCount = doc.GetPageCount();
  if (bsPageRange.IsEmpty()) {
    for (uint32_t i = 1; i <= nCount; ++i)
      page_numbers.push_back(i);
  } else {
    if (!ParsePageRangeString(bsPageRange, nCount, &page_numbers))
      page_numbers.clear();
  }
  return page_numbers;
}

class CPDF_PageOrganizer {
 protected:
  // Map source page object number to XObject object number.
  using ObjectNumberMap = std::map<uint32_t, uint32_t>;

  CPDF_PageOrganizer(CPDF_Document* pDestPDFDoc, CPDF_Document* pSrcPDFDoc);
  ~CPDF_PageOrganizer();

  // Must be called after construction before doing anything else.
  bool PDFDocInit();

  bool UpdateReference(CPDF_Object* pObj, ObjectNumberMap* pObjNumberMap);

  CPDF_Document* dest() { return m_pDestPDFDoc.Get(); }
  const CPDF_Document* dest() const { return m_pDestPDFDoc.Get(); }

  CPDF_Document* src() { return m_pSrcPDFDoc.Get(); }
  const CPDF_Document* src() const { return m_pSrcPDFDoc.Get(); }

 private:
  uint32_t GetNewObjId(ObjectNumberMap* pObjNumberMap, CPDF_Reference* pRef);

  UnownedPtr<CPDF_Document> const m_pDestPDFDoc;
  UnownedPtr<CPDF_Document> const m_pSrcPDFDoc;
};

CPDF_PageOrganizer::CPDF_PageOrganizer(CPDF_Document* pDestPDFDoc,
                                       CPDF_Document* pSrcPDFDoc)
    : m_pDestPDFDoc(pDestPDFDoc), m_pSrcPDFDoc(pSrcPDFDoc) {}

CPDF_PageOrganizer::~CPDF_PageOrganizer() {}

bool CPDF_PageOrganizer::PDFDocInit() {
  ASSERT(m_pDestPDFDoc);
  ASSERT(m_pSrcPDFDoc);

  CPDF_Dictionary* pNewRoot = dest()->GetRoot();
  if (!pNewRoot)
    return false;

  CPDF_Dictionary* pDocInfoDict = dest()->GetInfo();
  if (!pDocInfoDict)
    return false;

  pDocInfoDict->SetNewFor<CPDF_String>("Producer", "PDFium", false);

  ByteString cbRootType = pNewRoot->GetStringFor("Type", ByteString());
  if (cbRootType.IsEmpty())
    pNewRoot->SetNewFor<CPDF_Name>("Type", "Catalog");

  CPDF_Object* pElement = pNewRoot->GetObjectFor("Pages");
  CPDF_Dictionary* pNewPages =
      pElement ? ToDictionary(pElement->GetDirect()) : nullptr;
  if (!pNewPages) {
    pNewPages = dest()->NewIndirect<CPDF_Dictionary>();
    pNewRoot->SetFor("Pages", pNewPages->MakeReference(dest()));
  }

  ByteString cbPageType = pNewPages->GetStringFor("Type", ByteString());
  if (cbPageType.IsEmpty())
    pNewPages->SetNewFor<CPDF_Name>("Type", "Pages");

  if (!pNewPages->GetArrayFor("Kids")) {
    pNewPages->SetNewFor<CPDF_Number>("Count", 0);
    pNewPages->SetFor("Kids",
                      dest()->NewIndirect<CPDF_Array>()->MakeReference(dest()));
  }

  return true;
}

bool CPDF_PageOrganizer::UpdateReference(CPDF_Object* pObj,
                                         ObjectNumberMap* pObjNumberMap) {
  switch (pObj->GetType()) {
    case CPDF_Object::kReference: {
      CPDF_Reference* pReference = pObj->AsReference();
      uint32_t newobjnum = GetNewObjId(pObjNumberMap, pReference);
      if (newobjnum == 0)
        return false;
      pReference->SetRef(dest(), newobjnum);
      break;
    }
    case CPDF_Object::kDictionary: {
      CPDF_Dictionary* pDict = pObj->AsDictionary();
      std::vector<ByteString> bad_keys;
      {
        CPDF_DictionaryLocker locker(pDict);
        for (auto it = locker.begin(); it != locker.end(); ++it) {
          const ByteString& key = it->first;
          if (key == "Parent" || key == "Prev" || key == "First")
            continue;
          CPDF_Object* pNextObj = it->second.get();
          if (!pNextObj)
            return false;
          if (!UpdateReference(pNextObj, pObjNumberMap))
            bad_keys.push_back(key);
        }
      }
      for (const auto& key : bad_keys)
        pDict->RemoveFor(key);
      break;
    }
    case CPDF_Object::kArray: {
      CPDF_Array* pArray = pObj->AsArray();
      for (size_t i = 0; i < pArray->size(); ++i) {
        CPDF_Object* pNextObj = pArray->GetObjectAt(i);
        if (!pNextObj)
          return false;
        if (!UpdateReference(pNextObj, pObjNumberMap))
          return false;
      }
      break;
    }
    case CPDF_Object::kStream: {
      CPDF_Stream* pStream = pObj->AsStream();
      CPDF_Dictionary* pDict = pStream->GetDict();
      if (!pDict)
        return false;
      if (!UpdateReference(pDict, pObjNumberMap))
        return false;
      break;
    }
    default:
      break;
  }

  return true;
}

uint32_t CPDF_PageOrganizer::GetNewObjId(ObjectNumberMap* pObjNumberMap,
                                         CPDF_Reference* pRef) {
  if (!pRef)
    return 0;

  uint32_t dwObjnum = pRef->GetRefObjNum();
  uint32_t dwNewObjNum = 0;
  const auto it = pObjNumberMap->find(dwObjnum);
  if (it != pObjNumberMap->end())
    dwNewObjNum = it->second;
  if (dwNewObjNum)
    return dwNewObjNum;

  CPDF_Object* pDirect = pRef->GetDirect();
  if (!pDirect)
    return 0;

  std::unique_ptr<CPDF_Object> pClone = pDirect->Clone();
  if (CPDF_Dictionary* pDictClone = pClone->AsDictionary()) {
    if (pDictClone->KeyExist("Type")) {
      ByteString strType = pDictClone->GetStringFor("Type");
      if (!FXSYS_stricmp(strType.c_str(), "Pages"))
        return 4;
      if (!FXSYS_stricmp(strType.c_str(), "Page"))
        return 0;
    }
  }
  CPDF_Object* pUnownedClone = dest()->AddIndirectObject(std::move(pClone));
  dwNewObjNum = pUnownedClone->GetObjNum();
  (*pObjNumberMap)[dwObjnum] = dwNewObjNum;
  if (!UpdateReference(pUnownedClone, pObjNumberMap))
    return 0;

  return dwNewObjNum;
}

// Copies pages from a source document into a destination document.
// This class is intended to be used once via ExportPage() and then destroyed.
class CPDF_PageExporter final : public CPDF_PageOrganizer {
 public:
  CPDF_PageExporter(CPDF_Document* pDestPDFDoc, CPDF_Document* pSrcPDFDoc);
  ~CPDF_PageExporter();

  // For the pages from the source document with |pageNums| as their page
  // numbers, insert them into the destination document at page |nIndex|.
  // |pageNums| is 1-based.
  // |nIndex| is 0-based.
  bool ExportPage(const std::vector<uint32_t>& pageNums, int nIndex);
};

CPDF_PageExporter::CPDF_PageExporter(CPDF_Document* pDestPDFDoc,
                                     CPDF_Document* pSrcPDFDoc)
    : CPDF_PageOrganizer(pDestPDFDoc, pSrcPDFDoc) {}

CPDF_PageExporter::~CPDF_PageExporter() = default;

bool CPDF_PageExporter::ExportPage(const std::vector<uint32_t>& pageNums,
                                   int nIndex) {
  if (!PDFDocInit())
    return false;

  int curpage = nIndex;
  auto pObjNumberMap = pdfium::MakeUnique<ObjectNumberMap>();
  for (size_t i = 0; i < pageNums.size(); ++i) {
    CPDF_Dictionary* pDestPageDict = dest()->CreateNewPage(curpage);
    auto* pSrcPageDict = src()->GetPageDictionary(pageNums[i] - 1);
    if (!pSrcPageDict || !pDestPageDict)
      return false;

    // Clone the page dictionary
    CPDF_DictionaryLocker locker(pSrcPageDict);
    for (const auto& it : locker) {
      const ByteString& cbSrcKeyStr = it.first;
      if (cbSrcKeyStr == pdfium::page_object::kType ||
          cbSrcKeyStr == pdfium::page_object::kParent) {
        continue;
      }

      CPDF_Object* pObj = it.second.get();
      pDestPageDict->SetFor(cbSrcKeyStr, pObj->Clone());
    }

    // inheritable item
    // Even though some entries are required by the PDF spec, there exist
    // PDFs that omit them. Set some defaults in this case.
    // 1 MediaBox - required
    if (!CopyInheritable(pDestPageDict, pSrcPageDict,
                         pdfium::page_object::kMediaBox)) {
      // Search for "CropBox" in the source page dictionary.
      // If it does not exist, use the default letter size.
      const CPDF_Object* pInheritable = PageDictGetInheritableTag(
          pSrcPageDict, pdfium::page_object::kCropBox);
      if (pInheritable) {
        pDestPageDict->SetFor(pdfium::page_object::kMediaBox,
                              pInheritable->Clone());
      } else {
        // Make the default size letter size (8.5"x11")
        static const CFX_FloatRect kDefaultLetterRect(0, 0, 612, 792);
        pDestPageDict->SetRectFor(pdfium::page_object::kMediaBox,
                                  kDefaultLetterRect);
      }
    }

    // 2 Resources - required
    if (!CopyInheritable(pDestPageDict, pSrcPageDict,
                         pdfium::page_object::kResources)) {
      // Use a default empty resources if it does not exist.
      pDestPageDict->SetNewFor<CPDF_Dictionary>(
          pdfium::page_object::kResources);
    }

    // 3 CropBox - optional
    CopyInheritable(pDestPageDict, pSrcPageDict, pdfium::page_object::kCropBox);
    // 4 Rotate - optional
    CopyInheritable(pDestPageDict, pSrcPageDict, pdfium::page_object::kRotate);

    // Update the reference
    uint32_t dwOldPageObj = pSrcPageDict->GetObjNum();
    uint32_t dwNewPageObj = pDestPageDict->GetObjNum();
    (*pObjNumberMap)[dwOldPageObj] = dwNewPageObj;
    UpdateReference(pDestPageDict, pObjNumberMap.get());
    ++curpage;
  }

  return true;
}

// Copies pages from a source document into a destination document. Creates 1
// page in the destination document for every N source pages. This class is
// intended to be used once via ExportNPagesToOne() and then destroyed.
class CPDF_NPageToOneExporter final : public CPDF_PageOrganizer {
 public:
  CPDF_NPageToOneExporter(CPDF_Document* pDestPDFDoc,
                          CPDF_Document* pSrcPDFDoc);
  ~CPDF_NPageToOneExporter();

  // For the pages from the source document with |pageNums| as their page
  // numbers, insert them into the destination document, starting at page 0.
  // |pageNums| is 1-based.
  // |destPageSize| is the destination document page dimensions, measured in
  // PDF "user space" units.
  // |numPagesOnXAxis| and |numPagesOnXAxis| together defines how many source
  // pages fit on one destination page.
  bool ExportNPagesToOne(const std::vector<uint32_t>& pageNums,
                         const CFX_SizeF& destPageSize,
                         unsigned int numPagesOnXAxis,
                         unsigned int numPagesOnYAxis);

 private:
  // Map page object number to XObject object name.
  using PageXObjectMap = std::map<uint32_t, ByteString>;
  // Map XObject's object name to it's object number.
  using XObjectNameNumberMap = std::map<ByteString, uint32_t>;

  // Creates a xobject from the source page dictionary, and appends the
  // bsContent string with the xobject reference surrounded by the
  // transformation matrix.
  void AddSubPage(const CPDF_Dictionary* pSrcPageDict,
                  const NupPageSettings& settings,
                  ObjectNumberMap* pObjNumberMap,
                  PageXObjectMap* pPageXObjectMap,
                  XObjectNameNumberMap* pXObjNameNumberMap,
                  ByteString* bsContent);
  uint32_t MakeXObject(const CPDF_Dictionary* pSrcPageDict,
                       ObjectNumberMap* pObjNumberMap);

  void FinishPage(CPDF_Dictionary* pDestPageDict,
                  const ByteString& bsContent,
                  const XObjectNameNumberMap& xObjNameNumberMap);

  uint32_t m_xobjectNum = 0;
  XObjectNameNumberMap m_xobjs;
};

CPDF_NPageToOneExporter::CPDF_NPageToOneExporter(CPDF_Document* pDestPDFDoc,
                                                 CPDF_Document* pSrcPDFDoc)
    : CPDF_PageOrganizer(pDestPDFDoc, pSrcPDFDoc) {}

CPDF_NPageToOneExporter::~CPDF_NPageToOneExporter() = default;

bool CPDF_NPageToOneExporter::ExportNPagesToOne(
    const std::vector<uint32_t>& pageNums,
    const CFX_SizeF& destPageSize,
    unsigned int numPagesOnXAxis,
    unsigned int numPagesOnYAxis) {
  if (!PDFDocInit())
    return false;

  FX_SAFE_SIZE_T safe_numPagesPerSheet = numPagesOnXAxis;
  safe_numPagesPerSheet *= numPagesOnYAxis;
  if (!safe_numPagesPerSheet.IsValid())
    return false;

  size_t numPagesPerSheet = safe_numPagesPerSheet.ValueOrDie();

  // Mapping of source page object number and XObject object number.
  // Used to update refernece.
  ObjectNumberMap objectNumberMap;
  // Mapping of source page object number and XObject name of the entire doc.
  // If there are two pages that are identical and have the same object number,
  // we can reuse one created XObject.
  PageXObjectMap pageXObjectMap;
  NupState nupState(destPageSize, numPagesOnXAxis, numPagesOnYAxis);

  size_t curpage = 0;
  const CFX_FloatRect destPageRect(0, 0, destPageSize.width,
                                   destPageSize.height);
  for (size_t outerPage = 0; outerPage < pageNums.size();
       outerPage += numPagesPerSheet) {
    // Create a new page
    CPDF_Dictionary* pDestPageDict = dest()->CreateNewPage(curpage);
    if (!pDestPageDict)
      return false;

    pDestPageDict->SetRectFor(pdfium::page_object::kMediaBox, destPageRect);
    ByteString bsContent;
    size_t innerPageMax =
        std::min(outerPage + numPagesPerSheet, pageNums.size());
    // Mapping of XObject name and XObject object number of one page.
    XObjectNameNumberMap xObjNameNumberMap;
    for (size_t innerPage = outerPage; innerPage < innerPageMax; ++innerPage) {
      auto* pSrcPageDict = src()->GetPageDictionary(pageNums[innerPage] - 1);
      if (!pSrcPageDict)
        return false;

      auto srcPage = pdfium::MakeRetain<CPDF_Page>(src(), pSrcPageDict, true);
      NupPageSettings settings =
          nupState.CalculateNewPagePosition(srcPage->GetPageSize());
      AddSubPage(pSrcPageDict, settings, &objectNumberMap, &pageXObjectMap,
                 &xObjNameNumberMap, &bsContent);
    }

    // Finish up the current page.
    FinishPage(pDestPageDict, bsContent, xObjNameNumberMap);
    ++curpage;
  }

  return true;
}

void CPDF_NPageToOneExporter::AddSubPage(
    const CPDF_Dictionary* pSrcPageDict,
    const NupPageSettings& settings,
    ObjectNumberMap* pObjNumberMap,
    PageXObjectMap* pPageXObjectMap,
    XObjectNameNumberMap* pXObjNameNumberMap,
    ByteString* bsContent) {
  uint32_t dwPageObjnum = pSrcPageDict->GetObjNum();
  ByteString bsXObjectName;
  const auto it = pPageXObjectMap->find(dwPageObjnum);
  if (it != pPageXObjectMap->end()) {
    bsXObjectName = it->second;
  } else {
    ++m_xobjectNum;
    // TODO(Xlou): A better name schema to avoid possible object name collision.
    bsXObjectName = ByteString::Format("X%d", m_xobjectNum);
    m_xobjs[bsXObjectName] = MakeXObject(pSrcPageDict, pObjNumberMap);
    (*pPageXObjectMap)[dwPageObjnum] = bsXObjectName;
  }
  (*pXObjNameNumberMap)[bsXObjectName] = m_xobjs[bsXObjectName];

  CFX_Matrix matrix;
  matrix.Scale(settings.scale, settings.scale);
  matrix.Translate(settings.subPageStartPoint.x, settings.subPageStartPoint.y);

  std::ostringstream contentStream;
  contentStream << "q\n"
                << matrix.a << " " << matrix.b << " " << matrix.c << " "
                << matrix.d << " " << matrix.e << " " << matrix.f << " cm\n"
                << "/" << bsXObjectName << " Do Q\n";
  *bsContent += ByteString(contentStream);
}

uint32_t CPDF_NPageToOneExporter::MakeXObject(
    const CPDF_Dictionary* pSrcPageDict,
    ObjectNumberMap* pObjNumberMap) {
  ASSERT(pSrcPageDict);

  const CPDF_Object* pSrcContentObj = GetPageOrganizerPageContent(pSrcPageDict);
  CPDF_Stream* pNewXObject = dest()->NewIndirect<CPDF_Stream>(
      nullptr, 0, dest()->New<CPDF_Dictionary>());
  CPDF_Dictionary* pNewXObjectDict = pNewXObject->GetDict();
  const ByteString bsResourceString = "Resources";
  if (!CopyInheritable(pNewXObjectDict, pSrcPageDict, bsResourceString)) {
    // Use a default empty resources if it does not exist.
    pNewXObjectDict->SetNewFor<CPDF_Dictionary>(bsResourceString);
  }
  uint32_t dwSrcPageObj = pSrcPageDict->GetObjNum();
  uint32_t dwNewXobjectObj = pNewXObjectDict->GetObjNum();
  (*pObjNumberMap)[dwSrcPageObj] = dwNewXobjectObj;
  UpdateReference(pNewXObjectDict, pObjNumberMap);

  pNewXObjectDict->SetNewFor<CPDF_Name>("Type", "XObject");
  pNewXObjectDict->SetNewFor<CPDF_Name>("Subtype", "Form");
  pNewXObjectDict->SetNewFor<CPDF_Number>("FormType", 1);
  pNewXObjectDict->SetRectFor("BBox", GetCropBox(pSrcPageDict));
  // TODO(xlou): add matrix field to pNewXObjectDict.

  if (const CPDF_Array* pSrcContentArray = ToArray(pSrcContentObj)) {
    ByteString bsSrcContentStream;
    for (size_t i = 0; i < pSrcContentArray->size(); ++i) {
      const CPDF_Stream* pStream = pSrcContentArray->GetStreamAt(i);
      auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pStream);
      pAcc->LoadAllDataFiltered();
      bsSrcContentStream += ByteString(pAcc->GetData(), pAcc->GetSize());
      bsSrcContentStream += "\n";
    }
    pNewXObject->SetDataAndRemoveFilter(bsSrcContentStream.AsRawSpan());
  } else {
    ByteString bsStream;
    {
      const CPDF_Stream* pStream = pSrcContentObj->AsStream();
      auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pStream);
      pAcc->LoadAllDataFiltered();
      bsStream = ByteString(pAcc->GetData(), pAcc->GetSize());
    }
    pNewXObject->SetDataAndRemoveFilter(bsStream.AsRawSpan());
  }
  return pNewXObject->GetObjNum();
}

void CPDF_NPageToOneExporter::FinishPage(
    CPDF_Dictionary* pDestPageDict,
    const ByteString& bsContent,
    const XObjectNameNumberMap& xObjNameNumberMap) {
  ASSERT(pDestPageDict);

  CPDF_Dictionary* pRes =
      pDestPageDict->GetDictFor(pdfium::page_object::kResources);
  if (!pRes) {
    pRes = pDestPageDict->SetNewFor<CPDF_Dictionary>(
        pdfium::page_object::kResources);
  }

  CPDF_Dictionary* pPageXObject = pRes->GetDictFor("XObject");
  if (!pPageXObject)
    pPageXObject = pRes->SetNewFor<CPDF_Dictionary>("XObject");

  for (auto& it : xObjNameNumberMap)
    pPageXObject->SetNewFor<CPDF_Reference>(it.first, dest(), it.second);

  auto pDict = dest()->New<CPDF_Dictionary>();
  CPDF_Stream* pStream =
      dest()->NewIndirect<CPDF_Stream>(nullptr, 0, std::move(pDict));
  pStream->SetData(bsContent.AsRawSpan());
  pDestPageDict->SetFor(pdfium::page_object::kContents,
                        pStream->MakeReference(dest()));
}

}  // namespace

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDF_ImportPages(FPDF_DOCUMENT dest_doc,
                                                     FPDF_DOCUMENT src_doc,
                                                     FPDF_BYTESTRING pagerange,
                                                     int index) {
  CPDF_Document* pDestDoc = CPDFDocumentFromFPDFDocument(dest_doc);
  if (!dest_doc)
    return false;

  CPDF_Document* pSrcDoc = CPDFDocumentFromFPDFDocument(src_doc);
  if (!pSrcDoc)
    return false;

  std::vector<uint32_t> page_numbers = GetPageNumbers(*pSrcDoc, pagerange);
  if (page_numbers.empty())
    return false;

  CPDF_PageExporter exporter(pDestDoc, pSrcDoc);
  return exporter.ExportPage(page_numbers, index);
}

FPDF_EXPORT FPDF_DOCUMENT FPDF_CALLCONV
FPDF_ImportNPagesToOne(FPDF_DOCUMENT src_doc,
                       float output_width,
                       float output_height,
                       unsigned int num_pages_on_x_axis,
                       unsigned int num_pages_on_y_axis) {
  CPDF_Document* pSrcDoc = CPDFDocumentFromFPDFDocument(src_doc);
  if (!pSrcDoc)
    return nullptr;

  if (output_width <= 0 || output_height <= 0 || num_pages_on_x_axis <= 0 ||
      num_pages_on_y_axis <= 0) {
    return nullptr;
  }

  ScopedFPDFDocument output_doc(FPDF_CreateNewDocument());
  if (!output_doc)
    return nullptr;

  CPDF_Document* pDestDoc = CPDFDocumentFromFPDFDocument(output_doc.get());
  ASSERT(pDestDoc);

  std::vector<uint32_t> page_numbers = GetPageNumbers(*pSrcDoc, ByteString());
  if (page_numbers.empty())
    return nullptr;

  if (num_pages_on_x_axis == 1 && num_pages_on_y_axis == 1) {
    CPDF_PageExporter exporter(pDestDoc, pSrcDoc);
    if (!exporter.ExportPage(page_numbers, 0))
      return nullptr;
    return output_doc.release();
  }

  CPDF_NPageToOneExporter exporter(pDestDoc, pSrcDoc);
  if (!exporter.ExportNPagesToOne(page_numbers,
                                  CFX_SizeF(output_width, output_height),
                                  num_pages_on_x_axis, num_pages_on_y_axis)) {
    return nullptr;
  }
  return output_doc.release();
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDF_CopyViewerPreferences(FPDF_DOCUMENT dest_doc, FPDF_DOCUMENT src_doc) {
  CPDF_Document* pDstDoc = CPDFDocumentFromFPDFDocument(dest_doc);
  if (!pDstDoc)
    return false;

  CPDF_Document* pSrcDoc = CPDFDocumentFromFPDFDocument(src_doc);
  if (!pSrcDoc)
    return false;

  const CPDF_Dictionary* pSrcDict = pSrcDoc->GetRoot();
  pSrcDict = pSrcDict->GetDictFor("ViewerPreferences");
  if (!pSrcDict)
    return false;

  CPDF_Dictionary* pDstDict = pDstDoc->GetRoot();
  if (!pDstDict)
    return false;

  pDstDict->SetFor("ViewerPreferences", pSrcDict->CloneDirectObject());
  return true;
}
