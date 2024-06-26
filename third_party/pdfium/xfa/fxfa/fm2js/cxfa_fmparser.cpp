// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/fm2js/cxfa_fmparser.h"

#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/autorestorer.h"
#include "third_party/base/ptr_util.h"

namespace {

constexpr unsigned int kMaxParseDepth = 1250;
constexpr unsigned int kMaxPostExpressions = 256;
constexpr unsigned int kMaxExpressionListSize = 10000;

}  // namespace

CXFA_FMParser::CXFA_FMParser(WideStringView wsFormcalc)
    : m_lexer(pdfium::MakeUnique<CXFA_FMLexer>(wsFormcalc)),
      m_error(false),
      m_parse_depth(0),
      m_max_parse_depth(kMaxParseDepth) {}

CXFA_FMParser::~CXFA_FMParser() = default;

std::unique_ptr<CXFA_FMAST> CXFA_FMParser::Parse() {
  m_token = m_lexer->NextToken();
  if (HasError())
    return nullptr;

  auto expressions = ParseExpressionList();
  if (HasError())
    return nullptr;

  // We failed to parse all of the input so something has gone wrong.
  if (!m_lexer->IsComplete())
    return nullptr;

  return pdfium::MakeUnique<CXFA_FMAST>(std::move(expressions));
}

bool CXFA_FMParser::NextToken() {
  if (HasError())
    return false;

  m_token = m_lexer->NextToken();
  while (!HasError() && m_token.m_type == TOKreserver)
    m_token = m_lexer->NextToken();
  return !HasError();
}

bool CXFA_FMParser::CheckThenNext(XFA_FM_TOKEN op) {
  if (HasError())
    return false;

  if (m_token.m_type != op) {
    m_error = true;
    return false;
  }
  return NextToken();
}

bool CXFA_FMParser::IncrementParseDepthAndCheck() {
  return ++m_parse_depth < m_max_parse_depth;
}

std::vector<std::unique_ptr<CXFA_FMExpression>>
CXFA_FMParser::ParseExpressionList() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return std::vector<std::unique_ptr<CXFA_FMExpression>>();

  std::vector<std::unique_ptr<CXFA_FMExpression>> expressions;
  while (!HasError()) {
    if (m_token.m_type == TOKeof || m_token.m_type == TOKendfunc ||
        m_token.m_type == TOKendif || m_token.m_type == TOKelseif ||
        m_token.m_type == TOKelse || m_token.m_type == TOKendwhile ||
        m_token.m_type == TOKendfor || m_token.m_type == TOKend ||
        m_token.m_type == TOKendfunc || m_token.m_type == TOKreserver) {
      break;
    }

    std::unique_ptr<CXFA_FMExpression> expr =
        m_token.m_type == TOKfunc ? ParseFunction() : ParseExpression();
    if (!expr) {
      m_error = true;
      return std::vector<std::unique_ptr<CXFA_FMExpression>>();
    }

    if (expressions.size() >= kMaxExpressionListSize) {
      m_error = true;
      return std::vector<std::unique_ptr<CXFA_FMExpression>>();
    }

    expressions.push_back(std::move(expr));
  }
  return expressions;
}

// Func := 'func' Identifier '(' ParameterList ')' do ExpressionList 'endfunc'
// ParamterList := (Not actually defined in the grammar) .....
//                 (Identifier (',' Identifier)*)?
std::unique_ptr<CXFA_FMExpression> CXFA_FMParser::ParseFunction() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;
  if (!CheckThenNext(TOKfunc))
    return nullptr;
  if (m_token.m_type != TOKidentifier)
    return nullptr;

  WideStringView ident = m_token.m_string;
  if (!NextToken())
    return nullptr;
  if (!CheckThenNext(TOKlparen))
    return nullptr;

  std::vector<WideStringView> arguments;
  bool last_was_comma = false;
  while (1) {
    if (m_token.m_type == TOKrparen)
      break;
    if (m_token.m_type != TOKidentifier)
      return nullptr;

    last_was_comma = false;

    arguments.push_back(m_token.m_string);
    if (!NextToken())
      return nullptr;
    if (m_token.m_type != TOKcomma)
      continue;

    last_was_comma = true;
    if (!NextToken())
      return nullptr;
  }
  if (last_was_comma || !CheckThenNext(TOKrparen))
    return nullptr;
  if (!CheckThenNext(TOKdo))
    return nullptr;

  std::vector<std::unique_ptr<CXFA_FMExpression>> expressions;
  if (m_token.m_type != TOKendfunc)
    expressions = ParseExpressionList();

  if (!CheckThenNext(TOKendfunc))
    return nullptr;

  return pdfium::MakeUnique<CXFA_FMFunctionDefinition>(
      ident, std::move(arguments), std::move(expressions));
}

