// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_CBC_EAN13_H_
#define FXBARCODE_CBC_EAN13_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "fxbarcode/cbc_eancode.h"

class CBC_OnedEAN13Writer;

class CBC_EAN13 final : public CBC_EANCode {
 public:
  CBC_EAN13();
  ~CBC_EAN13() override;

  // CBC_EANCode:
  BC_TYPE GetType() override;
  BCFORMAT GetFormat() const override;
  size_t GetMaxLength() const override;
};

#endif  // FXBARCODE_CBC_EAN13_H_
