// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_doc.h"

#include <memory>
#include <set>

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfdoc/cpdf_bookmark.h"
#include "core/fpdfdoc/cpdf_bookmarktree.h"
#include "core/fpdfdoc/cpdf_dest.h"
#include "core/fpdfdoc/cpdf_pagelabel.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

namespace {

CPDF_Bookmark FindBookmark(const CPDF_BookmarkTree& tree,
                           CPDF_Bookmark bookmark,
                           const WideString& title,
                           std::set<const CPDF_Dictionary*>* visited) {
  // Return if already checked to avoid circular calling.
  if (pdfium::ContainsKey(*visited, bookmark.GetDict()))
    return CPDF_Bookmark();
  visited->insert(bookmark.GetDict());

  if (bookmark.GetDict() &&
      bookmark.GetTitle().CompareNoCase(title.c_str()) == 0) {
    // First check this item.
    return bookmark;
  }

  // Go into children items.
  CPDF_Bookmark child = tree.GetFirstChild(bookmark);
  while (child.GetDict() && !pdfium::ContainsKey(*visited, child.GetDict())) {
    // Check this item and its children.
    CPDF_Bookmark found = FindBookmark(tree, child, title, visited);
    if (found.GetDict())
      return found;
    child = tree.GetNextSibling(child);
  }
  return CPDF_Bookmark();
}

CPDF_LinkList* GetLinkList(CPDF_Page* page) {
  CPDF_Document* pDoc = page->GetDocument();
  std::unique_ptr<CPDF_LinkList>* pHolder = pDoc->LinksContext();
  if (!pHolder->get())
    *pHolder = pdfium::MakeUnique<CPDF_LinkList>();
  return pHolder->get();
}

}  // namespace

FPDF_EXPORT FPDF_BOOKMARK FPDF_CALLCONV
FPDFBookmark_GetFirstChild(FPDF_DOCUMENT document, FPDF_BOOKMARK pDict) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;
  CPDF_BookmarkTree tree(pDoc);
  CPDF_Bookmark bookmark(CPDFDictionaryFromFPDFBookmark(pDict));
  return FPDFBookmarkFromCPDFDictionary(tree.GetFirstChild(bookmark).GetDict());
}

FPDF_EXPORT FPDF_BOOKMARK FPDF_CALLCONV
FPDFBookmark_GetNextSibling(FPDF_DOCUMENT document, FPDF_BOOKMARK pDict) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;

  if (!pDict)
    return nullptr;

  CPDF_BookmarkTree tree(pDoc);
  CPDF_Bookmark bookmark(CPDFDictionaryFromFPDFBookmark(pDict));
  return FPDFBookmarkFromCPDFDictionary(
      tree.GetNextSibling(bookmark).GetDict());
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDFBookmark_GetTitle(FPDF_BOOKMARK pDict, void* buffer, unsigned long buflen) {
  if (!pDict)
    return 0;
  CPDF_Bookmark bookmark(CPDFDictionaryFromFPDFBookmark(pDict));
  WideString title = bookmark.GetTitle();
  return Utf16EncodeMaybeCopyAndReturnLength(title, buffer, buflen);
}

FPDF_EXPORT FPDF_BOOKMARK FPDF_CALLCONV
FPDFBookmark_Find(FPDF_DOCUMENT document, FPDF_WIDESTRING title) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;

  WideString encodedTitle = WideStringFromFPDFWideString(title);
  if (encodedTitle.IsEmpty())
    return nullptr;

  CPDF_BookmarkTree tree(pDoc);
  std::set<const CPDF_Dictionary*> visited;
  return FPDFBookmarkFromCPDFDictionary(
      FindBookmark(tree, CPDF_Bookmark(), encodedTitle, &visited).GetDict());
}

FPDF_EXPORT FPDF_DEST FPDF_CALLCONV FPDFBookmark_GetDest(FPDF_DOCUMENT document,
                                                         FPDF_BOOKMARK pDict) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;

  if (!pDict)
    return nullptr;

  CPDF_Bookmark bookmark(CPDFDictionaryFromFPDFBookmark(pDict));
  CPDF_Dest dest = bookmark.GetDest(pDoc);
  if (dest.GetArray())
    return FPDFDestFromCPDFArray(dest.GetArray());
  // If this bookmark is not directly associated with a dest, we try to get
  // action
  CPDF_Action action = bookmark.GetAction();
  if (!action.GetDict())
    return nullptr;
  return FPDFDestFromCPDFArray(action.GetDest(pDoc).GetArray());
}

FPDF_EXPORT FPDF_ACTION FPDF_CALLCONV
FPDFBookmark_GetAction(FPDF_BOOKMARK pDict) {
  if (!pDict)
    return nullptr;

  CPDF_Bookmark bookmark(CPDFDictionaryFromFPDFBookmark(pDict));
  return FPDFActionFromCPDFDictionary(bookmark.GetAction().GetDict());
}

