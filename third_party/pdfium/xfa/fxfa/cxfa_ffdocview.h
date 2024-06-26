// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFDOCVIEW_H_
#define XFA_FXFA_CXFA_FFDOCVIEW_H_

#include <memory>
#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffdoc.h"

class CXFA_BindItems;
class CXFA_ContainerLayoutItem;
class CXFA_FFWidgetHandler;
class CXFA_FFDoc;
class CXFA_FFWidget;
class CXFA_Node;
class CXFA_Subform;
class CXFA_ReadyNodeIterator;

extern const XFA_AttributeValue gs_EventActivity[];
enum XFA_DOCVIEW_LAYOUTSTATUS {
  XFA_DOCVIEW_LAYOUTSTATUS_None,
  XFA_DOCVIEW_LAYOUTSTATUS_Start,
  XFA_DOCVIEW_LAYOUTSTATUS_FormInitialize,
  XFA_DOCVIEW_LAYOUTSTATUS_FormInitCalculate,
  XFA_DOCVIEW_LAYOUTSTATUS_FormInitValidate,
  XFA_DOCVIEW_LAYOUTSTATUS_FormFormReady,
  XFA_DOCVIEW_LAYOUTSTATUS_Doing,
  XFA_DOCVIEW_LAYOUTSTATUS_PagesetInitialize,
  XFA_DOCVIEW_LAYOUTSTATUS_PagesetInitCalculate,
  XFA_DOCVIEW_LAYOUTSTATUS_PagesetInitValidate,
  XFA_DOCVIEW_LAYOUTSTATUS_PagesetFormReady,
  XFA_DOCVIEW_LAYOUTSTATUS_LayoutReady,
  XFA_DOCVIEW_LAYOUTSTATUS_DocReady,
  XFA_DOCVIEW_LAYOUTSTATUS_End
};

class CXFA_FFDocView {
 public:
  explicit CXFA_FFDocView(CXFA_FFDoc* pDoc);
  ~CXFA_FFDocView();

  CXFA_FFDoc* GetDoc() const { return m_pDoc.Get(); }
  int32_t StartLayout();
  int32_t DoLayout();
  void StopLayout();
  int32_t GetLayoutStatus() const { return m_iStatus; }
  void UpdateDocView();
  int32_t CountPageViews() const;
  CXFA_FFPageView* GetPageView(int32_t nIndex) const;

  void ResetNode(CXFA_Node* pNode);
  CXFA_FFWidgetHandler* GetWidgetHandler();
  std::unique_ptr<CXFA_ReadyNodeIterator> CreateReadyNodeIterator();
  CXFA_FFWidget* GetFocusWidget() const { return m_pFocusWidget.Get(); }
  bool SetFocus(CXFA_FFWidget* pNewFocus);
  CXFA_FFWidget* GetWidgetForNode(CXFA_Node* node);
  CXFA_FFWidget* GetWidgetByName(const WideString& wsName,
                                 CXFA_FFWidget* pRefWidget);
  CXFA_LayoutProcessor* GetXFALayout() const;
  void OnPageEvent(CXFA_ContainerLayoutItem* pSender, uint32_t dwEvent);
  void LockUpdate() { m_iLock++; }
  void UnlockUpdate() { m_iLock--; }
  void InvalidateRect(CXFA_FFPageView* pPageView,
                      const CFX_RectF& rtInvalidate);
  void RunDocClose();

  void ProcessValueChanged(CXFA_Node* node);

  void SetChangeMark();

  void AddValidateNode(CXFA_Node* node);
  void AddCalculateNodeNotify(CXFA_Node* pNodeChange);
  void AddCalculateNode(CXFA_Node* node);

  bool RunLayout();
  void AddNewFormNode(CXFA_Node* pNode);
  void AddIndexChangedSubform(CXFA_Node* pNode);
  CXFA_Node* GetFocusNode() const { return m_pFocusNode.Get(); }
  void SetFocusNode(CXFA_Node* pNode);
  void DeleteLayoutItem(CXFA_FFWidget* pWidget);
  int32_t ExecEventActivityByDeepFirst(CXFA_Node* pFormNode,
                                       XFA_EVENTTYPE eEventType,
                                       bool bIsFormReady,
                                       bool bRecursive);

  void AddBindItem(CXFA_BindItems* item) { m_BindItems.push_back(item); }

  bool m_bLayoutEvent = false;
  bool m_bInLayoutStatus = false;
  std::vector<WideString> m_arrNullTestMsg;

  void ResetLayoutProcessor() { m_pXFADocLayout.Release(); }

 private:
  bool RunEventLayoutReady();
  void RunBindItems();
  void InitCalculate(CXFA_Node* pNode);
  void InitLayout(CXFA_Node* pNode);
  size_t RunCalculateRecursive(size_t index);
  void ShowNullTestMsg();
  bool ResetSingleNodeData(CXFA_Node* pNode);
  CXFA_Subform* GetRootSubform();

  bool IsUpdateLocked() { return m_iLock > 0; }
  bool InitValidate(CXFA_Node* pNode);
  bool RunValidate();
  int32_t RunCalculateWidgets();
  void RunSubformIndexChange();

  UnownedPtr<CXFA_FFDoc> const m_pDoc;
  std::unique_ptr<CXFA_FFWidgetHandler> m_pWidgetHandler;
  UnownedPtr<CXFA_LayoutProcessor> m_pXFADocLayout;
  UnownedPtr<CXFA_Node> m_pFocusNode;
  UnownedPtr<CXFA_FFWidget> m_pFocusWidget;
  std::vector<CXFA_Node*> m_ValidateNodes;
  std::vector<CXFA_Node*> m_CalculateNodes;
  std::vector<CXFA_BindItems*> m_BindItems;
  std::vector<CXFA_Node*> m_NewAddedNodes;
  std::vector<CXFA_Node*> m_IndexChangedSubforms;
  XFA_DOCVIEW_LAYOUTSTATUS m_iStatus = XFA_DOCVIEW_LAYOUTSTATUS_None;
  int32_t m_iLock = 0;
};

#endif  // XFA_FXFA_CXFA_FFDOCVIEW_H_
