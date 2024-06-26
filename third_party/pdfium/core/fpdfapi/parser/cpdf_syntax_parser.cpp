// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_syntax_parser.h"

#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_boolean.h"
#include "core/fpdfapi/parser/cpdf_crypto_handler.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_null.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_read_validator.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/parser/fpdf_parser_utility.h"
#include "core/fxcrt/autorestorer.h"
#include "core/fxcrt/cfx_binarybuf.h"
#include "core/fxcrt/fx_extension.h"
#include "third_party/base/numerics/safe_math.h"
#include "third_party/base/ptr_util.h"

namespace {

enum class ReadStatus { Normal, Backslash, Octal, FinishOctal, CarriageReturn };

class ReadableSubStream final : public IFX_SeekableReadStream {
 public:
  ReadableSubStream(const RetainPtr<IFX_SeekableReadStream>& pFileRead,
                    FX_FILESIZE part_offset,
                    FX_FILESIZE part_size)
      : m_pFileRead(pFileRead),
        m_PartOffset(part_offset),
        m_PartSize(part_size) {}

  ~ReadableSubStream() override = default;

  // IFX_SeekableReadStream overrides:
  bool ReadBlockAtOffset(void* buffer,
                         FX_FILESIZE offset,
                         size_t size) override {
    FX_SAFE_FILESIZE safe_end = offset;
    safe_end += size;
    // Check that requested range is valid, to prevent calling of ReadBlock
    // of original m_pFileRead with incorrect params.
    if (!safe_end.IsValid() || safe_end.ValueOrDie() > m_PartSize)
      return false;

    return m_pFileRead->ReadBlockAtOffset(buffer, m_PartOffset + offset, size);
  }

  FX_FILESIZE GetSize() override { return m_PartSize; }

 private:
  RetainPtr<IFX_SeekableReadStream> m_pFileRead;
  FX_FILESIZE m_PartOffset;
  FX_FILESIZE m_PartSize;
};

}  // namespace

// static
int CPDF_SyntaxParser::s_CurrentRecursionDepth = 0;

// static
std::unique_ptr<CPDF_SyntaxParser> CPDF_SyntaxParser::CreateForTesting(
    const RetainPtr<IFX_SeekableReadStream>& pFileAccess,
    FX_FILESIZE HeaderOffset) {
  return pdfium::MakeUnique<CPDF_SyntaxParser>(
      pdfium::MakeRetain<CPDF_ReadValidator>(pFileAccess, nullptr),
      HeaderOffset);
}

CPDF_SyntaxParser::CPDF_SyntaxParser(
    const RetainPtr<IFX_SeekableReadStream>& pFileAccess)
    : CPDF_SyntaxParser(
          pdfium::MakeRetain<CPDF_ReadValidator>(pFileAccess, nullptr),
          0) {}

CPDF_SyntaxParser::CPDF_SyntaxParser(
    const RetainPtr<CPDF_ReadValidator>& validator,
    FX_FILESIZE HeaderOffset)
    : m_pFileAccess(validator),
      m_HeaderOffset(HeaderOffset),
      m_FileLen(m_pFileAccess->GetSize()) {
  ASSERT(m_HeaderOffset <= m_FileLen);
}

CPDF_SyntaxParser::~CPDF_SyntaxParser() = default;

bool CPDF_SyntaxParser::GetCharAt(FX_FILESIZE pos, uint8_t& ch) {
  AutoRestorer<FX_FILESIZE> save_pos(&m_Pos);
  m_Pos = pos;
  return GetNextChar(ch);
}

bool CPDF_SyntaxParser::ReadBlockAt(FX_FILESIZE read_pos) {
  if (read_pos >= m_FileLen)
    return false;
  size_t read_size = m_ReadBufferSize;
  FX_SAFE_FILESIZE safe_end = read_pos;
  safe_end += read_size;
  if (!safe_end.IsValid() || safe_end.ValueOrDie() > m_FileLen)
    read_size = m_FileLen - read_pos;

  m_pFileBuf.resize(read_size);
  if (!m_pFileAccess->ReadBlockAtOffset(m_pFileBuf.data(), read_pos,
                                        read_size)) {
    m_pFileBuf.clear();
    return false;
  }

  m_BufOffset = read_pos;
  return true;
}

