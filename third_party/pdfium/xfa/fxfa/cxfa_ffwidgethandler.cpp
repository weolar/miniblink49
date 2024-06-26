// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_ffwidgethandler.h"

#include <vector>

#include "fxjs/xfa/cjx_object.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_fffield.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_fwladapterwidgetmgr.h"
#include "xfa/fxfa/parser/cxfa_calculate.h"
#include "xfa/fxfa/parser/cxfa_checkbutton.h"
#include "xfa/fxfa/parser/cxfa_layoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_ui.h"
#include "xfa/fxfa/parser/cxfa_validate.h"

CXFA_FFWidgetHandler::CXFA_FFWidgetHandler(CXFA_FFDocView* pDocView)
    : m_pDocView(pDocView) {}

CXFA_FFWidgetHandler::~CXFA_FFWidgetHandler() {}

bool CXFA_FFWidgetHandler::OnMouseEnter(CXFA_FFWidget* hWidget) {
  m_pDocView->LockUpdate();
  bool bRet = hWidget->OnMouseEnter();
  m_pDocView->UnlockUpdate();
  m_pDocView->UpdateDocView();
  return bRet;
}

bool CXFA_FFWidgetHandler::OnMouseExit(CXFA_FFWidget* hWidget) {
  m_pDocView->LockUpdate();
  bool bRet = hWidget->OnMouseExit();
  m_pDocView->UnlockUpdate();
  m_pDocView->UpdateDocView();
  return bRet;
}

bool CXFA_FFWidgetHandler::OnLButtonDown(CXFA_FFWidget* hWidget,
                                         uint32_t dwFlags,
                                         const CFX_PointF& point) {
  m_pDocView->LockUpdate();
  bool bRet = hWidget->AcceptsFocusOnButtonDown(
      dwFlags, hWidget->Rotate2Normal(point), FWL_MouseCommand::LeftButtonDown);
  if (bRet) {
    if (m_pDocView->SetFocus(hWidget)) {
      m_pDocView->GetDoc()->GetDocEnvironment()->SetFocusWidget(
          m_pDocView->GetDoc(), hWidget);
    }
    hWidget->OnLButtonDown(dwFlags, hWidget->Rotate2Normal(point));
  }
  m_pDocView->UnlockUpdate();
  m_pDocView->UpdateDocView();
  return bRet;
}

bool CXFA_FFWidgetHandler::OnLButtonUp(CXFA_FFWidget* hWidget,
                                       uint32_t dwFlags,
                                       const CFX_PointF& point) {
  m_pDocView->LockUpdate();
  m_pDocView->m_bLayoutEvent = true;
  bool bRet = hWidget->OnLButtonUp(dwFlags, hWidget->Rotate2Normal(point));
  m_pDocView->UnlockUpdate();
  m_pDocView->UpdateDocView();
  return bRet;
}

bool CXFA_FFWidgetHandler::OnLButtonDblClk(CXFA_FFWidget* hWidget,
                                           uint32_t dwFlags,
                                           const CFX_PointF& point) {
  bool bRet = hWidget->OnLButtonDblClk(dwFlags, hWidget->Rotate2Normal(point));
  return bRet;
}

bool CXFA_FFWidgetHandler::OnMouseMove(CXFA_FFWidget* hWidget,
                                       uint32_t dwFlags,
                                       const CFX_PointF& point) {
  bool bRet = hWidget->OnMouseMove(dwFlags, hWidget->Rotate2Normal(point));
  return bRet;
}

bool CXFA_FFWidgetHandler::OnMouseWheel(CXFA_FFWidget* hWidget,
                                        uint32_t dwFlags,
                                        int16_t zDelta,
                                        const CFX_PointF& point) {
  bool bRet =
      hWidget->OnMouseWheel(dwFlags, zDelta, hWidget->Rotate2Normal(point));
  return bRet;
}

