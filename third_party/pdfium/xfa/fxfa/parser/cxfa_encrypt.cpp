// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_encrypt.h"

#include "fxjs/xfa/cjx_encrypt.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kEncryptPropertyData[] = {
    {XFA_Element::Certificate, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kEncryptAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Desc, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Lock, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Encrypt::CXFA_Encrypt(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(
          doc,
          packet,
          (XFA_XDPPACKET_Template | XFA_XDPPACKET_Config | XFA_XDPPACKET_Form),
          XFA_ObjectType::ContentNode,
          XFA_Element::Encrypt,
          kEncryptPropertyData,
          kEncryptAttributeData,
          pdfium::MakeUnique<CJX_Encrypt>(this)) {}

CXFA_Encrypt::~CXFA_Encrypt() = default;
