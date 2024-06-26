// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_object.h"

#include <tuple>

#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmltext.h"
#include "fxjs/cjs_result.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "fxjs/xfa/cjx_boolean.h"
#include "fxjs/xfa/cjx_draw.h"
#include "fxjs/xfa/cjx_field.h"
#include "fxjs/xfa/cjx_instancemanager.h"
#include "third_party/base/compiler_specific.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fgas/crt/cfgas_decimal.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/parser/cxfa_border.h"
#include "xfa/fxfa/parser/cxfa_datavalue.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_edge.h"
#include "xfa/fxfa/parser/cxfa_fill.h"
#include "xfa/fxfa/parser/cxfa_font.h"
#include "xfa/fxfa/parser/cxfa_layoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_object.h"
#include "xfa/fxfa/parser/cxfa_occur.h"
#include "xfa/fxfa/parser/cxfa_proto.h"
#include "xfa/fxfa/parser/cxfa_subform.h"
#include "xfa/fxfa/parser/cxfa_validate.h"
#include "xfa/fxfa/parser/cxfa_value.h"
#include "xfa/fxfa/parser/xfa_basic_data.h"
#include "xfa/fxfa/parser/xfa_utils.h"

namespace {

void XFA_DeleteWideString(void* pData) {
  delete static_cast<WideString*>(pData);
}

void XFA_CopyWideString(void*& pData) {
  if (!pData)
    return;
  pData = new WideString(*reinterpret_cast<WideString*>(pData));
}

const XFA_MAPDATABLOCKCALLBACKINFO deleteWideStringCallBack = {
    XFA_DeleteWideString, XFA_CopyWideString};

enum XFA_KEYTYPE {
  XFA_KEYTYPE_Custom,
  XFA_KEYTYPE_Element,
};

void* GetMapKey_Custom(WideStringView wsKey) {
  uint32_t dwKey = FX_HashCode_GetW(wsKey, false);
  return (void*)(uintptr_t)((dwKey << 1) | XFA_KEYTYPE_Custom);
}

void* GetMapKey_Element(XFA_Element eType, XFA_Attribute eAttribute) {
  return (void*)(uintptr_t)((static_cast<uint32_t>(eType) << 16) |
                            (static_cast<uint32_t>(eAttribute) << 8) |
                            XFA_KEYTYPE_Element);
}

void XFA_DefaultFreeData(void* pData) {}

const XFA_MAPDATABLOCKCALLBACKINFO gs_XFADefaultFreeData = {XFA_DefaultFreeData,
                                                            nullptr};

std::tuple<int32_t, int32_t, int32_t> StrToRGB(const WideString& strRGB) {
  int32_t r = 0;
  int32_t g = 0;
  int32_t b = 0;

  size_t iIndex = 0;
  for (size_t i = 0; i < strRGB.GetLength(); ++i) {
    wchar_t ch = strRGB[i];
    if (ch == L',')
      ++iIndex;
    if (iIndex > 2)
      break;

    int32_t iValue = ch - L'0';
    if (iValue >= 0 && iValue <= 9) {
      switch (iIndex) {
        case 0:
          r = r * 10 + iValue;
          break;
        case 1:
          g = g * 10 + iValue;
          break;
        default:
          b = b * 10 + iValue;
          break;
      }
    }
  }
  return {r, g, b};
}

}  // namespace

struct XFA_MAPDATABLOCK {
  uint8_t* GetData() const { return (uint8_t*)this + sizeof(XFA_MAPDATABLOCK); }

  const XFA_MAPDATABLOCKCALLBACKINFO* pCallbackInfo;
  int32_t iBytes;
};

struct XFA_MAPMODULEDATA {
  XFA_MAPMODULEDATA() {}
  ~XFA_MAPMODULEDATA() {}

  std::map<void*, void*> m_ValueMap;
  std::map<void*, XFA_MAPDATABLOCK*> m_BufferMap;
};

CJX_Object::CJX_Object(CXFA_Object* obj) : object_(obj) {}

CJX_Object::~CJX_Object() {
  ClearMapModuleBuffer();
}

bool CJX_Object::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__;
}

void CJX_Object::DefineMethods(pdfium::span<const CJX_MethodSpec> methods) {
  for (const auto& item : methods)
    method_specs_[item.pName] = item.pMethodCall;
}

CXFA_Document* CJX_Object::GetDocument() const {
  return object_->GetDocument();
}

void CJX_Object::className(CFXJSE_Value* pValue,
                           bool bSetting,
                           XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }
  pValue->SetString(GetXFAObject()->GetClassName());
}

int32_t CJX_Object::Subform_and_SubformSet_InstanceIndex() {
  int32_t index = 0;
  for (CXFA_Node* pNode = ToNode(GetXFAObject())->GetPrevSibling(); pNode;
       pNode = pNode->GetPrevSibling()) {
    if ((pNode->GetElementType() != XFA_Element::Subform) &&
        (pNode->GetElementType() != XFA_Element::SubformSet)) {
      break;
    }
    index++;
  }
  return index;
}

bool CJX_Object::HasMethod(const WideString& func) const {
  return pdfium::ContainsKey(method_specs_, func.ToUTF8());
}

CJS_Result CJX_Object::RunMethod(
    const WideString& func,
    const std::vector<v8::Local<v8::Value>>& params) {
  auto it = method_specs_.find(func.ToUTF8());
  if (it == method_specs_.end())
    return CJS_Result::Failure(JSMessage::kUnknownMethod);

  return it->second(this, GetXFAObject()->GetDocument()->GetScriptContext(),
                    params);
}

void CJX_Object::ThrowTooManyOccurancesException(const WideString& obj) const {
  ThrowException(WideString::FromASCII("The element [") + obj +
                 WideString::FromASCII(
                     "] has violated its allowable number of occurrences."));
}

void CJX_Object::ThrowInvalidPropertyException() const {
  ThrowException(WideString::FromASCII("Invalid property set operation."));
}

void CJX_Object::ThrowIndexOutOfBoundsException() const {
  ThrowException(WideString::FromASCII("Index value is out of bounds."));
}

void CJX_Object::ThrowParamCountMismatchException(
    const WideString& method) const {
  ThrowException(
      WideString::FromASCII("Incorrect number of parameters calling method '") +
      method + WideString::FromASCII("'."));
}

void CJX_Object::ThrowArgumentMismatchException() const {
  ThrowException(WideString::FromASCII(
      "Argument mismatch in property or function argument."));
}

void CJX_Object::ThrowException(const WideString& str) const {
  ASSERT(!str.IsEmpty());
  FXJSE_ThrowMessage(str.ToUTF8().AsStringView());
}

bool CJX_Object::HasAttribute(XFA_Attribute eAttr) {
  void* pKey = GetMapKey_Element(GetXFAObject()->GetElementType(), eAttr);
  return HasMapModuleKey(pKey);
}

void CJX_Object::SetAttribute(XFA_Attribute eAttr,
                              WideStringView wsValue,
                              bool bNotify) {
  switch (ToNode(GetXFAObject())->GetAttributeType(eAttr)) {
    case XFA_AttributeType::Enum: {
      Optional<XFA_AttributeValue> item = XFA_GetAttributeValueByName(wsValue);
      SetEnum(eAttr,
              item ? *item : *(ToNode(GetXFAObject())->GetDefaultEnum(eAttr)),
              bNotify);
      break;
    }
    case XFA_AttributeType::CData:
      SetCData(eAttr, WideString(wsValue), bNotify, false);
      break;
    case XFA_AttributeType::Boolean:
      SetBoolean(eAttr, !wsValue.EqualsASCII("0"), bNotify);
      break;
    case XFA_AttributeType::Integer:
      SetInteger(eAttr,
                 FXSYS_round(FXSYS_wcstof(wsValue.unterminated_c_str(),
                                          wsValue.GetLength(), nullptr)),
                 bNotify);
      break;
    case XFA_AttributeType::Measure:
      SetMeasure(eAttr, CXFA_Measurement(wsValue), bNotify);
      break;
    default:
      break;
  }
}

