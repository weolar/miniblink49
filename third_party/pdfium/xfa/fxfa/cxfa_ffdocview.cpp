// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_ffdocview.h"

#include "core/fxcrt/fx_extension.h"
//#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cjx_object.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffbarcode.h"
#include "xfa/fxfa/cxfa_ffcheckbutton.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffexclgroup.h"
#include "xfa/fxfa/cxfa_fffield.h"
#include "xfa/fxfa/cxfa_ffimage.h"
#include "xfa/fxfa/cxfa_ffimageedit.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffpushbutton.h"
#include "xfa/fxfa/cxfa_ffsignature.h"
#include "xfa/fxfa/cxfa_fftext.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_ffwidgethandler.h"
#include "xfa/fxfa/cxfa_fwladapterwidgetmgr.h"
#include "xfa/fxfa/cxfa_readynodeiterator.h"
#include "xfa/fxfa/cxfa_textprovider.h"
#include "xfa/fxfa/parser/cxfa_acrobat.h"
#include "xfa/fxfa/parser/cxfa_binditems.h"
#include "xfa/fxfa/parser/cxfa_calculate.h"
#include "xfa/fxfa/parser/cxfa_layoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_pageset.h"
#include "xfa/fxfa/parser/cxfa_present.h"
#include "xfa/fxfa/parser/cxfa_subform.h"
#include "xfa/fxfa/parser/cxfa_validate.h"
#include "xfa/fxfa/parser/xfa_resolvenode_rs.h"

const XFA_AttributeValue gs_EventActivity[] = {
    XFA_AttributeValue::Click,      XFA_AttributeValue::Change,
    XFA_AttributeValue::DocClose,   XFA_AttributeValue::DocReady,
    XFA_AttributeValue::Enter,      XFA_AttributeValue::Exit,
    XFA_AttributeValue::Full,       XFA_AttributeValue::IndexChange,
    XFA_AttributeValue::Initialize, XFA_AttributeValue::MouseDown,
    XFA_AttributeValue::MouseEnter, XFA_AttributeValue::MouseExit,
    XFA_AttributeValue::MouseUp,    XFA_AttributeValue::PostExecute,
    XFA_AttributeValue::PostOpen,   XFA_AttributeValue::PostPrint,
    XFA_AttributeValue::PostSave,   XFA_AttributeValue::PostSign,
    XFA_AttributeValue::PostSubmit, XFA_AttributeValue::PreExecute,
    XFA_AttributeValue::PreOpen,    XFA_AttributeValue::PrePrint,
    XFA_AttributeValue::PreSave,    XFA_AttributeValue::PreSign,
    XFA_AttributeValue::PreSubmit,  XFA_AttributeValue::Ready,
    XFA_AttributeValue::Unknown,
};

CXFA_FFDocView::CXFA_FFDocView(CXFA_FFDoc* pDoc) : m_pDoc(pDoc) {}

CXFA_FFDocView::~CXFA_FFDocView() {}

void CXFA_FFDocView::InitLayout(CXFA_Node* pNode) {
  RunBindItems();
  ExecEventActivityByDeepFirst(pNode, XFA_EVENT_Initialize, false, true);
  ExecEventActivityByDeepFirst(pNode, XFA_EVENT_IndexChange, false, true);
}

int32_t CXFA_FFDocView::StartLayout() {
  m_iStatus = XFA_DOCVIEW_LAYOUTSTATUS_Start;
  m_pDoc->GetXFADoc()->DoProtoMerge();
  m_pDoc->GetXFADoc()->DoDataMerge();
  m_pXFADocLayout = GetXFALayout();

  int32_t iStatus = m_pXFADocLayout->StartLayout(false);
  if (iStatus < 0)
    return iStatus;

  CXFA_Node* pRootItem =
      ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
  if (!pRootItem)
    return iStatus;

  InitLayout(pRootItem);
  InitCalculate(pRootItem);
  InitValidate(pRootItem);

  ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_Ready, true, true);
  m_iStatus = XFA_DOCVIEW_LAYOUTSTATUS_Start;
  return iStatus;
}

int32_t CXFA_FFDocView::DoLayout() {
  int32_t iStatus = 100;
  iStatus = m_pXFADocLayout->DoLayout();
  if (iStatus != 100)
    return iStatus;

  m_iStatus = XFA_DOCVIEW_LAYOUTSTATUS_Doing;
  return iStatus;
}