bool CXFA_FFWidgetHandler::OnRButtonDown(CXFA_FFWidget* hWidget,
                                         uint32_t dwFlags,
                                         const CFX_PointF& point) {
  bool bRet =
      hWidget->AcceptsFocusOnButtonDown(dwFlags, hWidget->Rotate2Normal(point),
                                        FWL_MouseCommand::RightButtonDown);
  if (bRet) {
    if (m_pDocView->SetFocus(hWidget)) {
      m_pDocView->GetDoc()->GetDocEnvironment()->SetFocusWidget(
          m_pDocView->GetDoc(), hWidget);
    }
    hWidget->OnRButtonDown(dwFlags, hWidget->Rotate2Normal(point));
  }
  return bRet;
}

bool CXFA_FFWidgetHandler::OnRButtonUp(CXFA_FFWidget* hWidget,
                                       uint32_t dwFlags,
                                       const CFX_PointF& point) {
  bool bRet = hWidget->OnRButtonUp(dwFlags, hWidget->Rotate2Normal(point));
  return bRet;
}

bool CXFA_FFWidgetHandler::OnRButtonDblClk(CXFA_FFWidget* hWidget,
                                           uint32_t dwFlags,
                                           const CFX_PointF& point) {
  bool bRet = hWidget->OnRButtonDblClk(dwFlags, hWidget->Rotate2Normal(point));
  return bRet;
}

bool CXFA_FFWidgetHandler::OnKeyDown(CXFA_FFWidget* hWidget,
                                     uint32_t dwKeyCode,
                                     uint32_t dwFlags) {
  bool bRet = hWidget->OnKeyDown(dwKeyCode, dwFlags);
  m_pDocView->UpdateDocView();
  return bRet;
}

bool CXFA_FFWidgetHandler::OnKeyUp(CXFA_FFWidget* hWidget,
                                   uint32_t dwKeyCode,
                                   uint32_t dwFlags) {
  bool bRet = hWidget->OnKeyUp(dwKeyCode, dwFlags);
  return bRet;
}

bool CXFA_FFWidgetHandler::OnChar(CXFA_FFWidget* hWidget,
                                  uint32_t dwChar,
                                  uint32_t dwFlags) {
  bool bRet = hWidget->OnChar(dwChar, dwFlags);
  return bRet;
}

WideString CXFA_FFWidgetHandler::GetText(CXFA_FFWidget* widget) {
  return widget->GetText();
}

WideString CXFA_FFWidgetHandler::GetSelectedText(CXFA_FFWidget* widget) {
  if (!widget->CanCopy())
    return WideString();

  return widget->Copy().value_or(WideString());
}

void CXFA_FFWidgetHandler::PasteText(CXFA_FFWidget* widget,
                                     const WideString& text) {
  if (!widget->CanPaste())
    return;

  widget->Paste(text);
}

bool CXFA_FFWidgetHandler::CanUndo(CXFA_FFWidget* widget) {
  return widget->CanUndo();
}

bool CXFA_FFWidgetHandler::CanRedo(CXFA_FFWidget* widget) {
  return widget->CanRedo();
}

bool CXFA_FFWidgetHandler::Undo(CXFA_FFWidget* widget) {
  return widget->Undo();
}

bool CXFA_FFWidgetHandler::Redo(CXFA_FFWidget* widget) {
  return widget->Redo();
}

FWL_WidgetHit CXFA_FFWidgetHandler::OnHitTest(CXFA_FFWidget* hWidget,
                                              const CFX_PointF& point) {
  if (!(hWidget->GetStatus() & XFA_WidgetStatus_Visible))
    return FWL_WidgetHit::Unknown;
  return hWidget->OnHitTest(hWidget->Rotate2Normal(point));
}

bool CXFA_FFWidgetHandler::OnSetCursor(CXFA_FFWidget* hWidget,
                                       const CFX_PointF& point) {
  return hWidget->OnSetCursor(hWidget->Rotate2Normal(point));
}

