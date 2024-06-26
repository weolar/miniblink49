// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_FM2JS_CXFA_FMLEXER_H_
#define XFA_FXFA_FM2JS_CXFA_FMLEXER_H_

#include "core/fxcrt/fx_string.h"

enum XFA_FM_TOKEN {
  TOKand,
  TOKlparen,
  TOKrparen,
  TOKmul,
  TOKplus,
  TOKcomma,
  TOKminus,
  TOKdot,
  TOKdiv,
  TOKlt,
  TOKassign,
  TOKgt,
  TOKlbracket,
  TOKrbracket,
  TOKor,
  TOKdotscream,
  TOKdotstar,
  TOKdotdot,
  TOKle,
  TOKne,
  TOKeq,
  TOKge,
  TOKdo,
  TOKkseq,
  TOKksge,
  TOKksgt,
  TOKif,
  TOKin,
  TOKksle,
  TOKkslt,
  TOKksne,
  TOKksor,
  TOKnull,
  TOKbreak,
  TOKksand,
  TOKend,
  TOKeof,
  TOKfor,
  TOKnan,
  TOKksnot,
  TOKvar,
  TOKthen,
  TOKelse,
  TOKexit,
  TOKdownto,
  TOKreturn,
  TOKinfinity,
  TOKendwhile,
  TOKforeach,
  TOKendfunc,
  TOKelseif,
  TOKwhile,
  TOKendfor,
  TOKthrow,
  TOKstep,
  TOKupto,
  TOKcontinue,
  TOKfunc,
  TOKendif,
  TOKstar,
  TOKidentifier,
  TOKunderscore,
  TOKdollar,
  TOKexclamation,
  TOKcall,
  TOKstring,
  TOKnumber,
  TOKreserver
};

struct XFA_FMKeyword {
  XFA_FM_TOKEN m_type;
  const char* m_keyword;  // Raw, POD struct.
};

class CXFA_FMToken {
 public:
  CXFA_FMToken();
  explicit CXFA_FMToken(XFA_FM_TOKEN token);
  CXFA_FMToken(const CXFA_FMToken&);
  ~CXFA_FMToken();

#ifndef NDEBUG
  WideString ToDebugString() const;
#endif  // NDEBUG

  WideStringView m_string;
  XFA_FM_TOKEN m_type;
};

class CXFA_FMLexer {
 public:
  explicit CXFA_FMLexer(WideStringView wsFormcalc);
  ~CXFA_FMLexer();

  CXFA_FMToken NextToken();
  bool IsComplete() const { return m_cursor >= m_end; }

 private:
  CXFA_FMToken AdvanceForNumber();
  CXFA_FMToken AdvanceForString();
  CXFA_FMToken AdvanceForIdentifier();
  void AdvanceForComment();

  void RaiseError() { m_lexer_error = true; }

  const wchar_t* m_cursor;
  const wchar_t* const m_end;
  bool m_lexer_error;
};

#endif  // XFA_FXFA_FM2JS_CXFA_FMLEXER_H_
