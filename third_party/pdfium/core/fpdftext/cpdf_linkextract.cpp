// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdftext/cpdf_linkextract.h"

#include <vector>

#include "core/fpdftext/cpdf_textpage.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"

namespace {

// Find the end of a web link starting from offset |start| and ending at offset
// |end|. The purpose of this function is to separate url from the surrounding
// context characters, we do not intend to fully validate the url. |str|
// contains lower case characters only.
size_t FindWebLinkEnding(const WideString& str, size_t start, size_t end) {
  if (str.Contains(L'/', start)) {
    // When there is a path and query after '/', most ASCII chars are allowed.
    // We don't sanitize in this case.
    return end;
  }

  // When there is no path, it only has IP address or host name.
  // Port is optional at the end.
  if (str[start] == L'[') {
    // IPv6 reference.
    // Find the end of the reference.
    auto result = str.Find(L']', start + 1);
    if (result.has_value()) {
      end = result.value();
      if (end > start + 1) {  // Has content inside brackets.
        size_t len = str.GetLength();
        size_t off = end + 1;
        if (off < len && str[off] == L':') {
          off++;
          while (off < len && FXSYS_IsDecimalDigit(str[off]))
            off++;
          if (off > end + 2 &&
              off <= len)   // At least one digit in port number.
            end = off - 1;  // |off| is offset of the first invalid char.
        }
      }
    }
    return end;
  }

  // According to RFC1123, host name only has alphanumeric chars, hyphens,
  // and periods. Hyphen should not at the end though.
  // Non-ASCII chars are ignored during checking.
  while (end > start && str[end] < 0x80) {
    if (FXSYS_IsDecimalDigit(str[end]) ||
        (str[end] >= L'a' && str[end] <= L'z') || str[end] == L'.') {
      break;
    }
    end--;
  }
  return end;
}

// Remove characters from the end of |str|, delimited by |start| and |end|, up
// to and including |charToFind|. No-op if |charToFind| is not present. Updates
// |end| if characters were removed.
void TrimBackwardsToChar(const WideString& str,
                         wchar_t charToFind,
                         size_t start,
                         size_t* end) {
  for (size_t pos = *end; pos >= start; pos--) {
    if (str[pos] == charToFind) {
      *end = pos - 1;
      break;
    }
  }
}

// Finds opening brackets ()[]{}<> and quotes "'  before the URL delimited by
// |start| and |end| in |str|. Matches a closing bracket or quote for each
// opening character and, if present, removes everything afterwards. Returns the
// new end position for the string.
size_t TrimExternalBracketsFromWebLink(const WideString& str,
                                       size_t start,
                                       size_t end) {
  for (size_t pos = 0; pos < start; pos++) {
    if (str[pos] == '(') {
      TrimBackwardsToChar(str, ')', start, &end);
    } else if (str[pos] == '[') {
      TrimBackwardsToChar(str, ']', start, &end);
    } else if (str[pos] == '{') {
      TrimBackwardsToChar(str, '}', start, &end);
    } else if (str[pos] == '<') {
      TrimBackwardsToChar(str, '>', start, &end);
    } else if (str[pos] == '"') {
      TrimBackwardsToChar(str, '"', start, &end);
    } else if (str[pos] == '\'') {
      TrimBackwardsToChar(str, '\'', start, &end);
    }
  }
  return end;
}

}  // namespace

CPDF_LinkExtract::CPDF_LinkExtract(const CPDF_TextPage* pTextPage)
    : m_pTextPage(pTextPage) {}

CPDF_LinkExtract::~CPDF_LinkExtract() {}

void CPDF_LinkExtract::ExtractLinks() {
  m_LinkArray.clear();
  if (!m_pTextPage->IsParsed())
    return;

  m_strPageText = m_pTextPage->GetAllPageText();
  if (m_strPageText.IsEmpty())
    return;

  ParseLink();
}

void CPDF_LinkExtract::ParseLink() {
  int start = 0;
  int pos = 0;
  int nTotalChar = m_pTextPage->CountChars();
  bool bAfterHyphen = false;
  bool bLineBreak = false;
  while (pos < nTotalChar) {
    FPDF_CHAR_INFO pageChar;
    m_pTextPage->GetCharInfo(pos, &pageChar);
    if (pageChar.m_Flag == FPDFTEXT_CHAR_GENERATED ||
        pageChar.m_Unicode == TEXT_SPACE_CHAR || pos == nTotalChar - 1) {
      int nCount = pos - start;
      if (pos == nTotalChar - 1) {
        nCount++;
      } else if (bAfterHyphen && (pageChar.m_Unicode == TEXT_LINEFEED_CHAR ||
                                  pageChar.m_Unicode == TEXT_RETURN_CHAR)) {
        // Handle text breaks with a hyphen to the next line.
        bLineBreak = true;
        pos++;
        continue;
      }
      WideString strBeCheck;
      strBeCheck = m_pTextPage->GetPageText(start, nCount);
      if (bLineBreak) {
        strBeCheck.Remove(TEXT_LINEFEED_CHAR);
        strBeCheck.Remove(TEXT_RETURN_CHAR);
        bLineBreak = false;
      }
      // Replace the generated code with the hyphen char.
      strBeCheck.Replace(L"\xfffe", TEXT_HYPHEN);

      if (strBeCheck.GetLength() > 5) {
        while (strBeCheck.GetLength() > 0) {
          wchar_t ch = strBeCheck[strBeCheck.GetLength() - 1];
          if (ch == L')' || ch == L',' || ch == L'>' || ch == L'.') {
            strBeCheck = strBeCheck.Left(strBeCheck.GetLength() - 1);
            nCount--;
          } else {
            break;
          }
        }
        // Check for potential web URLs and email addresses.
        // Ftp address, file system links, data, blob etc. are not checked.
        if (nCount > 5) {
          int32_t nStartOffset;
          int32_t nCountOverload;
          if (CheckWebLink(&strBeCheck, &nStartOffset, &nCountOverload)) {
            m_LinkArray.push_back(
                {start + nStartOffset, nCountOverload, strBeCheck});
          } else if (CheckMailLink(&strBeCheck)) {
            m_LinkArray.push_back({start, nCount, strBeCheck});
          }
        }
      }
      start = ++pos;
    } else {
      bAfterHyphen = (pageChar.m_Flag == FPDFTEXT_CHAR_HYPHEN ||
                      (pageChar.m_Flag == FPDFTEXT_CHAR_NORMAL &&
                       pageChar.m_Unicode == TEXT_HYPHEN_CHAR));
      pos++;
    }
  }
}

