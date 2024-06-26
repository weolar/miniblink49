// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_PSENGINE_H_
#define CORE_FPDFAPI_PAGE_CPDF_PSENGINE_H_

#include <memory>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "third_party/base/span.h"

class CPDF_PSEngine;
class CPDF_PSProc;
class CPDF_SimpleParser;

enum PDF_PSOP : uint8_t {
  PSOP_ADD,
  PSOP_SUB,
  PSOP_MUL,
  PSOP_DIV,
  PSOP_IDIV,
  PSOP_MOD,
  PSOP_NEG,
  PSOP_ABS,
  PSOP_CEILING,
  PSOP_FLOOR,
  PSOP_ROUND,
  PSOP_TRUNCATE,
  PSOP_SQRT,
  PSOP_SIN,
  PSOP_COS,
  PSOP_ATAN,
  PSOP_EXP,
  PSOP_LN,
  PSOP_LOG,
  PSOP_CVI,
  PSOP_CVR,
  PSOP_EQ,
  PSOP_NE,
  PSOP_GT,
  PSOP_GE,
  PSOP_LT,
  PSOP_LE,
  PSOP_AND,
  PSOP_OR,
  PSOP_XOR,
  PSOP_NOT,
  PSOP_BITSHIFT,
  PSOP_TRUE,
  PSOP_FALSE,
  PSOP_IF,
  PSOP_IFELSE,
  PSOP_POP,
  PSOP_EXCH,
  PSOP_DUP,
  PSOP_COPY,
  PSOP_INDEX,
  PSOP_ROLL,
  PSOP_PROC,
  PSOP_CONST
};

class CPDF_PSOP {
 public:
  CPDF_PSOP();
  explicit CPDF_PSOP(PDF_PSOP op);
  explicit CPDF_PSOP(float value);
  ~CPDF_PSOP();

  float GetFloatValue() const;
  CPDF_PSProc* GetProc() const;
  PDF_PSOP GetOp() const { return m_op; }

 private:
  const PDF_PSOP m_op;
  const float m_value;
  std::unique_ptr<CPDF_PSProc> m_proc;
};

class CPDF_PSProc {
 public:
  CPDF_PSProc();
  ~CPDF_PSProc();

  bool Parse(CPDF_SimpleParser* parser, int depth);
  bool Execute(CPDF_PSEngine* pEngine);

  // These methods are exposed for testing.
  void AddOperatorForTesting(ByteStringView word);
  size_t num_operators() const { return m_Operators.size(); }
  const std::unique_ptr<CPDF_PSOP>& last_operator() {
    return m_Operators.back();
  }

 private:
  static const int kMaxDepth = 128;

  void AddOperator(ByteStringView word);

  std::vector<std::unique_ptr<CPDF_PSOP>> m_Operators;
};

class CPDF_PSEngine {
 public:
  CPDF_PSEngine();
  ~CPDF_PSEngine();

  bool Parse(pdfium::span<const uint8_t> input);
  bool Execute();
  bool DoOperator(PDF_PSOP op);
  void Reset() { m_StackCount = 0; }
  void Push(float value);
  float Pop();
  int PopInt();
  uint32_t GetStackSize() const { return m_StackCount; }

 private:
  static constexpr uint32_t kPSEngineStackSize = 100;

  uint32_t m_StackCount = 0;
  CPDF_PSProc m_MainProc;
  float m_Stack[kPSEngineStackSize];
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_PSENGINE_H_
