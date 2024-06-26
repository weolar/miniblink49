// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_psengine.h"

#include <algorithm>
#include <utility>

#include "core/fpdfapi/parser/cpdf_simple_parser.h"
#include "core/fxcrt/fx_safe_types.h"
#include "core/fxcrt/fx_string.h"
#include "third_party/base/logging.h"
#include "third_party/base/ptr_util.h"

namespace {

struct PDF_PSOpName {
  const char* name;
  PDF_PSOP op;
};

constexpr PDF_PSOpName kPsOpNames[] = {
    {"abs", PSOP_ABS},
    {"add", PSOP_ADD},
    {"and", PSOP_AND},
    {"atan", PSOP_ATAN},
    {"bitshift", PSOP_BITSHIFT},
    {"ceiling", PSOP_CEILING},
    {"copy", PSOP_COPY},
    {"cos", PSOP_COS},
    {"cvi", PSOP_CVI},
    {"cvr", PSOP_CVR},
    {"div", PSOP_DIV},
    {"dup", PSOP_DUP},
    {"eq", PSOP_EQ},
    {"exch", PSOP_EXCH},
    {"exp", PSOP_EXP},
    {"false", PSOP_FALSE},
    {"floor", PSOP_FLOOR},
    {"ge", PSOP_GE},
    {"gt", PSOP_GT},
    {"idiv", PSOP_IDIV},
    {"if", PSOP_IF},
    {"ifelse", PSOP_IFELSE},
    {"index", PSOP_INDEX},
    {"le", PSOP_LE},
    {"ln", PSOP_LN},
    {"log", PSOP_LOG},
    {"lt", PSOP_LT},
    {"mod", PSOP_MOD},
    {"mul", PSOP_MUL},
    {"ne", PSOP_NE},
    {"neg", PSOP_NEG},
    {"not", PSOP_NOT},
    {"or", PSOP_OR},
    {"pop", PSOP_POP},
    {"roll", PSOP_ROLL},
    {"round", PSOP_ROUND},
    {"sin", PSOP_SIN},
    {"sqrt", PSOP_SQRT},
    {"sub", PSOP_SUB},
    {"true", PSOP_TRUE},
    {"truncate", PSOP_TRUNCATE},
    {"xor", PSOP_XOR},
};

}  // namespace

CPDF_PSOP::CPDF_PSOP()
    : m_op(PSOP_PROC), m_value(0), m_proc(pdfium::MakeUnique<CPDF_PSProc>()) {}

CPDF_PSOP::CPDF_PSOP(PDF_PSOP op) : m_op(op), m_value(0) {
  ASSERT(m_op != PSOP_CONST);
  ASSERT(m_op != PSOP_PROC);
}

CPDF_PSOP::CPDF_PSOP(float value) : m_op(PSOP_CONST), m_value(value) {}

CPDF_PSOP::~CPDF_PSOP() {}

float CPDF_PSOP::GetFloatValue() const {
  if (m_op == PSOP_CONST)
    return m_value;

  NOTREACHED();
  return 0;
}

CPDF_PSProc* CPDF_PSOP::GetProc() const {
  if (m_op == PSOP_PROC)
    return m_proc.get();
  NOTREACHED();
  return nullptr;
}

bool CPDF_PSEngine::Execute() {
  return m_MainProc.Execute(this);
}

CPDF_PSProc::CPDF_PSProc() {}
CPDF_PSProc::~CPDF_PSProc() {}

bool CPDF_PSProc::Parse(CPDF_SimpleParser* parser, int depth) {
  if (depth > kMaxDepth)
    return false;

  while (1) {
    ByteStringView word = parser->GetWord();
    if (word.IsEmpty())
      return false;

    if (word == "}")
      return true;

    if (word == "{") {
      m_Operators.push_back(pdfium::MakeUnique<CPDF_PSOP>());
      if (!m_Operators.back()->GetProc()->Parse(parser, depth + 1))
        return false;
      continue;
    }

    AddOperator(word);
  }
}

