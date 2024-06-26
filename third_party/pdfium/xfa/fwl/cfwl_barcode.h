// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_BARCODE_H_
#define XFA_FWL_CFWL_BARCODE_H_

#include <memory>

#include "fxbarcode/BC_Library.h"
#include "xfa/fwl/cfwl_edit.h"
#include "xfa/fwl/cfwl_scrollbar.h"
#include "xfa/fwl/cfwl_widget.h"

class CFWL_WidgetProperties;
class CFX_Barcode;
class CFWL_Widget;

#define XFA_BCS_NeedUpdate 0x0001
#define XFA_BCS_EncodeSuccess 0x0002

enum FWL_BCDAttribute {
  FWL_BCDATTRIBUTE_NONE = 0,
  FWL_BCDATTRIBUTE_CHARENCODING = 1 << 0,
  FWL_BCDATTRIBUTE_MODULEHEIGHT = 1 << 1,
  FWL_BCDATTRIBUTE_MODULEWIDTH = 1 << 2,
  FWL_BCDATTRIBUTE_DATALENGTH = 1 << 3,
  FWL_BCDATTRIBUTE_CALCHECKSUM = 1 << 4,
  FWL_BCDATTRIBUTE_PRINTCHECKSUM = 1 << 5,
  FWL_BCDATTRIBUTE_TEXTLOCATION = 1 << 6,
  FWL_BCDATTRIBUTE_WIDENARROWRATIO = 1 << 7,
  FWL_BCDATTRIBUTE_STARTCHAR = 1 << 8,
  FWL_BCDATTRIBUTE_ENDCHAR = 1 << 9,
  FWL_BCDATTRIBUTE_ECLEVEL = 1 << 10,
};

class CFWL_Barcode final : public CFWL_Edit {
 public:
  explicit CFWL_Barcode(const CFWL_App* pApp);
  ~CFWL_Barcode() override;

  // CFWL_Widget
  FWL_Type GetClassID() const override;
  void Update() override;
  void DrawWidget(CXFA_Graphics* pGraphics, const CFX_Matrix& matrix) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;

  // CFWL_Edit
  void SetText(
      const WideString& wsText,
      CFDE_TextEditEngine::RecordOperation op =
          CFDE_TextEditEngine::RecordOperation::kInsertRecord) override;

  void SetType(BC_TYPE type);
  bool IsProtectedType() const;

  void SetCharEncoding(BC_CHAR_ENCODING encoding);
  void SetModuleHeight(int32_t height);
  void SetModuleWidth(int32_t width);
  void SetDataLength(int32_t dataLength);
  void SetCalChecksum(bool calChecksum);
  void SetPrintChecksum(bool printChecksum);
  void SetTextLocation(BC_TEXT_LOC location);
  void SetWideNarrowRatio(int8_t ratio);
  void SetStartChar(char startChar);
  void SetEndChar(char endChar);
  void SetErrorCorrectionLevel(int32_t ecLevel);

 private:
  void GenerateBarcodeImageCache();
  void CreateBarcodeEngine();

  std::unique_ptr<CFX_Barcode> m_pBarcodeEngine;
  uint32_t m_dwStatus;
  BC_TYPE m_type;
  BC_CHAR_ENCODING m_eCharEncoding;
  int32_t m_nModuleHeight;
  int32_t m_nModuleWidth;
  int32_t m_nDataLength;
  bool m_bCalChecksum;
  bool m_bPrintChecksum;
  BC_TEXT_LOC m_eTextLocation;
  int8_t m_nWideNarrowRatio;
  char m_cStartChar;
  char m_cEndChar;
  int32_t m_nECLevel;
  uint32_t m_dwAttributeMask;
};

#endif  // XFA_FWL_CFWL_BARCODE_H_
