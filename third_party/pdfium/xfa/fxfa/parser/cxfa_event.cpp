// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_event.h"

#include "fxjs/xfa/cjx_event.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_script.h"
#include "xfa/fxfa/parser/cxfa_submit.h"

namespace {

const CXFA_Node::PropertyData kEventPropertyData[] = {
    {XFA_Element::Execute, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Script, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::SignData, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Submit, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kEventAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Ref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Listen, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::RefOnly},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Activity, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Click},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Event::CXFA_Event(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Event,
                kEventPropertyData,
                kEventAttributeData,
                pdfium::MakeUnique<CJX_Event>(this)) {}

CXFA_Event::~CXFA_Event() = default;

XFA_AttributeValue CXFA_Event::GetActivity() {
  return JSObject()->GetEnum(XFA_Attribute::Activity);
}

XFA_Element CXFA_Event::GetEventType() const {
  CXFA_Node* pChild = GetFirstChild();
  while (pChild) {
    XFA_Element eType = pChild->GetElementType();
    if (eType != XFA_Element::Extras)
      return eType;

    pChild = pChild->GetNextSibling();
  }
  return XFA_Element::Unknown;
}

WideString CXFA_Event::GetRef() {
  return JSObject()->GetCData(XFA_Attribute::Ref);
}

CXFA_Script* CXFA_Event::GetScriptIfExists() {
  return GetChild<CXFA_Script>(0, XFA_Element::Script, false);
}

#ifdef PDF_XFA_ELEMENT_SUBMIT_ENABLED
CXFA_Submit* CXFA_Event::GetSubmitIfExists() {
  return GetChild<CXFA_Submit>(0, XFA_Element::Submit, false);
}
#endif  // PDF_XFA_ELEMENT_SUBMIT_ENABLED
