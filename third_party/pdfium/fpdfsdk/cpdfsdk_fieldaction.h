// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_CPDFSDK_FIELDACTION_H_
#define FPDFSDK_CPDFSDK_FIELDACTION_H_

#include "core/fxcrt/fx_string.h"

#ifdef PDF_ENABLE_XFA
typedef enum {
  PDFSDK_XFA_Click = 0,
  PDFSDK_XFA_Full,
  PDFSDK_XFA_PreOpen,
  PDFSDK_XFA_PostOpen
} PDFSDK_XFAAActionType;
#endif  // PDF_ENABLE_XFA

struct CPDFSDK_FieldAction {
  CPDFSDK_FieldAction();
  CPDFSDK_FieldAction(const CPDFSDK_FieldAction& other) = delete;
  ~CPDFSDK_FieldAction();

  bool bModifier = false;
  bool bShift = false;
  bool bKeyDown = false;
  bool bWillCommit = false;
  bool bFieldFull = false;
  bool bRC = true;
  int nSelEnd = 0;
  int nSelStart = 0;
  WideString sChange;
  WideString sChangeEx;
  WideString sValue;
};

#endif  // FPDFSDK_CPDFSDK_FIELDACTION_H_
