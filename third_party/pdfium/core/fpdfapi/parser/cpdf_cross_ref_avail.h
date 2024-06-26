// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FPDFAPI_PARSER_CPDF_CROSS_REF_AVAIL_H_
#define CORE_FPDFAPI_PARSER_CPDF_CROSS_REF_AVAIL_H_

#include <queue>
#include <set>

#include "core/fpdfapi/parser/cpdf_data_avail.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_SyntaxParser;

class CPDF_CrossRefAvail {
 public:
  CPDF_CrossRefAvail(CPDF_SyntaxParser* parser,
                     FX_FILESIZE last_crossref_offset);
  ~CPDF_CrossRefAvail();

  FX_FILESIZE last_crossref_offset() const { return last_crossref_offset_; }

  CPDF_DataAvail::DocAvailStatus CheckAvail();

 private:
  enum class State {
    kCrossRefCheck,
    kCrossRefV4ItemCheck,
    kCrossRefV4TrailerCheck,
    kDone,
  };

  bool CheckReadProblems();
  bool CheckCrossRef();
  bool CheckCrossRefV4();
  bool CheckCrossRefV4Item();
  bool CheckCrossRefV4Trailer();
  bool CheckCrossRefStream();

  void AddCrossRefForCheck(FX_FILESIZE crossref_offset);

  fxcrt::RetainPtr<CPDF_ReadValidator> GetValidator();

  fxcrt::UnownedPtr<CPDF_SyntaxParser> parser_;
  const FX_FILESIZE last_crossref_offset_ = 0;
  CPDF_DataAvail::DocAvailStatus current_status_ =
      CPDF_DataAvail::DataNotAvailable;
  State current_state_ = State::kCrossRefCheck;
  FX_FILESIZE current_offset_ = 0;
  std::queue<FX_FILESIZE> cross_refs_for_check_;
  std::set<FX_FILESIZE> registered_crossrefs_;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_CROSS_REF_AVAIL_H_