void CXFA_FFDocView::StopLayout() {
  CXFA_Node* pRootItem =
      ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
  if (!pRootItem)
    return;

  CXFA_Subform* pSubformNode =
      pRootItem->GetChild<CXFA_Subform>(0, XFA_Element::Subform, false);
  if (!pSubformNode)
    return;

  CXFA_PageSet* pPageSetNode =
      pSubformNode->GetFirstChildByClass<CXFA_PageSet>(XFA_Element::PageSet);
  if (!pPageSetNode)
    return;

  RunCalculateWidgets();
  RunValidate();

  InitLayout(pPageSetNode);
  InitCalculate(pPageSetNode);
  InitValidate(pPageSetNode);

  ExecEventActivityByDeepFirst(pPageSetNode, XFA_EVENT_Ready, true, true);
  ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_Ready, false, true);
  ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_DocReady, false, true);

  RunCalculateWidgets();
  RunValidate();

  if (RunLayout())
    ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_Ready, false, true);

  m_CalculateNodes.clear();
  if (m_pFocusNode && !m_pFocusWidget)
    SetFocusNode(m_pFocusNode.Get());

  m_iStatus = XFA_DOCVIEW_LAYOUTSTATUS_End;
}

void CXFA_FFDocView::ShowNullTestMsg() {
  int32_t iCount = pdfium::CollectionSize<int32_t>(m_arrNullTestMsg);
  CXFA_FFApp* pApp = m_pDoc->GetApp();
  IXFA_AppProvider* pAppProvider = pApp->GetAppProvider();
  if (pAppProvider && iCount) {
    int32_t iRemain = iCount > 7 ? iCount - 7 : 0;
    iCount -= iRemain;
    WideString wsMsg;
    for (int32_t i = 0; i < iCount; i++)
      wsMsg += m_arrNullTestMsg[i] + L"\n";

    if (iRemain > 0) {
      wsMsg += L"\n" + WideString::Format(
                           L"Message limit exceeded. Remaining %d "
                           L"validation errors not reported.",
                           iRemain);
    }
    pAppProvider->MsgBox(wsMsg, pAppProvider->GetAppTitle(),
                         static_cast<uint32_t>(AlertIcon::kStatus),
                         static_cast<uint32_t>(AlertButton::kOK));
  }
  m_arrNullTestMsg.clear();
}

void CXFA_FFDocView::UpdateDocView() {
  if (IsUpdateLocked())
    return;

  LockUpdate();
  for (CXFA_Node* pNode : m_NewAddedNodes) {
    InitCalculate(pNode);
    InitValidate(pNode);
    ExecEventActivityByDeepFirst(pNode, XFA_EVENT_Ready, true, true);
  }
  m_NewAddedNodes.clear();

  RunSubformIndexChange();
  RunCalculateWidgets();
  RunValidate();

  ShowNullTestMsg();

  if (RunLayout() && m_bLayoutEvent)
    RunEventLayoutReady();

  m_bLayoutEvent = false;
  m_CalculateNodes.clear();
  UnlockUpdate();
}

int32_t CXFA_FFDocView::CountPageViews() const {
  return m_pXFADocLayout ? m_pXFADocLayout->CountPages() : 0;
}

CXFA_FFPageView* CXFA_FFDocView::GetPageView(int32_t nIndex) const {
  if (!m_pXFADocLayout)
    return nullptr;
  return static_cast<CXFA_FFPageView*>(m_pXFADocLayout->GetPage(nIndex));
}

CXFA_LayoutProcessor* CXFA_FFDocView::GetXFALayout() const {
  return m_pDoc->GetXFADoc()->GetLayoutProcessor();
}

bool CXFA_FFDocView::ResetSingleNodeData(CXFA_Node* pNode) {
  XFA_Element eType = pNode->GetElementType();
  if (eType != XFA_Element::Field && eType != XFA_Element::ExclGroup)
    return false;

  pNode->ResetData();
  pNode->UpdateUIDisplay(this, nullptr);
  CXFA_Validate* validate = pNode->GetValidateIfExists();
  if (!validate)
    return true;

  AddValidateNode(pNode);
  validate->SetFlag(XFA_NodeFlag_NeedsInitApp);
  return true;
}

