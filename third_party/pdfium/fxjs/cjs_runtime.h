// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_RUNTIME_H_
#define FXJS_CJS_RUNTIME_H_

#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "core/fxcrt/observable.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fxjs/cfxjs_engine.h"
#include "fxjs/cjs_eventhandler.h"
#include "fxjs/ijs_runtime.h"

class CJS_EventContext;

class CJS_Runtime final : public IJS_Runtime,
                          public CFXJS_Engine,
                          public Observable<CJS_Runtime> {
 public:
  using FieldEvent = std::pair<WideString, JS_EVENT_T>;

  explicit CJS_Runtime(CPDFSDK_FormFillEnvironment* pFormFillEnv);
  ~CJS_Runtime() override;

  // IJS_Runtime
  IJS_EventContext* NewEventContext() override;
  void ReleaseEventContext(IJS_EventContext* pContext) override;
  CPDFSDK_FormFillEnvironment* GetFormFillEnv() const override;
  Optional<IJS_Runtime::JS_Error> ExecuteScript(
      const WideString& script) override;

  CJS_EventContext* GetCurrentEventContext() const;

  // Returns true if the event isn't already found in the set.
  bool AddEventToSet(const FieldEvent& event);
  void RemoveEventFromSet(const FieldEvent& event);

  void BeginBlock() { m_bBlocking = true; }
  void EndBlock() { m_bBlocking = false; }
  bool IsBlocking() const { return m_bBlocking; }

  // Attempt to convert the |value| into a number. If successful the number
  // value will be returned, otherwise |value| is returned.
  v8::Local<v8::Value> MaybeCoerceToNumber(v8::Local<v8::Value> value);

#ifdef PDF_ENABLE_XFA
  CJS_Runtime* AsCJSRuntime() override;
  bool GetValueByNameFromGlobalObject(ByteStringView utf8Name,
                                      CFXJSE_Value* pValue) override;
  bool SetValueByNameInGlobalObject(ByteStringView utf8Name,
                                    CFXJSE_Value* pValue) override;
#endif  // PDF_ENABLE_XFA

 private:
  void DefineJSObjects();
  void SetFormFillEnvToDocument();

  std::vector<std::unique_ptr<CJS_EventContext>> m_EventContextArray;
  CPDFSDK_FormFillEnvironment::ObservedPtr m_pFormFillEnv;
  bool m_bBlocking = false;
  bool m_isolateManaged = false;
  std::set<FieldEvent> m_FieldEventSet;
};

#endif  // FXJS_CJS_RUNTIME_H_
