// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_datavalue.h"

#include "fxjs/xfa/cjx_datavalue.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kDataValueAttributeData[] = {
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::ContentType, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Contains, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Data},
    {XFA_Attribute::Value, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::IsNull, XFA_AttributeType::Boolean, (void*)0},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_DataValue::CXFA_DataValue(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_Datasets,
                XFA_ObjectType::Node,
                XFA_Element::DataValue,
                nullptr,
                kDataValueAttributeData,
                pdfium::MakeUnique<CJX_DataValue>(this)) {}

CXFA_DataValue::~CXFA_DataValue() = default;
