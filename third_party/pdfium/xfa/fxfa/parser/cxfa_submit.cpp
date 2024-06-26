// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_submit.h"

#include "fxjs/xfa/cjx_submit.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kSubmitPropertyData[] = {
    {XFA_Element::Encrypt, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kSubmitAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Format, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Xdp},
    {XFA_Attribute::EmbedPDF, XFA_AttributeType::Boolean, (void*)0},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Target, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::TextEncoding, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::XdpContent, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Submit::CXFA_Submit(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Submit,
                kSubmitPropertyData,
                kSubmitAttributeData,
                pdfium::MakeUnique<CJX_Submit>(this)) {}

CXFA_Submit::~CXFA_Submit() = default;

bool CXFA_Submit::IsSubmitEmbedPDF() {
  return JSObject()->GetBoolean(XFA_Attribute::EmbedPDF);
}

XFA_AttributeValue CXFA_Submit::GetSubmitFormat() {
  return JSObject()->GetEnum(XFA_Attribute::Format);
}

WideString CXFA_Submit::GetSubmitTarget() {
  return JSObject()->GetCData(XFA_Attribute::Target);
}

WideString CXFA_Submit::GetSubmitXDPContent() {
  return JSObject()->GetCData(XFA_Attribute::XdpContent);
}
