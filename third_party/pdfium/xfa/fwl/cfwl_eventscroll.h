// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_EVENTSCROLL_H_
#define XFA_FWL_CFWL_EVENTSCROLL_H_

#include "xfa/fwl/cfwl_event.h"

class CFWL_EventScroll final : public CFWL_Event {
 public:
  enum class Code {
    None = 1,
    Min,
    Max,
    PageBackward,
    PageForward,
    StepBackward,
    StepForward,
    Pos,
    TrackPos,
    EndScroll,
  };

  explicit CFWL_EventScroll(CFWL_Widget* pSrcTarget);
  ~CFWL_EventScroll() override;

  Code m_iScrollCode;
  float m_fPos;
};

#endif  // XFA_FWL_CFWL_EVENTSCROLL_H_
