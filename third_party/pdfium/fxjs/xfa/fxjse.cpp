// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/fxjse.h"

namespace pdfium {
namespace fxjse {

const char kFuncTag[] = "function descriptor tag";
const char kClassTag[] = "class descriptor tag";

}  // namespace fxjse
}  // namespace pdfium

CFXJSE_HostObject::CFXJSE_HostObject() = default;

CFXJSE_HostObject::~CFXJSE_HostObject() = default;

CFXJSE_FormCalcContext* CFXJSE_HostObject::AsFormCalcContext() {
  return nullptr;
}

CXFA_Object* CFXJSE_HostObject::AsCXFAObject() {
  return nullptr;
}
