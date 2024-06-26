// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_PatternDict.h"


CJBig2_PatternDict::CJBig2_PatternDict(uint32_t dict_size)
    : NUMPATS(dict_size), HDPATS(dict_size) {}

CJBig2_PatternDict::~CJBig2_PatternDict() {}
