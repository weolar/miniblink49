// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_themepart.h"

CFWL_ThemePart::CFWL_ThemePart()
    : m_pWidget(nullptr),
      m_iPart(CFWL_Part::None),
      m_dwStates(CFWL_PartState_Normal),
      m_bMaximize(false),
      m_bStaticBackground(false),
      m_pRtData(nullptr) {}