bool CPDF_SyntaxParser::GetNextChar(uint8_t& ch) {
  FX_FILESIZE pos = m_Pos + m_HeaderOffset;
  if (pos >= m_FileLen)
    return false;

  if (!IsPositionRead(pos) && !ReadBlockAt(pos))
    return false;

  ch = m_pFileBuf[pos - m_BufOffset];
  m_Pos++;
  return true;
}

FX_FILESIZE CPDF_SyntaxParser::GetDocumentSize() const {
  return m_FileLen - m_HeaderOffset;
}

bool CPDF_SyntaxParser::GetCharAtBackward(FX_FILESIZE pos, uint8_t* ch) {
  pos += m_HeaderOffset;
  if (pos >= m_FileLen)
    return false;

  if (!IsPositionRead(pos)) {
    FX_FILESIZE block_start = 0;
    if (pos >= CPDF_ModuleMgr::kFileBufSize)
      block_start = pos - CPDF_ModuleMgr::kFileBufSize + 1;
    if (!ReadBlockAt(block_start) || !IsPositionRead(pos))
      return false;
  }
  *ch = m_pFileBuf[pos - m_BufOffset];
  return true;
}

bool CPDF_SyntaxParser::ReadBlock(uint8_t* pBuf, uint32_t size) {
  if (!m_pFileAccess->ReadBlockAtOffset(pBuf, m_Pos + m_HeaderOffset, size))
    return false;
  m_Pos += size;
  return true;
}

void CPDF_SyntaxParser::GetNextWordInternal(bool* bIsNumber) {
  m_WordSize = 0;
  if (bIsNumber)
    *bIsNumber = true;

  ToNextWord();
  uint8_t ch;
  if (!GetNextChar(ch))
    return;

  if (PDFCharIsDelimiter(ch)) {
    if (bIsNumber)
      *bIsNumber = false;

    m_WordBuffer[m_WordSize++] = ch;
    if (ch == '/') {
      while (1) {
        if (!GetNextChar(ch))
          return;

        if (!PDFCharIsOther(ch) && !PDFCharIsNumeric(ch)) {
          m_Pos--;
          return;
        }

        if (m_WordSize < sizeof(m_WordBuffer) - 1)
          m_WordBuffer[m_WordSize++] = ch;
      }
    } else if (ch == '<') {
      if (!GetNextChar(ch))
        return;

      if (ch == '<')
        m_WordBuffer[m_WordSize++] = ch;
      else
        m_Pos--;
    } else if (ch == '>') {
      if (!GetNextChar(ch))
        return;

      if (ch == '>')
        m_WordBuffer[m_WordSize++] = ch;
      else
        m_Pos--;
    }
    return;
  }

  while (1) {
    if (m_WordSize < sizeof(m_WordBuffer) - 1)
      m_WordBuffer[m_WordSize++] = ch;

    if (!PDFCharIsNumeric(ch)) {
      if (bIsNumber)
        *bIsNumber = false;
    }

    if (!GetNextChar(ch))
      return;

    if (PDFCharIsDelimiter(ch) || PDFCharIsWhitespace(ch)) {
      m_Pos--;
      break;
    }
  }
}