void CJX_Object::SetMapModuleString(void* pKey, WideStringView wsValue) {
  SetMapModuleBuffer(pKey, const_cast<wchar_t*>(wsValue.unterminated_c_str()),
                     wsValue.GetLength() * sizeof(wchar_t), nullptr);
}

void CJX_Object::SetAttribute(WideStringView wsAttr,
                              WideStringView wsValue,
                              bool bNotify) {
  Optional<XFA_ATTRIBUTEINFO> attr = XFA_GetAttributeByName(wsValue);
  if (!attr.has_value()) {
    SetAttribute(attr.value().attribute, wsValue, bNotify);
    return;
  }
  void* pKey = GetMapKey_Custom(wsAttr);
  SetMapModuleString(pKey, wsValue);
}

WideString CJX_Object::GetAttribute(WideStringView attr) {
  return TryAttribute(attr, true).value_or(WideString());
}

WideString CJX_Object::GetAttribute(XFA_Attribute attr) {
  return TryAttribute(attr, true).value_or(WideString());
}

Optional<WideString> CJX_Object::TryAttribute(XFA_Attribute eAttr,
                                              bool bUseDefault) {
  switch (ToNode(GetXFAObject())->GetAttributeType(eAttr)) {
    case XFA_AttributeType::Enum: {
      Optional<XFA_AttributeValue> value = TryEnum(eAttr, bUseDefault);
      if (!value)
        return {};
      return WideString::FromASCII(XFA_AttributeValueToName(*value));
    }
    case XFA_AttributeType::CData:
      return TryCData(eAttr, bUseDefault);

    case XFA_AttributeType::Boolean: {
      Optional<bool> value = TryBoolean(eAttr, bUseDefault);
      if (!value)
        return {};
      return WideString(*value ? L"1" : L"0");
    }
    case XFA_AttributeType::Integer: {
      Optional<int32_t> iValue = TryInteger(eAttr, bUseDefault);
      if (!iValue)
        return {};
      return WideString::Format(L"%d", *iValue);
    }
    case XFA_AttributeType::Measure: {
      Optional<CXFA_Measurement> value = TryMeasure(eAttr, bUseDefault);
      if (!value)
        return {};

      return value->ToString();
    }
    default:
      break;
  }
  return {};
}

Optional<WideString> CJX_Object::TryAttribute(WideStringView wsAttr,
                                              bool bUseDefault) {
  Optional<XFA_ATTRIBUTEINFO> attr = XFA_GetAttributeByName(wsAttr);
  if (attr.has_value())
    return TryAttribute(attr.value().attribute, bUseDefault);
  return GetMapModuleString(GetMapKey_Custom(wsAttr));
}

void CJX_Object::RemoveAttribute(WideStringView wsAttr) {
  void* pKey = GetMapKey_Custom(wsAttr);
  if (pKey)
    RemoveMapModuleKey(pKey);
}

Optional<bool> CJX_Object::TryBoolean(XFA_Attribute eAttr, bool bUseDefault) {
  void* pKey = GetMapKey_Element(GetXFAObject()->GetElementType(), eAttr);
  Optional<void*> value = GetMapModuleValue(pKey);
  if (value.has_value())
    return !!value.value();
  if (!bUseDefault)
    return {};
  return ToNode(GetXFAObject())->GetDefaultBoolean(eAttr);
}

void CJX_Object::SetBoolean(XFA_Attribute eAttr, bool bValue, bool bNotify) {
  CFX_XMLElement* elem = SetValue(eAttr, (void*)(uintptr_t)bValue, bNotify);
  if (elem) {
    elem->SetAttribute(WideString::FromASCII(XFA_AttributeToName(eAttr)),
                       bValue ? L"1" : L"0");
  }
}

bool CJX_Object::GetBoolean(XFA_Attribute eAttr) {
  return TryBoolean(eAttr, true).value_or(false);
}

void CJX_Object::SetInteger(XFA_Attribute eAttr, int32_t iValue, bool bNotify) {
  CFX_XMLElement* elem = SetValue(eAttr, (void*)(uintptr_t)iValue, bNotify);
  if (elem) {
    elem->SetAttribute(WideString::FromASCII(XFA_AttributeToName(eAttr)),
                       WideString::Format(L"%d", iValue));
  }
}

int32_t CJX_Object::GetInteger(XFA_Attribute eAttr) {
  return TryInteger(eAttr, true).value_or(0);
}

Optional<int32_t> CJX_Object::TryInteger(XFA_Attribute eAttr,
                                         bool bUseDefault) {
  void* pKey = GetMapKey_Element(GetXFAObject()->GetElementType(), eAttr);
  Optional<void*> value = GetMapModuleValue(pKey);
  if (value.has_value())
    return static_cast<int32_t>(reinterpret_cast<uintptr_t>(value.value()));
  if (!bUseDefault)
    return {};
  return ToNode(GetXFAObject())->GetDefaultInteger(eAttr);
}

Optional<XFA_AttributeValue> CJX_Object::TryEnum(XFA_Attribute eAttr,
                                                 bool bUseDefault) const {
  void* pKey = GetMapKey_Element(GetXFAObject()->GetElementType(), eAttr);
  Optional<void*> value = GetMapModuleValue(pKey);
  if (value.has_value()) {
    return static_cast<XFA_AttributeValue>(
        reinterpret_cast<uintptr_t>(value.value()));
  }
  if (!bUseDefault)
    return {};
  return ToNode(GetXFAObject())->GetDefaultEnum(eAttr);
}

void CJX_Object::SetEnum(XFA_Attribute eAttr,
                         XFA_AttributeValue eValue,
                         bool bNotify) {
  CFX_XMLElement* elem = SetValue(eAttr, (void*)(uintptr_t)eValue, bNotify);
  if (elem) {
    elem->SetAttribute(WideString::FromASCII(XFA_AttributeToName(eAttr)),
                       WideString::FromASCII(XFA_AttributeValueToName(eValue)));
  }
}

XFA_AttributeValue CJX_Object::GetEnum(XFA_Attribute eAttr) const {
  return TryEnum(eAttr, true).value_or(XFA_AttributeValue::Unknown);
}

void CJX_Object::SetMeasure(XFA_Attribute eAttr,
                            CXFA_Measurement mValue,
                            bool bNotify) {
  void* pKey = GetMapKey_Element(GetXFAObject()->GetElementType(), eAttr);
  OnChanging(eAttr, bNotify);
  SetMapModuleBuffer(pKey, &mValue, sizeof(CXFA_Measurement), nullptr);
  OnChanged(eAttr, bNotify, false);
}

Optional<CXFA_Measurement> CJX_Object::TryMeasure(XFA_Attribute eAttr,
                                                  bool bUseDefault) const {
  void* pKey = GetMapKey_Element(GetXFAObject()->GetElementType(), eAttr);
  void* pValue;
  int32_t iBytes;
  if (GetMapModuleBuffer(pKey, &pValue, &iBytes) &&
      iBytes == sizeof(CXFA_Measurement)) {
    return *static_cast<CXFA_Measurement*>(pValue);
  }
  if (!bUseDefault)
    return {};
  return ToNode(GetXFAObject())->GetDefaultMeasurement(eAttr);
}

Optional<float> CJX_Object::TryMeasureAsFloat(XFA_Attribute attr) const {
  Optional<CXFA_Measurement> measure = TryMeasure(attr, false);
  if (measure)
    return measure->ToUnit(XFA_Unit::Pt);
  return {};
}

CXFA_Measurement CJX_Object::GetMeasure(XFA_Attribute eAttr) const {
  return TryMeasure(eAttr, true).value_or(CXFA_Measurement());
}

