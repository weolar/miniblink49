// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFDATETIMEEDIT_H_
#define XFA_FXFA_CXFA_FFDATETIMEEDIT_H_

#include "core/fxcrt/fx_coordinates.h"
#include "xfa/fxfa/cxfa_fftextedit.h"

enum XFA_DATETIMETYPE {
  XFA_DATETIMETYPE_Date = 0,
  XFA_DATETIMETYPE_Time,
  XFA_DATETIMETYPE_DateAndTime
};

class CFWL_DateTimePicker;
class CFWL_Event;
class CFWL_Widget;

class CXFA_FFDateTimeEdit final : public CXFA_FFTextEdit {
 public:
  explicit CXFA_FFDateTimeEdit(CXFA_Node* pNode);
  ~CXFA_FFDateTimeEdit() override;

  // CXFA_FFTextEdit
  CFX_RectF GetBBox(uint32_t dwStatus, FocusOption focus) override;
  bool LoadWidget() override;
  void UpdateWidgetProperty() override;
  void OnProcessEvent(CFWL_Event* pEvent) override;

  void OnSelectChanged(CFWL_Widget* pWidget,
                       int32_t iYear,
                       int32_t iMonth,
                       int32_t iDay);

  // CXFA_FFWidget
  bool CanUndo() override;
  bool CanRedo() override;
  bool Undo() override;
  bool Redo() override;
  bool CanCopy() override;
  bool CanCut() override;
  bool CanPaste() override;
  bool CanSelectAll() override;
  Optional<WideString> Copy() override;
  Optional<WideString> Cut() override;
  bool Paste(const WideString& wsPaste) override;
  void SelectAll() override;
  void Delete() override;
  void DeSelect() override;
  WideString GetText() override;

 private:
  bool PtInActiveRect(const CFX_PointF& point) override;
  bool CommitData() override;
  bool UpdateFWLData() override;
  bool IsDataChanged() override;

  CFWL_DateTimePicker* GetPickerWidget();

  uint32_t GetAlignment();
};

#endif  // XFA_FXFA_CXFA_FFDATETIMEEDIT_H_
