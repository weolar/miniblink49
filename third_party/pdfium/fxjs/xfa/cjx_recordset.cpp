// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_recordset.h"

#include <algorithm>

#include "xfa/fxfa/parser/cxfa_recordset.h"

CJX_RecordSet::CJX_RecordSet(CXFA_RecordSet* node) : CJX_Node(node) {}

CJX_RecordSet::~CJX_RecordSet() = default;