ByteString CPDF_SyntaxParser::ReadString() {
  uint8_t ch;
  if (!GetNextChar(ch))
    return ByteString();

  std::ostringstream buf;
  int32_t parlevel = 0;
  ReadStatus status = ReadStatus::Normal;
  int32_t iEscCode = 0;
  while (1) {
    switch (status) {
      case ReadStatus::Normal:
        if (ch == ')') {
          if (parlevel == 0)
            return ByteString(buf);
          parlevel--;
        } else if (ch == '(') {
          parlevel++;
        }
        if (ch == '\\')
          status = ReadStatus::Backslash;
        else
          buf << static_cast<char>(ch);
        break;
      case ReadStatus::Backslash:
        if (FXSYS_IsOctalDigit(ch)) {
          iEscCode = FXSYS_DecimalCharToInt(static_cast<wchar_t>(ch));
          status = ReadStatus::Octal;
          break;
        }

        if (ch == '\r') {
          status = ReadStatus::CarriageReturn;
          break;
        }
        if (ch == 'n') {
          buf << '\n';
        } else if (ch == 'r') {
          buf << '\r';
        } else if (ch == 't') {
          buf << '\t';
        } else if (ch == 'b') {
          buf << '\b';
        } else if (ch == 'f') {
          buf << '\f';
        } else if (ch != '\n') {
          buf << static_cast<char>(ch);
        }
        status = ReadStatus::Normal;
        break;
      case ReadStatus::Octal:
        if (FXSYS_IsOctalDigit(ch)) {
          iEscCode =
              iEscCode * 8 + FXSYS_DecimalCharToInt(static_cast<wchar_t>(ch));
          status = ReadStatus::FinishOctal;
        } else {
          buf << static_cast<char>(iEscCode);
          status = ReadStatus::Normal;
          continue;
        }
        break;
      case ReadStatus::FinishOctal:
        status = ReadStatus::Normal;
        if (FXSYS_IsOctalDigit(ch)) {
          iEscCode =
              iEscCode * 8 + FXSYS_DecimalCharToInt(static_cast<wchar_t>(ch));
          buf << static_cast<char>(iEscCode);
        } else {
          buf << static_cast<char>(iEscCode);
          continue;
        }
        break;
      case ReadStatus::CarriageReturn:
        status = ReadStatus::Normal;
        if (ch != '\n')
          continue;
        break;
    }

    if (!GetNextChar(ch))
      break;
  }

  GetNextChar(ch);
  return ByteString(buf);
}

ByteString CPDF_SyntaxParser::ReadHexString() {
  uint8_t ch;
  if (!GetNextChar(ch))
    return ByteString();

  std::ostringstream buf;
  bool bFirst = true;
  uint8_t code = 0;
  while (1) {
    if (ch == '>')
      break;

    if (std::isxdigit(ch)) {
      int val = FXSYS_HexCharToInt(ch);
      if (bFirst) {
        code = val * 16;
      } else {
        code += val;
        buf << static_cast<char>(code);
      }
      bFirst = !bFirst;
    }

    if (!GetNextChar(ch))
      break;
  }
  if (!bFirst)
    buf << static_cast<char>(code);

  return ByteString(buf);
}

void CPDF_SyntaxParser::ToNextLine() {
  uint8_t ch;
  while (GetNextChar(ch)) {
    if (ch == '\n')
      break;

    if (ch == '\r') {
      GetNextChar(ch);
      if (ch != '\n')
        --m_Pos;
      break;
    }
  }
}

void CPDF_SyntaxParser::ToNextWord() {
  uint8_t ch;
  if (!GetNextChar(ch))
    return;

  while (1) {
    while (PDFCharIsWhitespace(ch)) {
      if (!GetNextChar(ch))
        return;
    }

    if (ch != '%')
      break;

    while (1) {
      if (!GetNextChar(ch))
        return;
      if (PDFCharIsLineEnding(ch))
        break;
    }
  }
  m_Pos--;
}

ByteString CPDF_SyntaxParser::GetNextWord(bool* bIsNumber) {
  const CPDF_ReadValidator::Session read_session(GetValidator().Get());
  GetNextWordInternal(bIsNumber);
  ByteString ret;
  if (!GetValidator()->has_read_problems())
    ret = ByteString(m_WordBuffer, m_WordSize);
  return ret;
}

