// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_CONTENTPARSER_H_
#define CORE_FPDFAPI_PAGE_CPDF_CONTENTPARSER_H_

#include <memory>
#include <set>
#include <vector>

#include "core/fpdfapi/page/cpdf_streamcontentparser.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fxcrt/maybe_owned.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_AllStates;
class CPDF_Form;
class CPDF_Page;
class CPDF_PageObjectHolder;
class CPDF_StreamAcc;
class CPDF_Type3Char;

class CPDF_ContentParser {
 public:
  explicit CPDF_ContentParser(CPDF_Page* pPage);
  CPDF_ContentParser(CPDF_Form* pForm,
                     CPDF_AllStates* pGraphicStates,
                     const CFX_Matrix* pParentMatrix,
                     CPDF_Type3Char* pType3Char,
                     std::set<const uint8_t*>* parsedSet);
  ~CPDF_ContentParser();

  const CPDF_AllStates* GetCurStates() const {
    return m_pParser ? m_pParser->GetCurStates() : nullptr;
  }
  // Returns whether to continue or not.
  bool Continue(PauseIndicatorIface* pPause);

 private:
  enum class Stage : uint8_t {
    kGetContent = 1,
    kPrepareContent,
    kParse,
    kCheckClip,
    kComplete,
  };

  Stage GetContent();
  Stage PrepareContent();
  Stage Parse();
  Stage CheckClip();

  Stage m_CurrentStage;
  UnownedPtr<CPDF_PageObjectHolder> const m_pObjectHolder;
  UnownedPtr<CPDF_Type3Char> m_pType3Char;  // Only used when parsing forms.
  RetainPtr<CPDF_StreamAcc> m_pSingleStream;
  std::vector<RetainPtr<CPDF_StreamAcc>> m_StreamArray;
  std::vector<uint32_t> m_StreamSegmentOffsets;
  MaybeOwned<uint8_t, FxFreeDeleter> m_pData;
  uint32_t m_nStreams = 0;
  uint32_t m_Size = 0;
  uint32_t m_CurrentOffset = 0;

  // Only used when parsing pages.
  std::unique_ptr<std::set<const uint8_t*>> m_parsedSet;

  // |m_pParser| has a reference to |m_parsedSet|, so must be below and thus
  // destroyed first.
  std::unique_ptr<CPDF_StreamContentParser> m_pParser;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_CONTENTPARSER_H_
