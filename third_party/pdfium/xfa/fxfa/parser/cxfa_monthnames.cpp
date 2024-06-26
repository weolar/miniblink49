// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_monthnames.h"

namespace {

const CXFA_Node::PropertyData kMonthNamesPropertyData[] = {
    {XFA_Element::Month, 12, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kMonthNamesAttributeData[] = {
    {XFA_Attribute::Abbr, XFA_AttributeType::Boolean, (void*)0},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_MonthNames::CXFA_MonthNames(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_LocaleSet,
                XFA_ObjectType::Node,
                XFA_Element::MonthNames,
                kMonthNamesPropertyData,
                kMonthNamesAttributeData) {}

CXFA_MonthNames::~CXFA_MonthNames() = default;
