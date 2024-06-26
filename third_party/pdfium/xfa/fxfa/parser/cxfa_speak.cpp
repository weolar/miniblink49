// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_speak.h"

#include "fxjs/xfa/cjx_speak.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kSpeakAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Rid, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Priority, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Custom},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Disable, XFA_AttributeType::Boolean, (void*)0},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Speak::CXFA_Speak(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::TextNode,
                XFA_Element::Speak,
                nullptr,
                kSpeakAttributeData,
                pdfium::MakeUnique<CJX_Speak>(this)) {}

CXFA_Speak::~CXFA_Speak() = default;
