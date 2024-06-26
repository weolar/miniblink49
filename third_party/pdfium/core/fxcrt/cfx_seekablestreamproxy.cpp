// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/cfx_seekablestreamproxy.h"

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
#include <io.h>
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

#include <algorithm>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/fx_extension.h"
#include "third_party/base/stl_util.h"

namespace {

// Returns {src bytes consumed, dst bytes produced}.
std::pair<size_t, size_t> UTF8Decode(const char* pSrc,
                                     size_t srcLen,
                                     wchar_t* pDst,
                                     size_t dstLen) {
  ASSERT(pDst);
  ASSERT(dstLen > 0);

  if (srcLen < 1)
    return {0, 0};

  uint32_t dwCode = 0;
  int32_t iPending = 0;
  size_t iSrcNum = 0;
  size_t iDstNum = 0;
  size_t iIndex = 0;
  int32_t k = 1;
  while (iIndex < srcLen) {
    uint8_t byte = static_cast<uint8_t>(*(pSrc + iIndex));
    if (byte < 0x80) {
      iPending = 0;
      k = 1;
      iDstNum++;
      iSrcNum += k;
      *pDst++ = byte;
      if (iDstNum >= dstLen)
        break;
    } else if (byte < 0xc0) {
      if (iPending < 1)
        break;

      iPending--;
      dwCode |= (byte & 0x3f) << (iPending * 6);
      if (iPending == 0) {
        iDstNum++;
        iSrcNum += k;
        *pDst++ = dwCode;
        if (iDstNum >= dstLen)
          break;
      }
    } else if (byte < 0xe0) {
      iPending = 1;
      k = 2;
      dwCode = (byte & 0x1f) << 6;
    } else if (byte < 0xf0) {
      iPending = 2;
      k = 3;
      dwCode = (byte & 0x0f) << 12;
    } else if (byte < 0xf8) {
      iPending = 3;
      k = 4;
      dwCode = (byte & 0x07) << 18;
    } else if (byte < 0xfc) {
      iPending = 4;
      k = 5;
      dwCode = (byte & 0x03) << 24;
    } else if (byte < 0xfe) {
      iPending = 5;
      k = 6;
      dwCode = (byte & 0x01) << 30;
    } else {
      break;
    }
    iIndex++;
  }
  return {iSrcNum, iDstNum};
}

void UTF16ToWChar(void* pBuffer, size_t iLength) {
  ASSERT(pBuffer);
  ASSERT(iLength > 0);
  ASSERT(sizeof(wchar_t) > 2);

  uint16_t* pSrc = static_cast<uint16_t*>(pBuffer);
  wchar_t* pDst = static_cast<wchar_t*>(pBuffer);
  for (size_t i = 0; i < iLength; i++)
    pDst[i] = static_cast<wchar_t>(pSrc[i]);
}

void SwapByteOrder(wchar_t* pStr, size_t iLength) {
  ASSERT(pStr);

  uint16_t wch;
  if (sizeof(wchar_t) > 2) {
    while (iLength-- > 0) {
      wch = static_cast<uint16_t>(*pStr);
      wch = (wch >> 8) | (wch << 8);
      wch &= 0x00FF;
      *pStr = wch;
      ++pStr;
    }
    return;
  }

  while (iLength-- > 0) {
    wch = static_cast<uint16_t>(*pStr);
    wch = (wch >> 8) | (wch << 8);
    *pStr = wch;
    ++pStr;
  }
}

}  // namespace

#define BOM_MASK 0x00FFFFFF
#define BOM_UTF8 0x00BFBBEF
#define BOM_UTF16_MASK 0x0000FFFF
#define BOM_UTF16_BE 0x0000FFFE
#define BOM_UTF16_LE 0x0000FEFF

