// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFWIDGETHANDLER_H_
#define XFA_FXFA_CXFA_FFWIDGETHANDLER_H_

#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/fxfa.h"
#include "xfa/fxfa/parser/cxfa_document.h"

class CXFA_FFDocView;
class CXFA_Graphics;
enum class FWL_WidgetHit;

class CXFA_FFWidgetHandler {
 public:
  explicit CXFA_FFWidgetHandler(CXFA_FFDocView* pDocView);
  ~CXFA_FFWidgetHandler();

  CXFA_FFWidget* CreateWidget(CXFA_FFWidget* hParent,
                              XFA_WIDGETTYPE eType,
                              CXFA_FFWidget* hBefore);

  bool OnMouseEnter(CXFA_FFWidget* hWidget);
  bool OnMouseExit(CXFA_FFWidget* hWidget);
  bool OnLButtonDown(CXFA_FFWidget* hWidget,
                     uint32_t dwFlags,
                     const CFX_PointF& point);
  bool OnLButtonUp(CXFA_FFWidget* hWidget,
                   uint32_t dwFlags,
                   const CFX_PointF& point);
  bool OnLButtonDblClk(CXFA_FFWidget* hWidget,
                       uint32_t dwFlags,
                       const CFX_PointF& point);
  bool OnMouseMove(CXFA_FFWidget* hWidget,
                   uint32_t dwFlags,
                   const CFX_PointF& point);
  bool OnMouseWheel(CXFA_FFWidget* hWidget,
                    uint32_t dwFlags,
                    int16_t zDelta,
                    const CFX_PointF& point);
  bool OnRButtonDown(CXFA_FFWidget* hWidget,
                     uint32_t dwFlags,
                     const CFX_PointF& point);
  bool OnRButtonUp(CXFA_FFWidget* hWidget,
                   uint32_t dwFlags,
                   const CFX_PointF& point);
  bool OnRButtonDblClk(CXFA_FFWidget* hWidget,
                       uint32_t dwFlags,
                       const CFX_PointF& point);

  WideString GetText(CXFA_FFWidget* widget);
  WideString GetSelectedText(CXFA_FFWidget* widget);
  void PasteText(CXFA_FFWidget* widget, const WideString& text);

  bool CanUndo(CXFA_FFWidget* widget);
  bool CanRedo(CXFA_FFWidget* widget);
  bool Undo(CXFA_FFWidget* widget);
  bool Redo(CXFA_FFWidget* widget);

  bool OnKeyDown(CXFA_FFWidget* hWidget, uint32_t dwKeyCode, uint32_t dwFlags);
  bool OnKeyUp(CXFA_FFWidget* hWidget, uint32_t dwKeyCode, uint32_t dwFlags);
  bool OnChar(CXFA_FFWidget* hWidget, uint32_t dwChar, uint32_t dwFlags);
  FWL_WidgetHit OnHitTest(CXFA_FFWidget* hWidget, const CFX_PointF& point);
  bool OnSetCursor(CXFA_FFWidget* hWidget, const CFX_PointF& point);
  void RenderWidget(CXFA_FFWidget* hWidget,
                    CXFA_Graphics* pGS,
                    const CFX_Matrix& matrix,
                    bool bHighlight);
  bool HasEvent(CXFA_Node* pNode, XFA_EVENTTYPE eEventType);
  int32_t ProcessEvent(CXFA_Node* pNode, CXFA_EventParam* pParam);

 private:
  CXFA_Node* CreateWidgetFormItem(XFA_WIDGETTYPE eType,
                                  CXFA_Node* pParent,
                                  CXFA_Node* pBefore) const;

  CXFA_Node* CreatePushButton(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateCheckButton(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateExclGroup(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateRadioButton(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateDatetimeEdit(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateDecimalField(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateNumericField(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateSignature(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateTextEdit(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateDropdownList(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateListBox(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateImageField(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreatePasswordEdit(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateField(XFA_Element eElement,
                         CXFA_Node* pParent,
                         CXFA_Node* pBefore) const;
  CXFA_Node* CreateArc(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateRectangle(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateImage(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateLine(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateText(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateDraw(XFA_Element eElement,
                        CXFA_Node* pParent,
                        CXFA_Node* pBefore) const;

  CXFA_Node* CreateSubform(CXFA_Node* pParent, CXFA_Node* pBefore) const;
  CXFA_Node* CreateFormItem(XFA_Element eElement,
                            CXFA_Node* pParent,
                            CXFA_Node* pBefore) const;
  CXFA_Node* CreateCopyNode(XFA_Element eElement,
                            CXFA_Node* pParent,
                            CXFA_Node* pBefore) const;
  CXFA_Node* CreateTemplateNode(XFA_Element eElement,
                                CXFA_Node* pParent,
                                CXFA_Node* pBefore) const;
  CXFA_Node* CreateFontNode(CXFA_Node* pParent) const;
  CXFA_Node* CreateMarginNode(CXFA_Node* pParent,
                              uint32_t dwFlags,
                              float fInsets[4]) const;
  CXFA_Node* CreateValueNode(XFA_Element eValue, CXFA_Node* pParent) const;
  CXFA_Document* GetObjFactory() const;
  CXFA_Document* GetXFADoc() const;

  UnownedPtr<CXFA_FFDocView> m_pDocView;
};

#endif  //  XFA_FXFA_CXFA_FFWIDGETHANDLER_H_
