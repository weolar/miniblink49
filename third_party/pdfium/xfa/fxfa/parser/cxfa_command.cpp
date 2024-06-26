// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_command.h"

#include "fxjs/xfa/cjx_command.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kCommandPropertyData[] = {
    {XFA_Element::Query, 1, 0},
    {XFA_Element::Insert, 1, 0},
    {XFA_Element::Update, 1, 0},
    {XFA_Element::Delete, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kCommandAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Timeout, XFA_AttributeType::Integer, (void*)30},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Command::CXFA_Command(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_SourceSet,
                XFA_ObjectType::Node,
                XFA_Element::Command,
                kCommandPropertyData,
                kCommandAttributeData,
                pdfium::MakeUnique<CJX_Command>(this)) {}

CXFA_Command::~CXFA_Command() = default;