FPDF_EXPORT unsigned long FPDF_CALLCONV FPDFAction_GetType(FPDF_ACTION pDict) {
  if (!pDict)
    return PDFACTION_UNSUPPORTED;

  CPDF_Action action(CPDFDictionaryFromFPDFAction(pDict));
  CPDF_Action::ActionType type = action.GetType();
  switch (type) {
    case CPDF_Action::GoTo:
      return PDFACTION_GOTO;
    case CPDF_Action::GoToR:
      return PDFACTION_REMOTEGOTO;
    case CPDF_Action::URI:
      return PDFACTION_URI;
    case CPDF_Action::Launch:
      return PDFACTION_LAUNCH;
    default:
      return PDFACTION_UNSUPPORTED;
  }
}

FPDF_EXPORT FPDF_DEST FPDF_CALLCONV FPDFAction_GetDest(FPDF_DOCUMENT document,
                                                       FPDF_ACTION pDict) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;

  unsigned long type = FPDFAction_GetType(pDict);
  if (type != PDFACTION_GOTO && type != PDFACTION_REMOTEGOTO)
    return nullptr;

  CPDF_Action action(CPDFDictionaryFromFPDFAction(pDict));
  return FPDFDestFromCPDFArray(action.GetDest(pDoc).GetArray());
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDFAction_GetFilePath(FPDF_ACTION pDict, void* buffer, unsigned long buflen) {
  unsigned long type = FPDFAction_GetType(pDict);
  if (type != PDFACTION_REMOTEGOTO && type != PDFACTION_LAUNCH)
    return 0;

  CPDF_Action action(CPDFDictionaryFromFPDFAction(pDict));
  ByteString path = action.GetFilePath().ToUTF8();
  unsigned long len = path.GetLength() + 1;
  if (buffer && len <= buflen)
    memcpy(buffer, path.c_str(), len);
  return len;
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDFAction_GetURIPath(FPDF_DOCUMENT document,
                      FPDF_ACTION pDict,
                      void* buffer,
                      unsigned long buflen) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return 0;

  unsigned long type = FPDFAction_GetType(pDict);
  if (type != PDFACTION_URI)
    return 0;

  CPDF_Action action(CPDFDictionaryFromFPDFAction(pDict));
  ByteString path = action.GetURI(pDoc);
  unsigned long len = path.GetLength() + 1;
  if (buffer && len <= buflen)
    memcpy(buffer, path.c_str(), len);
  return len;
}

