// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_TEXTPROVIDER_H_
#define XFA_FXFA_CXFA_TEXTPROVIDER_H_

#include "core/fxcrt/fx_string.h"
#include "third_party/base/optional.h"
#include "xfa/fxfa/cxfa_textlayout.h"

class CXFA_Font;
class CXFA_Node;
class CXFA_Para;

enum XFA_TEXTPROVIDERTYPE {
  XFA_TEXTPROVIDERTYPE_Text,
  XFA_TEXTPROVIDERTYPE_Datasets,
  XFA_TEXTPROVIDERTYPE_Caption,
  XFA_TEXTPROVIDERTYPE_Rollover,
  XFA_TEXTPROVIDERTYPE_Down,
};

class CXFA_TextProvider {
 public:
  CXFA_TextProvider(CXFA_Node* pNode, XFA_TEXTPROVIDERTYPE eType)
      : m_pNode(pNode), m_eType(eType) {
    ASSERT(m_pNode);
  }
  ~CXFA_TextProvider() {}

  CXFA_Node* GetTextNode(bool* bRichText);
  CXFA_Para* GetParaIfExists();
  CXFA_Font* GetFontIfExists();
  bool IsCheckButtonAndAutoWidth() const;
  Optional<WideString> GetEmbeddedObj(const WideString& wsAttr) const;

 private:
  CXFA_Node* m_pNode;  // Raw, this class owned by tree node.
  XFA_TEXTPROVIDERTYPE m_eType;
};

#endif  // XFA_FXFA_CXFA_TEXTPROVIDER_H_