void CXFA_FFDocView::ResetNode(CXFA_Node* pNode) {
  m_bLayoutEvent = true;
  bool bChanged = false;
  CXFA_Node* pFormNode = nullptr;
  if (pNode) {
    bChanged = ResetSingleNodeData(pNode);
    pFormNode = pNode;
  } else {
    pFormNode = GetRootSubform();
  }
  if (!pFormNode)
    return;

  if (pFormNode->GetElementType() != XFA_Element::Field &&
      pFormNode->GetElementType() != XFA_Element::ExclGroup) {
    CXFA_ReadyNodeIterator it(pFormNode);
    while (CXFA_Node* next_node = it.MoveToNext()) {
      bChanged |= ResetSingleNodeData(next_node);
      if (next_node->GetElementType() == XFA_Element::ExclGroup)
        it.SkipTree();
    }
  }
  if (bChanged)
    m_pDoc->GetDocEnvironment()->SetChangeMark(m_pDoc.Get());
}

CXFA_FFWidget* CXFA_FFDocView::GetWidgetForNode(CXFA_Node* node) {
  return ToFFWidget(ToContentLayoutItem(GetXFALayout()->GetLayoutItem(node)));
}

CXFA_FFWidgetHandler* CXFA_FFDocView::GetWidgetHandler() {
  if (!m_pWidgetHandler)
    m_pWidgetHandler = pdfium::MakeUnique<CXFA_FFWidgetHandler>(this);
  return m_pWidgetHandler.get();
}

std::unique_ptr<CXFA_ReadyNodeIterator>
CXFA_FFDocView::CreateReadyNodeIterator() {
  CXFA_Subform* pFormRoot = GetRootSubform();
  return pFormRoot ? pdfium::MakeUnique<CXFA_ReadyNodeIterator>(pFormRoot)
                   : nullptr;
}

bool CXFA_FFDocView::SetFocus(CXFA_FFWidget* pNewFocus) {
  CXFA_FFWidget* pOldFocus = m_pFocusWidget.Get();

  if (pOldFocus == pNewFocus)
    return false;

  if (pOldFocus) {
    if (!(pOldFocus->GetStatus() & XFA_WidgetStatus_Focused) &&
        (pOldFocus->GetStatus() & XFA_WidgetStatus_Visible)) {
      if (!pOldFocus->IsLoaded())
        pOldFocus->LoadWidget();

      pOldFocus->OnSetFocus(pOldFocus);
    }

    pOldFocus->OnKillFocus(pNewFocus);
  }

  if (pNewFocus) {
    if (pNewFocus->GetStatus() & XFA_WidgetStatus_Visible) {
      if (!pNewFocus->IsLoaded())
        pNewFocus->LoadWidget();

      pNewFocus->OnSetFocus(pOldFocus);
    }

    CXFA_Node* node = pNewFocus->GetNode();
    m_pFocusNode = node->IsWidgetReady() ? node : nullptr;
    m_pFocusWidget = pNewFocus;
  } else {
    m_pFocusNode = nullptr;
    m_pFocusWidget = nullptr;
  }

  return true;
}

void CXFA_FFDocView::SetFocusNode(CXFA_Node* node) {
  CXFA_FFWidget* pNewFocus = nullptr;
  if (node)
    pNewFocus = GetWidgetForNode(node);
  if (!SetFocus(pNewFocus))
    return;

  m_pFocusNode = node;
  if (m_iStatus != XFA_DOCVIEW_LAYOUTSTATUS_End)
    return;

  m_pDoc->GetDocEnvironment()->SetFocusWidget(m_pDoc.Get(),
                                              m_pFocusWidget.Get());
}

void CXFA_FFDocView::DeleteLayoutItem(CXFA_FFWidget* pWidget) {
  if (m_pFocusNode != pWidget->GetNode())
    return;

  m_pFocusNode = nullptr;
  m_pFocusWidget = nullptr;
}

static int32_t XFA_ProcessEvent(CXFA_FFDocView* pDocView,
                                CXFA_Node* pNode,
                                CXFA_EventParam* pParam) {
  if (!pParam || pParam->m_eType == XFA_EVENT_Unknown)
    return XFA_EVENTERROR_NotExist;
  if (pNode && pNode->GetElementType() == XFA_Element::Draw)
    return XFA_EVENTERROR_NotExist;

  switch (pParam->m_eType) {
    case XFA_EVENT_Calculate:
      return pNode->ProcessCalculate(pDocView);
    case XFA_EVENT_Validate:
      if (pDocView->GetDoc()->GetDocEnvironment()->IsValidationsEnabled(
              pDocView->GetDoc())) {
        return pNode->ProcessValidate(pDocView, 0x01);
      }
      return XFA_EVENTERROR_Disabled;
    case XFA_EVENT_InitCalculate: {
      CXFA_Calculate* calc = pNode->GetCalculateIfExists();
      if (!calc)
        return XFA_EVENTERROR_NotExist;
      if (pNode->IsUserInteractive())
        return XFA_EVENTERROR_Disabled;

      return pNode->ExecuteScript(pDocView, calc->GetScriptIfExists(), pParam);
    }
    default:
      break;
  }

  return pNode->ProcessEvent(pDocView, gs_EventActivity[pParam->m_eType],
                             pParam);
}

