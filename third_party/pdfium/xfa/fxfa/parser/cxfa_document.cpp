// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_document.h"

#include <set>
#include <utility>

#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cjx_object.h"
#include "third_party/base/compiler_specific.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/parser/cscript_datawindow.h"
#include "xfa/fxfa/parser/cscript_eventpseudomodel.h"
#include "xfa/fxfa/parser/cscript_hostpseudomodel.h"
#include "xfa/fxfa/parser/cscript_layoutpseudomodel.h"
#include "xfa/fxfa/parser/cscript_logpseudomodel.h"
#include "xfa/fxfa/parser/cscript_signaturepseudomodel.h"
#include "xfa/fxfa/parser/cxfa_bind.h"
#include "xfa/fxfa/parser/cxfa_datagroup.h"
#include "xfa/fxfa/parser/cxfa_exdata.h"
#include "xfa/fxfa/parser/cxfa_form.h"
#include "xfa/fxfa/parser/cxfa_image.h"
#include "xfa/fxfa/parser/cxfa_interactive.h"
#include "xfa/fxfa/parser/cxfa_items.h"
#include "xfa/fxfa/parser/cxfa_layoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_localemgr.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_occur.h"
#include "xfa/fxfa/parser/cxfa_pageset.h"
#include "xfa/fxfa/parser/cxfa_pdf.h"
#include "xfa/fxfa/parser/cxfa_present.h"
#include "xfa/fxfa/parser/cxfa_subform.h"
#include "xfa/fxfa/parser/cxfa_template.h"
#include "xfa/fxfa/parser/cxfa_traversestrategy_xfacontainernode.h"
#include "xfa/fxfa/parser/cxfa_traversestrategy_xfanode.h"
#include "xfa/fxfa/parser/cxfa_value.h"
#include "xfa/fxfa/parser/xfa_document_datamerger_imp.h"
#include "xfa/fxfa/parser/xfa_resolvenode_rs.h"
#include "xfa/fxfa/parser/xfa_utils.h"

namespace {

constexpr const wchar_t kTemplateNS[] =
    L"http://www.xfa.org/schema/xfa-template/";

struct RecurseRecord {
  CXFA_Node* pTemplateChild;
  CXFA_Node* pDataChild;
};

class CXFA_TraverseStrategy_DDGroup {
 public:
  static CXFA_Node* GetFirstChild(CXFA_Node* pDDGroupNode) {
    return pDDGroupNode->GetFirstChildByName(XFA_HASHCODE_Group);
  }
  static CXFA_Node* GetNextSibling(CXFA_Node* pDDGroupNode) {
    return pDDGroupNode->GetNextSameNameSibling(XFA_HASHCODE_Group);
  }
  static CXFA_Node* GetParent(CXFA_Node* pDDGroupNode) {
    return pDDGroupNode->GetParent();
  }
};

void FormValueNode_MatchNoneCreateChild(CXFA_Node* pFormNode) {
  ASSERT(pFormNode->IsWidgetReady());
  // GetUIChildNode has the side effect of creating the UI child.
  pFormNode->GetUIChildNode();
}

CXFA_Node* FormValueNode_CreateChild(CXFA_Node* pValueNode, XFA_Element iType) {
  CXFA_Node* pChildNode = pValueNode->GetFirstChild();
  if (!pChildNode) {
    if (iType == XFA_Element::Unknown)
      return nullptr;

    pChildNode =
        pValueNode->JSObject()->GetOrCreateProperty<CXFA_Node>(0, iType);
  }
  return pChildNode;
}

void FormValueNode_SetChildContent(CXFA_Node* pValueNode,
                                   const WideString& wsContent,
                                   XFA_Element iType) {
  if (!pValueNode)
    return;

  ASSERT(pValueNode->GetPacketType() == XFA_PacketType::Form);
  CXFA_Node* pChildNode = FormValueNode_CreateChild(pValueNode, iType);
  if (!pChildNode)
    return;

  switch (pChildNode->GetObjectType()) {
    case XFA_ObjectType::ContentNode: {
      CXFA_Node* pContentRawDataNode = pChildNode->GetFirstChild();
      if (!pContentRawDataNode) {
        XFA_Element element = XFA_Element::Sharptext;
        if (pChildNode->GetElementType() == XFA_Element::ExData) {
          Optional<WideString> contentType =
              pChildNode->JSObject()->TryAttribute(XFA_Attribute::ContentType,
                                                   false);
          if (contentType.has_value()) {
            if (contentType.value().EqualsASCII("text/html"))
              element = XFA_Element::SharpxHTML;
            else if (contentType.value().EqualsASCII("text/xml"))
              element = XFA_Element::Sharpxml;
          }
        }
        pContentRawDataNode = pChildNode->CreateSamePacketNode(element);
        pChildNode->InsertChild(pContentRawDataNode, nullptr);
      }
      pContentRawDataNode->JSObject()->SetCData(XFA_Attribute::Value, wsContent,
                                                false, false);
      break;
    }
    case XFA_ObjectType::NodeC:
    case XFA_ObjectType::TextNode:
    case XFA_ObjectType::NodeV: {
      pChildNode->JSObject()->SetCData(XFA_Attribute::Value, wsContent, false,
                                       false);
      break;
    }
    default:
      break;
  }
}

void MergeNodeRecurse(CXFA_Node* pDestNodeParent, CXFA_Node* pProtoNode) {
  CXFA_Node* pExistingNode = nullptr;
  for (CXFA_Node* pFormChild = pDestNodeParent->GetFirstChild(); pFormChild;
       pFormChild = pFormChild->GetNextSibling()) {
    if (pFormChild->GetElementType() == pProtoNode->GetElementType() &&
        pFormChild->GetNameHash() == pProtoNode->GetNameHash() &&
        pFormChild->IsUnusedNode()) {
      pFormChild->ClearFlag(XFA_NodeFlag_UnusedNode);
      pExistingNode = pFormChild;
      break;
    }
  }

  if (pExistingNode) {
    pExistingNode->SetTemplateNode(pProtoNode);
    for (CXFA_Node* pTemplateChild = pProtoNode->GetFirstChild();
         pTemplateChild; pTemplateChild = pTemplateChild->GetNextSibling()) {
      MergeNodeRecurse(pExistingNode, pTemplateChild);
    }
    return;
  }
  CXFA_Node* pNewNode = pProtoNode->Clone(true);
  pNewNode->SetTemplateNode(pProtoNode);
  pDestNodeParent->InsertChild(pNewNode, nullptr);
}

void MergeNode(CXFA_Node* pDestNode, CXFA_Node* pProtoNode) {
  {
    CXFA_NodeIterator sIterator(pDestNode);
    for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
         pNode = sIterator.MoveToNext()) {
      pNode->SetFlag(XFA_NodeFlag_UnusedNode);
    }
  }
  pDestNode->SetTemplateNode(pProtoNode);
  for (CXFA_Node* pTemplateChild = pProtoNode->GetFirstChild(); pTemplateChild;
       pTemplateChild = pTemplateChild->GetNextSibling()) {
    MergeNodeRecurse(pDestNode, pTemplateChild);
  }
  {
    CXFA_NodeIterator sIterator(pDestNode);
    for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
         pNode = sIterator.MoveToNext()) {
      pNode->ClearFlag(XFA_NodeFlag_UnusedNode);
    }
  }
}

CXFA_Node* CloneOrMergeInstanceManager(CXFA_Document* pDocument,
                                       CXFA_Node* pFormParent,
                                       CXFA_Node* pTemplateNode,
                                       std::vector<CXFA_Node*>* subforms) {
  WideString wsSubformName =
      pTemplateNode->JSObject()->GetCData(XFA_Attribute::Name);
  WideString wsInstMgrNodeName = L"_" + wsSubformName;
  uint32_t dwInstNameHash =
      FX_HashCode_GetW(wsInstMgrNodeName.AsStringView(), false);
  CXFA_Node* pExistingNode = XFA_DataMerge_FindFormDOMInstance(
      pDocument, XFA_Element::InstanceManager, dwInstNameHash, pFormParent);
  if (pExistingNode) {
    uint32_t dwNameHash = pTemplateNode->GetNameHash();
    for (CXFA_Node* pNode = pExistingNode->GetNextSibling(); pNode;) {
      XFA_Element eCurType = pNode->GetElementType();
      if (eCurType == XFA_Element::InstanceManager)
        break;

      if ((eCurType != XFA_Element::Subform) &&
          (eCurType != XFA_Element::SubformSet)) {
        pNode = pNode->GetNextSibling();
        continue;
      }
      if (dwNameHash != pNode->GetNameHash())
        break;

      CXFA_Node* pNextNode = pNode->GetNextSibling();
      pFormParent->RemoveChild(pNode, true);
      subforms->push_back(pNode);
      pNode = pNextNode;
    }
    pFormParent->RemoveChild(pExistingNode, true);
    pFormParent->InsertChild(pExistingNode, nullptr);
    pExistingNode->ClearFlag(XFA_NodeFlag_UnusedNode);
    pExistingNode->SetTemplateNode(pTemplateNode);
    return pExistingNode;
  }

  CXFA_Node* pNewNode =
      pDocument->CreateNode(XFA_PacketType::Form, XFA_Element::InstanceManager);
  wsInstMgrNodeName =
      L"_" + pTemplateNode->JSObject()->GetCData(XFA_Attribute::Name);
  pNewNode->JSObject()->SetCData(XFA_Attribute::Name, wsInstMgrNodeName, false,
                                 false);
  pFormParent->InsertChild(pNewNode, nullptr);
  pNewNode->SetTemplateNode(pTemplateNode);
  return pNewNode;
}

