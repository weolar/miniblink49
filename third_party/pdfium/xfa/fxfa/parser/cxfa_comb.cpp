// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_comb.h"

#include "fxjs/xfa/cjx_comb.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kCombAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::NumberOfCells, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Comb::CXFA_Comb(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Comb,
                nullptr,
                kCombAttributeData,
                pdfium::MakeUnique<CJX_Comb>(this)) {}

CXFA_Comb::~CXFA_Comb() {}
