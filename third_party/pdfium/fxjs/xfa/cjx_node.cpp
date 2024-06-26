// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_node.h"

#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/cfx_memorystream.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "core/fxcrt/xml/cfx_xmlnode.h"
#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_document_parser.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/xfa_basic_data.h"
#include "xfa/fxfa/parser/xfa_utils.h"

namespace {

enum class EventAppliesToo : uint8_t {
  kNone = 0,
  kAll = 1,
  kAllNonRecursive = 2,
  kSubform = 3,
  kFieldOrExclusion = 4,
  kField = 5,
  kSignature = 6,
  kChoiceList = 7
};

struct XFA_ExecEventParaInfo {
 public:
  uint32_t m_uHash;  // hashed as wide string.
  XFA_EVENTTYPE m_eventType;
  EventAppliesToo m_validFlags;
};

#undef PARA
#define PARA(a, b, c, d) a, c, d
const XFA_ExecEventParaInfo gs_eventParaInfos[] = {
    {PARA(0x109d7ce7,
          "mouseEnter",
          XFA_EVENT_MouseEnter,
          EventAppliesToo::kField)},
    {PARA(0x1bfc72d9,
          "preOpen",
          XFA_EVENT_PreOpen,
          EventAppliesToo::kChoiceList)},
    {PARA(0x2196a452,
          "initialize",
          XFA_EVENT_Initialize,
          EventAppliesToo::kAll)},
    {PARA(0x27410f03,
          "mouseExit",
          XFA_EVENT_MouseExit,
          EventAppliesToo::kField)},
    {PARA(0x36f1c6d8,
          "preSign",
          XFA_EVENT_PreSign,
          EventAppliesToo::kSignature)},
    {PARA(0x4731d6ba,
          "exit",
          XFA_EVENT_Exit,
          EventAppliesToo::kAllNonRecursive)},
    {PARA(0x7233018a, "validate", XFA_EVENT_Validate, EventAppliesToo::kAll)},
    {PARA(0x8808385e,
          "indexChange",
          XFA_EVENT_IndexChange,
          EventAppliesToo::kSubform)},
    {PARA(0x891f4606,
          "change",
          XFA_EVENT_Change,
          EventAppliesToo::kFieldOrExclusion)},
    {PARA(0x9f693b21,
          "mouseDown",
          XFA_EVENT_MouseDown,
          EventAppliesToo::kField)},
    {PARA(0xcdce56b3,
          "full",
          XFA_EVENT_Full,
          EventAppliesToo::kFieldOrExclusion)},
    {PARA(0xd576d08e, "mouseUp", XFA_EVENT_MouseUp, EventAppliesToo::kField)},
    {PARA(0xd95657a6,
          "click",
          XFA_EVENT_Click,
          EventAppliesToo::kFieldOrExclusion)},
    {PARA(0xdbfbe02e, "calculate", XFA_EVENT_Calculate, EventAppliesToo::kAll)},
    {PARA(0xe25fa7b8,
          "postOpen",
          XFA_EVENT_PostOpen,
          EventAppliesToo::kChoiceList)},
    {PARA(0xe28dce7e,
          "enter",
          XFA_EVENT_Enter,
          EventAppliesToo::kAllNonRecursive)},
    {PARA(0xfd54fbb7,
          "postSign",
          XFA_EVENT_PostSign,
          EventAppliesToo::kSignature)},
};
#undef PARA

const XFA_ExecEventParaInfo* GetEventParaInfoByName(
    WideStringView wsEventName) {
  if (wsEventName.IsEmpty())
    return nullptr;

  uint32_t uHash = FX_HashCode_GetW(wsEventName, false);
  auto* result = std::lower_bound(
      std::begin(gs_eventParaInfos), std::end(gs_eventParaInfos), uHash,
      [](const XFA_ExecEventParaInfo& iter, const uint16_t& hash) {
        return iter.m_uHash < hash;
      });
  if (result != std::end(gs_eventParaInfos) && result->m_uHash == uHash)
    return result;
  return nullptr;
}

}  // namespace

