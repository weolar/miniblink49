// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_TEXTSTATE_H_
#define CORE_FPDFAPI_PAGE_CPDF_TEXTSTATE_H_

#include "core/fxcrt/shared_copy_on_write.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_Document;
class CPDF_Font;

// See PDF Reference 1.7, page 402, table 5.3.
enum class TextRenderingMode {
  MODE_FILL = 0,
  MODE_STROKE = 1,
  MODE_FILL_STROKE = 2,
  MODE_INVISIBLE = 3,
  MODE_FILL_CLIP = 4,
  MODE_STROKE_CLIP = 5,
  MODE_FILL_STROKE_CLIP = 6,
  MODE_CLIP = 7,
};

class CPDF_TextState {
 public:
  CPDF_TextState();
  ~CPDF_TextState();

  void Emplace();

  CPDF_Font* GetFont() const;
  void SetFont(CPDF_Font* pFont);

  float GetFontSize() const;
  void SetFontSize(float size);

  const float* GetMatrix() const;
  float* GetMutableMatrix();

  float GetCharSpace() const;
  void SetCharSpace(float sp);

  float GetWordSpace() const;
  void SetWordSpace(float sp);

  float GetFontSizeV() const;
  float GetFontSizeH() const;
  float GetBaselineAngle() const;
  float GetShearAngle() const;

  TextRenderingMode GetTextMode() const;
  void SetTextMode(TextRenderingMode mode);

  const float* GetCTM() const;
  float* GetMutableCTM();

 private:
  class TextData final : public Retainable {
   public:
    TextData();
    TextData(const TextData& src);
    ~TextData() override;

    void SetFont(CPDF_Font* pFont);
    float GetFontSizeV() const;
    float GetFontSizeH() const;
    float GetBaselineAngle() const;
    float GetShearAngle() const;

    CPDF_Font* m_pFont;
    UnownedPtr<CPDF_Document> m_pDocument;
    float m_FontSize;
    float m_CharSpace;
    float m_WordSpace;
    TextRenderingMode m_TextMode;
    float m_Matrix[4];
    float m_CTM[4];

   private:
    void ReleaseFont();
  };

  SharedCopyOnWrite<TextData> m_Ref;
};

bool SetTextRenderingModeFromInt(int iMode, TextRenderingMode* mode);
bool TextRenderingModeIsClipMode(const TextRenderingMode& mode);
bool TextRenderingModeIsStrokeMode(const TextRenderingMode& mode);

#endif  // CORE_FPDFAPI_PAGE_CPDF_TEXTSTATE_H_
