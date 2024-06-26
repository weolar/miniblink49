// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_NODE_H_
#define XFA_FXFA_PARSER_CXFA_NODE_H_

#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxge/fx_dib.h"
#include "third_party/base/optional.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/parser/cxfa_object.h"

class CFGAS_GEFont;
class CFX_DIBitmap;
class CFX_XMLNode;
class CXFA_Bind;
class CXFA_Border;
class CXFA_Calculate;
class CXFA_Caption;
class CXFA_Event;
class CXFA_EventParam;
class CXFA_FFDoc;
class CXFA_FFDocView;
class CXFA_FFWidget;
class CXFA_Font;
class CXFA_Keep;
class CXFA_Margin;
class CXFA_Occur;
class CXFA_Para;
class CXFA_Script;
class CXFA_TextLayout;
class CXFA_Ui;
class CXFA_Validate;
class CXFA_Value;
class CXFA_WidgetLayoutData;
class LocaleIface;

#define XFA_NODEFILTER_Children 0x01
#define XFA_NODEFILTER_Properties 0x02
#define XFA_NODEFILTER_OneOfProperty 0x04

enum XFA_CHECKSTATE {
  XFA_CHECKSTATE_On = 0,
  XFA_CHECKSTATE_Off = 1,
  XFA_CHECKSTATE_Neutral = 2,
};

enum XFA_VALUEPICTURE {
  XFA_VALUEPICTURE_Raw = 0,
  XFA_VALUEPICTURE_Display,
  XFA_VALUEPICTURE_Edit,
  XFA_VALUEPICTURE_DataBind,
};

enum XFA_NodeFlag {
  XFA_NodeFlag_None = 0,
  XFA_NodeFlag_Initialized = 1 << 0,
  XFA_NodeFlag_HasRemovedChildren = 1 << 1,
  XFA_NodeFlag_NeedsInitApp = 1 << 2,
  XFA_NodeFlag_BindFormItems = 1 << 3,
  XFA_NodeFlag_UserInteractive = 1 << 4,
  XFA_NodeFlag_UnusedNode = 1 << 5,
  XFA_NodeFlag_LayoutGeneratedNode = 1 << 6
};

class CXFA_Node : public CXFA_Object {
 public:
  struct PropertyData {
    XFA_Element property;
    uint8_t occurance_count;
    uint8_t flags;
  };

  struct AttributeData {
    XFA_Attribute attribute;
    XFA_AttributeType type;
    void* default_value;
  };

  static std::unique_ptr<CXFA_Node> Create(CXFA_Document* doc,
                                           XFA_Element element,
                                           XFA_PacketType packet);

  ~CXFA_Node() override;

  bool HasProperty(XFA_Element property) const;
  bool HasPropertyFlags(XFA_Element property, uint8_t flags) const;
  uint8_t PropertyOccuranceCount(XFA_Element property) const;

  void SendAttributeChangeMessage(XFA_Attribute eAttribute, bool bScriptModify);

  bool HasAttribute(XFA_Attribute attr) const;
  XFA_Attribute GetAttribute(size_t i) const;
  XFA_AttributeType GetAttributeType(XFA_Attribute type) const;

  XFA_PacketType GetPacketType() const { return m_ePacket; }

  void SetFlag(uint32_t dwFlag);
  void SetFlagAndNotify(uint32_t dwFlag);
  void ClearFlag(uint32_t dwFlag);

  CXFA_Node* CreateInstanceIfPossible(bool bDataMerge);
  int32_t GetCount();
  CXFA_Node* GetItemIfExists(int32_t iIndex);
  void RemoveItem(CXFA_Node* pRemoveInstance, bool bRemoveDataBinding);
  void InsertItem(CXFA_Node* pNewInstance,
                  int32_t iPos,
                  int32_t iCount,
                  bool bMoveDataBindingNodes);

  bool IsInitialized() const { return HasFlag(XFA_NodeFlag_Initialized); }
  bool IsUserInteractive() const {
    return HasFlag(XFA_NodeFlag_UserInteractive);
  }
  bool IsUnusedNode() const { return HasFlag(XFA_NodeFlag_UnusedNode); }
  bool IsLayoutGeneratedNode() const {
    return HasFlag(XFA_NodeFlag_LayoutGeneratedNode);
  }

  bool PresenceRequiresSpace() const;

  void SetBindingNode(CXFA_Node* node) {
    binding_nodes_.clear();
    if (node)
      binding_nodes_.emplace_back(node);
  }