WideString CJX_Object::GetCData(XFA_Attribute eAttr) {
  return TryCData(eAttr, true).value_or(WideString());
}

void CJX_Object::SetCData(XFA_Attribute eAttr,
                          const WideString& wsValue,
                          bool bNotify,
                          bool bScriptModify) {
  CXFA_Node* xfaObj = ToNode(GetXFAObject());
  void* pKey = GetMapKey_Element(xfaObj->GetElementType(), eAttr);
  OnChanging(eAttr, bNotify);
  if (eAttr == XFA_Attribute::Value) {
    WideString* pClone = new WideString(wsValue);
    SetUserData(pKey, pClone, &deleteWideStringCallBack);
  } else {
    SetMapModuleString(pKey, wsValue.AsStringView());
    if (eAttr == XFA_Attribute::Name)
      xfaObj->UpdateNameHash();
  }
  OnChanged(eAttr, bNotify, bScriptModify);

  if (!xfaObj->IsNeedSavingXMLNode() || eAttr == XFA_Attribute::QualifiedName ||
      eAttr == XFA_Attribute::BindingNode) {
    return;
  }

  if (eAttr == XFA_Attribute::Name &&
      (xfaObj->GetElementType() == XFA_Element::DataValue ||
       xfaObj->GetElementType() == XFA_Element::DataGroup)) {
    return;
  }

  if (eAttr == XFA_Attribute::Value) {
    xfaObj->SetToXML(wsValue);
    return;
  }

  WideString wsAttrName = WideString::FromASCII(XFA_AttributeToName(eAttr));
  if (eAttr == XFA_Attribute::ContentType)
    wsAttrName = L"xfa:" + wsAttrName;

  CFX_XMLElement* elem = ToXMLElement(xfaObj->GetXMLMappingNode());
  elem->SetAttribute(wsAttrName, wsValue);
  return;
}

void CJX_Object::SetAttributeValue(const WideString& wsValue,
                                   const WideString& wsXMLValue,
                                   bool bNotify,
                                   bool bScriptModify) {
  auto* xfaObj = ToNode(GetXFAObject());
  void* pKey =
      GetMapKey_Element(xfaObj->GetElementType(), XFA_Attribute::Value);

  OnChanging(XFA_Attribute::Value, bNotify);
  WideString* pClone = new WideString(wsValue);

  SetUserData(pKey, pClone, &deleteWideStringCallBack);
  OnChanged(XFA_Attribute::Value, bNotify, bScriptModify);

  if (!xfaObj->IsNeedSavingXMLNode())
    return;

  xfaObj->SetToXML(wsXMLValue);
}

Optional<WideString> CJX_Object::TryCData(XFA_Attribute eAttr,
                                          bool bUseDefault) {
  void* pKey = GetMapKey_Element(GetXFAObject()->GetElementType(), eAttr);
  if (eAttr == XFA_Attribute::Value) {
    void* pData;
    int32_t iBytes = 0;
    WideString* pStr = nullptr;
    if (GetMapModuleBuffer(pKey, &pData, &iBytes) && iBytes == sizeof(void*)) {
      memcpy(&pData, pData, iBytes);
      pStr = reinterpret_cast<WideString*>(pData);
    }
    if (pStr)
      return *pStr;
  } else {
    Optional<WideString> value = GetMapModuleString(pKey);
    if (value.has_value())
      return value;
  }
  if (!bUseDefault)
    return {};
  return ToNode(GetXFAObject())->GetDefaultCData(eAttr);
}

CFX_XMLElement* CJX_Object::SetValue(XFA_Attribute eAttr,
                                     void* pValue,
                                     bool bNotify) {
  void* pKey = GetMapKey_Element(GetXFAObject()->GetElementType(), eAttr);
  OnChanging(eAttr, bNotify);
  SetMapModuleValue(pKey, pValue);
  OnChanged(eAttr, bNotify, false);

  CXFA_Node* pNode = ToNode(GetXFAObject());
  return pNode->IsNeedSavingXMLNode() ? ToXMLElement(pNode->GetXMLMappingNode())
                                      : nullptr;
}

