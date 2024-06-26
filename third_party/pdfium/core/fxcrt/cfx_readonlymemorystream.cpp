// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/cfx_readonlymemorystream.h"

#include <utility>

#include "core/fxcrt/fx_safe_types.h"

CFX_ReadOnlyMemoryStream::CFX_ReadOnlyMemoryStream(
    std::unique_ptr<uint8_t, FxFreeDeleter> data,
    size_t size)
    : m_data(std::move(data)), m_span(m_data.get(), size) {}

CFX_ReadOnlyMemoryStream::CFX_ReadOnlyMemoryStream(
    pdfium::span<const uint8_t> span)
    : m_span(span) {}

CFX_ReadOnlyMemoryStream::~CFX_ReadOnlyMemoryStream() = default;

FX_FILESIZE CFX_ReadOnlyMemoryStream::GetSize() {
  return pdfium::base::checked_cast<FX_FILESIZE>(m_span.size());
}

bool CFX_ReadOnlyMemoryStream::ReadBlockAtOffset(void* buffer,
                                                 FX_FILESIZE offset,
                                                 size_t size) {
  if (!buffer || offset < 0 || size == 0)
    return false;

  FX_SAFE_SIZE_T pos = size;
  pos += offset;
  if (!pos.IsValid() || pos.ValueOrDie() > m_span.size())
    return false;

  auto copy_span = m_span.subspan(offset, size);
  memcpy(buffer, copy_span.data(), copy_span.size());
  return true;
}
