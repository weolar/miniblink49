// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_binditems.h"

#include "fxjs/xfa/cjx_binditems.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kBindItemsAttributeData[] = {
    {XFA_Attribute::Ref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Connection, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::LabelRef, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::ValueRef, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_BindItems::CXFA_BindItems(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::BindItems,
                nullptr,
                kBindItemsAttributeData,
                pdfium::MakeUnique<CJX_BindItems>(this)) {}

CXFA_BindItems::~CXFA_BindItems() = default;

WideString CXFA_BindItems::GetLabelRef() {
  return JSObject()->GetCData(XFA_Attribute::LabelRef);
}

WideString CXFA_BindItems::GetValueRef() {
  return JSObject()->GetCData(XFA_Attribute::ValueRef);
}

WideString CXFA_BindItems::GetRef() {
  return JSObject()->GetCData(XFA_Attribute::Ref);
}
