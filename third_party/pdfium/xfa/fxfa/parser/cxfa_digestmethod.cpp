// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_digestmethod.h"

#include "fxjs/xfa/cjx_digestmethod.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kDigestMethodAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_DigestMethod::CXFA_DigestMethod(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::NodeC,
                XFA_Element::DigestMethod,
                nullptr,
                kDigestMethodAttributeData,
                pdfium::MakeUnique<CJX_DigestMethod>(this)) {}

CXFA_DigestMethod::~CXFA_DigestMethod() = default;
