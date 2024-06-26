// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_operation.h"

#include "xfa/fxfa/parser/cxfa_operation.h"

CJX_Operation::CJX_Operation(CXFA_Operation* node) : CJX_TextNode(node) {}

CJX_Operation::~CJX_Operation() = default;