void CJX_Object::SetContent(const WideString& wsContent,
                            const WideString& wsXMLValue,
                            bool bNotify,
                            bool bScriptModify,
                            bool bSyncData) {
  CXFA_Node* pNode = nullptr;
  CXFA_Node* pBindNode = nullptr;
  switch (ToNode(GetXFAObject())->GetObjectType()) {
    case XFA_ObjectType::ContainerNode: {
      if (XFA_FieldIsMultiListBox(ToNode(GetXFAObject()))) {
        CXFA_Value* pValue =
            GetOrCreateProperty<CXFA_Value>(0, XFA_Element::Value);
        if (!pValue)
          break;

        CXFA_Node* pChildValue = pValue->GetFirstChild();
        ASSERT(pChildValue);
        pChildValue->JSObject()->SetCData(XFA_Attribute::ContentType,
                                          L"text/xml", false, false);
        pChildValue->JSObject()->SetContent(wsContent, wsContent, bNotify,
                                            bScriptModify, false);
        CXFA_Node* pBind = ToNode(GetXFAObject())->GetBindData();
        if (bSyncData && pBind) {
          std::vector<WideString> wsSaveTextArray;
          size_t iSize = 0;
          if (!wsContent.IsEmpty()) {
            size_t iStart = 0;
            size_t iLength = wsContent.GetLength();
            auto iEnd = wsContent.Find(L'\n', iStart);
            iEnd = !iEnd.has_value() ? iLength : iEnd;
            while (iEnd.value() >= iStart) {
              wsSaveTextArray.push_back(
                  wsContent.Mid(iStart, iEnd.value() - iStart));
              iStart = iEnd.value() + 1;
              if (iStart >= iLength)
                break;

              iEnd = wsContent.Find(L'\n', iStart);
              if (!iEnd.has_value()) {
                wsSaveTextArray.push_back(
                    wsContent.Mid(iStart, iLength - iStart));
              }
            }
            iSize = wsSaveTextArray.size();
          }
          if (iSize == 0) {
            while (CXFA_Node* pChildNode = pBind->GetFirstChild()) {
              pBind->RemoveChild(pChildNode, true);
            }
          } else {
            std::vector<CXFA_Node*> valueNodes = pBind->GetNodeList(
                XFA_NODEFILTER_Children, XFA_Element::DataValue);
            size_t iDatas = valueNodes.size();
            if (iDatas < iSize) {
              size_t iAddNodes = iSize - iDatas;
              CXFA_Node* pValueNodes = nullptr;
              while (iAddNodes-- > 0) {
                pValueNodes =
                    pBind->CreateSamePacketNode(XFA_Element::DataValue);
                pValueNodes->JSObject()->SetCData(XFA_Attribute::Name, L"value",
                                                  false, false);
                pValueNodes->CreateXMLMappingNode();
                pBind->InsertChild(pValueNodes, nullptr);
              }
              pValueNodes = nullptr;
            } else if (iDatas > iSize) {
              size_t iDelNodes = iDatas - iSize;
              while (iDelNodes-- > 0) {
                pBind->RemoveChild(pBind->GetFirstChild(), true);
              }
            }
            int32_t i = 0;
            for (CXFA_Node* pValueNode = pBind->GetFirstChild(); pValueNode;
                 pValueNode = pValueNode->GetNextSibling()) {
              pValueNode->JSObject()->SetAttributeValue(
                  wsSaveTextArray[i], wsSaveTextArray[i], false, false);
              i++;
            }
          }
          for (auto* pArrayNode : *(pBind->GetBindItems())) {
            if (pArrayNode != ToNode(GetXFAObject())) {
              pArrayNode->JSObject()->SetContent(wsContent, wsContent, bNotify,
                                                 bScriptModify, false);
            }
          }
        }
        break;
      }
      if (ToNode(GetXFAObject())->GetElementType() == XFA_Element::ExclGroup) {
        pNode = ToNode(GetXFAObject());
      } else {
        CXFA_Value* pValue =
            GetOrCreateProperty<CXFA_Value>(0, XFA_Element::Value);
        if (!pValue)
          break;

        CXFA_Node* pChildValue = pValue->GetFirstChild();
        ASSERT(pChildValue);
        pChildValue->JSObject()->SetContent(wsContent, wsContent, bNotify,
                                            bScriptModify, false);
      }
      pBindNode = ToNode(GetXFAObject())->GetBindData();
      if (pBindNode && bSyncData) {
        pBindNode->JSObject()->SetContent(wsContent, wsXMLValue, bNotify,
                                          bScriptModify, false);
        for (auto* pArrayNode : *(pBindNode->GetBindItems())) {
          if (pArrayNode != ToNode(GetXFAObject())) {
            pArrayNode->JSObject()->SetContent(wsContent, wsContent, bNotify,
                                               true, false);
          }
        }
      }
      pBindNode = nullptr;
      break;
    }
    case XFA_ObjectType::ContentNode: {
      WideString wsContentType;
      if (ToNode(GetXFAObject())->GetElementType() == XFA_Element::ExData) {
        Optional<WideString> ret =
            TryAttribute(XFA_Attribute::ContentType, false);
        if (ret)
          wsContentType = *ret;
        if (wsContentType.EqualsASCII("text/html")) {
          wsContentType.clear();
          SetAttribute(XFA_Attribute::ContentType, wsContentType.AsStringView(),
                       false);
        }
      }

      CXFA_Node* pContentRawDataNode = ToNode(GetXFAObject())->GetFirstChild();
      if (!pContentRawDataNode) {
        pContentRawDataNode =
            ToNode(GetXFAObject())
                ->CreateSamePacketNode(wsContentType.EqualsASCII("text/xml")
                                           ? XFA_Element::Sharpxml
                                           : XFA_Element::Sharptext);
        ToNode(GetXFAObject())->InsertChild(pContentRawDataNode, nullptr);
      }
      pContentRawDataNode->JSObject()->SetContent(
          wsContent, wsXMLValue, bNotify, bScriptModify, bSyncData);
      return;
    }
    case XFA_ObjectType::NodeC:
    case XFA_ObjectType::TextNode:
      pNode = ToNode(GetXFAObject());
      break;
    case XFA_ObjectType::NodeV:
      pNode = ToNode(GetXFAObject());
      if (bSyncData &&
          ToNode(GetXFAObject())->GetPacketType() == XFA_PacketType::Form) {
        CXFA_Node* pParent = ToNode(GetXFAObject())->GetParent();
        if (pParent) {
          pParent = pParent->GetParent();
        }
        if (pParent && pParent->GetElementType() == XFA_Element::Value) {
          pParent = pParent->GetParent();
          if (pParent && pParent->IsContainerNode()) {
            pBindNode = pParent->GetBindData();
            if (pBindNode) {
              pBindNode->JSObject()->SetContent(wsContent, wsXMLValue, bNotify,
                                                bScriptModify, false);
            }
          }
        }
      }
      break;
    default:
      if (ToNode(GetXFAObject())->GetElementType() == XFA_Element::DataValue) {
        pNode = ToNode(GetXFAObject());
        pBindNode = ToNode(GetXFAObject());
      }
      break;
  }
  if (!pNode)
    return;

  SetAttributeValue(wsContent, wsXMLValue, bNotify, bScriptModify);
  if (pBindNode && bSyncData) {
    for (auto* pArrayNode : *(pBindNode->GetBindItems())) {
      pArrayNode->JSObject()->SetContent(wsContent, wsContent, bNotify,
                                         bScriptModify, false);
    }
  }
}

WideString CJX_Object::GetContent(bool bScriptModify) {
  return TryContent(bScriptModify, true).value_or(WideString());
}

Optional<WideString> CJX_Object::TryContent(bool bScriptModify, bool bProto) {
  CXFA_Node* pNode = nullptr;
  switch (ToNode(GetXFAObject())->GetObjectType()) {
    case XFA_ObjectType::ContainerNode:
      if (ToNode(GetXFAObject())->GetElementType() == XFA_Element::ExclGroup) {
        pNode = ToNode(GetXFAObject());
      } else {
        CXFA_Value* pValue =
            ToNode(GetXFAObject())
                ->GetChild<CXFA_Value>(0, XFA_Element::Value, false);
        if (!pValue)
          return {};

        CXFA_Node* pChildValue = pValue->GetFirstChild();
        if (pChildValue && XFA_FieldIsMultiListBox(ToNode(GetXFAObject()))) {
          pChildValue->JSObject()->SetAttribute(XFA_Attribute::ContentType,
                                                L"text/xml", false);
        }
        if (pChildValue)
          return pChildValue->JSObject()->TryContent(bScriptModify, bProto);
        return {};
      }
      break;
    case XFA_ObjectType::ContentNode: {
      CXFA_Node* pContentRawDataNode = ToNode(GetXFAObject())->GetFirstChild();
      if (!pContentRawDataNode) {
        XFA_Element element = XFA_Element::Sharptext;
        if (ToNode(GetXFAObject())->GetElementType() == XFA_Element::ExData) {
          Optional<WideString> contentType =
              TryAttribute(XFA_Attribute::ContentType, false);
          if (contentType.has_value()) {
            if (contentType.value().EqualsASCII("text/html"))
              element = XFA_Element::SharpxHTML;
            else if (contentType.value().EqualsASCII("text/xml"))
              element = XFA_Element::Sharpxml;
          }
        }
        pContentRawDataNode =
            ToNode(GetXFAObject())->CreateSamePacketNode(element);
        ToNode(GetXFAObject())->InsertChild(pContentRawDataNode, nullptr);
      }
      return pContentRawDataNode->JSObject()->TryContent(bScriptModify, true);
    }
    case XFA_ObjectType::NodeC:
    case XFA_ObjectType::NodeV:
    case XFA_ObjectType::TextNode:
      pNode = ToNode(GetXFAObject());
      FALLTHROUGH;
    default:
      if (ToNode(GetXFAObject())->GetElementType() == XFA_Element::DataValue)
        pNode = ToNode(GetXFAObject());
      break;
  }
  if (pNode) {
    if (bScriptModify) {
      CFXJSE_Engine* pScriptContext = GetDocument()->GetScriptContext();
      if (pScriptContext)
        GetDocument()->GetScriptContext()->AddNodesOfRunScript(
            ToNode(GetXFAObject()));
    }
    return TryCData(XFA_Attribute::Value, false);
  }
  return {};
}

Optional<WideString> CJX_Object::TryNamespace() {
  if (ToNode(GetXFAObject())->IsModelNode() ||
      ToNode(GetXFAObject())->GetElementType() == XFA_Element::Packet) {
    CFX_XMLNode* pXMLNode = ToNode(GetXFAObject())->GetXMLMappingNode();
    CFX_XMLElement* element = ToXMLElement(pXMLNode);
    if (!element)
      return {};

    return element->GetNamespaceURI();
  }

  if (ToNode(GetXFAObject())->GetPacketType() != XFA_PacketType::Datasets)
    return ToNode(GetXFAObject())->GetModelNode()->JSObject()->TryNamespace();

  CFX_XMLNode* pXMLNode = ToNode(GetXFAObject())->GetXMLMappingNode();
  CFX_XMLElement* element = ToXMLElement(pXMLNode);
  if (!element)
    return {};

  if (ToNode(GetXFAObject())->GetElementType() == XFA_Element::DataValue &&
      GetEnum(XFA_Attribute::Contains) == XFA_AttributeValue::MetaData) {
    WideString wsNamespace;
    if (!XFA_FDEExtension_ResolveNamespaceQualifier(
            element, GetCData(XFA_Attribute::QualifiedName), &wsNamespace)) {
      return {};
    }
    return wsNamespace;
  }
  return element->GetNamespaceURI();
}

