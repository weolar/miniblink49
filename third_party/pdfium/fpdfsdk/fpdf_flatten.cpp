// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_flatten.h"

#include <algorithm>
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
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfdoc/cpdf_annot.h"
#include "fpdfsdk/cpdfsdk_helpers.h"

enum FPDF_TYPE { MAX, MIN };
enum FPDF_VALUE { TOP, LEFT, RIGHT, BOTTOM };

namespace {

bool IsValidRect(const CFX_FloatRect& rect, const CFX_FloatRect& rcPage) {
  constexpr float kMinSize = 0.000001f;
  if (rect.IsEmpty() || rect.Width() < kMinSize || rect.Height() < kMinSize)
    return false;

  if (rcPage.IsEmpty())
    return true;

  constexpr float kMinBorderSize = 10.000001f;
  return rect.left - rcPage.left >= -kMinBorderSize &&
         rect.right - rcPage.right <= kMinBorderSize &&
         rect.top - rcPage.top <= kMinBorderSize &&
         rect.bottom - rcPage.bottom >= -kMinBorderSize;
}

void GetContentsRect(CPDF_Document* pDoc,
                     CPDF_Dictionary* pDict,
                     std::vector<CFX_FloatRect>* pRectArray) {
  auto pPDFPage = pdfium::MakeRetain<CPDF_Page>(pDoc, pDict, false);
  pPDFPage->ParseContent();

  for (const auto& pPageObject : *pPDFPage->GetPageObjectList()) {
    const CFX_FloatRect& rc = pPageObject->GetRect();
    if (IsValidRect(rc, pDict->GetRectFor(pdfium::page_object::kMediaBox)))
      pRectArray->push_back(rc);
  }
}

void ParserStream(CPDF_Dictionary* pPageDic,
                  CPDF_Dictionary* pStream,
                  std::vector<CFX_FloatRect>* pRectArray,
                  std::vector<CPDF_Dictionary*>* pObjectArray) {
  if (!pStream)
    return;
  CFX_FloatRect rect;
  if (pStream->KeyExist("Rect"))
    rect = pStream->GetRectFor("Rect");
  else if (pStream->KeyExist("BBox"))
    rect = pStream->GetRectFor("BBox");

  if (IsValidRect(rect, pPageDic->GetRectFor(pdfium::page_object::kMediaBox)))
    pRectArray->push_back(rect);

  pObjectArray->push_back(pStream);
}

int ParserAnnots(CPDF_Document* pSourceDoc,
                 CPDF_Dictionary* pPageDic,
                 std::vector<CFX_FloatRect>* pRectArray,
                 std::vector<CPDF_Dictionary*>* pObjectArray,
                 int nUsage) {
  if (!pSourceDoc || !pPageDic)
    return FLATTEN_FAIL;

  GetContentsRect(pSourceDoc, pPageDic, pRectArray);
  CPDF_Array* pAnnots = pPageDic->GetArrayFor("Annots");
  if (!pAnnots)
    return FLATTEN_NOTHINGTODO;

  CPDF_ArrayLocker locker(pAnnots);
  for (const auto& pAnnot : locker) {
    CPDF_Dictionary* pAnnotDic = ToDictionary(pAnnot->GetDirect());
    if (!pAnnotDic)
      continue;

    ByteString sSubtype = pAnnotDic->GetStringFor("Subtype");
    if (sSubtype == "Popup")
      continue;

    int nAnnotFlag = pAnnotDic->GetIntegerFor("F");
    if (nAnnotFlag & ANNOTFLAG_HIDDEN)
      continue;

    bool bParseStream;
    if (nUsage == FLAT_NORMALDISPLAY)
      bParseStream = !(nAnnotFlag & ANNOTFLAG_INVISIBLE);
    else
      bParseStream = !!(nAnnotFlag & ANNOTFLAG_PRINT);
    if (bParseStream)
      ParserStream(pPageDic, pAnnotDic, pRectArray, pObjectArray);
  }
  return FLATTEN_SUCCESS;
}

float GetMinMaxValue(const std::vector<CFX_FloatRect>& array,
                     FPDF_TYPE type,
                     FPDF_VALUE value) {
  if (array.empty())
    return 0.0f;

  size_t nRects = array.size();
  std::vector<float> pArray(nRects);
  switch (value) {
    case LEFT:
      for (size_t i = 0; i < nRects; i++)
        pArray[i] = array[i].left;
      break;
    case TOP:
      for (size_t i = 0; i < nRects; i++)
        pArray[i] = array[i].top;
      break;
    case RIGHT:
      for (size_t i = 0; i < nRects; i++)
        pArray[i] = array[i].right;
      break;
    case BOTTOM:
      for (size_t i = 0; i < nRects; i++)
        pArray[i] = array[i].bottom;
      break;
    default:
      NOTREACHED();
      return 0.0f;
  }

  float fRet = pArray[0];
  if (type == MAX) {
    for (size_t i = 1; i < nRects; i++)
      fRet = std::max(fRet, pArray[i]);
  } else {
    for (size_t i = 1; i < nRects; i++)
      fRet = std::min(fRet, pArray[i]);
  }
  return fRet;
}

CFX_FloatRect CalculateRect(std::vector<CFX_FloatRect>* pRectArray) {
  CFX_FloatRect rcRet;

  rcRet.left = GetMinMaxValue(*pRectArray, MIN, LEFT);
  rcRet.top = GetMinMaxValue(*pRectArray, MAX, TOP);
  rcRet.right = GetMinMaxValue(*pRectArray, MAX, RIGHT);
  rcRet.bottom = GetMinMaxValue(*pRectArray, MIN, BOTTOM);

  return rcRet;
}

ByteString GenerateFlattenedContent(const ByteString& key) {
  return "q 1 0 0 1 0 0 cm /" + key + " Do Q";
}

CPDF_Object* NewIndirectContentsStream(CPDF_Document* pDocument,
                                       const ByteString& contents) {
  CPDF_Stream* pNewContents = pDocument->NewIndirect<CPDF_Stream>(
      nullptr, 0, pDocument->New<CPDF_Dictionary>());
  pNewContents->SetData(contents.AsRawSpan());
  return pNewContents;
}

void SetPageContents(const ByteString& key,
                     CPDF_Dictionary* pPage,
                     CPDF_Document* pDocument) {
  CPDF_Array* pContentsArray =
      pPage->GetArrayFor(pdfium::page_object::kContents);
  CPDF_Stream* pContentsStream =
      pPage->GetStreamFor(pdfium::page_object::kContents);
  if (!pContentsStream && !pContentsArray) {
    if (!key.IsEmpty()) {
      pPage->SetFor(
          pdfium::page_object::kContents,
          NewIndirectContentsStream(pDocument, GenerateFlattenedContent(key))
              ->MakeReference(pDocument));
    }
    return;
  }

  pPage->ConvertToIndirectObjectFor(pdfium::page_object::kContents, pDocument);
  if (pContentsArray) {
    pContentsArray->InsertAt(
        0, NewIndirectContentsStream(pDocument, "q")->MakeReference(pDocument));
    pContentsArray->Add(
        NewIndirectContentsStream(pDocument, "Q")->MakeReference(pDocument));
  } else {
    ByteString sStream = "q\n";
    {
      auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pContentsStream);
      pAcc->LoadAllDataFiltered();
      sStream += ByteString(pAcc->GetSpan());
      sStream += "\nQ";
    }
    pContentsStream->SetDataAndRemoveFilter(sStream.AsRawSpan());

    pContentsArray = pDocument->NewIndirect<CPDF_Array>();
    pContentsArray->Add(pContentsStream->MakeReference(pDocument));
    pPage->SetFor(pdfium::page_object::kContents,
                  pContentsArray->MakeReference(pDocument));
  }
  if (!key.IsEmpty()) {
    pContentsArray->Add(
        NewIndirectContentsStream(pDocument, GenerateFlattenedContent(key))
            ->MakeReference(pDocument));
  }
}

