// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_ACTION_H_
#define CORE_FPDFDOC_CPDF_ACTION_H_

#include "core/fpdfdoc/cpdf_dest.h"
#include "core/fxcrt/fx_string.h"

class CPDF_Dictionary;
class CPDF_Document;

class CPDF_Action {
 public:
  enum ActionType {
    Unknown = 0,
    GoTo,
    GoToR,
    GoToE,
    Launch,
    Thread,
    URI,
    Sound,
    Movie,
    Hide,
    Named,
    SubmitForm,
    ResetForm,
    ImportData,
    JavaScript,
    SetOCGState,
    Rendition,
    Trans,
    GoTo3DView
  };

  explicit CPDF_Action(const CPDF_Dictionary* pDict);
  CPDF_Action(const CPDF_Action& that);
  ~CPDF_Action();

  const CPDF_Dictionary* GetDict() const { return m_pDict.Get(); }
  ActionType GetType() const;
  CPDF_Dest GetDest(CPDF_Document* pDoc) const;
  WideString GetFilePath() const;
  ByteString GetURI(const CPDF_Document* pDoc) const;
  bool GetHideStatus() const;
  ByteString GetNamedAction() const;
  uint32_t GetFlags() const;
  WideString GetJavaScript() const;
  size_t GetSubActionsCount() const;
  CPDF_Action GetSubAction(size_t iIndex) const;

 private:
  UnownedPtr<const CPDF_Dictionary> const m_pDict;
};

#endif  // CORE_FPDFDOC_CPDF_ACTION_H_
