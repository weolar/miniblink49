// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_sourceset.h"

#include "fxjs/xfa/cjx_sourceset.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kSourceSetAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_SourceSet::CXFA_SourceSet(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_SourceSet,
                XFA_ObjectType::ModelNode,
                XFA_Element::SourceSet,
                nullptr,
                kSourceSetAttributeData,
                pdfium::MakeUnique<CJX_SourceSet>(this)) {}

CXFA_SourceSet::~CXFA_SourceSet() = default;
