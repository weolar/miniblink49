// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_MESSAGEKEY_H_
#define XFA_FWL_CFWL_MESSAGEKEY_H_

#include <memory>

#include "xfa/fwl/cfwl_message.h"

enum class FWL_KeyCommand { KeyDown, KeyUp, Char };

class CFWL_MessageKey final : public CFWL_Message {
 public:
  CFWL_MessageKey(CFWL_Widget* pSrcTarget, CFWL_Widget* pDstTarget);
  CFWL_MessageKey(const CFWL_MessageKey& that);
  ~CFWL_MessageKey() override;

  // CFWL_Message
  std::unique_ptr<CFWL_Message> Clone() override;

  uint32_t m_dwKeyCode;
  uint32_t m_dwFlags;
  FWL_KeyCommand m_dwCmd;
};

#endif  // XFA_FWL_CFWL_MESSAGEKEY_H_
