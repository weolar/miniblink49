// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_FM2JS_CXFA_FMSIMPLEEXPRESSION_H_
#define XFA_FXFA_FM2JS_CXFA_FMSIMPLEEXPRESSION_H_

#include <memory>
#include <vector>

#include "xfa/fxfa/fm2js/cxfa_fmlexer.h"

enum XFA_FM_AccessorIndex {
  ACCESSOR_NO_INDEX,
  ACCESSOR_NO_RELATIVEINDEX,
  ACCESSOR_POSITIVE_INDEX,
  ACCESSOR_NEGATIVE_INDEX
};

enum class ReturnType { kImplied, kInfered };

class CFX_WideTextBuf;

class CXFA_FMSimpleExpression {
 public:
  virtual ~CXFA_FMSimpleExpression() = default;
  virtual bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) = 0;

  XFA_FM_TOKEN GetOperatorToken() const;

 protected:
  explicit CXFA_FMSimpleExpression(XFA_FM_TOKEN op);

  const XFA_FM_TOKEN m_op;
};

class CXFA_FMNullExpression final : public CXFA_FMSimpleExpression {
 public:
  CXFA_FMNullExpression();
  ~CXFA_FMNullExpression() override {}

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;
};

class CXFA_FMNumberExpression final : public CXFA_FMSimpleExpression {
 public:
  explicit CXFA_FMNumberExpression(WideStringView wsNumber);
  ~CXFA_FMNumberExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  WideStringView m_wsNumber;
};

class CXFA_FMStringExpression final : public CXFA_FMSimpleExpression {
 public:
  explicit CXFA_FMStringExpression(WideStringView wsString);
  ~CXFA_FMStringExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  WideStringView m_wsString;
};

class CXFA_FMIdentifierExpression final : public CXFA_FMSimpleExpression {
 public:
  explicit CXFA_FMIdentifierExpression(WideStringView wsIdentifier);
  ~CXFA_FMIdentifierExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  WideStringView m_wsIdentifier;
};

class CXFA_FMAssignExpression final : public CXFA_FMSimpleExpression {
 public:
  CXFA_FMAssignExpression(XFA_FM_TOKEN op,
                          std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                          std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMAssignExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp1;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp2;
};

class CXFA_FMBinExpression : public CXFA_FMSimpleExpression {
 public:
  ~CXFA_FMBinExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 protected:
  CXFA_FMBinExpression(const WideString& opName,
                       XFA_FM_TOKEN op,
                       std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                       std::unique_ptr<CXFA_FMSimpleExpression> pExp2);

 private:
  WideString m_OpName;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp1;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp2;
};

class CXFA_FMLogicalOrExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMLogicalOrExpression(XFA_FM_TOKEN op,
                             std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                             std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMLogicalOrExpression() override {}
};

class CXFA_FMLogicalAndExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMLogicalAndExpression(XFA_FM_TOKEN op,
                              std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                              std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMLogicalAndExpression() override {}
};

class CXFA_FMEqualExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMEqualExpression(XFA_FM_TOKEN op,
                         std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                         std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMEqualExpression() override {}
};

class CXFA_FMNotEqualExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMNotEqualExpression(XFA_FM_TOKEN op,
                            std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                            std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMNotEqualExpression() override {}
};

class CXFA_FMGtExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMGtExpression(XFA_FM_TOKEN op,
                      std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                      std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMGtExpression() override {}
};

class CXFA_FMGeExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMGeExpression(XFA_FM_TOKEN op,
                      std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                      std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMGeExpression() override {}
};

class CXFA_FMLtExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMLtExpression(XFA_FM_TOKEN op,
                      std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                      std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMLtExpression() override {}
};

class CXFA_FMLeExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMLeExpression(XFA_FM_TOKEN op,
                      std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                      std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMLeExpression() override {}
};

class CXFA_FMPlusExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMPlusExpression(XFA_FM_TOKEN op,
                        std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                        std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMPlusExpression() override {}
};

class CXFA_FMMinusExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMMinusExpression(XFA_FM_TOKEN op,
                         std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                         std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMMinusExpression() override {}
};

class CXFA_FMMulExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMMulExpression(XFA_FM_TOKEN op,
                       std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                       std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMMulExpression() override {}
};

class CXFA_FMDivExpression final : public CXFA_FMBinExpression {
 public:
  CXFA_FMDivExpression(XFA_FM_TOKEN op,
                       std::unique_ptr<CXFA_FMSimpleExpression> pExp1,
                       std::unique_ptr<CXFA_FMSimpleExpression> pExp2);
  ~CXFA_FMDivExpression() override {}
};

class CXFA_FMUnaryExpression : public CXFA_FMSimpleExpression {
 public:
  ~CXFA_FMUnaryExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 protected:
  CXFA_FMUnaryExpression(const WideString& opName,
                         XFA_FM_TOKEN op,
                         std::unique_ptr<CXFA_FMSimpleExpression> pExp);

