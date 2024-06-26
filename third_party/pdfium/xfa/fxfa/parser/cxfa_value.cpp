// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_value.h"

#include "fxjs/xfa/cjx_object.h"
#include "fxjs/xfa/cjx_value.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_arc.h"
#include "xfa/fxfa/parser/cxfa_exdata.h"
#include "xfa/fxfa/parser/cxfa_image.h"
#include "xfa/fxfa/parser/cxfa_line.h"
#include "xfa/fxfa/parser/cxfa_rectangle.h"

namespace {

const CXFA_Node::PropertyData kValuePropertyData[] = {
    {XFA_Element::Arc, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Text, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Time, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::DateTime, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Image, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Decimal, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Boolean, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Integer, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::ExData, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Rectangle, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Date, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Float, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Line, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kValueAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Relevant, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Override, XFA_AttributeType::Boolean, (void*)0},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Value::CXFA_Value(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Value,
                kValuePropertyData,
                kValueAttributeData,
                pdfium::MakeUnique<CJX_Value>(this)) {}

CXFA_Value::~CXFA_Value() = default;

XFA_Element CXFA_Value::GetChildValueClassID() const {
  CXFA_Node* pNode = GetFirstChild();
  return pNode ? pNode->GetElementType() : XFA_Element::Unknown;
}

WideString CXFA_Value::GetChildValueContent() const {
  CXFA_Node* pNode = GetFirstChild();
  return pNode
             ? pNode->JSObject()->TryContent(false, true).value_or(WideString())
             : WideString();
}

CXFA_Arc* CXFA_Value::GetArcIfExists() const {
  CXFA_Node* node = GetFirstChild();
  if (!node || node->GetElementType() != XFA_Element::Arc)
    return nullptr;
  return static_cast<CXFA_Arc*>(node);
}

CXFA_Line* CXFA_Value::GetLineIfExists() const {
  CXFA_Node* node = GetFirstChild();
  if (!node || node->GetElementType() != XFA_Element::Line)
    return nullptr;
  return static_cast<CXFA_Line*>(node);
}

CXFA_Rectangle* CXFA_Value::GetRectangleIfExists() const {
  CXFA_Node* node = GetFirstChild();
  if (!node || node->GetElementType() != XFA_Element::Rectangle)
    return nullptr;
  return static_cast<CXFA_Rectangle*>(node);
}

CXFA_Text* CXFA_Value::GetTextIfExists() const {
  CXFA_Node* node = GetFirstChild();
  if (!node || node->GetElementType() != XFA_Element::Text)
    return nullptr;
  return static_cast<CXFA_Text*>(node);
}

CXFA_ExData* CXFA_Value::GetExDataIfExists() const {
  CXFA_Node* node = GetFirstChild();
  if (!node || node->GetElementType() != XFA_Element::ExData)
    return nullptr;
  return static_cast<CXFA_ExData*>(node);
}

CXFA_Image* CXFA_Value::GetImageIfExists() const {
  CXFA_Node* node = GetFirstChild();
  if (!node || node->GetElementType() != XFA_Element::Image)
    return nullptr;
  return static_cast<CXFA_Image*>(node);
}