// Expression := IfExpression | WhileExpression | ForExpression |
//               ForEachExpression | AssignmentExpression |
//               DeclarationExpression | SimpleExpression
std::unique_ptr<CXFA_FMExpression> CXFA_FMParser::ParseExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  std::unique_ptr<CXFA_FMExpression> expr;
  switch (m_token.m_type) {
    case TOKvar:
      expr = ParseDeclarationExpression();
      break;
    case TOKnull:
    case TOKnumber:
    case TOKstring:
    case TOKplus:
    case TOKminus:
    case TOKksnot:
    case TOKidentifier:
    case TOKlparen:
      expr = ParseExpExpression();
      break;
    case TOKif:
      expr = ParseIfExpression();
      break;
    case TOKwhile:
      expr = ParseWhileExpression();
      break;
    case TOKfor:
      expr = ParseForExpression();
      break;
    case TOKforeach:
      expr = ParseForeachExpression();
      break;
    case TOKdo:
      expr = ParseDoExpression();
      break;
    case TOKbreak:
      expr = pdfium::MakeUnique<CXFA_FMBreakExpression>();
      if (!NextToken())
        return nullptr;
      break;
    case TOKcontinue:
      expr = pdfium::MakeUnique<CXFA_FMContinueExpression>();
      if (!NextToken())
        return nullptr;
      break;
    default:
      return nullptr;
  }
  return expr;
}

// Declaration := 'var' Variable | 'var' Variable '=' SimpleExpression |
//           'Func' Identifier '(' ParameterList ')' do ExpressionList 'EndFunc'
// TODO(dsinclair): We appear to be handling the 'func' case elsewhere.
std::unique_ptr<CXFA_FMExpression> CXFA_FMParser::ParseDeclarationExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  WideStringView ident;
  if (!NextToken())
    return nullptr;
  if (m_token.m_type != TOKidentifier)
    return nullptr;

  ident = m_token.m_string;
  if (!NextToken())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> expr;
  if (m_token.m_type == TOKassign) {
    if (!NextToken())
      return nullptr;

    expr = ParseSimpleExpression();
    if (!expr)
      return nullptr;
  }

  return pdfium::MakeUnique<CXFA_FMVarExpression>(ident, std::move(expr));
}

// SimpleExpression := LogicalOrExpression
std::unique_ptr<CXFA_FMSimpleExpression>
CXFA_FMParser::ParseSimpleExpression() {
  if (HasError())
    return nullptr;

  return ParseLogicalOrExpression();
}

// Exp := SimpleExpression ( '=' SimpleExpression )?
std::unique_ptr<CXFA_FMExpression> CXFA_FMParser::ParseExpExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> pExp1 = ParseSimpleExpression();
  if (!pExp1)
    return nullptr;

  if (m_token.m_type == TOKassign) {
    if (!NextToken())
      return nullptr;

    std::unique_ptr<CXFA_FMSimpleExpression> pExp2 = ParseSimpleExpression();
    if (!pExp2)
      return nullptr;

    pExp1 = pdfium::MakeUnique<CXFA_FMAssignExpression>(
        TOKassign, std::move(pExp1), std::move(pExp2));
  }
  return pdfium::MakeUnique<CXFA_FMExpExpression>(std::move(pExp1));
}

