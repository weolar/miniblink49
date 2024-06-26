// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_occur.h"

#include "fxjs/xfa/cjx_occur.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kOccurPropertyData[] = {
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kOccurAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Max, XFA_AttributeType::Integer, (void*)1},
    {XFA_Attribute::Min, XFA_AttributeType::Integer, (void*)1},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Initial, XFA_AttributeType::Integer, (void*)1},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Occur::CXFA_Occur(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Occur,
                kOccurPropertyData,
                kOccurAttributeData,
                pdfium::MakeUnique<CJX_Occur>(this)) {}

CXFA_Occur::~CXFA_Occur() = default;

int32_t CXFA_Occur::GetMax() {
  Optional<int32_t> max = JSObject()->TryInteger(XFA_Attribute::Max, true);
  return max ? *max : GetMin();
}

int32_t CXFA_Occur::GetMin() {
  Optional<int32_t> min = JSObject()->TryInteger(XFA_Attribute::Min, true);
  return min && *min >= 0 ? *min : 1;
}

std::tuple<int32_t, int32_t, int32_t> CXFA_Occur::GetOccurInfo() {
  int32_t iMin = GetMin();
  int32_t iMax = GetMax();

  Optional<int32_t> init =
      JSObject()->TryInteger(XFA_Attribute::Initial, false);
  return {iMin, iMax, init && *init >= iMin ? *init : iMin};
}

void CXFA_Occur::SetMax(int32_t iMax) {
  iMax = (iMax != -1 && iMax < 1) ? 1 : iMax;
  JSObject()->SetInteger(XFA_Attribute::Max, iMax, false);

  int32_t iMin = GetMin();
  if (iMax != -1 && iMax < iMin) {
    iMin = iMax;
    JSObject()->SetInteger(XFA_Attribute::Min, iMin, false);
  }
}

void CXFA_Occur::SetMin(int32_t iMin) {
  iMin = (iMin < 0) ? 1 : iMin;
  JSObject()->SetInteger(XFA_Attribute::Min, iMin, false);

  int32_t iMax = GetMax();
  if (iMax > 0 && iMax < iMin) {
    iMax = iMin;
    JSObject()->SetInteger(XFA_Attribute::Max, iMax, false);
  }
}
