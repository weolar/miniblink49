// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_position.h"

const JSConstSpec CJS_Position::ConstSpecs[] = {
    {"textOnly", JSConstSpec::Number, 0, 0},
    {"iconOnly", JSConstSpec::Number, 1, 0},
    {"iconTextV", JSConstSpec::Number, 2, 0},
    {"textIconV", JSConstSpec::Number, 3, 0},
    {"iconTextH", JSConstSpec::Number, 4, 0},
    {"textIconH", JSConstSpec::Number, 5, 0},
    {"overlay", JSConstSpec::Number, 6, 0}};

int CJS_Position::ObjDefnID = -1;

// static
void CJS_Position::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID =
      pEngine->DefineObj("position", FXJSOBJTYPE_STATIC, nullptr, nullptr);
  DefineConsts(pEngine, ObjDefnID, ConstSpecs);
}
