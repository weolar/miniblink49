// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_color.h"

#include <algorithm>
#include <vector>

#include "core/fxge/cfx_color.h"
#include "fxjs/cjs_event_context.h"
#include "fxjs/cjs_eventhandler.h"
#include "fxjs/cjs_object.h"
#include "fxjs/cjs_runtime.h"
#include "fxjs/js_define.h"

const JSPropertySpec CJS_Color::PropertySpecs[] = {
    {"black", get_black_static, set_black_static},
    {"blue", get_blue_static, set_blue_static},
    {"cyan", get_cyan_static, set_cyan_static},
    {"dkGray", get_dark_gray_static, set_dark_gray_static},
    {"gray", get_gray_static, set_gray_static},
    {"green", get_green_static, set_green_static},
    {"ltGray", get_light_gray_static, set_light_gray_static},
    {"magenta", get_magenta_static, set_magenta_static},
    {"red", get_red_static, set_red_static},
    {"transparent", get_transparent_static, set_transparent_static},
    {"white", get_white_static, set_white_static},
    {"yellow", get_yellow_static, set_yellow_static}};

const JSMethodSpec CJS_Color::MethodSpecs[] = {{"convert", convert_static},
                                               {"equal", equal_static}};

int CJS_Color::ObjDefnID = -1;
const char CJS_Color::kName[] = "color";

// static
int CJS_Color::GetObjDefnID() {
  return ObjDefnID;
}

// static
void CJS_Color::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID = pEngine->DefineObj(CJS_Color::kName, FXJSOBJTYPE_STATIC,
                                 JSConstructor<CJS_Color>, JSDestructor);
  DefineProps(pEngine, ObjDefnID, PropertySpecs);
  DefineMethods(pEngine, ObjDefnID, MethodSpecs);
}

// static
v8::Local<v8::Array> CJS_Color::ConvertPWLColorToArray(CJS_Runtime* pRuntime,
                                                       const CFX_Color& color) {
  v8::Local<v8::Array> array;
  switch (color.nColorType) {
    case CFX_Color::kTransparent:
      array = pRuntime->NewArray();
      pRuntime->PutArrayElement(array, 0, pRuntime->NewString("T"));
      break;
    case CFX_Color::kGray:
      array = pRuntime->NewArray();
      pRuntime->PutArrayElement(array, 0, pRuntime->NewString("G"));
      pRuntime->PutArrayElement(array, 1, pRuntime->NewNumber(color.fColor1));
      break;
    case CFX_Color::kRGB:
      array = pRuntime->NewArray();
      pRuntime->PutArrayElement(array, 0, pRuntime->NewString("RGB"));
      pRuntime->PutArrayElement(array, 1, pRuntime->NewNumber(color.fColor1));
      pRuntime->PutArrayElement(array, 2, pRuntime->NewNumber(color.fColor2));
      pRuntime->PutArrayElement(array, 3, pRuntime->NewNumber(color.fColor3));
      break;
    case CFX_Color::kCMYK:
      array = pRuntime->NewArray();
      pRuntime->PutArrayElement(array, 0, pRuntime->NewString("CMYK"));
      pRuntime->PutArrayElement(array, 1, pRuntime->NewNumber(color.fColor1));
      pRuntime->PutArrayElement(array, 2, pRuntime->NewNumber(color.fColor2));
      pRuntime->PutArrayElement(array, 3, pRuntime->NewNumber(color.fColor3));
      pRuntime->PutArrayElement(array, 4, pRuntime->NewNumber(color.fColor4));
      break;
  }
  return array;
}

// static
CFX_Color CJS_Color::ConvertArrayToPWLColor(CJS_Runtime* pRuntime,
                                            v8::Local<v8::Array> array) {
  int nArrayLen = pRuntime->GetArrayLength(array);
  if (nArrayLen < 1)
    return CFX_Color();

  WideString sSpace =
      pRuntime->ToWideString(pRuntime->GetArrayElement(array, 0));
  if (sSpace.EqualsASCII("T"))
    return CFX_Color(CFX_Color::kTransparent);

  float d1 = 0;
  if (nArrayLen > 1) {
    d1 = static_cast<float>(
        pRuntime->ToDouble(pRuntime->GetArrayElement(array, 1)));
  }
  if (sSpace.EqualsASCII("G"))
    return CFX_Color(CFX_Color::kGray, d1);

  float d2 = 0;
  float d3 = 0;
  if (nArrayLen > 2) {
    d2 = static_cast<float>(
        pRuntime->ToDouble(pRuntime->GetArrayElement(array, 2)));
  }
  if (nArrayLen > 3) {
    d3 = static_cast<float>(
        pRuntime->ToDouble(pRuntime->GetArrayElement(array, 3)));
  }
  if (sSpace.EqualsASCII("RGB"))
    return CFX_Color(CFX_Color::kRGB, d1, d2, d3);

  float d4 = 0;
  if (nArrayLen > 4) {
    d4 = static_cast<float>(
        pRuntime->ToDouble(pRuntime->GetArrayElement(array, 4)));
  }
  if (sSpace.EqualsASCII("CMYK"))
    return CFX_Color(CFX_Color::kCMYK, d1, d2, d3, d4);

  return CFX_Color();
}

