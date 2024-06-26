// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_AACTION_H_
#define CORE_FPDFDOC_CPDF_AACTION_H_

#include "core/fpdfdoc/cpdf_action.h"

class CPDF_Dictionary;

class CPDF_AAction {
 public:
  enum AActionType {
    kCursorEnter = 0,
    kCursorExit,
    kButtonDown,
    kButtonUp,
    kGetFocus,
    kLoseFocus,
    kPageOpen,
    kPageClose,
    kPageVisible,
    kPageInvisible,
    kOpenPage,
    kClosePage,
    kKeyStroke,
    kFormat,
    kValidate,
    kCalculate,
    kCloseDocument,
    kSaveDocument,
    kDocumentSaved,
    kPrintDocument,
    kDocumentPrinted,
    kDocumentOpen,
    kNumberOfActions  // Must be last.
  };

  explicit CPDF_AAction(const CPDF_Dictionary* pDict);
  CPDF_AAction(const CPDF_AAction& that);
  ~CPDF_AAction();

  bool ActionExist(AActionType eType) const;
  CPDF_Action GetAction(AActionType eType) const;
  const CPDF_Dictionary* GetDict() const { return m_pDict.Get(); }

  static bool IsUserClick(AActionType eType);

 private:
  UnownedPtr<const CPDF_Dictionary> const m_pDict;
};

#endif  // CORE_FPDFDOC_CPDF_AACTION_H_
