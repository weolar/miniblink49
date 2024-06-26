// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/fm2js/cxfa_fmexpression.h"

#include <utility>

#include "core/fxcrt/cfx_widetextbuf.h"
#include "xfa/fxfa/fm2js/cxfa_fmsimpleexpression.h"
#include "xfa/fxfa/fm2js/cxfa_fmtojavascriptdepth.h"

namespace {

const wchar_t kLessEqual[] = L" <= ";
const wchar_t kGreaterEqual[] = L" >= ";
const wchar_t kPlusEqual[] = L" += ";
const wchar_t kMinusEqual[] = L" -= ";

WideString IdentifierToName(WideStringView ident) {
  if (ident.IsEmpty())
    return WideString();
  if (ident[0] != L'!')
    return WideString(ident);
  return L"pfm__excl__" + ident.Right(ident.GetLength() - 1);
}

}  // namespace

CXFA_FMExpression::CXFA_FMExpression() = default;

CXFA_FMFunctionDefinition::CXFA_FMFunctionDefinition(
    WideStringView wsName,
    std::vector<WideStringView>&& arguments,
    std::vector<std::unique_ptr<CXFA_FMExpression>>&& expressions)
    : CXFA_FMExpression(),
      m_wsName(wsName),
      m_pArguments(std::move(arguments)),
      m_pExpressions(std::move(expressions)) {
  ASSERT(!wsName.IsEmpty());
}

CXFA_FMFunctionDefinition::~CXFA_FMFunctionDefinition() = default;

bool CXFA_FMFunctionDefinition::ToJavaScript(CFX_WideTextBuf* js,
                                             ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  if (m_wsName.IsEmpty())
    return false;

  *js << "function " << IdentifierToName(m_wsName) << "(";
  for (const auto& identifier : m_pArguments) {
    if (identifier != m_pArguments.front())
      *js << ", ";

    *js << IdentifierToName(identifier);
  }
  *js << ") {\n";

  *js << "var pfm_ret = null;\n";
  for (const auto& expr : m_pExpressions) {
    ReturnType ret_type = expr == m_pExpressions.back() ? ReturnType::kImplied
                                                        : ReturnType::kInfered;
    if (!expr->ToJavaScript(js, ret_type))
      return false;
  }

  *js << "return pfm_ret;\n";
  *js << "}\n";

  return !CXFA_IsTooBig(js);
}

CXFA_FMAST::CXFA_FMAST(
    std::vector<std::unique_ptr<CXFA_FMExpression>> expressions)
    : expressions_(std::move(expressions)) {}

CXFA_FMAST::~CXFA_FMAST() = default;

bool CXFA_FMAST::ToJavaScript(CFX_WideTextBuf* js) {
  if (expressions_.empty()) {
    *js << "// comments only";
    return !CXFA_IsTooBig(js);
  }

  *js << "(function() {\n";
  *js << "let pfm_method_runner = function(obj, cb) {\n";
  *js << "  if (pfm_rt.is_ary(obj)) {\n";
  *js << "    let pfm_method_return = null;\n";
  *js << "    for (var idx = obj.length -1; idx > 1; idx--) {\n";
  *js << "      pfm_method_return = cb(obj[idx]);\n";
  *js << "    }\n";
  *js << "    return pfm_method_return;\n";
  *js << "  }\n";
  *js << "  return cb(obj);\n";
  *js << "};\n";
  *js << "var pfm_ret = null;\n";

  for (const auto& expr : expressions_) {
    ReturnType ret_type = expr == expressions_.back() ? ReturnType::kImplied
                                                      : ReturnType::kInfered;
    if (!expr->ToJavaScript(js, ret_type))
      return false;
  }

  *js << "return pfm_rt.get_val(pfm_ret);\n";
  *js << "}).call(this);";
  return !CXFA_IsTooBig(js);
}

CXFA_FMVarExpression::CXFA_FMVarExpression(
    WideStringView wsName,
    std::unique_ptr<CXFA_FMSimpleExpression> pInit)
    : CXFA_FMExpression(), m_wsName(wsName), m_pInit(std::move(pInit)) {}

CXFA_FMVarExpression::~CXFA_FMVarExpression() = default;

bool CXFA_FMVarExpression::ToJavaScript(CFX_WideTextBuf* js, ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  WideString tempName = IdentifierToName(m_wsName);
  *js << "var " << tempName << " = ";
  if (m_pInit) {
    if (!m_pInit->ToJavaScript(js, ReturnType::kInfered))
      return false;

    *js << ";\n";
    *js << tempName << " = pfm_rt.var_filter(" << tempName << ");\n";
  } else {
    *js << "\"\";\n";
  }

  if (type == ReturnType::kImplied)
    *js << "pfm_ret = " << tempName << ";\n";

  return !CXFA_IsTooBig(js);
}

CXFA_FMExpExpression::CXFA_FMExpExpression(
    std::unique_ptr<CXFA_FMSimpleExpression> pExpression)
    : CXFA_FMExpression(), m_pExpression(std::move(pExpression)) {}

CXFA_FMExpExpression::~CXFA_FMExpExpression() = default;