CJS_Color::CJS_Color(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime)
    : CJS_Object(pObject, pRuntime),
      m_crTransparent(CFX_Color::kTransparent),
      m_crBlack(CFX_Color::kGray, 0),
      m_crWhite(CFX_Color::kGray, 1),
      m_crRed(CFX_Color::kRGB, 1, 0, 0),
      m_crGreen(CFX_Color::kRGB, 0, 1, 0),
      m_crBlue(CFX_Color::kRGB, 0, 0, 1),
      m_crCyan(CFX_Color::kCMYK, 1, 0, 0, 0),
      m_crMagenta(CFX_Color::kCMYK, 0, 1, 0, 0),
      m_crYellow(CFX_Color::kCMYK, 0, 0, 1, 0),
      m_crDKGray(CFX_Color::kGray, 0.25),
      m_crGray(CFX_Color::kGray, 0.5),
      m_crLTGray(CFX_Color::kGray, 0.75) {}

CJS_Color::~CJS_Color() = default;

CJS_Result CJS_Color::get_transparent(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crTransparent);
}

CJS_Result CJS_Color::set_transparent(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crTransparent);
}

CJS_Result CJS_Color::get_black(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crBlack);
}

CJS_Result CJS_Color::set_black(CJS_Runtime* pRuntime,
                                v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crBlack);
}

CJS_Result CJS_Color::get_white(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crWhite);
}

CJS_Result CJS_Color::set_white(CJS_Runtime* pRuntime,
                                v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crWhite);
}

CJS_Result CJS_Color::get_red(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crRed);
}

CJS_Result CJS_Color::set_red(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crRed);
}

CJS_Result CJS_Color::get_green(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crGreen);
}

CJS_Result CJS_Color::set_green(CJS_Runtime* pRuntime,
                                v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crGreen);
}

CJS_Result CJS_Color::get_blue(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crBlue);
}

CJS_Result CJS_Color::set_blue(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crBlue);
}

CJS_Result CJS_Color::get_cyan(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crCyan);
}

CJS_Result CJS_Color::set_cyan(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crCyan);
}

CJS_Result CJS_Color::get_magenta(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crMagenta);
}

CJS_Result CJS_Color::set_magenta(CJS_Runtime* pRuntime,
                                  v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crMagenta);
}

CJS_Result CJS_Color::get_yellow(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crYellow);
}

CJS_Result CJS_Color::set_yellow(CJS_Runtime* pRuntime,
                                 v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crYellow);
}

CJS_Result CJS_Color::get_dark_gray(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crDKGray);
}

CJS_Result CJS_Color::set_dark_gray(CJS_Runtime* pRuntime,
                                    v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crDKGray);
}

CJS_Result CJS_Color::get_gray(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crGray);
}

CJS_Result CJS_Color::set_gray(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crGray);
}

CJS_Result CJS_Color::get_light_gray(CJS_Runtime* pRuntime) {
  return GetPropertyHelper(pRuntime, &m_crLTGray);
}

CJS_Result CJS_Color::set_light_gray(CJS_Runtime* pRuntime,
                                     v8::Local<v8::Value> vp) {
  return SetPropertyHelper(pRuntime, vp, &m_crLTGray);
}

CJS_Result CJS_Color::GetPropertyHelper(CJS_Runtime* pRuntime, CFX_Color* var) {
  v8::Local<v8::Value> array = ConvertPWLColorToArray(pRuntime, *var);
  if (array.IsEmpty())
    return CJS_Result::Success(pRuntime->NewArray());

  return CJS_Result::Success(array);
}

CJS_Result CJS_Color::SetPropertyHelper(CJS_Runtime* pRuntime,
                                        v8::Local<v8::Value> vp,
                                        CFX_Color* var) {
  if (vp.IsEmpty())
    return CJS_Result::Failure(JSMessage::kParamError);

  if (!vp->IsArray())
    return CJS_Result::Failure(JSMessage::kTypeError);

  *var = ConvertArrayToPWLColor(pRuntime, pRuntime->ToArray(vp));
  return CJS_Result::Success();
}

CJS_Result CJS_Color::convert(CJS_Runtime* pRuntime,
                              const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() < 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  if (params[0].IsEmpty() || !params[0]->IsArray())
    return CJS_Result::Failure(JSMessage::kTypeError);

  WideString sDestSpace = pRuntime->ToWideString(params[1]);
  int nColorType = CFX_Color::kTransparent;
  if (sDestSpace.EqualsASCII("T"))
    nColorType = CFX_Color::kTransparent;
  else if (sDestSpace.EqualsASCII("G"))
    nColorType = CFX_Color::kGray;
  else if (sDestSpace.EqualsASCII("RGB"))
    nColorType = CFX_Color::kRGB;
  else if (sDestSpace.EqualsASCII("CMYK"))
    nColorType = CFX_Color::kCMYK;

  CFX_Color color =
      ConvertArrayToPWLColor(pRuntime, pRuntime->ToArray(params[0]));
  v8::Local<v8::Value> array =
      ConvertPWLColorToArray(pRuntime, color.ConvertColorType(nColorType));
  if (array.IsEmpty())
    return CJS_Result::Success(pRuntime->NewArray());

  return CJS_Result::Success(array);
}

CJS_Result CJS_Color::equal(CJS_Runtime* pRuntime,
                            const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() < 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  if (params[0].IsEmpty() || !params[0]->IsArray() || params[1].IsEmpty() ||
      !params[1]->IsArray()) {
    return CJS_Result::Failure(JSMessage::kTypeError);
  }

  CFX_Color color1 =
      ConvertArrayToPWLColor(pRuntime, pRuntime->ToArray(params[0]));
  CFX_Color color2 =
      ConvertArrayToPWLColor(pRuntime, pRuntime->ToArray(params[1]));

  // Relies on higher values having more components.
  int32_t best = std::max(color1.nColorType, color2.nColorType);
  return CJS_Result::Success(pRuntime->NewBoolean(
      color1.ConvertColorType(best) == color2.ConvertColorType(best)));
}