ByteString CPDF_SyntaxParser::PeekNextWord(bool* bIsNumber) {
  AutoRestorer<FX_FILESIZE> save_pos(&m_Pos);
  return GetNextWord(bIsNumber);
}

ByteString CPDF_SyntaxParser::GetKeyword() {
  return GetNextWord(nullptr);
}

void CPDF_SyntaxParser::SetPos(FX_FILESIZE pos) {
  m_Pos = std::min(pos, m_FileLen);
}

std::unique_ptr<CPDF_Object> CPDF_SyntaxParser::GetObjectBody(
    CPDF_IndirectObjectHolder* pObjList) {
  const CPDF_ReadValidator::Session read_session(GetValidator().Get());
  auto result = GetObjectBodyInternal(pObjList, ParseType::kLoose);
  if (GetValidator()->has_read_problems())
    return nullptr;
  return result;
}

std::unique_ptr<CPDF_Object> CPDF_SyntaxParser::GetObjectBodyInternal(
    CPDF_IndirectObjectHolder* pObjList,
    ParseType parse_type) {
  AutoRestorer<int> depth_restorer(&s_CurrentRecursionDepth);
  if (++s_CurrentRecursionDepth > kParserMaxRecursionDepth)
    return nullptr;

  FX_FILESIZE SavedObjPos = m_Pos;
  bool bIsNumber;
  ByteString word = GetNextWord(&bIsNumber);
  if (word.GetLength() == 0)
    return nullptr;

  if (bIsNumber) {
    AutoRestorer<FX_FILESIZE> pos_restorer(&m_Pos);
    ByteString nextword = GetNextWord(&bIsNumber);
    if (!bIsNumber)
      return pdfium::MakeUnique<CPDF_Number>(word.AsStringView());

    ByteString nextword2 = GetNextWord(nullptr);
    if (nextword2 != "R")
      return pdfium::MakeUnique<CPDF_Number>(word.AsStringView());

    pos_restorer.AbandonRestoration();
    uint32_t refnum = FXSYS_atoui(word.c_str());
    if (refnum == CPDF_Object::kInvalidObjNum)
      return nullptr;

    return pdfium::MakeUnique<CPDF_Reference>(pObjList, refnum);
  }

  if (word == "true" || word == "false")
    return pdfium::MakeUnique<CPDF_Boolean>(word == "true");

  if (word == "null")
    return pdfium::MakeUnique<CPDF_Null>();

  if (word == "(") {
    ByteString str = ReadString();
    return pdfium::MakeUnique<CPDF_String>(m_pPool, str, false);
  }
  if (word == "<") {
    ByteString str = ReadHexString();
    return pdfium::MakeUnique<CPDF_String>(m_pPool, str, true);
  }
  if (word == "[") {
    auto pArray = pdfium::MakeUnique<CPDF_Array>();
    while (std::unique_ptr<CPDF_Object> pObj =
               GetObjectBodyInternal(pObjList, ParseType::kLoose)) {
      pArray->Add(std::move(pObj));
    }
    return (parse_type == ParseType::kLoose || m_WordBuffer[0] == ']')
               ? std::move(pArray)
               : nullptr;
  }
  if (word[0] == '/') {
    return pdfium::MakeUnique<CPDF_Name>(
        m_pPool,
        PDF_NameDecode(ByteStringView(m_WordBuffer + 1, m_WordSize - 1)));
  }
  if (word == "<<") {
    std::unique_ptr<CPDF_Dictionary> pDict =
        pdfium::MakeUnique<CPDF_Dictionary>(m_pPool);
    while (1) {
      ByteString inner_word = GetNextWord(nullptr);
      if (inner_word.IsEmpty())
        return nullptr;

      FX_FILESIZE SavedPos = m_Pos - inner_word.GetLength();
      if (inner_word == ">>")
        break;

      if (inner_word == "endobj") {
        m_Pos = SavedPos;
        break;
      }
      if (inner_word[0] != '/')
        continue;

      ByteString key = PDF_NameDecode(inner_word.AsStringView());
      if (key.IsEmpty() && parse_type == ParseType::kLoose)
        continue;

      std::unique_ptr<CPDF_Object> pObj =
          GetObjectBodyInternal(pObjList, ParseType::kLoose);
      if (!pObj) {
        if (parse_type == ParseType::kLoose)
          continue;

        ToNextLine();
        return nullptr;
      }

      if (!key.IsEmpty()) {
        ByteString keyNoSlash(key.raw_str() + 1, key.GetLength() - 1);
        pDict->SetFor(keyNoSlash, std::move(pObj));
      }
    }

    AutoRestorer<FX_FILESIZE> pos_restorer(&m_Pos);
    if (GetNextWord(nullptr) != "stream")
      return std::move(pDict);
    pos_restorer.AbandonRestoration();
    return ReadStream(std::move(pDict));
  }
  if (word == ">>")
    m_Pos = SavedObjPos;

  return nullptr;
}

