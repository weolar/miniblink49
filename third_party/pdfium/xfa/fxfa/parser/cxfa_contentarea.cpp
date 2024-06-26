// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_contentarea.h"

#include "fxjs/xfa/cjx_contentarea.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kContentAreaPropertyData[] = {
    {XFA_Element::Desc, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kContentAreaAttributeData[] = {
    {XFA_Attribute::H, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::W, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::X, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Y, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Relevant, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_ContentArea::CXFA_ContentArea(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::ContainerNode,
                XFA_Element::ContentArea,
                kContentAreaPropertyData,
                kContentAreaAttributeData,
                pdfium::MakeUnique<CJX_ContentArea>(this)) {}

CXFA_ContentArea::~CXFA_ContentArea() = default;
