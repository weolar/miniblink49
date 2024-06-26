// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_MESSAGESETFOCUS_H_
#define XFA_FWL_CFWL_MESSAGESETFOCUS_H_

#include <memory>

#include "xfa/fwl/cfwl_message.h"

class CFWL_MessageSetFocus final : public CFWL_Message {
 public:
  CFWL_MessageSetFocus(CFWL_Widget* pSrcTarget, CFWL_Widget* pDstTarget);
  CFWL_MessageSetFocus(const CFWL_MessageSetFocus& that);
  ~CFWL_MessageSetFocus() override;

  // CFWL_Message
  std::unique_ptr<CFWL_Message> Clone() override;
};

#endif  // XFA_FWL_CFWL_MESSAGESETFOCUS_H_
