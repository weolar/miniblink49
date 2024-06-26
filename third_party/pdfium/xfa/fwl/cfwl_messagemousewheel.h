// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_MESSAGEMOUSEWHEEL_H_
#define XFA_FWL_CFWL_MESSAGEMOUSEWHEEL_H_

#include <memory>

#include "core/fxcrt/fx_coordinates.h"
#include "xfa/fwl/cfwl_message.h"

class CFWL_MessageMouseWheel final : public CFWL_Message {
 public:
  CFWL_MessageMouseWheel(CFWL_Widget* pSrcTarget, CFWL_Widget* pDstTarget);
  CFWL_MessageMouseWheel(const CFWL_MessageMouseWheel&);
  ~CFWL_MessageMouseWheel() override;

  // CFWL_Message
  std::unique_ptr<CFWL_Message> Clone() override;

  CFX_PointF m_pos;
  CFX_PointF m_delta;
  uint32_t m_dwFlags;
};

#endif  // XFA_FWL_CFWL_MESSAGEMOUSEWHEEL_H_
