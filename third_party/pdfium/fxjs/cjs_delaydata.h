// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_DELAYDATA_H_
#define FXJS_CJS_DELAYDATA_H_

#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/widestring.h"
#include "core/fxge/cfx_color.h"
#include "fxjs/cjs_field.h"

struct CJS_DelayData {
  CJS_DelayData(FIELD_PROP prop, int idx, const WideString& name);
  ~CJS_DelayData();

  FIELD_PROP eProp;
  int nControlIndex;
  WideString sFieldName;
  int32_t num;
  bool b;
  ByteString bytestring;
  WideString widestring;
  CFX_FloatRect rect;
  CFX_Color color;
  std::vector<uint32_t> wordarray;
  std::vector<WideString> widestringarray;
};

#endif  // FXJS_CJS_DELAYDATA_H_
