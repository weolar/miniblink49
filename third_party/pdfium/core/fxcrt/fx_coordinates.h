// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_FX_COORDINATES_H_
#define CORE_FXCRT_FX_COORDINATES_H_

#include <algorithm>
#include <tuple>

#include "core/fxcrt/fx_system.h"
#include "third_party/base/numerics/safe_math.h"

template <class BaseType>
class CFX_PTemplate {
 public:
  CFX_PTemplate() : x(0), y(0) {}
  CFX_PTemplate(BaseType new_x, BaseType new_y) : x(new_x), y(new_y) {}
  CFX_PTemplate(const CFX_PTemplate& other) : x(other.x), y(other.y) {}

  CFX_PTemplate operator=(const CFX_PTemplate& other) {
    if (this != &other) {
      x = other.x;
      y = other.y;
    }
    return *this;
  }
  bool operator==(const CFX_PTemplate& other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const CFX_PTemplate& other) const {
    return !(*this == other);
  }
  CFX_PTemplate& operator+=(const CFX_PTemplate<BaseType>& obj) {
    x += obj.x;
    y += obj.y;
    return *this;
  }
  CFX_PTemplate& operator-=(const CFX_PTemplate<BaseType>& obj) {
    x -= obj.x;
    y -= obj.y;
    return *this;
  }
  CFX_PTemplate operator+(const CFX_PTemplate& other) const {
    return CFX_PTemplate(x + other.x, y + other.y);
  }
  CFX_PTemplate operator-(const CFX_PTemplate& other) const {
    return CFX_PTemplate(x - other.x, y - other.y);
  }

  BaseType x;
  BaseType y;
};
using CFX_Point = CFX_PTemplate<int32_t>;
using CFX_PointF = CFX_PTemplate<float>;

template <class BaseType>
class CFX_STemplate {
 public:
  CFX_STemplate() : width(0), height(0) {}

  CFX_STemplate(BaseType new_width, BaseType new_height)
      : width(new_width), height(new_height) {}

  CFX_STemplate(const CFX_STemplate& other)
      : width(other.width), height(other.height) {}

  template <typename OtherType>
  CFX_STemplate<OtherType> As() const {
    return CFX_STemplate<OtherType>(static_cast<OtherType>(width),
                                    static_cast<OtherType>(height));
  }

  void clear() {
    width = 0;
    height = 0;
  }
  CFX_STemplate operator=(const CFX_STemplate& other) {
    if (this != &other) {
      width = other.width;
      height = other.height;
    }
    return *this;
  }
  bool operator==(const CFX_STemplate& other) const {
    return width == other.width && height == other.height;
  }
  bool operator!=(const CFX_STemplate& other) const {
    return !(*this == other);
  }
  CFX_STemplate& operator+=(const CFX_STemplate<BaseType>& obj) {
    width += obj.width;
    height += obj.height;
    return *this;
  }
  CFX_STemplate& operator-=(const CFX_STemplate<BaseType>& obj) {
    width -= obj.width;
    height -= obj.height;
    return *this;
  }
  CFX_STemplate& operator*=(BaseType factor) {
    width *= factor;
    height *= factor;
    return *this;
  }
  CFX_STemplate& operator/=(BaseType divisor) {
    width /= divisor;
    height /= divisor;
    return *this;
  }
  CFX_STemplate operator+(const CFX_STemplate& other) const {
    return CFX_STemplate(width + other.width, height + other.height);
  }
  CFX_STemplate operator-(const CFX_STemplate& other) const {
    return CFX_STemplate(width - other.width, height - other.height);
  }
  CFX_STemplate operator*(BaseType factor) const {
    return CFX_STemplate(width * factor, height * factor);
  }
  CFX_STemplate operator/(BaseType divisor) const {
    return CFX_STemplate(width / divisor, height / divisor);
  }

  BaseType width;
  BaseType height;
};
using CFX_Size = CFX_STemplate<int32_t>;
using CFX_SizeF = CFX_STemplate<float>;

template <class BaseType>
class CFX_VTemplate final : public CFX_PTemplate<BaseType> {
 public:
  using CFX_PTemplate<BaseType>::x;
  using CFX_PTemplate<BaseType>::y;

  CFX_VTemplate() : CFX_PTemplate<BaseType>() {}
  CFX_VTemplate(BaseType new_x, BaseType new_y)
      : CFX_PTemplate<BaseType>(new_x, new_y) {}

  CFX_VTemplate(const CFX_VTemplate& other) : CFX_PTemplate<BaseType>(other) {}

  CFX_VTemplate(const CFX_PTemplate<BaseType>& point1,
                const CFX_PTemplate<BaseType>& point2)
      : CFX_PTemplate<BaseType>(point2.x - point1.x, point2.y - point1.y) {}

