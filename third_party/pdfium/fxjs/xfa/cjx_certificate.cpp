// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_certificate.h"

#include "xfa/fxfa/parser/cxfa_certificate.h"

CJX_Certificate::CJX_Certificate(CXFA_Certificate* node) : CJX_TextNode(node) {}

CJX_Certificate::~CJX_Certificate() = default;
