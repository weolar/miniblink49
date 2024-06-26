// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_numberpatterns.h"

namespace {

const CXFA_Node::PropertyData kNumberPatternsPropertyData[] = {
    {XFA_Element::NumberPattern, 4, 0},
    {XFA_Element::Unknown, 0, 0}};

}  // namespace

CXFA_NumberPatterns::CXFA_NumberPatterns(CXFA_Document* doc,
                                         XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_LocaleSet,
                XFA_ObjectType::Node,
                XFA_Element::NumberPatterns,
                kNumberPatternsPropertyData,
                nullptr) {}

CXFA_NumberPatterns::~CXFA_NumberPatterns() = default;
