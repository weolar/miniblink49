// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_imageedit.h"

#include "fxjs/xfa/cjx_imageedit.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kImageEditPropertyData[] = {
    {XFA_Element::Margin, 1, 0},
    {XFA_Element::Border, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kImageEditAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Data, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Link},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_ImageEdit::CXFA_ImageEdit(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::ImageEdit,
                kImageEditPropertyData,
                kImageEditAttributeData,
                pdfium::MakeUnique<CJX_ImageEdit>(this)) {}

CXFA_ImageEdit::~CXFA_ImageEdit() = default;

XFA_Element CXFA_ImageEdit::GetValueNodeType() const {
  return XFA_Element::Image;
}

XFA_FFWidgetType CXFA_ImageEdit::GetDefaultFFWidgetType() const {
  return XFA_FFWidgetType::kImageEdit;
}
