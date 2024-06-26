// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_traversal.h"

#include "xfa/fxfa/parser/cxfa_traversal.h"

CJX_Traversal::CJX_Traversal(CXFA_Traversal* node) : CJX_Node(node) {}

CJX_Traversal::~CJX_Traversal() = default;
