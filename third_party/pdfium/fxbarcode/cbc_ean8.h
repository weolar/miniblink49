// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_CBC_EAN8_H_
#define FXBARCODE_CBC_EAN8_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "fxbarcode/cbc_eancode.h"

class CBC_OnedEAN8Writer;

class CBC_EAN8 final : public CBC_EANCode {
 public:
  CBC_EAN8();
  ~CBC_EAN8() override;

  // CBC_EANCode:
  BC_TYPE GetType() override;
  BCFORMAT GetFormat() const override;
  size_t GetMaxLength() const override;
};

#endif  // FXBARCODE_CBC_EAN8_H_