std::pair<CXFA_Node*, int32_t> CJX_Object::GetPropertyInternal(
    int32_t index,
    XFA_Element eProperty) const {
  const CXFA_Node* xfaNode = ToNode(GetXFAObject());
  if (index < 0 || index >= xfaNode->PropertyOccuranceCount(eProperty))
    return {nullptr, 0};

  int32_t iCount = 0;
  for (CXFA_Node* pNode = xfaNode->GetFirstChild(); pNode;
       pNode = pNode->GetNextSibling()) {
    if (pNode->GetElementType() == eProperty) {
      iCount++;
      if (iCount > index)
        return {pNode, iCount};
    }
  }
  return {nullptr, iCount};
}

CXFA_Node* CJX_Object::GetOrCreatePropertyInternal(int32_t index,
                                                   XFA_Element eProperty) {
  CXFA_Node* xfaNode = ToNode(GetXFAObject());
  if (index < 0 || index >= xfaNode->PropertyOccuranceCount(eProperty))
    return nullptr;

  int32_t iCount = 0;
  CXFA_Node* node;
  std::tie(node, iCount) = GetPropertyInternal(index, eProperty);
  if (node)
    return node;

  if (xfaNode->HasPropertyFlags(eProperty, XFA_PROPERTYFLAG_OneOf)) {
    for (CXFA_Node* pNode = xfaNode->GetFirstChild(); pNode;
         pNode = pNode->GetNextSibling()) {
      if (xfaNode->HasPropertyFlags(pNode->GetElementType(),
                                    XFA_PROPERTYFLAG_OneOf)) {
        return nullptr;
      }
    }
  }

  CXFA_Node* pNewNode = nullptr;
  for (; iCount <= index; ++iCount) {
    pNewNode = GetDocument()->CreateNode(xfaNode->GetPacketType(), eProperty);
    if (!pNewNode)
      return nullptr;

    xfaNode->InsertChild(pNewNode, nullptr);
    pNewNode->SetFlagAndNotify(XFA_NodeFlag_Initialized);
  }
  return pNewNode;
}

void CJX_Object::SetUserData(
    void* pKey,
    void* pData,
    const XFA_MAPDATABLOCKCALLBACKINFO* pCallbackInfo) {
  SetMapModuleBuffer(pKey, &pData, sizeof(void*),
                     pCallbackInfo ? pCallbackInfo : &gs_XFADefaultFreeData);
}

XFA_MAPMODULEDATA* CJX_Object::CreateMapModuleData() {
  if (!map_module_data_)
    map_module_data_ = pdfium::MakeUnique<XFA_MAPMODULEDATA>();
  return map_module_data_.get();
}

XFA_MAPMODULEDATA* CJX_Object::GetMapModuleData() const {
  return map_module_data_.get();
}

void CJX_Object::SetMapModuleValue(void* pKey, void* pValue) {
  CreateMapModuleData()->m_ValueMap[pKey] = pValue;
}

Optional<void*> CJX_Object::GetMapModuleValue(void* pKey) const {
  for (const CXFA_Node* pNode = ToNode(GetXFAObject()); pNode;
       pNode = pNode->GetTemplateNodeIfExists()) {
    XFA_MAPMODULEDATA* pModule = pNode->JSObject()->GetMapModuleData();
    if (pModule) {
      auto it = pModule->m_ValueMap.find(pKey);
      if (it != pModule->m_ValueMap.end())
        return it->second;
    }
    if (pNode->GetPacketType() == XFA_PacketType::Datasets)
      break;
  }
  return {};
}

Optional<WideString> CJX_Object::GetMapModuleString(void* pKey) {
  void* pRawValue;
  int32_t iBytes;
  if (!GetMapModuleBuffer(pKey, &pRawValue, &iBytes))
    return {};

  // Defensive measure: no out-of-bounds pointers even if zero length.
  int32_t iChars = iBytes / sizeof(wchar_t);
  return WideString(iChars ? static_cast<const wchar_t*>(pRawValue) : nullptr,
                    iChars);
}

void CJX_Object::SetMapModuleBuffer(
    void* pKey,
    void* pValue,
    int32_t iBytes,
    const XFA_MAPDATABLOCKCALLBACKINFO* pCallbackInfo) {
  XFA_MAPDATABLOCK*& pBuffer = CreateMapModuleData()->m_BufferMap[pKey];
  if (!pBuffer) {
    pBuffer = reinterpret_cast<XFA_MAPDATABLOCK*>(
        FX_Alloc(uint8_t, sizeof(XFA_MAPDATABLOCK) + iBytes));
  } else if (pBuffer->iBytes != iBytes) {
    if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pFree)
      pBuffer->pCallbackInfo->pFree(*(void**)pBuffer->GetData());

    pBuffer = reinterpret_cast<XFA_MAPDATABLOCK*>(
        FX_Realloc(uint8_t, pBuffer, sizeof(XFA_MAPDATABLOCK) + iBytes));
  } else if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pFree) {
    pBuffer->pCallbackInfo->pFree(
        *reinterpret_cast<void**>(pBuffer->GetData()));
  }
  if (!pBuffer)
    return;

  pBuffer->pCallbackInfo = pCallbackInfo;
  pBuffer->iBytes = iBytes;
  memcpy(pBuffer->GetData(), pValue, iBytes);
}

bool CJX_Object::GetMapModuleBuffer(void* pKey,
                                    void** pValue,
                                    int32_t* pBytes) const {
  XFA_MAPDATABLOCK* pBuffer = nullptr;
  for (const CXFA_Node* pNode = ToNode(GetXFAObject()); pNode;
       pNode = pNode->GetTemplateNodeIfExists()) {
    XFA_MAPMODULEDATA* pModule = pNode->JSObject()->GetMapModuleData();
    if (pModule) {
      auto it = pModule->m_BufferMap.find(pKey);
      if (it != pModule->m_BufferMap.end()) {
        pBuffer = it->second;
        break;
      }
    }
    if (pNode->GetPacketType() == XFA_PacketType::Datasets)
      break;
  }
  if (!pBuffer)
    return false;

  *pValue = pBuffer->GetData();
  *pBytes = pBuffer->iBytes;
  return true;
}

bool CJX_Object::HasMapModuleKey(void* pKey) {
  XFA_MAPMODULEDATA* pModule = GetMapModuleData();
  return pModule && (pdfium::ContainsKey(pModule->m_ValueMap, pKey) ||
                     pdfium::ContainsKey(pModule->m_BufferMap, pKey));
}

void CJX_Object::ClearMapModuleBuffer() {
  XFA_MAPMODULEDATA* pModule = GetMapModuleData();
  if (!pModule)
    return;

  for (auto& pair : pModule->m_BufferMap) {
    XFA_MAPDATABLOCK* pBuffer = pair.second;
    if (pBuffer) {
      if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pFree)
        pBuffer->pCallbackInfo->pFree(*(void**)pBuffer->GetData());

      FX_Free(pBuffer);
    }
  }
  pModule->m_BufferMap.clear();
  pModule->m_ValueMap.clear();
}

void CJX_Object::RemoveMapModuleKey(void* pKey) {
  ASSERT(pKey);

  XFA_MAPMODULEDATA* pModule = GetMapModuleData();
  if (!pModule)
    return;

  auto it = pModule->m_BufferMap.find(pKey);
  if (it != pModule->m_BufferMap.end()) {
    XFA_MAPDATABLOCK* pBuffer = it->second;
    if (pBuffer) {
      if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pFree)
        pBuffer->pCallbackInfo->pFree(*(void**)pBuffer->GetData());

      FX_Free(pBuffer);
    }
    pModule->m_BufferMap.erase(it);
  }
  pModule->m_ValueMap.erase(pKey);
  return;
}

