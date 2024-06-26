// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_EVENT_CONTEXT_STUB_H_
#define FXJS_CJS_EVENT_CONTEXT_STUB_H_

#include "fxjs/ijs_event_context.h"

class CJS_EventContextStub final : public IJS_EventContext {
 public:
  CJS_EventContextStub() {}
  ~CJS_EventContextStub() override {}

  // IJS_EventContext:
  Optional<IJS_Runtime::JS_Error> RunScript(const WideString& script) override;

  void OnApp_Init() override {}
  void OnDoc_Open(CPDFSDK_FormFillEnvironment* pFormFillEnv,
                  const WideString& strTargetName) override {}
  void OnDoc_WillPrint(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnDoc_DidPrint(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnDoc_WillSave(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnDoc_DidSave(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnDoc_WillClose(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnPage_Open(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnPage_Close(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnPage_InView(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnPage_OutView(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnField_MouseDown(bool bModifier,
                         bool bShift,
                         CPDF_FormField* pTarget) override {}
  void OnField_MouseEnter(bool bModifier,
                          bool bShift,
                          CPDF_FormField* pTarget) override {}
  void OnField_MouseExit(bool bModifier,
                         bool bShift,
                         CPDF_FormField* pTarget) override {}
  void OnField_MouseUp(bool bModifier,
                       bool bShift,
                       CPDF_FormField* pTarget) override {}
  void OnField_Focus(bool bModifier,
                     bool bShift,
                     CPDF_FormField* pTarget,
                     WideString* Value) override {}
  void OnField_Blur(bool bModifier,
                    bool bShift,
                    CPDF_FormField* pTarget,
                    WideString* Value) override {}
  void OnField_Calculate(CPDF_FormField* pSource,
                         CPDF_FormField* pTarget,
                         WideString* pValue,
                         bool* pRc) override {}
  void OnField_Format(CPDF_FormField* pTarget,
                      WideString* Value,
                      bool bWillCommit) override {}
  void OnField_Keystroke(WideString* strChange,
                         const WideString& strChangeEx,
                         bool KeyDown,
                         bool bModifier,
                         int* nSelEnd,
                         int* nSelStart,
                         bool bShift,
                         CPDF_FormField* pTarget,
                         WideString* Value,
                         bool bWillCommit,
                         bool bFieldFull,
                         bool* bRc) override {}
  void OnField_Validate(WideString* strChange,
                        const WideString& strChangeEx,
                        bool bKeyDown,
                        bool bModifier,
                        bool bShift,
                        CPDF_FormField* pTarget,
                        WideString* Value,
                        bool* bRc) override {}
  void OnScreen_Focus(bool bModifier,
                      bool bShift,
                      CPDFSDK_Annot* pScreen) override {}
  void OnScreen_Blur(bool bModifier,
                     bool bShift,
                     CPDFSDK_Annot* pScreen) override {}
  void OnScreen_Open(bool bModifier,
                     bool bShift,
                     CPDFSDK_Annot* pScreen) override {}
  void OnScreen_Close(bool bModifier,
                      bool bShift,
                      CPDFSDK_Annot* pScreen) override {}
  void OnScreen_MouseDown(bool bModifier,
                          bool bShift,
                          CPDFSDK_Annot* pScreen) override {}
  void OnScreen_MouseUp(bool bModifier,
                        bool bShift,
                        CPDFSDK_Annot* pScreen) override {}
  void OnScreen_MouseEnter(bool bModifier,
                           bool bShift,
                           CPDFSDK_Annot* pScreen) override {}
  void OnScreen_MouseExit(bool bModifier,
                          bool bShift,
                          CPDFSDK_Annot* pScreen) override {}
  void OnScreen_InView(bool bModifier,
                       bool bShift,
                       CPDFSDK_Annot* pScreen) override {}
  void OnScreen_OutView(bool bModifier,
                        bool bShift,
                        CPDFSDK_Annot* pScreen) override {}
  void OnBookmark_MouseUp(CPDF_Bookmark* pBookMark) override {}
  void OnLink_MouseUp(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnMenu_Exec(CPDFSDK_FormFillEnvironment* pFormFillEnv,
                   const WideString&) override {}
  void OnBatchExec(CPDFSDK_FormFillEnvironment* pFormFillEnv) override {}
  void OnConsole_Exec() override {}
  void OnExternal_Exec() override {}
};

#endif  // FXJS_CJS_EVENT_CONTEXT_STUB_H_
