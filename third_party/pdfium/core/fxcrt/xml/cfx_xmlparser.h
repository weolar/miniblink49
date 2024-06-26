// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_XML_CFX_XMLPARSER_H_
#define CORE_FXCRT_XML_CFX_XMLPARSER_H_

#include <memory>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/retain_ptr.h"

class CFX_XMLDocument;
class CFX_XMLElement;
class CFX_XMLNode;
class IFX_SeekableReadStream;

class CFX_XMLParser {
 public:
  static bool IsXMLNameChar(wchar_t ch, bool bFirstChar);

  explicit CFX_XMLParser(const RetainPtr<IFX_SeekableReadStream>& pStream);
  virtual ~CFX_XMLParser();

  std::unique_ptr<CFX_XMLDocument> Parse();

 private:
  enum class FDE_XmlSyntaxState {
    Text,
    Node,
    Target,
    Tag,
    AttriName,
    AttriEqualSign,
    AttriQuotation,
    AttriValue,
    CloseInstruction,
    BreakElement,
    CloseElement,
    SkipDeclNode,
    SkipComment,
    SkipCommentOrDecl,
    SkipCData,
    TargetData
  };

  bool DoSyntaxParse(CFX_XMLDocument* doc);
  WideString GetTextData();
  void ProcessTextChar(wchar_t ch);
  void ProcessTargetData();

  CFX_XMLNode* current_node_ = nullptr;
  RetainPtr<IFX_SeekableReadStream> stream_;
  std::vector<wchar_t> current_text_;
  size_t xml_plane_size_ = 1024;
  int32_t entity_start_ = -1;
};

#endif  // CORE_FXCRT_XML_CFX_XMLPARSER_H_