  float Length() const { return sqrt(x * x + y * y); }
  void Normalize() {
    float fLen = Length();
    if (fLen < 0.0001f)
      return;

    x /= fLen;
    y /= fLen;
  }
  void Translate(BaseType dx, BaseType dy) {
    x += dx;
    y += dy;
  }
  void Scale(BaseType sx, BaseType sy) {
    x *= sx;
    y *= sy;
  }
  void Rotate(float fRadian) {
    float cosValue = cos(fRadian);
    float sinValue = sin(fRadian);
    x = x * cosValue - y * sinValue;
    y = x * sinValue + y * cosValue;
  }
};
using CFX_Vector = CFX_VTemplate<int32_t>;
using CFX_VectorF = CFX_VTemplate<float>;

// Rectangles.
// TODO(tsepez): Consolidate all these different rectangle classes.

// LTRB rectangles (y-axis runs downwards).
// Struct layout is compatible with win32 RECT.
struct FX_RECT {
  FX_RECT() : left(0), top(0), right(0), bottom(0) {}
  FX_RECT(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}

  int Width() const { return right - left; }
  int Height() const { return bottom - top; }
  bool IsEmpty() const { return right <= left || bottom <= top; }

  bool Valid() const {
    pdfium::base::CheckedNumeric<int> w = right;
    pdfium::base::CheckedNumeric<int> h = bottom;
    w -= left;
    h -= top;
    return w.IsValid() && h.IsValid();
  }

  void Normalize();

  void Intersect(const FX_RECT& src);
  void Intersect(int l, int t, int r, int b) { Intersect(FX_RECT(l, t, r, b)); }

  void Offset(int dx, int dy) {
    left += dx;
    right += dx;
    top += dy;
    bottom += dy;
  }

  bool operator==(const FX_RECT& src) const {
    return left == src.left && right == src.right && top == src.top &&
           bottom == src.bottom;
  }

  bool Contains(int x, int y) const {
    return x >= left && x < right && y >= top && y < bottom;
  }

  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;
};

// LTRB rectangles (y-axis runs upwards).
class CFX_FloatRect {
 public:
  /*constexpr*/ CFX_FloatRect() : CFX_FloatRect(0.0f, 0.0f, 0.0f, 0.0f) {}
  /*constexpr*/ CFX_FloatRect(float l, float b, float r, float t)
      : left(l), bottom(b), right(r), top(t) {}

  explicit CFX_FloatRect(const float* pArray)
      : CFX_FloatRect(pArray[0], pArray[1], pArray[2], pArray[3]) {}

  explicit CFX_FloatRect(const FX_RECT& rect);

  static CFX_FloatRect GetBBox(const CFX_PointF* pPoints, int nPoints);

  void Normalize();

  bool IsEmpty() const { return left >= right || bottom >= top; }
  bool Contains(const CFX_PointF& point) const;
  bool Contains(const CFX_FloatRect& other_rect) const;

  void Intersect(const CFX_FloatRect& other_rect);
  void Union(const CFX_FloatRect& other_rect);

  // These may be better at rounding than ToFxRect() and friends.
  //
  // Returned rect has bounds rounded up/down such that it is contained in the
  // original.
  FX_RECT GetInnerRect() const;

  // Returned rect has bounds rounded up/down such that the original is
  // contained in it.
  FX_RECT GetOuterRect() const;

  // Returned rect has bounds rounded up/down such that the dimensions are
  // rounded up and the sum of the error in the bounds is minimized.
  FX_RECT GetClosestRect() const;

  CFX_FloatRect GetCenterSquare() const;

  void InitRect(const CFX_PointF& point) {
    left = point.x;
    right = point.x;
    bottom = point.y;
    top = point.y;
  }
  void UpdateRect(const CFX_PointF& point);

  float Width() const { return right - left; }
  float Height() const { return top - bottom; }
  float Left() const { return left; }
  float Bottom() const { return bottom; }
  float Right() const { return right; }
  float Top() const { return top; }

  void Inflate(float x, float y);
  void Inflate(float other_left,
               float other_bottom,
               float other_right,
               float other_top);
  void Inflate(const CFX_FloatRect& rt);

  void Deflate(float x, float y);
  void Deflate(float other_left,
               float other_bottom,
               float other_right,
               float other_top);
  void Deflate(const CFX_FloatRect& rt);

  CFX_FloatRect GetDeflated(float x, float y) const;

  void Translate(float e, float f);

  void Scale(float fScale);
  void ScaleFromCenterPoint(float fScale);

