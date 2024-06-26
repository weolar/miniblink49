// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_fflistbox.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fwl/cfwl_listbox.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/parser/cxfa_para.h"

namespace {

CFWL_ListBox* ToListBox(CFWL_Widget* widget) {
  return static_cast<CFWL_ListBox*>(widget);
}

}  // namespace

CXFA_FFListBox::CXFA_FFListBox(CXFA_Node* pNode)
    : CXFA_FFDropDown(pNode), m_pOldDelegate(nullptr) {}

CXFA_FFListBox::~CXFA_FFListBox() {
  if (!m_pNormalWidget)
    return;

  CFWL_NoteDriver* pNoteDriver =
      m_pNormalWidget->GetOwnerApp()->GetNoteDriver();
  pNoteDriver->UnregisterEventTarget(m_pNormalWidget.get());
}

bool CXFA_FFListBox::LoadWidget() {
  auto pNew = pdfium::MakeUnique<CFWL_ListBox>(
      GetFWLApp(), pdfium::MakeUnique<CFWL_WidgetProperties>(), nullptr);
  CFWL_ListBox* pListBox = pNew.get();
  pListBox->ModifyStyles(FWL_WGTSTYLE_VScroll | FWL_WGTSTYLE_NoBackground,
                         0xFFFFFFFF);
  m_pNormalWidget = std::move(pNew);
  m_pNormalWidget->SetLayoutItem(this);

  CFWL_NoteDriver* pNoteDriver =
      m_pNormalWidget->GetOwnerApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(m_pNormalWidget.get(),
                                   m_pNormalWidget.get());
  m_pOldDelegate = m_pNormalWidget->GetDelegate();
  m_pNormalWidget->SetDelegate(this);
  m_pNormalWidget->LockUpdate();

  for (const auto& label : m_pNode->GetChoiceListItems(false))
    pListBox->AddString(label.AsStringView());

  uint32_t dwExtendedStyle = FWL_STYLEEXT_LTB_ShowScrollBarFocus;
  if (m_pNode->IsChoiceListMultiSelect())
    dwExtendedStyle |= FWL_STYLEEXT_LTB_MultiSelection;

  dwExtendedStyle |= GetAlignment();
  m_pNormalWidget->ModifyStylesEx(dwExtendedStyle, 0xFFFFFFFF);
  for (int32_t selected : m_pNode->GetSelectedItems())
    pListBox->SetSelItem(pListBox->GetItem(nullptr, selected), true);

  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}

bool CXFA_FFListBox::OnKillFocus(CXFA_FFWidget* pNewFocus) {
  if (!ProcessCommittedData())
    UpdateFWLData();

  CXFA_FFField::OnKillFocus(pNewFocus);
  return true;
}

bool CXFA_FFListBox::CommitData() {
  auto* pListBox = ToListBox(m_pNormalWidget.get());
  std::vector<int32_t> iSelArray;
  int32_t iSels = pListBox->CountSelItems();
  for (int32_t i = 0; i < iSels; ++i)
    iSelArray.push_back(pListBox->GetSelIndex(i));

  m_pNode->SetSelectedItems(iSelArray, true, false, true);
  return true;
}

bool CXFA_FFListBox::IsDataChanged() {
  std::vector<int32_t> iSelArray = m_pNode->GetSelectedItems();
  int32_t iOldSels = pdfium::CollectionSize<int32_t>(iSelArray);
  auto* pListBox = ToListBox(m_pNormalWidget.get());
  int32_t iSels = pListBox->CountSelItems();
  if (iOldSels != iSels)
    return true;

  for (int32_t i = 0; i < iSels; ++i) {
    CFWL_ListItem* hlistItem = pListBox->GetItem(nullptr, iSelArray[i]);
    if (!(hlistItem->GetStates() & FWL_ITEMSTATE_LTB_Selected))
      return true;
  }
  return false;
}

