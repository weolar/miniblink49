// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_text.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/page/cpdf_textobject.h"
#include "core/fpdfdoc/cpdf_viewerpreferences.h"
#include "core/fpdftext/cpdf_linkextract.h"
#include "core/fpdftext/cpdf_textpage.h"
#include "core/fpdftext/cpdf_textpagefind.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "third_party/base/numerics/safe_conversions.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_page.h"
#endif  // PDF_ENABLE_XFA

#ifdef _WIN32
#include <tchar.h>
#endif

namespace {

constexpr size_t kBytesPerCharacter = sizeof(unsigned short);

}  // namespace

FPDF_EXPORT FPDF_TEXTPAGE FPDF_CALLCONV FPDFText_LoadPage(FPDF_PAGE page) {
  CPDF_Page* pPDFPage = CPDFPageFromFPDFPage(page);
  if (!pPDFPage)
    return nullptr;

  CPDF_ViewerPreferences viewRef(pPDFPage->GetDocument());
  CPDF_TextPage* textpage = new CPDF_TextPage(
      pPDFPage, viewRef.IsDirectionR2L() ? FPDFText_Direction::Right
                                         : FPDFText_Direction::Left);
  textpage->ParseTextPage();
  return FPDFTextPageFromCPDFTextPage(textpage);
}

FPDF_EXPORT void FPDF_CALLCONV FPDFText_ClosePage(FPDF_TEXTPAGE text_page) {
  delete CPDFTextPageFromFPDFTextPage(text_page);
}

FPDF_EXPORT int FPDF_CALLCONV FPDFText_CountChars(FPDF_TEXTPAGE text_page) {
  if (!text_page)
    return -1;

  CPDF_TextPage* textpage = CPDFTextPageFromFPDFTextPage(text_page);
  return textpage->CountChars();
}

FPDF_EXPORT unsigned int FPDF_CALLCONV
FPDFText_GetUnicode(FPDF_TEXTPAGE text_page, int index) {
  if (!text_page)
    return 0;

  CPDF_TextPage* textpage = CPDFTextPageFromFPDFTextPage(text_page);
  if (index < 0 || index >= textpage->CountChars())
    return 0;

  FPDF_CHAR_INFO charinfo;
  textpage->GetCharInfo(index, &charinfo);
  return charinfo.m_Unicode;
}