  // GetInnerRect() and friends may be better at rounding than these methods.
  // Unlike the methods above, these two blindly floor / round the LBRT values.
  // Doing so may introduce rounding errors that are visible to users as
  // off-by-one pixels/lines.
  //
  // Floors LBRT values.
  FX_RECT ToFxRect() const;

  // Rounds LBRT values.
  FX_RECT ToRoundedFxRect() const;

  float left;
  float bottom;
  float right;
  float top;
};

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& os, const CFX_FloatRect& rect);
#endif

// LTWH rectangles (y-axis runs downwards).
class CFX_RectF {
 public:
  using PointType = CFX_PointF;
  using SizeType = CFX_SizeF;

  CFX_RectF() : left(0), top(0), width(0), height(0) {}
  CFX_RectF(float dst_left, float dst_top, float dst_width, float dst_height)
      : left(dst_left), top(dst_top), width(dst_width), height(dst_height) {}
  CFX_RectF(float dst_left, float dst_top, const SizeType& dst_size)
      : left(dst_left),
        top(dst_top),
        width(dst_size.width),
        height(dst_size.height) {}
  CFX_RectF(const PointType& p, float dst_width, float dst_height)
      : left(p.x), top(p.y), width(dst_width), height(dst_height) {}
  CFX_RectF(const PointType& p1, const SizeType& s2)
      : left(p1.x), top(p1.y), width(s2.width), height(s2.height) {}

  // NOLINTNEXTLINE(runtime/explicit)
  CFX_RectF(const CFX_RectF& other)
      : left(other.left),
        top(other.top),
        width(other.width),
        height(other.height) {}

  CFX_RectF& operator+=(const PointType& p) {
    left += p.x;
    top += p.y;
    return *this;
  }
  CFX_RectF& operator-=(const PointType& p) {
    left -= p.x;
    top -= p.y;
    return *this;
  }
  float right() const { return left + width; }
  float bottom() const { return top + height; }
  void Normalize() {
    if (width < 0) {
      left += width;
      width = -width;
    }
    if (height < 0) {
      top += height;
      height = -height;
    }
  }
  void Offset(float dx, float dy) {
    left += dx;
    top += dy;
  }
  void Inflate(float x, float y) {
    left -= x;
    width += x * 2;
    top -= y;
    height += y * 2;
  }
  void Inflate(const PointType& p) { Inflate(p.x, p.y); }
  void Inflate(float off_left,
               float off_top,
               float off_right,
               float off_bottom) {
    left -= off_left;
    top -= off_top;
    width += off_left + off_right;
    height += off_top + off_bottom;
  }
  void Inflate(const CFX_RectF& rt) {
    Inflate(rt.left, rt.top, rt.left + rt.width, rt.top + rt.height);
  }
  void Deflate(float x, float y) {
    left += x;
    width -= x * 2;
    top += y;
    height -= y * 2;
  }
  void Deflate(const PointType& p) { Deflate(p.x, p.y); }
  void Deflate(float off_left,
               float off_top,
               float off_right,
               float off_bottom) {
    left += off_left;
    top += off_top;
    width -= off_left + off_right;
    height -= off_top + off_bottom;
  }
  void Deflate(const CFX_RectF& rt) {
    Deflate(rt.left, rt.top, rt.top + rt.width, rt.top + rt.height);
  }
  bool IsEmpty() const { return width <= 0 || height <= 0; }
  bool IsEmpty(float fEpsilon) const {
    return width <= fEpsilon || height <= fEpsilon;
  }
  void Empty() { width = height = 0; }
  bool Contains(const PointType& p) const {
    return p.x >= left && p.x < left + width && p.y >= top &&
           p.y < top + height;
  }
  bool Contains(const CFX_RectF& rt) const {
    return rt.left >= left && rt.right() <= right() && rt.top >= top &&
           rt.bottom() <= bottom();
  }
  float Left() const { return left; }
  float Top() const { return top; }
  float Width() const { return width; }
  float Height() const { return height; }
  SizeType Size() const { return SizeType(width, height); }
  PointType TopLeft() const { return PointType(left, top); }
  PointType TopRight() const { return PointType(left + width, top); }
  PointType BottomLeft() const { return PointType(left, top + height); }
  PointType BottomRight() const {
    return PointType(left + width, top + height);
  }
  PointType Center() const {
    return PointType(left + width / 2, top + height / 2);
  }
  void Union(float x, float y) {
    float r = right();
    float b = bottom();

    left = std::min(left, x);
    top = std::min(top, y);
    r = std::max(r, x);
    b = std::max(b, y);

    width = r - left;
    height = b - top;
  }
  void Union(const PointType& p) { Union(p.x, p.y); }
  void Union(const CFX_RectF& rt) {
    float r = right();
    float b = bottom();

    left = std::min(left, rt.left);
    top = std::min(top, rt.top);
    r = std::max(r, rt.right());
    b = std::max(b, rt.bottom());

    width = r - left;
    height = b - top;
  }
  void Intersect(const CFX_RectF& rt) {
    float r = right();
    float b = bottom();

    left = std::max(left, rt.left);
    top = std::max(top, rt.top);
    r = std::min(r, rt.right());
    b = std::min(b, rt.bottom());

    width = r - left;
    height = b - top;
  }
  bool IntersectWith(const CFX_RectF& rt) const {
    CFX_RectF rect = rt;
    rect.Intersect(*this);
    return !rect.IsEmpty();
  }
  bool IntersectWith(const CFX_RectF& rt, float fEpsilon) const {
    CFX_RectF rect = rt;
    rect.Intersect(*this);
    return !rect.IsEmpty(fEpsilon);
  }
  friend bool operator==(const CFX_RectF& rc1, const CFX_RectF& rc2) {
    return rc1.left == rc2.left && rc1.top == rc2.top &&
           rc1.width == rc2.width && rc1.height == rc2.height;
  }
  friend bool operator!=(const CFX_RectF& rc1, const CFX_RectF& rc2) {
    return !(rc1 == rc2);
  }

