// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_exclgroup.h"

#include "fxjs/xfa/cjx_exclgroup.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kExclGroupPropertyData[] = {
    {XFA_Element::Margin, 1, 0},    {XFA_Element::Para, 1, 0},
    {XFA_Element::Border, 1, 0},    {XFA_Element::Assist, 1, 0},
    {XFA_Element::Traversal, 1, 0}, {XFA_Element::Validate, 1, 0},
    {XFA_Element::Caption, 1, 0},   {XFA_Element::Bind, 1, 0},
    {XFA_Element::Desc, 1, 0},      {XFA_Element::Calculate, 1, 0},
    {XFA_Element::Extras, 1, 0},    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kExclGroupAttributeData[] = {
    {XFA_Attribute::H, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::W, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::X, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Y, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::HAlign, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Left},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Access, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Open},
    {XFA_Attribute::Presence, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Visible},
    {XFA_Attribute::VAlign, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Top},
    {XFA_Attribute::MaxH, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::MaxW, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::MinH, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::MinW, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Layout, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Position},
    {XFA_Attribute::Relevant, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::ColSpan, XFA_AttributeType::Integer, (void*)1},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::AnchorType, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::TopLeft},
    {XFA_Attribute::AccessKey, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_ExclGroup::CXFA_ExclGroup(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::ContainerNode,
                XFA_Element::ExclGroup,
                kExclGroupPropertyData,
                kExclGroupAttributeData,
                pdfium::MakeUnique<CJX_ExclGroup>(this)) {}

CXFA_ExclGroup::~CXFA_ExclGroup() = default;
