// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_signature.h"

#include "fxjs/xfa/cjx_signature.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kSignaturePropertyData[] = {
    {XFA_Element::Margin, 1, 0}, {XFA_Element::Filter, 1, 0},
    {XFA_Element::Border, 1, 0}, {XFA_Element::Manifest, 1, 0},
    {XFA_Element::Extras, 1, 0}, {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kSignatureAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Type, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::PDF1_3},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Signature::CXFA_Signature(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Signature,
                kSignaturePropertyData,
                kSignatureAttributeData,
                pdfium::MakeUnique<CJX_Signature>(this)) {}

CXFA_Signature::~CXFA_Signature() = default;

XFA_FFWidgetType CXFA_Signature::GetDefaultFFWidgetType() const {
  return XFA_FFWidgetType::kSignature;
}