// LogicalOr := LogicalAndExpression |
//              LogicalOrExpression LogicalOrOperator LogicalAndExpression
std::unique_ptr<CXFA_FMSimpleExpression>
CXFA_FMParser::ParseLogicalOrExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> e1 = ParseLogicalAndExpression();
  if (!e1)
    return nullptr;

  // TODO(dsinclair): Is this for() needed?
  for (;;) {
    if (!IncrementParseDepthAndCheck())
      return nullptr;

    switch (m_token.m_type) {
      case TOKor:
      case TOKksor: {
        if (!NextToken())
          return nullptr;

        std::unique_ptr<CXFA_FMSimpleExpression> e2(
            ParseLogicalAndExpression());
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMLogicalOrExpression>(
            TOKor, std::move(e1), std::move(e2));
        continue;
      }
      default:
        break;
    }
    break;
  }
  return e1;
}

// LogicalAnd := EqualityExpression |
//               LogicalAndExpression LogicalAndOperator EqualityExpression
std::unique_ptr<CXFA_FMSimpleExpression>
CXFA_FMParser::ParseLogicalAndExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> e1 = ParseEqualityExpression();
  if (!e1)
    return nullptr;

  // TODO(dsinclair): Is this for() needed?
  for (;;) {
    if (!IncrementParseDepthAndCheck())
      return nullptr;

    switch (m_token.m_type) {
      case TOKand:
      case TOKksand: {
        if (!NextToken())
          return nullptr;

        std::unique_ptr<CXFA_FMSimpleExpression> e2 = ParseEqualityExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMLogicalAndExpression>(
            TOKand, std::move(e1), std::move(e2));
        continue;
      }
      default:
        break;
    }
    break;
  }
  return e1;
}

// Equality := RelationExpression |
//             EqualityExpression EqulaityOperator RelationalExpression
std::unique_ptr<CXFA_FMSimpleExpression>
CXFA_FMParser::ParseEqualityExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> e1 = ParseRelationalExpression();
  if (!e1)
    return nullptr;

  // TODO(dsinclair): Is this for() needed?
  for (;;) {
    if (!IncrementParseDepthAndCheck())
      return nullptr;

    switch (m_token.m_type) {
      case TOKeq:
      case TOKkseq: {
        if (!NextToken())
          return nullptr;

        std::unique_ptr<CXFA_FMSimpleExpression> e2 =
            ParseRelationalExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMEqualExpression>(TOKeq, std::move(e1),
                                                        std::move(e2));
        continue;
      }
      case TOKne:
      case TOKksne: {
        if (!NextToken())
          return nullptr;

        std::unique_ptr<CXFA_FMSimpleExpression> e2 =
            ParseRelationalExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMNotEqualExpression>(TOKne, std::move(e1),
                                                           std::move(e2));
        continue;
      }
      default:
        break;
    }
    break;
  }
  return e1;
}

// Relational := AdditiveExpression |
//               RelationalExpression RelationalOperator AdditiveExpression
std::unique_ptr<CXFA_FMSimpleExpression>
CXFA_FMParser::ParseRelationalExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> e1 = ParseAdditiveExpression();
  if (!e1)
    return nullptr;

  // TODO(dsinclair): Is this for() needed?
  for (;;) {
    if (!IncrementParseDepthAndCheck())
      return nullptr;

    std::unique_ptr<CXFA_FMSimpleExpression> e2;
    switch (m_token.m_type) {
      case TOKlt:
      case TOKkslt:
        if (!NextToken())
          return nullptr;

        e2 = ParseAdditiveExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMLtExpression>(TOKlt, std::move(e1),
                                                     std::move(e2));
        continue;
      case TOKgt:
      case TOKksgt:
        if (!NextToken())
          return nullptr;

        e2 = ParseAdditiveExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMGtExpression>(TOKgt, std::move(e1),
                                                     std::move(e2));
        continue;
      case TOKle:
      case TOKksle:
        if (!NextToken())
          return nullptr;

        e2 = ParseAdditiveExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMLeExpression>(TOKle, std::move(e1),
                                                     std::move(e2));
        continue;
      case TOKge:
      case TOKksge:
        if (!NextToken())
          return nullptr;

        e2 = ParseAdditiveExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMGeExpression>(TOKge, std::move(e1),
                                                     std::move(e2));
        continue;
      default:
        break;
    }
    break;
  }
  return e1;
}

