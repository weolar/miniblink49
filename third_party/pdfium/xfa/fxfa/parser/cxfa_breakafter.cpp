// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_breakafter.h"

#include "fxjs/xfa/cjx_breakafter.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kBreakAfterPropertyData[] = {
    {XFA_Element::Script, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kBreakAfterAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::StartNew, XFA_AttributeType::Boolean, (void*)0},
    {XFA_Attribute::Trailer, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::TargetType, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Auto},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Target, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Leader, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_BreakAfter::CXFA_BreakAfter(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::BreakAfter,
                kBreakAfterPropertyData,
                kBreakAfterAttributeData,
                pdfium::MakeUnique<CJX_BreakAfter>(this)) {}

CXFA_BreakAfter::~CXFA_BreakAfter() = default;