bool CPDF_LinkExtract::CheckWebLink(WideString* strBeCheck,
                                    int32_t* nStart,
                                    int32_t* nCount) {
  static const wchar_t kHttpScheme[] = L"http";
  static const wchar_t kWWWAddrStart[] = L"www.";

  const size_t kHttpSchemeLen = FXSYS_len(kHttpScheme);
  const size_t kWWWAddrStartLen = FXSYS_len(kWWWAddrStart);

  WideString str = *strBeCheck;
  str.MakeLower();

  size_t len = str.GetLength();
  // First, try to find the scheme.
  auto start = str.Find(kHttpScheme);
  if (start.has_value()) {
    size_t off = start.value() + kHttpSchemeLen;  // move after "http".
    if (len > off + 4) {                      // At least "://<char>" follows.
      if (str[off] == L's')                   // "https" scheme is accepted.
        off++;
      if (str[off] == L':' && str[off + 1] == L'/' && str[off + 2] == L'/') {
        off += 3;
        size_t end = TrimExternalBracketsFromWebLink(str, start.value(),
                                                     str.GetLength() - 1);
        end = FindWebLinkEnding(str, off, end);
        if (end > off) {  // Non-empty host name.
          *nStart = start.value();
          *nCount = end - start.value() + 1;
          *strBeCheck = strBeCheck->Mid(*nStart, *nCount);
          return true;
        }
      }
    }
  }

  // When there is no scheme, try to find url starting with "www.".
  start = str.Find(kWWWAddrStart);
  if (start.has_value() && len > start.value() + kWWWAddrStartLen) {
    size_t end = TrimExternalBracketsFromWebLink(str, start.value(),
                                                 str.GetLength() - 1);
    end = FindWebLinkEnding(str, start.value(), end);
    if (end > start.value() + kWWWAddrStartLen) {
      *nStart = start.value();
      *nCount = end - start.value() + 1;
      *strBeCheck = L"http://" + strBeCheck->Mid(*nStart, *nCount);
      return true;
    }
  }
  return false;
}

bool CPDF_LinkExtract::CheckMailLink(WideString* str) {
  auto aPos = str->Find(L'@');
  // Invalid when no '@' or when starts/ends with '@'.
  if (!aPos.has_value() || aPos.value() == 0 || aPos == str->GetLength() - 1)
    return false;

  // Check the local part.
  size_t pPos = aPos.value();  // Used to track the position of '@' or '.'.
  for (size_t i = aPos.value(); i > 0; i--) {
    wchar_t ch = (*str)[i - 1];
    if (ch == L'_' || ch == L'-' || FXSYS_iswalnum(ch))
      continue;

    if (ch != L'.' || i == pPos || i == 1) {
      if (i == aPos.value()) {
        // There is '.' or invalid char before '@'.
        return false;
      }
      // End extracting for other invalid chars, '.' at the beginning, or
      // consecutive '.'.
      size_t removed_len = i == pPos ? i + 1 : i;
      *str = str->Right(str->GetLength() - removed_len);
      break;
    }
    // Found a valid '.'.
    pPos = i - 1;
  }

  // Check the domain name part.
  aPos = str->Find(L'@');
  if (!aPos.has_value() || aPos.value() == 0)
    return false;

  str->TrimRight(L'.');
  // At least one '.' in domain name, but not at the beginning.
  // TODO(weili): RFC5322 allows domain names to be a local name without '.'.
  // Check whether we should remove this check.
  auto ePos = str->Find(L'.', aPos.value() + 1);
  if (!ePos.has_value() || ePos.value() == aPos.value() + 1)
    return false;

  // Validate all other chars in domain name.
  size_t nLen = str->GetLength();
  pPos = 0;  // Used to track the position of '.'.
  for (size_t i = aPos.value() + 1; i < nLen; i++) {
    wchar_t wch = (*str)[i];
    if (wch == L'-' || FXSYS_iswalnum(wch))
      continue;

    if (wch != L'.' || i == pPos + 1) {
      // Domain name should end before invalid char.
      size_t host_end = i == pPos + 1 ? i - 2 : i - 1;
      if (pPos > 0 && host_end - aPos.value() >= 3) {
        // Trim the ending invalid chars if there is at least one '.' and name.
        *str = str->Left(host_end + 1);
        break;
      }
      return false;
    }
    pPos = i;
  }

  if (!str->Contains(L"mailto:"))
    *str = L"mailto:" + *str;

  return true;
}

WideString CPDF_LinkExtract::GetURL(size_t index) const {
  return index < m_LinkArray.size() ? m_LinkArray[index].m_strUrl
                                    : WideString();
}

std::vector<CFX_FloatRect> CPDF_LinkExtract::GetRects(size_t index) const {
  if (index >= m_LinkArray.size())
    return std::vector<CFX_FloatRect>();

  return m_pTextPage->GetRectArray(m_LinkArray[index].m_Start,
                                   m_LinkArray[index].m_Count);
}
