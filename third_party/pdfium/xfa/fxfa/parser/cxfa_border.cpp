// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_border.h"

#include "fxjs/xfa/cjx_border.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kBorderPropertyData[] = {
    {XFA_Element::Margin, 1, 0}, {XFA_Element::Edge, 4, 0},
    {XFA_Element::Corner, 4, 0}, {XFA_Element::Fill, 1, 0},
    {XFA_Element::Extras, 1, 0}, {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kBorderAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Break, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Close},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Presence, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Visible},
    {XFA_Attribute::Relevant, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Hand, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Even},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Border::CXFA_Border(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Rectangle(doc,
                     packet,
                     (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                     XFA_ObjectType::Node,
                     XFA_Element::Border,
                     kBorderPropertyData,
                     kBorderAttributeData,
                     pdfium::MakeUnique<CJX_Border>(this)) {}

CXFA_Border::~CXFA_Border() = default;
