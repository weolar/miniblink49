// Copright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/fm2js/cxfa_fmlexer.h"

#include <algorithm>

#include "core/fxcrt/fx_extension.h"
#include "third_party/icu/source/common/unicode/uchar.h"

namespace {

bool IsFormCalcCharacter(wchar_t c) {
  return (c >= 0x09 && c <= 0x0D) || (c >= 0x20 && c <= 0xd7FF) ||
         (c >= 0xE000 && c <= 0xFFFD);
}

bool IsIdentifierCharacter(wchar_t c) {
  return u_isalnum(c) || c == 0x005F ||  // '_'
         c == 0x0024;                    // '$'
}

bool IsInitialIdentifierCharacter(wchar_t c) {
  return u_isalpha(c) || c == 0x005F ||  // '_'
         c == 0x0024 ||                  // '$'
         c == 0x0021;                    // '!'
}

bool IsWhitespaceCharacter(wchar_t c) {
  return c == 0x0009 ||  // Horizontal tab
         c == 0x000B ||  // Vertical tab
         c == 0x000C ||  // Form feed
         c == 0x0020;    // Space
}

const XFA_FMKeyword keyWords[] = {
    {TOKdo, "do"},
    {TOKkseq, "eq"},
    {TOKksge, "ge"},
    {TOKksgt, "gt"},
    {TOKif, "if"},
    {TOKin, "in"},
    {TOKksle, "le"},
    {TOKkslt, "lt"},
    {TOKksne, "ne"},
    {TOKksor, "or"},
    {TOKnull, "null"},
    {TOKbreak, "break"},
    {TOKksand, "and"},
    {TOKend, "end"},
    {TOKeof, "eof"},
    {TOKfor, "for"},
    {TOKnan, "nan"},
    {TOKksnot, "not"},
    {TOKvar, "var"},
    {TOKthen, "then"},
    {TOKelse, "else"},
    {TOKexit, "exit"},
    {TOKdownto, "downto"},
    {TOKreturn, "return"},
    {TOKinfinity, "infinity"},
    {TOKendwhile, "endwhile"},
    {TOKforeach, "foreach"},
    {TOKendfunc, "endfunc"},
    {TOKelseif, "elseif"},
    {TOKwhile, "while"},
    {TOKendfor, "endfor"},
    {TOKthrow, "throw"},
    {TOKstep, "step"},
    {TOKupto, "upto"},
    {TOKcontinue, "continue"},
    {TOKfunc, "func"},
    {TOKendif, "endif"},
};

#ifndef NDEBUG
const char* const tokenStrings[] = {
    "TOKand",        "TOKlparen",     "TOKrparen",   "TOKmul",
    "TOKplus",       "TOKcomma",      "TOKminus",    "TOKdot",
    "TOKdiv",        "TOKlt",         "TOKassign",   "TOKgt",
    "TOKlbracket",   "TOKrbracket",   "TOKor",       "TOKdotscream",
    "TOKdotstar",    "TOKdotdot",     "TOKle",       "TOKne",
    "TOKeq",         "TOKge",         "TOKdo",       "TOKkseq",
    "TOKksge",       "TOKksgt",       "TOKif",       "TOKin",
    "TOKksle",       "TOKkslt",       "TOKksne",     "TOKksor",
    "TOKnull",       "TOKbreak",      "TOKksand",    "TOKend",
    "TOKeof",        "TOKfor",        "TOKnan",      "TOKksnot",
    "TOKvar",        "TOKthen",       "TOKelse",     "TOKexit",
    "TOKdownto",     "TOKreturn",     "TOKinfinity", "TOKendwhile",
    "TOKforeach",    "TOKendfunc",    "TOKelseif",   "TOKwhile",
    "TOKendfor",     "TOKthrow",      "TOKstep",     "TOKupto",
    "TOKcontinue",   "TOKfunc",       "TOKendif",    "TOKstar",
    "TOKidentifier", "TOKunderscore", "TOKdollar",   "TOKexclamation",
    "TOKcall",       "TOKstring",     "TOKnumber",   "TOKreserver",
};
#endif  // NDEBUG

XFA_FM_TOKEN TokenizeIdentifier(WideStringView str) {
  const XFA_FMKeyword* result =
      std::find_if(std::begin(keyWords), std::end(keyWords),
                   [str](const XFA_FMKeyword& iter) {
                     return str.EqualsASCII(iter.m_keyword);
                   });
  if (result != std::end(keyWords) && str.EqualsASCII(result->m_keyword))
    return result->m_type;
  return TOKidentifier;
}

}  // namespace

