// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_delta.h"

#include "fxjs/xfa/cjx_delta.h"
#include "third_party/base/ptr_util.h"

CXFA_Delta::CXFA_Delta(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_Form,
                XFA_ObjectType::Object,
                XFA_Element::Delta,
                nullptr,
                nullptr,
                pdfium::MakeUnique<CJX_Delta>(this)) {}

CXFA_Delta::~CXFA_Delta() = default;
