// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_IFWL_THEMEPROVIDER_H_
#define XFA_FWL_IFWL_THEMEPROVIDER_H_

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/fx_dib.h"

class CFGAS_GEFont;
class CFWL_ThemeBackground;
class CFWL_ThemePart;
class CFWL_ThemeText;
class CFWL_Widget;

class IFWL_ThemeProvider {
 public:
  virtual ~IFWL_ThemeProvider() = default;

  virtual void DrawBackground(const CFWL_ThemeBackground& pParams) = 0;
  virtual void DrawText(const CFWL_ThemeText& pParams) = 0;
  virtual void CalcTextRect(const CFWL_ThemeText& pParams,
                            CFX_RectF* pRect) = 0;
  virtual float GetCXBorderSize() const = 0;
  virtual float GetCYBorderSize() const = 0;
  virtual CFX_RectF GetUIMargin(const CFWL_ThemePart& pThemePart) const = 0;
  virtual float GetFontSize(const CFWL_ThemePart& pThemePart) const = 0;
  virtual RetainPtr<CFGAS_GEFont> GetFont(
      const CFWL_ThemePart& pThemePart) const = 0;
  virtual float GetLineHeight(const CFWL_ThemePart& pThemePart) const = 0;
  virtual float GetScrollBarWidth() const = 0;
  virtual FX_COLORREF GetTextColor(const CFWL_ThemePart& pThemePart) const = 0;
  virtual CFX_SizeF GetSpaceAboveBelow(
      const CFWL_ThemePart& pThemePart) const = 0;
};

#endif  // XFA_FWL_IFWL_THEMEPROVIDER_H_