CXFA_FMToken::CXFA_FMToken(XFA_FM_TOKEN token) : m_type(token) {}

CXFA_FMToken::CXFA_FMToken() : CXFA_FMToken(TOKreserver) {}

CXFA_FMToken::CXFA_FMToken(const CXFA_FMToken&) = default;

CXFA_FMToken::~CXFA_FMToken() = default;

#ifndef NDEBUG
WideString CXFA_FMToken::ToDebugString() const {
  WideString str = WideString::FromASCII("type = ");
  str += WideString::FromASCII(tokenStrings[m_type]);
  str += WideString::FromASCII(", string = ");
  str += m_string;
  return str;
}
#endif  // NDEBUG

CXFA_FMLexer::CXFA_FMLexer(WideStringView wsFormCalc)
    : m_cursor(wsFormCalc.unterminated_c_str()),
      m_end(m_cursor + wsFormCalc.GetLength()),
      m_lexer_error(false) {}

CXFA_FMLexer::~CXFA_FMLexer() = default;

CXFA_FMToken CXFA_FMLexer::NextToken() {
  if (m_lexer_error)
    return CXFA_FMToken();

  while (!IsComplete() && *m_cursor) {
    if (!IsFormCalcCharacter(*m_cursor)) {
      RaiseError();
      return CXFA_FMToken();
    }

    switch (*m_cursor) {
      case '\n':
        ++m_cursor;
        break;
      case '\r':
        ++m_cursor;
        break;
      case ';':
        AdvanceForComment();
        break;
      case '"':
        return AdvanceForString();
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        return AdvanceForNumber();
      case '=':
        ++m_cursor;
        if (m_cursor >= m_end)
          return CXFA_FMToken(TOKassign);

        if (!IsFormCalcCharacter(*m_cursor)) {
          RaiseError();
          return CXFA_FMToken();
        }
        if (*m_cursor == '=') {
          ++m_cursor;
          return CXFA_FMToken(TOKeq);
        }
        return CXFA_FMToken(TOKassign);
      case '<':
        ++m_cursor;
        if (m_cursor >= m_end)
          return CXFA_FMToken(TOKlt);

        if (!IsFormCalcCharacter(*m_cursor)) {
          RaiseError();
          return CXFA_FMToken();
        }
        if (*m_cursor == '=') {
          ++m_cursor;
          return CXFA_FMToken(TOKle);
        }
        if (*m_cursor == '>') {
          ++m_cursor;
          return CXFA_FMToken(TOKne);
        }
        return CXFA_FMToken(TOKlt);
      case '>':
        ++m_cursor;
        if (m_cursor >= m_end)
          return CXFA_FMToken(TOKgt);

        if (!IsFormCalcCharacter(*m_cursor)) {
          RaiseError();
          return CXFA_FMToken();
        }
        if (*m_cursor == '=') {
          ++m_cursor;
          return CXFA_FMToken(TOKge);
        }
        return CXFA_FMToken(TOKgt);
      case ',':
        ++m_cursor;
        return CXFA_FMToken(TOKcomma);
      case '(':
        ++m_cursor;
        return CXFA_FMToken(TOKlparen);
      case ')':
        ++m_cursor;
        return CXFA_FMToken(TOKrparen);
      case '[':
        ++m_cursor;
        return CXFA_FMToken(TOKlbracket);
      case ']':
        ++m_cursor;
        return CXFA_FMToken(TOKrbracket);
      case '&':
        ++m_cursor;
        return CXFA_FMToken(TOKand);
      case '|':
        ++m_cursor;
        return CXFA_FMToken(TOKor);
      case '+':
        ++m_cursor;
        return CXFA_FMToken(TOKplus);
      case '-':
        ++m_cursor;
        return CXFA_FMToken(TOKminus);
      case '*':
        ++m_cursor;
        return CXFA_FMToken(TOKmul);
      case '/': {
        ++m_cursor;
        if (m_cursor >= m_end)
          return CXFA_FMToken(TOKdiv);

        if (!IsFormCalcCharacter(*m_cursor)) {
          RaiseError();
          return CXFA_FMToken();
        }
        if (*m_cursor != '/')
          return CXFA_FMToken(TOKdiv);

        AdvanceForComment();
        break;
      }
      case '.':
        ++m_cursor;
        if (m_cursor >= m_end)
          return CXFA_FMToken(TOKdot);

        if (!IsFormCalcCharacter(*m_cursor)) {
          RaiseError();
          return CXFA_FMToken();
        }

        if (*m_cursor == '.') {
          ++m_cursor;
          return CXFA_FMToken(TOKdotdot);
        }
        if (*m_cursor == '*') {
          ++m_cursor;
          return CXFA_FMToken(TOKdotstar);
        }
        if (*m_cursor == '#') {
          ++m_cursor;
          return CXFA_FMToken(TOKdotscream);
        }
        if (FXSYS_IsDecimalDigit(*m_cursor)) {
          --m_cursor;
          return AdvanceForNumber();
        }
        return CXFA_FMToken(TOKdot);
      default:
        if (IsWhitespaceCharacter(*m_cursor)) {
          ++m_cursor;
          break;
        }
        if (!IsInitialIdentifierCharacter(*m_cursor)) {
          RaiseError();
          return CXFA_FMToken();
        }
        return AdvanceForIdentifier();
    }
  }
  return CXFA_FMToken(TOKeof);
}