CFX_SeekableStreamProxy::CFX_SeekableStreamProxy(
    const RetainPtr<IFX_SeekableReadStream>& stream)
    : m_wCodePage(FX_CODEPAGE_DefANSI),
      m_wBOMLength(0),
      m_iPosition(0),
      m_pStream(stream) {
  ASSERT(m_pStream);

  Seek(From::Begin, 0);

  uint32_t bom = 0;
  ReadData(reinterpret_cast<uint8_t*>(&bom), 3);

  bom &= BOM_MASK;
  if (bom == BOM_UTF8) {
    m_wBOMLength = 3;
    m_wCodePage = FX_CODEPAGE_UTF8;
  } else {
    bom &= BOM_UTF16_MASK;
    if (bom == BOM_UTF16_BE) {
      m_wBOMLength = 2;
      m_wCodePage = FX_CODEPAGE_UTF16BE;
    } else if (bom == BOM_UTF16_LE) {
      m_wBOMLength = 2;
      m_wCodePage = FX_CODEPAGE_UTF16LE;
    } else {
      m_wBOMLength = 0;
      m_wCodePage = FXSYS_GetACP();
    }
  }

  Seek(From::Begin, static_cast<FX_FILESIZE>(m_wBOMLength));
}

CFX_SeekableStreamProxy::~CFX_SeekableStreamProxy() = default;

FX_FILESIZE CFX_SeekableStreamProxy::GetSize() {
  return m_pStream->GetSize();
}

FX_FILESIZE CFX_SeekableStreamProxy::GetPosition() {
  return m_iPosition;
}

bool CFX_SeekableStreamProxy::IsEOF() {
  return m_iPosition >= GetSize();
}

void CFX_SeekableStreamProxy::Seek(From eSeek, FX_FILESIZE iOffset) {
  switch (eSeek) {
    case From::Begin:
      m_iPosition = iOffset;
      break;
    case From::Current: {
      pdfium::base::CheckedNumeric<FX_FILESIZE> new_pos = m_iPosition;
      new_pos += iOffset;
      m_iPosition =
          new_pos.ValueOrDefault(std::numeric_limits<FX_FILESIZE>::max());
    } break;
  }
  m_iPosition =
      pdfium::clamp(m_iPosition, static_cast<FX_FILESIZE>(0), GetSize());
}

void CFX_SeekableStreamProxy::SetCodePage(uint16_t wCodePage) {
  if (m_wBOMLength > 0)
    return;
  m_wCodePage = wCodePage;
}

size_t CFX_SeekableStreamProxy::ReadData(uint8_t* pBuffer, size_t iBufferSize) {
  ASSERT(pBuffer);
  ASSERT(iBufferSize > 0);

  iBufferSize =
      std::min(iBufferSize, static_cast<size_t>(GetSize() - m_iPosition));
  if (iBufferSize <= 0)
    return 0;

  if (!m_pStream->ReadBlockAtOffset(pBuffer, m_iPosition, iBufferSize))
    return 0;

  pdfium::base::CheckedNumeric<FX_FILESIZE> new_pos = m_iPosition;
  new_pos += iBufferSize;
  m_iPosition = new_pos.ValueOrDefault(m_iPosition);
  return new_pos.IsValid() ? iBufferSize : 0;
}

size_t CFX_SeekableStreamProxy::ReadBlock(void* pStr, size_t size) {
  if (!pStr || size == 0)
    return 0;

  if (m_wCodePage == FX_CODEPAGE_UTF16LE ||
      m_wCodePage == FX_CODEPAGE_UTF16BE) {
    size_t iBytes = size * 2;
    size_t iLen = ReadData(reinterpret_cast<uint8_t*>(pStr), iBytes);
    size = iLen / 2;
    if (sizeof(wchar_t) > 2 && size > 0)
      UTF16ToWChar(pStr, size);

    if (m_wCodePage == FX_CODEPAGE_UTF16BE)
      SwapByteOrder(static_cast<wchar_t*>(pStr), size);

  } else {
    FX_FILESIZE pos = GetPosition();
    size_t iBytes = std::min(size, static_cast<size_t>(GetSize() - pos));

    if (iBytes > 0) {
      std::vector<uint8_t> buf(iBytes);

      size_t iLen = ReadData(buf.data(), iBytes);
      if (m_wCodePage != FX_CODEPAGE_UTF8)
        return 0;

      size_t iSrc = 0;
      std::tie(iSrc, size) =
          UTF8Decode(reinterpret_cast<const char*>(buf.data()), iLen,
                     static_cast<wchar_t*>(pStr), size);
      Seek(From::Current, iSrc - iLen);
    } else {
      size = 0;
    }
  }

  return size;
}

bool CFX_SeekableStreamProxy::ReadBlockAtOffset(void* pStr,
                                                FX_FILESIZE offset,
                                                size_t size) {
  NOTREACHED();
  return false;
}