void SortRecurseRecord(std::vector<RecurseRecord>* rgRecords,
                       CXFA_Node* pDataScope,
                       bool bChoiceMode) {
  std::vector<RecurseRecord> rgResultRecord;
  for (CXFA_Node* pNode = pDataScope->GetFirstChild(); pNode;
       pNode = pNode->GetNextSibling()) {
    auto it = std::find_if(rgRecords->begin(), rgRecords->end(),
                           [pNode](const RecurseRecord& record) {
                             return pNode == record.pDataChild;
                           });
    if (it != rgRecords->end()) {
      rgResultRecord.push_back(*it);
      rgRecords->erase(it);
      if (bChoiceMode)
        break;
    }
  }
  if (rgResultRecord.empty())
    return;

  if (!bChoiceMode) {
    rgResultRecord.insert(rgResultRecord.end(), rgRecords->begin(),
                          rgRecords->end());
  }
  *rgRecords = rgResultRecord;
}

CXFA_Node* ScopeMatchGlobalBinding(CXFA_Node* pDataScope,
                                   uint32_t dwNameHash,
                                   XFA_Element eMatchDataNodeType,
                                   bool bUpLevel) {
  for (CXFA_Node *pCurDataScope = pDataScope, *pLastDataScope = nullptr;
       pCurDataScope &&
       pCurDataScope->GetPacketType() == XFA_PacketType::Datasets;
       pLastDataScope = pCurDataScope,
                 pCurDataScope = pCurDataScope->GetParent()) {
    for (CXFA_Node* pDataChild = pCurDataScope->GetFirstChildByName(dwNameHash);
         pDataChild;
         pDataChild = pDataChild->GetNextSameNameSibling(dwNameHash)) {
      if (pDataChild == pLastDataScope ||
          (eMatchDataNodeType != XFA_Element::DataModel &&
           pDataChild->GetElementType() != eMatchDataNodeType) ||
          pDataChild->HasBindItem()) {
        continue;
      }
      return pDataChild;
    }

    for (CXFA_DataGroup* pDataChild =
             pCurDataScope->GetFirstChildByClass<CXFA_DataGroup>(
                 XFA_Element::DataGroup);
         pDataChild;
         pDataChild = pDataChild->GetNextSameClassSibling<CXFA_DataGroup>(
             XFA_Element::DataGroup)) {
      CXFA_Node* pDataNode = ScopeMatchGlobalBinding(pDataChild, dwNameHash,
                                                     eMatchDataNodeType, false);
      if (pDataNode)
        return pDataNode;
    }
    if (!bUpLevel)
      break;
  }
  return nullptr;
}

CXFA_Node* FindGlobalDataNode(CXFA_Document* pDocument,
                              const WideString& wsName,
                              CXFA_Node* pDataScope,
                              XFA_Element eMatchNodeType) {
  if (wsName.IsEmpty())
    return nullptr;

  uint32_t dwNameHash = FX_HashCode_GetW(wsName.AsStringView(), false);
  CXFA_Node* pBounded = pDocument->GetGlobalBinding(dwNameHash);
  if (!pBounded) {
    pBounded =
        ScopeMatchGlobalBinding(pDataScope, dwNameHash, eMatchNodeType, true);
    if (pBounded)
      pDocument->RegisterGlobalBinding(dwNameHash, pBounded);
  }
  return pBounded;
}

CXFA_Node* FindOnceDataNode(const WideString& wsName,
                            CXFA_Node* pDataScope,
                            XFA_Element eMatchNodeType) {
  if (wsName.IsEmpty())
    return nullptr;

  uint32_t dwNameHash = FX_HashCode_GetW(wsName.AsStringView(), false);
  CXFA_Node* pLastDataScope = nullptr;
  for (CXFA_Node* pCurDataScope = pDataScope;
       pCurDataScope &&
       pCurDataScope->GetPacketType() == XFA_PacketType::Datasets;
       pCurDataScope = pCurDataScope->GetParent()) {
    for (CXFA_Node* pDataChild = pCurDataScope->GetFirstChildByName(dwNameHash);
         pDataChild;
         pDataChild = pDataChild->GetNextSameNameSibling(dwNameHash)) {
      if (pDataChild == pLastDataScope || pDataChild->HasBindItem() ||
          (eMatchNodeType != XFA_Element::DataModel &&
           pDataChild->GetElementType() != eMatchNodeType)) {
        continue;
      }
      return pDataChild;
    }
    pLastDataScope = pCurDataScope;
  }
  return nullptr;
}

CXFA_Node* FindDataRefDataNode(CXFA_Document* pDocument,
                               const WideString& wsRef,
                               CXFA_Node* pDataScope,
                               XFA_Element eMatchNodeType,
                               CXFA_Node* pTemplateNode,
                               bool bForceBind,
                               bool bUpLevel) {
  uint32_t dFlags = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_BindNew;
  if (bUpLevel || !wsRef.EqualsASCII("name"))
    dFlags |= (XFA_RESOLVENODE_Parent | XFA_RESOLVENODE_Siblings);

  XFA_RESOLVENODE_RS rs;
  pDocument->GetScriptContext()->ResolveObjects(
      pDataScope, wsRef.AsStringView(), &rs, dFlags, pTemplateNode);
  if (rs.dwFlags == XFA_ResolveNode_RSType_CreateNodeAll ||
      rs.dwFlags == XFA_ResolveNode_RSType_CreateNodeMidAll ||
      rs.objects.size() > 1) {
    return pDocument->GetNotBindNode(rs.objects);
  }

  if (rs.dwFlags == XFA_ResolveNode_RSType_CreateNodeOne) {
    CXFA_Object* pObject =
        !rs.objects.empty() ? rs.objects.front().Get() : nullptr;
    CXFA_Node* pNode = ToNode(pObject);
    return (bForceBind || !pNode || !pNode->HasBindItem()) ? pNode : nullptr;
  }
  return nullptr;
}

CXFA_Node* FindMatchingDataNode(
    CXFA_Document* pDocument,
    CXFA_Node* pTemplateNode,
    CXFA_Node* pDataScope,
    bool& bAccessedDataDOM,
    bool bForceBind,
    CXFA_NodeIteratorTemplate<CXFA_Node,
                              CXFA_TraverseStrategy_XFAContainerNode>*
        pIterator,
    bool& bSelfMatch,
    XFA_AttributeValue& eBindMatch,
    bool bUpLevel) {
  CXFA_Node* pResult = nullptr;
  CXFA_Node* pCurTemplateNode = pIterator->GetCurrent();
  while (pCurTemplateNode) {
    XFA_Element eMatchNodeType;
    switch (pCurTemplateNode->GetElementType()) {
      case XFA_Element::Subform:
        eMatchNodeType = XFA_Element::DataGroup;
        break;
      case XFA_Element::Field: {
        eMatchNodeType = XFA_FieldIsMultiListBox(pCurTemplateNode)
                             ? XFA_Element::DataGroup
                             : XFA_Element::DataValue;
      } break;
      case XFA_Element::ExclGroup:
        eMatchNodeType = XFA_Element::DataValue;
        break;
      default:
        pCurTemplateNode = pIterator->MoveToNext();
        continue;
    }

    CXFA_Occur* pTemplateNodeOccur =
        pCurTemplateNode->GetFirstChildByClass<CXFA_Occur>(XFA_Element::Occur);
    if (pTemplateNodeOccur) {
      int32_t iMin;
      int32_t iMax;
      int32_t iInit;
      std::tie(iMin, iMax, iInit) = pTemplateNodeOccur->GetOccurInfo();
      if (iMax == 0) {
        pCurTemplateNode = pIterator->MoveToNext();
        continue;
      }
    }

    CXFA_Bind* pTemplateNodeBind =
        pCurTemplateNode->GetFirstChildByClass<CXFA_Bind>(XFA_Element::Bind);
    XFA_AttributeValue eMatch =
        pTemplateNodeBind
            ? pTemplateNodeBind->JSObject()->GetEnum(XFA_Attribute::Match)
            : XFA_AttributeValue::Once;
    eBindMatch = eMatch;
    switch (eMatch) {
      case XFA_AttributeValue::None:
        pCurTemplateNode = pIterator->MoveToNext();
        continue;
      case XFA_AttributeValue::Global:
        bAccessedDataDOM = true;
        if (!bForceBind) {
          pCurTemplateNode = pIterator->MoveToNext();
          continue;
        }
        if (eMatchNodeType == XFA_Element::DataValue ||
            (eMatchNodeType == XFA_Element::DataGroup &&
             XFA_FieldIsMultiListBox(pTemplateNodeBind))) {
          CXFA_Node* pGlobalBindNode = FindGlobalDataNode(
              pDocument,
              pCurTemplateNode->JSObject()->GetCData(XFA_Attribute::Name),
              pDataScope, eMatchNodeType);
          if (!pGlobalBindNode) {
            pCurTemplateNode = pIterator->MoveToNext();
            continue;
          }
          pResult = pGlobalBindNode;
          break;
        }
        FALLTHROUGH;
      case XFA_AttributeValue::Once: {
        bAccessedDataDOM = true;
        CXFA_Node* pOnceBindNode = FindOnceDataNode(
            pCurTemplateNode->JSObject()->GetCData(XFA_Attribute::Name),
            pDataScope, eMatchNodeType);
        if (!pOnceBindNode) {
          pCurTemplateNode = pIterator->MoveToNext();
          continue;
        }
        pResult = pOnceBindNode;
        break;
      }
      case XFA_AttributeValue::DataRef: {
        bAccessedDataDOM = true;
        CXFA_Node* pDataRefBindNode = FindDataRefDataNode(
            pDocument,
            pTemplateNodeBind->JSObject()->GetCData(XFA_Attribute::Ref),
            pDataScope, eMatchNodeType, pTemplateNode, bForceBind, bUpLevel);
        if (pDataRefBindNode &&
            pDataRefBindNode->GetElementType() == eMatchNodeType) {
          pResult = pDataRefBindNode;
        }
        if (!pResult) {
          pCurTemplateNode = pIterator->SkipChildrenAndMoveToNext();
          continue;
        }
        break;
      }
      default:
        break;
    }
    if (pCurTemplateNode == pTemplateNode && pResult)
      bSelfMatch = true;
    break;
  }
  return pResult;
}