// Additive := MultiplicativeExpression |
//             AdditiveExpression AdditiveOperator MultiplicativeExpression
std::unique_ptr<CXFA_FMSimpleExpression>
CXFA_FMParser::ParseAdditiveExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> e1 = ParseMultiplicativeExpression();
  if (!e1)
    return nullptr;

  // TODO(dsinclair): Is this for() needed?
  for (;;) {
    if (!IncrementParseDepthAndCheck())
      return nullptr;

    std::unique_ptr<CXFA_FMSimpleExpression> e2;
    switch (m_token.m_type) {
      case TOKplus:
        if (!NextToken())
          return nullptr;

        e2 = ParseMultiplicativeExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMPlusExpression>(TOKplus, std::move(e1),
                                                       std::move(e2));
        continue;
      case TOKminus:
        if (!NextToken())
          return nullptr;

        e2 = ParseMultiplicativeExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMMinusExpression>(TOKminus, std::move(e1),
                                                        std::move(e2));
        continue;
      default:
        break;
    }
    break;
  }
  return e1;
}

// Multiplicative := UnaryExpression |
//                 MultiplicateExpression MultiplicativeOperator UnaryExpression
std::unique_ptr<CXFA_FMSimpleExpression>
CXFA_FMParser::ParseMultiplicativeExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> e1 = ParseUnaryExpression();
  if (!e1)
    return nullptr;

  // TODO(dsinclair): Is this for() needed?
  for (;;) {
    if (!IncrementParseDepthAndCheck())
      return nullptr;

    std::unique_ptr<CXFA_FMSimpleExpression> e2;
    switch (m_token.m_type) {
      case TOKmul:
        if (!NextToken())
          return nullptr;

        e2 = ParseUnaryExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMMulExpression>(TOKmul, std::move(e1),
                                                      std::move(e2));
        continue;
      case TOKdiv:
        if (!NextToken())
          return nullptr;

        e2 = ParseUnaryExpression();
        if (!e2)
          return nullptr;

        e1 = pdfium::MakeUnique<CXFA_FMDivExpression>(TOKdiv, std::move(e1),
                                                      std::move(e2));
        continue;
      default:
        break;
    }
    break;
  }
  return e1;
}

// Unary := PrimaryExpression | UnaryOperator UnaryExpression
std::unique_ptr<CXFA_FMSimpleExpression> CXFA_FMParser::ParseUnaryExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> expr;
  switch (m_token.m_type) {
    case TOKplus:
      if (!NextToken())
        return nullptr;

      expr = ParseUnaryExpression();
      if (!expr)
        return nullptr;

      expr = pdfium::MakeUnique<CXFA_FMPosExpression>(std::move(expr));
      break;
    case TOKminus:
      if (!NextToken())
        return nullptr;

      expr = ParseUnaryExpression();
      if (!expr)
        return nullptr;

      expr = pdfium::MakeUnique<CXFA_FMNegExpression>(std::move(expr));
      break;
    case TOKksnot:
      if (!NextToken())
        return nullptr;

      expr = ParseUnaryExpression();
      if (!expr)
        return nullptr;

      expr = pdfium::MakeUnique<CXFA_FMNotExpression>(std::move(expr));
      break;
    default:
      return ParsePrimaryExpression();
  }
  return expr;
}

// Primary := Literal | FunctionCall | Accessor ('.*' )? |
//           '(' SimpleExpression ')'
std::unique_ptr<CXFA_FMSimpleExpression>
CXFA_FMParser::ParsePrimaryExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> expr = ParseLiteral();
  if (expr)
    return NextToken() ? std::move(expr) : nullptr;

  switch (m_token.m_type) {
    case TOKidentifier: {
      WideStringView wsIdentifier(m_token.m_string);
      if (!NextToken())
        return nullptr;
      if (m_token.m_type == TOKlbracket) {
        std::unique_ptr<CXFA_FMSimpleExpression> s = ParseIndexExpression();
        if (!s)
          return nullptr;

        expr = pdfium::MakeUnique<CXFA_FMDotAccessorExpression>(
            nullptr, TOKdot, wsIdentifier, std::move(s));
        if (!expr)
          return nullptr;
        if (!NextToken())
          return nullptr;
      } else {
        expr = pdfium::MakeUnique<CXFA_FMIdentifierExpression>(wsIdentifier);
      }
      break;
    }
    case TOKlparen:
      expr = ParseParenExpression();
      if (!expr)
        return nullptr;
      break;
    default:
      return nullptr;
  }
  return ParsePostExpression(std::move(expr));
}

