// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_eventmouse.h"

CFWL_EventMouse::CFWL_EventMouse(CFWL_Widget* pSrcTarget)
    : CFWL_EventMouse(pSrcTarget, nullptr) {}

CFWL_EventMouse::CFWL_EventMouse(CFWL_Widget* pSrcTarget,
                                 CFWL_Widget* pDstTarget)
    : CFWL_Event(CFWL_Event::Type::Mouse, pSrcTarget, pDstTarget) {}

CFWL_EventMouse::~CFWL_EventMouse() {}
