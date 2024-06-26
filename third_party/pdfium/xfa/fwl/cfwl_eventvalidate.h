// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_EVENTVALIDATE_H_
#define XFA_FWL_CFWL_EVENTVALIDATE_H_

#include "xfa/fwl/cfwl_event.h"

class CFWL_EventValidate final : public CFWL_Event {
 public:
  explicit CFWL_EventValidate(CFWL_Widget* pSrcTarget);
  ~CFWL_EventValidate() override;

  WideString wsInsert;
  bool bValidate;
};

#endif  // XFA_FWL_CFWL_EVENTVALIDATE_H_
