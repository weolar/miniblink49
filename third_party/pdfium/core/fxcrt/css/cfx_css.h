// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSS_H_
#define CORE_FXCRT_CSS_CFX_CSS_H_

#include <stdint.h>

enum CFX_CSSVALUETYPE {
  CFX_CSSVALUETYPE_Primitive = 1 << 0,
  CFX_CSSVALUETYPE_List = 1 << 1,
  CFX_CSSVALUETYPE_Shorthand = 1 << 2,
  // Note the values below this comment must be > 0x0F so we can mask the above.
  CFX_CSSVALUETYPE_MaybeNumber = 1 << 4,
  CFX_CSSVALUETYPE_MaybeEnum = 1 << 5,
  CFX_CSSVALUETYPE_MaybeString = 1 << 7,
  CFX_CSSVALUETYPE_MaybeColor = 1 << 8
};

enum class CFX_CSSPrimitiveType : uint8_t {
  Unknown = 0,
  Number,
  String,
  RGB,
  Enum,
  Function,
  List,
};

// Any entries added/removed here, will need to be mirrored in
// propertyValueTable, in core/fxcrt/css/cfx_cssdata.cpp.
enum class CFX_CSSPropertyValue : uint8_t {
  Bolder = 0,
  None,
  Dot,
  Sub,
  Top,
  Right,
  Normal,
  Auto,
  Text,
  XSmall,
  Thin,
  Small,
  Bottom,
  Underline,
  Double,
  Lighter,
  Oblique,
  Super,
  Center,
  XxLarge,
  Smaller,
  Baseline,
  Thick,
  Justify,
  Middle,
  Medium,
  ListItem,
  XxSmall,
  Bold,
  SmallCaps,
  Inline,
  Overline,
  TextBottom,
  Larger,
  InlineTable,
  InlineBlock,
  Blink,
  Block,
  Italic,
  LineThrough,
  XLarge,
  Large,
  Left,
  TextTop,
};

// Any entries added/removed here, will need to be mirrored in
// propertyTable, in core/fxcrt/css/cfx_cssdata.cpp.
enum class CFX_CSSProperty : uint8_t {
  BorderLeft = 0,
  Top,
  Margin,
  TextIndent,
  Right,
  PaddingLeft,
  MarginLeft,
  Border,
  BorderTop,
  Bottom,
  PaddingRight,
  BorderBottom,
  FontFamily,
  FontWeight,
  Color,
  LetterSpacing,
  TextAlign,
  BorderRightWidth,
  VerticalAlign,
  PaddingTop,
  FontVariant,
  BorderWidth,
  BorderBottomWidth,
  BorderRight,
  FontSize,
  BorderSpacing,
  FontStyle,
  Font,
  LineHeight,
  MarginRight,
  BorderLeftWidth,
  Display,
  PaddingBottom,
  BorderTopWidth,
  WordSpacing,
  Left,
  TextDecoration,
  Padding,
  MarginBottom,
  MarginTop,
};

enum class CFX_CSSSelectorType : uint8_t { Element = 0, Descendant };

enum class CFX_CSSLengthUnit : uint8_t {
  Auto,
  None,
  Normal,
  Point,
  Percent,
};

enum class CFX_CSSDisplay : uint8_t {
  None,
  ListItem,
  Block,
  Inline,
  InlineBlock,
  InlineTable,
};

enum class CFX_CSSFontStyle : uint8_t {
  Normal,
  Italic,
};

enum class CFX_CSSTextAlign : uint8_t {
  Left,
  Right,
  Center,
  Justify,
  JustifyAll,
};

enum class CFX_CSSVerticalAlign : uint8_t {
  Baseline,
  Sub,
  Super,
  Top,
  TextTop,
  Middle,
  Bottom,
  TextBottom,
  Number,
};

enum class CFX_CSSFontVariant : uint8_t {
  Normal,
  SmallCaps,
};

enum CFX_CSSTEXTDECORATION {
  CFX_CSSTEXTDECORATION_None = 0,
  CFX_CSSTEXTDECORATION_Underline = 1 << 0,
  CFX_CSSTEXTDECORATION_Overline = 1 << 1,
  CFX_CSSTEXTDECORATION_LineThrough = 1 << 2,
  CFX_CSSTEXTDECORATION_Blink = 1 << 3,
  CFX_CSSTEXTDECORATION_Double = 1 << 4,
};

class CFX_CSSLength {
 public:
  CFX_CSSLength() {}

  CFX_CSSLength(CFX_CSSLengthUnit eUnit, float fValue)
      : m_unit(eUnit), m_fValue(fValue) {}

  CFX_CSSLength& Set(CFX_CSSLengthUnit eUnit) {
    m_unit = eUnit;
    return *this;
  }

  CFX_CSSLength& Set(CFX_CSSLengthUnit eUnit, float fValue) {
    m_unit = eUnit;
    m_fValue = fValue;
    return *this;
  }

  CFX_CSSLengthUnit GetUnit() const { return m_unit; }

  float GetValue() const { return m_fValue; }
  bool NonZero() const { return static_cast<int>(m_fValue) != 0; }

 private:
  CFX_CSSLengthUnit m_unit;
  float m_fValue;
};

class CFX_CSSRect {
 public:
  CFX_CSSRect() {}

  CFX_CSSRect(CFX_CSSLengthUnit eUnit, float val)
      : left(eUnit, val),
        top(eUnit, val),
        right(eUnit, val),
        bottom(eUnit, val) {}

  CFX_CSSRect& Set(CFX_CSSLengthUnit eUnit) {
    left.Set(eUnit);
    top.Set(eUnit);
    right.Set(eUnit);
    bottom.Set(eUnit);
    return *this;
  }
  CFX_CSSRect& Set(CFX_CSSLengthUnit eUnit, float fValue) {
    left.Set(eUnit, fValue);
    top.Set(eUnit, fValue);
    right.Set(eUnit, fValue);
    bottom.Set(eUnit, fValue);
    return *this;
  }

  CFX_CSSLength left;
  CFX_CSSLength top;
  CFX_CSSLength right;
  CFX_CSSLength bottom;
};

#endif  // CORE_FXCRT_CSS_CFX_CSS_H_
