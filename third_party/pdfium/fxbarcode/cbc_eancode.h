// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_CBC_EANCODE_H_
#define FXBARCODE_CBC_EANCODE_H_

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "fxbarcode/cbc_onecode.h"
#include "fxbarcode/utils.h"

class CBC_OneDimEANWriter;

class CBC_EANCode : public CBC_OneCode {
 public:
  explicit CBC_EANCode(std::unique_ptr<CBC_OneDimEANWriter> pWriter);
  ~CBC_EANCode() override;

  virtual BCFORMAT GetFormat() const = 0;
  virtual size_t GetMaxLength() const = 0;

  // CBC_EANCode:
  bool Encode(WideStringView contents) override;
  bool RenderDevice(CFX_RenderDevice* device,
                    const CFX_Matrix* matrix) override;

 protected:
  CBC_OneDimEANWriter* GetOneDimEANWriter();
  WideString Preprocess(WideStringView contents);

  WideString m_renderContents;
};

#endif  // FXBARCODE_CBC_EANCODE_H_