void CreateDataBinding(CXFA_Node* pFormNode,
                       CXFA_Node* pDataNode,
                       bool bDataToForm) {
  pFormNode->SetBindingNode(pDataNode);
  pDataNode->AddBindItem(pFormNode);
  XFA_Element eType = pFormNode->GetElementType();
  if (eType != XFA_Element::Field && eType != XFA_Element::ExclGroup)
    return;

  ASSERT(pFormNode->IsWidgetReady());
  auto* defValue = pFormNode->JSObject()->GetOrCreateProperty<CXFA_Value>(
      0, XFA_Element::Value);
  if (!bDataToForm) {
    WideString wsValue;
    switch (pFormNode->GetFFWidgetType()) {
      case XFA_FFWidgetType::kImageEdit: {
        CXFA_Image* image = defValue ? defValue->GetImageIfExists() : nullptr;
        WideString wsContentType;
        WideString wsHref;
        if (image) {
          wsValue = image->GetContent();
          wsContentType = image->GetContentType();
          wsHref = image->GetHref();
        }
        CFX_XMLElement* pXMLDataElement =
            ToXMLElement(pDataNode->GetXMLMappingNode());
        ASSERT(pXMLDataElement);
        pDataNode->JSObject()->SetAttributeValue(
            wsValue, pFormNode->GetFormatDataValue(wsValue), false, false);
        pDataNode->JSObject()->SetCData(XFA_Attribute::ContentType,
                                        wsContentType, false, false);
        if (!wsHref.IsEmpty())
          pXMLDataElement->SetAttribute(L"href", wsHref);

        break;
      }
      case XFA_FFWidgetType::kChoiceList:
        wsValue = defValue ? defValue->GetChildValueContent() : WideString();
        if (pFormNode->IsChoiceListMultiSelect()) {
          std::vector<WideString> wsSelTextArray =
              pFormNode->GetSelectedItemsValue();
          if (!wsSelTextArray.empty()) {
            for (const auto& text : wsSelTextArray) {
              CXFA_Node* pValue =
                  pDataNode->CreateSamePacketNode(XFA_Element::DataValue);
              pValue->JSObject()->SetCData(XFA_Attribute::Name, L"value", false,
                                           false);
              pValue->CreateXMLMappingNode();
              pDataNode->InsertChild(pValue, nullptr);
              pValue->JSObject()->SetCData(XFA_Attribute::Value, text, false,
                                           false);
            }
          } else {
            CFX_XMLElement* pElement =
                ToXMLElement(pDataNode->GetXMLMappingNode());
            pElement->SetAttribute(L"xfa:dataNode", L"dataGroup");
          }
        } else if (!wsValue.IsEmpty()) {
          pDataNode->JSObject()->SetAttributeValue(
              wsValue, pFormNode->GetFormatDataValue(wsValue), false, false);
        }
        break;
      case XFA_FFWidgetType::kCheckButton:
        wsValue = defValue ? defValue->GetChildValueContent() : WideString();
        if (wsValue.IsEmpty())
          break;

        pDataNode->JSObject()->SetAttributeValue(
            wsValue, pFormNode->GetFormatDataValue(wsValue), false, false);
        break;
      case XFA_FFWidgetType::kExclGroup: {
        CXFA_Node* pChecked = nullptr;
        CXFA_Node* pChild = pFormNode->GetFirstChild();
        for (; pChild; pChild = pChild->GetNextSibling()) {
          if (pChild->GetElementType() != XFA_Element::Field)
            continue;

          auto* pValue =
              pChild->GetChild<CXFA_Value>(0, XFA_Element::Value, false);
          if (!pValue)
            continue;

          wsValue = pValue->GetChildValueContent();
          if (wsValue.IsEmpty())
            continue;

          CXFA_Items* pItems =
              pChild->GetChild<CXFA_Items>(0, XFA_Element::Items, false);
          if (!pItems)
            continue;

          CXFA_Node* pText = pItems->GetFirstChild();
          if (!pText)
            continue;

          WideString wsContent = pText->JSObject()->GetContent(false);
          if (wsContent == wsValue) {
            pChecked = pChild;
            pDataNode->JSObject()->SetAttributeValue(wsValue, wsValue, false,
                                                     false);
            pFormNode->JSObject()->SetCData(XFA_Attribute::Value, wsContent,
                                            false, false);
            break;
          }
        }
        if (!pChecked)
          break;

        pChild = pFormNode->GetFirstChild();
        for (; pChild; pChild = pChild->GetNextSibling()) {
          if (pChild == pChecked)
            continue;
          if (pChild->GetElementType() != XFA_Element::Field)
            continue;

          CXFA_Value* pValue =
              pChild->JSObject()->GetOrCreateProperty<CXFA_Value>(
                  0, XFA_Element::Value);
          CXFA_Items* pItems =
              pChild->GetChild<CXFA_Items>(0, XFA_Element::Items, false);
          CXFA_Node* pText = pItems ? pItems->GetFirstChild() : nullptr;
          if (pText)
            pText = pText->GetNextSibling();

          WideString wsContent;
          if (pText)
            wsContent = pText->JSObject()->GetContent(false);

          FormValueNode_SetChildContent(pValue, wsContent, XFA_Element::Text);
        }
        break;
      }
      case XFA_FFWidgetType::kNumericEdit: {
        wsValue = defValue ? defValue->GetChildValueContent() : WideString();
        if (wsValue.IsEmpty())
          break;

        wsValue = pFormNode->NormalizeNumStr(wsValue);
        pDataNode->JSObject()->SetAttributeValue(
            wsValue, pFormNode->GetFormatDataValue(wsValue), false, false);
        CXFA_Value* pValue =
            pFormNode->JSObject()->GetOrCreateProperty<CXFA_Value>(
                0, XFA_Element::Value);
        FormValueNode_SetChildContent(pValue, wsValue, XFA_Element::Float);
        break;
      }
      default:
        wsValue = defValue ? defValue->GetChildValueContent() : WideString();
        if (wsValue.IsEmpty())
          break;

        pDataNode->JSObject()->SetAttributeValue(
            wsValue, pFormNode->GetFormatDataValue(wsValue), false, false);
        break;
    }
    return;
  }

  WideString wsXMLValue = pDataNode->JSObject()->GetContent(false);
  WideString wsNormalizeValue = pFormNode->GetNormalizeDataValue(wsXMLValue);

  pDataNode->JSObject()->SetAttributeValue(wsNormalizeValue, wsXMLValue, false,
                                           false);
  switch (pFormNode->GetFFWidgetType()) {
    case XFA_FFWidgetType::kImageEdit: {
      FormValueNode_SetChildContent(defValue, wsNormalizeValue,
                                    XFA_Element::Image);
      CXFA_Image* image = defValue ? defValue->GetImageIfExists() : nullptr;
      if (image) {
        CFX_XMLElement* pXMLDataElement =
            ToXMLElement(pDataNode->GetXMLMappingNode());
        WideString wsContentType =
            pXMLDataElement->GetAttribute(L"xfa:contentType");
        if (!wsContentType.IsEmpty()) {
          pDataNode->JSObject()->SetCData(XFA_Attribute::ContentType,
                                          wsContentType, false, false);
          image->SetContentType(wsContentType);
        }

        WideString wsHref = pXMLDataElement->GetAttribute(L"href");
        if (!wsHref.IsEmpty())
          image->SetHref(wsHref);
      }
      break;
    }
    case XFA_FFWidgetType::kChoiceList:
      if (pFormNode->IsChoiceListMultiSelect()) {
        std::vector<CXFA_Node*> items = pDataNode->GetNodeList(
            XFA_NODEFILTER_Children | XFA_NODEFILTER_Properties,
            XFA_Element::Unknown);
        if (!items.empty()) {
          bool single = items.size() == 1;
          wsNormalizeValue.clear();

          for (CXFA_Node* pNode : items) {
            WideString wsItem = pNode->JSObject()->GetContent(false);
            if (single)
              wsItem += L"\n";

            wsNormalizeValue += wsItem;
          }
          CXFA_ExData* exData =
              defValue ? defValue->GetExDataIfExists() : nullptr;
          ASSERT(exData);

          exData->SetContentType(single ? L"text/plain" : L"text/xml");
        }
        FormValueNode_SetChildContent(defValue, wsNormalizeValue,
                                      XFA_Element::ExData);
      } else {
        FormValueNode_SetChildContent(defValue, wsNormalizeValue,
                                      XFA_Element::Text);
      }
      break;
    case XFA_FFWidgetType::kExclGroup: {
      pFormNode->SetSelectedMemberByValue(wsNormalizeValue.AsStringView(),
                                          false, false, false);
      break;
    }
    case XFA_FFWidgetType::kDateTimeEdit:
      FormValueNode_SetChildContent(defValue, wsNormalizeValue,
                                    XFA_Element::DateTime);
      break;
    case XFA_FFWidgetType::kNumericEdit: {
      WideString wsPicture =
          pFormNode->GetPictureContent(XFA_VALUEPICTURE_DataBind);
      if (wsPicture.IsEmpty())
        wsNormalizeValue = pFormNode->NormalizeNumStr(wsNormalizeValue);

      FormValueNode_SetChildContent(defValue, wsNormalizeValue,
                                    XFA_Element::Float);
      break;
    }
    default:
      FormValueNode_SetChildContent(defValue, wsNormalizeValue,
                                    XFA_Element::Text);
      break;
  }
}

