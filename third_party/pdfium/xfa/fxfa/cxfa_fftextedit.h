// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFTEXTEDIT_H_
#define XFA_FXFA_CXFA_FFTEXTEDIT_H_

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/cxfa_fffield.h"

class CFWL_Event;
class CFWL_EventTextWillChange;
class CFWL_Widget;
class CFX_Matrix;
class CXFA_FFWidget;
class IFWL_WidgetDelegate;

class CXFA_FFTextEdit : public CXFA_FFField {
 public:
  explicit CXFA_FFTextEdit(CXFA_Node* pNode);
  ~CXFA_FFTextEdit() override;

  // CXFA_FFField
  bool LoadWidget() override;
  void UpdateWidgetProperty() override;
  bool AcceptsFocusOnButtonDown(uint32_t dwFlags,
                                const CFX_PointF& point,
                                FWL_MouseCommand command) override;
  void OnLButtonDown(uint32_t dwFlags, const CFX_PointF& point) override;
  void OnRButtonDown(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnRButtonUp(uint32_t dwFlags, const CFX_PointF& point) override;
  bool OnSetFocus(CXFA_FFWidget* pOldWidget) override;
  bool OnKillFocus(CXFA_FFWidget* pNewWidget) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;

  void OnTextWillChange(CFWL_Widget* pWidget, CFWL_EventTextWillChange* change);
  void OnTextFull(CFWL_Widget* pWidget);

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
  FormFieldType GetFormFieldType() override;

 protected:
  uint32_t GetAlignment();

  UnownedPtr<IFWL_WidgetDelegate> m_pOldDelegate;

 private:
  bool CommitData() override;
  bool UpdateFWLData() override;
  bool IsDataChanged() override;
  void ValidateNumberField(const WideString& wsText);
};

#endif  // XFA_FXFA_CXFA_FFTEXTEDIT_H_
