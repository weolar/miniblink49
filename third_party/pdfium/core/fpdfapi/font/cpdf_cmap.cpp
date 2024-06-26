// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/font/cpdf_cmap.h"

#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/cmaps/cmap_int.h"
#include "core/fpdfapi/font/cpdf_cmapmanager.h"
#include "core/fpdfapi/font/cpdf_cmapparser.h"
#include "core/fpdfapi/parser/cpdf_simple_parser.h"

namespace {

struct ByteRange {
  uint8_t m_First;
  uint8_t m_Last;  // Inclusive.
};

struct PredefinedCMap {
  const char* m_pName;  // Raw, POD struct.
  CIDSet m_Charset;
  CIDCoding m_Coding;
  CPDF_CMap::CodingScheme m_CodingScheme;
  uint8_t m_LeadingSegCount;
  ByteRange m_LeadingSegs[2];
};

const PredefinedCMap g_PredefinedCMaps[] = {
    {"GB-EUC",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0xa1, 0xfe}}},
    {"GBpc-EUC",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0xa1, 0xfc}}},
    {"GBK-EUC",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0x81, 0xfe}}},
    {"GBKp-EUC",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0x81, 0xfe}}},
    {"GBK2K-EUC",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0x81, 0xfe}}},
    {"GBK2K",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0x81, 0xfe}}},
    {"UniGB-UCS2", CIDSET_GB1, CIDCODING_UCS2, CPDF_CMap::TwoBytes, 0, {}},
    {"UniGB-UTF16", CIDSET_GB1, CIDCODING_UTF16, CPDF_CMap::TwoBytes, 0, {}},
    {"B5pc",
     CIDSET_CNS1,
     CIDCODING_BIG5,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0xa1, 0xfc}}},
    {"HKscs-B5",
     CIDSET_CNS1,
     CIDCODING_BIG5,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0x88, 0xfe}}},
    {"ETen-B5",
     CIDSET_CNS1,
     CIDCODING_BIG5,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0xa1, 0xfe}}},
    {"ETenms-B5",
     CIDSET_CNS1,
     CIDCODING_BIG5,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0xa1, 0xfe}}},
    {"UniCNS-UCS2", CIDSET_CNS1, CIDCODING_UCS2, CPDF_CMap::TwoBytes, 0, {}},
    {"UniCNS-UTF16", CIDSET_CNS1, CIDCODING_UTF16, CPDF_CMap::TwoBytes, 0, {}},
    {"83pv-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {{0x81, 0x9f}, {0xe0, 0xfc}}},
    {"90ms-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {{0x81, 0x9f}, {0xe0, 0xfc}}},
    {"90msp-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {{0x81, 0x9f}, {0xe0, 0xfc}}},
    {"90pv-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {{0x81, 0x9f}, {0xe0, 0xfc}}},
    {"Add-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {{0x81, 0x9f}, {0xe0, 0xfc}}},
    {"EUC",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {{0x8e, 0x8e}, {0xa1, 0xfe}}},
    {"H", CIDSET_JAPAN1, CIDCODING_JIS, CPDF_CMap::TwoBytes, 1, {{0x21, 0x7e}}},
    {"V", CIDSET_JAPAN1, CIDCODING_JIS, CPDF_CMap::TwoBytes, 1, {{0x21, 0x7e}}},
    {"Ext-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {{0x81, 0x9f}, {0xe0, 0xfc}}},
    {"UniJIS-UCS2", CIDSET_JAPAN1, CIDCODING_UCS2, CPDF_CMap::TwoBytes, 0, {}},
    {"UniJIS-UCS2-HW",
     CIDSET_JAPAN1,
     CIDCODING_UCS2,
     CPDF_CMap::TwoBytes,
     0,
     {}},
    {"UniJIS-UTF16",
     CIDSET_JAPAN1,
     CIDCODING_UTF16,
     CPDF_CMap::TwoBytes,
     0,
     {}},
    {"KSC-EUC",
     CIDSET_KOREA1,
     CIDCODING_KOREA,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0xa1, 0xfe}}},
    {"KSCms-UHC",
     CIDSET_KOREA1,
     CIDCODING_KOREA,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0x81, 0xfe}}},
    {"KSCms-UHC-HW",
     CIDSET_KOREA1,
     CIDCODING_KOREA,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0x81, 0xfe}}},
    {"KSCpc-EUC",
     CIDSET_KOREA1,
     CIDCODING_KOREA,
     CPDF_CMap::MixedTwoBytes,
     1,
     {{0xa1, 0xfd}}},
    {"UniKS-UCS2", CIDSET_KOREA1, CIDCODING_UCS2, CPDF_CMap::TwoBytes, 0, {}},
    {"UniKS-UTF16", CIDSET_KOREA1, CIDCODING_UTF16, CPDF_CMap::TwoBytes, 0, {}},
};

