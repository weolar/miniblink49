// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFTEXT_CPDF_TEXTPAGEFIND_H_
#define CORE_FPDFTEXT_CPDF_TEXTPAGEFIND_H_

#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"
#include "third_party/base/optional.h"

class CPDF_TextPage;

class CPDF_TextPageFind {
 public:
  explicit CPDF_TextPageFind(const CPDF_TextPage* pTextPage);
  ~CPDF_TextPageFind();

  bool FindFirst(const WideString& findwhat,
                 int flags,
                 Optional<size_t> startPos);
  bool FindNext();
  bool FindPrev();
  int GetCurOrder() const;
  int GetMatchedCount() const;

 protected:
  void ExtractFindWhat(const WideString& findwhat);
  bool IsMatchWholeWord(const WideString& csPageText,
                        size_t startPos,
                        size_t endPos);
  Optional<WideString> ExtractSubString(const wchar_t* lpszFullString,
                                        int iSubString,
                                        wchar_t chSep);
  int GetCharIndex(int index) const;

 private:
  std::vector<uint16_t> m_CharIndex;
  UnownedPtr<const CPDF_TextPage> m_pTextPage;
  WideString m_strText;
  WideString m_findWhat;
  int m_flags;
  std::vector<WideString> m_csFindWhatArray;
  Optional<size_t> m_findNextStart;
  Optional<size_t> m_findPreStart;
  bool m_bMatchCase;
  bool m_bMatchWholeWord;
  int m_resStart;
  int m_resEnd;
  std::vector<CFX_FloatRect> m_resArray;
  bool m_IsFind;
};

#endif  // CORE_FPDFTEXT_CPDF_TEXTPAGEFIND_H_
