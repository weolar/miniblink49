// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_XML_CFX_XMLINSTRUCTION_H_
#define CORE_FXCRT_XML_CFX_XMLINSTRUCTION_H_

#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/xml/cfx_xmlnode.h"

class CFX_XMLDocument;

class CFX_XMLInstruction final : public CFX_XMLNode {
 public:
  explicit CFX_XMLInstruction(const WideString& wsTarget);
  ~CFX_XMLInstruction() override;

  // CFX_XMLNode
  FX_XMLNODETYPE GetType() const override;
  CFX_XMLNode* Clone(CFX_XMLDocument* doc) override;
  void Save(const RetainPtr<IFX_SeekableWriteStream>& pXMLStream) override;

  bool IsOriginalXFAVersion() const;
  bool IsAcrobat() const;

  const std::vector<WideString>& GetTargetData() const { return target_data_; }
  void AppendData(const WideString& wsData);

 private:
  const WideString name_;
  std::vector<WideString> target_data_;
};

inline CFX_XMLInstruction* ToXMLInstruction(CFX_XMLNode* pNode) {
  return pNode && pNode->GetType() == FX_XMLNODE_Instruction
             ? static_cast<CFX_XMLInstruction*>(pNode)
             : nullptr;
}

#endif  // CORE_FXCRT_XML_CFX_XMLINSTRUCTION_H_
