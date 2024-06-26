// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_form.h"

#include "fxjs/xfa/cjx_form.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kFormAttributeData[] = {
    {XFA_Attribute::Checksum, XFA_AttributeType::CData, (void*)nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Form::CXFA_Form(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_Form,
                XFA_ObjectType::ModelNode,
                XFA_Element::Form,
                nullptr,
                kFormAttributeData,
                pdfium::MakeUnique<CJX_Form>(this)) {}

CXFA_Form::~CXFA_Form() = default;