CXFA_Node* MaybeCreateDataNode(CXFA_Document* pDocument,
                               CXFA_Node* pDataParent,
                               XFA_Element eNodeType,
                               const WideString& wsName) {
  if (!pDataParent)
    return nullptr;

  CXFA_Node* pParentDDNode = pDataParent->GetDataDescriptionNode();
  if (!pParentDDNode) {
    CXFA_Node* pDataNode =
        pDocument->CreateNode(XFA_PacketType::Datasets, eNodeType);
    pDataNode->JSObject()->SetCData(XFA_Attribute::Name, wsName, false, false);
    pDataNode->CreateXMLMappingNode();
    pDataParent->InsertChild(pDataNode, nullptr);
    pDataNode->SetFlag(XFA_NodeFlag_Initialized);
    return pDataNode;
  }

  CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_DDGroup> sIterator(
      pParentDDNode);
  for (CXFA_Node* pDDGroupNode = sIterator.GetCurrent(); pDDGroupNode;
       pDDGroupNode = sIterator.MoveToNext()) {
    if (pDDGroupNode != pParentDDNode) {
      if (pDDGroupNode->GetElementType() != XFA_Element::DataGroup)
        continue;

      Optional<WideString> ns = pDDGroupNode->JSObject()->TryNamespace();
      if (!ns.has_value() ||
          !ns.value().EqualsASCII("http://ns.adobe.com/data-description/")) {
        continue;
      }
    }

    CXFA_Node* pDDNode =
        pDDGroupNode->GetFirstChildByName(wsName.AsStringView());
    if (!pDDNode)
      continue;
    if (pDDNode->GetElementType() != eNodeType)
      break;

    CXFA_Node* pDataNode =
        pDocument->CreateNode(XFA_PacketType::Datasets, eNodeType);
    pDataNode->JSObject()->SetCData(XFA_Attribute::Name, wsName, false, false);
    pDataNode->CreateXMLMappingNode();
    if (eNodeType == XFA_Element::DataValue &&
        pDDNode->JSObject()->GetEnum(XFA_Attribute::Contains) ==
            XFA_AttributeValue::MetaData) {
      pDataNode->JSObject()->SetEnum(XFA_Attribute::Contains,
                                     XFA_AttributeValue::MetaData, false);
    }
    pDataParent->InsertChild(pDataNode, nullptr);
    pDataNode->SetDataDescriptionNode(pDDNode);
    pDataNode->SetFlag(XFA_NodeFlag_Initialized);
    return pDataNode;
  }
  return nullptr;
}

CXFA_Node* CopyContainer_Field(CXFA_Document* pDocument,
                               CXFA_Node* pTemplateNode,
                               CXFA_Node* pFormNode,
                               CXFA_Node* pDataScope,
                               bool bDataMerge,
                               bool bUpLevel) {
  CXFA_Node* pFieldNode = XFA_NodeMerge_CloneOrMergeContainer(
      pDocument, pFormNode, pTemplateNode, false, nullptr);
  ASSERT(pFieldNode);
  for (CXFA_Node* pTemplateChildNode = pTemplateNode->GetFirstChild();
       pTemplateChildNode;
       pTemplateChildNode = pTemplateChildNode->GetNextSibling()) {
    if (XFA_DataMerge_NeedGenerateForm(pTemplateChildNode, true)) {
      XFA_NodeMerge_CloneOrMergeContainer(pDocument, pFieldNode,
                                          pTemplateChildNode, true, nullptr);
    } else if (pTemplateNode->GetElementType() == XFA_Element::ExclGroup &&
               pTemplateChildNode->IsContainerNode()) {
      if (pTemplateChildNode->GetElementType() == XFA_Element::Field) {
        CopyContainer_Field(pDocument, pTemplateChildNode, pFieldNode, nullptr,
                            false, true);
      }
    }
  }
  if (bDataMerge) {
    bool bAccessedDataDOM = false;
    bool bSelfMatch = false;
    XFA_AttributeValue eBindMatch;
    CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFAContainerNode>
        sNodeIter(pTemplateNode);
    CXFA_Node* pDataNode = FindMatchingDataNode(
        pDocument, pTemplateNode, pDataScope, bAccessedDataDOM, true,
        &sNodeIter, bSelfMatch, eBindMatch, bUpLevel);
    if (pDataNode)
      CreateDataBinding(pFieldNode, pDataNode, true);
  } else {
    FormValueNode_MatchNoneCreateChild(pFieldNode);
  }
  return pFieldNode;
}

