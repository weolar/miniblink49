// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_bind.h"

#include "fxjs/xfa/cjx_bind.h"
#include "fxjs/xfa/cjx_object.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_picture.h"

namespace {

const CXFA_Node::PropertyData kBindPropertyData[] = {
    {XFA_Element::Picture, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kBindAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Ref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::ContentType, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::TransferEncoding, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::None},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Match, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Once},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Bind::CXFA_Bind(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_SourceSet | XFA_XDPPACKET_Template |
                 XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Bind,
                kBindPropertyData,
                kBindAttributeData,
                pdfium::MakeUnique<CJX_Bind>(this)) {}

CXFA_Bind::~CXFA_Bind() = default;

WideString CXFA_Bind::GetPicture() {
  CXFA_Picture* pPicture =
      GetChild<CXFA_Picture>(0, XFA_Element::Picture, false);
  return pPicture ? pPicture->JSObject()->GetContent(false) : WideString();
}
