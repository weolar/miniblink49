// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_edge.h"

#include "fxjs/xfa/cjx_edge.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kEdgePropertyData[] = {
    {XFA_Element::Color, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kEdgeAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Cap, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Square},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Stroke, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Solid},
    {XFA_Attribute::Presence, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Visible},
    {XFA_Attribute::Thickness, XFA_AttributeType::Measure, (void*)L"0.5pt"},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Edge::CXFA_Edge(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Stroke(doc,
                  packet,
                  (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                  XFA_ObjectType::Node,
                  XFA_Element::Edge,
                  kEdgePropertyData,
                  kEdgeAttributeData,
                  pdfium::MakeUnique<CJX_Edge>(this)) {}

CXFA_Edge::~CXFA_Edge() = default;
