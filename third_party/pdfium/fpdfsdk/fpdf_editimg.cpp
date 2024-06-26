// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_edit.h"

#include <utility>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/page/cpdf_image.h"
#include "core/fpdfapi/page/cpdf_imageobject.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfapi/render/cpdf_dibbase.h"
#include "fpdfsdk/cpdfsdk_customaccess.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "third_party/base/ptr_util.h"

namespace {

// These checks ensure the consistency of colorspace values across core/ and
// public/.
static_assert(PDFCS_DEVICEGRAY == FPDF_COLORSPACE_DEVICEGRAY,
              "PDFCS_DEVICEGRAY value mismatch");
static_assert(PDFCS_DEVICERGB == FPDF_COLORSPACE_DEVICERGB,
              "PDFCS_DEVICERGB value mismatch");
static_assert(PDFCS_DEVICECMYK == FPDF_COLORSPACE_DEVICECMYK,
              "PDFCS_DEVICECMYK value mismatch");
static_assert(PDFCS_CALGRAY == FPDF_COLORSPACE_CALGRAY,
              "PDFCS_CALGRAY value mismatch");
static_assert(PDFCS_CALRGB == FPDF_COLORSPACE_CALRGB,
              "PDFCS_CALRGB value mismatch");
static_assert(PDFCS_LAB == FPDF_COLORSPACE_LAB, "PDFCS_LAB value mismatch");
static_assert(PDFCS_ICCBASED == FPDF_COLORSPACE_ICCBASED,
              "PDFCS_ICCBASED value mismatch");
static_assert(PDFCS_SEPARATION == FPDF_COLORSPACE_SEPARATION,
              "PDFCS_SEPARATION value mismatch");
static_assert(PDFCS_DEVICEN == FPDF_COLORSPACE_DEVICEN,
              "PDFCS_DEVICEN value mismatch");
static_assert(PDFCS_INDEXED == FPDF_COLORSPACE_INDEXED,
              "PDFCS_INDEXED value mismatch");
static_assert(PDFCS_PATTERN == FPDF_COLORSPACE_PATTERN,
              "PDFCS_PATTERN value mismatch");

RetainPtr<IFX_SeekableReadStream> MakeSeekableReadStream(
    FPDF_FILEACCESS* pFileAccess) {
  return pdfium::MakeRetain<CPDFSDK_CustomAccess>(pFileAccess);
}

CPDF_ImageObject* CPDFImageObjectFromFPDFPageObject(
    FPDF_PAGEOBJECT image_object) {
  CPDF_PageObject* pPageObject = CPDFPageObjectFromFPDFPageObject(image_object);
  return pPageObject ? pPageObject->AsImage() : nullptr;
}

bool LoadJpegHelper(FPDF_PAGE* pages,
                    int count,
                    FPDF_PAGEOBJECT image_object,
                    FPDF_FILEACCESS* file_access,
                    bool inline_jpeg) {
  CPDF_ImageObject* pImgObj = CPDFImageObjectFromFPDFPageObject(image_object);
  if (!pImgObj)
    return false;

  if (!file_access)
    return false;

  if (pages) {
    for (int index = 0; index < count; index++) {
      CPDF_Page* pPage = CPDFPageFromFPDFPage(pages[index]);
      if (pPage)
        pImgObj->GetImage()->ResetCache(pPage);
    }
  }

  RetainPtr<IFX_SeekableReadStream> pFile = MakeSeekableReadStream(file_access);
  if (inline_jpeg)
    pImgObj->GetImage()->SetJpegImageInline(pFile);
  else
    pImgObj->GetImage()->SetJpegImage(pFile);
  pImgObj->SetDirty(true);
  return true;
}

}  // namespace