CXFA_FMToken CXFA_FMLexer::AdvanceForNumber() {
  // This will set end to the character after the end of the number.
  int32_t used_length = 0;
  if (m_cursor)
    FXSYS_wcstof(m_cursor, m_end - m_cursor, &used_length);

  const wchar_t* end = m_cursor + used_length;
  if (used_length == 0 || !end || FXSYS_iswalpha(*end)) {
    RaiseError();
    return CXFA_FMToken();
  }

  CXFA_FMToken token(TOKnumber);
  token.m_string =
      WideStringView(m_cursor, static_cast<size_t>(end - m_cursor));
  m_cursor = end;
  return token;
}

CXFA_FMToken CXFA_FMLexer::AdvanceForString() {
  CXFA_FMToken token(TOKstring);

  const wchar_t* start = m_cursor;
  ++m_cursor;
  while (!IsComplete() && *m_cursor) {
    if (!IsFormCalcCharacter(*m_cursor))
      break;

    if (*m_cursor == '"') {
      // Check for escaped "s, i.e. "".
      ++m_cursor;
      // If the end of the input has been reached it was not escaped.
      if (m_cursor >= m_end) {
        token.m_string =
            WideStringView(start, static_cast<size_t>(m_cursor - start));
        return token;
      }
      // If the next character is not a " then the end of the string has been
      // found.
      if (*m_cursor != '"') {
        if (!IsFormCalcCharacter(*m_cursor))
          break;

        token.m_string = WideStringView(start, (m_cursor - start));
        return token;
      }
    }
    ++m_cursor;
  }

  // Didn't find the end of the string.
  RaiseError();
  return CXFA_FMToken();
}

CXFA_FMToken CXFA_FMLexer::AdvanceForIdentifier() {
  const wchar_t* start = m_cursor;
  ++m_cursor;
  while (!IsComplete() && *m_cursor) {
    if (!IsFormCalcCharacter(*m_cursor)) {
      RaiseError();
      return CXFA_FMToken();
    }
    if (!IsIdentifierCharacter(*m_cursor))
      break;

    ++m_cursor;
  }

  WideStringView str =
      WideStringView(start, static_cast<size_t>(m_cursor - start));
  CXFA_FMToken token(TokenizeIdentifier(str));
  token.m_string = str;
  return token;
}

void CXFA_FMLexer::AdvanceForComment() {
  m_cursor++;
  while (!IsComplete() && *m_cursor) {
    if (!IsFormCalcCharacter(*m_cursor)) {
      RaiseError();
      return;
    }
    if (*m_cursor == L'\r') {
      ++m_cursor;
      return;
    }
    if (*m_cursor == L'\n') {
      ++m_cursor;
      return;
    }
    ++m_cursor;
  }
}
