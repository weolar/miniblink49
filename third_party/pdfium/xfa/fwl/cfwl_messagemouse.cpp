// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_messagemouse.h"

#include <memory>

#include "third_party/base/ptr_util.h"

CFWL_MessageMouse::CFWL_MessageMouse(CFWL_Widget* pSrcTarget,
                                     CFWL_Widget* pDstTarget)
    : CFWL_Message(CFWL_Message::Type::Mouse, pSrcTarget, pDstTarget) {}

CFWL_MessageMouse::CFWL_MessageMouse(const CFWL_MessageMouse& other) = default;

CFWL_MessageMouse::~CFWL_MessageMouse() {}

std::unique_ptr<CFWL_Message> CFWL_MessageMouse::Clone() {
  return pdfium::MakeUnique<CFWL_MessageMouse>(*this);
}
