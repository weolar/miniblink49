// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_runtime.h"

#include <algorithm>

#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fxjs/cfx_globaldata.h"
#include "fxjs/cjs_annot.h"
#include "fxjs/cjs_app.h"
#include "fxjs/cjs_border.h"
#include "fxjs/cjs_color.h"
#include "fxjs/cjs_console.h"
#include "fxjs/cjs_display.h"
#include "fxjs/cjs_document.h"
#include "fxjs/cjs_event.h"
#include "fxjs/cjs_event_context.h"
#include "fxjs/cjs_eventhandler.h"
#include "fxjs/cjs_field.h"
#include "fxjs/cjs_font.h"
#include "fxjs/cjs_global.h"
#include "fxjs/cjs_globalarrays.h"
#include "fxjs/cjs_globalconsts.h"
#include "fxjs/cjs_highlight.h"
#include "fxjs/cjs_icon.h"
#include "fxjs/cjs_object.h"
#include "fxjs/cjs_position.h"
#include "fxjs/cjs_publicmethods.h"
#include "fxjs/cjs_scalehow.h"
#include "fxjs/cjs_scalewhen.h"
#include "fxjs/cjs_style.h"
#include "fxjs/cjs_timerobj.h"
#include "fxjs/cjs_util.h"
#include "fxjs/cjs_zoomtype.h"
#include "fxjs/js_define.h"
#include "third_party/base/ptr_util.h"

#ifdef PDF_ENABLE_XFA
#include "fxjs/xfa/cfxjse_value.h"
#endif  // PDF_ENABLE_XFA

CJS_Runtime::CJS_Runtime(CPDFSDK_FormFillEnvironment* pFormFillEnv)
    : m_pFormFillEnv(pFormFillEnv) {
  v8::Isolate* pIsolate = nullptr;

  IPDF_JSPLATFORM* pPlatform = m_pFormFillEnv->GetFormFillInfo()->m_pJsPlatform;
  if (pPlatform->version <= 2) {
    unsigned int embedderDataSlot = 0;
    v8::Isolate* pExternalIsolate = nullptr;
    if (pPlatform->version == 2) {
      pExternalIsolate = static_cast<v8::Isolate*>(pPlatform->m_isolate);
      embedderDataSlot = pPlatform->m_v8EmbedderSlot;
    }
    FXJS_Initialize(embedderDataSlot, pExternalIsolate);
  }
  m_isolateManaged = FXJS_GetIsolate(&pIsolate);
  SetIsolate(pIsolate);

#ifdef PDF_ENABLE_XFA
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);
#endif

  if (m_isolateManaged || FXJS_GlobalIsolateRefCount() == 0)
    DefineJSObjects();

  ScopedEventContext pContext(this);
  InitializeEngine();
  SetFormFillEnvToDocument();
}

CJS_Runtime::~CJS_Runtime() {
  NotifyObservers();
  ReleaseEngine();
  if (m_isolateManaged)
    DisposeIsolate();
}

void CJS_Runtime::DefineJSObjects() {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  v8::Local<v8::Context> context = v8::Context::New(GetIsolate());
  v8::Context::Scope context_scope(context);

  // The call order determines the "ObjDefID" assigned to each class.
  // ObjDefIDs 0 - 2
  CJS_Border::DefineJSObjects(this);
  CJS_Display::DefineJSObjects(this);
  CJS_Font::DefineJSObjects(this);

  // ObjDefIDs 3 - 5
  CJS_Highlight::DefineJSObjects(this);
  CJS_Position::DefineJSObjects(this);
  CJS_ScaleHow::DefineJSObjects(this);

  // ObjDefIDs 6 - 8
  CJS_ScaleWhen::DefineJSObjects(this);
  CJS_Style::DefineJSObjects(this);
  CJS_Zoomtype::DefineJSObjects(this);

  // ObjDefIDs 9 - 11
  CJS_App::DefineJSObjects(this);
  CJS_Color::DefineJSObjects(this);
  CJS_Console::DefineJSObjects(this);

  // ObjDefIDs 12 - 14
  CJS_Document::DefineJSObjects(this);
  CJS_Event::DefineJSObjects(this);
  CJS_Field::DefineJSObjects(this);

  // ObjDefIDs 15 - 17
  CJS_Global::DefineJSObjects(this);
  CJS_Icon::DefineJSObjects(this);
  CJS_Util::DefineJSObjects(this);

  // ObjDefIDs 18 - 20 (these can't fail, return void).
  CJS_PublicMethods::DefineJSObjects(this);
  CJS_GlobalConsts::DefineJSObjects(this);
  CJS_GlobalArrays::DefineJSObjects(this);

  // ObjDefIDs 21 - 22.
  CJS_TimerObj::DefineJSObjects(this);
  CJS_Annot::DefineJSObjects(this);
}

