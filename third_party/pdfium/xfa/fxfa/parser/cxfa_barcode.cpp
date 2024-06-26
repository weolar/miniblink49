// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_barcode.h"

#include "fxjs/xfa/cjx_barcode.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"

namespace {

const CXFA_Node::AttributeData kBarcodeAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::DataRowCount, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::DataPrep, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::None},
    {XFA_Attribute::Type, XFA_AttributeType::CData, (void*)nullptr},
    {XFA_Attribute::TextLocation, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Below},
    {XFA_Attribute::ModuleWidth, XFA_AttributeType::Measure, (void*)L"0.25mm"},
    {XFA_Attribute::PrintCheckDigit, XFA_AttributeType::Boolean, (void*)0},
    {XFA_Attribute::ModuleHeight, XFA_AttributeType::Measure, (void*)L"5mm"},
    {XFA_Attribute::StartChar, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Truncate, XFA_AttributeType::Boolean, (void*)0},
    {XFA_Attribute::WideNarrowRatio, XFA_AttributeType::CData, (void*)L"3:1"},
    {XFA_Attribute::ErrorCorrectionLevel, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::UpsMode, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::UsCarrier},
    {XFA_Attribute::Checksum, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::None},
    {XFA_Attribute::CharEncoding, XFA_AttributeType::CData, (void*)L"UTF-8"},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::DataColumnCount, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::RowColumnRatio, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::DataLength, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::EndChar, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Barcode::CXFA_Barcode(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Barcode,
                nullptr,
                kBarcodeAttributeData,
                pdfium::MakeUnique<CJX_Barcode>(this)) {}

CXFA_Barcode::~CXFA_Barcode() = default;

XFA_FFWidgetType CXFA_Barcode::GetDefaultFFWidgetType() const {
  return XFA_FFWidgetType::kBarcode;
}

WideString CXFA_Barcode::GetBarcodeType() {
  return WideString(JSObject()->GetCData(XFA_Attribute::Type));
}

Optional<BC_CHAR_ENCODING> CXFA_Barcode::GetCharEncoding() {
  Optional<WideString> wsCharEncoding =
      JSObject()->TryCData(XFA_Attribute::CharEncoding, true);
  if (!wsCharEncoding)
    return {};
  if (wsCharEncoding->CompareNoCase(L"UTF-16"))
    return {CHAR_ENCODING_UNICODE};
  if (wsCharEncoding->CompareNoCase(L"UTF-8"))
    return {CHAR_ENCODING_UTF8};
  return {};
}

Optional<bool> CXFA_Barcode::GetChecksum() {
  Optional<XFA_AttributeValue> checksum =
      JSObject()->TryEnum(XFA_Attribute::Checksum, true);
  if (!checksum)
    return {};

  switch (*checksum) {
    case XFA_AttributeValue::None:
      return {false};
    case XFA_AttributeValue::Auto:
      return {true};
    case XFA_AttributeValue::Checksum_1mod10:
    case XFA_AttributeValue::Checksum_1mod10_1mod11:
    case XFA_AttributeValue::Checksum_2mod10:
    default:
      break;
  }
  return {};
}

Optional<int32_t> CXFA_Barcode::GetDataLength() {
  Optional<WideString> wsDataLength =
      JSObject()->TryCData(XFA_Attribute::DataLength, true);
  if (!wsDataLength)
    return {};

  return {FXSYS_wtoi(wsDataLength->c_str())};
}

Optional<char> CXFA_Barcode::GetStartChar() {
  Optional<WideString> wsStartEndChar =
      JSObject()->TryCData(XFA_Attribute::StartChar, true);
  if (!wsStartEndChar || wsStartEndChar->IsEmpty())
    return {};

  return {static_cast<char>((*wsStartEndChar)[0])};
}

Optional<char> CXFA_Barcode::GetEndChar() {
  Optional<WideString> wsStartEndChar =
      JSObject()->TryCData(XFA_Attribute::EndChar, true);
  if (!wsStartEndChar || wsStartEndChar->IsEmpty())
    return {};

  return {static_cast<char>((*wsStartEndChar)[0])};
}

Optional<int32_t> CXFA_Barcode::GetECLevel() {
  Optional<WideString> wsECLevel =
      JSObject()->TryCData(XFA_Attribute::ErrorCorrectionLevel, true);
  if (!wsECLevel)
    return {};
  return {FXSYS_wtoi(wsECLevel->c_str())};
}

Optional<int32_t> CXFA_Barcode::GetModuleWidth() {
  Optional<CXFA_Measurement> moduleWidthHeight =
      JSObject()->TryMeasure(XFA_Attribute::ModuleWidth, true);
  if (!moduleWidthHeight)
    return {};

  return {static_cast<int32_t>(moduleWidthHeight->ToUnit(XFA_Unit::Pt))};
}

Optional<int32_t> CXFA_Barcode::GetModuleHeight() {
  Optional<CXFA_Measurement> moduleWidthHeight =
      JSObject()->TryMeasure(XFA_Attribute::ModuleHeight, true);
  if (!moduleWidthHeight)
    return {};

  return {static_cast<int32_t>(moduleWidthHeight->ToUnit(XFA_Unit::Pt))};
}

Optional<bool> CXFA_Barcode::GetPrintChecksum() {
  return JSObject()->TryBoolean(XFA_Attribute::PrintCheckDigit, true);
}

Optional<BC_TEXT_LOC> CXFA_Barcode::GetTextLocation() {
  Optional<XFA_AttributeValue> textLocation =
      JSObject()->TryEnum(XFA_Attribute::TextLocation, true);
  if (!textLocation)
    return {};

  switch (*textLocation) {
    case XFA_AttributeValue::None:
      return {BC_TEXT_LOC_NONE};
    case XFA_AttributeValue::Above:
      return {BC_TEXT_LOC_ABOVE};
    case XFA_AttributeValue::Below:
      return {BC_TEXT_LOC_BELOW};
    case XFA_AttributeValue::AboveEmbedded:
      return {BC_TEXT_LOC_ABOVEEMBED};
    case XFA_AttributeValue::BelowEmbedded:
      return {BC_TEXT_LOC_BELOWEMBED};
    default:
      break;
  }
  return {};
}

Optional<bool> CXFA_Barcode::GetTruncate() {
  return JSObject()->TryBoolean(XFA_Attribute::Truncate, true);
}

Optional<int8_t> CXFA_Barcode::GetWideNarrowRatio() {
  Optional<WideString> wsWideNarrowRatio =
      JSObject()->TryCData(XFA_Attribute::WideNarrowRatio, true);
  if (!wsWideNarrowRatio)
    return {};

  Optional<size_t> ptPos = wsWideNarrowRatio->Find(':');
  if (!ptPos)
    return {static_cast<int8_t>(FXSYS_wtoi(wsWideNarrowRatio->c_str()))};

  int32_t fB = FXSYS_wtoi(
      wsWideNarrowRatio->Right(wsWideNarrowRatio->GetLength() - (*ptPos + 1))
          .c_str());
  if (!fB)
    return {0};

  int32_t fA = FXSYS_wtoi(wsWideNarrowRatio->Left(*ptPos).c_str());
  float result = static_cast<float>(fA) / static_cast<float>(fB);
  return {static_cast<int8_t>(result)};
}
