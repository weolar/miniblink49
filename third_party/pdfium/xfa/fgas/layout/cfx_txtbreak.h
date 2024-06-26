// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_LAYOUT_CFX_TXTBREAK_H_
#define XFA_FGAS_LAYOUT_CFX_TXTBREAK_H_

#include <deque>
#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "xfa/fgas/layout/cfx_break.h"
#include "xfa/fgas/layout/cfx_char.h"

class CFGAS_GEFont;
class FXTEXT_CHARPOS;

#define FX_TXTCHARSTYLE_ArabicShadda 0x0020
#define FX_TXTCHARSTYLE_OddBidiLevel 0x0040

enum CFX_TxtLineAlignment {
  CFX_TxtLineAlignment_Left = 0,
  CFX_TxtLineAlignment_Center = 1 << 0,
  CFX_TxtLineAlignment_Right = 1 << 1,
  CFX_TxtLineAlignment_Justified = 1 << 2
};

inline bool CFX_BreakTypeNoneOrPiece(CFX_BreakType type) {
  return type == CFX_BreakType::None || type == CFX_BreakType::Piece;
}

class CFX_TxtBreak final : public CFX_Break {
 public:
  class Engine {
   public:
    virtual ~Engine();
    virtual wchar_t GetChar(size_t idx) const = 0;
    // Non-const so we can force a layout if needed.
    virtual size_t GetWidthOfChar(size_t idx) = 0;
  };

  struct Run {
    Run();
    Run(const Run& other);
    ~Run();

    CFX_TxtBreak::Engine* pEdtEngine = nullptr;
    WideString wsStr;
    int32_t* pWidths = nullptr;
    int32_t iStart = 0;
    int32_t iLength = 0;
    RetainPtr<CFGAS_GEFont> pFont;
    float fFontSize = 12.0f;
    uint32_t dwStyles = 0;
    int32_t iHorizontalScale = 100;
    int32_t iVerticalScale = 100;
    uint32_t dwCharStyles = 0;
    const CFX_RectF* pRect = nullptr;
    bool bSkipSpace = true;
  };

  CFX_TxtBreak();
  ~CFX_TxtBreak() override;

  void SetLineWidth(float fLineWidth);
  void SetAlignment(int32_t iAlignment);
  void SetCombWidth(float fCombWidth);
  CFX_BreakType EndBreak(CFX_BreakType dwStatus);

  size_t GetDisplayPos(const Run* pTxtRun, FXTEXT_CHARPOS* pCharPos) const;
  std::vector<CFX_RectF> GetCharRects(const Run* pTxtRun, bool bCharBBox) const;
  CFX_BreakType AppendChar(wchar_t wch);

 private:
  void AppendChar_Combination(CFX_Char* pCurChar);
  void AppendChar_Tab(CFX_Char* pCurChar);
  CFX_BreakType AppendChar_Control(CFX_Char* pCurChar);
  CFX_BreakType AppendChar_Arabic(CFX_Char* pCurChar);
  CFX_BreakType AppendChar_Others(CFX_Char* pCurChar);

  void ResetContextCharStyles();
  bool EndBreak_SplitLine(CFX_BreakLine* pNextLine, bool bAllChars);
  void EndBreak_BidiLine(std::deque<FX_TPO>* tpos, CFX_BreakType dwStatus);
  void EndBreak_Alignment(const std::deque<FX_TPO>& tpos,
                          bool bAllChars,
                          CFX_BreakType dwStatus);
  int32_t GetBreakPos(std::vector<CFX_Char>* pChars,
                      bool bAllChars,
                      bool bOnlyBrk,
                      int32_t* pEndPos);
  void SplitTextLine(CFX_BreakLine* pCurLine,
                     CFX_BreakLine* pNextLine,
                     bool bAllChars);

  int32_t m_iAlignment;
  int32_t m_iCombWidth;
};

#endif  // XFA_FGAS_LAYOUT_CFX_TXTBREAK_H_
