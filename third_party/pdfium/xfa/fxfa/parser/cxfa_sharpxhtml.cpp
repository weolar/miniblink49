// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_sharpxhtml.h"

namespace {

const CXFA_Node::AttributeData kSharpxHTMLAttributeData[] = {
    {XFA_Attribute::Value, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_SharpxHTML::CXFA_SharpxHTML(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Config |
                 XFA_XDPPACKET_LocaleSet | XFA_XDPPACKET_ConnectionSet |
                 XFA_XDPPACKET_SourceSet | XFA_XDPPACKET_Form),
                XFA_ObjectType::NodeV,
                XFA_Element::SharpxHTML,
                nullptr,
                kSharpxHTMLAttributeData) {}

CXFA_SharpxHTML::~CXFA_SharpxHTML() = default;
