// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_CBC_CODEBASE_H_
#define FXBARCODE_CBC_CODEBASE_H_

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxge/fx_dib.h"
#include "fxbarcode/BC_Library.h"

class CBC_Reader;
class CBC_Writer;
class CFX_DIBitmap;
class CFX_Matrix;
class CFX_RenderDevice;

class CBC_CodeBase {
 public:
  explicit CBC_CodeBase(std::unique_ptr<CBC_Writer> pWriter);
  virtual ~CBC_CodeBase();

  virtual BC_TYPE GetType() = 0;
  virtual bool Encode(WideStringView contents) = 0;
  virtual bool RenderDevice(CFX_RenderDevice* device,
                            const CFX_Matrix* matrix) = 0;

  bool SetTextLocation(BC_TEXT_LOC location);
  bool SetWideNarrowRatio(int8_t ratio);
  bool SetStartChar(char start);
  bool SetEndChar(char end);
  bool SetErrorCorrectionLevel(int32_t level);
  bool SetCharEncoding(int32_t encoding);
  bool SetModuleHeight(int32_t moduleHeight);
  bool SetModuleWidth(int32_t moduleWidth);
  bool SetHeight(int32_t height);
  bool SetWidth(int32_t width);
  void SetBackgroundColor(FX_ARGB backgroundColor);
  void SetBarcodeColor(FX_ARGB foregroundColor);

 protected:
  std::unique_ptr<CBC_Writer> m_pBCWriter;
};

#endif  // FXBARCODE_CBC_CODEBASE_H_
