// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/cpdf_annotcontext.h"

#include "core/fpdfapi/page/cpdf_form.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "third_party/base/ptr_util.h"

CPDF_AnnotContext::CPDF_AnnotContext(CPDF_Dictionary* pAnnotDict,
                                     CPDF_Page* pPage,
                                     CPDF_Stream* pStream)
    : m_pAnnotDict(pAnnotDict), m_pPage(pPage) {
  SetForm(pStream);
}

CPDF_AnnotContext::~CPDF_AnnotContext() = default;

void CPDF_AnnotContext::SetForm(CPDF_Stream* pStream) {
  if (!pStream)
    return;

  // Reset the annotation matrix to be the identity matrix, since the
  // appearance stream already takes matrix into account.
  pStream->GetDict()->SetMatrixFor("Matrix", CFX_Matrix());

  m_pAnnotForm = pdfium::MakeUnique<CPDF_Form>(
      m_pPage->GetDocument(), m_pPage->m_pResources.Get(), pStream);
  m_pAnnotForm->ParseContent(nullptr, nullptr, nullptr, nullptr);
}