void CXFA_FFWidgetHandler::RenderWidget(CXFA_FFWidget* hWidget,
                                        CXFA_Graphics* pGS,
                                        const CFX_Matrix& matrix,
                                        bool bHighlight) {
  hWidget->RenderWidget(pGS, matrix,
                        bHighlight ? XFA_WidgetStatus_Highlight : 0);
}

bool CXFA_FFWidgetHandler::HasEvent(CXFA_Node* pNode,
                                    XFA_EVENTTYPE eEventType) {
  if (eEventType == XFA_EVENT_Unknown)
    return false;
  if (!pNode || pNode->GetElementType() == XFA_Element::Draw)
    return false;

  switch (eEventType) {
    case XFA_EVENT_Calculate: {
      CXFA_Calculate* calc = pNode->GetCalculateIfExists();
      return calc && calc->GetScriptIfExists();
    }
    case XFA_EVENT_Validate: {
      CXFA_Validate* validate = pNode->GetValidateIfExists();
      return validate && validate->GetScriptIfExists();
    }
    default:
      break;
  }
  return !pNode->GetEventByActivity(gs_EventActivity[eEventType], false)
              .empty();
}

int32_t CXFA_FFWidgetHandler::ProcessEvent(CXFA_Node* pNode,
                                           CXFA_EventParam* pParam) {
  if (!pParam || pParam->m_eType == XFA_EVENT_Unknown)
    return XFA_EVENTERROR_NotExist;
  if (!pNode || pNode->GetElementType() == XFA_Element::Draw)
    return XFA_EVENTERROR_NotExist;

  switch (pParam->m_eType) {
    case XFA_EVENT_Calculate:
      return pNode->ProcessCalculate(m_pDocView.Get());
    case XFA_EVENT_Validate:
      if (m_pDocView->GetDoc()->GetDocEnvironment()->IsValidationsEnabled(
              m_pDocView->GetDoc())) {
        return pNode->ProcessValidate(m_pDocView.Get(), 0);
      }
      return XFA_EVENTERROR_Disabled;
    case XFA_EVENT_InitCalculate: {
      CXFA_Calculate* calc = pNode->GetCalculateIfExists();
      if (!calc)
        return XFA_EVENTERROR_NotExist;
      if (pNode->IsUserInteractive())
        return XFA_EVENTERROR_Disabled;
      return pNode->ExecuteScript(m_pDocView.Get(), calc->GetScriptIfExists(),
                                  pParam);
    }
    default:
      break;
  }
  return pNode->ProcessEvent(m_pDocView.Get(),
                             gs_EventActivity[pParam->m_eType], pParam);
}

CXFA_FFWidget* CXFA_FFWidgetHandler::CreateWidget(CXFA_FFWidget* hParent,
                                                  XFA_WIDGETTYPE eType,
                                                  CXFA_FFWidget* hBefore) {
  CXFA_Node* pParentFormItem = hParent ? hParent->GetNode() : nullptr;
  CXFA_Node* pBeforeFormItem = hBefore ? hBefore->GetNode() : nullptr;
  CXFA_Node* pNewFormItem =
      CreateWidgetFormItem(eType, pParentFormItem, pBeforeFormItem);
  if (!pNewFormItem)
    return nullptr;

  CXFA_Node* templateNode = pNewFormItem->GetTemplateNodeIfExists();
  if (!templateNode)
    return nullptr;

  templateNode->SetFlagAndNotify(XFA_NodeFlag_Initialized);
  pNewFormItem->SetFlagAndNotify(XFA_NodeFlag_Initialized);
  m_pDocView->RunLayout();
  CXFA_LayoutItem* pLayout =
      m_pDocView->GetXFALayout()->GetLayoutItem(pNewFormItem);
  return ToFFWidget(ToContentLayoutItem(pLayout));
}

