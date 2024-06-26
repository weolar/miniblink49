// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_streamparser.h"

#include <limits.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <utility>

#include "constants/stream_dict_common.h"
#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/page/cpdf_docpagedata.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_boolean.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_null.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"
#include "core/fpdfapi/parser/fpdf_parser_utility.h"
#include "core/fxcodec/codec/ccodec_jpegmodule.h"
#include "core/fxcodec/codec/ccodec_scanlinedecoder.h"
#include "core/fxcodec/fx_codec.h"
#include "core/fxcrt/fx_extension.h"
#include "third_party/base/ptr_util.h"

namespace {

const uint32_t kMaxNestedParsingLevel = 512;
const size_t kMaxStringLength = 32767;

uint32_t DecodeAllScanlines(std::unique_ptr<CCodec_ScanlineDecoder> pDecoder) {
  if (!pDecoder)
    return FX_INVALID_OFFSET;

  int ncomps = pDecoder->CountComps();
  int bpc = pDecoder->GetBPC();
  int width = pDecoder->GetWidth();
  int height = pDecoder->GetHeight();
  if (width <= 0 || height <= 0)
    return FX_INVALID_OFFSET;

  FX_SAFE_UINT32 size = CalculatePitch8(bpc, ncomps, width);
  size *= height;
  if (size.ValueOrDefault(0) == 0)
    return FX_INVALID_OFFSET;

  for (int row = 0; row < height; ++row) {
    if (!pDecoder->GetScanline(row))
      break;
  }
  return pDecoder->GetSrcOffset();
}

uint32_t DecodeInlineStream(pdfium::span<const uint8_t> src_span,
                            int width,
                            int height,
                            const ByteString& decoder,
                            CPDF_Dictionary* pParam,
                            uint32_t orig_size) {
  std::unique_ptr<uint8_t, FxFreeDeleter> ignored_result;
  uint32_t ignored_size;
  if (decoder == "FlateDecode" || decoder == "Fl") {
    return FlateOrLZWDecode(false, src_span, pParam, orig_size, &ignored_result,
                            &ignored_size);
  }
  if (decoder == "LZWDecode" || decoder == "LZW") {
    return FlateOrLZWDecode(true, src_span, pParam, 0, &ignored_result,
                            &ignored_size);
  }
  if (decoder == "DCTDecode" || decoder == "DCT") {
    std::unique_ptr<CCodec_ScanlineDecoder> pDecoder =
        CPDF_ModuleMgr::Get()->GetJpegModule()->CreateDecoder(
            src_span, width, height, 0,
            !pParam || pParam->GetIntegerFor("ColorTransform", 1));
    return DecodeAllScanlines(std::move(pDecoder));
  }
  if (decoder == "CCITTFaxDecode" || decoder == "CCF") {
    std::unique_ptr<CCodec_ScanlineDecoder> pDecoder =
        CreateFaxDecoder(src_span, width, height, pParam);
    return DecodeAllScanlines(std::move(pDecoder));
  }

  if (decoder == "ASCII85Decode" || decoder == "A85")
    return A85Decode(src_span, &ignored_result, &ignored_size);
  if (decoder == "ASCIIHexDecode" || decoder == "AHx")
    return HexDecode(src_span, &ignored_result, &ignored_size);
  if (decoder == "RunLengthDecode" || decoder == "RL")
    return RunLengthDecode(src_span, &ignored_result, &ignored_size);

  return FX_INVALID_OFFSET;
}

}  // namespace

CPDF_StreamParser::CPDF_StreamParser(pdfium::span<const uint8_t> span)
    : m_pBuf(span) {}

CPDF_StreamParser::CPDF_StreamParser(pdfium::span<const uint8_t> span,
                                     const WeakPtr<ByteStringPool>& pPool)
    : m_pPool(pPool), m_pBuf(span) {}

CPDF_StreamParser::~CPDF_StreamParser() {}

