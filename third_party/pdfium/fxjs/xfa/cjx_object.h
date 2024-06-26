// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_OBJECT_H_
#define FXJS_XFA_CJX_OBJECT_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "core/fxcrt/widestring.h"
#include "fxjs/xfa/jse_define.h"
#include "third_party/base/optional.h"
#include "third_party/base/span.h"
#include "xfa/fxfa/fxfa_basic.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"

class CFX_XMLElement;
class CFXJSE_Value;
class CFX_V8;
class CJX_Object;
class CXFA_CalcData;
class CXFA_Document;
class CXFA_LayoutItem;
class CXFA_Node;
class CXFA_Object;
struct XFA_MAPMODULEDATA;

// typedef CJS_Result (*CJX_MethodCall)(
//     CJX_Object* obj,
//     CFX_V8* runtime,
//     const std::vector<v8::Local<v8::Value>>& params);
// struct CJX_MethodSpec {
//   const char* pName;
//   CJX_MethodCall pMethodCall;
// };

typedef void (*PD_CALLBACK_FREEDATA)(void* pData);
typedef void (*PD_CALLBACK_DUPLICATEDATA)(void*& pData);

struct XFA_MAPDATABLOCKCALLBACKINFO {
  PD_CALLBACK_FREEDATA pFree;
  PD_CALLBACK_DUPLICATEDATA pCopy;
};

enum XFA_SOM_MESSAGETYPE {
  XFA_SOM_ValidationMessage,
  XFA_SOM_FormatMessage,
  XFA_SOM_MandatoryMessage
};

class CJX_Object {
 public:
  // Similar, but not exactly equal to XFA_Element enum.
  // TODO(tsepez): unify with XFA_Element.
  enum class TypeTag {
    Boolean,
    Comb,
    Container,
    DataValue,
    DataWindow,
    Date,
    DateTime,
    Decimal,
    Delta,
    Desc,
    Draw,
    Encrypt,
    EventPseudoModel,
    ExclGroup,
    ExData,
    Extras,
    Field,
    Float,
    Form,
    Handler,
    HostPseudoModel,
    Image,
    InstanceManager,
    Integer,
    LayoutPseudoModel,
    List,
    LogPseudoModel,
    Manifest,
    Model,
    Node,
    Object,
    Occur,
    Packet,
    Picture,
    Script,
    SignaturePesudoModel,
    Source,
    Subform,
    SubformSet,
    Template,
    Text,
    TextNode,
    Time,
    Tree,
    TreeList,
    Value,
    WsdlConnection,
    Xfa,
  };

  explicit CJX_Object(CXFA_Object* obj);
  virtual ~CJX_Object();

  virtual bool DynamicTypeIs(TypeTag eType) const;

  JSE_PROP(className);

  CXFA_Document* GetDocument() const;
  CXFA_Object* GetXFAObject() const { return object_.Get(); }

  void SetCalcRecursionCount(size_t count) { calc_recursion_count_ = count; }
  size_t GetCalcRecursionCount() const { return calc_recursion_count_; }

  void SetLayoutItem(CXFA_LayoutItem* item) { layout_item_ = item; }
  CXFA_LayoutItem* GetLayoutItem() const { return layout_item_; }

  bool HasMethod(const WideString& func) const;
  //CJS_Result RunMethod(const WideString& func, const std::vector<v8::Local<v8::Value>>& params);

  bool HasAttribute(XFA_Attribute eAttr);
  void SetAttribute(XFA_Attribute eAttr, WideStringView wsValue, bool bNotify);
  void SetAttribute(WideStringView wsAttr,
                    WideStringView wsValue,
                    bool bNotify);
  void RemoveAttribute(WideStringView wsAttr);
  WideString GetAttribute(WideStringView attr);
  WideString GetAttribute(XFA_Attribute attr);
  Optional<WideString> TryAttribute(WideStringView wsAttr, bool bUseDefault);
  Optional<WideString> TryAttribute(XFA_Attribute eAttr, bool bUseDefault);

