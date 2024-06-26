// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/bmp/fx_bmp.h"

static_assert(sizeof(BmpFileHeader) == 14,
              "BmpFileHeader should have a size of 14");

