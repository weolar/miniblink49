// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_desc.h"

#include "fxjs/xfa/cjx_desc.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kDescPropertyData[] = {
    {XFA_Element::Text, 1, 0},     {XFA_Element::Time, 1, 0},
    {XFA_Element::DateTime, 1, 0}, {XFA_Element::Image, 1, 0},
    {XFA_Element::Decimal, 1, 0},  {XFA_Element::Boolean, 1, 0},
    {XFA_Element::Integer, 1, 0},  {XFA_Element::ExData, 1, 0},
    {XFA_Element::Date, 1, 0},     {XFA_Element::Float, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kDescAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Desc::CXFA_Desc(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Desc,
                kDescPropertyData,
                kDescAttributeData,
                pdfium::MakeUnique<CJX_Desc>(this)) {}

CXFA_Desc::~CXFA_Desc() = default;
