// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_CBC_QRCODE_H_
#define FXBARCODE_CBC_QRCODE_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "fxbarcode/cbc_codebase.h"

class CBC_QRCodeWriter;

class CBC_QRCode final : public CBC_CodeBase {
 public:
  CBC_QRCode();
  ~CBC_QRCode() override;

  // CBC_CodeBase:
  bool Encode(WideStringView contents) override;
  bool RenderDevice(CFX_RenderDevice* device,
                    const CFX_Matrix* matrix) override;
  BC_TYPE GetType() override;

 private:
  CBC_QRCodeWriter* GetQRCodeWriter();
};

#endif  // FXBARCODE_CBC_QRCODE_H_
