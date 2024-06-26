// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_EVENTTEXTWILLCHANGE_H_
#define XFA_FWL_CFWL_EVENTTEXTWILLCHANGE_H_

#include "xfa/fwl/cfwl_event.h"

class CFWL_EventTextWillChange final : public CFWL_Event {
 public:
  explicit CFWL_EventTextWillChange(CFWL_Widget* pSrcTarget);
  ~CFWL_EventTextWillChange() override;

  WideString change_text;
  WideString previous_text;
  bool cancelled = false;
  size_t selection_start = 0;
  size_t selection_end = 0;
};

#endif  // XFA_FWL_CFWL_EVENTTEXTWILLCHANGE_H_
