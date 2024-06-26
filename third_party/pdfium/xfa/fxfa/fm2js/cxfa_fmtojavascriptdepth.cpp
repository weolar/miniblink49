// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/fm2js/cxfa_fmtojavascriptdepth.h"

unsigned long CXFA_FMToJavaScriptDepth::depth_ = 0;

void CXFA_FMToJavaScriptDepth::Reset() {
  depth_ = 0;
}