int32_t CXFA_FFDocView::ExecEventActivityByDeepFirst(CXFA_Node* pFormNode,
                                                     XFA_EVENTTYPE eEventType,
                                                     bool bIsFormReady,
                                                     bool bRecursive) {
  if (!pFormNode)
    return XFA_EVENTERROR_NotExist;

  XFA_Element elementType = pFormNode->GetElementType();
  if (elementType == XFA_Element::Field) {
    if (eEventType == XFA_EVENT_IndexChange)
      return XFA_EVENTERROR_NotExist;

    if (!pFormNode->IsWidgetReady())
      return XFA_EVENTERROR_NotExist;

    CXFA_EventParam eParam;
    eParam.m_eType = eEventType;
    eParam.m_pTarget = pFormNode;
    eParam.m_bIsFormReady = bIsFormReady;
    return XFA_ProcessEvent(this, pFormNode, &eParam);
  }

  int32_t iRet = XFA_EVENTERROR_NotExist;
  if (bRecursive) {
    for (CXFA_Node* pNode = pFormNode->GetFirstContainerChild(); pNode;
         pNode = pNode->GetNextContainerSibling()) {
      elementType = pNode->GetElementType();
      if (elementType != XFA_Element::Variables &&
          elementType != XFA_Element::Draw) {
        iRet |= ExecEventActivityByDeepFirst(pNode, eEventType, bIsFormReady,
                                             bRecursive);
      }
    }
  }
  if (!pFormNode->IsWidgetReady())
    return iRet;

  CXFA_EventParam eParam;
  eParam.m_eType = eEventType;
  eParam.m_pTarget = pFormNode;
  eParam.m_bIsFormReady = bIsFormReady;
  iRet |= XFA_ProcessEvent(this, pFormNode, &eParam);

  return iRet;
}

CXFA_FFWidget* CXFA_FFDocView::GetWidgetByName(const WideString& wsName,
                                               CXFA_FFWidget* pRefWidget) {
//   CFXJSE_Engine* pScriptContext = m_pDoc->GetXFADoc()->GetScriptContext();
//   if (!pScriptContext)
//     return nullptr;
// 
//   CXFA_Node* pRefNode = nullptr;
//   if (pRefWidget) {
//     CXFA_Node* node = pRefWidget->GetNode();
//     pRefNode = node->IsWidgetReady() ? node : nullptr;
//   }
//   WideString wsExpression = (!pRefNode ? L"$form." : L"") + wsName;
// 
//   XFA_RESOLVENODE_RS resolveNodeRS;
//   uint32_t dwStyle = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Properties |
//                      XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_Parent;
//   if (!pScriptContext->ResolveObjects(pRefNode, wsExpression.AsStringView(),
//                                       &resolveNodeRS, dwStyle, nullptr)) {
//     return nullptr;
//   }
// 
//   if (resolveNodeRS.dwFlags == XFA_ResolveNode_RSType_Nodes) {
//     CXFA_Node* pNode = resolveNodeRS.objects.front()->AsNode();
//     if (pNode && pNode->IsWidgetReady())
//       return GetWidgetForNode(pNode);
//   }
//   return nullptr;
  DebugBreak();
  return nullptr;
}

void CXFA_FFDocView::OnPageEvent(CXFA_ContainerLayoutItem* pSender,
                                 uint32_t dwEvent) {
  CXFA_FFPageView* pFFPageView = static_cast<CXFA_FFPageView*>(pSender);
  m_pDoc->GetDocEnvironment()->PageViewEvent(pFFPageView, dwEvent);
}

void CXFA_FFDocView::InvalidateRect(CXFA_FFPageView* pPageView,
                                    const CFX_RectF& rtInvalidate) {
  m_pDoc->GetDocEnvironment()->InvalidateRect(pPageView, rtInvalidate);
}