bool CPDF_PSProc::Execute(CPDF_PSEngine* pEngine) {
  for (size_t i = 0; i < m_Operators.size(); ++i) {
    const PDF_PSOP op = m_Operators[i]->GetOp();
    if (op == PSOP_PROC)
      continue;

    if (op == PSOP_CONST) {
      pEngine->Push(m_Operators[i]->GetFloatValue());
      continue;
    }

    if (op == PSOP_IF) {
      if (i == 0 || m_Operators[i - 1]->GetOp() != PSOP_PROC)
        return false;

      if (pEngine->PopInt())
        m_Operators[i - 1]->GetProc()->Execute(pEngine);
    } else if (op == PSOP_IFELSE) {
      if (i < 2 || m_Operators[i - 1]->GetOp() != PSOP_PROC ||
          m_Operators[i - 2]->GetOp() != PSOP_PROC) {
        return false;
      }
      size_t offset = pEngine->PopInt() ? 2 : 1;
      m_Operators[i - offset]->GetProc()->Execute(pEngine);
    } else {
      pEngine->DoOperator(op);
    }
  }
  return true;
}

void CPDF_PSProc::AddOperatorForTesting(ByteStringView word) {
  AddOperator(word);
}

void CPDF_PSProc::AddOperator(ByteStringView word) {
  const auto* pFound =
      std::lower_bound(std::begin(kPsOpNames), std::end(kPsOpNames), word,
                       [](const PDF_PSOpName& name, ByteStringView word) {
                         return name.name < word;
                       });
  if (pFound != std::end(kPsOpNames) && pFound->name == word)
    m_Operators.push_back(pdfium::MakeUnique<CPDF_PSOP>(pFound->op));
  else
    m_Operators.push_back(pdfium::MakeUnique<CPDF_PSOP>(StringToFloat(word)));
}

CPDF_PSEngine::CPDF_PSEngine() = default;

CPDF_PSEngine::~CPDF_PSEngine() = default;

void CPDF_PSEngine::Push(float v) {
  if (m_StackCount < kPSEngineStackSize)
    m_Stack[m_StackCount++] = v;
}

float CPDF_PSEngine::Pop() {
  return m_StackCount > 0 ? m_Stack[--m_StackCount] : 0;
}

int CPDF_PSEngine::PopInt() {
  return static_cast<int>(Pop());
}

bool CPDF_PSEngine::Parse(pdfium::span<const uint8_t> input) {
  CPDF_SimpleParser parser(input);
  return parser.GetWord() == "{" && m_MainProc.Parse(&parser, 0);
}

