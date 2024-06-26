// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_PIECELINE_H_
#define XFA_FXFA_CXFA_PIECELINE_H_

#include <memory>
#include <vector>

class CXFA_TextPiece;

class CXFA_PieceLine {
 public:
  CXFA_PieceLine();
  ~CXFA_PieceLine();

  std::vector<std::unique_ptr<CXFA_TextPiece>> m_textPieces;
  std::vector<size_t> m_charCounts;
};

#endif  // XFA_FXFA_CXFA_PIECELINE_H_