std::unique_ptr<CPDF_Stream> CPDF_StreamParser::ReadInlineStream(
    CPDF_Document* pDoc,
    std::unique_ptr<CPDF_Dictionary> pDict,
    const CPDF_Object* pCSObj) {
  if (m_Pos < m_pBuf.size() && PDFCharIsWhitespace(m_pBuf[m_Pos]))
    m_Pos++;

  if (m_Pos == m_pBuf.size())
    return nullptr;

  ByteString Decoder;
  CPDF_Dictionary* pParam = nullptr;
  CPDF_Object* pFilter = pDict->GetDirectObjectFor("Filter");
  if (pFilter) {
    if (CPDF_Array* pArray = pFilter->AsArray()) {
      Decoder = pArray->GetStringAt(0);
      CPDF_Array* pParams = pDict->GetArrayFor(pdfium::stream::kDecodeParms);
      if (pParams)
        pParam = pParams->GetDictAt(0);
    } else {
      Decoder = pFilter->GetString();
      pParam = pDict->GetDictFor(pdfium::stream::kDecodeParms);
    }
  }
  uint32_t width = pDict->GetIntegerFor("Width");
  uint32_t height = pDict->GetIntegerFor("Height");
  uint32_t OrigSize = 0;
  if (pCSObj) {
    uint32_t bpc = pDict->GetIntegerFor("BitsPerComponent");
    uint32_t nComponents = 1;
    CPDF_ColorSpace* pCS = pDoc->LoadColorSpace(pCSObj, nullptr);
    if (pCS) {
      nComponents = pCS->CountComponents();
      pDoc->GetPageData()->ReleaseColorSpace(pCSObj);
    } else {
      nComponents = 3;
    }
    uint32_t pitch = width;
    if (bpc && pitch > INT_MAX / bpc)
      return nullptr;

    pitch *= bpc;
    if (nComponents && pitch > INT_MAX / nComponents)
      return nullptr;

    pitch *= nComponents;
    if (pitch > INT_MAX - 7)
      return nullptr;

    pitch += 7;
    pitch /= 8;
    OrigSize = pitch;
  } else {
    if (width > INT_MAX - 7)
      return nullptr;

    OrigSize = ((width + 7) / 8);
  }
  if (height && OrigSize > INT_MAX / height)
    return nullptr;

  OrigSize *= height;
  std::unique_ptr<uint8_t, FxFreeDeleter> pData;
  uint32_t dwStreamSize;
  if (Decoder.IsEmpty()) {
    if (OrigSize > m_pBuf.size() - m_Pos)
      OrigSize = m_pBuf.size() - m_Pos;
    pData.reset(FX_Alloc(uint8_t, OrigSize));
    auto copy_span = m_pBuf.subspan(m_Pos, OrigSize);
    memcpy(pData.get(), copy_span.data(), copy_span.size());
    dwStreamSize = OrigSize;
    m_Pos += OrigSize;
  } else {
    dwStreamSize = DecodeInlineStream(m_pBuf.subspan(m_Pos), width, height,
                                      Decoder, pParam, OrigSize);
    if (static_cast<int>(dwStreamSize) < 0)
      return nullptr;

    uint32_t dwSavePos = m_Pos;
    m_Pos += dwStreamSize;
    while (1) {
      uint32_t dwPrevPos = m_Pos;
      CPDF_StreamParser::SyntaxType type = ParseNextElement();
      if (type == CPDF_StreamParser::EndOfData)
        break;

      if (type != CPDF_StreamParser::Keyword) {
        dwStreamSize += m_Pos - dwPrevPos;
        continue;
      }
      if (GetWord() == "EI") {
        m_Pos = dwPrevPos;
        break;
      }
      dwStreamSize += m_Pos - dwPrevPos;
    }
    m_Pos = dwSavePos;
    pData.reset(FX_Alloc(uint8_t, dwStreamSize));
    auto copy_span = m_pBuf.subspan(m_Pos, dwStreamSize);
    memcpy(pData.get(), copy_span.data(), copy_span.size());
    m_Pos += dwStreamSize;
  }
  pDict->SetNewFor<CPDF_Number>("Length", static_cast<int>(dwStreamSize));
  return pdfium::MakeUnique<CPDF_Stream>(std::move(pData), dwStreamSize,
                                         std::move(pDict));
}

CPDF_StreamParser::SyntaxType CPDF_StreamParser::ParseNextElement() {
  m_pLastObj.reset();
  m_WordSize = 0;
  if (!PositionIsInBounds())
    return EndOfData;

  int ch = m_pBuf[m_Pos++];
  while (1) {
    while (PDFCharIsWhitespace(ch)) {
      if (!PositionIsInBounds())
        return EndOfData;

      ch = m_pBuf[m_Pos++];
    }

    if (ch != '%')
      break;

    while (1) {
      if (!PositionIsInBounds())
        return EndOfData;

      ch = m_pBuf[m_Pos++];
      if (PDFCharIsLineEnding(ch))
        break;
    }
  }

  if (PDFCharIsDelimiter(ch) && ch != '/') {
    m_Pos--;
    m_pLastObj = ReadNextObject(false, false, 0);
    return Others;
  }

  bool bIsNumber = true;
  while (1) {
    if (m_WordSize < kMaxWordLength)
      m_WordBuffer[m_WordSize++] = ch;

    if (!PDFCharIsNumeric(ch))
      bIsNumber = false;

    if (!PositionIsInBounds())
      break;

    ch = m_pBuf[m_Pos++];

    if (PDFCharIsDelimiter(ch) || PDFCharIsWhitespace(ch)) {
      m_Pos--;
      break;
    }
  }

  m_WordBuffer[m_WordSize] = 0;
  if (bIsNumber)
    return Number;

  if (m_WordBuffer[0] == '/')
    return Name;

  if (m_WordSize == 4) {
    if (memcmp(m_WordBuffer, "true", 4) == 0) {
      m_pLastObj = pdfium::MakeUnique<CPDF_Boolean>(true);
      return Others;
    }
    if (memcmp(m_WordBuffer, "null", 4) == 0) {
      m_pLastObj = pdfium::MakeUnique<CPDF_Null>();
      return Others;
    }
  } else if (m_WordSize == 5) {
    if (memcmp(m_WordBuffer, "false", 5) == 0) {
      m_pLastObj = pdfium::MakeUnique<CPDF_Boolean>(false);
      return Others;
    }
  }
  return Keyword;
}

