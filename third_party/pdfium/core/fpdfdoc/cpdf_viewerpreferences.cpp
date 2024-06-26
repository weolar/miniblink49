// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_viewerpreferences.h"

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"

CPDF_ViewerPreferences::CPDF_ViewerPreferences(const CPDF_Document* pDoc)
    : m_pDoc(pDoc) {}

CPDF_ViewerPreferences::~CPDF_ViewerPreferences() = default;

bool CPDF_ViewerPreferences::IsDirectionR2L() const {
  const CPDF_Dictionary* pDict = GetViewerPreferences();
  return pDict ? pDict->GetStringFor("Direction") == "R2L" : false;
}

bool CPDF_ViewerPreferences::PrintScaling() const {
  const CPDF_Dictionary* pDict = GetViewerPreferences();
  return pDict ? pDict->GetStringFor("PrintScaling") != "None" : true;
}

int32_t CPDF_ViewerPreferences::NumCopies() const {
  const CPDF_Dictionary* pDict = GetViewerPreferences();
  return pDict ? pDict->GetIntegerFor("NumCopies") : 1;
}

const CPDF_Array* CPDF_ViewerPreferences::PrintPageRange() const {
  const CPDF_Dictionary* pDict = GetViewerPreferences();
  return pDict ? pDict->GetArrayFor("PrintPageRange") : nullptr;
}

ByteString CPDF_ViewerPreferences::Duplex() const {
  const CPDF_Dictionary* pDict = GetViewerPreferences();
  return pDict ? pDict->GetStringFor("Duplex") : ByteString("None");
}

Optional<ByteString> CPDF_ViewerPreferences::GenericName(
    const ByteString& bsKey) const {
  const CPDF_Dictionary* pDict = GetViewerPreferences();
  if (!pDict)
    return {};

  const CPDF_Name* pName = ToName(pDict->GetObjectFor(bsKey));
  if (!pName)
    return {};

  return pName->GetString();
}

const CPDF_Dictionary* CPDF_ViewerPreferences::GetViewerPreferences() const {
  const CPDF_Dictionary* pDict = m_pDoc->GetRoot();
  return pDict ? pDict->GetDictFor("ViewerPreferences") : nullptr;
}