FPDF_EXPORT int FPDF_CALLCONV FPDFDest_GetDestPageIndex(FPDF_DOCUMENT document,
                                                        FPDF_DEST dest) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return -1;

  if (!dest)
    return -1;

  CPDF_Dest destination(CPDFArrayFromFPDFDest(dest));
  return destination.GetDestPageIndex(pDoc);
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDFDest_GetView(FPDF_DEST pDict,
                 unsigned long* pNumParams,
                 FS_FLOAT* pParams) {
  if (!pDict) {
    *pNumParams = 0;
    return 0;
  }

  CPDF_Dest destination(CPDFArrayFromFPDFDest(pDict));
  unsigned long nParams = destination.GetNumParams();
  ASSERT(nParams <= 4);
  *pNumParams = nParams;
  for (unsigned long i = 0; i < nParams; ++i)
    pParams[i] = destination.GetParam(i);
  return destination.GetZoomMode();
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFDest_GetLocationInPage(FPDF_DEST pDict,
                           FPDF_BOOL* hasXVal,
                           FPDF_BOOL* hasYVal,
                           FPDF_BOOL* hasZoomVal,
                           FS_FLOAT* x,
                           FS_FLOAT* y,
                           FS_FLOAT* zoom) {
  if (!pDict)
    return false;

  auto dest = pdfium::MakeUnique<CPDF_Dest>(CPDFArrayFromFPDFDest(pDict));

  // FPDF_BOOL is an int, GetXYZ expects bools.
  bool bHasX;
  bool bHasY;
  bool bHasZoom;
  if (!dest->GetXYZ(&bHasX, &bHasY, &bHasZoom, x, y, zoom))
    return false;

  *hasXVal = bHasX;
  *hasYVal = bHasY;
  *hasZoomVal = bHasZoom;
  return true;
}

FPDF_EXPORT FPDF_LINK FPDF_CALLCONV FPDFLink_GetLinkAtPoint(FPDF_PAGE page,
                                                            double x,
                                                            double y) {
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return nullptr;

  CPDF_LinkList* pLinkList = GetLinkList(pPage);
  if (!pLinkList)
    return nullptr;

  CPDF_Link link = pLinkList->GetLinkAtPoint(
      pPage, CFX_PointF(static_cast<float>(x), static_cast<float>(y)), nullptr);

  return FPDFLinkFromCPDFDictionary(link.GetDict());
}

FPDF_EXPORT int FPDF_CALLCONV FPDFLink_GetLinkZOrderAtPoint(FPDF_PAGE page,
                                                            double x,
                                                            double y) {
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return -1;

  CPDF_LinkList* pLinkList = GetLinkList(pPage);
  if (!pLinkList)
    return -1;

  int z_order = -1;
  pLinkList->GetLinkAtPoint(
      pPage, CFX_PointF(static_cast<float>(x), static_cast<float>(y)),
      &z_order);
  return z_order;
}

FPDF_EXPORT FPDF_DEST FPDF_CALLCONV FPDFLink_GetDest(FPDF_DOCUMENT document,
                                                     FPDF_LINK pDict) {
  if (!pDict)
    return nullptr;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;
  CPDF_Link link(CPDFDictionaryFromFPDFLink(pDict));
  FPDF_DEST dest = FPDFDestFromCPDFArray(link.GetDest(pDoc).GetArray());
  if (dest)
    return dest;
  // If this link is not directly associated with a dest, we try to get action
  CPDF_Action action = link.GetAction();
  if (!action.GetDict())
    return nullptr;
  return FPDFDestFromCPDFArray(action.GetDest(pDoc).GetArray());
}

FPDF_EXPORT FPDF_ACTION FPDF_CALLCONV FPDFLink_GetAction(FPDF_LINK pDict) {
  if (!pDict)
    return nullptr;

  CPDF_Link link(CPDFDictionaryFromFPDFLink(pDict));
  return FPDFActionFromCPDFDictionary(link.GetAction().GetDict());
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFLink_Enumerate(FPDF_PAGE page,
                                                       int* start_pos,
                                                       FPDF_LINK* link_annot) {
  if (!start_pos || !link_annot)
    return false;
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage || !pPage->GetDict())
    return false;
  CPDF_Array* pAnnots = pPage->GetDict()->GetArrayFor("Annots");
  if (!pAnnots)
    return false;
  for (size_t i = *start_pos; i < pAnnots->size(); i++) {
    CPDF_Dictionary* pDict = ToDictionary(pAnnots->GetDirectObjectAt(i));
    if (!pDict)
      continue;
    if (pDict->GetStringFor("Subtype") == "Link") {
      *start_pos = static_cast<int>(i + 1);
      *link_annot = FPDFLinkFromCPDFDictionary(pDict);
      return true;
    }
  }
  return false;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFLink_GetAnnotRect(FPDF_LINK link_annot,
                                                          FS_RECTF* rect) {
  if (!link_annot || !rect)
    return false;
  CPDF_Dictionary* pAnnotDict = CPDFDictionaryFromFPDFLink(link_annot);
  FSRECTFFromCFXFloatRect(pAnnotDict->GetRectFor("Rect"), rect);
  return true;
}

FPDF_EXPORT int FPDF_CALLCONV FPDFLink_CountQuadPoints(FPDF_LINK link_annot) {
  const CPDF_Array* pArray =
      GetQuadPointsArrayFromDictionary(CPDFDictionaryFromFPDFLink(link_annot));
  return pArray ? static_cast<int>(pArray->size() / 8) : 0;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFLink_GetQuadPoints(FPDF_LINK link_annot,
                       int quad_index,
                       FS_QUADPOINTSF* quad_points) {
  if (!quad_points || quad_index < 0)
    return false;

  CPDF_Dictionary* pLinkDict = CPDFDictionaryFromFPDFLink(link_annot);
  if (!pLinkDict)
    return false;

  const CPDF_Array* pArray = GetQuadPointsArrayFromDictionary(pLinkDict);
  if (!pArray)
    return false;

  return GetQuadPointsAtIndex(pArray, static_cast<size_t>(quad_index),
                              quad_points);
}

FPDF_EXPORT unsigned long FPDF_CALLCONV FPDF_GetMetaText(FPDF_DOCUMENT document,
                                                         FPDF_BYTESTRING tag,
                                                         void* buffer,
                                                         unsigned long buflen) {
  if (!tag)
    return 0;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return 0;

  const CPDF_Dictionary* pInfo = pDoc->GetInfo();
  if (!pInfo)
    return 0;
  WideString text = pInfo->GetUnicodeTextFor(tag);
  return Utf16EncodeMaybeCopyAndReturnLength(text, buffer, buflen);
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDF_GetPageLabel(FPDF_DOCUMENT document,
                  int page_index,
                  void* buffer,
                  unsigned long buflen) {
  if (page_index < 0)
    return 0;

  // CPDF_PageLabel can deal with NULL |document|.
  CPDF_PageLabel label(CPDFDocumentFromFPDFDocument(document));
  Optional<WideString> str = label.GetLabel(page_index);
  return str.has_value()
             ? Utf16EncodeMaybeCopyAndReturnLength(str.value(), buffer, buflen)
             : 0;
}
