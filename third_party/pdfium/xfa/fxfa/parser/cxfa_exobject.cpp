// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_exobject.h"

#include "fxjs/xfa/cjx_exobject.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kExObjectPropertyData[] = {
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kExObjectAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::CodeType, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Archive, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::CodeBase, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::ClassId, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_ExObject::CXFA_ExObject(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::ExObject,
                kExObjectPropertyData,
                kExObjectAttributeData,
                pdfium::MakeUnique<CJX_ExObject>(this)) {}

CXFA_ExObject::~CXFA_ExObject() = default;