  bool HasRemovedChildren() const {
    return HasFlag(XFA_NodeFlag_HasRemovedChildren);
  }

  bool IsAttributeInXML();
  bool IsFormContainer() const {
    return m_ePacket == XFA_PacketType::Form && IsContainerNode();
  }

  void SetXMLMappingNode(CFX_XMLNode* node) { xml_node_ = node; }
  CFX_XMLNode* GetXMLMappingNode() const { return xml_node_.Get(); }
  CFX_XMLNode* CreateXMLMappingNode();
  bool IsNeedSavingXMLNode();

  void SetToXML(const WideString& value);

  uint32_t GetNameHash() const { return m_dwNameHash; }
  bool IsUnnamed() const { return m_dwNameHash == 0; }
  CXFA_Node* GetModelNode();
  void UpdateNameHash();

  size_t CountChildren(XFA_Element eType, bool bOnlyChild);

  template <typename T>
  T* GetChild(size_t index, XFA_Element eType, bool bOnlyChild) {
    return static_cast<T*>(GetChildInternal(index, eType, bOnlyChild));
  }

  void InsertChild(int32_t index, CXFA_Node* pNode);
  void InsertChild(CXFA_Node* pNode, CXFA_Node* pBeforeNode);
  void RemoveChild(CXFA_Node* pNode, bool bNotify);

  CXFA_Node* Clone(bool bRecursive);

  CXFA_Node* GetNextSibling() const { return next_sibling_; }
  CXFA_Node* GetPrevSibling() const { return prev_sibling_; }
  CXFA_Node* GetFirstChild() const { return first_child_; }
  CXFA_Node* GetLastChild() const { return last_child_; }
  CXFA_Node* GetParent() const { return parent_; }

  CXFA_Node* GetNextContainerSibling() const;
  CXFA_Node* GetPrevContainerSibling() const;
  CXFA_Node* GetFirstContainerChild() const;
  CXFA_Node* GetContainerParent() const;

  std::vector<CXFA_Node*> GetNodeList(uint32_t dwTypeFilter,
                                      XFA_Element eTypeFilter);
  CXFA_Node* CreateSamePacketNode(XFA_Element eType);
  CXFA_Node* CloneTemplateToForm(bool bRecursive);
  CXFA_Node* GetTemplateNodeIfExists() const;
  void SetTemplateNode(CXFA_Node* pTemplateNode);
  CXFA_Node* GetDataDescriptionNode();
  void SetDataDescriptionNode(CXFA_Node* pDataDescriptionNode);
  CXFA_Node* GetBindData();
  std::vector<CXFA_Node*>* GetBindItems() { return &binding_nodes_; }
  int32_t AddBindItem(CXFA_Node* pFormNode);
  int32_t RemoveBindItem(CXFA_Node* pFormNode);
  bool HasBindItem();
  CXFA_Node* GetContainerNode();
  LocaleIface* GetLocale();
  Optional<WideString> GetLocaleName();
  XFA_AttributeValue GetIntact();

  CXFA_Node* GetFirstChildByName(WideStringView wsNodeName) const;
  CXFA_Node* GetFirstChildByName(uint32_t dwNodeNameHash) const;
  template <typename T>
  T* GetFirstChildByClass(XFA_Element eType) const {
    return static_cast<T*>(GetFirstChildByClassInternal(eType));
  }
  CXFA_Node* GetNextSameNameSibling(uint32_t dwNodeNameHash) const;
  template <typename T>
  T* GetNextSameNameSibling(WideStringView wsNodeName) const {
    return static_cast<T*>(GetNextSameNameSiblingInternal(wsNodeName));
  }
  template <typename T>
  T* GetNextSameClassSibling(XFA_Element eType) const {
    return static_cast<T*>(GetNextSameClassSiblingInternal(eType));
  }

  CXFA_Node* GetInstanceMgrOfSubform();

  Optional<bool> GetDefaultBoolean(XFA_Attribute attr) const;
  Optional<int32_t> GetDefaultInteger(XFA_Attribute attr) const;
  Optional<CXFA_Measurement> GetDefaultMeasurement(XFA_Attribute attr) const;
  Optional<WideString> GetDefaultCData(XFA_Attribute attr) const;
  Optional<XFA_AttributeValue> GetDefaultEnum(XFA_Attribute attr) const;

  bool IsOpenAccess();