std::unique_ptr<CPDF_Object> CPDF_SyntaxParser::GetIndirectObject(
    CPDF_IndirectObjectHolder* pObjList,
    ParseType parse_type) {
  const CPDF_ReadValidator::Session read_session(GetValidator().Get());
  const FX_FILESIZE saved_pos = GetPos();
  bool is_number = false;
  ByteString word = GetNextWord(&is_number);
  if (!is_number || word.IsEmpty()) {
    SetPos(saved_pos);
    return nullptr;
  }
  const uint32_t parser_objnum = FXSYS_atoui(word.c_str());

  word = GetNextWord(&is_number);
  if (!is_number || word.IsEmpty()) {
    SetPos(saved_pos);
    return nullptr;
  }
  const uint32_t parser_gennum = FXSYS_atoui(word.c_str());

  if (GetKeyword() != "obj") {
    SetPos(saved_pos);
    return nullptr;
  }

  std::unique_ptr<CPDF_Object> pObj =
      GetObjectBodyInternal(pObjList, parse_type);
  if (pObj) {
    pObj->SetObjNum(parser_objnum);
    pObj->SetGenNum(parser_gennum);
  }

  return GetValidator()->has_read_problems() ? nullptr : std::move(pObj);
}

unsigned int CPDF_SyntaxParser::ReadEOLMarkers(FX_FILESIZE pos) {
  unsigned char byte1 = 0;
  unsigned char byte2 = 0;

  GetCharAt(pos, byte1);
  GetCharAt(pos + 1, byte2);

  if (byte1 == '\r' && byte2 == '\n')
    return 2;

  if (byte1 == '\r' || byte1 == '\n')
    return 1;

  return 0;
}

FX_FILESIZE CPDF_SyntaxParser::FindWordPos(ByteStringView word) {
  AutoRestorer<FX_FILESIZE> pos_restorer(&m_Pos);
  FX_FILESIZE end_offset = FindTag(word);
  while (end_offset >= 0) {
    // Stop searching when word is found.
    if (IsWholeWord(GetPos() - word.GetLength(), m_FileLen, word, true))
      return GetPos() - word.GetLength();

    end_offset = FindTag(word);
  }
  return -1;
}

