// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_line.h"

#include "fxjs/xfa/cjx_line.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_edge.h"
#include "xfa/fxfa/parser/cxfa_node.h"

namespace {

const CXFA_Node::PropertyData kLinePropertyData[] = {
    {XFA_Element::Edge, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kLineAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Slope, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Backslash},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Hand, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Even},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Line::CXFA_Line(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Line,
                kLinePropertyData,
                kLineAttributeData,
                pdfium::MakeUnique<CJX_Line>(this)) {}

CXFA_Line::~CXFA_Line() = default;

XFA_AttributeValue CXFA_Line::GetHand() {
  return JSObject()->GetEnum(XFA_Attribute::Hand);
}

bool CXFA_Line::GetSlope() {
  return JSObject()->GetEnum(XFA_Attribute::Slope) == XFA_AttributeValue::Slash;
}

CXFA_Edge* CXFA_Line::GetEdgeIfExists() {
  return GetChild<CXFA_Edge>(0, XFA_Element::Edge, false);
}