uint32_t CXFA_FFListBox::GetAlignment() {
  CXFA_Para* para = m_pNode->GetParaIfExists();
  if (!para)
    return 0;

  uint32_t dwExtendedStyle = 0;
  switch (para->GetHorizontalAlign()) {
    case XFA_AttributeValue::Center:
      dwExtendedStyle |= FWL_STYLEEXT_LTB_CenterAlign;
      break;
    case XFA_AttributeValue::Justify:
      break;
    case XFA_AttributeValue::JustifyAll:
      break;
    case XFA_AttributeValue::Radix:
      break;
    case XFA_AttributeValue::Right:
      dwExtendedStyle |= FWL_STYLEEXT_LTB_RightAlign;
      break;
    default:
      dwExtendedStyle |= FWL_STYLEEXT_LTB_LeftAlign;
      break;
  }
  return dwExtendedStyle;
}

bool CXFA_FFListBox::UpdateFWLData() {
  if (!m_pNormalWidget)
    return false;

  auto* pListBox = ToListBox(m_pNormalWidget.get());
  std::vector<int32_t> iSelArray = m_pNode->GetSelectedItems();
  std::vector<CFWL_ListItem*> selItemArray(iSelArray.size());
  std::transform(iSelArray.begin(), iSelArray.end(), selItemArray.begin(),
                 [pListBox](int32_t val) { return pListBox->GetSelItem(val); });

  pListBox->SetSelItem(pListBox->GetSelItem(-1), false);
  for (CFWL_ListItem* pItem : selItemArray)
    pListBox->SetSelItem(pItem, true);

  m_pNormalWidget->Update();
  return true;
}

void CXFA_FFListBox::OnSelectChanged(CFWL_Widget* pWidget) {
  CXFA_EventParam eParam;
  eParam.m_eType = XFA_EVENT_Change;
  eParam.m_pTarget = m_pNode.Get();
  eParam.m_wsPrevText = m_pNode->GetValue(XFA_VALUEPICTURE_Raw);
  m_pNode->ProcessEvent(GetDocView(), XFA_AttributeValue::Change, &eParam);
}

void CXFA_FFListBox::SetItemState(int32_t nIndex, bool bSelected) {
  auto* pListBox = ToListBox(m_pNormalWidget.get());
  pListBox->SetSelItem(pListBox->GetSelItem(nIndex), bSelected);
  m_pNormalWidget->Update();
  InvalidateRect();
}

void CXFA_FFListBox::InsertItem(WideStringView wsLabel, int32_t nIndex) {
  WideString wsTemp(wsLabel);
  ToListBox(m_pNormalWidget.get())->AddString(wsTemp.AsStringView());
  m_pNormalWidget->Update();
  InvalidateRect();
}

void CXFA_FFListBox::DeleteItem(int32_t nIndex) {
  auto* pListBox = ToListBox(m_pNormalWidget.get());
  if (nIndex < 0)
    pListBox->DeleteAll();
  else
    pListBox->DeleteString(pListBox->GetItem(nullptr, nIndex));

  pListBox->Update();
  InvalidateRect();
}

void CXFA_FFListBox::OnProcessMessage(CFWL_Message* pMessage) {
  m_pOldDelegate->OnProcessMessage(pMessage);
}

void CXFA_FFListBox::OnProcessEvent(CFWL_Event* pEvent) {
  CXFA_FFField::OnProcessEvent(pEvent);
  switch (pEvent->GetType()) {
    case CFWL_Event::Type::SelectChanged:
      OnSelectChanged(m_pNormalWidget.get());
      break;
    default:
      break;
  }
  m_pOldDelegate->OnProcessEvent(pEvent);
}

void CXFA_FFListBox::OnDrawWidget(CXFA_Graphics* pGraphics,
                                  const CFX_Matrix& matrix) {
  m_pOldDelegate->OnDrawWidget(pGraphics, matrix);
}

FormFieldType CXFA_FFListBox::GetFormFieldType() {
  return FormFieldType::kXFA_ListBox;
}
