// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/fpdf_structtree.h"

#include <memory>

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfdoc/cpdf_structelement.h"
#include "core/fpdfdoc/cpdf_structtree.h"
#include "fpdfsdk/cpdfsdk_helpers.h"

namespace {

unsigned long WideStringToBuffer(const WideString& str,
                                 void* buffer,
                                 unsigned long buflen) {
  if (str.IsEmpty())
    return 0;

  ByteString encodedStr = str.ToUTF16LE();
  const unsigned long len = encodedStr.GetLength();
  if (buffer && len <= buflen)
    memcpy(buffer, encodedStr.c_str(), len);
  return len;
}

}  // namespace

FPDF_EXPORT FPDF_STRUCTTREE FPDF_CALLCONV
FPDF_StructTree_GetForPage(FPDF_PAGE page) {
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return nullptr;

  // Caller takes onwership.
  return FPDFStructTreeFromCPDFStructTree(
      CPDF_StructTree::LoadPage(pPage->GetDocument(), pPage->GetDict())
          .release());
}

FPDF_EXPORT void FPDF_CALLCONV
FPDF_StructTree_Close(FPDF_STRUCTTREE struct_tree) {
  std::unique_ptr<CPDF_StructTree>(
      CPDFStructTreeFromFPDFStructTree(struct_tree));
}

FPDF_EXPORT int FPDF_CALLCONV
FPDF_StructTree_CountChildren(FPDF_STRUCTTREE struct_tree) {
  CPDF_StructTree* tree = CPDFStructTreeFromFPDFStructTree(struct_tree);
  if (!tree)
    return -1;

  pdfium::base::CheckedNumeric<int> tmp_size = tree->CountTopElements();
  return tmp_size.ValueOrDefault(-1);
}

FPDF_EXPORT FPDF_STRUCTELEMENT FPDF_CALLCONV
FPDF_StructTree_GetChildAtIndex(FPDF_STRUCTTREE struct_tree, int index) {
  CPDF_StructTree* tree = CPDFStructTreeFromFPDFStructTree(struct_tree);
  if (!tree || index < 0 ||
      static_cast<size_t>(index) >= tree->CountTopElements()) {
    return nullptr;
  }
  return FPDFStructElementFromCPDFStructElement(
      tree->GetTopElement(static_cast<size_t>(index)));
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDF_StructElement_GetAltText(FPDF_STRUCTELEMENT struct_element,
                              void* buffer,
                              unsigned long buflen) {
  CPDF_StructElement* elem =
      CPDFStructElementFromFPDFStructElement(struct_element);
  return (elem && elem->GetDict())
             ? WideStringToBuffer(elem->GetDict()->GetUnicodeTextFor("Alt"),
                                  buffer, buflen)
             : 0;
}

FPDF_EXPORT int FPDF_CALLCONV
FPDF_StructElement_GetMarkedContentID(FPDF_STRUCTELEMENT struct_element) {
  CPDF_StructElement* elem =
      CPDFStructElementFromFPDFStructElement(struct_element);
  const CPDF_Object* p =
      (elem && elem->GetDict()) ? elem->GetDict()->GetObjectFor("K") : nullptr;
  return p && p->IsNumber() ? p->GetInteger() : -1;
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDF_StructElement_GetType(FPDF_STRUCTELEMENT struct_element,
                           void* buffer,
                           unsigned long buflen) {
  CPDF_StructElement* elem =
      CPDFStructElementFromFPDFStructElement(struct_element);
  return elem ? WideStringToBuffer(
                    WideString::FromUTF8(elem->GetType().AsStringView()),
                    buffer, buflen)
              : 0;
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDF_StructElement_GetTitle(FPDF_STRUCTELEMENT struct_element,
                            void* buffer,
                            unsigned long buflen) {
  CPDF_StructElement* elem =
      CPDFStructElementFromFPDFStructElement(struct_element);
  return elem ? WideStringToBuffer(
                    WideString::FromUTF8(elem->GetTitle().AsStringView()),
                    buffer, buflen)
              : 0;
}

FPDF_EXPORT int FPDF_CALLCONV
FPDF_StructElement_CountChildren(FPDF_STRUCTELEMENT struct_element) {
  CPDF_StructElement* elem =
      CPDFStructElementFromFPDFStructElement(struct_element);
  if (!elem)
    return -1;

  pdfium::base::CheckedNumeric<int> tmp_size = elem->CountKids();
  return tmp_size.ValueOrDefault(-1);
}

FPDF_EXPORT FPDF_STRUCTELEMENT FPDF_CALLCONV
FPDF_StructElement_GetChildAtIndex(FPDF_STRUCTELEMENT struct_element,
                                   int index) {
  CPDF_StructElement* elem =
      CPDFStructElementFromFPDFStructElement(struct_element);
  if (!elem || index < 0 || static_cast<size_t>(index) >= elem->CountKids())
    return nullptr;

  return FPDFStructElementFromCPDFStructElement(
      elem->GetKidIfElement(static_cast<size_t>(index)));
}
