// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_FM2JS_CXFA_FMEXPRESSION_H_
#define XFA_FXFA_FM2JS_CXFA_FMEXPRESSION_H_

#include <memory>
#include <vector>

#include "xfa/fxfa/fm2js/cxfa_fmsimpleexpression.h"

class CFX_WideTextBuf;

class CXFA_FMExpression {
 public:
  virtual ~CXFA_FMExpression() = default;
  virtual bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) = 0;

 protected:
  CXFA_FMExpression();
};

class CXFA_FMFunctionDefinition final : public CXFA_FMExpression {
 public:
  CXFA_FMFunctionDefinition(
      WideStringView wsName,
      std::vector<WideStringView>&& arguments,
      std::vector<std::unique_ptr<CXFA_FMExpression>>&& expressions);
  ~CXFA_FMFunctionDefinition() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  WideStringView m_wsName;
  std::vector<WideStringView> m_pArguments;
  std::vector<std::unique_ptr<CXFA_FMExpression>> m_pExpressions;
};

class CXFA_FMAST {
 public:
  explicit CXFA_FMAST(
      std::vector<std::unique_ptr<CXFA_FMExpression>> expressions);
  ~CXFA_FMAST();

  bool ToJavaScript(CFX_WideTextBuf* js);

 private:
  std::vector<std::unique_ptr<CXFA_FMExpression>> expressions_;
};

class CXFA_FMVarExpression final : public CXFA_FMExpression {
 public:
  CXFA_FMVarExpression(WideStringView wsName,
                       std::unique_ptr<CXFA_FMSimpleExpression> pInit);
  ~CXFA_FMVarExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  WideStringView m_wsName;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pInit;
};

class CXFA_FMExpExpression final : public CXFA_FMExpression {
 public:
  explicit CXFA_FMExpExpression(
      std::unique_ptr<CXFA_FMSimpleExpression> pExpression);
  ~CXFA_FMExpExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExpression;
};

class CXFA_FMBlockExpression final : public CXFA_FMExpression {
 public:
  CXFA_FMBlockExpression(
      std::vector<std::unique_ptr<CXFA_FMExpression>>&& pExpressionList);
  ~CXFA_FMBlockExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  std::vector<std::unique_ptr<CXFA_FMExpression>> m_ExpressionList;
};

class CXFA_FMDoExpression final : public CXFA_FMExpression {
 public:
  explicit CXFA_FMDoExpression(std::unique_ptr<CXFA_FMExpression> pList);
  ~CXFA_FMDoExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  std::unique_ptr<CXFA_FMExpression> m_pList;
};

class CXFA_FMIfExpression final : public CXFA_FMExpression {
 public:
  CXFA_FMIfExpression(
      std::unique_ptr<CXFA_FMSimpleExpression> pExpression,
      std::unique_ptr<CXFA_FMExpression> pIfExpression,
      std::vector<std::unique_ptr<CXFA_FMIfExpression>> pElseIfExpressions,
      std::unique_ptr<CXFA_FMExpression> pElseExpression);
  ~CXFA_FMIfExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExpression;
  std::unique_ptr<CXFA_FMExpression> m_pIfExpression;
  std::vector<std::unique_ptr<CXFA_FMIfExpression>> m_pElseIfExpressions;
  std::unique_ptr<CXFA_FMExpression> m_pElseExpression;
};

class CXFA_FMWhileExpression final : public CXFA_FMExpression {
 public:
  CXFA_FMWhileExpression(std::unique_ptr<CXFA_FMSimpleExpression> pCodition,
                         std::unique_ptr<CXFA_FMExpression> pExpression);
  ~CXFA_FMWhileExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  std::unique_ptr<CXFA_FMSimpleExpression> m_pCondition;
  std::unique_ptr<CXFA_FMExpression> m_pExpression;
};

class CXFA_FMBreakExpression final : public CXFA_FMExpression {
 public:
  CXFA_FMBreakExpression();
  ~CXFA_FMBreakExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;
};

class CXFA_FMContinueExpression final : public CXFA_FMExpression {
 public:
  CXFA_FMContinueExpression();
  ~CXFA_FMContinueExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;
};

class CXFA_FMForExpression final : public CXFA_FMExpression {
 public:
  CXFA_FMForExpression(WideStringView wsVariant,
                       std::unique_ptr<CXFA_FMSimpleExpression> pAssignment,
                       std::unique_ptr<CXFA_FMSimpleExpression> pAccessor,
                       int32_t iDirection,
                       std::unique_ptr<CXFA_FMSimpleExpression> pStep,
                       std::unique_ptr<CXFA_FMExpression> pList);
  ~CXFA_FMForExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  WideStringView m_wsVariant;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pAssignment;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pAccessor;
  const bool m_bDirection;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pStep;
  std::unique_ptr<CXFA_FMExpression> m_pList;
};

class CXFA_FMForeachExpression final : public CXFA_FMExpression {
 public:
  // Takes ownership of |pAccessors|.
  CXFA_FMForeachExpression(
      WideStringView wsIdentifier,
      std::vector<std::unique_ptr<CXFA_FMSimpleExpression>>&& pAccessors,
      std::unique_ptr<CXFA_FMExpression> pList);
  ~CXFA_FMForeachExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  WideStringView m_wsIdentifier;
  std::vector<std::unique_ptr<CXFA_FMSimpleExpression>> m_pAccessors;
  std::unique_ptr<CXFA_FMExpression> m_pList;
};

#endif  // XFA_FXFA_FM2JS_CXFA_FMEXPRESSION_H_
