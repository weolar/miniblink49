// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_contentarea.h"

#include "xfa/fxfa/parser/cxfa_contentarea.h"

CJX_ContentArea::CJX_ContentArea(CXFA_ContentArea* node)
    : CJX_Container(node) {}

CJX_ContentArea::~CJX_ContentArea() = default;
