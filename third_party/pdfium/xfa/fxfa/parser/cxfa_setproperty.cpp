// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_setproperty.h"

#include "fxjs/xfa/cjx_setproperty.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kSetPropertyAttributeData[] = {
    {XFA_Attribute::Ref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Connection, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Target, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_SetProperty::CXFA_SetProperty(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::SetProperty,
                nullptr,
                kSetPropertyAttributeData,
                pdfium::MakeUnique<CJX_SetProperty>(this)) {}

CXFA_SetProperty::~CXFA_SetProperty() = default;