FX_FILESIZE CPDF_SyntaxParser::FindStreamEndPos() {
  const ByteStringView kEndStreamStr("endstream");
  const ByteStringView kEndObjStr("endobj");

  FX_FILESIZE endStreamWordOffset = FindWordPos(kEndStreamStr);
  FX_FILESIZE endObjWordOffset = FindWordPos(kEndObjStr);

  // Can't find "endstream" or "endobj".
  if (endStreamWordOffset < 0 && endObjWordOffset < 0) {
    return -1;
  }

  if (endStreamWordOffset < 0 && endObjWordOffset >= 0) {
    // Correct the position of end stream.
    endStreamWordOffset = endObjWordOffset;
  } else if (endStreamWordOffset >= 0 && endObjWordOffset < 0) {
    // Correct the position of end obj.
    endObjWordOffset = endStreamWordOffset;
  } else if (endStreamWordOffset > endObjWordOffset) {
    endStreamWordOffset = endObjWordOffset;
  }

  int numMarkers = ReadEOLMarkers(endStreamWordOffset - 2);
  if (numMarkers == 2) {
    endStreamWordOffset -= 2;
  } else {
    numMarkers = ReadEOLMarkers(endStreamWordOffset - 1);
    if (numMarkers == 1) {
      endStreamWordOffset -= 1;
    }
  }
  if (endStreamWordOffset < GetPos()) {
    return -1;
  }
  return endStreamWordOffset;
}

std::unique_ptr<CPDF_Stream> CPDF_SyntaxParser::ReadStream(
    std::unique_ptr<CPDF_Dictionary> pDict) {
  const CPDF_Number* pLenObj = ToNumber(pDict->GetDirectObjectFor("Length"));
  FX_FILESIZE len = pLenObj ? pLenObj->GetInteger() : -1;

  // Locate the start of stream.
  ToNextLine();
  const FX_FILESIZE streamStartPos = GetPos();

  if (len > 0) {
    FX_SAFE_FILESIZE pos = GetPos();
    pos += len;
    if (!pos.IsValid() || pos.ValueOrDie() >= m_FileLen)
      len = -1;
  }

  RetainPtr<IFX_SeekableReadStream> data;
  if (len > 0) {
    // Check data availability first to allow the Validator to request data
    // smoothly, without jumps.
    if (!GetValidator()->CheckDataRangeAndRequestIfUnavailable(
            m_HeaderOffset + GetPos(), len)) {
      return nullptr;
    }

    data = pdfium::MakeRetain<ReadableSubStream>(
        GetValidator(), m_HeaderOffset + GetPos(), len);
    SetPos(GetPos() + len);
  }

  const ByteStringView kEndStreamStr("endstream");
  const ByteStringView kEndObjStr("endobj");

  // Note, we allow zero length streams as we need to pass them through when we
  // are importing pages into a new document.
  if (len >= 0) {
    const CPDF_ReadValidator::Session read_session(GetValidator().Get());
    m_Pos += ReadEOLMarkers(GetPos());
    memset(m_WordBuffer, 0, kEndStreamStr.GetLength() + 1);
    GetNextWordInternal(nullptr);
    if (GetValidator()->has_read_problems())
      return nullptr;

    // Earlier version of PDF specification doesn't require EOL marker before
    // 'endstream' keyword. If keyword 'endstream' follows the bytes in
    // specified length, it signals the end of stream.
    if (memcmp(m_WordBuffer, kEndStreamStr.raw_str(),
               kEndStreamStr.GetLength()) != 0) {
      data.Reset();
      len = -1;
      SetPos(streamStartPos);
    }
  }

  if (len < 0) {
    // If len is not available or incorrect, len needs to be calculated
    // by searching the keywords "endstream" or "endobj".
    const FX_FILESIZE streamEndPos = FindStreamEndPos();
    if (streamEndPos < 0)
      return nullptr;

    len = streamEndPos - streamStartPos;
    ASSERT(len >= 0);
    if (len > 0) {
      SetPos(streamStartPos);
      // Check data availability first to allow the Validator to request data
      // smoothly, without jumps.
      if (!GetValidator()->CheckDataRangeAndRequestIfUnavailable(
              m_HeaderOffset + GetPos(), len)) {
        return nullptr;
      }

      data = pdfium::MakeRetain<ReadableSubStream>(
          GetValidator(), m_HeaderOffset + GetPos(), len);
      SetPos(GetPos() + len);
    }
  }

  auto pStream = pdfium::MakeUnique<CPDF_Stream>();
  if (data) {
    pStream->InitStreamFromFile(data, std::move(pDict));
  } else {
    DCHECK(!len);
    pStream->InitStream({}, std::move(pDict));  // Empty stream
  }
  const FX_FILESIZE end_stream_offset = GetPos();
  memset(m_WordBuffer, 0, kEndObjStr.GetLength() + 1);
  GetNextWordInternal(nullptr);

  int numMarkers = ReadEOLMarkers(GetPos());
  if (m_WordSize == static_cast<unsigned int>(kEndObjStr.GetLength()) &&
      numMarkers != 0 &&
      memcmp(m_WordBuffer, kEndObjStr.raw_str(), kEndObjStr.GetLength()) == 0) {
    SetPos(end_stream_offset);
  }
  return pStream;
}

