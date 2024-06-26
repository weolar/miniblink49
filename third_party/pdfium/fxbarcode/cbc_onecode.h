// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_CBC_ONECODE_H_
#define FXBARCODE_CBC_ONECODE_H_

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "fxbarcode/cbc_codebase.h"

class CBC_OneDimWriter;
class CFX_Font;

class CBC_OneCode : public CBC_CodeBase {
 public:
  explicit CBC_OneCode(std::unique_ptr<CBC_Writer> pWriter);
  ~CBC_OneCode() override;

  virtual bool CheckContentValidity(WideStringView contents);
  virtual WideString FilterContents(WideStringView contents);

  void SetPrintChecksum(bool checksum);
  void SetDataLength(int32_t length);
  void SetCalChecksum(bool calc);
  bool SetFont(CFX_Font* cFont);
  void SetFontSize(float size);
  void SetFontStyle(int32_t style);
  void SetFontColor(FX_ARGB color);

 private:
  CBC_OneDimWriter* GetOneDimWriter();
};

#endif  // FXBARCODE_CBC_ONECODE_H_
