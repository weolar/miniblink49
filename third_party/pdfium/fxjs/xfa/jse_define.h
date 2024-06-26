// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_JSE_DEFINE_H_
#define FXJS_XFA_JSE_DEFINE_H_

#include <vector>

#include "fxjs/cjs_result.h"

class CFX_V8;

#define JSE_METHOD(method_name)                                      \
  static CJS_Result method_name##_static(                            \
      CJX_Object* node, CFX_V8* runtime,                             \
      const std::vector<v8::Local<v8::Value>>& params) {             \
    if (!node->DynamicTypeIs(static_type__))                         \
      return CJS_Result::Failure(JSMessage::kBadObjectError);        \
    return static_cast<Type__*>(node)->method_name(runtime, params); \
  }                                                                  \
  CJS_Result method_name(CFX_V8* runtime,                            \
                         const std::vector<v8::Local<v8::Value>>& params)

#define JSE_PROP(prop_name)                                               \
  static void prop_name##_static(CJX_Object* node, CFXJSE_Value* value,   \
                                 bool setting, XFA_Attribute attribute) { \
    if (node->DynamicTypeIs(static_type__))                               \
      static_cast<Type__*>(node)->prop_name(value, setting, attribute);   \
  }                                                                       \
  void prop_name(CFXJSE_Value* pValue, bool bSetting, XFA_Attribute eAttribute)

#endif  // FXJS_XFA_JSE_DEFINE_H_
