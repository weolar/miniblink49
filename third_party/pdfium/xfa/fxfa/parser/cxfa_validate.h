// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_VALIDATE_H_
#define XFA_FXFA_PARSER_CXFA_VALIDATE_H_

#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_Script;

class CXFA_Validate final : public CXFA_Node {
 public:
  CXFA_Validate(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Validate() override;

  XFA_AttributeValue GetFormatTest();
  WideString GetFormatMessageText();
  void SetFormatMessageText(const WideString& wsMessage);

  XFA_AttributeValue GetNullTest();
  void SetNullTest(const WideString& wsValue);

  WideString GetNullMessageText();
  void SetNullMessageText(const WideString& wsMessage);

  XFA_AttributeValue GetScriptTest();
  WideString GetScriptMessageText();
  void SetScriptMessageText(const WideString& wsMessage);

  WideString GetPicture();
  CXFA_Script* GetScriptIfExists();

 private:
  WideString GetMessageText(const WideString& wsMessageType);
  void SetMessageText(const WideString& wsMessageType,
                      const WideString& wsMessage);
};

#endif  // XFA_FXFA_PARSER_CXFA_VALIDATE_H_
