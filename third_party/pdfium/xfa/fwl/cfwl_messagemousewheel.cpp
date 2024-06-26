// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_messagemousewheel.h"

#include <memory>

#include "third_party/base/ptr_util.h"

CFWL_MessageMouseWheel::CFWL_MessageMouseWheel(CFWL_Widget* pSrcTarget,
                                               CFWL_Widget* pDstTarget)
    : CFWL_Message(CFWL_Message::Type::MouseWheel, pSrcTarget, pDstTarget) {}

CFWL_MessageMouseWheel::CFWL_MessageMouseWheel(const CFWL_MessageMouseWheel&) =
    default;

CFWL_MessageMouseWheel::~CFWL_MessageMouseWheel() {}

std::unique_ptr<CFWL_Message> CFWL_MessageMouseWheel::Clone() {
  return pdfium::MakeUnique<CFWL_MessageMouseWheel>(*this);
}