// Literal := String | Number | Null
std::unique_ptr<CXFA_FMSimpleExpression> CXFA_FMParser::ParseLiteral() {
  switch (m_token.m_type) {
    case TOKnumber:
      return pdfium::MakeUnique<CXFA_FMNumberExpression>(m_token.m_string);
    case TOKstring:
      return pdfium::MakeUnique<CXFA_FMStringExpression>(m_token.m_string);
    case TOKnull:
      return pdfium::MakeUnique<CXFA_FMNullExpression>();
    default:
      return nullptr;
  }
}

// TODO(dsinclair): Make this match up to the grammar
// I believe this is parsing the accessor ( '.' | '..' | '.#' )
std::unique_ptr<CXFA_FMSimpleExpression> CXFA_FMParser::ParsePostExpression(
    std::unique_ptr<CXFA_FMSimpleExpression> expr) {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  size_t expr_count = 0;
  while (1) {
    ++expr_count;
    // Limit the number of expressions allowed in the post expression statement.
    // If we don't do this then its possible to generate a stack overflow
    // by having a very large number of things like .. expressions.
    if (expr_count > kMaxPostExpressions)
      return nullptr;

    switch (m_token.m_type) {
      case TOKlparen: {
        std::unique_ptr<std::vector<std::unique_ptr<CXFA_FMSimpleExpression>>>
            expressions = ParseArgumentList();
        if (!expressions)
          return nullptr;

        expr = pdfium::MakeUnique<CXFA_FMCallExpression>(
            std::move(expr), std::move(*expressions), false);
        if (!NextToken())
          return nullptr;
        if (m_token.m_type != TOKlbracket)
          continue;

        std::unique_ptr<CXFA_FMSimpleExpression> s = ParseIndexExpression();
        if (!s)
          return nullptr;

        expr = pdfium::MakeUnique<CXFA_FMDotAccessorExpression>(
            std::move(expr), TOKcall, WideStringView(), std::move(s));
        break;
      }
      case TOKdot: {
        if (!NextToken())
          return nullptr;
        if (m_token.m_type != TOKidentifier)
          return nullptr;

        WideStringView tempStr = m_token.m_string;
        if (!NextToken())
          return nullptr;
        if (m_token.m_type == TOKlparen) {
          std::unique_ptr<std::vector<std::unique_ptr<CXFA_FMSimpleExpression>>>
              expressions = ParseArgumentList();
          if (!expressions)
            return nullptr;

          auto pIdentifier =
              pdfium::MakeUnique<CXFA_FMIdentifierExpression>(tempStr);
          auto pExpCall = pdfium::MakeUnique<CXFA_FMCallExpression>(
              std::move(pIdentifier), std::move(*expressions), true);
          expr = pdfium::MakeUnique<CXFA_FMMethodCallExpression>(
              std::move(expr), std::move(pExpCall));
          if (!NextToken())
            return nullptr;
          if (m_token.m_type != TOKlbracket)
            continue;

          std::unique_ptr<CXFA_FMSimpleExpression> s = ParseIndexExpression();
          if (!s)
            return nullptr;

          expr = pdfium::MakeUnique<CXFA_FMDotAccessorExpression>(
              std::move(expr), TOKcall, WideStringView(), std::move(s));
        } else if (m_token.m_type == TOKlbracket) {
          std::unique_ptr<CXFA_FMSimpleExpression> s = ParseIndexExpression();
          if (!s)
            return nullptr;

          expr = pdfium::MakeUnique<CXFA_FMDotAccessorExpression>(
              std::move(expr), TOKdot, tempStr, std::move(s));
        } else {
          std::unique_ptr<CXFA_FMSimpleExpression> s =
              pdfium::MakeUnique<CXFA_FMIndexExpression>(ACCESSOR_NO_INDEX,
                                                         nullptr, false);
          expr = pdfium::MakeUnique<CXFA_FMDotAccessorExpression>(
              std::move(expr), TOKdot, tempStr, std::move(s));
          continue;
        }
        break;
      }
      case TOKdotdot: {
        if (!NextToken())
          return nullptr;
        if (m_token.m_type != TOKidentifier)
          return nullptr;

        WideStringView tempStr = m_token.m_string;
        if (!NextToken())
          return nullptr;
        if (m_token.m_type == TOKlbracket) {
          std::unique_ptr<CXFA_FMSimpleExpression> s = ParseIndexExpression();
          if (!s)
            return nullptr;

          expr = pdfium::MakeUnique<CXFA_FMDotDotAccessorExpression>(
              std::move(expr), TOKdotdot, tempStr, std::move(s));
        } else {
          std::unique_ptr<CXFA_FMSimpleExpression> s =
              pdfium::MakeUnique<CXFA_FMIndexExpression>(ACCESSOR_NO_INDEX,
                                                         nullptr, false);
          expr = pdfium::MakeUnique<CXFA_FMDotDotAccessorExpression>(
              std::move(expr), TOKdotdot, tempStr, std::move(s));
          continue;
        }
        break;
      }
      case TOKdotscream: {
        if (!NextToken())
          return nullptr;
        if (m_token.m_type != TOKidentifier)
          return nullptr;

        WideStringView tempStr = m_token.m_string;
        if (!NextToken())
          return nullptr;

        if (m_token.m_type != TOKlbracket) {
          std::unique_ptr<CXFA_FMSimpleExpression> s =
              pdfium::MakeUnique<CXFA_FMIndexExpression>(ACCESSOR_NO_INDEX,
                                                         nullptr, false);
          expr = pdfium::MakeUnique<CXFA_FMDotAccessorExpression>(
              std::move(expr), TOKdotscream, tempStr, std::move(s));
          continue;
        }

        std::unique_ptr<CXFA_FMSimpleExpression> s = ParseIndexExpression();
        if (!s)
          return nullptr;

        expr = pdfium::MakeUnique<CXFA_FMDotAccessorExpression>(
            std::move(expr), TOKdotscream, tempStr, std::move(s));
        break;
      }
      case TOKdotstar: {
        std::unique_ptr<CXFA_FMSimpleExpression> s =
            pdfium::MakeUnique<CXFA_FMIndexExpression>(ACCESSOR_NO_INDEX,
                                                       nullptr, false);
        expr = pdfium::MakeUnique<CXFA_FMDotAccessorExpression>(
            std::move(expr), TOKdotstar, L"*", std::move(s));
        break;
      }
      default:
        return expr;
    }
    if (!NextToken())
      return nullptr;
  }
  return expr;
}