const CJX_MethodSpec CJX_Node::MethodSpecs[] = {
    {"applyXSL", applyXSL_static},
    {"assignNode", assignNode_static},
    {"clone", clone_static},
    {"getAttribute", getAttribute_static},
    {"getElement", getElement_static},
    {"isPropertySpecified", isPropertySpecified_static},
    {"loadXML", loadXML_static},
    {"saveFilteredXML", saveFilteredXML_static},
    {"saveXML", saveXML_static},
    {"setAttribute", setAttribute_static},
    {"setElement", setElement_static}};

CJX_Node::CJX_Node(CXFA_Node* node) : CJX_Tree(node) {
  DefineMethods(MethodSpecs);
}

CJX_Node::~CJX_Node() = default;

bool CJX_Node::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CXFA_Node* CJX_Node::GetXFANode() const {
  return ToNode(GetXFAObject());
}

CJS_Result CJX_Node::applyXSL(CFX_V8* runtime,
                              const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  // TODO(weili): check whether we need to implement this, pdfium:501.
  return CJS_Result::Success();
}

CJS_Result CJX_Node::assignNode(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.empty() || params.size() > 3)
    return CJS_Result::Failure(JSMessage::kParamError);

  // TODO(weili): check whether we need to implement this, pdfium:501.
  return CJS_Result::Success();
}

CJS_Result CJX_Node::clone(CFX_V8* runtime,
                           const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  CXFA_Node* pCloneNode = GetXFANode()->Clone(runtime->ToBoolean(params[0]));
  CFXJSE_Value* value =
      GetDocument()->GetScriptContext()->GetJSValueFromMap(pCloneNode);
  if (!value)
    return CJS_Result::Success(runtime->NewNull());

  return CJS_Result::Success(
      value->DirectGetValue().Get(runtime->GetIsolate()));
}

CJS_Result CJX_Node::getAttribute(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  WideString expression = runtime->ToWideString(params[0]);
  return CJS_Result::Success(runtime->NewString(
      GetAttribute(expression.AsStringView()).ToUTF8().AsStringView()));
}

CJS_Result CJX_Node::getElement(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.empty() || params.size() > 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  WideString expression = runtime->ToWideString(params[0]);
  int32_t iValue = params.size() >= 2 ? runtime->ToInt32(params[1]) : 0;
  CXFA_Node* pNode = GetOrCreateProperty<CXFA_Node>(
      iValue, XFA_GetElementByName(expression.AsStringView()));
  if (!pNode)
    return CJS_Result::Success(runtime->NewNull());

  CFXJSE_Value* value =
      GetDocument()->GetScriptContext()->GetJSValueFromMap(pNode);
  if (!value)
    return CJS_Result::Success(runtime->NewNull());

  return CJS_Result::Success(
      value->DirectGetValue().Get(runtime->GetIsolate()));
}

CJS_Result CJX_Node::isPropertySpecified(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.empty() || params.size() > 3)
    return CJS_Result::Failure(JSMessage::kParamError);

  WideString expression = runtime->ToWideString(params[0]);
  Optional<XFA_ATTRIBUTEINFO> attr =
      XFA_GetAttributeByName(expression.AsStringView());
  if (attr.has_value() && HasAttribute(attr.value().attribute))
    return CJS_Result::Success(runtime->NewBoolean(true));

  bool bParent = params.size() < 2 || runtime->ToBoolean(params[1]);
  int32_t iIndex = params.size() == 3 ? runtime->ToInt32(params[2]) : 0;
  XFA_Element eType = XFA_GetElementByName(expression.AsStringView());
  bool bHas = !!GetOrCreateProperty<CXFA_Node>(iIndex, eType);
  if (!bHas && bParent && GetXFANode()->GetParent()) {
    // Also check on the parent.
    auto* jsnode = GetXFANode()->GetParent()->JSObject();
    bHas = jsnode->HasAttribute(attr.value().attribute) ||
           !!jsnode->GetOrCreateProperty<CXFA_Node>(iIndex, eType);
  }
  return CJS_Result::Success(runtime->NewBoolean(bHas));
}

