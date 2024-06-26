// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_ext.h"

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfdoc/cpdf_interactiveform.h"
#include "core/fpdfdoc/cpdf_metadata.h"
#include "core/fxcrt/fx_extension.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "third_party/base/ptr_util.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#endif  // PDF_ENABLE_XFA

static_assert(static_cast<int>(UnsupportedFeature::kDocumentXFAForm) ==
                  FPDF_UNSP_DOC_XFAFORM,
              "UnsupportedFeature::kDocumentXFAForm value mismatch");
static_assert(
    static_cast<int>(UnsupportedFeature::kDocumentPortableCollection) ==
        FPDF_UNSP_DOC_PORTABLECOLLECTION,
    "UnsupportedFeature::kDocumentPortableCollection value mismatch");
static_assert(static_cast<int>(UnsupportedFeature::kDocumentAttachment) ==
                  FPDF_UNSP_DOC_ATTACHMENT,
              "UnsupportedFeature::kDocumentAttachment value mismatch");
static_assert(static_cast<int>(UnsupportedFeature::kDocumentSecurity) ==
                  FPDF_UNSP_DOC_SECURITY,
              "UnsupportedFeature::kDocumentSecurity value mismatch");
static_assert(static_cast<int>(UnsupportedFeature::kDocumentSharedReview) ==
                  FPDF_UNSP_DOC_SHAREDREVIEW,
              "UnsupportedFeature::kDocumentSharedReview value mismatch");
static_assert(
    static_cast<int>(UnsupportedFeature::kDocumentSharedFormAcrobat) ==
        FPDF_UNSP_DOC_SHAREDFORM_ACROBAT,
    "UnsupportedFeature::kDocumentSharedFormAcrobat value mismatch");
static_assert(
    static_cast<int>(UnsupportedFeature::kDocumentSharedFormFilesystem) ==
        FPDF_UNSP_DOC_SHAREDFORM_FILESYSTEM,
    "UnsupportedFeature::kDocumentSharedFormFilesystem value mismatch");
static_assert(static_cast<int>(UnsupportedFeature::kDocumentSharedFormEmail) ==
                  FPDF_UNSP_DOC_SHAREDFORM_EMAIL,
              "UnsupportedFeature::kDocumentSharedFormEmail value mismatch");
static_assert(static_cast<int>(UnsupportedFeature::kAnnotation3d) ==
                  FPDF_UNSP_ANNOT_3DANNOT,
              "UnsupportedFeature::kAnnotation3d value mismatch");
static_assert(static_cast<int>(UnsupportedFeature::kAnnotationMovie) ==
                  FPDF_UNSP_ANNOT_MOVIE,
              "UnsupportedFeature::kAnnotationMovie value mismatch");
static_assert(static_cast<int>(UnsupportedFeature::kAnnotationSound) ==
                  FPDF_UNSP_ANNOT_SOUND,
              "UnsupportedFeature::kAnnotationSound value mismatch");
static_assert(static_cast<int>(UnsupportedFeature::kAnnotationScreenMedia) ==
                  FPDF_UNSP_ANNOT_SCREEN_MEDIA,
              "UnsupportedFeature::kAnnotationScreenMedia value mismatch");
static_assert(
    static_cast<int>(UnsupportedFeature::kAnnotationScreenRichMedia) ==
        FPDF_UNSP_ANNOT_SCREEN_RICHMEDIA,
    "UnsupportedFeature::kAnnotationScreenRichMedia value mismatch");
static_assert(static_cast<int>(UnsupportedFeature::kAnnotationAttachment) ==
                  FPDF_UNSP_ANNOT_ATTACHMENT,
              "UnsupportedFeature::kAnnotationAttachment value mismatch");
static_assert(static_cast<int>(UnsupportedFeature::kAnnotationSignature) ==
                  FPDF_UNSP_ANNOT_SIG,
              "UnsupportedFeature::kAnnotationSignature value mismatch");

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FSDK_SetUnSpObjProcessHandler(UNSUPPORT_INFO* unsp_info) {
  if (!unsp_info || unsp_info->version != 1)
    return false;

  CPDF_ModuleMgr::Get()->SetUnsupportInfoAdapter(
      pdfium::MakeUnique<CFSDK_UnsupportInfo_Adapter>(unsp_info));
  return true;
}

FPDF_EXPORT void FPDF_CALLCONV FSDK_SetTimeFunction(time_t (*func)()) {
  FXSYS_SetTimeFunction(func);
}

FPDF_EXPORT void FPDF_CALLCONV
FSDK_SetLocaltimeFunction(struct tm* (*func)(const time_t* tp)) {
  FXSYS_SetLocaltimeFunction(func);
}

FPDF_EXPORT int FPDF_CALLCONV FPDFDoc_GetPageMode(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return PAGEMODE_UNKNOWN;

  const CPDF_Dictionary* pRoot = pDoc->GetRoot();
  if (!pRoot)
    return PAGEMODE_UNKNOWN;

  const CPDF_Object* pName = pRoot->GetObjectFor("PageMode");
  if (!pName)
    return PAGEMODE_USENONE;

  ByteString strPageMode = pName->GetString();
  if (strPageMode.IsEmpty() || strPageMode.EqualNoCase("UseNone"))
    return PAGEMODE_USENONE;
  if (strPageMode.EqualNoCase("UseOutlines"))
    return PAGEMODE_USEOUTLINES;
  if (strPageMode.EqualNoCase("UseThumbs"))
    return PAGEMODE_USETHUMBS;
  if (strPageMode.EqualNoCase("FullScreen"))
    return PAGEMODE_FULLSCREEN;
  if (strPageMode.EqualNoCase("UseOC"))
    return PAGEMODE_USEOC;
  if (strPageMode.EqualNoCase("UseAttachments"))
    return PAGEMODE_USEATTACHMENTS;

  return PAGEMODE_UNKNOWN;
}