// Argument lists are zero or more comma seperated simple expressions found
// between '(' and ')'
std::unique_ptr<std::vector<std::unique_ptr<CXFA_FMSimpleExpression>>>
CXFA_FMParser::ParseArgumentList() {
  if (m_token.m_type != TOKlparen || !NextToken())
    return nullptr;

  auto expressions = pdfium::MakeUnique<
      std::vector<std::unique_ptr<CXFA_FMSimpleExpression>>>();
  bool first_arg = true;
  while (m_token.m_type != TOKrparen) {
    if (first_arg) {
      first_arg = false;
    } else {
      if (m_token.m_type != TOKcomma || !NextToken())
        return nullptr;
    }

    std::unique_ptr<CXFA_FMSimpleExpression> exp = ParseSimpleExpression();
    if (!exp)
      return nullptr;

    expressions->push_back(std::move(exp));
    if (expressions->size() > kMaxPostExpressions)
      return nullptr;
  }

  return expressions;
}

// Index := '[' ('*' | '+' SimpleExpression | '-' SimpleExpression) ']'
std::unique_ptr<CXFA_FMSimpleExpression> CXFA_FMParser::ParseIndexExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;
  if (!CheckThenNext(TOKlbracket))
    return nullptr;

  if (m_token.m_type == TOKmul) {
    auto pExp = pdfium::MakeUnique<CXFA_FMIndexExpression>(
        ACCESSOR_NO_RELATIVEINDEX, nullptr, true);
    if (!pExp || !NextToken())
      return nullptr;

    // TODO(dsinclair): This should CheckThenNext(TOKrbracket) but need to clean
    // up the callsites.
    if (m_token.m_type != TOKrbracket)
      return nullptr;
    return pExp;
  }

  XFA_FM_AccessorIndex accessorIndex = ACCESSOR_NO_RELATIVEINDEX;
  if (m_token.m_type == TOKplus) {
    accessorIndex = ACCESSOR_POSITIVE_INDEX;
    if (!NextToken())
      return nullptr;
  } else if (m_token.m_type == TOKminus) {
    accessorIndex = ACCESSOR_NEGATIVE_INDEX;
    if (!NextToken())
      return nullptr;
  }

  std::unique_ptr<CXFA_FMSimpleExpression> s = ParseSimpleExpression();
  if (!s)
    return nullptr;
  if (m_token.m_type != TOKrbracket)
    return nullptr;

  return pdfium::MakeUnique<CXFA_FMIndexExpression>(accessorIndex, std::move(s),
                                                    false);
}

