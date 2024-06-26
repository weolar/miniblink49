// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_ONED_BC_ONEDIMWRITER_H_
#define FXBARCODE_ONED_BC_ONEDIMWRITER_H_

#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"
#include "fxbarcode/BC_Library.h"
#include "fxbarcode/BC_Writer.h"
#include "fxbarcode/utils.h"

class CFX_Font;
class CFX_PathData;
class CFX_RenderDevice;
class FXTEXT_CHARPOS;

class CBC_OneDimWriter : public CBC_Writer {
 public:
  CBC_OneDimWriter();
  ~CBC_OneDimWriter() override;

  virtual bool RenderResult(WideStringView contents,
                            uint8_t* code,
                            int32_t codeLength);
  virtual bool CheckContentValidity(WideStringView contents) = 0;
  virtual WideString FilterContents(WideStringView contents) = 0;
  virtual WideString RenderTextContents(WideStringView contents);
  virtual void SetPrintChecksum(bool checksum);
  virtual void SetDataLength(int32_t length);
  virtual void SetCalcChecksum(bool state);
  virtual void SetFontSize(float size);
  virtual void SetFontStyle(int32_t style);
  virtual void SetFontColor(FX_ARGB color);

  uint8_t* Encode(const ByteString& contents,
                  BCFORMAT format,
                  int32_t& outWidth,
                  int32_t& outHeight);
  bool RenderDeviceResult(CFX_RenderDevice* device,
                          const CFX_Matrix* matrix,
                          WideStringView contents);
  bool SetFont(CFX_Font* cFont);

 protected:
  virtual uint8_t* EncodeWithHint(const ByteString& contents,
                                  BCFORMAT format,
                                  int32_t& outWidth,
                                  int32_t& outHeight,
                                  int32_t hints);
  virtual uint8_t* EncodeImpl(const ByteString& contents,
                              int32_t& outLength) = 0;
  virtual bool ShowChars(WideStringView contents,
                         CFX_RenderDevice* device,
                         const CFX_Matrix* matrix,
                         int32_t barWidth,
                         int32_t multiple);
  void ShowDeviceChars(CFX_RenderDevice* device,
                       const CFX_Matrix* matrix,
                       const ByteString str,
                       float geWidth,
                       FXTEXT_CHARPOS* pCharPos,
                       float locX,
                       float locY,
                       int32_t barWidth);
  void CalcTextInfo(const ByteString& text,
                    FXTEXT_CHARPOS* charPos,
                    CFX_Font* cFont,
                    float geWidth,
                    int32_t fontSize,
                    float& charsLen);
  int32_t AppendPattern(uint8_t* target,
                        int32_t pos,
                        const int8_t* pattern,
                        int32_t patternLength,
                        bool startColor);

  void RenderVerticalBars(int32_t outputX, int32_t width, int32_t height);

  bool m_bPrintChecksum = true;
  bool m_bCalcChecksum = false;
  bool m_bLeftPadding = false;
  bool m_bRightPadding = false;

  UnownedPtr<CFX_Font> m_pFont;
  float m_fFontSize = 10.0f;
  int32_t m_iFontStyle = 0;
  uint32_t m_fontColor = 0xff000000;
  BC_TEXT_LOC m_locTextLoc = BC_TEXT_LOC_BELOWEMBED;

  int32_t m_iDataLenth = 0;
  size_t m_iContentLen = 0;

  std::vector<CFX_PathData> m_output;
  int32_t m_barWidth;
  int32_t m_multiple;
  float m_outputHScale;
};

#endif  // FXBARCODE_ONED_BC_ONEDIMWRITER_H_
