// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_MESSAGEMOUSE_H_
#define XFA_FWL_CFWL_MESSAGEMOUSE_H_

#include <memory>

#include "core/fxcrt/fx_coordinates.h"
#include "xfa/fwl/cfwl_message.h"

enum class FWL_MouseCommand {
  LeftButtonDown,
  LeftButtonUp,
  LeftButtonDblClk,
  RightButtonDown,
  RightButtonUp,
  RightButtonDblClk,
  Move,
  Enter,
  Leave,
  Hover
};

class CFWL_MessageMouse final : public CFWL_Message {
 public:
  CFWL_MessageMouse(CFWL_Widget* pSrcTarget, CFWL_Widget* pDstTarget);
  CFWL_MessageMouse(const CFWL_MessageMouse& other);
  ~CFWL_MessageMouse() override;

  // CFWL_Message
  std::unique_ptr<CFWL_Message> Clone() override;

  CFX_PointF m_pos;
  uint32_t m_dwFlags;
  FWL_MouseCommand m_dwCmd;
};

#endif  // XFA_FWL_CFWL_MESSAGEMOUSE_H_
