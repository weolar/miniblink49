// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_eventselectchanged.h"

CFWL_EventSelectChanged::CFWL_EventSelectChanged(CFWL_Widget* pSrcTarget)
    : CFWL_Event(CFWL_Event::Type::SelectChanged, pSrcTarget) {}

CFWL_EventSelectChanged::~CFWL_EventSelectChanged() {}
