// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_OCCUR_H_
#define XFA_FXFA_PARSER_CXFA_OCCUR_H_

#include <tuple>

#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_Occur final : public CXFA_Node {
 public:
  static constexpr int32_t kDefaultMax = 1;
  static constexpr int32_t kDefaultMin = 1;

  CXFA_Occur(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Occur() override;

  int32_t GetMax();
  void SetMax(int32_t iMax);

  int32_t GetMin();
  void SetMin(int32_t iMin);

  std::tuple<int32_t, int32_t, int32_t> GetOccurInfo();
};

#endif  // XFA_FXFA_PARSER_CXFA_OCCUR_H_
