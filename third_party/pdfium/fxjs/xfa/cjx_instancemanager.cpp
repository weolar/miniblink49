// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_instancemanager.h"

#include <algorithm>
#include <vector>

#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_instancemanager.h"
#include "xfa/fxfa/parser/cxfa_layoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_occur.h"

const CJX_MethodSpec CJX_InstanceManager::MethodSpecs[] = {
    {"addInstance", addInstance_static},
    {"insertInstance", insertInstance_static},
    {"moveInstance", moveInstance_static},
    {"removeInstance", removeInstance_static},
    {"setInstances", setInstances_static}};

CJX_InstanceManager::CJX_InstanceManager(CXFA_InstanceManager* mgr)
    : CJX_Node(mgr) {
  DefineMethods(MethodSpecs);
}

CJX_InstanceManager::~CJX_InstanceManager() {}

bool CJX_InstanceManager::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

int32_t CJX_InstanceManager::SetInstances(int32_t iDesired) {
  CXFA_Occur* occur = GetXFANode()->GetOccurIfExists();
  int32_t iMin = occur ? occur->GetMin() : CXFA_Occur::kDefaultMin;
  if (iDesired < iMin) {
    ThrowTooManyOccurancesException(L"min");
    return 1;
  }

  int32_t iMax = occur ? occur->GetMax() : CXFA_Occur::kDefaultMax;
  if (iMax >= 0 && iDesired > iMax) {
    ThrowTooManyOccurancesException(L"max");
    return 2;
  }

  int32_t iCount = GetXFANode()->GetCount();
  if (iDesired == iCount)
    return 0;

  if (iDesired < iCount) {
    WideString wsInstManagerName = GetCData(XFA_Attribute::Name);
    WideString wsInstanceName = WideString(
        wsInstManagerName.IsEmpty()
            ? wsInstManagerName
            : wsInstManagerName.Right(wsInstManagerName.GetLength() - 1));
    uint32_t dInstanceNameHash =
        FX_HashCode_GetW(wsInstanceName.AsStringView(), false);
    CXFA_Node* pPrevSibling = iDesired == 0
                                  ? GetXFANode()
                                  : GetXFANode()->GetItemIfExists(iDesired - 1);
    if (!pPrevSibling) {
      // TODO(dsinclair): Better error?
      ThrowIndexOutOfBoundsException();
      return 0;
    }

    while (iCount > iDesired) {
      CXFA_Node* pRemoveInstance = pPrevSibling->GetNextSibling();
      if (pRemoveInstance->GetElementType() != XFA_Element::Subform &&
          pRemoveInstance->GetElementType() != XFA_Element::SubformSet) {
        continue;
      }
      if (pRemoveInstance->GetElementType() == XFA_Element::InstanceManager) {
        NOTREACHED();
        break;
      }
      if (pRemoveInstance->GetNameHash() == dInstanceNameHash) {
        GetXFANode()->RemoveItem(pRemoveInstance, true);
        iCount--;
      }
    }
  } else {
    while (iCount < iDesired) {
      CXFA_Node* pNewInstance = GetXFANode()->CreateInstanceIfPossible(true);
      if (!pNewInstance)
        return 0;

      GetXFANode()->InsertItem(pNewInstance, iCount, iCount, false);
      ++iCount;

      CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
      if (!pNotify)
        return 0;

      pNotify->RunNodeInitialize(pNewInstance);
    }
  }

  CXFA_LayoutProcessor* pLayoutPro = GetDocument()->GetLayoutProcessor();
  if (pLayoutPro) {
    pLayoutPro->AddChangedContainer(
        ToNode(GetDocument()->GetXFAObject(XFA_HASHCODE_Form)));
  }
  return 0;
}

int32_t CJX_InstanceManager::MoveInstance(int32_t iTo, int32_t iFrom) {
  int32_t iCount = GetXFANode()->GetCount();
  if (iFrom > iCount || iTo > iCount - 1) {
    ThrowIndexOutOfBoundsException();
    return 1;
  }
  if (iFrom < 0 || iTo < 0 || iFrom == iTo)
    return 0;

  CXFA_Node* pMoveInstance = GetXFANode()->GetItemIfExists(iFrom);
  if (!pMoveInstance) {
    ThrowIndexOutOfBoundsException();
    return 1;
  }

  GetXFANode()->RemoveItem(pMoveInstance, false);
  GetXFANode()->InsertItem(pMoveInstance, iTo, iCount - 1, true);
  CXFA_LayoutProcessor* pLayoutPro = GetDocument()->GetLayoutProcessor();
  if (pLayoutPro) {
    pLayoutPro->AddChangedContainer(
        ToNode(GetDocument()->GetXFAObject(XFA_HASHCODE_Form)));
  }
  return 0;
}

