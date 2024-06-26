// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxbarcode/cbc_eancode.h"

#include <utility>

#include "fxbarcode/oned/BC_OnedEANWriter.h"

CBC_EANCode::CBC_EANCode(std::unique_ptr<CBC_OneDimEANWriter> pWriter)
    : CBC_OneCode(std::move(pWriter)) {}

CBC_EANCode::~CBC_EANCode() = default;

CBC_OneDimEANWriter* CBC_EANCode::GetOneDimEANWriter() {
  return static_cast<CBC_OneDimEANWriter*>(m_pBCWriter.get());
}

bool CBC_EANCode::Encode(WideStringView contents) {
  if (contents.IsEmpty())
    return false;

  BCFORMAT format = GetFormat();
  int32_t out_width = 0;
  int32_t out_height = 0;
  m_renderContents = Preprocess(contents);
  ByteString str = m_renderContents.ToUTF8();
  auto* pWriter = GetOneDimEANWriter();
  pWriter->InitEANWriter();
  std::unique_ptr<uint8_t, FxFreeDeleter> data(
      pWriter->Encode(str, format, out_width, out_height));
  return data && pWriter->RenderResult(m_renderContents.AsStringView(),
                                       data.get(), out_width);
}

bool CBC_EANCode::RenderDevice(CFX_RenderDevice* device,
                               const CFX_Matrix* matrix) {
  return GetOneDimEANWriter()->RenderDeviceResult(
      device, matrix, m_renderContents.AsStringView());
}

WideString CBC_EANCode::Preprocess(WideStringView contents) {
  auto* pWriter = GetOneDimEANWriter();
  WideString encoded_contents = pWriter->FilterContents(contents);
  size_t length = encoded_contents.GetLength();
  size_t max_length = GetMaxLength();
  if (length <= max_length) {
    for (size_t i = 0; i < max_length - length; i++)
      encoded_contents.InsertAtFront(L'0');

    ByteString str = encoded_contents.ToUTF8();
    int32_t checksum = pWriter->CalcChecksum(str);
    str += '0' + checksum;
    encoded_contents = WideString::FromUTF8(str.AsStringView());
  } else {
    encoded_contents = encoded_contents.Left(max_length + 1);
  }

  return encoded_contents;
}
