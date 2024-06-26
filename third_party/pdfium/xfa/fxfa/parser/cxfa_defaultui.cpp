// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_defaultui.h"

#include "fxjs/xfa/cjx_defaultui.h"

namespace {

const CXFA_Node::PropertyData kDefaultUiPropertyData[] = {
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kDefaultUiAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_DefaultUi::CXFA_DefaultUi(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::DefaultUi,
                kDefaultUiPropertyData,
                kDefaultUiAttributeData) {}

CXFA_DefaultUi::~CXFA_DefaultUi() = default;

XFA_FFWidgetType CXFA_DefaultUi::GetDefaultFFWidgetType() const {
  return XFA_FFWidgetType::kTextEdit;
}
