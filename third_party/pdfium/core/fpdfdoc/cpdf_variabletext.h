// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_VARIABLETEXT_H_
#define CORE_FPDFDOC_CPDF_VARIABLETEXT_H_

#include <memory>
#include <vector>

#include "core/fpdfdoc/cpvt_floatrect.h"
#include "core/fpdfdoc/cpvt_line.h"
#include "core/fpdfdoc/cpvt_lineinfo.h"
#include "core/fpdfdoc/cpvt_wordplace.h"
#include "core/fpdfdoc/cpvt_wordrange.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CPVT_Word;
class CSection;
class IPVT_FontMap;
struct CPVT_WordInfo;

#define VARIABLETEXT_HALF 0.5f

class CPDF_VariableText {
 public:
  class Iterator {
   public:
    explicit Iterator(CPDF_VariableText* pVT);
    ~Iterator();

    bool NextWord();
    bool PrevWord();
    bool NextLine();
    bool GetWord(CPVT_Word& word) const;
    bool GetLine(CPVT_Line& line) const;
    void SetAt(int32_t nWordIndex);
    void SetAt(const CPVT_WordPlace& place);
    const CPVT_WordPlace& GetWordPlace() const { return m_CurPos; }

   private:
    CPVT_WordPlace m_CurPos;
    UnownedPtr<CPDF_VariableText> const m_pVT;
  };

  class Provider {
   public:
    explicit Provider(IPVT_FontMap* pFontMap);
    virtual ~Provider();

    virtual uint32_t GetCharWidth(int32_t nFontIndex, uint16_t word);
    virtual int32_t GetTypeAscent(int32_t nFontIndex);
    virtual int32_t GetTypeDescent(int32_t nFontIndex);
    virtual int32_t GetWordFontIndex(uint16_t word,
                                     int32_t charset,
                                     int32_t nFontIndex);
    virtual bool IsLatinWord(uint16_t word);
    virtual int32_t GetDefaultFontIndex();

   private:
    UnownedPtr<IPVT_FontMap> const m_pFontMap;
  };

  CPDF_VariableText();
  ~CPDF_VariableText();

  void SetProvider(CPDF_VariableText::Provider* pProvider);
  CPDF_VariableText::Iterator* GetIterator();

  void SetContentRect(const CPVT_FloatRect& rect);
  CFX_FloatRect GetContentRect() const;
  void SetPlateRect(const CFX_FloatRect& rect);
  const CFX_FloatRect& GetPlateRect() const;

  void SetAlignment(int32_t nFormat) { m_nAlignment = nFormat; }
  void SetPasswordChar(uint16_t wSubWord) { m_wSubWord = wSubWord; }
  void SetLimitChar(int32_t nLimitChar) { m_nLimitChar = nLimitChar; }
  void SetCharSpace(float fCharSpace) { m_fCharSpace = fCharSpace; }
  void SetMultiLine(bool bMultiLine) { m_bMultiLine = bMultiLine; }
  void SetAutoReturn(bool bAuto) { m_bLimitWidth = bAuto; }
  void SetFontSize(float fFontSize) { m_fFontSize = fFontSize; }
  void SetCharArray(int32_t nCharArray) { m_nCharArray = nCharArray; }
  void SetAutoFontSize(bool bAuto) { m_bAutoFontSize = bAuto; }
  void Initialize();

  bool IsValid() const { return m_bInitialized; }

  void RearrangeAll();
  void RearrangePart(const CPVT_WordRange& PlaceRange);
  void SetText(const WideString& text);
  CPVT_WordPlace InsertWord(const CPVT_WordPlace& place,
                            uint16_t word,
                            int32_t charset);
  CPVT_WordPlace InsertSection(const CPVT_WordPlace& place);
  CPVT_WordPlace DeleteWords(const CPVT_WordRange& PlaceRange);
  CPVT_WordPlace DeleteWord(const CPVT_WordPlace& place);
  CPVT_WordPlace BackSpaceWord(const CPVT_WordPlace& place);

  int32_t GetTotalWords() const;
  float GetFontSize() const { return m_fFontSize; }
  int32_t GetAlignment() const { return m_nAlignment; }
  uint16_t GetPasswordChar() const { return GetSubWord(); }
  int32_t GetCharArray() const { return m_nCharArray; }
  int32_t GetLimitChar() const { return m_nLimitChar; }
  bool IsMultiLine() const { return m_bMultiLine; }
  float GetCharSpace() const { return m_fCharSpace; }
  bool IsAutoReturn() const { return m_bLimitWidth; }

