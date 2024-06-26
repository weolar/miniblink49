// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_effectiveoutputpolicy.h"

namespace {

const CXFA_Node::AttributeData kEffectiveOutputPolicyAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_EffectiveOutputPolicy::CXFA_EffectiveOutputPolicy(CXFA_Document* doc,
                                                       XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_ConnectionSet,
                XFA_ObjectType::Node,
                XFA_Element::EffectiveOutputPolicy,
                nullptr,
                kEffectiveOutputPolicyAttributeData) {}

CXFA_EffectiveOutputPolicy::~CXFA_EffectiveOutputPolicy() = default;
