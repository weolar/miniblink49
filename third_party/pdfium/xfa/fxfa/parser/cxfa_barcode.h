// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_BARCODE_H_
#define XFA_FXFA_PARSER_CXFA_BARCODE_H_

#include "fxbarcode/BC_Library.h"
#include "third_party/base/optional.h"
#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_Barcode final : public CXFA_Node {
 public:
  CXFA_Barcode(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Barcode() override;

  XFA_FFWidgetType GetDefaultFFWidgetType() const override;

  WideString GetBarcodeType();
  Optional<BC_CHAR_ENCODING> GetCharEncoding();
  Optional<bool> GetChecksum();
  Optional<int32_t> GetDataLength();
  Optional<char> GetStartChar();
  Optional<char> GetEndChar();
  Optional<int32_t> GetECLevel();
  Optional<int32_t> GetModuleWidth();
  Optional<int32_t> GetModuleHeight();
  Optional<bool> GetPrintChecksum();
  Optional<BC_TEXT_LOC> GetTextLocation();
  Optional<bool> GetTruncate();
  Optional<int8_t> GetWideNarrowRatio();
};

#endif  // XFA_FXFA_PARSER_CXFA_BARCODE_H_
