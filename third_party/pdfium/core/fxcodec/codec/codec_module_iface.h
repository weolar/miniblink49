// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CODEC_MODULE_IFACE_H_
#define CORE_FXCODEC_CODEC_CODEC_MODULE_IFACE_H_

#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"

class CFX_CodecMemory;
class CFX_DIBAttribute;

class CodecModuleIface {
 public:
  class Context {
   public:
    virtual ~Context() = default;
  };

  virtual ~CodecModuleIface() = default;

  // Returns the number of unprocessed bytes remaining in the input buffer.
  virtual FX_FILESIZE GetAvailInput(Context* pContext) const = 0;

  // Provides a new input buffer to the codec. Returns true on success,
  // setting details about the image extracted from the buffer into |pAttribute|
  // (if provided and the codec is capable providing that information).
  virtual bool Input(Context* pContext,
                     RetainPtr<CFX_CodecMemory> codec_memory,
                     CFX_DIBAttribute* pAttribute) = 0;
};

#endif  // CORE_FXCODEC_CODEC_CODEC_MODULE_IFACE_H_
