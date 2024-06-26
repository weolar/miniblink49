// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_execute.h"

#include "fxjs/xfa/cjx_execute.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kExecuteAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Connection, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::RunAt, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Client},
    {XFA_Attribute::ExecuteType, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Import},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Execute::CXFA_Execute(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Execute,
                nullptr,
                kExecuteAttributeData,
                pdfium::MakeUnique<CJX_Execute>(this)) {}

CXFA_Execute::~CXFA_Execute() = default;