CXFA_Node* CopyContainer_SubformSet(CXFA_Document* pDocument,
                                    CXFA_Node* pTemplateNode,
                                    CXFA_Node* pFormParentNode,
                                    CXFA_Node* pDataScope,
                                    bool bOneInstance,
                                    bool bDataMerge) {
  XFA_Element eType = pTemplateNode->GetElementType();
  CXFA_Node* pOccurNode = nullptr;
  CXFA_Node* pFirstInstance = nullptr;
  bool bUseInstanceManager =
      pFormParentNode->GetElementType() != XFA_Element::Area;
  CXFA_Node* pInstMgrNode = nullptr;
  std::vector<CXFA_Node*> subformArray;
  std::vector<CXFA_Node*>* pSearchArray = nullptr;
  if (!bOneInstance &&
      (eType == XFA_Element::SubformSet || eType == XFA_Element::Subform)) {
    pInstMgrNode = bUseInstanceManager ? CloneOrMergeInstanceManager(
                                             pDocument, pFormParentNode,
                                             pTemplateNode, &subformArray)
                                       : nullptr;
    if (CXFA_Occur* pOccurTemplateNode =
            pTemplateNode->GetFirstChildByClass<CXFA_Occur>(
                XFA_Element::Occur)) {
      pOccurNode = pInstMgrNode ? XFA_NodeMerge_CloneOrMergeContainer(
                                      pDocument, pInstMgrNode,
                                      pOccurTemplateNode, false, nullptr)
                                : pOccurTemplateNode;
    } else if (pInstMgrNode) {
      pOccurNode =
          pInstMgrNode->GetFirstChildByClass<CXFA_Occur>(XFA_Element::Occur);
      if (pOccurNode)
        pOccurNode->ClearFlag(XFA_NodeFlag_UnusedNode);
    }
    if (pInstMgrNode) {
      pInstMgrNode->SetFlagAndNotify(XFA_NodeFlag_Initialized);
      pSearchArray = &subformArray;
      if (pFormParentNode->GetElementType() == XFA_Element::PageArea) {
        bOneInstance = true;
        if (subformArray.empty())
          pSearchArray = nullptr;
      } else if (pTemplateNode->GetNameHash() == 0 && subformArray.empty()) {
        pSearchArray = nullptr;
      }
    }
  }

  int32_t iMax = 1;
  int32_t iInit = 1;
  int32_t iMin = 1;
  if (!bOneInstance && pOccurNode) {
    std::tie(iMin, iMax, iInit) =
        static_cast<CXFA_Occur*>(pOccurNode)->GetOccurInfo();
  }

  XFA_AttributeValue eRelation =
      eType == XFA_Element::SubformSet
          ? pTemplateNode->JSObject()->GetEnum(XFA_Attribute::Relation)
          : XFA_AttributeValue::Ordered;
  int32_t iCurRepeatIndex = 0;
  XFA_AttributeValue eParentBindMatch = XFA_AttributeValue::None;
  if (bDataMerge) {
    CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFAContainerNode>
        sNodeIterator(pTemplateNode);
    bool bAccessedDataDOM = false;
    if (eType == XFA_Element::SubformSet || eType == XFA_Element::Area) {
      sNodeIterator.MoveToNext();
    } else {
      std::map<CXFA_Node*, CXFA_Node*> subformMapArray;
      std::vector<CXFA_Node*> nodeArray;
      for (; iMax < 0 || iCurRepeatIndex < iMax; iCurRepeatIndex++) {
        bool bSelfMatch = false;
        XFA_AttributeValue eBindMatch = XFA_AttributeValue::None;
        CXFA_Node* pDataNode = FindMatchingDataNode(
            pDocument, pTemplateNode, pDataScope, bAccessedDataDOM, false,
            &sNodeIterator, bSelfMatch, eBindMatch, true);
        if (!pDataNode || sNodeIterator.GetCurrent() != pTemplateNode)
          break;

        eParentBindMatch = eBindMatch;
        CXFA_Node* pSubformNode = XFA_NodeMerge_CloneOrMergeContainer(
            pDocument, pFormParentNode, pTemplateNode, false, pSearchArray);
        if (!pFirstInstance)
          pFirstInstance = pSubformNode;

        CreateDataBinding(pSubformNode, pDataNode, true);
        ASSERT(pSubformNode);
        subformMapArray[pSubformNode] = pDataNode;
        nodeArray.push_back(pSubformNode);
      }

      for (CXFA_Node* pSubform : nodeArray) {
        CXFA_Node* pDataNode = nullptr;
        auto it = subformMapArray.find(pSubform);
        if (it != subformMapArray.end())
          pDataNode = it->second;
        for (CXFA_Node* pTemplateChild = pTemplateNode->GetFirstChild();
             pTemplateChild;
             pTemplateChild = pTemplateChild->GetNextSibling()) {
          if (XFA_DataMerge_NeedGenerateForm(pTemplateChild,
                                             bUseInstanceManager)) {
            XFA_NodeMerge_CloneOrMergeContainer(pDocument, pSubform,
                                                pTemplateChild, true, nullptr);
          } else if (pTemplateChild->IsContainerNode()) {
            pDocument->DataMerge_CopyContainer(pTemplateChild, pSubform,
                                               pDataNode, false, true, false);
          }
        }
      }
      subformMapArray.clear();
    }

    for (; iMax < 0 || iCurRepeatIndex < iMax; iCurRepeatIndex++) {
      bool bSelfMatch = false;
      XFA_AttributeValue eBindMatch = XFA_AttributeValue::None;
      if (!FindMatchingDataNode(pDocument, pTemplateNode, pDataScope,
                                bAccessedDataDOM, false, &sNodeIterator,
                                bSelfMatch, eBindMatch, true)) {
        break;
      }
      if (eBindMatch == XFA_AttributeValue::DataRef &&
          eParentBindMatch == XFA_AttributeValue::DataRef) {
        break;
      }

      if (eRelation == XFA_AttributeValue::Choice ||
          eRelation == XFA_AttributeValue::Unordered) {
        CXFA_Node* pSubformSetNode = XFA_NodeMerge_CloneOrMergeContainer(
            pDocument, pFormParentNode, pTemplateNode, false, pSearchArray);
        ASSERT(pSubformSetNode);
        if (!pFirstInstance)
          pFirstInstance = pSubformSetNode;

        std::vector<RecurseRecord> rgItemMatchList;
        std::vector<CXFA_Node*> rgItemUnmatchList;
        for (CXFA_Node* pTemplateChild = pTemplateNode->GetFirstChild();
             pTemplateChild;
             pTemplateChild = pTemplateChild->GetNextSibling()) {
          if (XFA_DataMerge_NeedGenerateForm(pTemplateChild,
                                             bUseInstanceManager)) {
            XFA_NodeMerge_CloneOrMergeContainer(pDocument, pSubformSetNode,
                                                pTemplateChild, true, nullptr);
          } else if (pTemplateChild->IsContainerNode()) {
            bSelfMatch = false;
            eBindMatch = XFA_AttributeValue::None;
            if (eRelation != XFA_AttributeValue::Ordered) {
              CXFA_NodeIteratorTemplate<CXFA_Node,
                                        CXFA_TraverseStrategy_XFAContainerNode>
                  sChildIter(pTemplateChild);
              CXFA_Node* pDataMatch = FindMatchingDataNode(
                  pDocument, pTemplateChild, pDataScope, bAccessedDataDOM,
                  false, &sChildIter, bSelfMatch, eBindMatch, true);
              if (pDataMatch) {
                RecurseRecord sNewRecord = {pTemplateChild, pDataMatch};
                if (bSelfMatch)
                  rgItemMatchList.insert(rgItemMatchList.begin(), sNewRecord);
                else
                  rgItemMatchList.push_back(sNewRecord);
              } else {
                rgItemUnmatchList.push_back(pTemplateChild);
              }
            } else {
              rgItemUnmatchList.push_back(pTemplateChild);
            }
          }
        }

        switch (eRelation) {
          case XFA_AttributeValue::Choice: {
            ASSERT(!rgItemMatchList.empty());
            SortRecurseRecord(&rgItemMatchList, pDataScope, true);
            pDocument->DataMerge_CopyContainer(
                rgItemMatchList.front().pTemplateChild, pSubformSetNode,
                pDataScope, false, true, true);
            break;
          }
          case XFA_AttributeValue::Unordered: {
            if (!rgItemMatchList.empty()) {
              SortRecurseRecord(&rgItemMatchList, pDataScope, false);
              for (const auto& matched : rgItemMatchList) {
                pDocument->DataMerge_CopyContainer(matched.pTemplateChild,
                                                   pSubformSetNode, pDataScope,
                                                   false, true, true);
              }
            }
            for (auto* unmatched : rgItemUnmatchList) {
              pDocument->DataMerge_CopyContainer(unmatched, pSubformSetNode,
                                                 pDataScope, false, true, true);
            }
            break;
          }
          default:
            break;
        }
      } else {
        CXFA_Node* pSubformSetNode = XFA_NodeMerge_CloneOrMergeContainer(
            pDocument, pFormParentNode, pTemplateNode, false, pSearchArray);
        ASSERT(pSubformSetNode);
        if (!pFirstInstance)
          pFirstInstance = pSubformSetNode;

        for (CXFA_Node* pTemplateChild = pTemplateNode->GetFirstChild();
             pTemplateChild;
             pTemplateChild = pTemplateChild->GetNextSibling()) {
          if (XFA_DataMerge_NeedGenerateForm(pTemplateChild,
                                             bUseInstanceManager)) {
            XFA_NodeMerge_CloneOrMergeContainer(pDocument, pSubformSetNode,
                                                pTemplateChild, true, nullptr);
          } else if (pTemplateChild->IsContainerNode()) {
            pDocument->DataMerge_CopyContainer(pTemplateChild, pSubformSetNode,
                                               pDataScope, false, true, true);
          }
        }
      }
    }

    if (iCurRepeatIndex == 0 && bAccessedDataDOM == false) {
      int32_t iLimit = iMax;
      if (pInstMgrNode && pTemplateNode->GetNameHash() == 0) {
        iLimit = pdfium::CollectionSize<int32_t>(subformArray);
        if (iLimit < iMin)
          iLimit = iInit;
      }

      for (; (iLimit < 0 || iCurRepeatIndex < iLimit); iCurRepeatIndex++) {
        if (pInstMgrNode) {
          if (pSearchArray && pSearchArray->empty()) {
            if (pTemplateNode->GetNameHash() != 0)
              break;
            pSearchArray = nullptr;
          }
        } else if (!XFA_DataMerge_FindFormDOMInstance(
                       pDocument, pTemplateNode->GetElementType(),
                       pTemplateNode->GetNameHash(), pFormParentNode)) {
          break;
        }
        CXFA_Node* pSubformNode = XFA_NodeMerge_CloneOrMergeContainer(
            pDocument, pFormParentNode, pTemplateNode, false, pSearchArray);
        ASSERT(pSubformNode);
        if (!pFirstInstance)
          pFirstInstance = pSubformNode;

        for (CXFA_Node* pTemplateChild = pTemplateNode->GetFirstChild();
             pTemplateChild;
             pTemplateChild = pTemplateChild->GetNextSibling()) {
          if (XFA_DataMerge_NeedGenerateForm(pTemplateChild,
                                             bUseInstanceManager)) {
            XFA_NodeMerge_CloneOrMergeContainer(pDocument, pSubformNode,
                                                pTemplateChild, true, nullptr);
          } else if (pTemplateChild->IsContainerNode()) {
            pDocument->DataMerge_CopyContainer(pTemplateChild, pSubformNode,
                                               pDataScope, false, true, true);
          }
        }
      }
    }
  }

  int32_t iMinimalLimit = iCurRepeatIndex == 0 ? iInit : iMin;
  for (; iCurRepeatIndex < iMinimalLimit; iCurRepeatIndex++) {
    CXFA_Node* pSubformSetNode = XFA_NodeMerge_CloneOrMergeContainer(
        pDocument, pFormParentNode, pTemplateNode, false, pSearchArray);
    ASSERT(pSubformSetNode);
    if (!pFirstInstance)
      pFirstInstance = pSubformSetNode;

    bool bFound = false;
    for (CXFA_Node* pTemplateChild = pTemplateNode->GetFirstChild();
         pTemplateChild; pTemplateChild = pTemplateChild->GetNextSibling()) {
      if (XFA_DataMerge_NeedGenerateForm(pTemplateChild, bUseInstanceManager)) {
        XFA_NodeMerge_CloneOrMergeContainer(pDocument, pSubformSetNode,
                                            pTemplateChild, true, nullptr);
      } else if (pTemplateChild->IsContainerNode()) {
        if (bFound && eRelation == XFA_AttributeValue::Choice)
          continue;

        pDocument->DataMerge_CopyContainer(pTemplateChild, pSubformSetNode,
                                           pDataScope, false, bDataMerge, true);
        bFound = true;
      }
    }
  }
  return pFirstInstance;
}

