// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFX_BARCODE_H_
#define XFA_FWL_CFX_BARCODE_H_

#include <memory>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxge/fx_dib.h"
#include "fxbarcode/BC_Library.h"

class CBC_CodeBase;
class CFX_Font;
class CFX_RenderDevice;
class CFX_Matrix;

class CFX_Barcode {
 public:
  ~CFX_Barcode();

  static std::unique_ptr<CFX_Barcode> Create(BC_TYPE type);
  BC_TYPE GetType();
  bool Encode(WideStringView contents);

  bool RenderDevice(CFX_RenderDevice* device, const CFX_Matrix* matrix);

  bool SetCharEncoding(BC_CHAR_ENCODING encoding);

  bool SetModuleHeight(int32_t moduleHeight);
  bool SetModuleWidth(int32_t moduleWidth);

  bool SetHeight(int32_t height);
  bool SetWidth(int32_t width);

  bool SetPrintChecksum(bool checksum);
  bool SetDataLength(int32_t length);
  bool SetCalChecksum(bool state);

  bool SetFont(CFX_Font* pFont);
  bool SetFontSize(float size);
  bool SetFontColor(FX_ARGB color);

  bool SetTextLocation(BC_TEXT_LOC location);

  bool SetWideNarrowRatio(int8_t ratio);
  bool SetStartChar(char start);
  bool SetEndChar(char end);
  bool SetErrorCorrectionLevel(int32_t level);

 private:
  CFX_Barcode();

  std::unique_ptr<CBC_CodeBase> m_pBCEngine;
};

#endif  // XFA_FWL_CFX_BARCODE_H_