FPDF_EXPORT double FPDF_CALLCONV FPDFText_GetFontSize(FPDF_TEXTPAGE text_page,
                                                      int index) {
  if (!text_page)
    return 0;
  CPDF_TextPage* textpage = CPDFTextPageFromFPDFTextPage(text_page);

  if (index < 0 || index >= textpage->CountChars())
    return 0;

  FPDF_CHAR_INFO charinfo;
  textpage->GetCharInfo(index, &charinfo);
  return charinfo.m_FontSize;
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDFText_GetFontInfo(FPDF_TEXTPAGE text_page,
                     int index,
                     void* buffer,
                     unsigned long buflen,
                     int* flags) {
  if (!text_page)
    return 0;
  CPDF_TextPage* pTextObj = CPDFTextPageFromFPDFTextPage(text_page);

  if (index < 0 || index >= pTextObj->CountChars())
    return 0;

  FPDF_CHAR_INFO charinfo;
  pTextObj->GetCharInfo(index, &charinfo);
  if (!charinfo.m_pTextObj)
    return 0;

  CPDF_Font* font = charinfo.m_pTextObj->GetFont();
  if (!font)
    return 0;

  if (flags)
    *flags = font->GetFontFlags();
  ByteString basefont = font->GetBaseFont();
  unsigned long length = basefont.GetLength() + 1;
  if (buffer && buflen >= length)
    memcpy(buffer, basefont.c_str(), length);
  return length;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFText_GetCharBox(FPDF_TEXTPAGE text_page,
                                                        int index,
                                                        double* left,
                                                        double* right,
                                                        double* bottom,
                                                        double* top) {
  if (!text_page || index < 0)
    return false;

  CPDF_TextPage* textpage = CPDFTextPageFromFPDFTextPage(text_page);
  if (index >= textpage->CountChars())
    return false;

  FPDF_CHAR_INFO charinfo;
  textpage->GetCharInfo(index, &charinfo);
  *left = charinfo.m_CharBox.left;
  *right = charinfo.m_CharBox.right;
  *bottom = charinfo.m_CharBox.bottom;
  *top = charinfo.m_CharBox.top;
  return true;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFText_GetCharOrigin(FPDF_TEXTPAGE text_page,
                       int index,
                       double* x,
                       double* y) {
  if (!text_page)
    return false;
  CPDF_TextPage* textpage = CPDFTextPageFromFPDFTextPage(text_page);

  if (index < 0 || index >= textpage->CountChars())
    return false;
  FPDF_CHAR_INFO charinfo;
  textpage->GetCharInfo(index, &charinfo);
  *x = charinfo.m_Origin.x;
  *y = charinfo.m_Origin.y;
  return true;
}

// select
FPDF_EXPORT int FPDF_CALLCONV
FPDFText_GetCharIndexAtPos(FPDF_TEXTPAGE text_page,
                           double x,
                           double y,
                           double xTolerance,
                           double yTolerance) {
  if (!text_page)
    return -3;

  CPDF_TextPage* textpage = CPDFTextPageFromFPDFTextPage(text_page);
  return textpage->GetIndexAtPos(
      CFX_PointF(static_cast<float>(x), static_cast<float>(y)),
      CFX_SizeF(static_cast<float>(xTolerance),
                static_cast<float>(yTolerance)));
}

FPDF_EXPORT int FPDF_CALLCONV FPDFText_GetText(FPDF_TEXTPAGE page,
                                               int char_start,
                                               int char_count,
                                               unsigned short* result) {
  if (!page || char_start < 0 || char_count < 0 || !result)
    return 0;

  CPDF_TextPage* textpage = CPDFTextPageFromFPDFTextPage(page);
  int char_available = textpage->CountChars() - char_start;
  if (char_available <= 0)
    return 0;

  char_count = std::min(char_count, char_available);
  if (char_count == 0) {
    // Writing out "", which has a character count of 1 due to the NUL.
    *result = '\0';
    return 1;
  }

  WideString str = textpage->GetPageText(char_start, char_count);

  if (str.GetLength() > static_cast<size_t>(char_count))
    str = str.Left(static_cast<size_t>(char_count));

  // UFT16LE_Encode doesn't handle surrogate pairs properly, so it is expected
  // the number of items to stay the same.
  ByteString byte_str = str.ToUTF16LE();
  size_t byte_str_len = byte_str.GetLength();
  int ret_count = byte_str_len / kBytesPerCharacter;

  ASSERT(ret_count <= char_count + 1);  // +1 to account for the NUL terminator.
  memcpy(result, byte_str.c_str(), byte_str_len);
  return ret_count;
}

FPDF_EXPORT int FPDF_CALLCONV FPDFText_CountRects(FPDF_TEXTPAGE text_page,
                                                  int start,
                                                  int count) {
  if (!text_page)
    return 0;

  CPDF_TextPage* textpage = CPDFTextPageFromFPDFTextPage(text_page);
  return textpage->CountRects(start, count);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFText_GetRect(FPDF_TEXTPAGE text_page,
                                                     int rect_index,
                                                     double* left,
                                                     double* top,
                                                     double* right,
                                                     double* bottom) {
  if (!text_page)
    return false;

  CPDF_TextPage* textpage = CPDFTextPageFromFPDFTextPage(text_page);
  CFX_FloatRect rect;
  bool result = textpage->GetRect(rect_index, &rect);

  *left = rect.left;
  *top = rect.top;
  *right = rect.right;
  *bottom = rect.bottom;
  return result;
}

FPDF_EXPORT int FPDF_CALLCONV FPDFText_GetBoundedText(FPDF_TEXTPAGE text_page,
                                                      double left,
                                                      double top,
                                                      double right,
                                                      double bottom,
                                                      unsigned short* buffer,
                                                      int buflen) {
  if (!text_page)
    return 0;

  CPDF_TextPage* textpage = CPDFTextPageFromFPDFTextPage(text_page);
  CFX_FloatRect rect((float)left, (float)bottom, (float)right, (float)top);
  WideString str = textpage->GetTextByRect(rect);

  if (buflen <= 0 || !buffer)
    return str.GetLength();

  ByteString cbUTF16Str = str.ToUTF16LE();
  int len = cbUTF16Str.GetLength() / sizeof(unsigned short);
  int size = buflen > len ? len : buflen;
  memcpy(buffer, cbUTF16Str.c_str(), size * sizeof(unsigned short));
  cbUTF16Str.ReleaseBuffer(size * sizeof(unsigned short));

  return size;
}

// Search
// -1 for end
FPDF_EXPORT FPDF_SCHHANDLE FPDF_CALLCONV
FPDFText_FindStart(FPDF_TEXTPAGE text_page,
                   FPDF_WIDESTRING findwhat,
                   unsigned long flags,
                   int start_index) {
  if (!text_page)
    return nullptr;

  CPDF_TextPageFind* textpageFind =
      new CPDF_TextPageFind(CPDFTextPageFromFPDFTextPage(text_page));
  textpageFind->FindFirst(
      WideStringFromFPDFWideString(findwhat), flags,
      start_index >= 0 ? Optional<size_t>(start_index) : Optional<size_t>());
  return FPDFSchHandleFromCPDFTextPageFind(textpageFind);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFText_FindNext(FPDF_SCHHANDLE handle) {
  if (!handle)
    return false;

  CPDF_TextPageFind* textpageFind = CPDFTextPageFindFromFPDFSchHandle(handle);
  return textpageFind->FindNext();
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFText_FindPrev(FPDF_SCHHANDLE handle) {
  if (!handle)
    return false;

  CPDF_TextPageFind* textpageFind = CPDFTextPageFindFromFPDFSchHandle(handle);
  return textpageFind->FindPrev();
}

FPDF_EXPORT int FPDF_CALLCONV
FPDFText_GetSchResultIndex(FPDF_SCHHANDLE handle) {
  if (!handle)
    return 0;

  CPDF_TextPageFind* textpageFind = CPDFTextPageFindFromFPDFSchHandle(handle);
  return textpageFind->GetCurOrder();
}

FPDF_EXPORT int FPDF_CALLCONV FPDFText_GetSchCount(FPDF_SCHHANDLE handle) {
  if (!handle)
    return 0;

  CPDF_TextPageFind* textpageFind = CPDFTextPageFindFromFPDFSchHandle(handle);
  return textpageFind->GetMatchedCount();
}

FPDF_EXPORT void FPDF_CALLCONV FPDFText_FindClose(FPDF_SCHHANDLE handle) {
  if (!handle)
    return;

  // Take ownership back from caller and destroy.
  std::unique_ptr<CPDF_TextPageFind> textpageFind(
      CPDFTextPageFindFromFPDFSchHandle(handle));
}

// web link
FPDF_EXPORT FPDF_PAGELINK FPDF_CALLCONV
FPDFLink_LoadWebLinks(FPDF_TEXTPAGE text_page) {
  if (!text_page)
    return nullptr;

  CPDF_TextPage* pPage = CPDFTextPageFromFPDFTextPage(text_page);
  auto pageLink = pdfium::MakeUnique<CPDF_LinkExtract>(pPage);
  pageLink->ExtractLinks();

  // Caller takes ownership.
  return FPDFPageLinkFromCPDFLinkExtract(pageLink.release());
}

FPDF_EXPORT int FPDF_CALLCONV FPDFLink_CountWebLinks(FPDF_PAGELINK link_page) {
  if (!link_page)
    return 0;

  CPDF_LinkExtract* pageLink = CPDFLinkExtractFromFPDFPageLink(link_page);
  return pdfium::base::checked_cast<int>(pageLink->CountLinks());
}

FPDF_EXPORT int FPDF_CALLCONV FPDFLink_GetURL(FPDF_PAGELINK link_page,
                                              int link_index,
                                              unsigned short* buffer,
                                              int buflen) {
  WideString wsUrl(L"");
  if (link_page && link_index >= 0) {
    CPDF_LinkExtract* pageLink = CPDFLinkExtractFromFPDFPageLink(link_page);
    wsUrl = pageLink->GetURL(link_index);
  }
  ByteString cbUTF16URL = wsUrl.ToUTF16LE();
  int required = cbUTF16URL.GetLength() / sizeof(unsigned short);
  if (!buffer || buflen <= 0)
    return required;

  int size = std::min(required, buflen);
  if (size > 0) {
    int buf_size = size * sizeof(unsigned short);
    memcpy(buffer, cbUTF16URL.c_str(), buf_size);
  }
  return size;
}

FPDF_EXPORT int FPDF_CALLCONV FPDFLink_CountRects(FPDF_PAGELINK link_page,
                                                  int link_index) {
  if (!link_page || link_index < 0)
    return 0;

  CPDF_LinkExtract* pageLink = CPDFLinkExtractFromFPDFPageLink(link_page);
  return pdfium::CollectionSize<int>(pageLink->GetRects(link_index));
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFLink_GetRect(FPDF_PAGELINK link_page,
                                                     int link_index,
                                                     int rect_index,
                                                     double* left,
                                                     double* top,
                                                     double* right,
                                                     double* bottom) {
  if (!link_page || link_index < 0 || rect_index < 0)
    return false;

  CPDF_LinkExtract* pageLink = CPDFLinkExtractFromFPDFPageLink(link_page);
  std::vector<CFX_FloatRect> rectArray = pageLink->GetRects(link_index);
  if (rect_index >= pdfium::CollectionSize<int>(rectArray))
    return false;

  *left = rectArray[rect_index].left;
  *right = rectArray[rect_index].right;
  *top = rectArray[rect_index].top;
  *bottom = rectArray[rect_index].bottom;
  return true;
}

FPDF_EXPORT void FPDF_CALLCONV FPDFLink_CloseWebLinks(FPDF_PAGELINK link_page) {
  delete CPDFLinkExtractFromFPDFPageLink(link_page);
}