CJS_Result CJX_Node::loadXML(CFX_V8* runtime,
                             const std::vector<v8::Local<v8::Value>>& params) {
  if (params.empty() || params.size() > 3)
    return CJS_Result::Failure(JSMessage::kParamError);

  ByteString expression = runtime->ToByteString(params[0]);
  if (expression.IsEmpty())
    return CJS_Result::Success();

  bool bIgnoreRoot = true;
  if (params.size() >= 2)
    bIgnoreRoot = runtime->ToBoolean(params[1]);

  bool bOverwrite = 0;
  if (params.size() >= 3)
    bOverwrite = runtime->ToBoolean(params[2]);

  auto pParser = pdfium::MakeUnique<CXFA_DocumentParser>(GetDocument());
  CFX_XMLNode* pXMLNode = pParser->ParseXMLData(expression);
  if (!pXMLNode)
    return CJS_Result::Success();

  CFX_XMLDocument* top_xml_doc =
      GetXFANode()->GetDocument()->GetNotify()->GetHDOC()->GetXMLDocument();
  top_xml_doc->AppendNodesFrom(pParser->GetXMLDoc().get());

  if (bIgnoreRoot &&
      (pXMLNode->GetType() != FX_XMLNODE_Element ||
       XFA_RecognizeRichText(static_cast<CFX_XMLElement*>(pXMLNode)))) {
    bIgnoreRoot = false;
  }

  CXFA_Node* pFakeRoot = GetXFANode()->Clone(false);
  WideString wsContentType = GetCData(XFA_Attribute::ContentType);
  if (!wsContentType.IsEmpty()) {
    pFakeRoot->JSObject()->SetCData(XFA_Attribute::ContentType,
                                    WideString(wsContentType), false, false);
  }

  CFX_XMLNode* pFakeXMLRoot = pFakeRoot->GetXMLMappingNode();
  if (!pFakeXMLRoot) {
    CFX_XMLNode* pThisXMLRoot = GetXFANode()->GetXMLMappingNode();
    CFX_XMLNode* clone;
    if (pThisXMLRoot) {
      clone = pThisXMLRoot->Clone(top_xml_doc);
    } else {
      clone = top_xml_doc->CreateNode<CFX_XMLElement>(
          WideString::FromASCII(GetXFANode()->GetClassName()));
    }
    pFakeXMLRoot = clone;
  }

  if (bIgnoreRoot) {
    CFX_XMLNode* pXMLChild = pXMLNode->GetFirstChild();
    while (pXMLChild) {
      CFX_XMLNode* pXMLSibling = pXMLChild->GetNextSibling();
      pXMLNode->RemoveChildNode(pXMLChild);
      pFakeXMLRoot->AppendChild(pXMLChild);
      pXMLChild = pXMLSibling;
    }
  } else {
    CFX_XMLNode* pXMLParent = pXMLNode->GetParent();
    if (pXMLParent)
      pXMLParent->RemoveChildNode(pXMLNode);

    pFakeXMLRoot->AppendChild(pXMLNode);
  }

  pParser->ConstructXFANode(pFakeRoot, pFakeXMLRoot);
  pFakeRoot = pParser->GetRootNode();
  if (!pFakeRoot)
    return CJS_Result::Success();

  if (bOverwrite) {
    CXFA_Node* pChild = GetXFANode()->GetFirstChild();
    CXFA_Node* pNewChild = pFakeRoot->GetFirstChild();
    int32_t index = 0;
    while (pNewChild) {
      CXFA_Node* pItem = pNewChild->GetNextSibling();
      pFakeRoot->RemoveChild(pNewChild, true);
      GetXFANode()->InsertChild(index++, pNewChild);
      pNewChild->SetFlagAndNotify(XFA_NodeFlag_Initialized);
      pNewChild = pItem;
    }

    while (pChild) {
      CXFA_Node* pItem = pChild->GetNextSibling();
      GetXFANode()->RemoveChild(pChild, true);
      pFakeRoot->InsertChild(pChild, nullptr);
      pChild = pItem;
    }

    if (GetXFANode()->GetPacketType() == XFA_PacketType::Form &&
        GetXFANode()->GetElementType() == XFA_Element::ExData) {
      CFX_XMLNode* pTempXMLNode = GetXFANode()->GetXMLMappingNode();
      GetXFANode()->SetXMLMappingNode(pFakeXMLRoot);

      if (pTempXMLNode && !pTempXMLNode->GetParent())
        pFakeXMLRoot = pTempXMLNode;
      else
        pFakeXMLRoot = nullptr;
    }
    MoveBufferMapData(pFakeRoot, GetXFANode());
  } else {
    CXFA_Node* pChild = pFakeRoot->GetFirstChild();
    while (pChild) {
      CXFA_Node* pItem = pChild->GetNextSibling();
      pFakeRoot->RemoveChild(pChild, true);
      GetXFANode()->InsertChild(pChild, nullptr);
      pChild->SetFlagAndNotify(XFA_NodeFlag_Initialized);
      pChild = pItem;
    }
  }

  if (pFakeXMLRoot) {
    pFakeRoot->SetXMLMappingNode(std::move(pFakeXMLRoot));
  }
  pFakeRoot->SetFlag(XFA_NodeFlag_HasRemovedChildren);

  return CJS_Result::Success();
}