void UpdateBindingRelations(CXFA_Document* pDocument,
                            CXFA_Node* pFormNode,
                            CXFA_Node* pDataScope,
                            bool bDataRef,
                            bool bParentDataRef) {
  bool bMatchRef = true;
  XFA_Element eType = pFormNode->GetElementType();
  CXFA_Node* pDataNode = pFormNode->GetBindData();
  if (eType == XFA_Element::Subform || eType == XFA_Element::ExclGroup ||
      eType == XFA_Element::Field) {
    CXFA_Node* pTemplateNode = pFormNode->GetTemplateNodeIfExists();
    CXFA_Bind* pTemplateNodeBind =
        pTemplateNode
            ? pTemplateNode->GetFirstChildByClass<CXFA_Bind>(XFA_Element::Bind)
            : nullptr;
    XFA_AttributeValue eMatch =
        pTemplateNodeBind
            ? pTemplateNodeBind->JSObject()->GetEnum(XFA_Attribute::Match)
            : XFA_AttributeValue::Once;
    switch (eMatch) {
      case XFA_AttributeValue::None:
        if (!bDataRef || bParentDataRef)
          FormValueNode_MatchNoneCreateChild(pFormNode);
        break;
      case XFA_AttributeValue::Once:
        if (!bDataRef || bParentDataRef) {
          if (!pDataNode) {
            if (pFormNode->GetNameHash() != 0 &&
                pFormNode->JSObject()->GetEnum(XFA_Attribute::Scope) !=
                    XFA_AttributeValue::None) {
              XFA_Element eDataNodeType = (eType == XFA_Element::Subform ||
                                           XFA_FieldIsMultiListBox(pFormNode))
                                              ? XFA_Element::DataGroup
                                              : XFA_Element::DataValue;
              pDataNode = MaybeCreateDataNode(
                  pDocument, pDataScope, eDataNodeType,
                  WideString(
                      pFormNode->JSObject()->GetCData(XFA_Attribute::Name)));
              if (pDataNode)
                CreateDataBinding(pFormNode, pDataNode, false);
            }
            if (!pDataNode)
              FormValueNode_MatchNoneCreateChild(pFormNode);

          } else {
            CXFA_Node* pDataParent = pDataNode->GetParent();
            if (pDataParent != pDataScope) {
              ASSERT(pDataParent);
              pDataParent->RemoveChild(pDataNode, true);
              pDataScope->InsertChild(pDataNode, nullptr);
            }
          }
        }
        break;
      case XFA_AttributeValue::Global:
        if (!bDataRef || bParentDataRef) {
          uint32_t dwNameHash = pFormNode->GetNameHash();
          if (dwNameHash != 0 && !pDataNode) {
            pDataNode = pDocument->GetGlobalBinding(dwNameHash);
            if (!pDataNode) {
              XFA_Element eDataNodeType = (eType == XFA_Element::Subform ||
                                           XFA_FieldIsMultiListBox(pFormNode))
                                              ? XFA_Element::DataGroup
                                              : XFA_Element::DataValue;
              CXFA_Node* pRecordNode =
                  ToNode(pDocument->GetXFAObject(XFA_HASHCODE_Record));
              pDataNode = MaybeCreateDataNode(
                  pDocument, pRecordNode, eDataNodeType,
                  WideString(
                      pFormNode->JSObject()->GetCData(XFA_Attribute::Name)));
              if (pDataNode) {
                CreateDataBinding(pFormNode, pDataNode, false);
                pDocument->RegisterGlobalBinding(pFormNode->GetNameHash(),
                                                 pDataNode);
              }
            } else {
              CreateDataBinding(pFormNode, pDataNode, true);
            }
          }
          if (!pDataNode)
            FormValueNode_MatchNoneCreateChild(pFormNode);
        }
        break;
      case XFA_AttributeValue::DataRef: {
        bMatchRef = bDataRef;
        bParentDataRef = true;
        if (!pDataNode && bDataRef) {
          WideString wsRef =
              pTemplateNodeBind
                  ? pTemplateNodeBind->JSObject()->GetCData(XFA_Attribute::Ref)
                  : WideString();
          uint32_t dFlags =
              XFA_RESOLVENODE_Children | XFA_RESOLVENODE_CreateNode;
          XFA_RESOLVENODE_RS rs;
          pDocument->GetScriptContext()->ResolveObjects(
              pDataScope, wsRef.AsStringView(), &rs, dFlags, pTemplateNode);
          CXFA_Object* pObject =
              !rs.objects.empty() ? rs.objects.front().Get() : nullptr;
          pDataNode = ToNode(pObject);
          if (pDataNode) {
            CreateDataBinding(pFormNode, pDataNode,
                              rs.dwFlags == XFA_ResolveNode_RSType_ExistNodes);
          } else {
            FormValueNode_MatchNoneCreateChild(pFormNode);
          }
        }
        break;
      }
      default:
        break;
    }
  }

  if (bMatchRef &&
      (eType == XFA_Element::Subform || eType == XFA_Element::SubformSet ||
       eType == XFA_Element::Area || eType == XFA_Element::PageArea ||
       eType == XFA_Element::PageSet)) {
    for (CXFA_Node* pFormChild = pFormNode->GetFirstChild(); pFormChild;
         pFormChild = pFormChild->GetNextSibling()) {
      if (!pFormChild->IsContainerNode())
        continue;
      if (pFormChild->IsUnusedNode())
        continue;

      UpdateBindingRelations(pDocument, pFormChild,
                             pDataNode ? pDataNode : pDataScope, bDataRef,
                             bParentDataRef);
    }
  }
}

void UpdateDataRelation(CXFA_Node* pDataNode, CXFA_Node* pDataDescriptionNode) {
  ASSERT(pDataDescriptionNode);
  for (CXFA_Node* pDataChild = pDataNode->GetFirstChild(); pDataChild;
       pDataChild = pDataChild->GetNextSibling()) {
    uint32_t dwNameHash = pDataChild->GetNameHash();
    if (!dwNameHash)
      continue;

    CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_DDGroup>
        sIterator(pDataDescriptionNode);
    for (CXFA_Node* pDDGroupNode = sIterator.GetCurrent(); pDDGroupNode;
         pDDGroupNode = sIterator.MoveToNext()) {
      if (pDDGroupNode != pDataDescriptionNode) {
        if (pDDGroupNode->GetElementType() != XFA_Element::DataGroup)
          continue;

        Optional<WideString> ns = pDDGroupNode->JSObject()->TryNamespace();
        if (!ns.has_value() ||
            !ns.value().EqualsASCII("http://ns.adobe.com/data-description/")) {
          continue;
        }
      }

      CXFA_Node* pDDNode = pDDGroupNode->GetFirstChildByName(dwNameHash);
      if (!pDDNode)
        continue;
      if (pDDNode->GetElementType() != pDataChild->GetElementType())
        break;

      pDataChild->SetDataDescriptionNode(pDDNode);
      UpdateDataRelation(pDataChild, pDDNode);
      break;
    }
  }
}

}  // namespace

CXFA_Document::CXFA_Document(CXFA_FFNotify* notify)
    : CXFA_NodeOwner(),
      notify_(notify),
      m_pRootNode(nullptr),
      m_eCurVersionMode(XFA_VERSION_DEFAULT),
      m_dwDocFlags(0) {}

CXFA_Document::~CXFA_Document() = default;

CXFA_LayoutProcessor* CXFA_Document::GetLayoutProcessor() {
  if (!m_pLayoutProcessor)
    m_pLayoutProcessor = pdfium::MakeUnique<CXFA_LayoutProcessor>(this);
  return m_pLayoutProcessor.get();
}

void CXFA_Document::ClearLayoutData() {
  m_pLayoutProcessor.reset();
  m_pScriptContext.reset();
  m_pLocaleMgr.reset();
  m_pScriptDataWindow.reset();
  m_pScriptEvent.reset();
  m_pScriptHost.reset();
  m_pScriptLog.reset();
  m_pScriptLayout.reset();
  m_pScriptSignature.reset();
}

CXFA_Object* CXFA_Document::GetXFAObject(XFA_HashCode dwNodeNameHash) {
  switch (dwNodeNameHash) {
    case XFA_HASHCODE_Data: {
      CXFA_Node* pDatasetsNode = ToNode(GetXFAObject(XFA_HASHCODE_Datasets));
      if (!pDatasetsNode)
        return nullptr;

      for (CXFA_DataGroup* pDatasetsChild =
               pDatasetsNode->GetFirstChildByClass<CXFA_DataGroup>(
                   XFA_Element::DataGroup);
           pDatasetsChild;
           pDatasetsChild =
               pDatasetsChild->GetNextSameClassSibling<CXFA_DataGroup>(
                   XFA_Element::DataGroup)) {
        if (pDatasetsChild->GetNameHash() != XFA_HASHCODE_Data)
          continue;

        Optional<WideString> namespaceURI =
            pDatasetsChild->JSObject()->TryNamespace();
        if (!namespaceURI)
          continue;

        Optional<WideString> datasetsURI =
            pDatasetsNode->JSObject()->TryNamespace();
        if (!datasetsURI)
          continue;
        if (*namespaceURI == *datasetsURI)
          return pDatasetsChild;
      }
      return nullptr;
    }
    case XFA_HASHCODE_Record: {
      CXFA_Node* pData = ToNode(GetXFAObject(XFA_HASHCODE_Data));
      return pData ? pData->GetFirstChildByClass<CXFA_DataGroup>(
                         XFA_Element::DataGroup)
                   : nullptr;
    }
    case XFA_HASHCODE_DataWindow: {
      if (!m_pScriptDataWindow)
        m_pScriptDataWindow = pdfium::MakeUnique<CScript_DataWindow>(this);
      return m_pScriptDataWindow.get();
    }
    case XFA_HASHCODE_Event: {
      if (!m_pScriptEvent)
        m_pScriptEvent = pdfium::MakeUnique<CScript_EventPseudoModel>(this);
      return m_pScriptEvent.get();
    }
    case XFA_HASHCODE_Host: {
      if (!m_pScriptHost)
        m_pScriptHost = pdfium::MakeUnique<CScript_HostPseudoModel>(this);
      return m_pScriptHost.get();
    }
    case XFA_HASHCODE_Log: {
      if (!m_pScriptLog)
        m_pScriptLog = pdfium::MakeUnique<CScript_LogPseudoModel>(this);
      return m_pScriptLog.get();
    }
    case XFA_HASHCODE_Signature: {
      if (!m_pScriptSignature)
        m_pScriptSignature =
            pdfium::MakeUnique<CScript_SignaturePseudoModel>(this);
      return m_pScriptSignature.get();
    }
    case XFA_HASHCODE_Layout: {
      if (!m_pScriptLayout)
        m_pScriptLayout = pdfium::MakeUnique<CScript_LayoutPseudoModel>(this);
      return m_pScriptLayout.get();
    }
    default:
      return m_pRootNode->GetFirstChildByName(dwNodeNameHash);
  }
}

CXFA_Node* CXFA_Document::CreateNode(XFA_PacketType packet,
                                     XFA_Element eElement) {
  if (eElement == XFA_Element::Unknown)
    return nullptr;
  return AddOwnedNode(CXFA_Node::Create(this, eElement, packet));
}

void CXFA_Document::SetFlag(uint32_t dwFlag, bool bOn) {
  if (bOn)
    m_dwDocFlags |= dwFlag;
  else
    m_dwDocFlags &= ~dwFlag;
}