// Paren := '(' SimpleExpression ')'
std::unique_ptr<CXFA_FMSimpleExpression> CXFA_FMParser::ParseParenExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  if (!CheckThenNext(TOKlparen))
    return nullptr;
  if (m_token.m_type == TOKrparen)
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> pExp1 = ParseSimpleExpression();
  if (!pExp1)
    return nullptr;

  if (!CheckThenNext(TOKrparen))
    return nullptr;
  return pExp1;
}

// If := 'if' '(' SimpleExpression ')' 'then' ExpressionList
//       ('elseif' '(' SimpleExpression ')' 'then' ExpressionList)*
//       ('else' ExpressionList)?
//       'endif'
std::unique_ptr<CXFA_FMExpression> CXFA_FMParser::ParseIfExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;

  if (!CheckThenNext(TOKif))
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> pCondition = ParseParenExpression();
  if (!pCondition)
    return nullptr;
  if (!CheckThenNext(TOKthen))
    return nullptr;

  auto pIfExpressions =
      pdfium::MakeUnique<CXFA_FMBlockExpression>(ParseExpressionList());

  std::vector<std::unique_ptr<CXFA_FMIfExpression>> pElseIfExpressions;
  while (m_token.m_type == TOKelseif) {
    if (!NextToken())
      return nullptr;

    auto elseIfCondition = ParseParenExpression();
    if (!elseIfCondition)
      return nullptr;
    if (!CheckThenNext(TOKthen))
      return nullptr;

    auto elseIfExprs = ParseExpressionList();
    pElseIfExpressions.push_back(pdfium::MakeUnique<CXFA_FMIfExpression>(
        std::move(elseIfCondition),
        pdfium::MakeUnique<CXFA_FMBlockExpression>(std::move(elseIfExprs)),
        std::vector<std::unique_ptr<CXFA_FMIfExpression>>(), nullptr));
  }

  std::unique_ptr<CXFA_FMExpression> pElseExpression;
  if (m_token.m_type == TOKelse) {
    if (!NextToken())
      return nullptr;

    pElseExpression =
        pdfium::MakeUnique<CXFA_FMBlockExpression>(ParseExpressionList());
  }
  if (!CheckThenNext(TOKendif))
    return nullptr;

  return pdfium::MakeUnique<CXFA_FMIfExpression>(
      std::move(pCondition), std::move(pIfExpressions),
      std::move(pElseIfExpressions), std::move(pElseExpression));
}

// While := 'while' '(' SimpleExpression ')' 'do' ExpressionList 'endwhile'
std::unique_ptr<CXFA_FMExpression> CXFA_FMParser::ParseWhileExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;
  if (!CheckThenNext(TOKwhile))
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> pCondition = ParseParenExpression();
  if (!pCondition || !CheckThenNext(TOKdo))
    return nullptr;

  auto exprs = ParseExpressionList();
  if (!CheckThenNext(TOKendwhile))
    return nullptr;

  return pdfium::MakeUnique<CXFA_FMWhileExpression>(
      std::move(pCondition),
      pdfium::MakeUnique<CXFA_FMBlockExpression>(std::move(exprs)));
}