CFX_Matrix GetMatrix(const CFX_FloatRect& rcAnnot,
                     const CFX_FloatRect& rcStream,
                     const CFX_Matrix& matrix) {
  if (rcStream.IsEmpty())
    return CFX_Matrix();

  CFX_FloatRect rcTransformed = matrix.TransformRect(rcStream);
  rcTransformed.Normalize();

  float a = rcAnnot.Width() / rcTransformed.Width();
  float d = rcAnnot.Height() / rcTransformed.Height();

  float e = rcAnnot.left - rcTransformed.left * a;
  float f = rcAnnot.bottom - rcTransformed.bottom * d;
  return CFX_Matrix(a, 0.0f, 0.0f, d, e, f);
}

}  // namespace

FPDF_EXPORT int FPDF_CALLCONV FPDFPage_Flatten(FPDF_PAGE page, int nFlag) {
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!page)
    return FLATTEN_FAIL;

  CPDF_Document* pDocument = pPage->GetDocument();
  CPDF_Dictionary* pPageDict = pPage->GetDict();
  if (!pDocument || !pPageDict)
    return FLATTEN_FAIL;

  std::vector<CPDF_Dictionary*> ObjectArray;
  std::vector<CFX_FloatRect> RectArray;
  int iRet =
      ParserAnnots(pDocument, pPageDict, &RectArray, &ObjectArray, nFlag);
  if (iRet == FLATTEN_NOTHINGTODO || iRet == FLATTEN_FAIL)
    return iRet;

  CFX_FloatRect rcMerger = CalculateRect(&RectArray);
  CFX_FloatRect rcOriginalMB =
      pPageDict->GetRectFor(pdfium::page_object::kMediaBox);
  if (pPageDict->KeyExist(pdfium::page_object::kCropBox))
    rcOriginalMB = pPageDict->GetRectFor(pdfium::page_object::kCropBox);

  if (rcOriginalMB.IsEmpty())
    rcOriginalMB = CFX_FloatRect(0.0f, 0.0f, 612.0f, 792.0f);

  CFX_FloatRect rcOriginalCB;
  if (pPageDict->KeyExist(pdfium::page_object::kCropBox))
    rcOriginalCB = pPageDict->GetRectFor(pdfium::page_object::kCropBox);
  if (rcOriginalCB.IsEmpty())
    rcOriginalCB = rcOriginalMB;

  rcMerger.left = std::max(rcMerger.left, rcOriginalMB.left);
  rcMerger.right = std::min(rcMerger.right, rcOriginalMB.right);
  rcMerger.bottom = std::max(rcMerger.bottom, rcOriginalMB.bottom);
  rcMerger.top = std::min(rcMerger.top, rcOriginalMB.top);

  pPageDict->SetRectFor(pdfium::page_object::kMediaBox, rcOriginalMB);
  pPageDict->SetRectFor(pdfium::page_object::kCropBox, rcOriginalCB);

  CPDF_Dictionary* pRes =
      pPageDict->GetDictFor(pdfium::page_object::kResources);
  if (!pRes) {
    pRes =
        pPageDict->SetNewFor<CPDF_Dictionary>(pdfium::page_object::kResources);
  }

  CPDF_Stream* pNewXObject = pDocument->NewIndirect<CPDF_Stream>(
      nullptr, 0, pDocument->New<CPDF_Dictionary>());

  CPDF_Dictionary* pPageXObject = pRes->GetDictFor("XObject");
  if (!pPageXObject)
    pPageXObject = pRes->SetNewFor<CPDF_Dictionary>("XObject");

  ByteString key;
  if (!ObjectArray.empty()) {
    int i = 0;
    while (i < INT_MAX) {
      ByteString sKey = ByteString::Format("FFT%d", i);
      if (!pPageXObject->KeyExist(sKey)) {
        key = std::move(sKey);
        break;
      }
      ++i;
    }
  }

  SetPageContents(key, pPageDict, pDocument);

  CPDF_Dictionary* pNewXORes = nullptr;
  if (!key.IsEmpty()) {
    pPageXObject->SetFor(key, pNewXObject->MakeReference(pDocument));
    CPDF_Dictionary* pNewOXbjectDic = pNewXObject->GetDict();
    pNewXORes = pNewOXbjectDic->SetNewFor<CPDF_Dictionary>("Resources");
    pNewOXbjectDic->SetNewFor<CPDF_Name>("Type", "XObject");
    pNewOXbjectDic->SetNewFor<CPDF_Name>("Subtype", "Form");
    pNewOXbjectDic->SetNewFor<CPDF_Number>("FormType", 1);
    pNewOXbjectDic->SetRectFor("BBox", rcOriginalCB);
  }

  for (size_t i = 0; i < ObjectArray.size(); ++i) {
    CPDF_Dictionary* pAnnotDic = ObjectArray[i];
    if (!pAnnotDic)
      continue;

    CFX_FloatRect rcAnnot = pAnnotDic->GetRectFor("Rect");
    rcAnnot.Normalize();

    ByteString sAnnotState = pAnnotDic->GetStringFor("AS");
    CPDF_Dictionary* pAnnotAP = pAnnotDic->GetDictFor("AP");
    if (!pAnnotAP)
      continue;

    CPDF_Stream* pAPStream = pAnnotAP->GetStreamFor("N");
    if (!pAPStream) {
      CPDF_Dictionary* pAPDic = pAnnotAP->GetDictFor("N");
      if (!pAPDic)
        continue;

      if (!sAnnotState.IsEmpty()) {
        pAPStream = pAPDic->GetStreamFor(sAnnotState);
      } else {
        if (pAPDic->size() > 0) {
          CPDF_DictionaryLocker locker(pAPDic);
          CPDF_Object* pFirstObj = locker.begin()->second.get();
          if (pFirstObj) {
            if (pFirstObj->IsReference())
              pFirstObj = pFirstObj->GetDirect();
            if (!pFirstObj->IsStream())
              continue;
            pAPStream = pFirstObj->AsStream();
          }
        }
      }
    }
    if (!pAPStream)
      continue;

    CPDF_Dictionary* pAPDic = pAPStream->GetDict();
    CFX_FloatRect rcStream;
    if (pAPDic->KeyExist("Rect"))
      rcStream = pAPDic->GetRectFor("Rect");
    else if (pAPDic->KeyExist("BBox"))
      rcStream = pAPDic->GetRectFor("BBox");
    rcStream.Normalize();

    if (rcStream.IsEmpty())
      continue;

    CPDF_Object* pObj = pAPStream;
    if (pObj->IsInline()) {
      std::unique_ptr<CPDF_Object> pNew = pObj->Clone();
      pObj = pNew.get();
      pDocument->AddIndirectObject(std::move(pNew));
    }

    CPDF_Dictionary* pObjDic = pObj->GetDict();
    if (pObjDic) {
      pObjDic->SetNewFor<CPDF_Name>("Type", "XObject");
      pObjDic->SetNewFor<CPDF_Name>("Subtype", "Form");
    }

    CPDF_Dictionary* pXObject = pNewXORes->GetDictFor("XObject");
    if (!pXObject)
      pXObject = pNewXORes->SetNewFor<CPDF_Dictionary>("XObject");

    ByteString sFormName = ByteString::Format("F%d", i);
    pXObject->SetFor(sFormName, pObj->MakeReference(pDocument));

    ByteString sStream;
    {
      auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pNewXObject);
      pAcc->LoadAllDataFiltered();
      sStream = ByteString(pAcc->GetSpan());
    }
    CFX_Matrix matrix = pAPDic->GetMatrixFor("Matrix");
    CFX_Matrix m = GetMatrix(rcAnnot, rcStream, matrix);
    sStream += ByteString::Format("q %f 0 0 %f %f %f cm /%s Do Q\n", m.a, m.d,
                                  m.e, m.f, sFormName.c_str());
    pNewXObject->SetDataAndRemoveFilter(sStream.AsRawSpan());
  }
  pPageDict->RemoveFor("Annots");
  return FLATTEN_SUCCESS;
}
