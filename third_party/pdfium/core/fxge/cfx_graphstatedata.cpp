// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/cfx_graphstatedata.h"

CFX_GraphStateData::CFX_GraphStateData() = default;

CFX_GraphStateData::CFX_GraphStateData(const CFX_GraphStateData& src) {
  *this = src;
}

CFX_GraphStateData::~CFX_GraphStateData() = default;

CFX_GraphStateData& CFX_GraphStateData::operator=(
    const CFX_GraphStateData& that) {
  if (this != &that) {
    m_LineCap = that.m_LineCap;
    m_LineJoin = that.m_LineJoin;
    m_DashPhase = that.m_DashPhase;
    m_MiterLimit = that.m_MiterLimit;
    m_LineWidth = that.m_LineWidth;
    m_DashArray = that.m_DashArray;
  }
  return *this;
}