CJS_Result CJX_Node::saveFilteredXML(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // TODO(weili): Check whether we need to implement this, pdfium:501.
  return CJS_Result::Success();
}

CJS_Result CJX_Node::saveXML(CFX_V8* runtime,
                             const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() > 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  if (params.size() == 1 &&
      !runtime->ToWideString(params[0]).EqualsASCII("pretty")) {
    return CJS_Result::Failure(JSMessage::kValueError);
  }

  // TODO(weili): Check whether we need to save pretty print XML, pdfium:501.

  ByteString bsXMLHeader = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  if (GetXFANode()->GetPacketType() != XFA_PacketType::Form &&
      GetXFANode()->GetPacketType() != XFA_PacketType::Datasets) {
    return CJS_Result::Success(runtime->NewString(""));
  }

  CFX_XMLNode* pElement = nullptr;
  if (GetXFANode()->GetPacketType() == XFA_PacketType::Datasets) {
    pElement = GetXFANode()->GetXMLMappingNode();
    if (!pElement || pElement->GetType() != FX_XMLNODE_Element) {
      return CJS_Result::Success(
          runtime->NewString(bsXMLHeader.AsStringView()));
    }

    XFA_DataExporter_DealWithDataGroupNode(GetXFANode());
  }

  auto pMemoryStream = pdfium::MakeRetain<CFX_MemoryStream>();
  pMemoryStream->WriteString(bsXMLHeader.AsStringView());

  if (GetXFANode()->GetPacketType() == XFA_PacketType::Form) {
    XFA_DataExporter_RegenerateFormFile(GetXFANode(), pMemoryStream, true);
  } else {
    pElement->Save(pMemoryStream);
  }

  return CJS_Result::Success(runtime->NewString(
      ByteStringView(pMemoryStream->GetBuffer(), pMemoryStream->GetSize())));
}

CJS_Result CJX_Node::setAttribute(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  WideString attributeValue = runtime->ToWideString(params[0]);
  WideString attribute = runtime->ToWideString(params[1]);
  SetAttribute(attribute.AsStringView(), attributeValue.AsStringView(), true);
  return CJS_Result::Success();
}

CJS_Result CJX_Node::setElement(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1 && params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  // TODO(weili): check whether we need to implement this, pdfium:501.
  return CJS_Result::Success();
}

void CJX_Node::ns(CFXJSE_Value* pValue,
                  bool bSetting,
                  XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }
  pValue->SetString(
      TryNamespace().value_or(WideString()).ToUTF8().AsStringView());
}

void CJX_Node::model(CFXJSE_Value* pValue,
                     bool bSetting,
                     XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }
  pValue->Assign(GetDocument()->GetScriptContext()->GetJSValueFromMap(
      GetXFANode()->GetModelNode()));
}

