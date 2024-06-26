// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_event.h"

CFWL_Event::CFWL_Event(CFWL_Event::Type type) : m_type(type) {}

CFWL_Event::CFWL_Event(Type type, CFWL_Widget* pSrcTarget)
    : m_type(type), m_pSrcTarget(pSrcTarget) {}

CFWL_Event::CFWL_Event(Type type,
                       CFWL_Widget* pSrcTarget,
                       CFWL_Widget* pDstTarget)
    : m_type(type), m_pSrcTarget(pSrcTarget), m_pDstTarget(pDstTarget) {}

CFWL_Event::~CFWL_Event() = default;
