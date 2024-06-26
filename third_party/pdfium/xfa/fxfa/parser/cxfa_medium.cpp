// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_medium.h"

#include "fxjs/xfa/cjx_medium.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kMediumAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::TrayOut, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Auto},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Orientation, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Portrait},
    {XFA_Attribute::ImagingBBox, XFA_AttributeType::CData, (void*)L"none"},
    {XFA_Attribute::Short, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::TrayIn, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Auto},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Stock, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Long, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Medium::CXFA_Medium(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Medium,
                nullptr,
                kMediumAttributeData,
                pdfium::MakeUnique<CJX_Medium>(this)) {}

CXFA_Medium::~CXFA_Medium() = default;
