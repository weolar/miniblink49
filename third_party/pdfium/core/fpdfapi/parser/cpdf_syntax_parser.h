// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_SYNTAX_PARSER_H_
#define CORE_FPDFAPI_PARSER_CPDF_SYNTAX_PARSER_H_

#include <memory>
#include <vector>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fxcrt/string_pool_template.h"
#include "core/fxcrt/weak_ptr.h"

class CPDF_CryptoHandler;
class CPDF_Dictionary;
class CPDF_IndirectObjectHolder;
class CPDF_Object;
class CPDF_ReadValidator;
class CPDF_Stream;
class IFX_SeekableReadStream;

class CPDF_SyntaxParser {
 public:
  enum class ParseType { kStrict, kLoose };

  static std::unique_ptr<CPDF_SyntaxParser> CreateForTesting(
      const RetainPtr<IFX_SeekableReadStream>& pFileAccess,
      FX_FILESIZE HeaderOffset);

  explicit CPDF_SyntaxParser(
      const RetainPtr<IFX_SeekableReadStream>& pFileAccess);
  CPDF_SyntaxParser(const RetainPtr<CPDF_ReadValidator>& pValidator,
                    FX_FILESIZE HeaderOffset);
  ~CPDF_SyntaxParser();

  void SetReadBufferSize(uint32_t read_buffer_size) {
    m_ReadBufferSize = read_buffer_size;
  }

  FX_FILESIZE GetPos() const { return m_Pos; }
  void SetPos(FX_FILESIZE pos);

  std::unique_ptr<CPDF_Object> GetObjectBody(
      CPDF_IndirectObjectHolder* pObjList);

  std::unique_ptr<CPDF_Object> GetIndirectObject(
      CPDF_IndirectObjectHolder* pObjList,
      ParseType parse_type);

  ByteString GetKeyword();
  void ToNextLine();
  void ToNextWord();
  bool BackwardsSearchToWord(ByteStringView word, FX_FILESIZE limit);
  FX_FILESIZE FindTag(ByteStringView tag);
  bool ReadBlock(uint8_t* pBuf, uint32_t size);
  bool GetCharAt(FX_FILESIZE pos, uint8_t& ch);
  ByteString GetNextWord(bool* bIsNumber);
  ByteString PeekNextWord(bool* bIsNumber);

  const RetainPtr<CPDF_ReadValidator>& GetValidator() const {
    return m_pFileAccess;
  }
  uint32_t GetDirectNum();
  bool GetNextChar(uint8_t& ch);

  // The document size may be smaller than the file size.
  // The syntax parser use position relative to document
  // offset (|m_HeaderOffset|).
  // The document size will be FileSize - "Header offset".
  // All offsets was readed from document, should not be great than document
  // size. Use it for checks instead of real file size.
  FX_FILESIZE GetDocumentSize() const;

  ByteString ReadString();
  ByteString ReadHexString();

 private:
  friend class CPDF_DataAvail;
  friend class cpdf_syntax_parser_ReadHexString_Test;

  static const int kParserMaxRecursionDepth = 64;
  static int s_CurrentRecursionDepth;

  bool ReadBlockAt(FX_FILESIZE read_pos);
  bool GetCharAtBackward(FX_FILESIZE pos, uint8_t* ch);
  void GetNextWordInternal(bool* bIsNumber);
  bool IsWholeWord(FX_FILESIZE startpos,
                   FX_FILESIZE limit,
                   ByteStringView tag,
                   bool checkKeyword);

  unsigned int ReadEOLMarkers(FX_FILESIZE pos);
  FX_FILESIZE FindWordPos(ByteStringView word);
  FX_FILESIZE FindStreamEndPos();
  std::unique_ptr<CPDF_Stream> ReadStream(
      std::unique_ptr<CPDF_Dictionary> pDict);

  bool IsPositionRead(FX_FILESIZE pos) const;

  std::unique_ptr<CPDF_Object> GetObjectBodyInternal(
      CPDF_IndirectObjectHolder* pObjList,
      ParseType parse_type);

  RetainPtr<CPDF_ReadValidator> m_pFileAccess;
  // The syntax parser use position relative to header offset.
  // The header contains at file start, and can follow after some stuff. We
  // ignore this stuff.
  const FX_FILESIZE m_HeaderOffset;
  const FX_FILESIZE m_FileLen;
  FX_FILESIZE m_Pos = 0;
  WeakPtr<ByteStringPool> m_pPool;
  std::vector<uint8_t> m_pFileBuf;
  FX_FILESIZE m_BufOffset = 0;
  uint32_t m_WordSize = 0;
  uint8_t m_WordBuffer[257];
  uint32_t m_ReadBufferSize = CPDF_ModuleMgr::kFileBufSize;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_SYNTAX_PARSER_H_