int CheckFourByteCodeRange(uint8_t* codes,
                           size_t size,
                           const std::vector<CPDF_CMap::CodeRange>& ranges) {
  for (size_t i = ranges.size(); i > 0; i--) {
    size_t seg = i - 1;
    if (ranges[seg].m_CharSize < size)
      continue;
    size_t iChar = 0;
    while (iChar < size) {
      if (codes[iChar] < ranges[seg].m_Lower[iChar] ||
          codes[iChar] > ranges[seg].m_Upper[iChar]) {
        break;
      }
      ++iChar;
    }
    if (iChar == ranges[seg].m_CharSize)
      return 2;
    if (iChar)
      return (size == ranges[seg].m_CharSize) ? 2 : 1;
  }
  return 0;
}

size_t GetFourByteCharSizeImpl(
    uint32_t charcode,
    const std::vector<CPDF_CMap::CodeRange>& ranges) {
  if (ranges.empty())
    return 1;

  uint8_t codes[4];
  codes[0] = codes[1] = 0x00;
  codes[2] = static_cast<uint8_t>(charcode >> 8 & 0xFF);
  codes[3] = static_cast<uint8_t>(charcode);
  for (size_t offset = 0; offset < 4; offset++) {
    size_t size = 4 - offset;
    for (size_t j = 0; j < ranges.size(); j++) {
      size_t iSeg = (ranges.size() - 1) - j;
      if (ranges[iSeg].m_CharSize < size)
        continue;
      size_t iChar = 0;
      while (iChar < size) {
        if (codes[offset + iChar] < ranges[iSeg].m_Lower[iChar] ||
            codes[offset + iChar] > ranges[iSeg].m_Upper[iChar]) {
          break;
        }
        ++iChar;
      }
      if (iChar == ranges[iSeg].m_CharSize)
        return size;
    }
  }
  return 1;
}

}  // namespace

CPDF_CMap::CPDF_CMap()
    : m_bLoaded(false),
      m_bVertical(false),
      m_Charset(CIDSET_UNKNOWN),
      m_CodingScheme(TwoBytes),
      m_Coding(CIDCODING_UNKNOWN),
      m_pEmbedMap(nullptr) {}

CPDF_CMap::~CPDF_CMap() {}

void CPDF_CMap::LoadPredefined(CPDF_CMapManager* pMgr,
                               const ByteString& bsName) {
  m_PredefinedCMap = bsName;
  if (m_PredefinedCMap == "Identity-H" || m_PredefinedCMap == "Identity-V") {
    m_Coding = CIDCODING_CID;
    m_bVertical = bsName.Last() == 'V';
    m_bLoaded = true;
    return;
  }
  ByteString cmapid = m_PredefinedCMap;
  m_bVertical = cmapid.Last() == 'V';
  if (cmapid.GetLength() > 2) {
    cmapid = cmapid.Left(cmapid.GetLength() - 2);
  }
  const PredefinedCMap* map = nullptr;
  for (size_t i = 0; i < FX_ArraySize(g_PredefinedCMaps); ++i) {
    if (cmapid == ByteStringView(g_PredefinedCMaps[i].m_pName)) {
      map = &g_PredefinedCMaps[i];
      break;
    }
  }
  if (!map)
    return;

  m_Charset = map->m_Charset;
  m_Coding = map->m_Coding;
  m_CodingScheme = map->m_CodingScheme;
  if (m_CodingScheme == MixedTwoBytes) {
    m_MixedTwoByteLeadingBytes = std::vector<bool>(256);
    for (uint32_t i = 0; i < map->m_LeadingSegCount; ++i) {
      const ByteRange& seg = map->m_LeadingSegs[i];
      for (int b = seg.m_First; b <= seg.m_Last; ++b)
        m_MixedTwoByteLeadingBytes[b] = true;
    }
  }
  m_pEmbedMap = FindEmbeddedCMap(bsName, m_Charset, m_Coding);
  if (!m_pEmbedMap)
    return;

  m_bLoaded = true;
}

void CPDF_CMap::LoadEmbedded(pdfium::span<const uint8_t> data) {
  m_DirectCharcodeToCIDTable = std::vector<uint16_t>(65536);
  CPDF_CMapParser parser(this);
  CPDF_SimpleParser syntax(data);
  while (1) {
    ByteStringView word = syntax.GetWord();
    if (word.IsEmpty()) {
      break;
    }
    parser.ParseWord(word);
  }
  if (m_CodingScheme == MixedFourBytes && parser.HasAdditionalMappings()) {
    m_AdditionalCharcodeToCIDMappings = parser.TakeAdditionalMappings();
    std::sort(
        m_AdditionalCharcodeToCIDMappings.begin(),
        m_AdditionalCharcodeToCIDMappings.end(),
        [](const CPDF_CMap::CIDRange& arg1, const CPDF_CMap::CIDRange& arg2) {
          return arg1.m_EndCode < arg2.m_EndCode;
        });
  }
}

