// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_EVENTSELECTCHANGED_H_
#define XFA_FWL_CFWL_EVENTSELECTCHANGED_H_

#include "xfa/fwl/cfwl_event.h"

class CFWL_EventSelectChanged final : public CFWL_Event {
 public:
  explicit CFWL_EventSelectChanged(CFWL_Widget* pSrcTarget);
  ~CFWL_EventSelectChanged() override;

  // Used by ComboBox.
  bool bLButtonUp;

  // Used by DateTimePIcker
  int32_t iYear;
  int32_t iMonth;
  int32_t iDay;
};

#endif  // XFA_FWL_CFWL_EVENTSELECTCHANGED_H_