std::unique_ptr<CPDF_Object> CPDF_StreamParser::ReadNextObject(
    bool bAllowNestedArray,
    bool bInArray,
    uint32_t dwRecursionLevel) {
  bool bIsNumber;
  // Must get the next word before returning to avoid infinite loops.
  GetNextWord(bIsNumber);
  if (!m_WordSize || dwRecursionLevel > kMaxNestedParsingLevel)
    return nullptr;

  if (bIsNumber) {
    m_WordBuffer[m_WordSize] = 0;
    return pdfium::MakeUnique<CPDF_Number>(
        ByteStringView(m_WordBuffer, m_WordSize));
  }

  int first_char = m_WordBuffer[0];
  if (first_char == '/') {
    ByteString name =
        PDF_NameDecode(ByteStringView(m_WordBuffer + 1, m_WordSize - 1));
    return pdfium::MakeUnique<CPDF_Name>(m_pPool, name);
  }

  if (first_char == '(') {
    ByteString str = ReadString();
    return pdfium::MakeUnique<CPDF_String>(m_pPool, str, false);
  }

  if (first_char == '<') {
    if (m_WordSize == 1)
      return pdfium::MakeUnique<CPDF_String>(m_pPool, ReadHexString(), true);

    auto pDict = pdfium::MakeUnique<CPDF_Dictionary>(m_pPool);
    while (1) {
      GetNextWord(bIsNumber);
      if (m_WordSize == 2 && m_WordBuffer[0] == '>')
        break;

      if (!m_WordSize || m_WordBuffer[0] != '/')
        return nullptr;

      ByteString key =
          PDF_NameDecode(ByteStringView(m_WordBuffer + 1, m_WordSize - 1));
      std::unique_ptr<CPDF_Object> pObj =
          ReadNextObject(true, bInArray, dwRecursionLevel + 1);
      if (!pObj)
        return nullptr;

      if (!key.IsEmpty())
        pDict->SetFor(key, std::move(pObj));
    }
    return std::move(pDict);
  }

  if (first_char == '[') {
    if ((!bAllowNestedArray && bInArray))
      return nullptr;

    auto pArray = pdfium::MakeUnique<CPDF_Array>();
    while (1) {
      std::unique_ptr<CPDF_Object> pObj =
          ReadNextObject(bAllowNestedArray, true, dwRecursionLevel + 1);
      if (pObj) {
        pArray->Add(std::move(pObj));
        continue;
      }
      if (!m_WordSize || m_WordBuffer[0] == ']')
        break;
    }
    return std::move(pArray);
  }

  if (m_WordSize == 5 && !memcmp(m_WordBuffer, "false", 5))
    return pdfium::MakeUnique<CPDF_Boolean>(false);

  if (m_WordSize == 4) {
    if (memcmp(m_WordBuffer, "true", 4) == 0)
      return pdfium::MakeUnique<CPDF_Boolean>(true);
    if (memcmp(m_WordBuffer, "null", 4) == 0)
      return pdfium::MakeUnique<CPDF_Null>();
  }

  return nullptr;
}

