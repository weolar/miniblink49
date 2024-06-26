// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_area.h"

#include "fxjs/xfa/cjx_area.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kAreaPropertyData[] = {
    {XFA_Element::Desc, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kAreaAttributeData[] = {
    {XFA_Attribute::X, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Y, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Level, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::Relevant, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::ColSpan, XFA_AttributeType::Integer, (void*)1},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Lock, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Area::CXFA_Area(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(
          doc,
          packet,
          (XFA_XDPPACKET_Config | XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
          XFA_ObjectType::ContainerNode,
          XFA_Element::Area,
          kAreaPropertyData,
          kAreaAttributeData,
          pdfium::MakeUnique<CJX_Area>(this)) {}

CXFA_Area::~CXFA_Area() = default;
