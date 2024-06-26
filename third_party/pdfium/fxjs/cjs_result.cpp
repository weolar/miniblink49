// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_result.h"

CJS_Result::CJS_Result() {}

CJS_Result::CJS_Result(v8::Local<v8::Value> ret) : return_(ret) {}

CJS_Result::CJS_Result(const WideString& err) : error_(err) {}

CJS_Result::CJS_Result(JSMessage id) : CJS_Result(JSGetStringFromID(id)) {}

CJS_Result::CJS_Result(const CJS_Result&) = default;

CJS_Result::~CJS_Result() = default;