// For := 'for' Assignment 'upto' Accessor ('step' SimpleExpression)?
//            'do' ExpressionList 'endfor' |
//         'for' Assignment 'downto' Accessor ('step' SimpleExpression)?
//            'do' ExpressionList 'endfor'
std::unique_ptr<CXFA_FMExpression> CXFA_FMParser::ParseForExpression() {
  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;
  if (!CheckThenNext(TOKfor))
    return nullptr;
  if (m_token.m_type != TOKidentifier)
    return nullptr;

  WideStringView wsVariant = m_token.m_string;
  if (!NextToken())
    return nullptr;
  if (!CheckThenNext(TOKassign))
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> pAssignment =
      ParseSimpleExpression();
  if (!pAssignment)
    return nullptr;

  int32_t iDirection = 0;
  if (m_token.m_type == TOKupto)
    iDirection = 1;
  else if (m_token.m_type == TOKdownto)
    iDirection = -1;
  else
    return nullptr;

  if (!NextToken())
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> pAccessor = ParseSimpleExpression();
  if (!pAccessor)
    return nullptr;

  std::unique_ptr<CXFA_FMSimpleExpression> pStep;
  if (m_token.m_type == TOKstep) {
    if (!NextToken())
      return nullptr;
    pStep = ParseSimpleExpression();
    if (!pStep)
      return nullptr;
  }
  if (!CheckThenNext(TOKdo))
    return nullptr;

  auto exprs = ParseExpressionList();
  if (!CheckThenNext(TOKendfor))
    return nullptr;

  return pdfium::MakeUnique<CXFA_FMForExpression>(
      wsVariant, std::move(pAssignment), std::move(pAccessor), iDirection,
      std::move(pStep),
      pdfium::MakeUnique<CXFA_FMBlockExpression>(std::move(exprs)));
}

// Foreach := 'foreach' Identifier 'in' '(' ArgumentList ')'
//            'do' ExpressionList 'endfor'
std::unique_ptr<CXFA_FMExpression> CXFA_FMParser::ParseForeachExpression() {
  if (m_token.m_type != TOKforeach)
    return nullptr;

  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;
  if (!CheckThenNext(TOKforeach))
    return nullptr;
  if (m_token.m_type != TOKidentifier)
    return nullptr;

  WideStringView wsIdentifier = m_token.m_string;
  if (!NextToken() || !CheckThenNext(TOKin) || !CheckThenNext(TOKlparen))
    return nullptr;

  std::vector<std::unique_ptr<CXFA_FMSimpleExpression>> pArgumentList;
  while (m_token.m_type != TOKrparen) {
    std::unique_ptr<CXFA_FMSimpleExpression> s = ParseSimpleExpression();
    if (!s)
      return nullptr;

    pArgumentList.push_back(std::move(s));
    if (m_token.m_type != TOKcomma)
      break;
    if (!NextToken())
      return nullptr;
  }
  // We must have arguments.
  if (pArgumentList.empty())
    return nullptr;
  if (!CheckThenNext(TOKrparen))
    return nullptr;

  auto exprs = ParseExpressionList();
  if (!CheckThenNext(TOKendfor))
    return nullptr;

  return pdfium::MakeUnique<CXFA_FMForeachExpression>(
      wsIdentifier, std::move(pArgumentList),
      pdfium::MakeUnique<CXFA_FMBlockExpression>(std::move(exprs)));
}

// Block := 'do' ExpressionList 'end'
std::unique_ptr<CXFA_FMExpression> CXFA_FMParser::ParseDoExpression() {
  if (m_token.m_type != TOKdo)
    return nullptr;

  AutoRestorer<unsigned long> restorer(&m_parse_depth);
  if (HasError() || !IncrementParseDepthAndCheck())
    return nullptr;
  if (!CheckThenNext(TOKdo))
    return nullptr;

  auto exprs = ParseExpressionList();
  if (!CheckThenNext(TOKend))
    return nullptr;

  return pdfium::MakeUnique<CXFA_FMDoExpression>(
      pdfium::MakeUnique<CXFA_FMBlockExpression>(std::move(exprs)));
}

bool CXFA_FMParser::HasError() const {
  return m_error || m_token.m_type == TOKreserver;
}
