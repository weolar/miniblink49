// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_pageset.h"

#include "fxjs/xfa/cjx_pageset.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kPageSetPropertyData[] = {
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Occur, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kPageSetAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Relation, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::OrderedOccurrence},
    {XFA_Attribute::Relevant, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::DuplexImposition, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::LongEdge},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_PageSet::CXFA_PageSet(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::ContainerNode,
                XFA_Element::PageSet,
                kPageSetPropertyData,
                kPageSetAttributeData,
                pdfium::MakeUnique<CJX_PageSet>(this)) {}

CXFA_PageSet::~CXFA_PageSet() = default;
