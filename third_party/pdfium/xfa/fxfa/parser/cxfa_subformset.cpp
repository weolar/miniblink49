// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_subformset.h"

#include "fxjs/xfa/cjx_subformset.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kSubformSetPropertyData[] = {
    {XFA_Element::Break, 1, 0},  {XFA_Element::Overflow, 1, 0},
    {XFA_Element::Desc, 1, 0},   {XFA_Element::Bookend, 1, 0},
    {XFA_Element::Extras, 1, 0}, {XFA_Element::Occur, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kSubformSetAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Relation, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Ordered},
    {XFA_Attribute::Relevant, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_SubformSet::CXFA_SubformSet(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::ContainerNode,
                XFA_Element::SubformSet,
                kSubformSetPropertyData,
                kSubformSetAttributeData,
                pdfium::MakeUnique<CJX_SubformSet>(this)) {}

CXFA_SubformSet::~CXFA_SubformSet() = default;