 private:
  WideString m_OpName;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp;
};

class CXFA_FMPosExpression final : public CXFA_FMUnaryExpression {
 public:
  explicit CXFA_FMPosExpression(std::unique_ptr<CXFA_FMSimpleExpression> pExp);
  ~CXFA_FMPosExpression() override {}
};

class CXFA_FMNegExpression final : public CXFA_FMUnaryExpression {
 public:
  explicit CXFA_FMNegExpression(std::unique_ptr<CXFA_FMSimpleExpression> pExp);
  ~CXFA_FMNegExpression() override {}
};

class CXFA_FMNotExpression final : public CXFA_FMUnaryExpression {
 public:
  explicit CXFA_FMNotExpression(std::unique_ptr<CXFA_FMSimpleExpression> pExp);
  ~CXFA_FMNotExpression() override {}
};

class CXFA_FMCallExpression final : public CXFA_FMSimpleExpression {
 public:
  CXFA_FMCallExpression(
      std::unique_ptr<CXFA_FMSimpleExpression> pExp,
      std::vector<std::unique_ptr<CXFA_FMSimpleExpression>>&& pArguments,
      bool bIsSomMethod);
  ~CXFA_FMCallExpression() override;

  bool IsBuiltInFunc(CFX_WideTextBuf* funcName);
  uint32_t IsMethodWithObjParam(const WideString& methodName);
  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp;
  bool m_bIsSomMethod;
  std::vector<std::unique_ptr<CXFA_FMSimpleExpression>> m_Arguments;
};

class CXFA_FMDotAccessorExpression final : public CXFA_FMSimpleExpression {
 public:
  CXFA_FMDotAccessorExpression(
      std::unique_ptr<CXFA_FMSimpleExpression> pAccessor,
      XFA_FM_TOKEN op,
      WideStringView wsIdentifier,
      std::unique_ptr<CXFA_FMSimpleExpression> pIndexExp);
  ~CXFA_FMDotAccessorExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  WideStringView m_wsIdentifier;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp1;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp2;
};

class CXFA_FMIndexExpression final : public CXFA_FMSimpleExpression {
 public:
  CXFA_FMIndexExpression(XFA_FM_AccessorIndex accessorIndex,
                         std::unique_ptr<CXFA_FMSimpleExpression> pIndexExp,
                         bool bIsStarIndex);
  ~CXFA_FMIndexExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp;
  XFA_FM_AccessorIndex m_accessorIndex;
  bool m_bIsStarIndex;
};

class CXFA_FMDotDotAccessorExpression final : public CXFA_FMSimpleExpression {
 public:
  CXFA_FMDotDotAccessorExpression(
      std::unique_ptr<CXFA_FMSimpleExpression> pAccessor,
      XFA_FM_TOKEN op,
      WideStringView wsIdentifier,
      std::unique_ptr<CXFA_FMSimpleExpression> pIndexExp);
  ~CXFA_FMDotDotAccessorExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  WideStringView m_wsIdentifier;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp1;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp2;
};

class CXFA_FMMethodCallExpression final : public CXFA_FMSimpleExpression {
 public:
  CXFA_FMMethodCallExpression(
      std::unique_ptr<CXFA_FMSimpleExpression> pAccessorExp1,
      std::unique_ptr<CXFA_FMSimpleExpression> pCallExp);
  ~CXFA_FMMethodCallExpression() override;

  bool ToJavaScript(CFX_WideTextBuf* js, ReturnType type) override;

 private:
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp1;
  std::unique_ptr<CXFA_FMSimpleExpression> m_pExp2;
};

bool CXFA_IsTooBig(const CFX_WideTextBuf* js);

#endif  // XFA_FXFA_FM2JS_CXFA_FMSIMPLEEXPRESSION_H_
