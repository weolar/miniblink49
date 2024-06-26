// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_deltas.h"

#include "fxjs/xfa/cjx_deltas.h"
#include "third_party/base/ptr_util.h"

CXFA_Deltas::CXFA_Deltas(CXFA_Document* doc)
    : CXFA_List(doc, pdfium::MakeUnique<CJX_Deltas>(this)) {}

CXFA_Deltas::~CXFA_Deltas() {}
