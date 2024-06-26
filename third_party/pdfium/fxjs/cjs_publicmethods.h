// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_PUBLICMETHODS_H_
#define FXJS_CJS_PUBLICMETHODS_H_

#include <vector>

#include "fxjs/cjs_object.h"
#include "fxjs/cjs_result.h"

class CJS_PublicMethods final : public CJS_Object {
 public:
  CJS_PublicMethods() = delete;

  static void DefineJSObjects(CFXJS_Engine* pEngine);

  static double ParseDate(const WideString& value, bool* bWrongFormat);
  static double ParseDateAsGMT(const WideString& value);
  static double ParseDateUsingFormat(const WideString& value,
                                     const WideString& format,
                                     bool* bWrongFormat);

  // Exposed for testing.
  static WideString PrintDateUsingFormat(double dDate,
                                         const WideString& format);
  static bool IsNumber(const WideString& str);

  static CJS_Result AFNumber_Format(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFNumber_Keystroke(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFPercent_Format(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFPercent_Keystroke(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFDate_FormatEx(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFDate_KeystrokeEx(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFDate_Format(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFDate_Keystroke(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFTime_FormatEx(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFTime_KeystrokeEx(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFTime_Format(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFTime_Keystroke(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFSpecial_Format(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFSpecial_Keystroke(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFSpecial_KeystrokeEx(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFSimple(CJS_Runtime* pRuntime,
                             const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFMakeNumber(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFSimple_Calculate(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFRange_Validate(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFMergeChange(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFParseDateEx(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  static CJS_Result AFExtractNums(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);

  static void AFNumber_Format_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFNumber_Keystroke_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFPercent_Format_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFPercent_Keystroke_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFDate_FormatEx_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFDate_KeystrokeEx_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFDate_Format_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFDate_Keystroke_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFTime_FormatEx_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFTime_KeystrokeEx_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFTime_Format_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFTime_Keystroke_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFSpecial_Format_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFSpecial_Keystroke_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFSpecial_KeystrokeEx_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFSimple_static(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFMakeNumber_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFSimple_Calculate_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFRange_Validate_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFMergeChange_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFParseDateEx_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AFExtractNums_static(
      const v8::FunctionCallbackInfo<v8::Value>& info);

  static bool MaskSatisfied(wchar_t c_Change, wchar_t c_Mask);
  static bool IsReservedMaskChar(wchar_t ch);
  static v8::Local<v8::Array> AF_MakeArrayFromList(CJS_Runtime* pRuntime,
                                                   v8::Local<v8::Value> val);

 private:
  static const JSMethodSpec GlobalFunctionSpecs[];
};

#endif  // FXJS_CJS_PUBLICMETHODS_H_
