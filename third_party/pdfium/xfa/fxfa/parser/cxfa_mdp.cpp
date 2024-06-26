// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_mdp.h"

#include "fxjs/xfa/cjx_mdp.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kMdpAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::SignatureType, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Filter},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Permissions, XFA_AttributeType::Integer, (void*)2},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Mdp::CXFA_Mdp(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Mdp,
                nullptr,
                kMdpAttributeData,
                pdfium::MakeUnique<CJX_Mdp>(this)) {}

CXFA_Mdp::~CXFA_Mdp() = default;
