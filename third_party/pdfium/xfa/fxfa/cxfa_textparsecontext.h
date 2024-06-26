// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_TEXTPARSECONTEXT_H_
#define XFA_FXFA_CXFA_TEXTPARSECONTEXT_H_

#include <utility>
#include <vector>

#include "core/fxcrt/css/cfx_css.h"
#include "core/fxcrt/css/cfx_cssdeclaration.h"

class CFX_CSSComputedStyle;

class CXFA_TextParseContext {
 public:
  CXFA_TextParseContext();
  ~CXFA_TextParseContext();

  void SetDisplay(CFX_CSSDisplay eDisplay) { m_eDisplay = eDisplay; }
  CFX_CSSDisplay GetDisplay() const { return m_eDisplay; }

  void SetDecls(std::vector<const CFX_CSSDeclaration*>&& decl) {
    decls_ = std::move(decl);
  }
  const std::vector<const CFX_CSSDeclaration*>& GetDecls() { return decls_; }

  RetainPtr<CFX_CSSComputedStyle> m_pParentStyle;

 private:
  std::vector<const CFX_CSSDeclaration*> decls_;
  CFX_CSSDisplay m_eDisplay;
};

#endif  // XFA_FXFA_CXFA_TEXTPARSECONTEXT_H_