  CXFA_Occur* GetOccurIfExists();
  CXFA_Border* GetBorderIfExists() const;
  CXFA_Border* GetOrCreateBorderIfPossible();
  CXFA_Caption* GetCaptionIfExists() const;
  CXFA_Font* GetFontIfExists() const;
  CXFA_Font* GetOrCreateFontIfPossible();

  float GetFontSize() const;
  FX_ARGB GetTextColor() const;
  float GetLineHeight() const;

  CXFA_Margin* GetMarginIfExists() const;
  CXFA_Para* GetParaIfExists() const;
  CXFA_Calculate* GetCalculateIfExists() const;
  CXFA_Validate* GetValidateIfExists() const;
  CXFA_Validate* GetOrCreateValidateIfPossible();

  CXFA_Value* GetFormValueIfExists() const;
  WideString GetRawValue();

  int32_t GetRotate();
  Optional<float> TryWidth();

  CXFA_Node* GetExclGroupIfExists();

  int32_t ProcessEvent(CXFA_FFDocView* docView,
                       XFA_AttributeValue iActivity,
                       CXFA_EventParam* pEventParam);
  int32_t ProcessCalculate(CXFA_FFDocView* docView);
  int32_t ProcessValidate(CXFA_FFDocView* docView, int32_t iFlags);
  int32_t ExecuteScript(CXFA_FFDocView* docView,
                        CXFA_Script* script,
                        CXFA_EventParam* pEventParam);
  std::pair<int32_t, bool> ExecuteBoolScript(CXFA_FFDocView* docView,
                                             CXFA_Script* script,
                                             CXFA_EventParam* pEventParam);

  CXFA_Node* GetUIChildNode();
  XFA_FFWidgetType GetFFWidgetType();

  CFX_RectF GetUIMargin();
  CXFA_Border* GetUIBorder();

  void SetPreNull(bool val) { m_bPreNull = val; }
  bool IsNull() const { return m_bIsNull; }
  void SetIsNull(bool val) { m_bIsNull = val; }

  void SetWidgetReady() { is_widget_ready_ = true; }
  bool IsWidgetReady() const { return is_widget_ready_; }
  std::vector<CXFA_Event*> GetEventByActivity(XFA_AttributeValue iActivity,
                                              bool bIsFormReady);

  void ResetData();

  CXFA_FFWidget* GetNextWidget(CXFA_FFWidget* pWidget);
  void StartWidgetLayout(CXFA_FFDoc* doc,
                         float* pCalcWidth,
                         float* pCalcHeight);
  bool FindSplitPos(CXFA_FFDocView* docView,
                    int32_t iBlockIndex,
                    float* pCalcHeight);

  bool LoadCaption(CXFA_FFDoc* doc);
  CXFA_TextLayout* GetCaptionTextLayout();
  CXFA_TextLayout* GetTextLayout();

  bool LoadImageImage(CXFA_FFDoc* doc);
  bool LoadImageEditImage(CXFA_FFDoc* doc);
  CFX_Size GetImageDpi() const;
  CFX_Size GetImageEditDpi() const;

  RetainPtr<CFX_DIBitmap> GetImageImage();
  RetainPtr<CFX_DIBitmap> GetImageEditImage();
  void SetImageImage(const RetainPtr<CFX_DIBitmap>& newImage);
  void SetImageEditImage(const RetainPtr<CFX_DIBitmap>& newImage);
  void UpdateUIDisplay(CXFA_FFDocView* docView, CXFA_FFWidget* pExcept);

  RetainPtr<CFGAS_GEFont> GetFDEFont(CXFA_FFDoc* doc);

  bool IsListBox();
  bool IsRadioButton();
  bool IsMultiLine();

  bool HasButtonRollover();
  bool HasButtonDown();

  float GetCheckButtonSize();

  XFA_CHECKSTATE GetCheckState();
  void SetCheckState(XFA_CHECKSTATE eCheckState, bool bNotify);

  CXFA_Node* GetSelectedMember();
  CXFA_Node* SetSelectedMember(WideStringView wsName, bool bNotify);
  void SetSelectedMemberByValue(WideStringView wsValue,
                                bool bNotify,
                                bool bScriptModify,
                                bool bSyncData);

  CXFA_Node* GetExclGroupFirstMember();
  CXFA_Node* GetExclGroupNextMember(CXFA_Node* pNode);

  bool IsChoiceListAllowTextEntry();
  int32_t CountChoiceListItems(bool bSaveValue);
  Optional<WideString> GetChoiceListItem(int32_t nIndex, bool bSaveValue);
  bool IsChoiceListMultiSelect();
  bool IsChoiceListCommitOnSelect();
  std::vector<WideString> GetChoiceListItems(bool bSaveValue);

