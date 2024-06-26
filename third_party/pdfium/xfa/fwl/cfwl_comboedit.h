// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_COMBOEDIT_H_
#define XFA_FWL_CFWL_COMBOEDIT_H_

#include <memory>

#include "xfa/fwl/cfwl_edit.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/cfwl_widgetproperties.h"

class CFWL_ComboBox;

class CFWL_ComboEdit final : public CFWL_Edit {
 public:
  CFWL_ComboEdit(const CFWL_App* app,
                 std::unique_ptr<CFWL_WidgetProperties> properties,
                 CFWL_Widget* pOuter);

  // CFWL_Edit.
  void OnProcessMessage(CFWL_Message* pMessage) override;

  void ClearSelected();
  void SetSelected();
  void FlagFocus(bool bSet);

 private:
  CFWL_ComboBox* m_pOuter;
};

#endif  // XFA_FWL_CFWL_COMBOEDIT_H_
