// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_aaction.h"

#include "core/fpdfapi/parser/cpdf_dictionary.h"

namespace {

constexpr const char* g_sAATypes[] = {
    "E",   // kCursorEnter
    "X",   // kCursorExit
    "D",   // kButtonDown
    "U",   // kButtonUp
    "Fo",  // kGetFocus
    "Bl",  // kLoseFocus
    "PO",  // kPageOpen
    "PC",  // kPageClose
    "PV",  // kPageVisible
    "PI",  // kPageInvisible
    "O",   // kOpenPage
    "C",   // kClosePage
    "K",   // kKeyStroke
    "F",   // kFormat
    "V",   // kValidate
    "C",   // kCalculate
    "WC",  // kCloseDocument
    "WS",  // kSaveDocument
    "DS",  // kDocumentSaved
    "WP",  // kPrintDocument
    "DP",  // kDocumentPrinted
};

// |g_sAATypes| should have one less element than enum AActionType due to
// kDocumentOpen, which is an artificial type.
static_assert(FX_ArraySize(g_sAATypes) == CPDF_AAction::kNumberOfActions - 1,
              "g_sAATypes count mismatch");

}  // namespace

CPDF_AAction::CPDF_AAction(const CPDF_Dictionary* pDict) : m_pDict(pDict) {}

CPDF_AAction::CPDF_AAction(const CPDF_AAction& that) = default;

CPDF_AAction::~CPDF_AAction() = default;

bool CPDF_AAction::ActionExist(AActionType eType) const {
  return m_pDict && m_pDict->KeyExist(g_sAATypes[eType]);
}

CPDF_Action CPDF_AAction::GetAction(AActionType eType) const {
  return CPDF_Action(m_pDict ? m_pDict->GetDictFor(g_sAATypes[eType])
                             : nullptr);
}

// static
bool CPDF_AAction::IsUserClick(AActionType eType) {
  switch (eType) {
    case kButtonUp:
    case kButtonDown:
      return true;
    default:
      return false;
  }
}