void CJX_Object::MergeAllData(CXFA_Object* pDstModule) {
  XFA_MAPMODULEDATA* pDstModuleData =
      ToNode(pDstModule)->JSObject()->CreateMapModuleData();
  XFA_MAPMODULEDATA* pSrcModuleData = GetMapModuleData();
  if (!pSrcModuleData)
    return;

  for (const auto& pair : pSrcModuleData->m_ValueMap)
    pDstModuleData->m_ValueMap[pair.first] = pair.second;

  for (const auto& pair : pSrcModuleData->m_BufferMap) {
    XFA_MAPDATABLOCK* pSrcBuffer = pair.second;
    XFA_MAPDATABLOCK*& pDstBuffer = pDstModuleData->m_BufferMap[pair.first];
    if (pSrcBuffer->pCallbackInfo && pSrcBuffer->pCallbackInfo->pFree &&
        !pSrcBuffer->pCallbackInfo->pCopy) {
      if (pDstBuffer) {
        pDstBuffer->pCallbackInfo->pFree(*(void**)pDstBuffer->GetData());
        pDstModuleData->m_BufferMap.erase(pair.first);
      }
      continue;
    }
    if (!pDstBuffer) {
      pDstBuffer = (XFA_MAPDATABLOCK*)FX_Alloc(
          uint8_t, sizeof(XFA_MAPDATABLOCK) + pSrcBuffer->iBytes);
    } else if (pDstBuffer->iBytes != pSrcBuffer->iBytes) {
      if (pDstBuffer->pCallbackInfo && pDstBuffer->pCallbackInfo->pFree) {
        pDstBuffer->pCallbackInfo->pFree(*(void**)pDstBuffer->GetData());
      }
      pDstBuffer = (XFA_MAPDATABLOCK*)FX_Realloc(
          uint8_t, pDstBuffer, sizeof(XFA_MAPDATABLOCK) + pSrcBuffer->iBytes);
    } else if (pDstBuffer->pCallbackInfo && pDstBuffer->pCallbackInfo->pFree) {
      pDstBuffer->pCallbackInfo->pFree(*(void**)pDstBuffer->GetData());
    }
    if (!pDstBuffer)
      continue;

    pDstBuffer->pCallbackInfo = pSrcBuffer->pCallbackInfo;
    pDstBuffer->iBytes = pSrcBuffer->iBytes;
    memcpy(pDstBuffer->GetData(), pSrcBuffer->GetData(), pSrcBuffer->iBytes);
    if (pDstBuffer->pCallbackInfo && pDstBuffer->pCallbackInfo->pCopy) {
      pDstBuffer->pCallbackInfo->pCopy(*(void**)pDstBuffer->GetData());
    }
  }
}

void CJX_Object::MoveBufferMapData(CXFA_Object* pDstModule) {
  if (!pDstModule)
    return;

  bool bNeedMove = true;
  if (pDstModule->GetElementType() != GetXFAObject()->GetElementType())
    bNeedMove = false;

  if (bNeedMove)
    ToNode(pDstModule)->JSObject()->SetCalcData(ReleaseCalcData());
  if (!pDstModule->IsNodeV())
    return;

  WideString wsValue = ToNode(pDstModule)->JSObject()->GetContent(false);
  WideString wsFormatValue(wsValue);
  CXFA_Node* pNode = ToNode(pDstModule)->GetContainerNode();
  if (pNode)
    wsFormatValue = pNode->GetFormatDataValue(wsValue);

  ToNode(pDstModule)
      ->JSObject()
      ->SetContent(wsValue, wsFormatValue, true, true, true);
}

void CJX_Object::MoveBufferMapData(CXFA_Object* pSrcModule,
                                   CXFA_Object* pDstModule) {
  if (!pSrcModule || !pDstModule)
    return;

  CXFA_Node* pSrcChild = ToNode(pSrcModule)->GetFirstChild();
  CXFA_Node* pDstChild = ToNode(pDstModule)->GetFirstChild();
  while (pSrcChild && pDstChild) {
    MoveBufferMapData(pSrcChild, pDstChild);

    pSrcChild = pSrcChild->GetNextSibling();
    pDstChild = pDstChild->GetNextSibling();
  }
  ToNode(pSrcModule)->JSObject()->MoveBufferMapData(pDstModule);
}

void CJX_Object::OnChanging(XFA_Attribute eAttr, bool bNotify) {
  if (!bNotify || !ToNode(GetXFAObject())->IsInitialized())
    return;

  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (pNotify)
    pNotify->OnValueChanging(ToNode(GetXFAObject()), eAttr);
}

void CJX_Object::OnChanged(XFA_Attribute eAttr,
                           bool bNotify,
                           bool bScriptModify) {
  if (bNotify && ToNode(GetXFAObject())->IsInitialized())
    ToNode(GetXFAObject())->SendAttributeChangeMessage(eAttr, bScriptModify);
}

void CJX_Object::SetCalcData(std::unique_ptr<CXFA_CalcData> data) {
  calc_data_ = std::move(data);
}

std::unique_ptr<CXFA_CalcData> CJX_Object::ReleaseCalcData() {
  return std::move(calc_data_);
}

void CJX_Object::ScriptAttributeString(CFXJSE_Value* pValue,
                                       bool bSetting,
                                       XFA_Attribute eAttribute) {
  if (!bSetting) {
    pValue->SetString(GetAttribute(eAttribute).ToUTF8().AsStringView());
    return;
  }

  WideString wsValue = pValue->ToWideString();
  SetAttribute(eAttribute, wsValue.AsStringView(), true);
  if (eAttribute != XFA_Attribute::Use ||
      GetXFAObject()->GetElementType() != XFA_Element::Desc) {
    return;
  }

  CXFA_Node* pTemplateNode =
      ToNode(GetDocument()->GetXFAObject(XFA_HASHCODE_Template));
  CXFA_Subform* pSubForm =
      pTemplateNode->GetFirstChildByClass<CXFA_Subform>(XFA_Element::Subform);
  CXFA_Proto* pProtoRoot =
      pSubForm ? pSubForm->GetFirstChildByClass<CXFA_Proto>(XFA_Element::Proto)
               : nullptr;

  WideString wsID;
  WideString wsSOM;
  if (!wsValue.IsEmpty()) {
    if (wsValue[0] == '#')
      wsID = wsValue.Mid(1, wsValue.GetLength() - 1);
    else
      wsSOM = std::move(wsValue);
  }

  CXFA_Node* pProtoNode = nullptr;
  if (!wsSOM.IsEmpty()) {
    XFA_RESOLVENODE_RS resolveNodeRS;
    bool iRet = GetDocument()->GetScriptContext()->ResolveObjects(
        pProtoRoot, wsSOM.AsStringView(), &resolveNodeRS,
        XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Attributes |
            XFA_RESOLVENODE_Properties | XFA_RESOLVENODE_Parent |
            XFA_RESOLVENODE_Siblings,
        nullptr);
    if (iRet && resolveNodeRS.objects.front()->IsNode())
      pProtoNode = resolveNodeRS.objects.front()->AsNode();

  } else if (!wsID.IsEmpty()) {
    pProtoNode = GetDocument()->GetNodeByID(pProtoRoot, wsID.AsStringView());
  }
  if (!pProtoNode)
    return;

  CXFA_Node* pHeadChild = ToNode(GetXFAObject())->GetFirstChild();
  while (pHeadChild) {
    CXFA_Node* pSibling = pHeadChild->GetNextSibling();
    ToNode(GetXFAObject())->RemoveChild(pHeadChild, true);
    pHeadChild = pSibling;
  }

  CXFA_Node* pProtoForm = pProtoNode->CloneTemplateToForm(true);
  pHeadChild = pProtoForm->GetFirstChild();
  while (pHeadChild) {
    CXFA_Node* pSibling = pHeadChild->GetNextSibling();
    pProtoForm->RemoveChild(pHeadChild, true);
    ToNode(GetXFAObject())->InsertChild(pHeadChild, nullptr);
    pHeadChild = pSibling;
  }
  GetDocument()->FreeOwnedNode(pProtoForm);
  pProtoForm = nullptr;
}

