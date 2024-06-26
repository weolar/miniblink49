// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_template.h"

#include "fxjs/xfa/cjx_template.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kTemplatePropertyData[] = {
    {XFA_Element::Uri, 1, 0},       {XFA_Element::Xsl, 1, 0},
    {XFA_Element::StartPage, 1, 0}, {XFA_Element::Relevant, 1, 0},
    {XFA_Element::Base, 1, 0},      {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kTemplateAttributeData[] = {
    {XFA_Attribute::Desc, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::BaseProfile, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Full},
    {XFA_Attribute::Lock, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Template::CXFA_Template(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(
          doc,
          packet,
          (XFA_XDPPACKET_Config | XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
          XFA_ObjectType::ModelNode,
          XFA_Element::Template,
          kTemplatePropertyData,
          kTemplateAttributeData,
          pdfium::MakeUnique<CJX_Template>(this)) {}

CXFA_Template::~CXFA_Template() = default;