bool CXFA_FMExpExpression::ToJavaScript(CFX_WideTextBuf* js, ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  if (type == ReturnType::kInfered) {
    bool ret = m_pExpression->ToJavaScript(js, ReturnType::kInfered);
    if (m_pExpression->GetOperatorToken() != TOKassign)
      *js << ";\n";

    return ret;
  }

  if (m_pExpression->GetOperatorToken() == TOKassign)
    return m_pExpression->ToJavaScript(js, ReturnType::kImplied);

  if (m_pExpression->GetOperatorToken() == TOKstar ||
      m_pExpression->GetOperatorToken() == TOKdotstar ||
      m_pExpression->GetOperatorToken() == TOKdotscream ||
      m_pExpression->GetOperatorToken() == TOKdotdot ||
      m_pExpression->GetOperatorToken() == TOKdot) {
    *js << "pfm_ret = pfm_rt.get_val(";
    if (!m_pExpression->ToJavaScript(js, ReturnType::kInfered))
      return false;

    *js << ");\n";
    return !CXFA_IsTooBig(js);
  }

  *js << "pfm_ret = ";
  if (!m_pExpression->ToJavaScript(js, ReturnType::kInfered))
    return false;

  *js << ";\n";
  return !CXFA_IsTooBig(js);
}

CXFA_FMBlockExpression::CXFA_FMBlockExpression(
    std::vector<std::unique_ptr<CXFA_FMExpression>>&& pExpressionList)
    : CXFA_FMExpression(), m_ExpressionList(std::move(pExpressionList)) {}

CXFA_FMBlockExpression::~CXFA_FMBlockExpression() = default;

bool CXFA_FMBlockExpression::ToJavaScript(CFX_WideTextBuf* js,
                                          ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  *js << "{\n";
  for (const auto& expr : m_ExpressionList) {
    if (type == ReturnType::kInfered) {
      if (!expr->ToJavaScript(js, ReturnType::kInfered))
        return false;
    } else {
      ReturnType ret_type = expr == m_ExpressionList.back()
                                ? ReturnType::kImplied
                                : ReturnType::kInfered;
      if (!expr->ToJavaScript(js, ret_type))
        return false;
    }
  }
  *js << "}\n";

  return !CXFA_IsTooBig(js);
}

CXFA_FMDoExpression::CXFA_FMDoExpression(
    std::unique_ptr<CXFA_FMExpression> pList)
    : CXFA_FMExpression(), m_pList(std::move(pList)) {}

CXFA_FMDoExpression::~CXFA_FMDoExpression() = default;

bool CXFA_FMDoExpression::ToJavaScript(CFX_WideTextBuf* js, ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  return m_pList->ToJavaScript(js, type);
}

CXFA_FMIfExpression::CXFA_FMIfExpression(
    std::unique_ptr<CXFA_FMSimpleExpression> pExpression,
    std::unique_ptr<CXFA_FMExpression> pIfExpression,
    std::vector<std::unique_ptr<CXFA_FMIfExpression>> pElseIfExpressions,
    std::unique_ptr<CXFA_FMExpression> pElseExpression)
    : CXFA_FMExpression(),
      m_pExpression(std::move(pExpression)),
      m_pIfExpression(std::move(pIfExpression)),
      m_pElseIfExpressions(std::move(pElseIfExpressions)),
      m_pElseExpression(std::move(pElseExpression)) {
  ASSERT(m_pExpression);
}

CXFA_FMIfExpression::~CXFA_FMIfExpression() = default;

bool CXFA_FMIfExpression::ToJavaScript(CFX_WideTextBuf* js, ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  if (type == ReturnType::kImplied)
    *js << "pfm_ret = 0;\n";

  *js << "if (pfm_rt.get_val(";
  if (!m_pExpression->ToJavaScript(js, ReturnType::kInfered))
    return false;
  *js << "))\n";

  if (CXFA_IsTooBig(js))
    return false;

  if (m_pIfExpression) {
    if (!m_pIfExpression->ToJavaScript(js, type))
      return false;
    if (CXFA_IsTooBig(js))
      return false;
  }

  for (auto& expr : m_pElseIfExpressions) {
    *js << "else ";
    if (!expr->ToJavaScript(js, ReturnType::kInfered))
      return false;
  }

  if (m_pElseExpression) {
    *js << "else ";
    if (!m_pElseExpression->ToJavaScript(js, type))
      return false;
  }
  return !CXFA_IsTooBig(js);
}

CXFA_FMWhileExpression::CXFA_FMWhileExpression(
    std::unique_ptr<CXFA_FMSimpleExpression> pCondition,
    std::unique_ptr<CXFA_FMExpression> pExpression)
    : CXFA_FMExpression(),
      m_pCondition(std::move(pCondition)),
      m_pExpression(std::move(pExpression)) {}

CXFA_FMWhileExpression::~CXFA_FMWhileExpression() = default;

