// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_wsdlconnection.h"

#include <vector>

#include "fxjs/cfx_v8.h"
#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/parser/cxfa_wsdlconnection.h"

const CJX_MethodSpec CJX_WsdlConnection::MethodSpecs[] = {
    {"execute", execute_static}};

CJX_WsdlConnection::CJX_WsdlConnection(CXFA_WsdlConnection* connection)
    : CJX_Node(connection) {
  DefineMethods(MethodSpecs);
}

CJX_WsdlConnection::~CJX_WsdlConnection() {}

bool CJX_WsdlConnection::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CJS_Result CJX_WsdlConnection::execute(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty() && params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  return CJS_Result::Success(runtime->NewBoolean(false));
}
