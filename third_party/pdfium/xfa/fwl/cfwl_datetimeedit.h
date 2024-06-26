// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_DATETIMEEDIT_H_
#define XFA_FWL_CFWL_DATETIMEEDIT_H_

#include <memory>

#include "xfa/fwl/cfwl_edit.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/cfwl_widgetproperties.h"

class CFWL_DateTimeEdit final : public CFWL_Edit {
 public:
  CFWL_DateTimeEdit(const CFWL_App* app,
                    std::unique_ptr<CFWL_WidgetProperties> properties,
                    CFWL_Widget* pOuter);

  // CFWL_Edit.
  void OnProcessMessage(CFWL_Message* pMessage) override;
};

#endif  // XFA_FWL_CFWL_DATETIMEEDIT_H_