bool CXFA_Document::IsInteractive() {
  if (m_dwDocFlags & XFA_DOCFLAG_HasInteractive)
    return !!(m_dwDocFlags & XFA_DOCFLAG_Interactive);

  CXFA_Node* pConfig = ToNode(GetXFAObject(XFA_HASHCODE_Config));
  if (!pConfig)
    return false;

  CXFA_Present* pPresent =
      pConfig->GetFirstChildByClass<CXFA_Present>(XFA_Element::Present);
  if (!pPresent)
    return false;

  CXFA_Pdf* pPDF = pPresent->GetFirstChildByClass<CXFA_Pdf>(XFA_Element::Pdf);
  if (!pPDF)
    return false;

  CXFA_Interactive* pFormFiller =
      pPDF->GetChild<CXFA_Interactive>(0, XFA_Element::Interactive, false);
  if (pFormFiller) {
    m_dwDocFlags |= XFA_DOCFLAG_HasInteractive;

    WideString wsInteractive = pFormFiller->JSObject()->GetContent(false);
    if (wsInteractive.EqualsASCII("1")) {
      m_dwDocFlags |= XFA_DOCFLAG_Interactive;
      return true;
    }
  }
  return false;
}

CXFA_LocaleMgr* CXFA_Document::GetLocaleMgr() {
  if (!m_pLocaleMgr) {
    m_pLocaleMgr = pdfium::MakeUnique<CXFA_LocaleMgr>(
        ToNode(GetXFAObject(XFA_HASHCODE_LocaleSet)),
        GetNotify()->GetAppProvider()->GetLanguage());
  }
  return m_pLocaleMgr.get();
}

CFXJSE_Engine* CXFA_Document::InitScriptContext(CJS_Runtime* fxjs_runtime) {
  ASSERT(!m_pScriptContext);
  m_pScriptContext = pdfium::MakeUnique<CFXJSE_Engine>(this, fxjs_runtime);
  return m_pScriptContext.get();
}

// We have to call |InitScriptContext| before any calls to |GetScriptContext|
// or the context won't have an isolate set into it.
CFXJSE_Engine* CXFA_Document::GetScriptContext() const {
  ASSERT(m_pScriptContext);
  return m_pScriptContext.get();
}

XFA_VERSION CXFA_Document::RecognizeXFAVersionNumber(
    const WideString& wsTemplateNS) {
  WideStringView wsTemplateURIPrefix(kTemplateNS);
  if (wsTemplateNS.GetLength() <= wsTemplateURIPrefix.GetLength())
    return XFA_VERSION_UNKNOWN;

  size_t prefixLength = wsTemplateURIPrefix.GetLength();
  if (WideStringView(wsTemplateNS.c_str(), prefixLength) != wsTemplateURIPrefix)
    return XFA_VERSION_UNKNOWN;

  auto nDotPos = wsTemplateNS.Find('.', prefixLength);
  if (!nDotPos.has_value())
    return XFA_VERSION_UNKNOWN;

  int8_t iMajor = FXSYS_wtoi(
      wsTemplateNS.Mid(prefixLength, nDotPos.value() - prefixLength).c_str());
  int8_t iMinor =
      FXSYS_wtoi(wsTemplateNS
                     .Mid(nDotPos.value() + 1,
                          wsTemplateNS.GetLength() - nDotPos.value() - 2)
                     .c_str());
  XFA_VERSION eVersion = (XFA_VERSION)((int32_t)iMajor * 100 + iMinor);
  if (eVersion < XFA_VERSION_MIN || eVersion > XFA_VERSION_MAX)
    return XFA_VERSION_UNKNOWN;

  m_eCurVersionMode = eVersion;
  return eVersion;
}

FormType CXFA_Document::GetFormType() const {
  return GetNotify()->GetHDOC()->GetFormType();
}

CXFA_Node* CXFA_Document::GetNodeByID(CXFA_Node* pRoot,
                                      WideStringView wsID) const {
  if (!pRoot || wsID.IsEmpty())
    return nullptr;

  CXFA_NodeIterator sIterator(pRoot);
  for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
       pNode = sIterator.MoveToNext()) {
    WideString wsIDVal = pNode->JSObject()->GetCData(XFA_Attribute::Id);
    if (!wsIDVal.IsEmpty() && wsIDVal == wsID)
      return pNode;
  }
  return nullptr;
}

void CXFA_Document::DoProtoMerge() {
  CXFA_Node* pTemplateRoot = ToNode(GetXFAObject(XFA_HASHCODE_Template));
  if (!pTemplateRoot)
    return;

  std::map<uint32_t, CXFA_Node*> mIDMap;
  std::set<CXFA_Node*> sUseNodes;
  CXFA_NodeIterator sIterator(pTemplateRoot);
  for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
       pNode = sIterator.MoveToNext()) {
    WideString wsIDVal = pNode->JSObject()->GetCData(XFA_Attribute::Id);
    if (!wsIDVal.IsEmpty())
      mIDMap[FX_HashCode_GetW(wsIDVal.AsStringView(), false)] = pNode;

    WideString wsUseVal = pNode->JSObject()->GetCData(XFA_Attribute::Use);
    if (!wsUseVal.IsEmpty()) {
      sUseNodes.insert(pNode);
    } else {
      wsUseVal = pNode->JSObject()->GetCData(XFA_Attribute::Usehref);
      if (!wsUseVal.IsEmpty())
        sUseNodes.insert(pNode);
    }
  }

  for (CXFA_Node* pUseHrefNode : sUseNodes) {
    // Must outlive the WideStringViews below.
    WideString wsUseVal =
        pUseHrefNode->JSObject()->GetCData(XFA_Attribute::Usehref);
    WideStringView wsURI;
    WideStringView wsID;
    WideStringView wsSOM;

    if (!wsUseVal.IsEmpty()) {
      auto uSharpPos = wsUseVal.Find('#');
      if (!uSharpPos.has_value()) {
        wsURI = wsUseVal.AsStringView();
      } else {
        wsURI = WideStringView(wsUseVal.c_str(), uSharpPos.value());
        size_t uLen = wsUseVal.GetLength();
        if (uLen >= uSharpPos.value() + 5 &&
            WideStringView(wsUseVal.c_str() + uSharpPos.value(), 5) ==
                L"#som(" &&
            wsUseVal[uLen - 1] == ')') {
          wsSOM = WideStringView(wsUseVal.c_str() + uSharpPos.value() + 5,
                                 uLen - 1 - uSharpPos.value() - 5);
        } else {
          wsID = WideStringView(wsUseVal.c_str() + uSharpPos.value() + 1,
                                uLen - uSharpPos.value() - 1);
        }
      }
    } else {
      wsUseVal = pUseHrefNode->JSObject()->GetCData(XFA_Attribute::Use);
      if (!wsUseVal.IsEmpty()) {
        if (wsUseVal[0] == '#')
          wsID = WideStringView(wsUseVal.c_str() + 1, wsUseVal.GetLength() - 1);
        else
          wsSOM = WideStringView(wsUseVal.c_str(), wsUseVal.GetLength());
      }
    }
    if (!wsURI.IsEmpty() && !wsURI.EqualsASCII("."))
      continue;

    CXFA_Node* pProtoNode = nullptr;
    if (!wsSOM.IsEmpty()) {
      uint32_t dwFlag = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Attributes |
                        XFA_RESOLVENODE_Properties | XFA_RESOLVENODE_Parent |
                        XFA_RESOLVENODE_Siblings;
      XFA_RESOLVENODE_RS resolveNodeRS;
      if (m_pScriptContext->ResolveObjects(pUseHrefNode, wsSOM, &resolveNodeRS,
                                           dwFlag, nullptr)) {
        auto* pFirstObject = resolveNodeRS.objects.front().Get();
        if (pFirstObject && pFirstObject->IsNode())
          pProtoNode = pFirstObject->AsNode();
      }
    } else if (!wsID.IsEmpty()) {
      auto it = mIDMap.find(FX_HashCode_GetW(wsID, false));
      if (it == mIDMap.end())
        continue;
      pProtoNode = it->second;
    }
    if (!pProtoNode)
      continue;

    MergeNode(pUseHrefNode, pProtoNode);
  }
}

CXFA_Node* CXFA_Document::DataMerge_CopyContainer(CXFA_Node* pTemplateNode,
                                                  CXFA_Node* pFormNode,
                                                  CXFA_Node* pDataScope,
                                                  bool bOneInstance,
                                                  bool bDataMerge,
                                                  bool bUpLevel) {
  switch (pTemplateNode->GetElementType()) {
    case XFA_Element::SubformSet:
    case XFA_Element::Subform:
    case XFA_Element::Area:
    case XFA_Element::PageArea:
      return CopyContainer_SubformSet(this, pTemplateNode, pFormNode,
                                      pDataScope, bOneInstance, bDataMerge);
    case XFA_Element::ExclGroup:
    case XFA_Element::Field:
    case XFA_Element::Draw:
    case XFA_Element::ContentArea:
      return CopyContainer_Field(this, pTemplateNode, pFormNode, pDataScope,
                                 bDataMerge, bUpLevel);
    case XFA_Element::PageSet:
    case XFA_Element::Variables:
      break;
    default:
      NOTREACHED();
      break;
  }
  return nullptr;
}

void CXFA_Document::DataMerge_UpdateBindingRelations(
    CXFA_Node* pFormUpdateRoot) {
  CXFA_Node* pDataScope =
      XFA_DataMerge_FindDataScope(pFormUpdateRoot->GetParent());
  if (!pDataScope)
    return;

  UpdateBindingRelations(this, pFormUpdateRoot, pDataScope, false, false);
  UpdateBindingRelations(this, pFormUpdateRoot, pDataScope, true, false);
}

