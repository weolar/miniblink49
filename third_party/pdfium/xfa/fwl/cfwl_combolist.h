// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_COMBOLIST_H_
#define XFA_FWL_CFWL_COMBOLIST_H_

#include <memory>

#include "xfa/fwl/cfwl_listbox.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/cfwl_widgetproperties.h"

class CFWL_ComboList final : public CFWL_ListBox {
 public:
  CFWL_ComboList(const CFWL_App* app,
                 std::unique_ptr<CFWL_WidgetProperties> properties,
                 CFWL_Widget* pOuter);

  // CFWL_ListBox.
  void OnProcessMessage(CFWL_Message* pMessage) override;

  int32_t MatchItem(const WideString& wsMatch);

  void ChangeSelected(int32_t iSel);

  void SetNotifyOwner(bool notify) { m_bNotifyOwner = notify; }

 private:
  CFX_PointF ClientToOuter(const CFX_PointF& point);
  void OnDropListFocusChanged(CFWL_Message* pMsg, bool bSet);
  void OnDropListMouseMove(CFWL_MessageMouse* pMsg);
  void OnDropListLButtonDown(CFWL_MessageMouse* pMsg);
  void OnDropListLButtonUp(CFWL_MessageMouse* pMsg);
  bool OnDropListKey(CFWL_MessageKey* pKey);
  void OnDropListKeyDown(CFWL_MessageKey* pKey);

  bool m_bNotifyOwner;
};

#endif  // XFA_FWL_CFWL_COMBOLIST_H_