void CJX_Node::isContainer(CFXJSE_Value* pValue,
                           bool bSetting,
                           XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }
  pValue->SetBoolean(GetXFANode()->IsContainerNode());
}

void CJX_Node::isNull(CFXJSE_Value* pValue,
                      bool bSetting,
                      XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }
  if (GetXFANode()->GetElementType() == XFA_Element::Subform) {
    pValue->SetBoolean(false);
    return;
  }
  pValue->SetBoolean(GetContent(false).IsEmpty());
}

void CJX_Node::oneOfChild(CFXJSE_Value* pValue,
                          bool bSetting,
                          XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  std::vector<CXFA_Node*> properties = GetXFANode()->GetNodeList(
      XFA_NODEFILTER_OneOfProperty, XFA_Element::Unknown);
  if (!properties.empty()) {
    pValue->Assign(GetDocument()->GetScriptContext()->GetJSValueFromMap(
        properties.front()));
  }
}

int32_t CJX_Node::execSingleEventByName(WideStringView wsEventName,
                                        XFA_Element eType) {
  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (!pNotify)
    return XFA_EVENTERROR_NotExist;

  const XFA_ExecEventParaInfo* eventParaInfo =
      GetEventParaInfoByName(wsEventName);
  if (!eventParaInfo)
    return XFA_EVENTERROR_NotExist;

  switch (eventParaInfo->m_validFlags) {
    case EventAppliesToo::kNone:
      return XFA_EVENTERROR_NotExist;
    case EventAppliesToo::kAll:
    case EventAppliesToo::kAllNonRecursive:
      return pNotify->ExecEventByDeepFirst(
          GetXFANode(), eventParaInfo->m_eventType, false,
          eventParaInfo->m_validFlags == EventAppliesToo::kAll);
    case EventAppliesToo::kSubform:
      if (eType != XFA_Element::Subform)
        return XFA_EVENTERROR_NotExist;

      return pNotify->ExecEventByDeepFirst(
          GetXFANode(), eventParaInfo->m_eventType, false, false);
    case EventAppliesToo::kFieldOrExclusion: {
      if (eType != XFA_Element::ExclGroup && eType != XFA_Element::Field)
        return XFA_EVENTERROR_NotExist;

      CXFA_Node* pParentNode = GetXFANode()->GetParent();
      if (pParentNode &&
          pParentNode->GetElementType() == XFA_Element::ExclGroup) {
        // TODO(dsinclair): This seems like a bug, we do the same work twice?
        pNotify->ExecEventByDeepFirst(GetXFANode(), eventParaInfo->m_eventType,
                                      false, false);
      }
      return pNotify->ExecEventByDeepFirst(
          GetXFANode(), eventParaInfo->m_eventType, false, false);
    }
    case EventAppliesToo::kField:
      if (eType != XFA_Element::Field)
        return XFA_EVENTERROR_NotExist;

      return pNotify->ExecEventByDeepFirst(
          GetXFANode(), eventParaInfo->m_eventType, false, false);
    case EventAppliesToo::kSignature: {
      if (!GetXFANode()->IsWidgetReady())
        return XFA_EVENTERROR_NotExist;
      if (GetXFANode()->GetUIChildNode()->GetElementType() !=
          XFA_Element::Signature) {
        return XFA_EVENTERROR_NotExist;
      }
      return pNotify->ExecEventByDeepFirst(
          GetXFANode(), eventParaInfo->m_eventType, false, false);
    }
    case EventAppliesToo::kChoiceList: {
      if (!GetXFANode()->IsWidgetReady())
        return XFA_EVENTERROR_NotExist;
      if (GetXFANode()->GetUIChildNode()->GetElementType() !=
          XFA_Element::ChoiceList) {
        return XFA_EVENTERROR_NotExist;
      }
      return pNotify->ExecEventByDeepFirst(
          GetXFANode(), eventParaInfo->m_eventType, false, false);
    }
  }
  return XFA_EVENTERROR_NotExist;
}