  int32_t CountSelectedItems();
  int32_t GetSelectedItem(int32_t nIndex);
  std::vector<int32_t> GetSelectedItems();
  std::vector<WideString> GetSelectedItemsValue();
  void SetSelectedItems(const std::vector<int32_t>& iSelArray,
                        bool bNotify,
                        bool bScriptModify,
                        bool bSyncData);
  void InsertItem(const WideString& wsLabel,
                  const WideString& wsValue,
                  bool bNotify);
  bool DeleteItem(int32_t nIndex, bool bNotify, bool bScriptModify);
  void ClearAllSelections();

  bool GetItemState(int32_t nIndex);
  void SetItemState(int32_t nIndex,
                    bool bSelected,
                    bool bNotify,
                    bool bScriptModify,
                    bool bSyncData);

  WideString GetItemValue(WideStringView wsLabel);

  bool IsHorizontalScrollPolicyOff();
  bool IsVerticalScrollPolicyOff();
  Optional<int32_t> GetNumberOfCells();

  bool SetValue(XFA_VALUEPICTURE eValueType, const WideString& wsValue);
  WideString GetValue(XFA_VALUEPICTURE eValueType);

  WideString GetPictureContent(XFA_VALUEPICTURE ePicture);
  WideString GetNormalizeDataValue(const WideString& wsValue);
  WideString GetFormatDataValue(const WideString& wsValue);
  WideString NormalizeNumStr(const WideString& wsValue);

  std::pair<XFA_Element, int32_t> GetMaxChars();
  int32_t GetFracDigits();
  int32_t GetLeadDigits();

  WideString NumericLimit(const WideString& wsValue);

 protected:
  virtual XFA_Element GetValueNodeType() const;
  virtual XFA_FFWidgetType GetDefaultFFWidgetType() const;

  CXFA_Node(CXFA_Document* pDoc,
            XFA_PacketType ePacket,
            uint32_t validPackets,
            XFA_ObjectType oType,
            XFA_Element eType,
            const PropertyData* properties,
            const AttributeData* attributes,
            std::unique_ptr<CJX_Object> js_node);
  CXFA_Node(CXFA_Document* pDoc,
            XFA_PacketType ePacket,
            uint32_t validPackets,
            XFA_ObjectType oType,
            XFA_Element eType,
            const PropertyData* properties,
            const AttributeData* attributes);

 private:
  void ProcessScriptTestValidate(CXFA_FFDocView* docView,
                                 CXFA_Validate* validate,
                                 int32_t iRet,
                                 bool pRetValue,
                                 bool bVersionFlag);
  int32_t ProcessFormatTestValidate(CXFA_FFDocView* docView,
                                    CXFA_Validate* validate,
                                    bool bVersionFlag);
  int32_t ProcessNullTestValidate(CXFA_FFDocView* docView,
                                  CXFA_Validate* validate,
                                  int32_t iFlags,
                                  bool bVersionFlag);
  WideString GetValidateCaptionName(bool bVersionFlag);
  WideString GetValidateMessage(bool bError, bool bVersionFlag);

