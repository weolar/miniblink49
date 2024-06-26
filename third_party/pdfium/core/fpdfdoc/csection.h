// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CSECTION_H_
#define CORE_FPDFDOC_CSECTION_H_

#include <memory>
#include <vector>

#include "core/fpdfdoc/cline.h"
#include "core/fpdfdoc/cpvt_wordinfo.h"
#include "core/fpdfdoc/cpvt_wordrange.h"
#include "core/fpdfdoc/ctypeset.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"

class CPDF_VariableText;
class CPVT_LineInfo;
struct CPVT_WordLine;
struct CPVT_WordPlace;

class CSection final {
 public:
  explicit CSection(CPDF_VariableText* pVT);
  ~CSection();

  void ResetLinePlace();
  CPVT_WordPlace AddWord(const CPVT_WordPlace& place,
                         const CPVT_WordInfo& wordinfo);
  CPVT_WordPlace AddLine(const CPVT_LineInfo& lineinfo);
  void ClearWords(const CPVT_WordRange& PlaceRange);
  void ClearWord(const CPVT_WordPlace& place);
  CPVT_FloatRect Rearrange();
  CFX_SizeF GetSectionSize(float fFontSize);
  CPVT_WordPlace GetBeginWordPlace() const;
  CPVT_WordPlace GetEndWordPlace() const;
  CPVT_WordPlace GetPrevWordPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace GetNextWordPlace(const CPVT_WordPlace& place) const;
  void UpdateWordPlace(CPVT_WordPlace& place) const;
  CPVT_WordPlace SearchWordPlace(const CFX_PointF& point) const;
  CPVT_WordPlace SearchWordPlace(float fx,
                                 const CPVT_WordPlace& lineplace) const;
  CPVT_WordPlace SearchWordPlace(float fx, const CPVT_WordRange& range) const;

  CPVT_WordPlace SecPlace;
  CPVT_FloatRect m_Rect;
  std::vector<std::unique_ptr<CLine>> m_LineArray;
  std::vector<std::unique_ptr<CPVT_WordInfo>> m_WordArray;

 private:
  friend class CTypeset;

  void ClearLeftWords(int32_t nWordIndex);
  void ClearRightWords(int32_t nWordIndex);
  void ClearMidWords(int32_t nBeginIndex, int32_t nEndIndex);

  UnownedPtr<CPDF_VariableText> const m_pVT;
};

#endif  // CORE_FPDFDOC_CSECTION_H_