FPDF_EXPORT FPDF_PAGEOBJECT FPDF_CALLCONV
FPDFPageObj_NewImageObj(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;

  auto pImageObj = pdfium::MakeUnique<CPDF_ImageObject>();
  pImageObj->SetImage(pdfium::MakeRetain<CPDF_Image>(pDoc));

  // Caller takes ownership.
  return FPDFPageObjectFromCPDFPageObject(pImageObj.release());
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFImageObj_LoadJpegFile(FPDF_PAGE* pages,
                          int count,
                          FPDF_PAGEOBJECT image_object,
                          FPDF_FILEACCESS* file_access) {
  return LoadJpegHelper(pages, count, image_object, file_access, false);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFImageObj_LoadJpegFileInline(FPDF_PAGE* pages,
                                int count,
                                FPDF_PAGEOBJECT image_object,
                                FPDF_FILEACCESS* file_access) {
  return LoadJpegHelper(pages, count, image_object, file_access, true);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFImageObj_GetMatrix(FPDF_PAGEOBJECT image_object,
                       double* a,
                       double* b,
                       double* c,
                       double* d,
                       double* e,
                       double* f) {
  CPDF_ImageObject* pImgObj = CPDFImageObjectFromFPDFPageObject(image_object);
  if (!pImgObj || !a || !b || !c || !d || !e || !f)
    return false;

  std::tie(*a, *b, *c, *d, *e, *f) = pImgObj->matrix().AsTuple();
  return true;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFImageObj_SetMatrix(FPDF_PAGEOBJECT image_object,
                       double a,
                       double b,
                       double c,
                       double d,
                       double e,
                       double f) {
  CPDF_ImageObject* pImgObj = CPDFImageObjectFromFPDFPageObject(image_object);
  if (!pImgObj)
    return false;

  pImgObj->set_matrix(CFX_Matrix(static_cast<float>(a), static_cast<float>(b),
                                 static_cast<float>(c), static_cast<float>(d),
                                 static_cast<float>(e), static_cast<float>(f)));
  pImgObj->CalcBoundingBox();
  pImgObj->SetDirty(true);
  return true;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFImageObj_SetBitmap(FPDF_PAGE* pages,
                       int count,
                       FPDF_PAGEOBJECT image_object,
                       FPDF_BITMAP bitmap) {
  CPDF_ImageObject* pImgObj = CPDFImageObjectFromFPDFPageObject(image_object);
  if (!pImgObj)
    return false;

  if (!bitmap)
    return false;

  if (pages) {
    for (int index = 0; index < count; index++) {
      CPDF_Page* pPage = CPDFPageFromFPDFPage(pages[index]);
      if (pPage)
        pImgObj->GetImage()->ResetCache(pPage);
    }
  }

  RetainPtr<CFX_DIBitmap> holder(CFXDIBitmapFromFPDFBitmap(bitmap));
  pImgObj->GetImage()->SetImage(holder);
  pImgObj->CalcBoundingBox();
  pImgObj->SetDirty(true);
  return true;
}

FPDF_EXPORT FPDF_BITMAP FPDF_CALLCONV
FPDFImageObj_GetBitmap(FPDF_PAGEOBJECT image_object) {
  CPDF_ImageObject* pImgObj = CPDFImageObjectFromFPDFPageObject(image_object);
  if (!pImgObj)
    return nullptr;

  RetainPtr<CPDF_Image> pImg = pImgObj->GetImage();
  if (!pImg)
    return nullptr;

  RetainPtr<CFX_DIBBase> pSource = pImg->LoadDIBBase();
  if (!pSource)
    return nullptr;

  RetainPtr<CFX_DIBitmap> pBitmap;
  // If the source image has a representation of 1 bit per pixel, then convert
  // it to a grayscale bitmap having 1 byte per pixel, since bitmaps have no
  // concept of bits. Otherwise, convert the source image to a bitmap directly,
  // retaining its color representation.
  if (pSource->GetBPP() == 1)
    pBitmap = pSource->CloneConvert(FXDIB_8bppRgb);
  else
    pBitmap = pSource->Clone(nullptr);

  return FPDFBitmapFromCFXDIBitmap(pBitmap.Leak());
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDFImageObj_GetImageDataDecoded(FPDF_PAGEOBJECT image_object,
                                 void* buffer,
                                 unsigned long buflen) {
  CPDF_ImageObject* pImgObj = CPDFImageObjectFromFPDFPageObject(image_object);
  if (!pImgObj)
    return 0;

  RetainPtr<CPDF_Image> pImg = pImgObj->GetImage();
  if (!pImg)
    return 0;

  CPDF_Stream* pImgStream = pImg->GetStream();
  if (!pImgStream)
    return 0;

  return DecodeStreamMaybeCopyAndReturnLength(pImgStream, buffer, buflen);
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDFImageObj_GetImageDataRaw(FPDF_PAGEOBJECT image_object,
                             void* buffer,
                             unsigned long buflen) {
  CPDF_ImageObject* pImgObj = CPDFImageObjectFromFPDFPageObject(image_object);
  if (!pImgObj)
    return 0;

  RetainPtr<CPDF_Image> pImg = pImgObj->GetImage();
  if (!pImg)
    return 0;

  CPDF_Stream* pImgStream = pImg->GetStream();
  if (!pImgStream)
    return 0;

  auto streamAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pImgStream);
  streamAcc->LoadAllDataRaw();

  const uint32_t len = streamAcc->GetSize();
  if (!buffer || buflen < len)
    return len;

  memcpy(buffer, streamAcc->GetData(), len);
  return len;
}

FPDF_EXPORT int FPDF_CALLCONV
FPDFImageObj_GetImageFilterCount(FPDF_PAGEOBJECT image_object) {
  CPDF_ImageObject* pImgObj = CPDFImageObjectFromFPDFPageObject(image_object);
  if (!pImgObj)
    return 0;

  RetainPtr<CPDF_Image> pImg = pImgObj->GetImage();
  if (!pImg)
    return 0;

  CPDF_Dictionary* pDict = pImg->GetDict();
  CPDF_Object* pFilter = pDict ? pDict->GetDirectObjectFor("Filter") : nullptr;
  if (!pFilter)
    return 0;

  if (pFilter->IsArray())
    return pFilter->AsArray()->size();
  if (pFilter->IsName())
    return 1;

  return 0;
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDFImageObj_GetImageFilter(FPDF_PAGEOBJECT image_object,
                            int index,
                            void* buffer,
                            unsigned long buflen) {
  if (index < 0 || index >= FPDFImageObj_GetImageFilterCount(image_object))
    return 0;

  CPDF_PageObject* pObj = CPDFPageObjectFromFPDFPageObject(image_object);
  CPDF_Object* pFilter =
      pObj->AsImage()->GetImage()->GetDict()->GetDirectObjectFor("Filter");
  ByteString bsFilter;
  if (pFilter->IsName())
    bsFilter = pFilter->AsName()->GetString();
  else
    bsFilter = pFilter->AsArray()->GetStringAt(index);

  unsigned long len = bsFilter.GetLength() + 1;
  if (buffer && len <= buflen)
    memcpy(buffer, bsFilter.c_str(), len);
  return len;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFImageObj_GetImageMetadata(FPDF_PAGEOBJECT image_object,
                              FPDF_PAGE page,
                              FPDF_IMAGEOBJ_METADATA* metadata) {
  CPDF_ImageObject* pImgObj = CPDFImageObjectFromFPDFPageObject(image_object);
  if (!pImgObj || !metadata)
    return false;

  RetainPtr<CPDF_Image> pImg = pImgObj->GetImage();
  if (!pImg)
    return false;

  metadata->marked_content_id = pImgObj->m_ContentMarks.GetMarkedContentID();

  const int nPixelWidth = pImg->GetPixelWidth();
  const int nPixelHeight = pImg->GetPixelHeight();
  metadata->width = nPixelWidth;
  metadata->height = nPixelHeight;

  const float nWidth = pImgObj->GetRect().Width();
  const float nHeight = pImgObj->GetRect().Height();
  constexpr int nPointsPerInch = 72;
  if (nWidth != 0 && nHeight != 0) {
    metadata->horizontal_dpi = nPixelWidth / nWidth * nPointsPerInch;
    metadata->vertical_dpi = nPixelHeight / nHeight * nPointsPerInch;
  }

  metadata->bits_per_pixel = 0;
  metadata->colorspace = FPDF_COLORSPACE_UNKNOWN;

  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage || !pPage->GetDocument() || !pImg->GetStream())
    return true;

  auto pSource = pdfium::MakeRetain<CPDF_DIBBase>();
  CPDF_DIBBase::LoadState ret = pSource->StartLoadDIBBase(
      pPage->GetDocument(), pImg->GetStream(), false, nullptr,
      pPage->m_pPageResources.Get(), false, 0, false);
  if (ret == CPDF_DIBBase::LoadState::kFail)
    return true;

  metadata->bits_per_pixel = pSource->GetBPP();
  if (pSource->GetColorSpace())
    metadata->colorspace = pSource->GetColorSpace()->GetFamily();

  return true;
}
