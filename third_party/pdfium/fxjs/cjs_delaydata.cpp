// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_delaydata.h"

CJS_DelayData::CJS_DelayData(FIELD_PROP prop, int idx, const WideString& name)
    : eProp(prop), nControlIndex(idx), sFieldName(name) {}

CJS_DelayData::~CJS_DelayData() {}
