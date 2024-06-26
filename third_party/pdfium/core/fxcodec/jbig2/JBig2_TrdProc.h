// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_JBIG2_JBIG2_TRDPROC_H_
#define CORE_FXCODEC_JBIG2_JBIG2_TRDPROC_H_

#include <memory>
#include <vector>

#include "core/fxcodec/jbig2/JBig2_Image.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CJBig2_ArithDecoder;
class CJBig2_ArithIaidDecoder;
class CJBig2_ArithIntDecoder;
class CJBig2_BitStream;
class CJBig2_HuffmanTable;
class JBig2ArithCtx;
struct JBig2HuffmanCode;

struct JBig2IntDecoderState {
  JBig2IntDecoderState();
  ~JBig2IntDecoderState();

  UnownedPtr<CJBig2_ArithIntDecoder> IADT;
  UnownedPtr<CJBig2_ArithIntDecoder> IAFS;
  UnownedPtr<CJBig2_ArithIntDecoder> IADS;
  UnownedPtr<CJBig2_ArithIntDecoder> IAIT;
  UnownedPtr<CJBig2_ArithIntDecoder> IARI;
  UnownedPtr<CJBig2_ArithIntDecoder> IARDW;
  UnownedPtr<CJBig2_ArithIntDecoder> IARDH;
  UnownedPtr<CJBig2_ArithIntDecoder> IARDX;
  UnownedPtr<CJBig2_ArithIntDecoder> IARDY;
  UnownedPtr<CJBig2_ArithIaidDecoder> IAID;
};

enum JBig2Corner {
  JBIG2_CORNER_BOTTOMLEFT = 0,
  JBIG2_CORNER_TOPLEFT = 1,
  JBIG2_CORNER_BOTTOMRIGHT = 2,
  JBIG2_CORNER_TOPRIGHT = 3
};

class CJBig2_TRDProc {
 public:
  CJBig2_TRDProc();
  ~CJBig2_TRDProc();

  std::unique_ptr<CJBig2_Image> DecodeHuffman(CJBig2_BitStream* pStream,
                                              JBig2ArithCtx* grContext);

  std::unique_ptr<CJBig2_Image> DecodeArith(CJBig2_ArithDecoder* pArithDecoder,
                                            JBig2ArithCtx* grContext,
                                            JBig2IntDecoderState* pIDS);

  bool SBHUFF;
  bool SBREFINE;
  bool SBRTEMPLATE;
  bool TRANSPOSED;
  bool SBDEFPIXEL;
  int8_t SBDSOFFSET;
  uint8_t SBSYMCODELEN;
  uint32_t SBW;
  uint32_t SBH;
  uint32_t SBNUMINSTANCES;
  uint32_t SBSTRIPS;
  uint32_t SBNUMSYMS;
  std::vector<JBig2HuffmanCode> SBSYMCODES;
  CJBig2_Image** SBSYMS;
  JBig2ComposeOp SBCOMBOP;
  JBig2Corner REFCORNER;
  UnownedPtr<const CJBig2_HuffmanTable> SBHUFFFS;
  UnownedPtr<const CJBig2_HuffmanTable> SBHUFFDS;
  UnownedPtr<const CJBig2_HuffmanTable> SBHUFFDT;
  UnownedPtr<const CJBig2_HuffmanTable> SBHUFFRDW;
  UnownedPtr<const CJBig2_HuffmanTable> SBHUFFRDH;
  UnownedPtr<const CJBig2_HuffmanTable> SBHUFFRDX;
  UnownedPtr<const CJBig2_HuffmanTable> SBHUFFRDY;
  UnownedPtr<const CJBig2_HuffmanTable> SBHUFFRSIZE;
  int8_t SBRAT[4];

 private:
  struct ComposeData {
    int32_t x;
    int32_t y;
    uint32_t increment = 0;
  };
  ComposeData GetComposeData(int32_t SI,
                             int32_t TI,
                             uint32_t WI,
                             uint32_t HI) const;
};

#endif  // CORE_FXCODEC_JBIG2_JBIG2_TRDPROC_H_
