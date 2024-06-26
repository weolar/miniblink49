// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_JBIG2_JBIG2_PATTERNDICT_H_
#define CORE_FXCODEC_JBIG2_JBIG2_PATTERNDICT_H_

#include <memory>
#include <vector>

#include "core/fxcodec/jbig2/JBig2_Define.h"
#include "core/fxcodec/jbig2/JBig2_Image.h"

class CJBig2_PatternDict {
 public:
  explicit CJBig2_PatternDict(uint32_t dict_size);
  ~CJBig2_PatternDict();

  uint32_t NUMPATS;
  std::vector<std::unique_ptr<CJBig2_Image>> HDPATS;
};

#endif  // CORE_FXCODEC_JBIG2_JBIG2_PATTERNDICT_H_