  Optional<WideString> TryContent(bool bScriptModify, bool bProto);
  void SetContent(const WideString& wsContent,
                  const WideString& wsXMLValue,
                  bool bNotify,
                  bool bScriptModify,
                  bool bSyncData);
  WideString GetContent(bool bScriptModify);

  template <typename T>
  T* GetProperty(int32_t index, XFA_Element eType) const {
    CXFA_Node* node;
    int32_t count;
    std::tie(node, count) = GetPropertyInternal(index, eType);
    return static_cast<T*>(node);
  }
  template <typename T>
  T* GetOrCreateProperty(int32_t index, XFA_Element eType) {
    return static_cast<T*>(GetOrCreatePropertyInternal(index, eType));
  }

  void SetAttributeValue(const WideString& wsValue,
                         const WideString& wsXMLValue,
                         bool bNotify,
                         bool bScriptModify);

  // Not actual properties, but invoked as property handlers to cover
  // a broad range of underlying properties.
  JSE_PROP(ScriptAttributeString);
  JSE_PROP(ScriptAttributeBool);
  JSE_PROP(ScriptAttributeInteger);
  JSE_PROP(ScriptSomFontColor);
  JSE_PROP(ScriptSomFillColor);
  JSE_PROP(ScriptSomBorderColor);
  JSE_PROP(ScriptSomBorderWidth);
  JSE_PROP(ScriptSomValidationMessage);
  JSE_PROP(ScriptSomMandatoryMessage);
  JSE_PROP(ScriptFieldLength);
  JSE_PROP(ScriptSomDefaultValue);
  JSE_PROP(ScriptSomDefaultValue_Read);
  JSE_PROP(ScriptSomDataNode);
  JSE_PROP(ScriptSomMandatory);
  JSE_PROP(ScriptSomInstanceIndex);
  JSE_PROP(ScriptSubformInstanceManager);
  JSE_PROP(ScriptSubmitFormatMode);
  JSE_PROP(ScriptFormChecksumS);
  JSE_PROP(ScriptExclGroupErrorText);

  void ScriptSomMessage(CFXJSE_Value* pValue,
                        bool bSetting,
                        XFA_SOM_MESSAGETYPE iMessageType);

  Optional<WideString> TryNamespace();

  Optional<int32_t> TryInteger(XFA_Attribute eAttr, bool bUseDefault);
  void SetInteger(XFA_Attribute eAttr, int32_t iValue, bool bNotify);
  int32_t GetInteger(XFA_Attribute eAttr);

  Optional<WideString> TryCData(XFA_Attribute eAttr, bool bUseDefault);
  void SetCData(XFA_Attribute eAttr,
                const WideString& wsValue,
                bool bNotify,
                bool bScriptModify);
  WideString GetCData(XFA_Attribute eAttr);

  Optional<XFA_AttributeValue> TryEnum(XFA_Attribute eAttr,
                                       bool bUseDefault) const;
  void SetEnum(XFA_Attribute eAttr, XFA_AttributeValue eValue, bool bNotify);
  XFA_AttributeValue GetEnum(XFA_Attribute eAttr) const;

  Optional<bool> TryBoolean(XFA_Attribute eAttr, bool bUseDefault);
  void SetBoolean(XFA_Attribute eAttr, bool bValue, bool bNotify);
  bool GetBoolean(XFA_Attribute eAttr);

  Optional<CXFA_Measurement> TryMeasure(XFA_Attribute eAttr,
                                        bool bUseDefault) const;
  Optional<float> TryMeasureAsFloat(XFA_Attribute attr) const;
  void SetMeasure(XFA_Attribute eAttr, CXFA_Measurement mValue, bool bNotify);
  CXFA_Measurement GetMeasure(XFA_Attribute eAttr) const;

  void MergeAllData(CXFA_Object* pDstModule);