CJS_Result CJX_InstanceManager::moveInstance(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  CXFA_Document* doc = static_cast<CFXJSE_Engine*>(runtime)->GetDocument();
  if (doc->GetFormType() != FormType::kXFAFull)
    return CJS_Result::Failure(JSMessage::kNotSupportedError);

  if (params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  int32_t iFrom = runtime->ToInt32(params[0]);
  int32_t iTo = runtime->ToInt32(params[1]);
  MoveInstance(iTo, iFrom);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (!pNotify)
    return CJS_Result::Success();

  CXFA_Node* pToInstance = GetXFANode()->GetItemIfExists(iTo);
  if (pToInstance && pToInstance->GetElementType() == XFA_Element::Subform)
    pNotify->RunSubformIndexChange(pToInstance);

  CXFA_Node* pFromInstance = GetXFANode()->GetItemIfExists(iFrom);
  if (pFromInstance &&
      pFromInstance->GetElementType() == XFA_Element::Subform) {
    pNotify->RunSubformIndexChange(pFromInstance);
  }

  return CJS_Result::Success();
}

CJS_Result CJX_InstanceManager::removeInstance(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  CXFA_Document* doc = static_cast<CFXJSE_Engine*>(runtime)->GetDocument();
  if (doc->GetFormType() != FormType::kXFAFull)
    return CJS_Result::Failure(JSMessage::kNotSupportedError);

  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  int32_t iIndex = runtime->ToInt32(params[0]);
  int32_t iCount = GetXFANode()->GetCount();
  if (iIndex < 0 || iIndex >= iCount)
    return CJS_Result::Failure(JSMessage::kInvalidInputError);

  CXFA_Occur* occur = GetXFANode()->GetOccurIfExists();
  int32_t iMin = occur ? occur->GetMin() : CXFA_Occur::kDefaultMin;
  if (iCount - 1 < iMin)
    return CJS_Result::Failure(JSMessage::kTooManyOccurances);

  CXFA_Node* pRemoveInstance = GetXFANode()->GetItemIfExists(iIndex);
  if (!pRemoveInstance)
    return CJS_Result::Failure(JSMessage::kParamError);

  GetXFANode()->RemoveItem(pRemoveInstance, true);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (pNotify) {
    for (int32_t i = iIndex; i < iCount - 1; i++) {
      CXFA_Node* pSubformInstance = GetXFANode()->GetItemIfExists(i);
      if (pSubformInstance &&
          pSubformInstance->GetElementType() == XFA_Element::Subform) {
        pNotify->RunSubformIndexChange(pSubformInstance);
      }
    }
  }
  CXFA_LayoutProcessor* pLayoutPro = GetDocument()->GetLayoutProcessor();
  if (pLayoutPro) {
    pLayoutPro->AddChangedContainer(
        ToNode(GetDocument()->GetXFAObject(XFA_HASHCODE_Form)));
  }
  return CJS_Result::Success();
}

CJS_Result CJX_InstanceManager::setInstances(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  CXFA_Document* doc = static_cast<CFXJSE_Engine*>(runtime)->GetDocument();
  if (doc->GetFormType() != FormType::kXFAFull)
    return CJS_Result::Failure(JSMessage::kNotSupportedError);

  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  SetInstances(runtime->ToInt32(params[0]));
  return CJS_Result::Success();
}

CJS_Result CJX_InstanceManager::addInstance(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  CXFA_Document* doc = static_cast<CFXJSE_Engine*>(runtime)->GetDocument();
  if (doc->GetFormType() != FormType::kXFAFull)
    return CJS_Result::Failure(JSMessage::kNotSupportedError);

  if (!params.empty() && params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  bool fFlags = true;
  if (params.size() == 1)
    fFlags = runtime->ToBoolean(params[0]);

  int32_t iCount = GetXFANode()->GetCount();
  CXFA_Occur* occur = GetXFANode()->GetOccurIfExists();
  int32_t iMax = occur ? occur->GetMax() : CXFA_Occur::kDefaultMax;
  if (iMax >= 0 && iCount >= iMax)
    return CJS_Result::Failure(JSMessage::kTooManyOccurances);

  CXFA_Node* pNewInstance = GetXFANode()->CreateInstanceIfPossible(fFlags);
  if (!pNewInstance)
    return CJS_Result::Success(runtime->NewNull());

  GetXFANode()->InsertItem(pNewInstance, iCount, iCount, false);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (pNotify) {
    pNotify->RunNodeInitialize(pNewInstance);

    CXFA_LayoutProcessor* pLayoutPro = GetDocument()->GetLayoutProcessor();
    if (pLayoutPro) {
      pLayoutPro->AddChangedContainer(
          ToNode(GetDocument()->GetXFAObject(XFA_HASHCODE_Form)));
    }
  }

  CFXJSE_Value* value =
      GetDocument()->GetScriptContext()->GetJSValueFromMap(pNewInstance);
  if (!value)
    return CJS_Result::Success(runtime->NewNull());

  return CJS_Result::Success(
      value->DirectGetValue().Get(runtime->GetIsolate()));
}

CJS_Result CJX_InstanceManager::insertInstance(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  CXFA_Document* doc = static_cast<CFXJSE_Engine*>(runtime)->GetDocument();
  if (doc->GetFormType() != FormType::kXFAFull)
    return CJS_Result::Failure(JSMessage::kNotSupportedError);

  if (params.size() != 1 && params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  int32_t iIndex = runtime->ToInt32(params[0]);
  bool bBind = false;
  if (params.size() == 2)
    bBind = runtime->ToBoolean(params[1]);

  int32_t iCount = GetXFANode()->GetCount();
  if (iIndex < 0 || iIndex > iCount)
    return CJS_Result::Failure(JSMessage::kInvalidInputError);

  CXFA_Occur* occur = GetXFANode()->GetOccurIfExists();
  int32_t iMax = occur ? occur->GetMax() : CXFA_Occur::kDefaultMax;
  if (iMax >= 0 && iCount >= iMax)
    return CJS_Result::Failure(JSMessage::kInvalidInputError);

  CXFA_Node* pNewInstance = GetXFANode()->CreateInstanceIfPossible(bBind);
  if (!pNewInstance)
    return CJS_Result::Success(runtime->NewNull());

  GetXFANode()->InsertItem(pNewInstance, iIndex, iCount, true);

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (pNotify) {
    pNotify->RunNodeInitialize(pNewInstance);
    CXFA_LayoutProcessor* pLayoutPro = GetDocument()->GetLayoutProcessor();
    if (pLayoutPro) {
      pLayoutPro->AddChangedContainer(
          ToNode(GetDocument()->GetXFAObject(XFA_HASHCODE_Form)));
    }
  }

  CFXJSE_Value* value =
      GetDocument()->GetScriptContext()->GetJSValueFromMap(pNewInstance);
  if (!value)
    return CJS_Result::Success(runtime->NewNull());

  return CJS_Result::Success(
      value->DirectGetValue().Get(runtime->GetIsolate()));
}

void CJX_InstanceManager::max(CFXJSE_Value* pValue,
                              bool bSetting,
                              XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }
  CXFA_Occur* occur = GetXFANode()->GetOccurIfExists();
  pValue->SetInteger(occur ? occur->GetMax() : CXFA_Occur::kDefaultMax);
}

void CJX_InstanceManager::min(CFXJSE_Value* pValue,
                              bool bSetting,
                              XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }
  CXFA_Occur* occur = GetXFANode()->GetOccurIfExists();
  pValue->SetInteger(occur ? occur->GetMin() : CXFA_Occur::kDefaultMin);
}

void CJX_InstanceManager::count(CFXJSE_Value* pValue,
                                bool bSetting,
                                XFA_Attribute eAttribute) {
  if (bSetting) {
    SetInstances(pValue->ToInteger());
    return;
  }
  pValue->SetInteger(GetXFANode()->GetCount());
}