// TODO(npm): the following methods are almost identical in cpdf_syntaxparser
void CPDF_StreamParser::GetNextWord(bool& bIsNumber) {
  m_WordSize = 0;
  bIsNumber = true;
  if (!PositionIsInBounds())
    return;

  int ch = m_pBuf[m_Pos++];
  while (1) {
    while (PDFCharIsWhitespace(ch)) {
      if (!PositionIsInBounds()) {
        return;
      }
      ch = m_pBuf[m_Pos++];
    }

    if (ch != '%')
      break;

    while (1) {
      if (!PositionIsInBounds())
        return;
      ch = m_pBuf[m_Pos++];
      if (PDFCharIsLineEnding(ch))
        break;
    }
  }

  if (PDFCharIsDelimiter(ch)) {
    bIsNumber = false;
    m_WordBuffer[m_WordSize++] = ch;
    if (ch == '/') {
      while (1) {
        if (!PositionIsInBounds())
          return;
        ch = m_pBuf[m_Pos++];
        if (!PDFCharIsOther(ch) && !PDFCharIsNumeric(ch)) {
          m_Pos--;
          return;
        }
        if (m_WordSize < kMaxWordLength)
          m_WordBuffer[m_WordSize++] = ch;
      }
    } else if (ch == '<') {
      if (!PositionIsInBounds())
        return;
      ch = m_pBuf[m_Pos++];
      if (ch == '<')
        m_WordBuffer[m_WordSize++] = ch;
      else
        m_Pos--;
    } else if (ch == '>') {
      if (!PositionIsInBounds())
        return;
      ch = m_pBuf[m_Pos++];
      if (ch == '>')
        m_WordBuffer[m_WordSize++] = ch;
      else
        m_Pos--;
    }
    return;
  }

  while (1) {
    if (m_WordSize < kMaxWordLength)
      m_WordBuffer[m_WordSize++] = ch;
    if (!PDFCharIsNumeric(ch))
      bIsNumber = false;
    if (!PositionIsInBounds())
      return;

    ch = m_pBuf[m_Pos++];
    if (PDFCharIsDelimiter(ch) || PDFCharIsWhitespace(ch)) {
      m_Pos--;
      break;
    }
  }
}

ByteString CPDF_StreamParser::ReadString() {
  if (!PositionIsInBounds())
    return ByteString();

  uint8_t ch = m_pBuf[m_Pos++];
  std::ostringstream buf;
  int parlevel = 0;
  int status = 0;
  int iEscCode = 0;
  while (1) {
    switch (status) {
      case 0:
        if (ch == ')') {
          if (parlevel == 0) {
            if (buf.tellp() <= 0)
              return ByteString();

            return ByteString(
                buf.str().c_str(),
                std::min(static_cast<size_t>(buf.tellp()), kMaxStringLength));
          }
          parlevel--;
          buf << ')';
        } else if (ch == '(') {
          parlevel++;
          buf << '(';
        } else if (ch == '\\') {
          status = 1;
        } else {
          buf << static_cast<char>(ch);
        }
        break;
      case 1:
        if (FXSYS_IsOctalDigit(ch)) {
          iEscCode = FXSYS_DecimalCharToInt(static_cast<char>(ch));
          status = 2;
          break;
        }
        if (ch == '\r') {
          status = 4;
          break;
        }
        if (ch == '\n') {
          // Do nothing.
        } else if (ch == 'n') {
          buf << '\n';
        } else if (ch == 'r') {
          buf << '\r';
        } else if (ch == 't') {
          buf << '\t';
        } else if (ch == 'b') {
          buf << '\b';
        } else if (ch == 'f') {
          buf << '\f';
        } else {
          buf << static_cast<char>(ch);
        }
        status = 0;
        break;
      case 2:
        if (FXSYS_IsOctalDigit(ch)) {
          iEscCode =
              iEscCode * 8 + FXSYS_DecimalCharToInt(static_cast<char>(ch));
          status = 3;
        } else {
          buf << static_cast<char>(iEscCode);
          status = 0;
          continue;
        }
        break;
      case 3:
        if (FXSYS_IsOctalDigit(ch)) {
          iEscCode =
              iEscCode * 8 + FXSYS_DecimalCharToInt(static_cast<char>(ch));
          buf << static_cast<char>(iEscCode);
          status = 0;
        } else {
          buf << static_cast<char>(iEscCode);
          status = 0;
          continue;
        }
        break;
      case 4:
        status = 0;
        if (ch != '\n')
          continue;
        break;
    }
    if (!PositionIsInBounds())
      break;

    ch = m_pBuf[m_Pos++];
  }
  if (PositionIsInBounds())
    ++m_Pos;

  if (buf.tellp() <= 0)
    return ByteString();

  return ByteString(
      buf.str().c_str(),
      std::min(static_cast<size_t>(buf.tellp()), kMaxStringLength));
}

ByteString CPDF_StreamParser::ReadHexString() {
  if (!PositionIsInBounds())
    return ByteString();

  std::ostringstream buf;
  bool bFirst = true;
  int code = 0;
  while (PositionIsInBounds()) {
    int ch = m_pBuf[m_Pos++];

    if (ch == '>')
      break;

    if (!std::isxdigit(ch))
      continue;

    int val = FXSYS_HexCharToInt(ch);
    if (bFirst) {
      code = val * 16;
    } else {
      code += val;
      buf << static_cast<uint8_t>(code);
    }
    bFirst = !bFirst;
  }
  if (!bFirst)
    buf << static_cast<char>(code);

  if (buf.tellp() <= 0)
    return ByteString();

  return ByteString(
      buf.str().c_str(),
      std::min(static_cast<size_t>(buf.tellp()), kMaxStringLength));
}

bool CPDF_StreamParser::PositionIsInBounds() const {
  return m_Pos < m_pBuf.size();
}