void CJX_Object::ScriptAttributeBool(CFXJSE_Value* pValue,
                                     bool bSetting,
                                     XFA_Attribute eAttribute) {
  if (bSetting) {
    SetBoolean(eAttribute, pValue->ToBoolean(), true);
    return;
  }
  pValue->SetString(GetBoolean(eAttribute) ? "1" : "0");
}

void CJX_Object::ScriptAttributeInteger(CFXJSE_Value* pValue,
                                        bool bSetting,
                                        XFA_Attribute eAttribute) {
  if (bSetting) {
    SetInteger(eAttribute, pValue->ToInteger(), true);
    return;
  }
  pValue->SetInteger(GetInteger(eAttribute));
}

void CJX_Object::ScriptSomFontColor(CFXJSE_Value* pValue,
                                    bool bSetting,
                                    XFA_Attribute eAttribute) {
  CXFA_Font* font = ToNode(object_.Get())->GetOrCreateFontIfPossible();
  if (!font)
    return;

  if (bSetting) {
    int32_t r;
    int32_t g;
    int32_t b;
    std::tie(r, g, b) = StrToRGB(pValue->ToWideString());
    FX_ARGB color = ArgbEncode(0xff, r, g, b);
    font->SetColor(color);
    return;
  }

  int32_t a;
  int32_t r;
  int32_t g;
  int32_t b;
  std::tie(a, r, g, b) = ArgbDecode(font->GetColor());
  pValue->SetString(ByteString::Format("%d,%d,%d", r, g, b).AsStringView());
}

void CJX_Object::ScriptSomFillColor(CFXJSE_Value* pValue,
                                    bool bSetting,
                                    XFA_Attribute eAttribute) {
  CXFA_Border* border = ToNode(object_.Get())->GetOrCreateBorderIfPossible();
  CXFA_Fill* borderfill = border->GetOrCreateFillIfPossible();
  if (!borderfill)
    return;

  if (bSetting) {
    int32_t r;
    int32_t g;
    int32_t b;
    std::tie(r, g, b) = StrToRGB(pValue->ToWideString());
    FX_ARGB color = ArgbEncode(0xff, r, g, b);
    borderfill->SetColor(color);
    return;
  }

  FX_ARGB color = borderfill->GetColor(false);
  int32_t a;
  int32_t r;
  int32_t g;
  int32_t b;
  std::tie(a, r, g, b) = ArgbDecode(color);
  pValue->SetString(
      WideString::Format(L"%d,%d,%d", r, g, b).ToUTF8().AsStringView());
}

void CJX_Object::ScriptSomBorderColor(CFXJSE_Value* pValue,
                                      bool bSetting,
                                      XFA_Attribute eAttribute) {
  CXFA_Border* border = ToNode(object_.Get())->GetOrCreateBorderIfPossible();
  int32_t iSize = border->CountEdges();
  if (bSetting) {
    int32_t r = 0;
    int32_t g = 0;
    int32_t b = 0;
    std::tie(r, g, b) = StrToRGB(pValue->ToWideString());
    FX_ARGB rgb = ArgbEncode(100, r, g, b);
    for (int32_t i = 0; i < iSize; ++i) {
      CXFA_Edge* edge = border->GetEdgeIfExists(i);
      if (edge)
        edge->SetColor(rgb);
    }

    return;
  }

  CXFA_Edge* edge = border->GetEdgeIfExists(0);
  FX_ARGB color = edge ? edge->GetColor() : CXFA_Edge::kDefaultColor;
  int32_t a;
  int32_t r;
  int32_t g;
  int32_t b;
  std::tie(a, r, g, b) = ArgbDecode(color);
  pValue->SetString(
      WideString::Format(L"%d,%d,%d", r, g, b).ToUTF8().AsStringView());
}

void CJX_Object::ScriptSomBorderWidth(CFXJSE_Value* pValue,
                                      bool bSetting,
                                      XFA_Attribute eAttribute) {
  CXFA_Border* border = ToNode(object_.Get())->GetOrCreateBorderIfPossible();
  if (bSetting) {
    CXFA_Edge* edge = border->GetEdgeIfExists(0);
    CXFA_Measurement thickness =
        edge ? edge->GetMSThickness() : CXFA_Measurement(0.5, XFA_Unit::Pt);
    pValue->SetString(thickness.ToString().ToUTF8().AsStringView());
    return;
  }

  if (pValue->IsEmpty())
    return;

  WideString wsThickness = pValue->ToWideString();
  for (int32_t i = 0; i < border->CountEdges(); ++i) {
    CXFA_Edge* edge = border->GetEdgeIfExists(i);
    if (edge)
      edge->SetMSThickness(CXFA_Measurement(wsThickness.AsStringView()));
  }
}

void CJX_Object::ScriptSomMessage(CFXJSE_Value* pValue,
                                  bool bSetting,
                                  XFA_SOM_MESSAGETYPE iMessageType) {
  bool bNew = false;
  CXFA_Validate* validate = ToNode(object_.Get())->GetValidateIfExists();
  if (!validate) {
    validate = ToNode(object_.Get())->GetOrCreateValidateIfPossible();
    bNew = true;
  }

  if (bSetting) {
    if (validate) {
      switch (iMessageType) {
        case XFA_SOM_ValidationMessage:
          validate->SetScriptMessageText(pValue->ToWideString());
          break;
        case XFA_SOM_FormatMessage:
          validate->SetFormatMessageText(pValue->ToWideString());
          break;
        case XFA_SOM_MandatoryMessage:
          validate->SetNullMessageText(pValue->ToWideString());
          break;
        default:
          break;
      }
    }

    if (!bNew) {
      CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
      if (!pNotify)
        return;

      pNotify->AddCalcValidate(ToNode(GetXFAObject()));
    }
    return;
  }

  if (!validate) {
    // TODO(dsinclair): Better error message?
    ThrowInvalidPropertyException();
    return;
  }

  WideString wsMessage;
  switch (iMessageType) {
    case XFA_SOM_ValidationMessage:
      wsMessage = validate->GetScriptMessageText();
      break;
    case XFA_SOM_FormatMessage:
      wsMessage = validate->GetFormatMessageText();
      break;
    case XFA_SOM_MandatoryMessage:
      wsMessage = validate->GetNullMessageText();
      break;
    default:
      break;
  }
  pValue->SetString(wsMessage.ToUTF8().AsStringView());
}

void CJX_Object::ScriptSomValidationMessage(CFXJSE_Value* pValue,
                                            bool bSetting,
                                            XFA_Attribute eAttribute) {
  ScriptSomMessage(pValue, bSetting, XFA_SOM_ValidationMessage);
}

void CJX_Object::ScriptSomMandatoryMessage(CFXJSE_Value* pValue,
                                           bool bSetting,
                                           XFA_Attribute eAttribute) {
  ScriptSomMessage(pValue, bSetting, XFA_SOM_MandatoryMessage);
}

void CJX_Object::ScriptFieldLength(CFXJSE_Value* pValue,
                                   bool bSetting,
                                   XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  CXFA_Node* node = ToNode(object_.Get());
  if (!node->IsWidgetReady()) {
    pValue->SetInteger(0);
    return;
  }
  pValue->SetInteger(node->CountChoiceListItems(true));
}