bool CXFA_FFDocView::RunLayout() {
  LockUpdate();
  m_bInLayoutStatus = true;
  if (!m_pXFADocLayout->IncrementLayout() &&
      m_pXFADocLayout->StartLayout(false) < 100) {
    m_pXFADocLayout->DoLayout();
    UnlockUpdate();
    m_bInLayoutStatus = false;
    m_pDoc->GetDocEnvironment()->PageViewEvent(nullptr,
                                               XFA_PAGEVIEWEVENT_StopLayout);
    return true;
  }

  m_bInLayoutStatus = false;
  m_pDoc->GetDocEnvironment()->PageViewEvent(nullptr,
                                             XFA_PAGEVIEWEVENT_StopLayout);
  UnlockUpdate();
  return false;
}

void CXFA_FFDocView::RunSubformIndexChange() {
  for (CXFA_Node* pSubformNode : m_IndexChangedSubforms) {
    if (!pSubformNode->IsWidgetReady())
      continue;

    CXFA_EventParam eParam;
    eParam.m_eType = XFA_EVENT_IndexChange;
    eParam.m_pTarget = pSubformNode;
    pSubformNode->ProcessEvent(this, XFA_AttributeValue::IndexChange, &eParam);
  }
  m_IndexChangedSubforms.clear();
}

void CXFA_FFDocView::AddNewFormNode(CXFA_Node* pNode) {
  m_NewAddedNodes.push_back(pNode);
  InitLayout(pNode);
}

void CXFA_FFDocView::AddIndexChangedSubform(CXFA_Node* pNode) {
  ASSERT(pNode->GetElementType() == XFA_Element::Subform);
  m_IndexChangedSubforms.push_back(pNode);
}

void CXFA_FFDocView::RunDocClose() {
  CXFA_Node* pRootItem =
      ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
  if (!pRootItem)
    return;

  ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_DocClose, false, true);
}

void CXFA_FFDocView::AddCalculateNode(CXFA_Node* node) {
  CXFA_Node* pCurrentNode =
      !m_CalculateNodes.empty() ? m_CalculateNodes.back() : nullptr;
  if (pCurrentNode != node)
    m_CalculateNodes.push_back(node);
}

void CXFA_FFDocView::AddCalculateNodeNotify(CXFA_Node* pNodeChange) {
  CXFA_CalcData* pGlobalData = pNodeChange->JSObject()->GetCalcData();
  if (!pGlobalData)
    return;

  for (auto* pResult : pGlobalData->m_Globals) {
    if (!pResult->HasRemovedChildren() && pResult->IsWidgetReady())
      AddCalculateNode(pResult);
  }
}

size_t CXFA_FFDocView::RunCalculateRecursive(size_t index) {
  while (index < m_CalculateNodes.size()) {
    CXFA_Node* node = m_CalculateNodes[index];

    AddCalculateNodeNotify(node);
    size_t recurse = node->JSObject()->GetCalcRecursionCount() + 1;
    node->JSObject()->SetCalcRecursionCount(recurse);
    if (recurse > 11)
      break;
    if (node->ProcessCalculate(this) == XFA_EVENTERROR_Success &&
        node->IsWidgetReady()) {
      AddValidateNode(node);
    }

    index = RunCalculateRecursive(++index);
  }
  return index;
}

int32_t CXFA_FFDocView::RunCalculateWidgets() {
  if (!m_pDoc->GetDocEnvironment()->IsCalculationsEnabled(m_pDoc.Get()))
    return XFA_EVENTERROR_Disabled;
  if (!m_CalculateNodes.empty())
    RunCalculateRecursive(0);

  for (CXFA_Node* node : m_CalculateNodes)
    node->JSObject()->SetCalcRecursionCount(0);

  m_CalculateNodes.clear();
  return XFA_EVENTERROR_Success;
}

void CXFA_FFDocView::AddValidateNode(CXFA_Node* node) {
  if (!pdfium::ContainsValue(m_ValidateNodes, node))
    m_ValidateNodes.push_back(node);
}

void CXFA_FFDocView::InitCalculate(CXFA_Node* pNode) {
  ExecEventActivityByDeepFirst(pNode, XFA_EVENT_InitCalculate, false, true);
}

void CXFA_FFDocView::ProcessValueChanged(CXFA_Node* node) {
  AddValidateNode(node);
  AddCalculateNode(node);
  RunCalculateWidgets();
  RunValidate();
}

