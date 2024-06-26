// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_LAYOUTPROCESSOR_H_
#define XFA_FXFA_PARSER_CXFA_LAYOUTPROCESSOR_H_

#include <memory>
#include <vector>

#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CXFA_ContainerLayoutItem;
class CXFA_Document;
class CXFA_ItemLayoutProcessor;
class CXFA_LayoutItem;
class CXFA_LayoutPageMgr;
class CXFA_Node;

class CXFA_LayoutProcessor {
 public:
  explicit CXFA_LayoutProcessor(CXFA_Document* pDocument);
  ~CXFA_LayoutProcessor();

  CXFA_Document* GetDocument() const;
  int32_t StartLayout(bool bForceRestart);
  int32_t DoLayout();
  bool IncrementLayout();
  int32_t CountPages() const;
  CXFA_ContainerLayoutItem* GetPage(int32_t index) const;
  CXFA_LayoutItem* GetLayoutItem(CXFA_Node* pFormItem);
  void AddChangedContainer(CXFA_Node* pContainer);
  void SetForceReLayout(bool bForceRestart) { m_bNeedLayout = bForceRestart; }
  CXFA_ContainerLayoutItem* GetRootLayoutItem() const;
  CXFA_ItemLayoutProcessor* GetRootRootItemLayoutProcessor() const {
    return m_pRootItemLayoutProcessor.get();
  }
  CXFA_LayoutPageMgr* GetLayoutPageMgr() const {
    return m_pLayoutPageMgr.get();
  }

 private:
  bool IsNeedLayout();

  UnownedPtr<CXFA_Document> const m_pDocument;
  std::unique_ptr<CXFA_LayoutPageMgr> m_pLayoutPageMgr;
  std::unique_ptr<CXFA_ItemLayoutProcessor> m_pRootItemLayoutProcessor;
  std::vector<CXFA_Node*> m_rgChangedContainers;
  uint32_t m_nProgressCounter;
  bool m_bNeedLayout;
};

#endif  // XFA_FXFA_PARSER_CXFA_LAYOUTPROCESSOR_H_
