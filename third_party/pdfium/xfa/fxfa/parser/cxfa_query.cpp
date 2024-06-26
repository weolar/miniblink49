// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_query.h"

#include "fxjs/xfa/cjx_query.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kQueryPropertyData[] = {
    {XFA_Element::RecordSet, 1, 0},
    {XFA_Element::Select, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kQueryAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::CommandType, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Unknown},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Query::CXFA_Query(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_SourceSet,
                XFA_ObjectType::Node,
                XFA_Element::Query,
                kQueryPropertyData,
                kQueryAttributeData,
                pdfium::MakeUnique<CJX_Query>(this)) {}

CXFA_Query::~CXFA_Query() = default;