IJS_EventContext* CJS_Runtime::NewEventContext() {
  m_EventContextArray.push_back(pdfium::MakeUnique<CJS_EventContext>(this));
  return m_EventContextArray.back().get();
}

void CJS_Runtime::ReleaseEventContext(IJS_EventContext* pContext) {
  ASSERT(pContext == m_EventContextArray.back().get());
  m_EventContextArray.pop_back();
}

CJS_EventContext* CJS_Runtime::GetCurrentEventContext() const {
  return m_EventContextArray.empty() ? nullptr
                                     : m_EventContextArray.back().get();
}

void CJS_Runtime::SetFormFillEnvToDocument() {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);

  v8::Local<v8::Object> pThis = GetThisObj();
  if (pThis.IsEmpty())
    return;

  auto pJSDocument = JSGetObject<CJS_Document>(pThis);
  if (!pJSDocument)
    return;

  pJSDocument->SetFormFillEnv(m_pFormFillEnv.Get());
}

CPDFSDK_FormFillEnvironment* CJS_Runtime::GetFormFillEnv() const {
  return m_pFormFillEnv.Get();
}

Optional<IJS_Runtime::JS_Error> CJS_Runtime::ExecuteScript(
    const WideString& script) {
  return Execute(script);
}

bool CJS_Runtime::AddEventToSet(const FieldEvent& event) {
  return m_FieldEventSet.insert(event).second;
}

void CJS_Runtime::RemoveEventFromSet(const FieldEvent& event) {
  m_FieldEventSet.erase(event);
}

#ifdef PDF_ENABLE_XFA
WideString ChangeObjName(const WideString& str) {
  WideString sRet = str;
  sRet.Replace(L"_", L".");
  return sRet;
}

CJS_Runtime* CJS_Runtime::AsCJSRuntime() {
  return this;
}

bool CJS_Runtime::GetValueByNameFromGlobalObject(ByteStringView utf8Name,
                                                 CFXJSE_Value* pValue) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);
  v8::Local<v8::String> str =
      v8::String::NewFromUtf8(GetIsolate(), utf8Name.unterminated_c_str(),
                              v8::NewStringType::kNormal, utf8Name.GetLength())
          .ToLocalChecked();
  v8::MaybeLocal<v8::Value> maybe_propvalue =
      context->Global()->Get(context, str);
  if (maybe_propvalue.IsEmpty()) {
    pValue->SetUndefined();
    return false;
  }
  v8::Local<v8::Value> propvalue = maybe_propvalue.ToLocalChecked();
  if (propvalue.IsEmpty()) {
    pValue->SetUndefined();
    return false;
  }
  pValue->ForceSetValue(propvalue);
  return true;
}

bool CJS_Runtime::SetValueByNameInGlobalObject(ByteStringView utf8Name,
                                               CFXJSE_Value* pValue) {
  if (utf8Name.IsEmpty() || !pValue)
    return false;

  v8::Isolate* pIsolate = GetIsolate();
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);
  v8::Local<v8::Value> propvalue =
      v8::Local<v8::Value>::New(pIsolate, pValue->DirectGetValue());
  v8::Local<v8::String> str =
      v8::String::NewFromUtf8(pIsolate, utf8Name.unterminated_c_str(),
                              v8::NewStringType::kNormal, utf8Name.GetLength())
          .ToLocalChecked();
  return context->Global()->Set(context, str, propvalue).FromJust();
}
#endif

v8::Local<v8::Value> CJS_Runtime::MaybeCoerceToNumber(
    v8::Local<v8::Value> value) {
  bool bAllowNaN = false;
  if (value->IsString()) {
    ByteString bstr = ToWideString(value).ToDefANSI();
    if (bstr.GetLength() == 0)
      return value;
    if (bstr == "NaN")
      bAllowNaN = true;
  }

  v8::Isolate* pIsolate = GetIsolate();
  v8::TryCatch try_catch(pIsolate);
  v8::MaybeLocal<v8::Number> maybeNum =
      value->ToNumber(pIsolate->GetCurrentContext());
  if (maybeNum.IsEmpty())
    return value;

  v8::Local<v8::Number> num = maybeNum.ToLocalChecked();
  if (std::isnan(num->Value()) && !bAllowNaN)
    return value;

  return num;
}