bool CPDF_PSEngine::DoOperator(PDF_PSOP op) {
  int i1;
  int i2;
  float d1;
  float d2;
  FX_SAFE_INT32 result;
  switch (op) {
    case PSOP_ADD:
      d1 = Pop();
      d2 = Pop();
      Push(d1 + d2);
      break;
    case PSOP_SUB:
      d2 = Pop();
      d1 = Pop();
      Push(d1 - d2);
      break;
    case PSOP_MUL:
      d1 = Pop();
      d2 = Pop();
      Push(d1 * d2);
      break;
    case PSOP_DIV:
      d2 = Pop();
      d1 = Pop();
      Push(d1 / d2);
      break;
    case PSOP_IDIV:
      i2 = PopInt();
      i1 = PopInt();
      if (i2) {
        result = i1;
        result /= i2;
        Push(result.ValueOrDefault(0));
      } else {
        Push(0);
      }
      break;
    case PSOP_MOD:
      i2 = PopInt();
      i1 = PopInt();
      if (i2) {
        result = i1;
        result %= i2;
        Push(result.ValueOrDefault(0));
      } else {
        Push(0);
      }
      break;
    case PSOP_NEG:
      d1 = Pop();
      Push(-d1);
      break;
    case PSOP_ABS:
      d1 = Pop();
      Push(fabs(d1));
      break;
    case PSOP_CEILING:
      d1 = Pop();
      Push(ceil(d1));
      break;
    case PSOP_FLOOR:
      d1 = Pop();
      Push(floor(d1));
      break;
    case PSOP_ROUND:
      d1 = Pop();
      Push(FXSYS_round(d1));
      break;
    case PSOP_TRUNCATE:
      i1 = PopInt();
      Push(i1);
      break;
    case PSOP_SQRT:
      d1 = Pop();
      Push(sqrt(d1));
      break;
    case PSOP_SIN:
      d1 = Pop();
      Push(sin(d1 * FX_PI / 180.0f));
      break;
    case PSOP_COS:
      d1 = Pop();
      Push(cos(d1 * FX_PI / 180.0f));
      break;
    case PSOP_ATAN:
      d2 = Pop();
      d1 = Pop();
      d1 = atan2(d1, d2) * 180.0 / FX_PI;
      if (d1 < 0) {
        d1 += 360;
      }
      Push(d1);
      break;
    case PSOP_EXP:
      d2 = Pop();
      d1 = Pop();
      Push(FXSYS_pow(d1, d2));
      break;
    case PSOP_LN:
      d1 = Pop();
      Push(log(d1));
      break;
    case PSOP_LOG:
      d1 = Pop();
      Push(log10(d1));
      break;
    case PSOP_CVI:
      i1 = PopInt();
      Push(i1);
      break;
    case PSOP_CVR:
      break;
    case PSOP_EQ:
      d2 = Pop();
      d1 = Pop();
      Push(d1 == d2);
      break;
    case PSOP_NE:
      d2 = Pop();
      d1 = Pop();
      Push(d1 != d2);
      break;
    case PSOP_GT:
      d2 = Pop();
      d1 = Pop();
      Push(d1 > d2);
      break;
    case PSOP_GE:
      d2 = Pop();
      d1 = Pop();
      Push(d1 >= d2);
      break;
    case PSOP_LT:
      d2 = Pop();
      d1 = Pop();
      Push(d1 < d2);
      break;
    case PSOP_LE:
      d2 = Pop();
      d1 = Pop();
      Push(d1 <= d2);
      break;
    case PSOP_AND:
      i1 = PopInt();
      i2 = PopInt();
      Push(i1 & i2);
      break;
    case PSOP_OR:
      i1 = PopInt();
      i2 = PopInt();
      Push(i1 | i2);
      break;
    case PSOP_XOR:
      i1 = PopInt();
      i2 = PopInt();
      Push(i1 ^ i2);
      break;
    case PSOP_NOT:
      i1 = PopInt();
      Push(!i1);
      break;
    case PSOP_BITSHIFT: {
      int shift = PopInt();
      result = PopInt();
      if (shift > 0) {
        result <<= shift;
      } else {
        // Avoids unsafe negation of INT_MIN.
        FX_SAFE_INT32 safe_shift = shift;
        result >>= (-safe_shift).ValueOrDefault(0);
      }
      Push(result.ValueOrDefault(0));
      break;
    }
    case PSOP_TRUE:
      Push(1);
      break;
    case PSOP_FALSE:
      Push(0);
      break;
    case PSOP_POP:
      Pop();
      break;
    case PSOP_EXCH:
      d2 = Pop();
      d1 = Pop();
      Push(d2);
      Push(d1);
      break;
    case PSOP_DUP:
      d1 = Pop();
      Push(d1);
      Push(d1);
      break;
    case PSOP_COPY: {
      int n = PopInt();
      if (n < 0 || m_StackCount + n > kPSEngineStackSize ||
          n > static_cast<int>(m_StackCount))
        break;
      for (int i = 0; i < n; i++)
        m_Stack[m_StackCount + i] = m_Stack[m_StackCount + i - n];
      m_StackCount += n;
      break;
    }
    case PSOP_INDEX: {
      int n = PopInt();
      if (n < 0 || n >= static_cast<int>(m_StackCount))
        break;
      Push(m_Stack[m_StackCount - n - 1]);
      break;
    }
    case PSOP_ROLL: {
      int j = PopInt();
      int n = PopInt();
      if (j == 0 || n == 0 || m_StackCount == 0)
        break;
      if (n < 0 || n > static_cast<int>(m_StackCount))
        break;

      j %= n;
      if (j > 0)
        j -= n;
      auto* begin_it = std::begin(m_Stack) + m_StackCount - n;
      auto* middle_it = begin_it - j;
      auto* end_it = std::begin(m_Stack) + m_StackCount;
      std::rotate(begin_it, middle_it, end_it);
      break;
    }
    default:
      break;
  }
  return true;
}
