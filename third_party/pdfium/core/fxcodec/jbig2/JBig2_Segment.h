// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_JBIG2_JBIG2_SEGMENT_H_
#define CORE_FXCODEC_JBIG2_JBIG2_SEGMENT_H_

#include <memory>
#include <vector>

#include "core/fxcodec/jbig2/JBig2_Define.h"
#include "core/fxcodec/jbig2/JBig2_HuffmanTable.h"
#include "core/fxcodec/jbig2/JBig2_PatternDict.h"
#include "core/fxcodec/jbig2/JBig2_SymbolDict.h"

enum JBig2_SegmentState {
  JBIG2_SEGMENT_HEADER_UNPARSED,
  JBIG2_SEGMENT_DATA_UNPARSED,
  JBIG2_SEGMENT_PARSE_COMPLETE,
  JBIG2_SEGMENT_PAUSED,
  JBIG2_SEGMENT_ERROR
};

enum JBig2_ResultType {
  JBIG2_VOID_POINTER = 0,
  JBIG2_IMAGE_POINTER,
  JBIG2_SYMBOL_DICT_POINTER,
  JBIG2_PATTERN_DICT_POINTER,
  JBIG2_HUFFMAN_TABLE_POINTER
};

class CJBig2_Segment {
 public:
  CJBig2_Segment();
  ~CJBig2_Segment();

  uint32_t m_dwNumber;
  union {
    struct {
      uint8_t type : 6;
      uint8_t page_association_size : 1;
      uint8_t deferred_non_retain : 1;
    } s;
    uint8_t c;
  } m_cFlags;
  int32_t m_nReferred_to_segment_count;
  std::vector<uint32_t> m_Referred_to_segment_numbers;
  uint32_t m_dwPage_association;
  uint32_t m_dwData_length;

  uint32_t m_dwHeader_Length;
  uint32_t m_dwObjNum;
  uint32_t m_dwDataOffset;
  JBig2_SegmentState m_State;
  JBig2_ResultType m_nResultType;
  std::unique_ptr<CJBig2_SymbolDict> m_SymbolDict;
  std::unique_ptr<CJBig2_PatternDict> m_PatternDict;
  std::unique_ptr<CJBig2_Image> m_Image;
  std::unique_ptr<CJBig2_HuffmanTable> m_HuffmanTable;
};

#endif  // CORE_FXCODEC_JBIG2_JBIG2_SEGMENT_H_