  void SetCalcData(std::unique_ptr<CXFA_CalcData> data);
  CXFA_CalcData* GetCalcData() const { return calc_data_.get(); }
  std::unique_ptr<CXFA_CalcData> ReleaseCalcData();

  int32_t InstanceManager_SetInstances(int32_t iDesired);
  int32_t InstanceManager_MoveInstance(int32_t iTo, int32_t iFrom);

  void ThrowInvalidPropertyException() const;
  void ThrowArgumentMismatchException() const;
  void ThrowIndexOutOfBoundsException() const;
  void ThrowParamCountMismatchException(const WideString& method) const;
  void ThrowTooManyOccurancesException(const WideString& obj) const;

 protected:
  //void DefineMethods(pdfium::span<const CJX_MethodSpec> methods);
  void MoveBufferMapData(CXFA_Object* pSrcModule, CXFA_Object* pDstModule);
  void SetMapModuleString(void* pKey, WideStringView wsValue);
  void ThrowException(const WideString& str) const;

 private:
  using Type__ = CJX_Object;
  static const TypeTag static_type__ = TypeTag::Object;

  std::pair<CXFA_Node*, int32_t> GetPropertyInternal(int32_t index,
                                                     XFA_Element eType) const;
  CXFA_Node* GetOrCreatePropertyInternal(int32_t index, XFA_Element eType);

  void OnChanged(XFA_Attribute eAttr, bool bNotify, bool bScriptModify);
  void OnChanging(XFA_Attribute eAttr, bool bNotify);
  void SetUserData(void* pKey,
                   void* pData,
                   const XFA_MAPDATABLOCKCALLBACKINFO* pCallbackInfo);

  // Returns a pointer to the XML node that needs to be updated with the new
  // attribute value. |nullptr| if no update is needed.
  CFX_XMLElement* SetValue(XFA_Attribute eAttr, void* pValue, bool bNotify);
  int32_t Subform_and_SubformSet_InstanceIndex();

  XFA_MAPMODULEDATA* CreateMapModuleData();
  XFA_MAPMODULEDATA* GetMapModuleData() const;
  void SetMapModuleValue(void* pKey, void* pValue);
  Optional<void*> GetMapModuleValue(void* pKey) const;
  Optional<WideString> GetMapModuleString(void* pKey);
  void SetMapModuleBuffer(void* pKey,
                          void* pValue,
                          int32_t iBytes,
                          const XFA_MAPDATABLOCKCALLBACKINFO* pCallbackInfo);
  bool GetMapModuleBuffer(void* pKey, void** pValue, int32_t* pBytes) const;
  bool HasMapModuleKey(void* pKey);
  void ClearMapModuleBuffer();
  void RemoveMapModuleKey(void* pKey);
  void MoveBufferMapData(CXFA_Object* pDstModule);

  UnownedPtr<CXFA_Object> object_;
  // This is an UnownedPtr but, due to lifetime issues, can't be marked as such
  // at this point. The CJX_Node is freed by its parent CXFA_Node. The CXFA_Node
  // will be freed during CXFA_NodeHolder destruction (CXFA_Document
  // destruction as the only implementation). This will happen after the
  // CXFA_LayoutProcessor is destroyed in the CXFA_Document, leaving this as a
  // bad unowned ptr.
  CXFA_LayoutItem* layout_item_ = nullptr;
  std::unique_ptr<XFA_MAPMODULEDATA> map_module_data_;
  std::unique_ptr<CXFA_CalcData> calc_data_;
  //std::map<ByteString, CJX_MethodCall> method_specs_;
  size_t calc_recursion_count_ = 0;
};

typedef void (*XFA_ATTRIBUTE_CALLBACK)(CJX_Object* pNode,
                                       CFXJSE_Value* pValue,
                                       bool bSetting,
                                       XFA_Attribute eAttribute);

#endif  // FXJS_XFA_CJX_OBJECT_H_
