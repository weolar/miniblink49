// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_certificates.h"

#include "fxjs/xfa/cjx_certificates.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kCertificatesPropertyData[] = {
    {XFA_Element::KeyUsage, 1, 0}, {XFA_Element::SubjectDNs, 1, 0},
    {XFA_Element::Issuers, 1, 0},  {XFA_Element::Signing, 1, 0},
    {XFA_Element::Oids, 1, 0},     {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kCertificatesAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Url, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::CredentialServerPolicy, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Optional},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::UrlPolicy, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Certificates::CXFA_Certificates(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Certificates,
                kCertificatesPropertyData,
                kCertificatesAttributeData,
                pdfium::MakeUnique<CJX_Certificates>(this)) {}

CXFA_Certificates::~CXFA_Certificates() = default;
