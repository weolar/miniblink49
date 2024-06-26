// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxgraphics/cxfa_gecolor.h"

CXFA_GEColor::CXFA_GEColor() = default;

CXFA_GEColor::CXFA_GEColor(const FX_ARGB argb) : m_type(Solid), m_argb(argb) {}

CXFA_GEColor::CXFA_GEColor(CXFA_GEPattern* pattern, const FX_ARGB argb)
    : m_type(Pattern), m_argb(argb), m_pPattern(pattern) {}

CXFA_GEColor::CXFA_GEColor(CXFA_GEShading* shading)
    : m_type(Shading), m_pShading(shading) {}

CXFA_GEColor::CXFA_GEColor(const CXFA_GEColor& that) = default;

CXFA_GEColor::~CXFA_GEColor() = default;

CXFA_GEColor& CXFA_GEColor::operator=(const CXFA_GEColor& that) = default;
