// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_decimal.h"

#include "fxjs/xfa/cjx_decimal.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kDecimalAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::FracDigits, XFA_AttributeType::Integer, (void*)2},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::LeadDigits, XFA_AttributeType::Integer, (void*)-1},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Decimal::CXFA_Decimal(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::ContentNode,
                XFA_Element::Decimal,
                nullptr,
                kDecimalAttributeData,
                pdfium::MakeUnique<CJX_Decimal>(this)) {}

CXFA_Decimal::~CXFA_Decimal() = default;