bool CXFA_FMWhileExpression::ToJavaScript(CFX_WideTextBuf* js,
                                          ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  if (type == ReturnType::kImplied)
    *js << "pfm_ret = 0;\n";

  *js << "while (";
  if (!m_pCondition->ToJavaScript(js, ReturnType::kInfered))
    return false;

  *js << ")\n";
  if (CXFA_IsTooBig(js))
    return false;

  if (!m_pExpression->ToJavaScript(js, type))
    return false;

  return !CXFA_IsTooBig(js);
}

CXFA_FMBreakExpression::CXFA_FMBreakExpression() : CXFA_FMExpression() {}

CXFA_FMBreakExpression::~CXFA_FMBreakExpression() = default;

bool CXFA_FMBreakExpression::ToJavaScript(CFX_WideTextBuf* js,
                                          ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  *js << "pfm_ret = 0;\nbreak;\n";
  return !CXFA_IsTooBig(js);
}

CXFA_FMContinueExpression::CXFA_FMContinueExpression() : CXFA_FMExpression() {}

CXFA_FMContinueExpression::~CXFA_FMContinueExpression() = default;

bool CXFA_FMContinueExpression::ToJavaScript(CFX_WideTextBuf* js,
                                             ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  *js << "pfm_ret = 0;\ncontinue;\n";
  return !CXFA_IsTooBig(js);
}

CXFA_FMForExpression::CXFA_FMForExpression(
    WideStringView wsVariant,
    std::unique_ptr<CXFA_FMSimpleExpression> pAssignment,
    std::unique_ptr<CXFA_FMSimpleExpression> pAccessor,
    int32_t iDirection,
    std::unique_ptr<CXFA_FMSimpleExpression> pStep,
    std::unique_ptr<CXFA_FMExpression> pList)
    : CXFA_FMExpression(),
      m_wsVariant(wsVariant),
      m_pAssignment(std::move(pAssignment)),
      m_pAccessor(std::move(pAccessor)),
      m_bDirection(iDirection == 1),
      m_pStep(std::move(pStep)),
      m_pList(std::move(pList)) {}

CXFA_FMForExpression::~CXFA_FMForExpression() = default;

bool CXFA_FMForExpression::ToJavaScript(CFX_WideTextBuf* js, ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  if (type == ReturnType::kImplied)
    *js << "pfm_ret = 0;\n";

  *js << "{\n";

  WideString tmpName = IdentifierToName(m_wsVariant);
  *js << "var " << tmpName << " = null;\n";

  *js << "for (" << tmpName << " = pfm_rt.get_val(";
  if (!m_pAssignment->ToJavaScript(js, ReturnType::kInfered))
    return false;
  *js << "); ";

  *js << tmpName << (m_bDirection ? kLessEqual : kGreaterEqual);
  *js << "pfm_rt.get_val(";
  if (!m_pAccessor->ToJavaScript(js, ReturnType::kInfered))
    return false;
  *js << "); ";

  *js << tmpName << (m_bDirection ? kPlusEqual : kMinusEqual);
  if (m_pStep) {
    *js << "pfm_rt.get_val(";
    if (!m_pStep->ToJavaScript(js, ReturnType::kInfered))
      return false;
    *js << ")";
  } else {
    *js << "1";
  }
  *js << ")\n";
  if (CXFA_IsTooBig(js))
    return false;

  if (!m_pList->ToJavaScript(js, type))
    return false;

  *js << "}\n";
  return !CXFA_IsTooBig(js);
}

CXFA_FMForeachExpression::CXFA_FMForeachExpression(
    WideStringView wsIdentifier,
    std::vector<std::unique_ptr<CXFA_FMSimpleExpression>>&& pAccessors,
    std::unique_ptr<CXFA_FMExpression> pList)
    : CXFA_FMExpression(),
      m_wsIdentifier(wsIdentifier),
      m_pAccessors(std::move(pAccessors)),
      m_pList(std::move(pList)) {}

CXFA_FMForeachExpression::~CXFA_FMForeachExpression() = default;

bool CXFA_FMForeachExpression::ToJavaScript(CFX_WideTextBuf* js,
                                            ReturnType type) {
  CXFA_FMToJavaScriptDepth depthManager;
  if (CXFA_IsTooBig(js) || !depthManager.IsWithinMaxDepth())
    return false;

  if (type == ReturnType::kImplied)
    *js << "pfm_ret = 0;\n";

  *js << "{\n";

  WideString tmpName = IdentifierToName(m_wsIdentifier);
  *js << "var " << tmpName << " = null;\n";
  *js << "var pfm_ary = pfm_rt.concat_obj(";
  for (const auto& expr : m_pAccessors) {
    if (!expr->ToJavaScript(js, ReturnType::kInfered))
      return false;
    if (expr != m_pAccessors.back())
      *js << ", ";
  }
  *js << ");\n";

  *js << "var pfm_ary_idx = 0;\n";
  *js << "while(pfm_ary_idx < pfm_ary.length)\n{\n";
  *js << tmpName << " = pfm_ary[pfm_ary_idx++];\n";
  if (!m_pList->ToJavaScript(js, type))
    return false;
  *js << "}\n";  // while

  *js << "}\n";  // block
  return !CXFA_IsTooBig(js);
}