CXFA_Node* CXFA_FFWidgetHandler::CreateWidgetFormItem(
    XFA_WIDGETTYPE eType,
    CXFA_Node* pParent,
    CXFA_Node* pBefore) const {
  switch (eType) {
    case XFA_WIDGETTYPE_Barcode:
      return nullptr;
    case XFA_WIDGETTYPE_PushButton:
      return CreatePushButton(pParent, pBefore);
    case XFA_WIDGETTYPE_CheckButton:
      return CreateCheckButton(pParent, pBefore);
    case XFA_WIDGETTYPE_ExcludeGroup:
      return CreateExclGroup(pParent, pBefore);
    case XFA_WIDGETTYPE_RadioButton:
      return CreateRadioButton(pParent, pBefore);
    case XFA_WIDGETTYPE_Arc:
      return CreateArc(pParent, pBefore);
    case XFA_WIDGETTYPE_Rectangle:
      return CreateRectangle(pParent, pBefore);
    case XFA_WIDGETTYPE_Image:
      return CreateImage(pParent, pBefore);
    case XFA_WIDGETTYPE_Line:
      return CreateLine(pParent, pBefore);
    case XFA_WIDGETTYPE_Text:
      return CreateText(pParent, pBefore);
    case XFA_WIDGETTYPE_DatetimeEdit:
      return CreateDatetimeEdit(pParent, pBefore);
    case XFA_WIDGETTYPE_DecimalField:
      return CreateDecimalField(pParent, pBefore);
    case XFA_WIDGETTYPE_NumericField:
      return CreateNumericField(pParent, pBefore);
    case XFA_WIDGETTYPE_Signature:
      return CreateSignature(pParent, pBefore);
    case XFA_WIDGETTYPE_TextEdit:
      return CreateTextEdit(pParent, pBefore);
    case XFA_WIDGETTYPE_DropdownList:
      return CreateDropdownList(pParent, pBefore);
    case XFA_WIDGETTYPE_ListBox:
      return CreateListBox(pParent, pBefore);
    case XFA_WIDGETTYPE_ImageField:
      return CreateImageField(pParent, pBefore);
    case XFA_WIDGETTYPE_PasswordEdit:
      return CreatePasswordEdit(pParent, pBefore);
    case XFA_WIDGETTYPE_Subform:
      return CreateSubform(pParent, pBefore);
    default:
      return nullptr;
  }
}

