// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_Segment.h"


CJBig2_Segment::CJBig2_Segment()
    : m_dwNumber(0),
      m_nReferred_to_segment_count(0),
      m_dwPage_association(0),
      m_dwData_length(0),
      m_dwHeader_Length(0),
      m_dwObjNum(0),
      m_dwDataOffset(0),
      m_State(JBIG2_SEGMENT_HEADER_UNPARSED),
      m_nResultType(JBIG2_VOID_POINTER) {
  m_cFlags.c = 0;
}

CJBig2_Segment::~CJBig2_Segment() {}