void CJX_Object::ScriptSomDefaultValue(CFXJSE_Value* pValue,
                                       bool bSetting,
                                       XFA_Attribute /* unused */) {
  XFA_Element eType = ToNode(GetXFAObject())->GetElementType();

  // TODO(dsinclair): This should look through the properties on the node to see
  // if defaultValue is defined and, if so, call that one. Just have to make
  // sure that those defaultValue calls don't call back to this one ....
  if (eType == XFA_Element::Field) {
    static_cast<CJX_Field*>(this)->defaultValue(pValue, bSetting,
                                                XFA_Attribute::Unknown);
    return;
  }
  if (eType == XFA_Element::Draw) {
    static_cast<CJX_Draw*>(this)->defaultValue(pValue, bSetting,
                                               XFA_Attribute::Unknown);
    return;
  }
  if (eType == XFA_Element::Boolean) {
    static_cast<CJX_Boolean*>(this)->defaultValue(pValue, bSetting,
                                                  XFA_Attribute::Unknown);
    return;
  }

  if (bSetting) {
    WideString wsNewValue;
    if (pValue && !(pValue->IsNull() || pValue->IsUndefined()))
      wsNewValue = pValue->ToWideString();

    WideString wsFormatValue(wsNewValue);
    CXFA_Node* pContainerNode = nullptr;
    if (ToNode(GetXFAObject())->GetPacketType() == XFA_PacketType::Datasets) {
      WideString wsPicture;
      for (auto* pFormNode : *(ToNode(GetXFAObject())->GetBindItems())) {
        if (!pFormNode || pFormNode->HasRemovedChildren())
          continue;

        pContainerNode = pFormNode->GetContainerNode();
        if (pContainerNode) {
          wsPicture =
              pContainerNode->GetPictureContent(XFA_VALUEPICTURE_DataBind);
        }
        if (!wsPicture.IsEmpty())
          break;

        pContainerNode = nullptr;
      }
    } else if (ToNode(GetXFAObject())->GetPacketType() ==
               XFA_PacketType::Form) {
      pContainerNode = ToNode(GetXFAObject())->GetContainerNode();
    }

    if (pContainerNode)
      wsFormatValue = pContainerNode->GetFormatDataValue(wsNewValue);

    SetContent(wsNewValue, wsFormatValue, true, true, true);
    return;
  }

  WideString content = GetContent(true);
  if (content.IsEmpty() && eType != XFA_Element::Text &&
      eType != XFA_Element::SubmitUrl) {
    pValue->SetNull();
  } else if (eType == XFA_Element::Integer) {
    pValue->SetInteger(FXSYS_wtoi(content.c_str()));
  } else if (eType == XFA_Element::Float || eType == XFA_Element::Decimal) {
    CFGAS_Decimal decimal(content.AsStringView());
    pValue->SetFloat((float)(double)decimal);
  } else {
    pValue->SetString(content.ToUTF8().AsStringView());
  }
}

void CJX_Object::ScriptSomDefaultValue_Read(CFXJSE_Value* pValue,
                                            bool bSetting,
                                            XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  WideString content = GetContent(true);
  if (content.IsEmpty()) {
    pValue->SetNull();
    return;
  }
  pValue->SetString(content.ToUTF8().AsStringView());
}

void CJX_Object::ScriptSomDataNode(CFXJSE_Value* pValue,
                                   bool bSetting,
                                   XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  CXFA_Node* pDataNode = ToNode(GetXFAObject())->GetBindData();
  if (!pDataNode) {
    pValue->SetNull();
    return;
  }

  pValue->Assign(
      GetDocument()->GetScriptContext()->GetJSValueFromMap(pDataNode));
}

void CJX_Object::ScriptSomMandatory(CFXJSE_Value* pValue,
                                    bool bSetting,
                                    XFA_Attribute eAttribute) {
  CXFA_Validate* validate =
      ToNode(object_.Get())->GetOrCreateValidateIfPossible();
  if (!validate)
    return;

  if (bSetting) {
    validate->SetNullTest(pValue->ToWideString());
    return;
  }

  pValue->SetString(XFA_AttributeValueToName(validate->GetNullTest()));
}

void CJX_Object::ScriptSomInstanceIndex(CFXJSE_Value* pValue,
                                        bool bSetting,
                                        XFA_Attribute eAttribute) {
  if (!bSetting) {
    pValue->SetInteger(Subform_and_SubformSet_InstanceIndex());
    return;
  }

  int32_t iTo = pValue->ToInteger();
  int32_t iFrom = Subform_and_SubformSet_InstanceIndex();
  CXFA_Node* pManagerNode = nullptr;
  for (CXFA_Node* pNode = ToNode(GetXFAObject())->GetPrevSibling(); pNode;
       pNode = pNode->GetPrevSibling()) {
    if (pNode->GetElementType() == XFA_Element::InstanceManager) {
      pManagerNode = pNode;
      break;
    }
  }
  if (!pManagerNode)
    return;

  auto* mgr = static_cast<CJX_InstanceManager*>(pManagerNode->JSObject());
  mgr->MoveInstance(iTo, iFrom);
  CXFA_FFNotify* pNotify = GetDocument()->GetNotify();
  if (!pNotify)
    return;

  CXFA_Node* pToInstance = pManagerNode->GetItemIfExists(iTo);
  if (pToInstance && pToInstance->GetElementType() == XFA_Element::Subform) {
    pNotify->RunSubformIndexChange(pToInstance);
  }

  CXFA_Node* pFromInstance = pManagerNode->GetItemIfExists(iFrom);
  if (pFromInstance &&
      pFromInstance->GetElementType() == XFA_Element::Subform) {
    pNotify->RunSubformIndexChange(pFromInstance);
  }
}

void CJX_Object::ScriptSubformInstanceManager(CFXJSE_Value* pValue,
                                              bool bSetting,
                                              XFA_Attribute eAttribute) {
  if (bSetting) {
    ThrowInvalidPropertyException();
    return;
  }

  WideString wsName = GetCData(XFA_Attribute::Name);
  CXFA_Node* pInstanceMgr = nullptr;
  for (CXFA_Node* pNode = ToNode(GetXFAObject())->GetPrevSibling(); pNode;
       pNode = pNode->GetPrevSibling()) {
    if (pNode->GetElementType() == XFA_Element::InstanceManager) {
      WideString wsInstMgrName =
          pNode->JSObject()->GetCData(XFA_Attribute::Name);
      if (wsInstMgrName.GetLength() >= 1 && wsInstMgrName[0] == '_' &&
          wsInstMgrName.Right(wsInstMgrName.GetLength() - 1) == wsName) {
        pInstanceMgr = pNode;
      }
      break;
    }
  }
  if (!pInstanceMgr) {
    pValue->SetNull();
    return;
  }

  pValue->Assign(
      GetDocument()->GetScriptContext()->GetJSValueFromMap(pInstanceMgr));
}

void CJX_Object::ScriptSubmitFormatMode(CFXJSE_Value* pValue,
                                        bool bSetting,
                                        XFA_Attribute eAttribute) {}

void CJX_Object::ScriptFormChecksumS(CFXJSE_Value* pValue,
                                     bool bSetting,
                                     XFA_Attribute eAttribute) {
  if (bSetting) {
    SetAttribute(XFA_Attribute::Checksum, pValue->ToWideString().AsStringView(),
                 false);
    return;
  }

  Optional<WideString> checksum = TryAttribute(XFA_Attribute::Checksum, false);
  pValue->SetString(checksum ? checksum->ToUTF8().AsStringView() : "");
}

void CJX_Object::ScriptExclGroupErrorText(CFXJSE_Value* pValue,
                                          bool bSetting,
                                          XFA_Attribute eAttribute) {
  if (bSetting)
    ThrowInvalidPropertyException();
}
