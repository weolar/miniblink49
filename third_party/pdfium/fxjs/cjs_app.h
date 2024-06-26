// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_APP_H_
#define FXJS_CJS_APP_H_

#include <memory>
#include <set>
#include <vector>

#include "fxjs/cjs_object.h"
#include "fxjs/js_define.h"

class CJS_Runtime;
class GlobalTimer;

class CJS_App final : public CJS_Object {
 public:
  static int GetObjDefnID();
  static void DefineJSObjects(CFXJS_Engine* pEngine);

  CJS_App(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime);
  ~CJS_App() override;

  void TimerProc(GlobalTimer* pTimer);
  void CancelProc(GlobalTimer* pTimer);

  static WideString SysPathToPDFPath(const WideString& sOldPath);

  JS_STATIC_PROP(activeDocs, active_docs, CJS_App);
  JS_STATIC_PROP(calculate, calculate, CJS_App);
  JS_STATIC_PROP(formsVersion, forms_version, CJS_App);
  JS_STATIC_PROP(fs, fs, CJS_App);
  JS_STATIC_PROP(fullscreen, fullscreen, CJS_App);
  JS_STATIC_PROP(language, language, CJS_App);
  JS_STATIC_PROP(media, media, CJS_App);
  JS_STATIC_PROP(platform, platform, CJS_App);
  JS_STATIC_PROP(runtimeHighlight, runtime_highlight, CJS_App);
  JS_STATIC_PROP(viewerType, viewer_type, CJS_App);
  JS_STATIC_PROP(viewerVariation, viewer_variation, CJS_App);
  JS_STATIC_PROP(viewerVersion, viewer_version, CJS_App);

  JS_STATIC_METHOD(alert, CJS_App);
  JS_STATIC_METHOD(beep, CJS_App);
  JS_STATIC_METHOD(browseForDoc, CJS_App);
  JS_STATIC_METHOD(clearInterval, CJS_App);
  JS_STATIC_METHOD(clearTimeOut, CJS_App);
  JS_STATIC_METHOD(execDialog, CJS_App);
  JS_STATIC_METHOD(execMenuItem, CJS_App);
  JS_STATIC_METHOD(findComponent, CJS_App);
  JS_STATIC_METHOD(goBack, CJS_App);
  JS_STATIC_METHOD(goForward, CJS_App);
  JS_STATIC_METHOD(launchURL, CJS_App);
  JS_STATIC_METHOD(mailMsg, CJS_App);
  JS_STATIC_METHOD(newFDF, CJS_App);
  JS_STATIC_METHOD(newDoc, CJS_App);
  JS_STATIC_METHOD(openDoc, CJS_App);
  JS_STATIC_METHOD(openFDF, CJS_App);
  JS_STATIC_METHOD(popUpMenuEx, CJS_App);
  JS_STATIC_METHOD(popUpMenu, CJS_App);
  JS_STATIC_METHOD(response, CJS_App);
  JS_STATIC_METHOD(setInterval, CJS_App);
  JS_STATIC_METHOD(setTimeOut, CJS_App);

 private:
  static int ObjDefnID;
  static const char kName[];
  static const JSPropertySpec PropertySpecs[];
  static const JSMethodSpec MethodSpecs[];

  CJS_Result get_active_docs(CJS_Runtime* pRuntime);
  CJS_Result set_active_docs(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_calculate(CJS_Runtime* pRuntime);
  CJS_Result set_calculate(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_forms_version(CJS_Runtime* pRuntime);
  CJS_Result set_forms_version(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_fs(CJS_Runtime* pRuntime);
  CJS_Result set_fs(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_fullscreen(CJS_Runtime* pRuntime);
  CJS_Result set_fullscreen(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_language(CJS_Runtime* pRuntime);
  CJS_Result set_language(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_media(CJS_Runtime* pRuntime);
  CJS_Result set_media(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_platform(CJS_Runtime* pRuntime);
  CJS_Result set_platform(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_runtime_highlight(CJS_Runtime* pRuntime);
  CJS_Result set_runtime_highlight(CJS_Runtime* pRuntime,
                                   v8::Local<v8::Value> vp);

  CJS_Result get_viewer_type(CJS_Runtime* pRuntime);
  CJS_Result set_viewer_type(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_viewer_variation(CJS_Runtime* pRuntime);
  CJS_Result set_viewer_variation(CJS_Runtime* pRuntime,
                                  v8::Local<v8::Value> vp);

  CJS_Result get_viewer_version(CJS_Runtime* pRuntime);
  CJS_Result set_viewer_version(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result alert(CJS_Runtime* pRuntime,
                   const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result beep(CJS_Runtime* pRuntime,
                  const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result browseForDoc(CJS_Runtime* pRuntime,
                          const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result clearInterval(CJS_Runtime* pRuntime,
                           const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result clearTimeOut(CJS_Runtime* pRuntime,
                          const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result execDialog(CJS_Runtime* pRuntime,
                        const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result execMenuItem(CJS_Runtime* pRuntime,
                          const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result findComponent(CJS_Runtime* pRuntime,
                           const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result goBack(CJS_Runtime* pRuntime,
                    const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result goForward(CJS_Runtime* pRuntime,
                       const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result launchURL(CJS_Runtime* pRuntime,
                       const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result mailMsg(CJS_Runtime* pRuntime,
                     const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result newFDF(CJS_Runtime* pRuntime,
                    const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result newDoc(CJS_Runtime* pRuntime,
                    const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result openDoc(CJS_Runtime* pRuntime,
                     const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result openFDF(CJS_Runtime* pRuntime,
                     const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result popUpMenuEx(CJS_Runtime* pRuntime,
                         const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result popUpMenu(CJS_Runtime* pRuntime,
                       const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result response(CJS_Runtime* pRuntime,
                      const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result setInterval(CJS_Runtime* pRuntime,
                         const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result setTimeOut(CJS_Runtime* pRuntime,
                        const std::vector<v8::Local<v8::Value>>& params);

  void RunJsScript(CJS_Runtime* pRuntime, const WideString& wsScript);
  void ClearTimerCommon(CJS_Runtime* pRuntime, v8::Local<v8::Value> param);

  bool m_bCalculate = true;
  bool m_bRuntimeHighLight = false;
  std::set<std::unique_ptr<GlobalTimer>> m_Timers;
};

#endif  // FXJS_CJS_APP_H_