  CFX_FloatRect ToFloatRect() const {
    // Note, we flip top/bottom here because the CFX_FloatRect has the
    // y-axis running in the opposite direction.
    return CFX_FloatRect(left, top, right(), bottom());
  }

  float left;
  float top;
  float width;
  float height;
};

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& os, const CFX_RectF& rect);
#endif  // NDEBUG

// The matrix is of the form:
// | a  b  0 |
// | c  d  0 |
// | e  f  1 |
// See PDF spec 1.7 Section 4.2.3.
//
class CFX_Matrix {
 public:
  CFX_Matrix() = default;

  explicit CFX_Matrix(const float n[6])
      : a(n[0]), b(n[1]), c(n[2]), d(n[3]), e(n[4]), f(n[5]) {}

  CFX_Matrix(float a1, float b1, float c1, float d1, float e1, float f1)
      : a(a1), b(b1), c(c1), d(d1), e(e1), f(f1) {}

  CFX_Matrix(const CFX_Matrix& other) = default;

  std::tuple<float, float, float, float, float, float> AsTuple() const;

  CFX_Matrix& operator=(const CFX_Matrix& other) = default;

  bool operator==(const CFX_Matrix& other) const {
    return a == other.a && b == other.b && c == other.c && d == other.d &&
           e == other.e && f == other.f;
  }
  bool operator!=(const CFX_Matrix& other) const { return !(*this == other); }

  CFX_Matrix operator*(const CFX_Matrix& right) const {
    return CFX_Matrix(a * right.a + b * right.c, a * right.b + b * right.d,
                      c * right.a + d * right.c, c * right.b + d * right.d,
                      e * right.a + f * right.c + right.e,
                      e * right.b + f * right.d + right.f);
  }
  CFX_Matrix& operator*=(const CFX_Matrix& other) {
    *this = *this * other;
    return *this;
  }

  bool IsIdentity() const { return *this == CFX_Matrix(); }
  CFX_Matrix GetInverse() const;

  bool Is90Rotated() const;
  bool IsScaled() const;
  bool WillScale() const { return a != 1.0f || b != 0 || c != 0 || d != 1.0f; }

  void Concat(const CFX_Matrix& right) { *this *= right; }
  void Translate(float x, float y);
  void TranslatePrepend(float x, float y);
  void Translate(int32_t x, int32_t y) {
    Translate(static_cast<float>(x), static_cast<float>(y));
  }
  void TranslatePrepend(int32_t x, int32_t y) {
    TranslatePrepend(static_cast<float>(x), static_cast<float>(y));
  }

  void Scale(float sx, float sy);
  void Rotate(float fRadian);
  void Shear(float fAlphaRadian, float fBetaRadian);

  void MatchRect(const CFX_FloatRect& dest, const CFX_FloatRect& src);

  float GetXUnit() const;
  float GetYUnit() const;
  CFX_FloatRect GetUnitRect() const;

  float TransformXDistance(float dx) const;
  float TransformDistance(float distance) const;

  CFX_PointF Transform(const CFX_PointF& point) const;

  CFX_RectF TransformRect(const CFX_RectF& rect) const;
  CFX_FloatRect TransformRect(const CFX_FloatRect& rect) const;

  float a = 1.0f;
  float b = 0.0f;
  float c = 0.0f;
  float d = 1.0f;
  float e = 0.0f;
  float f = 0.0f;
};

#endif  // CORE_FXCRT_FX_COORDINATES_H_
