// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CFXJSE_RESOLVEPROCESSOR_H_
#define FXJS_XFA_CFXJSE_RESOLVEPROCESSOR_H_

#include <memory>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "xfa/fxfa/fxfa_basic.h"
#include "xfa/fxfa/parser/xfa_basic_data.h"
#include "xfa/fxfa/parser/xfa_resolvenode_rs.h"

class CXFA_NodeHelper;

class CFXJSE_ResolveNodeData {
 public:
  explicit CFXJSE_ResolveNodeData(CFXJSE_Engine* pSC);
  ~CFXJSE_ResolveNodeData();

  UnownedPtr<CFXJSE_Engine> const m_pSC;
  UnownedPtr<CXFA_Object> m_CurObject;
  WideString m_wsName;
  WideString m_wsCondition;
  XFA_HashCode m_uHashName = XFA_HASHCODE_None;
  int32_t m_nLevel = 0;
  uint32_t m_dwStyles = XFA_RESOLVENODE_Children;
  XFA_ResolveNode_RSType m_dwFlag = XFA_ResolveNode_RSType_Nodes;
  std::vector<UnownedPtr<CXFA_Object>> m_Objects;
  XFA_SCRIPTATTRIBUTEINFO m_ScriptAttribute;
};

class CFXJSE_ResolveProcessor {
 public:
  CFXJSE_ResolveProcessor();
  ~CFXJSE_ResolveProcessor();

  bool Resolve(CFXJSE_ResolveNodeData& rnd);
  int32_t GetFilter(WideStringView wsExpression,
                    int32_t nStart,
                    CFXJSE_ResolveNodeData& rnd);
  void SetIndexDataBind(WideString& wsNextCondition,
                        int32_t& iIndex,
                        int32_t iCount);
  void SetCurStart(int32_t start) { m_iCurStart = start; }

  CXFA_NodeHelper* GetNodeHelper() { return m_pNodeHelper.get(); }

 private:
  bool ResolveForAttributeRs(CXFA_Object* curNode,
                             CFXJSE_ResolveNodeData& rnd,
                             WideStringView strAttr);
  bool ResolveAnyChild(CFXJSE_ResolveNodeData& rnd);
  bool ResolveDollar(CFXJSE_ResolveNodeData& rnd);
  bool ResolveExcalmatory(CFXJSE_ResolveNodeData& rnd);
  bool ResolveNumberSign(CFXJSE_ResolveNodeData& rnd);
  bool ResolveAsterisk(CFXJSE_ResolveNodeData& rnd);
  bool ResolveNormal(CFXJSE_ResolveNodeData& rnd);
  void SetStylesForChild(uint32_t dwParentStyles, CFXJSE_ResolveNodeData& rnd);

  void ConditionArray(int32_t iCurIndex,
                      WideString wsCondition,
                      int32_t iFoundCount,
                      CFXJSE_ResolveNodeData& rnd);
  void DoPredicateFilter(int32_t iCurIndex,
                         WideString wsCondition,
                         int32_t iFoundCount,
                         CFXJSE_ResolveNodeData& rnd);
  void FilterCondition(CFXJSE_ResolveNodeData& rnd, WideString wsCondition);

  int32_t m_iCurStart = 0;
  std::unique_ptr<CXFA_NodeHelper> const m_pNodeHelper;
};

#endif  // FXJS_XFA_CFXJSE_RESOLVEPROCESSOR_H_