uint16_t CPDF_CMap::CIDFromCharCode(uint32_t charcode) const {
  if (m_Coding == CIDCODING_CID)
    return static_cast<uint16_t>(charcode);

  if (m_pEmbedMap)
    return ::CIDFromCharCode(m_pEmbedMap.Get(), charcode);

  if (m_DirectCharcodeToCIDTable.empty())
    return static_cast<uint16_t>(charcode);

  if (charcode < 0x10000)
    return m_DirectCharcodeToCIDTable[charcode];

  auto it = std::lower_bound(m_AdditionalCharcodeToCIDMappings.begin(),
                             m_AdditionalCharcodeToCIDMappings.end(), charcode,
                             [](const CPDF_CMap::CIDRange& arg, uint32_t val) {
                               return arg.m_EndCode < val;
                             });
  if (it == m_AdditionalCharcodeToCIDMappings.end() ||
      it->m_StartCode > charcode) {
    return 0;
  }
  return it->m_StartCID + charcode - it->m_StartCode;
}

uint32_t CPDF_CMap::GetNextChar(ByteStringView pString, size_t* pOffset) const {
  size_t& offset = *pOffset;
  auto pBytes = pString.span();
  switch (m_CodingScheme) {
    case OneByte: {
      return offset < pBytes.size() ? pBytes[offset++] : 0;
    }
    case TwoBytes: {
      uint8_t byte1 = offset < pBytes.size() ? pBytes[offset++] : 0;
      uint8_t byte2 = offset < pBytes.size() ? pBytes[offset++] : 0;
      return 256 * byte1 + byte2;
    }
    case MixedTwoBytes: {
      uint8_t byte1 = offset < pBytes.size() ? pBytes[offset++] : 0;
      if (!m_MixedTwoByteLeadingBytes[byte1])
        return byte1;
      uint8_t byte2 = offset < pBytes.size() ? pBytes[offset++] : 0;
      return 256 * byte1 + byte2;
    }
    case MixedFourBytes: {
      uint8_t codes[4];
      int char_size = 1;
      codes[0] = offset < pBytes.size() ? pBytes[offset++] : 0;
      while (1) {
        int ret = CheckFourByteCodeRange(codes, char_size,
                                         m_MixedFourByteLeadingRanges);
        if (ret == 0)
          return 0;
        if (ret == 2) {
          uint32_t charcode = 0;
          for (int i = 0; i < char_size; i++)
            charcode = (charcode << 8) + codes[i];
          return charcode;
        }
        if (char_size == 4 || offset == pBytes.size())
          return 0;
        codes[char_size++] = pBytes[offset++];
      }
      break;
    }
  }
  return 0;
}

int CPDF_CMap::GetCharSize(uint32_t charcode) const {
  switch (m_CodingScheme) {
    case OneByte:
      return 1;
    case TwoBytes:
      return 2;
    case MixedTwoBytes:
      if (charcode < 0x100)
        return 1;
      return 2;
    case MixedFourBytes:
      if (charcode < 0x100)
        return 1;
      if (charcode < 0x10000)
        return 2;
      if (charcode < 0x1000000)
        return 3;
      return 4;
  }
  return 1;
}

size_t CPDF_CMap::CountChar(ByteStringView pString) const {
  switch (m_CodingScheme) {
    case OneByte:
      return pString.GetLength();
    case TwoBytes:
      return (pString.GetLength() + 1) / 2;
    case MixedTwoBytes: {
      size_t count = 0;
      for (size_t i = 0; i < pString.GetLength(); i++) {
        count++;
        if (m_MixedTwoByteLeadingBytes[pString[i]])
          i++;
      }
      return count;
    }
    case MixedFourBytes: {
      size_t count = 0;
      size_t offset = 0;
      while (offset < pString.GetLength()) {
        GetNextChar(pString, &offset);
        count++;
      }
      return count;
    }
  }
  return pString.GetLength();
}

int CPDF_CMap::AppendChar(char* str, uint32_t charcode) const {
  switch (m_CodingScheme) {
    case OneByte:
      str[0] = static_cast<char>(charcode);
      return 1;
    case TwoBytes:
      str[0] = static_cast<char>(charcode / 256);
      str[1] = static_cast<char>(charcode % 256);
      return 2;
    case MixedTwoBytes:
      if (charcode < 0x100 && !m_MixedTwoByteLeadingBytes[charcode]) {
        str[0] = static_cast<char>(charcode);
        return 1;
      }
      str[0] = static_cast<char>(charcode >> 8);
      str[1] = static_cast<char>(charcode);
      return 2;
    case MixedFourBytes:
      if (charcode < 0x100) {
        int iSize = static_cast<int>(
            GetFourByteCharSizeImpl(charcode, m_MixedFourByteLeadingRanges));
        if (iSize == 0)
          iSize = 1;
        str[iSize - 1] = static_cast<char>(charcode);
        if (iSize > 1)
          memset(str, 0, iSize - 1);
        return iSize;
      }
      if (charcode < 0x10000) {
        str[0] = static_cast<char>(charcode >> 8);
        str[1] = static_cast<char>(charcode);
        return 2;
      }
      if (charcode < 0x1000000) {
        str[0] = static_cast<char>(charcode >> 16);
        str[1] = static_cast<char>(charcode >> 8);
        str[2] = static_cast<char>(charcode);
        return 3;
      }
      str[0] = static_cast<char>(charcode >> 24);
      str[1] = static_cast<char>(charcode >> 16);
      str[2] = static_cast<char>(charcode >> 8);
      str[3] = static_cast<char>(charcode);
      return 4;
  }
  return 0;
}
