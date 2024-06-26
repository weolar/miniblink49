// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_messagekey.h"

#include <memory>

#include "third_party/base/ptr_util.h"

CFWL_MessageKey::CFWL_MessageKey(CFWL_Widget* pSrcTarget,
                                 CFWL_Widget* pDstTarget)
    : CFWL_Message(CFWL_Message::Type::Key, pSrcTarget, pDstTarget) {}

CFWL_MessageKey::CFWL_MessageKey(const CFWL_MessageKey& that) = default;

CFWL_MessageKey::~CFWL_MessageKey() = default;

std::unique_ptr<CFWL_Message> CFWL_MessageKey::Clone() {
  return pdfium::MakeUnique<CFWL_MessageKey>(*this);
}
