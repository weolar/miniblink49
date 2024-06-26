// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxgraphics/cxfa_gepattern.h"

CXFA_GEPattern::CXFA_GEPattern(FX_HatchStyle hatchStyle,
                               const FX_ARGB foreArgb,
                               const FX_ARGB backArgb)
    : m_hatchStyle(hatchStyle), m_foreArgb(foreArgb), m_backArgb(backArgb) {}

CXFA_GEPattern::~CXFA_GEPattern() {}