CXFA_Node* CXFA_Document::GetNotBindNode(
    const std::vector<UnownedPtr<CXFA_Object>>& arrayObjects) const {
  for (auto& pObject : arrayObjects) {
    CXFA_Node* pNode = pObject->AsNode();
    if (pNode && !pNode->HasBindItem())
      return pNode;
  }
  return nullptr;
}

void CXFA_Document::DoDataMerge() {
  CXFA_Node* pDatasetsRoot = ToNode(GetXFAObject(XFA_HASHCODE_Datasets));
  if (!pDatasetsRoot) {
    // Ownership will be passed in the AppendChild below to the XML tree.
    auto* pDatasetsXMLNode =
        notify_->GetHDOC()->GetXMLDocument()->CreateNode<CFX_XMLElement>(
            L"xfa:datasets");
    pDatasetsXMLNode->SetAttribute(L"xmlns:xfa",
                                   L"http://www.xfa.org/schema/xfa-data/1.0/");
    pDatasetsRoot =
        CreateNode(XFA_PacketType::Datasets, XFA_Element::DataModel);
    pDatasetsRoot->JSObject()->SetCData(XFA_Attribute::Name, L"datasets", false,
                                        false);

    m_pRootNode->GetXMLMappingNode()->AppendChild(pDatasetsXMLNode);
    m_pRootNode->InsertChild(pDatasetsRoot, nullptr);

    pDatasetsRoot->SetXMLMappingNode(pDatasetsXMLNode);
  }

  CXFA_Node* pDataRoot = nullptr;
  CXFA_Node* pDDRoot = nullptr;
  WideString wsDatasetsURI =
      pDatasetsRoot->JSObject()->TryNamespace().value_or(WideString());
  for (CXFA_Node* pChildNode = pDatasetsRoot->GetFirstChild(); pChildNode;
       pChildNode = pChildNode->GetNextSibling()) {
    if (pChildNode->GetElementType() != XFA_Element::DataGroup)
      continue;

    if (!pDDRoot && pChildNode->GetNameHash() == XFA_HASHCODE_DataDescription) {
      Optional<WideString> namespaceURI =
          pChildNode->JSObject()->TryNamespace();
      if (!namespaceURI.has_value())
        continue;
      if (namespaceURI.value().EqualsASCII(
              "http://ns.adobe.com/data-description/")) {
        pDDRoot = pChildNode;
      }
    } else if (!pDataRoot && pChildNode->GetNameHash() == XFA_HASHCODE_Data) {
      Optional<WideString> namespaceURI =
          pChildNode->JSObject()->TryNamespace();
      if (!namespaceURI)
        continue;
      if (*namespaceURI == wsDatasetsURI)
        pDataRoot = pChildNode;
    }
    if (pDataRoot && pDDRoot)
      break;
  }

  if (!pDataRoot) {
    pDataRoot = CreateNode(XFA_PacketType::Datasets, XFA_Element::DataGroup);
    pDataRoot->JSObject()->SetCData(XFA_Attribute::Name, L"data", false, false);

    auto* elem =
        notify_->GetHDOC()->GetXMLDocument()->CreateNode<CFX_XMLElement>(
            L"xfa:data");
    pDataRoot->SetXMLMappingNode(elem);
    pDatasetsRoot->InsertChild(pDataRoot, nullptr);
  }

  CXFA_DataGroup* pDataTopLevel =
      pDataRoot->GetFirstChildByClass<CXFA_DataGroup>(XFA_Element::DataGroup);
  uint32_t dwNameHash = pDataTopLevel ? pDataTopLevel->GetNameHash() : 0;
  CXFA_Template* pTemplateRoot =
      m_pRootNode->GetFirstChildByClass<CXFA_Template>(XFA_Element::Template);
  if (!pTemplateRoot)
    return;

  CXFA_Node* pTemplateChosen =
      dwNameHash != 0 ? pTemplateRoot->GetFirstChildByName(dwNameHash)
                      : nullptr;
  if (!pTemplateChosen ||
      pTemplateChosen->GetElementType() != XFA_Element::Subform) {
    pTemplateChosen =
        pTemplateRoot->GetFirstChildByClass<CXFA_Subform>(XFA_Element::Subform);
  }
  if (!pTemplateChosen)
    return;

  CXFA_Form* pFormRoot =
      m_pRootNode->GetFirstChildByClass<CXFA_Form>(XFA_Element::Form);
  bool bEmptyForm = false;
  if (!pFormRoot) {
    bEmptyForm = true;
    pFormRoot = static_cast<CXFA_Form*>(
        CreateNode(XFA_PacketType::Form, XFA_Element::Form));
    ASSERT(pFormRoot);
    pFormRoot->JSObject()->SetCData(XFA_Attribute::Name, L"form", false, false);
    m_pRootNode->InsertChild(pFormRoot, nullptr);
  } else {
    CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFANode>
        sIterator(pFormRoot);
    for (CXFA_Node* pNode = sIterator.MoveToNext(); pNode;
         pNode = sIterator.MoveToNext()) {
      pNode->SetFlag(XFA_NodeFlag_UnusedNode);
    }
  }

  CXFA_Node* pSubformSetNode = XFA_NodeMerge_CloneOrMergeContainer(
      this, pFormRoot, pTemplateChosen, false, nullptr);
  ASSERT(pSubformSetNode);
  if (!pDataTopLevel) {
    WideString wsFormName =
        pSubformSetNode->JSObject()->GetCData(XFA_Attribute::Name);
    WideString wsDataTopLevelName(wsFormName.IsEmpty() ? L"form" : wsFormName);

    pDataTopLevel = static_cast<CXFA_DataGroup*>(
        CreateNode(XFA_PacketType::Datasets, XFA_Element::DataGroup));
    pDataTopLevel->JSObject()->SetCData(XFA_Attribute::Name, wsDataTopLevelName,
                                        false, false);

    auto* elem =
        notify_->GetHDOC()->GetXMLDocument()->CreateNode<CFX_XMLElement>(
            wsDataTopLevelName);
    pDataTopLevel->SetXMLMappingNode(elem);

    CXFA_Node* pBeforeNode = pDataRoot->GetFirstChild();
    pDataRoot->InsertChild(pDataTopLevel, pBeforeNode);
  }

  ASSERT(pDataTopLevel);
  CreateDataBinding(pSubformSetNode, pDataTopLevel, true);
  for (CXFA_Node* pTemplateChild = pTemplateChosen->GetFirstChild();
       pTemplateChild; pTemplateChild = pTemplateChild->GetNextSibling()) {
    if (XFA_DataMerge_NeedGenerateForm(pTemplateChild, true)) {
      XFA_NodeMerge_CloneOrMergeContainer(this, pSubformSetNode, pTemplateChild,
                                          true, nullptr);
    } else if (pTemplateChild->IsContainerNode()) {
      DataMerge_CopyContainer(pTemplateChild, pSubformSetNode, pDataTopLevel,
                              false, true, true);
    }
  }
  if (pDDRoot)
    UpdateDataRelation(pDataRoot, pDDRoot);

  DataMerge_UpdateBindingRelations(pSubformSetNode);
  CXFA_PageSet* pPageSetNode =
      pSubformSetNode->GetFirstChildByClass<CXFA_PageSet>(XFA_Element::PageSet);
  while (pPageSetNode) {
    m_pPendingPageSet.push_back(pPageSetNode);
    CXFA_PageSet* pNextPageSetNode =
        pPageSetNode->GetNextSameClassSibling<CXFA_PageSet>(
            XFA_Element::PageSet);
    pSubformSetNode->RemoveChild(pPageSetNode, true);
    pPageSetNode = pNextPageSetNode;
  }

  if (bEmptyForm)
    return;

  CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFANode> sIterator(
      pFormRoot);
  CXFA_Node* pNode = sIterator.MoveToNext();
  while (pNode) {
    if (pNode->IsUnusedNode()) {
      if (pNode->IsContainerNode() ||
          pNode->GetElementType() == XFA_Element::InstanceManager) {
        CXFA_Node* pNext = sIterator.SkipChildrenAndMoveToNext();
        pNode->GetParent()->RemoveChild(pNode, true);
        pNode = pNext;
      } else {
        pNode->ClearFlag(XFA_NodeFlag_UnusedNode);
        pNode->SetFlagAndNotify(XFA_NodeFlag_Initialized);
        pNode = sIterator.MoveToNext();
      }
    } else {
      pNode->SetFlagAndNotify(XFA_NodeFlag_Initialized);
      pNode = sIterator.MoveToNext();
    }
  }
}

void CXFA_Document::DoDataRemerge(bool bDoDataMerge) {
  CXFA_Node* pFormRoot = ToNode(GetXFAObject(XFA_HASHCODE_Form));
  if (pFormRoot) {
    while (CXFA_Node* pNode = pFormRoot->GetFirstChild())
      pFormRoot->RemoveChild(pNode, true);

    pFormRoot->SetBindingNode(nullptr);
  }
  m_rgGlobalBinding.clear();

  if (bDoDataMerge)
    DoDataMerge();

  CXFA_LayoutProcessor* pLayoutProcessor = GetLayoutProcessor();
  pLayoutProcessor->SetForceReLayout(true);
}

CXFA_Node* CXFA_Document::GetGlobalBinding(uint32_t dwNameHash) {
  auto it = m_rgGlobalBinding.find(dwNameHash);
  return it != m_rgGlobalBinding.end() ? it->second : nullptr;
}

void CXFA_Document::RegisterGlobalBinding(uint32_t dwNameHash,
                                          CXFA_Node* pDataNode) {
  m_rgGlobalBinding[dwNameHash] = pDataNode;
}
