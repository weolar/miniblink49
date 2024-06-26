// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_locale.h"

namespace {

const CXFA_Node::PropertyData kLocalePropertyData[] = {
    {XFA_Element::DatePatterns, 1, 0},    {XFA_Element::CalendarSymbols, 1, 0},
    {XFA_Element::CurrencySymbols, 1, 0}, {XFA_Element::Typefaces, 1, 0},
    {XFA_Element::DateTimeSymbols, 1, 0}, {XFA_Element::NumberPatterns, 1, 0},
    {XFA_Element::NumberSymbols, 1, 0},   {XFA_Element::TimePatterns, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kLocaleAttributeData[] = {
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Desc, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Lock, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Locale::CXFA_Locale(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Config | XFA_XDPPACKET_LocaleSet),
                XFA_ObjectType::Node,
                XFA_Element::Locale,
                kLocalePropertyData,
                kLocaleAttributeData) {}

CXFA_Locale::~CXFA_Locale() = default;
