// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_messagesetfocus.h"

#include <memory>

#include "third_party/base/ptr_util.h"

CFWL_MessageSetFocus::CFWL_MessageSetFocus(CFWL_Widget* pSrcTarget,
                                           CFWL_Widget* pDstTarget)
    : CFWL_Message(CFWL_Message::Type::SetFocus, pSrcTarget, pDstTarget) {}

CFWL_MessageSetFocus::CFWL_MessageSetFocus(const CFWL_MessageSetFocus& that) =
    default;

CFWL_MessageSetFocus::~CFWL_MessageSetFocus() = default;

std::unique_ptr<CFWL_Message> CFWL_MessageSetFocus::Clone() {
  return pdfium::MakeUnique<CFWL_MessageSetFocus>(*this);
}
