// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_CPDFSDK_ANNOT_H_
#define FPDFSDK_CPDFSDK_ANNOT_H_

#include "core/fpdfdoc/cpdf_aaction.h"
#include "core/fpdfdoc/cpdf_annot.h"
#include "core/fpdfdoc/cpdf_defaultappearance.h"
#include "core/fxcrt/observable.h"
#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/cfx_systemhandler.h"
#include "fpdfsdk/cpdfsdk_common.h"
#include "fpdfsdk/cpdfsdk_helpers.h"

class CFX_Matrix;
class CFX_RenderDevice;
class CPDF_Page;
class CPDF_RenderOptions;
class CPDFSDK_BAAnnot;
class CPDFSDK_PageView;

#ifdef PDF_ENABLE_XFA
class CXFA_FFWidget;
#endif  // PDF_ENABLE_XFA

class CPDFSDK_Annot : public Observable<CPDFSDK_Annot> {
 public:
  explicit CPDFSDK_Annot(CPDFSDK_PageView* pPageView);
  virtual ~CPDFSDK_Annot();

  virtual CPDFSDK_BAAnnot* AsBAAnnot();

#ifdef PDF_ENABLE_XFA
  virtual bool IsXFAField() const;
  virtual CXFA_FFWidget* GetXFAWidget() const;
#endif  // PDF_ENABLE_XFA

  virtual int GetLayoutOrder() const;
  virtual CPDF_Annot* GetPDFAnnot() const;
  virtual CPDF_Annot::Subtype GetAnnotSubtype() const;
  virtual bool IsSignatureWidget() const;
  virtual CFX_FloatRect GetRect() const;
  virtual void SetRect(const CFX_FloatRect& rect);

  IPDF_Page* GetPage();  // Returns XFA Page if possible, else PDF page.
  CPDF_Page* GetPDFPage();
#ifdef PDF_ENABLE_XFA
  CPDFXFA_Page* GetPDFXFAPage();
#endif  // PDF_ENABLE_XFA

  CPDFSDK_PageView* GetPageView() const { return m_pPageView.Get(); }

 protected:
  UnownedPtr<CPDFSDK_PageView> const m_pPageView;
};

inline CPDFSDK_BAAnnot* ToBAAnnot(CPDFSDK_Annot* pAnnot) {
  return pAnnot ? pAnnot->AsBAAnnot() : nullptr;
}

#endif  // FPDFSDK_CPDFSDK_ANNOT_H_
