// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_draw.h"

#include "fxjs/xfa/cjx_draw.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kDrawPropertyData[] = {
    {XFA_Element::Ui, 1, 0},     {XFA_Element::Margin, 1, 0},
    {XFA_Element::Para, 1, 0},   {XFA_Element::Border, 1, 0},
    {XFA_Element::Assist, 1, 0}, {XFA_Element::Traversal, 1, 0},
    {XFA_Element::Keep, 1, 0},   {XFA_Element::Caption, 1, 0},
    {XFA_Element::Desc, 1, 0},   {XFA_Element::Font, 1, 0},
    {XFA_Element::Value, 1, 0},  {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kDrawAttributeData[] = {
    {XFA_Attribute::H, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::W, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::X, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Y, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::HAlign, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Left},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Rotate, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::Presence, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Visible},
    {XFA_Attribute::VAlign, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Top},
    {XFA_Attribute::MaxH, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::MaxW, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::MinH, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::MinW, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Relevant, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::ColSpan, XFA_AttributeType::Integer, (void*)1},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Locale, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::AnchorType, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::TopLeft},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Draw::CXFA_Draw(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::ContainerNode,
                XFA_Element::Draw,
                kDrawPropertyData,
                kDrawAttributeData,
                pdfium::MakeUnique<CJX_Draw>(this)) {}

CXFA_Draw::~CXFA_Draw() = default;
