// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_recordset.h"

#include "fxjs/xfa/cjx_recordset.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kRecordSetAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Max, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::EofAction, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::MoveLast},
    {XFA_Attribute::CursorType, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::ForwardOnly},
    {XFA_Attribute::LockType, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::ReadOnly},
    {XFA_Attribute::BofAction, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::MoveFirst},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::CursorLocation, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Client},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_RecordSet::CXFA_RecordSet(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_SourceSet,
                XFA_ObjectType::Node,
                XFA_Element::RecordSet,
                nullptr,
                kRecordSetAttributeData,
                pdfium::MakeUnique<CJX_RecordSet>(this)) {}

CXFA_RecordSet::~CXFA_RecordSet() = default;
