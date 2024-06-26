// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_xmlconnection.h"

#include "fxjs/xfa/cjx_xmlconnection.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kXmlConnectionPropertyData[] = {
    {XFA_Element::Uri, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kXmlConnectionAttributeData[] = {
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::DataDescription, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_XmlConnection::CXFA_XmlConnection(CXFA_Document* doc,
                                       XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_ConnectionSet,
                XFA_ObjectType::Node,
                XFA_Element::XmlConnection,
                kXmlConnectionPropertyData,
                kXmlConnectionAttributeData,
                pdfium::MakeUnique<CJX_XmlConnection>(this)) {}

CXFA_XmlConnection::~CXFA_XmlConnection() = default;