  CPVT_WordPlace GetBeginWordPlace() const;
  CPVT_WordPlace GetEndWordPlace() const;
  CPVT_WordPlace GetPrevWordPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace GetNextWordPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace SearchWordPlace(const CFX_PointF& point) const;
  CPVT_WordPlace GetUpWordPlace(const CPVT_WordPlace& place,
                                const CFX_PointF& point) const;
  CPVT_WordPlace GetDownWordPlace(const CPVT_WordPlace& place,
                                  const CFX_PointF& point) const;
  CPVT_WordPlace GetLineBeginPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace GetLineEndPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace GetSectionBeginPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace GetSectionEndPlace(const CPVT_WordPlace& place) const;
  void UpdateWordPlace(CPVT_WordPlace& place) const;
  CPVT_WordPlace AdjustLineHeader(const CPVT_WordPlace& place,
                                  bool bPrevOrNext) const;
  int32_t WordPlaceToWordIndex(const CPVT_WordPlace& place) const;
  CPVT_WordPlace WordIndexToWordPlace(int32_t index) const;

  uint16_t GetSubWord() const { return m_wSubWord; }

  float GetPlateWidth() const { return m_rcPlate.right - m_rcPlate.left; }
  float GetPlateHeight() const { return m_rcPlate.top - m_rcPlate.bottom; }
  CFX_PointF GetBTPoint() const;
  CFX_PointF GetETPoint() const;

  CFX_PointF InToOut(const CFX_PointF& point) const;
  CFX_PointF OutToIn(const CFX_PointF& point) const;
  CFX_FloatRect InToOut(const CPVT_FloatRect& rect) const;
  CPVT_FloatRect OutToIn(const CFX_FloatRect& rect) const;

  float GetFontAscent(int32_t nFontIndex, float fFontSize);
  float GetFontDescent(int32_t nFontIndex, float fFontSize);
  int32_t GetDefaultFontIndex();
  float GetLineLeading();
  int32_t GetAlignment();
  float GetWordWidth(const CPVT_WordInfo& WordInfo);
  float GetWordWidth(int32_t nFontIndex,
                     uint16_t Word,
                     uint16_t SubWord,
                     float fCharSpace,
                     float fFontSize,
                     float fWordTail);
  float GetWordAscent(const CPVT_WordInfo& WordInfo);
  float GetWordDescent(const CPVT_WordInfo& WordInfo);
  float GetWordAscent(const CPVT_WordInfo& WordInfo, float fFontSize);
  float GetWordDescent(const CPVT_WordInfo& WordInfo, float fFontSize);
  float GetLineAscent();
  float GetLineDescent();
  float GetLineIndent();

 private:
  uint32_t GetCharWidth(int32_t nFontIndex, uint16_t Word, uint16_t SubWord);
  int32_t GetTypeAscent(int32_t nFontIndex);
  int32_t GetTypeDescent(int32_t nFontIndex);
  int32_t GetWordFontIndex(uint16_t word, int32_t charset, int32_t nFontIndex);
  bool IsLatinWord(uint16_t word);

  CPVT_WordPlace AddSection(const CPVT_WordPlace& place);
  CPVT_WordPlace AddLine(const CPVT_WordPlace& place,
                         const CPVT_LineInfo& lineinfo);
  CPVT_WordPlace AddWord(const CPVT_WordPlace& place,
                         const CPVT_WordInfo& wordinfo);
  float GetWordFontSize();
  int32_t GetWordFontIndex(const CPVT_WordInfo& WordInfo);

  void ClearSectionRightWords(const CPVT_WordPlace& place);

  bool ClearEmptySection(const CPVT_WordPlace& place);
  void ClearEmptySections(const CPVT_WordRange& PlaceRange);
  void LinkLatterSection(const CPVT_WordPlace& place);
  void ClearWords(const CPVT_WordRange& PlaceRange);
  CPVT_WordPlace ClearLeftWord(const CPVT_WordPlace& place);
  CPVT_WordPlace ClearRightWord(const CPVT_WordPlace& place);

  CPVT_FloatRect Rearrange(const CPVT_WordRange& PlaceRange);
  float GetAutoFontSize();
  bool IsBigger(float fFontSize) const;
  CPVT_FloatRect RearrangeSections(const CPVT_WordRange& PlaceRange);

  bool m_bInitialized = false;
  bool m_bMultiLine = false;
  bool m_bLimitWidth = false;
  bool m_bAutoFontSize = false;
  uint16_t m_wSubWord = 0;
  int32_t m_nLimitChar = 0;
  int32_t m_nCharArray = 0;
  int32_t m_nAlignment = 0;
  float m_fLineLeading = 0.0f;
  float m_fCharSpace = 0.0f;
  float m_fFontSize = 0.0f;
  std::vector<std::unique_ptr<CSection>> m_SectionArray;
  UnownedPtr<CPDF_VariableText::Provider> m_pVTProvider;
  std::unique_ptr<CPDF_VariableText::Iterator> m_pVTIterator;
  CFX_FloatRect m_rcPlate;
  CPVT_FloatRect m_rcContent;
};

#endif  // CORE_FPDFDOC_CPDF_VARIABLETEXT_H_
