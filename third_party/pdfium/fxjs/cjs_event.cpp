// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_event.h"

#include "fxjs/cjs_event_context.h"
#include "fxjs/cjs_eventhandler.h"
#include "fxjs/cjs_field.h"
#include "fxjs/cjs_object.h"
#include "fxjs/js_define.h"

const JSPropertySpec CJS_Event::PropertySpecs[] = {
    {"change", get_change_static, set_change_static},
    {"changeEx", get_change_ex_static, set_change_ex_static},
    {"commitKey", get_commit_key_static, set_commit_key_static},
    {"fieldFull", get_field_full_static, set_field_full_static},
    {"keyDown", get_key_down_static, set_key_down_static},
    {"modifier", get_modifier_static, set_modifier_static},
    {"name", get_name_static, set_name_static},
    {"rc", get_rc_static, set_rc_static},
    {"richChange", get_rich_change_static, set_rich_change_static},
    {"richChangeEx", get_rich_change_ex_static, set_rich_change_ex_static},
    {"richValue", get_rich_value_static, set_rich_value_static},
    {"selEnd", get_sel_end_static, set_sel_end_static},
    {"selStart", get_sel_start_static, set_sel_start_static},
    {"shift", get_shift_static, set_shift_static},
    {"source", get_source_static, set_source_static},
    {"target", get_target_static, set_target_static},
    {"targetName", get_target_name_static, set_target_name_static},
    {"type", get_type_static, set_type_static},
    {"value", get_value_static, set_value_static},
    {"willCommit", get_will_commit_static, set_will_commit_static}};

int CJS_Event::ObjDefnID = -1;
const char CJS_Event::kName[] = "event";

// static
int CJS_Event::GetObjDefnID() {
  return ObjDefnID;
}

// static
void CJS_Event::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID = pEngine->DefineObj(CJS_Event::kName, FXJSOBJTYPE_STATIC,
                                 JSConstructor<CJS_Event>, JSDestructor);
  DefineProps(pEngine, ObjDefnID, PropertySpecs);
}

CJS_Event::CJS_Event(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime)
    : CJS_Object(pObject, pRuntime) {}

CJS_Event::~CJS_Event() = default;

CJS_Result CJS_Event::get_change(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(
      pRuntime->NewString(pEvent->Change().AsStringView()));
}

CJS_Result CJS_Event::set_change(CJS_Runtime* pRuntime,
                                 v8::Local<v8::Value> vp) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();

  if (vp->IsString()) {
    WideString& wChange = pEvent->Change();
    wChange = pRuntime->ToWideString(vp);
  }
  return CJS_Result::Success();
}

CJS_Result CJS_Event::get_change_ex(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(
      pRuntime->NewString(pEvent->ChangeEx().AsStringView()));
}

CJS_Result CJS_Event::set_change_ex(CJS_Runtime* pRuntime,
                                    v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_commit_key(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(pRuntime->NewNumber(pEvent->CommitKey()));
}

CJS_Result CJS_Event::set_commit_key(CJS_Runtime* pRuntime,
                                     v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_field_full(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  if (pEvent->Name() != "Keystroke")
    return CJS_Result::Failure(L"unrecognized event");

  return CJS_Result::Success(pRuntime->NewBoolean(pEvent->FieldFull()));
}

CJS_Result CJS_Event::set_field_full(CJS_Runtime* pRuntime,
                                     v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_key_down(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(pRuntime->NewBoolean(pEvent->KeyDown()));
}

CJS_Result CJS_Event::set_key_down(CJS_Runtime* pRuntime,
                                   v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_modifier(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(pRuntime->NewBoolean(pEvent->Modifier()));
}

CJS_Result CJS_Event::set_modifier(CJS_Runtime* pRuntime,
                                   v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_name(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(pRuntime->NewString(pEvent->Name()));
}

CJS_Result CJS_Event::set_name(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_rc(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(pRuntime->NewBoolean(pEvent->Rc()));
}

CJS_Result CJS_Event::set_rc(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  pEvent->Rc() = pRuntime->ToBoolean(vp);
  return CJS_Result::Success();
}

CJS_Result CJS_Event::get_rich_change(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Event::set_rich_change(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Event::get_rich_change_ex(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Event::set_rich_change_ex(CJS_Runtime* pRuntime,
                                         v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Event::get_rich_value(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Event::set_rich_value(CJS_Runtime* pRuntime,
                                     v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Event::get_sel_end(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  if (pEvent->Name() != "Keystroke")
    return CJS_Result::Success();

  return CJS_Result::Success(pRuntime->NewNumber(pEvent->SelEnd()));
}

CJS_Result CJS_Event::set_sel_end(CJS_Runtime* pRuntime,
                                  v8::Local<v8::Value> vp) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  if (pEvent->Name() == "Keystroke")
    pEvent->SetSelEnd(pRuntime->ToInt32(vp));

  return CJS_Result::Success();
}

CJS_Result CJS_Event::get_sel_start(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  if (pEvent->Name() != "Keystroke")
    return CJS_Result::Success();

  return CJS_Result::Success(pRuntime->NewNumber(pEvent->SelStart()));
}

CJS_Result CJS_Event::set_sel_start(CJS_Runtime* pRuntime,
                                    v8::Local<v8::Value> vp) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  if (pEvent->Name() == "Keystroke")
    pEvent->SetSelStart(pRuntime->ToInt32(vp));

  return CJS_Result::Success();
}

CJS_Result CJS_Event::get_shift(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(pRuntime->NewBoolean(pEvent->Shift()));
}

CJS_Result CJS_Event::set_shift(CJS_Runtime* pRuntime,
                                v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_source(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(pEvent->Source()->ToV8Object());
}

CJS_Result CJS_Event::set_source(CJS_Runtime* pRuntime,
                                 v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_target(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(pEvent->Target_Field()->ToV8Object());
}

CJS_Result CJS_Event::set_target(CJS_Runtime* pRuntime,
                                 v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_target_name(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(
      pRuntime->NewString(pEvent->TargetName().AsStringView()));
}

CJS_Result CJS_Event::set_target_name(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_type(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  return CJS_Result::Success(pRuntime->NewString(pEvent->Type()));
}

CJS_Result CJS_Event::set_type(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Event::get_value(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  if (pEvent->Type() != "Field")
    return CJS_Result::Failure(L"Bad event type.");

  if (!pEvent->m_pValue)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  return CJS_Result::Success(
      pRuntime->NewString(pEvent->Value().AsStringView()));
}

CJS_Result CJS_Event::set_value(CJS_Runtime* pRuntime,
                                v8::Local<v8::Value> vp) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();
  if (pEvent->Type() != "Field")
    return CJS_Result::Failure(L"Bad event type.");

  if (!pEvent->m_pValue)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  if (vp.IsEmpty())
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  if (vp->IsNullOrUndefined() || vp->IsBoolean())
    return CJS_Result::Failure(JSMessage::kInvalidSetError);

  pEvent->Value() = pRuntime->ToWideString(vp);
  return CJS_Result::Success();
}

CJS_Result CJS_Event::get_will_commit(CJS_Runtime* pRuntime) {
  CJS_EventHandler* pEvent =
      pRuntime->GetCurrentEventContext()->GetEventHandler();

  return CJS_Result::Success(pRuntime->NewBoolean(pEvent->WillCommit()));
}

CJS_Result CJS_Event::set_will_commit(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}