uint32_t CPDF_SyntaxParser::GetDirectNum() {
  bool bIsNumber;
  GetNextWordInternal(&bIsNumber);
  if (!bIsNumber)
    return 0;

  m_WordBuffer[m_WordSize] = 0;
  return FXSYS_atoui(reinterpret_cast<const char*>(m_WordBuffer));
}

bool CPDF_SyntaxParser::IsWholeWord(FX_FILESIZE startpos,
                                    FX_FILESIZE limit,
                                    ByteStringView tag,
                                    bool checkKeyword) {
  const uint32_t taglen = tag.GetLength();

  bool bCheckLeft = !PDFCharIsDelimiter(tag[0]) && !PDFCharIsWhitespace(tag[0]);
  bool bCheckRight = !PDFCharIsDelimiter(tag[taglen - 1]) &&
                     !PDFCharIsWhitespace(tag[taglen - 1]);

  uint8_t ch;
  if (bCheckRight && startpos + (int32_t)taglen <= limit &&
      GetCharAt(startpos + (int32_t)taglen, ch)) {
    if (PDFCharIsNumeric(ch) || PDFCharIsOther(ch) ||
        (checkKeyword && PDFCharIsDelimiter(ch))) {
      return false;
    }
  }

  if (bCheckLeft && startpos > 0 && GetCharAt(startpos - 1, ch)) {
    if (PDFCharIsNumeric(ch) || PDFCharIsOther(ch) ||
        (checkKeyword && PDFCharIsDelimiter(ch))) {
      return false;
    }
  }
  return true;
}

bool CPDF_SyntaxParser::BackwardsSearchToWord(ByteStringView tag,
                                              FX_FILESIZE limit) {
  int32_t taglen = tag.GetLength();
  if (taglen == 0)
    return false;

  FX_FILESIZE pos = m_Pos;
  int32_t offset = taglen - 1;
  while (1) {
    if (limit && pos <= m_Pos - limit)
      return false;

    uint8_t byte;
    if (!GetCharAtBackward(pos, &byte))
      return false;

    if (byte == tag[offset]) {
      offset--;
      if (offset >= 0) {
        pos--;
        continue;
      }
      if (IsWholeWord(pos, limit, tag, false)) {
        m_Pos = pos;
        return true;
      }
    }
    offset = byte == tag[taglen - 1] ? taglen - 2 : taglen - 1;
    pos--;
    if (pos < 0)
      return false;
  }
}

FX_FILESIZE CPDF_SyntaxParser::FindTag(ByteStringView tag) {
  const FX_FILESIZE startpos = GetPos();
  const int32_t taglen = tag.GetLength();
  ASSERT(taglen > 0);

  int32_t match = 0;
  while (1) {
    uint8_t ch;
    if (!GetNextChar(ch))
      return -1;

    if (ch == tag[match]) {
      match++;
      if (match == taglen)
        return GetPos() - startpos - taglen;
    } else {
      match = ch == tag[0] ? 1 : 0;
    }
  }
  return -1;
}

bool CPDF_SyntaxParser::IsPositionRead(FX_FILESIZE pos) const {
  return m_BufOffset <= pos &&
         pos < static_cast<FX_FILESIZE>(m_BufOffset + m_pFileBuf.size());
}
