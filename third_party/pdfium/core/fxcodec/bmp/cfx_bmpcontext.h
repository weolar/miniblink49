// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_BMP_CFX_BMPCONTEXT_H_
#define CORE_FXCODEC_BMP_CFX_BMPCONTEXT_H_

#include "core/fxcodec/bmp/cfx_bmpdecompressor.h"
#include "core/fxcodec/bmp/fx_bmp.h"
#include "core/fxcrt/unowned_ptr.h"

class CFX_BmpContext final : public CodecModuleIface::Context {
 public:
  CFX_BmpContext(CCodec_BmpModule* pModule,
                 CCodec_BmpModule::Delegate* pDelegate);
  ~CFX_BmpContext() override;

  CFX_BmpDecompressor m_Bmp;
  UnownedPtr<CCodec_BmpModule> const m_pModule;
  UnownedPtr<CCodec_BmpModule::Delegate> const m_pDelegate;
};

#endif  // CORE_FXCODEC_BMP_CFX_BMPCONTEXT_H_
