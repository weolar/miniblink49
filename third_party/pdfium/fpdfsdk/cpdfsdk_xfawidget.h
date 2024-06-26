// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_CPDFSDK_XFAWIDGET_H_
#define FPDFSDK_CPDFSDK_XFAWIDGET_H_

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/cpdfsdk_annot.h"

class CPDFSDK_InteractiveForm;
class CPDFSDK_PageView;
class CXFA_FFWidget;

class CPDFSDK_XFAWidget final : public CPDFSDK_Annot {
 public:
  CPDFSDK_XFAWidget(CXFA_FFWidget* pAnnot,
                    CPDFSDK_PageView* pPageView,
                    CPDFSDK_InteractiveForm* pInteractiveForm);
  ~CPDFSDK_XFAWidget() override;

  // CPDFSDK_Annot:
  bool IsXFAField() const override;
  CXFA_FFWidget* GetXFAWidget() const override;
  CPDF_Annot::Subtype GetAnnotSubtype() const override;
  CFX_FloatRect GetRect() const override;

  CPDFSDK_InteractiveForm* GetInteractiveForm() const {
    return m_pInteractiveForm.Get();
  }

 private:
  UnownedPtr<CPDFSDK_InteractiveForm> const m_pInteractiveForm;
  UnownedPtr<CXFA_FFWidget> const m_hXFAWidget;
};

inline CPDFSDK_XFAWidget* ToXFAWidget(CPDFSDK_Annot* pAnnot) {
  return pAnnot && pAnnot->GetAnnotSubtype() == CPDF_Annot::Subtype::XFAWIDGET
      ? static_cast<CPDFSDK_XFAWidget*>(pAnnot) : nullptr;
}

#endif  // FPDFSDK_CPDFSDK_XFAWIDGET_H_
