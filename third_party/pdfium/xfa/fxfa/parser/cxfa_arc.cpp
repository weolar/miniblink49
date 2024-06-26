// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_arc.h"

#include "fxjs/xfa/cjx_arc.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kArcPropertyData[] = {
    {XFA_Element::Edge, 1, 0},
    {XFA_Element::Fill, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kArcAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::StartAngle, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::SweepAngle, XFA_AttributeType::Integer, (void*)360},
    {XFA_Attribute::Circular, XFA_AttributeType::Boolean, (void*)0},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Hand, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Even},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};


}  // namespace

CXFA_Arc::CXFA_Arc(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Box(doc,
               packet,
               (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
               XFA_ObjectType::Node,
               XFA_Element::Arc,
               kArcPropertyData,
               kArcAttributeData,
               pdfium::MakeUnique<CJX_Arc>(this)) {}

CXFA_Arc::~CXFA_Arc() = default;