CXFA_Node* CXFA_FFWidgetHandler::CreatePushButton(CXFA_Node* pParent,
                                                  CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateField(XFA_Element::Button, pParent, pBefore);
  CXFA_Node* pCaption = CreateCopyNode(XFA_Element::Caption, pField, nullptr);
  CXFA_Node* pValue = CreateCopyNode(XFA_Element::Value, pCaption, nullptr);
  CXFA_Node* pText = CreateCopyNode(XFA_Element::Text, pValue, nullptr);
  pText->JSObject()->SetContent(L"Button", L"Button", false, false, true);

  CXFA_Node* pPara = CreateCopyNode(XFA_Element::Para, pCaption, nullptr);
  pPara->JSObject()->SetEnum(XFA_Attribute::VAlign, XFA_AttributeValue::Middle,
                             false);
  pPara->JSObject()->SetEnum(XFA_Attribute::HAlign, XFA_AttributeValue::Center,
                             false);
  CreateFontNode(pCaption);

  CXFA_Node* pBorder = CreateCopyNode(XFA_Element::Border, pField, nullptr);
  pBorder->JSObject()->SetEnum(XFA_Attribute::Hand, XFA_AttributeValue::Right,
                               false);

  CXFA_Node* pEdge = CreateCopyNode(XFA_Element::Edge, pBorder, nullptr);
  pEdge->JSObject()->SetEnum(XFA_Attribute::Stroke, XFA_AttributeValue::Raised,
                             false);

  CXFA_Node* pFill = CreateCopyNode(XFA_Element::Fill, pBorder, nullptr);
  CXFA_Node* pColor = CreateCopyNode(XFA_Element::Color, pFill, nullptr);
  pColor->JSObject()->SetCData(XFA_Attribute::Value, L"212, 208, 200", false,
                               false);

  CXFA_Node* pBind = CreateCopyNode(XFA_Element::Bind, pField, nullptr);
  pBind->JSObject()->SetEnum(XFA_Attribute::Match, XFA_AttributeValue::None,
                             false);

  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateCheckButton(CXFA_Node* pParent,
                                                   CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::CheckButton, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateExclGroup(CXFA_Node* pParent,
                                                 CXFA_Node* pBefore) const {
  return CreateFormItem(XFA_Element::ExclGroup, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateRadioButton(CXFA_Node* pParent,
                                                   CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateField(XFA_Element::CheckButton, pParent, pBefore);
  CXFA_Ui* pUi = pField->GetFirstChildByClass<CXFA_Ui>(XFA_Element::Ui);
  if (!pUi)
    return nullptr;

  CXFA_CheckButton* pWidget =
      pUi->GetFirstChildByClass<CXFA_CheckButton>(XFA_Element::CheckButton);
  if (!pWidget)
    return nullptr;

  pWidget->JSObject()->SetEnum(XFA_Attribute::Shape, XFA_AttributeValue::Round,
                               false);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateDatetimeEdit(CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateField(XFA_Element::DateTimeEdit, pParent, pBefore);
  CreateValueNode(XFA_Element::Date, pField);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateDecimalField(CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateNumericField(pParent, pBefore);
  CreateValueNode(XFA_Element::Decimal, pField);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateNumericField(CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::NumericEdit, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateSignature(CXFA_Node* pParent,
                                                 CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::Signature, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateTextEdit(CXFA_Node* pParent,
                                                CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::TextEdit, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateDropdownList(CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::ChoiceList, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateListBox(CXFA_Node* pParent,
                                               CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateDropdownList(pParent, pBefore);
  CXFA_Node* pUi = pField->GetFirstChild();
  CXFA_Node* pListBox = pUi->GetFirstChild();
  pListBox->JSObject()->SetEnum(XFA_Attribute::Open, XFA_AttributeValue::Always,
                                false);
  pListBox->JSObject()->SetEnum(XFA_Attribute::CommitOn,
                                XFA_AttributeValue::Exit, false);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateImageField(CXFA_Node* pParent,
                                                  CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::ImageEdit, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreatePasswordEdit(CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateField(XFA_Element::PasswordEdit, pParent, pBefore);
  CXFA_Node* pBind = CreateCopyNode(XFA_Element::Bind, pField, nullptr);
  pBind->JSObject()->SetEnum(XFA_Attribute::Match, XFA_AttributeValue::None,
                             false);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateField(XFA_Element eElement,
                                             CXFA_Node* pParent,
                                             CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateFormItem(XFA_Element::Field, pParent, pBefore);
  CreateCopyNode(eElement, CreateCopyNode(XFA_Element::Ui, pField, nullptr),
                 nullptr);
  CreateFontNode(pField);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateArc(CXFA_Node* pParent,
                                           CXFA_Node* pBefore) const {
  return CreateDraw(XFA_Element::Arc, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateRectangle(CXFA_Node* pParent,
                                                 CXFA_Node* pBefore) const {
  return CreateDraw(XFA_Element::Rectangle, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateImage(CXFA_Node* pParent,
                                             CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateDraw(XFA_Element::Image, pParent, pBefore);
  CreateCopyNode(XFA_Element::ImageEdit,
                 CreateCopyNode(XFA_Element::Ui, pField, nullptr), nullptr);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateLine(CXFA_Node* pParent,
                                            CXFA_Node* pBefore) const {
  return CreateDraw(XFA_Element::Line, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateText(CXFA_Node* pParent,
                                            CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateDraw(XFA_Element::Text, pParent, pBefore);
  CreateCopyNode(XFA_Element::TextEdit,
                 CreateCopyNode(XFA_Element::Ui, pField, nullptr), nullptr);
  CreateFontNode(pField);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateDraw(XFA_Element eElement,
                                            CXFA_Node* pParent,
                                            CXFA_Node* pBefore) const {
  CXFA_Node* pDraw = CreateFormItem(XFA_Element::Draw, pParent, pBefore);
  CreateValueNode(eElement, pDraw);
  return pDraw;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateSubform(CXFA_Node* pParent,
                                               CXFA_Node* pBefore) const {
  return CreateFormItem(XFA_Element::Subform, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateFormItem(XFA_Element eElement,
                                                CXFA_Node* pParent,
                                                CXFA_Node* pBefore) const {
  if (!pParent)
    return nullptr;

  CXFA_Node* pTemplateParent = pParent->GetTemplateNodeIfExists();
  if (!pTemplateParent)
    return nullptr;

  CXFA_Node* pNewFormItem = pTemplateParent->CloneTemplateToForm(false);
  if (pParent)
    pParent->InsertChild(pNewFormItem, pBefore);
  return pNewFormItem;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateCopyNode(XFA_Element eElement,
                                                CXFA_Node* pParent,
                                                CXFA_Node* pBefore) const {
  if (!pParent)
    return nullptr;

  CXFA_Node* pTemplateParent = pParent->GetTemplateNodeIfExists();
  CXFA_Node* pNewNode =
      CreateTemplateNode(eElement, pTemplateParent,
                         pBefore ? pBefore->GetTemplateNodeIfExists() : nullptr)
          ->Clone(false);
  if (pParent)
    pParent->InsertChild(pNewNode, pBefore);
  return pNewNode;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateTemplateNode(XFA_Element eElement,
                                                    CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  CXFA_Document* pXFADoc = GetXFADoc();
  CXFA_Node* pNewTemplateNode =
      pXFADoc->CreateNode(XFA_PacketType::Template, eElement);
  if (pParent)
    pParent->InsertChild(pNewTemplateNode, pBefore);
  return pNewTemplateNode;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateFontNode(CXFA_Node* pParent) const {
  CXFA_Node* pFont = CreateCopyNode(XFA_Element::Font, pParent, nullptr);
  pFont->JSObject()->SetCData(XFA_Attribute::Typeface, L"Myriad Pro", false,
                              false);
  return pFont;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateMarginNode(CXFA_Node* pParent,
                                                  uint32_t dwFlags,
                                                  float fInsets[4]) const {
  CXFA_Node* pMargin = CreateCopyNode(XFA_Element::Margin, pParent, nullptr);
  if (dwFlags & 0x01)
    pMargin->JSObject()->SetMeasure(XFA_Attribute::LeftInset,
                                    CXFA_Measurement(fInsets[0], XFA_Unit::Pt),
                                    false);
  if (dwFlags & 0x02)
    pMargin->JSObject()->SetMeasure(XFA_Attribute::TopInset,
                                    CXFA_Measurement(fInsets[1], XFA_Unit::Pt),
                                    false);
  if (dwFlags & 0x04)
    pMargin->JSObject()->SetMeasure(XFA_Attribute::RightInset,
                                    CXFA_Measurement(fInsets[2], XFA_Unit::Pt),
                                    false);
  if (dwFlags & 0x08)
    pMargin->JSObject()->SetMeasure(XFA_Attribute::BottomInset,
                                    CXFA_Measurement(fInsets[3], XFA_Unit::Pt),
                                    false);
  return pMargin;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateValueNode(XFA_Element eValue,
                                                 CXFA_Node* pParent) const {
  CXFA_Node* pValue = CreateCopyNode(XFA_Element::Value, pParent, nullptr);
  CreateCopyNode(eValue, pValue, nullptr);
  return pValue;
}

CXFA_Document* CXFA_FFWidgetHandler::GetObjFactory() const {
  return GetXFADoc();
}

CXFA_Document* CXFA_FFWidgetHandler::GetXFADoc() const {
  return m_pDocView->GetDoc()->GetXFADoc();
}
