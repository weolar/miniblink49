// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_font.h"

const JSConstSpec CJS_Font::ConstSpecs[] = {
    {"Times", JSConstSpec::String, 0, "Times-Roman"},
    {"TimesB", JSConstSpec::String, 0, "Times-Bold"},
    {"TimesI", JSConstSpec::String, 0, "Times-Italic"},
    {"TimesBI", JSConstSpec::String, 0, "Times-BoldItalic"},
    {"Helv", JSConstSpec::String, 0, "Helvetica"},
    {"HelvB", JSConstSpec::String, 0, "Helvetica-Bold"},
    {"HelvI", JSConstSpec::String, 0, "Helvetica-Oblique"},
    {"HelvBI", JSConstSpec::String, 0, "Helvetica-BoldOblique"},
    {"Cour", JSConstSpec::String, 0, "Courier"},
    {"CourB", JSConstSpec::String, 0, "Courier-Bold"},
    {"CourI", JSConstSpec::String, 0, "Courier-Oblique"},
    {"CourBI", JSConstSpec::String, 0, "Courier-BoldOblique"},
    {"Symbol", JSConstSpec::String, 0, "Symbol"},
    {"ZapfD", JSConstSpec::String, 0, "ZapfDingbats"}};

int CJS_Font::ObjDefnID = -1;

// static
void CJS_Font::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID = pEngine->DefineObj("font", FXJSOBJTYPE_STATIC, nullptr, nullptr);
  DefineConsts(pEngine, ObjDefnID, ConstSpecs);
}
