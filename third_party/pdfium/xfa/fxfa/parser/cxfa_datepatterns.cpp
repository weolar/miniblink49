// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_datepatterns.h"

namespace {

const CXFA_Node::PropertyData kDatePatternsPropertyData[] = {
    {XFA_Element::DatePattern, 4, 0},
    {XFA_Element::Unknown, 0, 0}};

}  // namespace

CXFA_DatePatterns::CXFA_DatePatterns(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_LocaleSet,
                XFA_ObjectType::Node,
                XFA_Element::DatePatterns,
                kDatePatternsPropertyData,
                nullptr) {}

CXFA_DatePatterns::~CXFA_DatePatterns() = default;