  bool HasFlag(XFA_NodeFlag dwFlag) const;
  CXFA_Node* Deprecated_GetPrevSibling();
  const PropertyData* GetPropertyData(XFA_Element property) const;
  const AttributeData* GetAttributeData(XFA_Attribute attr) const;
  Optional<XFA_Element> GetFirstPropertyWithFlag(uint8_t flag);
  void OnRemoved(bool bNotify);
  Optional<void*> GetDefaultValue(XFA_Attribute attr,
                                  XFA_AttributeType eType) const;
  CXFA_Node* GetChildInternal(size_t index, XFA_Element eType, bool bOnlyChild);
  CXFA_Node* GetFirstChildByClassInternal(XFA_Element eType) const;
  CXFA_Node* GetNextSameNameSiblingInternal(WideStringView wsNodeName) const;
  CXFA_Node* GetNextSameClassSiblingInternal(XFA_Element eType) const;
  void CalcCaptionSize(CXFA_FFDoc* doc, CFX_SizeF* pszCap);
  bool CalculateFieldAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize);
  bool CalculateWidgetAutoSize(CFX_SizeF* pSize);
  bool CalculateTextEditAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize);
  bool CalculateCheckButtonAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize);
  bool CalculatePushButtonAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize);
  CFX_SizeF CalculateImageSize(float img_width,
                               float img_height,
                               const CFX_Size& dpi);
  bool CalculateImageEditAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize);
  bool CalculateImageAutoSize(CXFA_FFDoc* doc, CFX_SizeF* pSize);
  float CalculateWidgetAutoHeight(float fHeightCalc);
  float CalculateWidgetAutoWidth(float fWidthCalc);
  float GetWidthWithoutMargin(float fWidthCalc) const;
  float GetHeightWithoutMargin(float fHeightCalc) const;
  void CalculateTextContentSize(CXFA_FFDoc* doc, CFX_SizeF* pSize);
  CFX_SizeF CalculateAccWidthAndHeight(CXFA_FFDoc* doc, float fWidth);
  void InitLayoutData();
  void StartTextLayout(CXFA_FFDoc* doc, float* pCalcWidth, float* pCalcHeight);

  void InsertListTextItem(CXFA_Node* pItems,
                          const WideString& wsText,
                          int32_t nIndex);
  WideString GetItemLabel(WideStringView wsValue) const;

  std::pair<XFA_FFWidgetType, CXFA_Ui*> CreateChildUIAndValueNodesIfNeeded();
  void CreateValueNodeIfNeeded(CXFA_Value* value, CXFA_Node* pUIChild);
  CXFA_Node* CreateUINodeIfNeeded(CXFA_Ui* ui, XFA_Element type);
  bool IsValidInPacket(XFA_PacketType packet) const;
  void SetImageEdit(const WideString& wsContentType,
                    const WideString& wsHref,
                    const WideString& wsData);
  CXFA_Node* GetBindingNode() const {
    if (binding_nodes_.empty())
      return nullptr;
    return binding_nodes_[0];
  }
  bool BindsFormItems() const { return HasFlag(XFA_NodeFlag_BindFormItems); }
  bool NeedsInitApp() const { return HasFlag(XFA_NodeFlag_NeedsInitApp); }
  void SyncValue(const WideString& wsValue, bool bNotify);
  CXFA_Value* GetDefaultValueIfExists();
  CXFA_Bind* GetBindIfExists() const;
  Optional<XFA_AttributeValue> GetIntactFromKeep(
      const CXFA_Keep* pKeep,
      XFA_AttributeValue eLayoutType) const;

  Optional<float> TryHeight();
  Optional<float> TryMinWidth();
  Optional<float> TryMinHeight();
  Optional<float> TryMaxWidth();
  Optional<float> TryMaxHeight();
  int32_t ProcessEvent(CXFA_FFDocView* docView,
                       CXFA_Event* event,
                       CXFA_EventParam* pEventParam);

  const PropertyData* const m_Properties;
  const AttributeData* const m_Attributes;
  const uint32_t m_ValidPackets;

  // These members are responsible for building the CXFA_Node tree. Node
  // pointers within the tree (or in objects owned by nodes in the tree)
  // can't be UnownedPtr<> because the cleanup process will remove the nodes
  // in an order that doesn't necessarily match up to the tree structure.
  CXFA_Node* parent_ = nullptr;        // Raw, intra-tree node pointer.
  CXFA_Node* next_sibling_ = nullptr;  // Raw, intra-tree node pointer.
  CXFA_Node* prev_sibling_ = nullptr;  // Raw, intra-tree node pointer.
  CXFA_Node* first_child_ = nullptr;   // Raw, intra-tree node pointer.
  CXFA_Node* last_child_ = nullptr;    // Raw, intra-tree node pointer.

  UnownedPtr<CFX_XMLNode> xml_node_;
  const XFA_PacketType m_ePacket;
  uint8_t m_ExecuteRecursionDepth = 0;
  uint16_t m_uNodeFlags = XFA_NodeFlag_None;
  uint32_t m_dwNameHash = 0;
  CXFA_Node* m_pAuxNode = nullptr;         // Raw, node tree cleanup order.
  std::vector<CXFA_Node*> binding_nodes_;  // Raw, node tree cleanup order.
  bool m_bIsNull = true;
  bool m_bPreNull = true;
  bool is_widget_ready_ = false;
  std::unique_ptr<CXFA_WidgetLayoutData> m_pLayoutData;

  CXFA_Ui* ui_ = nullptr;
  XFA_FFWidgetType ff_widget_type_ = XFA_FFWidgetType::kNone;
};

#endif  // XFA_FXFA_PARSER_CXFA_NODE_H_
