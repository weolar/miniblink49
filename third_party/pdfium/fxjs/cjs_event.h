// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_EVENT_H_
#define FXJS_CJS_EVENT_H_

#include "fxjs/cjs_object.h"
#include "fxjs/js_define.h"

class CJS_Event final : public CJS_Object {
 public:
  static int GetObjDefnID();
  static void DefineJSObjects(CFXJS_Engine* pEngine);

  CJS_Event(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime);
  ~CJS_Event() override;

  JS_STATIC_PROP(change, change, CJS_Event);
  JS_STATIC_PROP(changeEx, change_ex, CJS_Event);
  JS_STATIC_PROP(commitKey, commit_key, CJS_Event);
  JS_STATIC_PROP(fieldFull, field_full, CJS_Event);
  JS_STATIC_PROP(keyDown, key_down, CJS_Event);
  JS_STATIC_PROP(modifier, modifier, CJS_Event);
  JS_STATIC_PROP(name, name, CJS_Event);
  JS_STATIC_PROP(rc, rc, CJS_Event);
  JS_STATIC_PROP(richChange, rich_change, CJS_Event);
  JS_STATIC_PROP(richChangeEx, rich_change_ex, CJS_Event);
  JS_STATIC_PROP(richValue, rich_value, CJS_Event);
  JS_STATIC_PROP(selEnd, sel_end, CJS_Event);
  JS_STATIC_PROP(selStart, sel_start, CJS_Event);
  JS_STATIC_PROP(shift, shift, CJS_Event);
  JS_STATIC_PROP(source, source, CJS_Event);
  JS_STATIC_PROP(target, target, CJS_Event);
  JS_STATIC_PROP(targetName, target_name, CJS_Event);
  JS_STATIC_PROP(type, type, CJS_Event);
  JS_STATIC_PROP(value, value, CJS_Event);
  JS_STATIC_PROP(willCommit, will_commit, CJS_Event);

 private:
  static int ObjDefnID;
  static const char kName[];
  static const JSPropertySpec PropertySpecs[];

  CJS_Result get_change(CJS_Runtime* pRuntime);
  CJS_Result set_change(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_change_ex(CJS_Runtime* pRuntime);
  CJS_Result set_change_ex(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_commit_key(CJS_Runtime* pRuntime);
  CJS_Result set_commit_key(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_field_full(CJS_Runtime* pRuntime);
  CJS_Result set_field_full(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_key_down(CJS_Runtime* pRuntime);
  CJS_Result set_key_down(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_modifier(CJS_Runtime* pRuntime);
  CJS_Result set_modifier(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_name(CJS_Runtime* pRuntime);
  CJS_Result set_name(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_rc(CJS_Runtime* pRuntime);
  CJS_Result set_rc(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_rich_change(CJS_Runtime* pRuntime);
  CJS_Result set_rich_change(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_rich_change_ex(CJS_Runtime* pRuntime);
  CJS_Result set_rich_change_ex(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_rich_value(CJS_Runtime* pRuntime);
  CJS_Result set_rich_value(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_sel_end(CJS_Runtime* pRuntime);
  CJS_Result set_sel_end(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_sel_start(CJS_Runtime* pRuntime);
  CJS_Result set_sel_start(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_shift(CJS_Runtime* pRuntime);
  CJS_Result set_shift(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_source(CJS_Runtime* pRuntime);
  CJS_Result set_source(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_target(CJS_Runtime* pRuntime);
  CJS_Result set_target(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_target_name(CJS_Runtime* pRuntime);
  CJS_Result set_target_name(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_type(CJS_Runtime* pRuntime);
  CJS_Result set_type(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_value(CJS_Runtime* pRuntime);
  CJS_Result set_value(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_will_commit(CJS_Runtime* pRuntime);
  CJS_Result set_will_commit(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);
};

#endif  // FXJS_CJS_EVENT_H_
