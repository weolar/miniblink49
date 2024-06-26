// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_traverse.h"

#include "fxjs/xfa/cjx_traverse.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kTraversePropertyData[] = {
    {XFA_Element::Script, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kTraverseAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Ref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Operation, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Next},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Traverse::CXFA_Traverse(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Traverse,
                kTraversePropertyData,
                kTraverseAttributeData,
                pdfium::MakeUnique<CJX_Traverse>(this)) {}

CXFA_Traverse::~CXFA_Traverse() {}
