// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_transform.h"

namespace {

const CXFA_Node::PropertyData kTransformPropertyData[] = {
    {XFA_Element::Whitespace, 1, 0},  {XFA_Element::Rename, 1, 0},
    {XFA_Element::IfEmpty, 1, 0},     {XFA_Element::Presence, 1, 0},
    {XFA_Element::Picture, 1, 0},     {XFA_Element::NameAttr, 1, 0},
    {XFA_Element::GroupParent, 1, 0}, {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kTransformAttributeData[] = {
    {XFA_Attribute::Ref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Desc, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Lock, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Transform::CXFA_Transform(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_Config,
                XFA_ObjectType::Node,
                XFA_Element::Transform,
                kTransformPropertyData,
                kTransformAttributeData) {}

CXFA_Transform::~CXFA_Transform() = default;