bool CXFA_FFDocView::InitValidate(CXFA_Node* pNode) {
  if (!m_pDoc->GetDocEnvironment()->IsValidationsEnabled(m_pDoc.Get()))
    return false;

  ExecEventActivityByDeepFirst(pNode, XFA_EVENT_Validate, false, true);
  m_ValidateNodes.clear();
  return true;
}

bool CXFA_FFDocView::RunValidate() {
  if (!m_pDoc->GetDocEnvironment()->IsValidationsEnabled(m_pDoc.Get()))
    return false;

  for (CXFA_Node* node : m_ValidateNodes) {
    if (!node->HasRemovedChildren())
      node->ProcessValidate(this, 0);
  }
  m_ValidateNodes.clear();
  return true;
}

bool CXFA_FFDocView::RunEventLayoutReady() {
  CXFA_Node* pRootItem =
      ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
  if (!pRootItem)
    return false;

  ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_Ready, false, true);
  RunLayout();
  return true;
}

void CXFA_FFDocView::RunBindItems() {
//   for (auto* item : m_BindItems) {
//     if (item->HasRemovedChildren())
//       continue;
// 
//     CXFA_Node* pWidgetNode = item->GetParent();
//     if (!pWidgetNode->IsWidgetReady())
//       continue;
// 
//     CFXJSE_Engine* pScriptContext =
//         pWidgetNode->GetDocument()->GetScriptContext();
//     WideString wsRef = item->GetRef();
//     uint32_t dwStyle = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Properties |
//                        XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_Parent |
//                        XFA_RESOLVENODE_ALL;
//     XFA_RESOLVENODE_RS rs;
//     pScriptContext->ResolveObjects(pWidgetNode, wsRef.AsStringView(), &rs,
//                                    dwStyle, nullptr);
//     pWidgetNode->DeleteItem(-1, false, false);
//     if (rs.dwFlags != XFA_ResolveNode_RSType_Nodes || rs.objects.empty())
//       continue;
// 
//     WideString wsValueRef = item->GetValueRef();
//     WideString wsLabelRef = item->GetLabelRef();
//     const bool bUseValue = wsLabelRef.IsEmpty() || wsLabelRef == wsValueRef;
//     const bool bLabelUseContent =
//         wsLabelRef.IsEmpty() || wsLabelRef.EqualsASCII("$");
//     const bool bValueUseContent =
//         wsValueRef.IsEmpty() || wsValueRef.EqualsASCII("$");
//     WideString wsValue;
//     WideString wsLabel;
//     uint32_t uValueHash = FX_HashCode_GetW(wsValueRef.AsStringView(), false);
//     for (auto& refObject : rs.objects) {
//       CXFA_Node* refNode = refObject->AsNode();
//       if (!refNode)
//         continue;
// 
//       if (bValueUseContent) {
//         wsValue = refNode->JSObject()->GetContent(false);
//       } else {
//         CXFA_Node* nodeValue = refNode->GetFirstChildByName(uValueHash);
//         wsValue = nodeValue ? nodeValue->JSObject()->GetContent(false)
//                             : refNode->JSObject()->GetContent(false);
//       }
// 
//       if (!bUseValue) {
//         if (bLabelUseContent) {
//           wsLabel = refNode->JSObject()->GetContent(false);
//         } else {
//           CXFA_Node* nodeLabel =
//               refNode->GetFirstChildByName(wsLabelRef.AsStringView());
//           if (nodeLabel)
//             wsLabel = nodeLabel->JSObject()->GetContent(false);
//         }
//       } else {
//         wsLabel = wsValue;
//       }
//       pWidgetNode->InsertItem(wsLabel, wsValue, false);
//     }
//   }
//   m_BindItems.clear();
  DebugBreak();
}

void CXFA_FFDocView::SetChangeMark() {
  if (m_iStatus < XFA_DOCVIEW_LAYOUTSTATUS_End)
    return;

  m_pDoc->GetDocEnvironment()->SetChangeMark(m_pDoc.Get());
}

CXFA_Subform* CXFA_FFDocView::GetRootSubform() {
  CXFA_Node* pFormPacketNode =
      ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
  if (!pFormPacketNode)
    return nullptr;

  return pFormPacketNode->GetFirstChildByClass<CXFA_Subform>(
      XFA_Element::Subform);
}
