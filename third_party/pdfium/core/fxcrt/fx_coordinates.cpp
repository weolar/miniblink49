// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/fx_coordinates.h"

#include <utility>

#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/fx_safe_types.h"

namespace {

void MatchFloatRange(float f1, float f2, int* i1, int* i2) {
  float length = ceilf(f2 - f1);
  float f1_floor = floorf(f1);
  float f1_ceil = ceilf(f1);
  float error1 = f1 - f1_floor + fabsf(f2 - f1_floor - length);
  float error2 = f1_ceil - f1 + fabsf(f2 - f1_ceil - length);
  float start = error1 > error2 ? f1_ceil : f1_floor;
  FX_SAFE_INT32 safe1 = start;
  FX_SAFE_INT32 safe2 = start + length;
  if (safe1.IsValid() && safe2.IsValid()) {
    *i1 = safe1.ValueOrDie();
    *i2 = safe2.ValueOrDie();
  } else {
    *i1 = 0;
    *i2 = 0;
  }
}

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
static_assert(sizeof(FX_RECT) == sizeof(RECT), "FX_RECT vs. RECT mismatch");
static_assert(offsetof(FX_RECT, left) == offsetof(RECT, left),
              "FX_RECT vs. RECT mismatch");
static_assert(offsetof(FX_RECT, top) == offsetof(RECT, top),
              "FX_RECT vs. RECT mismatch");
static_assert(offsetof(FX_RECT, right) == offsetof(RECT, right),
              "FX_RECT vs. RECT mismatch");
static_assert(offsetof(FX_RECT, bottom) == offsetof(RECT, bottom),
              "FX_RECT vs. RECT mismatch");
static_assert(sizeof(FX_RECT::left) == sizeof(RECT::left),
              "FX_RECT vs. RECT mismatch");
static_assert(sizeof(FX_RECT::top) == sizeof(RECT::top),
              "FX_RECT vs. RECT mismatch");
static_assert(sizeof(FX_RECT::right) == sizeof(RECT::right),
              "FX_RECT vs. RECT mismatch");
static_assert(sizeof(FX_RECT::bottom) == sizeof(RECT::bottom),
              "FX_RECT vs. RECT mismatch");
#endif

}  // namespace

void FX_RECT::Normalize() {
  if (left > right)
    std::swap(left, right);
  if (top > bottom)
    std::swap(top, bottom);
}

void FX_RECT::Intersect(const FX_RECT& src) {
  FX_RECT src_n = src;
  src_n.Normalize();
  Normalize();
  left = std::max(left, src_n.left);
  top = std::max(top, src_n.top);
  right = std::min(right, src_n.right);
  bottom = std::min(bottom, src_n.bottom);
  if (left > right || top > bottom) {
    left = top = right = bottom = 0;
  }
}

CFX_FloatRect::CFX_FloatRect(const FX_RECT& rect) {
  left = rect.left;
  top = rect.bottom;
  right = rect.right;
  bottom = rect.top;
}

// static
CFX_FloatRect CFX_FloatRect::GetBBox(const CFX_PointF* pPoints, int nPoints) {
  if (nPoints == 0)
    return CFX_FloatRect();

  float min_x = pPoints->x;
  float max_x = pPoints->x;
  float min_y = pPoints->y;
  float max_y = pPoints->y;
  for (int i = 1; i < nPoints; i++) {
    min_x = std::min(min_x, pPoints[i].x);
    max_x = std::max(max_x, pPoints[i].x);
    min_y = std::min(min_y, pPoints[i].y);
    max_y = std::max(max_y, pPoints[i].y);
  }
  return CFX_FloatRect(min_x, min_y, max_x, max_y);
}

void CFX_FloatRect::Normalize() {
  if (left > right)
    std::swap(left, right);
  if (bottom > top)
    std::swap(top, bottom);
}

void CFX_FloatRect::Intersect(const CFX_FloatRect& other_rect) {
  Normalize();
  CFX_FloatRect other = other_rect;
  other.Normalize();
  left = std::max(left, other.left);
  bottom = std::max(bottom, other.bottom);
  right = std::min(right, other.right);
  top = std::min(top, other.top);
  if (left > right || bottom > top)
    *this = CFX_FloatRect();
}

void CFX_FloatRect::Union(const CFX_FloatRect& other_rect) {
  Normalize();
  CFX_FloatRect other = other_rect;
  other.Normalize();
  left = std::min(left, other.left);
  bottom = std::min(bottom, other.bottom);
  right = std::max(right, other.right);
  top = std::max(top, other.top);
}

FX_RECT CFX_FloatRect::GetOuterRect() const {
  FX_RECT rect;
  rect.left = static_cast<int>(floor(left));
  rect.bottom = static_cast<int>(ceil(top));
  rect.right = static_cast<int>(ceil(right));
  rect.top = static_cast<int>(floor(bottom));
  rect.Normalize();
  return rect;
}

FX_RECT CFX_FloatRect::GetInnerRect() const {
  FX_RECT rect;
  rect.left = static_cast<int>(ceil(left));
  rect.bottom = static_cast<int>(floor(top));
  rect.right = static_cast<int>(floor(right));
  rect.top = static_cast<int>(ceil(bottom));
  rect.Normalize();
  return rect;
}

FX_RECT CFX_FloatRect::GetClosestRect() const {
  FX_RECT rect;
  MatchFloatRange(left, right, &rect.left, &rect.right);
  MatchFloatRange(bottom, top, &rect.top, &rect.bottom);
  rect.Normalize();
  return rect;
}

CFX_FloatRect CFX_FloatRect::GetCenterSquare() const {
  float fWidth = Width();
  float fHeight = Height();
  float fHalfWidth = (fWidth > fHeight) ? fHeight / 2 : fWidth / 2;

  float fCenterX = (left + right) / 2.0f;
  float fCenterY = (top + bottom) / 2.0f;
  return CFX_FloatRect(fCenterX - fHalfWidth, fCenterY - fHalfWidth,
                       fCenterX + fHalfWidth, fCenterY + fHalfWidth);
}

bool CFX_FloatRect::Contains(const CFX_PointF& point) const {
  CFX_FloatRect n1(*this);
  n1.Normalize();
  return point.x <= n1.right && point.x >= n1.left && point.y <= n1.top &&
         point.y >= n1.bottom;
}

bool CFX_FloatRect::Contains(const CFX_FloatRect& other_rect) const {
  CFX_FloatRect n1(*this);
  CFX_FloatRect n2(other_rect);
  n1.Normalize();
  n2.Normalize();
  return n2.left >= n1.left && n2.right <= n1.right && n2.bottom >= n1.bottom &&
         n2.top <= n1.top;
}

void CFX_FloatRect::UpdateRect(const CFX_PointF& point) {
  left = std::min(left, point.x);
  bottom = std::min(bottom, point.y);
  right = std::max(right, point.x);
  top = std::max(top, point.y);
}

void CFX_FloatRect::Inflate(float x, float y) {
  Inflate(x, y, x, y);
}

void CFX_FloatRect::Inflate(float other_left,
                            float other_bottom,
                            float other_right,
                            float other_top) {
  Normalize();
  left -= other_left;
  bottom -= other_bottom;
  right += other_right;
  top += other_top;
}

void CFX_FloatRect::Inflate(const CFX_FloatRect& rt) {
  Inflate(rt.left, rt.bottom, rt.right, rt.top);
}

void CFX_FloatRect::Deflate(float x, float y) {
  Deflate(x, y, x, y);
}

void CFX_FloatRect::Deflate(float other_left,
                            float other_bottom,
                            float other_right,
                            float other_top) {
  Inflate(-other_left, -other_bottom, -other_right, -other_top);
}

void CFX_FloatRect::Deflate(const CFX_FloatRect& rt) {
  Deflate(rt.left, rt.bottom, rt.right, rt.top);
}

CFX_FloatRect CFX_FloatRect::GetDeflated(float x, float y) const {
  if (IsEmpty())
    return CFX_FloatRect();

  CFX_FloatRect that = *this;
  that.Deflate(x, y);
  that.Normalize();
  return that;
}

void CFX_FloatRect::Translate(float e, float f) {
  left += e;
  right += e;
  top += f;
  bottom += f;
}

void CFX_FloatRect::Scale(float fScale) {
  left *= fScale;
  bottom *= fScale;
  right *= fScale;
  top *= fScale;
}

void CFX_FloatRect::ScaleFromCenterPoint(float fScale) {
  float fHalfWidth = (right - left) / 2.0f;
  float fHalfHeight = (top - bottom) / 2.0f;

  float center_x = (left + right) / 2;
  float center_y = (top + bottom) / 2;

  left = center_x - fHalfWidth * fScale;
  bottom = center_y - fHalfHeight * fScale;
  right = center_x + fHalfWidth * fScale;
  top = center_y + fHalfHeight * fScale;
}

FX_RECT CFX_FloatRect::ToFxRect() const {
  return FX_RECT(static_cast<int>(left), static_cast<int>(top),
                 static_cast<int>(right), static_cast<int>(bottom));
}

FX_RECT CFX_FloatRect::ToRoundedFxRect() const {
  return FX_RECT(FXSYS_round(left), FXSYS_round(top), FXSYS_round(right),
                 FXSYS_round(bottom));
}

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& os, const CFX_FloatRect& rect) {
  os << "rect[w " << rect.Width() << " x h " << rect.Height() << " (left "
     << rect.left << ", bot " << rect.bottom << ")]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const CFX_RectF& rect) {
  os << "rect[w " << rect.Width() << " x h " << rect.Height() << " (left "
     << rect.left << ", top " << rect.top << ")]";
  return os;
}
#endif  // NDEBUG

std::tuple<float, float, float, float, float, float> CFX_Matrix::AsTuple()
    const {
  return std::make_tuple(a, b, c, d, e, f);
}

CFX_Matrix CFX_Matrix::GetInverse() const {
  CFX_Matrix inverse;
  float i = a * d - b * c;
  if (fabs(i) == 0)
    return inverse;

  float j = -i;
  inverse.a = d / i;
  inverse.b = b / j;
  inverse.c = c / j;
  inverse.d = a / i;
  inverse.e = (c * f - d * e) / i;
  inverse.f = (a * f - b * e) / j;
  return inverse;
}

bool CFX_Matrix::Is90Rotated() const {
  return fabs(a * 1000) < fabs(b) && fabs(d * 1000) < fabs(c);
}

bool CFX_Matrix::IsScaled() const {
  return fabs(b * 1000) < fabs(a) && fabs(c * 1000) < fabs(d);
}

void CFX_Matrix::Translate(float x, float y) {
  e += x;
  f += y;
}

void CFX_Matrix::TranslatePrepend(float x, float y) {
  e += x * a + y * c;
  f += y * d + x * b;
}

void CFX_Matrix::Scale(float sx, float sy) {
  a *= sx;
  b *= sy;
  c *= sx;
  d *= sy;
  e *= sx;
  f *= sy;
}

void CFX_Matrix::Rotate(float fRadian) {
  float cosValue = cos(fRadian);
  float sinValue = sin(fRadian);
  Concat(CFX_Matrix(cosValue, sinValue, -sinValue, cosValue, 0, 0));
}

void CFX_Matrix::Shear(float fAlphaRadian, float fBetaRadian) {
  Concat(CFX_Matrix(1, tan(fAlphaRadian), tan(fBetaRadian), 1, 0, 0));
}

void CFX_Matrix::MatchRect(const CFX_FloatRect& dest,
                           const CFX_FloatRect& src) {
  float fDiff = src.left - src.right;
  a = fabs(fDiff) < 0.001f ? 1 : (dest.left - dest.right) / fDiff;

  fDiff = src.bottom - src.top;
  d = fabs(fDiff) < 0.001f ? 1 : (dest.bottom - dest.top) / fDiff;
  e = dest.left - src.left * a;
  f = dest.bottom - src.bottom * d;
  b = 0;
  c = 0;
}

float CFX_Matrix::GetXUnit() const {
  if (b == 0)
    return (a > 0 ? a : -a);
  if (a == 0)
    return (b > 0 ? b : -b);
  return sqrt(a * a + b * b);
}

float CFX_Matrix::GetYUnit() const {
  if (c == 0)
    return (d > 0 ? d : -d);
  if (d == 0)
    return (c > 0 ? c : -c);
  return sqrt(c * c + d * d);
}

CFX_FloatRect CFX_Matrix::GetUnitRect() const {
  return TransformRect(CFX_FloatRect(0.f, 0.f, 1.f, 1.f));
}

float CFX_Matrix::TransformXDistance(float dx) const {
  float fx = a * dx;
  float fy = b * dx;
  return sqrt(fx * fx + fy * fy);
}

float CFX_Matrix::TransformDistance(float distance) const {
  return distance * (GetXUnit() + GetYUnit()) / 2;
}

CFX_PointF CFX_Matrix::Transform(const CFX_PointF& point) const {
  return CFX_PointF(a * point.x + c * point.y + e,
                    b * point.x + d * point.y + f);
}

CFX_RectF CFX_Matrix::TransformRect(const CFX_RectF& rect) const {
  CFX_FloatRect result_rect = TransformRect(rect.ToFloatRect());
  return CFX_RectF(result_rect.left, result_rect.bottom, result_rect.Width(),
                   result_rect.Height());
}

CFX_FloatRect CFX_Matrix::TransformRect(const CFX_FloatRect& rect) const {
  CFX_PointF points[] = {{rect.left, rect.top},
                         {rect.left, rect.bottom},
                         {rect.right, rect.top},
                         {rect.right, rect.bottom}};
  for (CFX_PointF& point : points)
    point = Transform(point);

  float new_right = points[0].x;
  float new_left = points[0].x;
  float new_top = points[0].y;
  float new_bottom = points[0].y;
  for (size_t i = 1; i < FX_ArraySize(points); i++) {
    new_right = std::max(new_right, points[i].x);
    new_left = std::min(new_left, points[i].x);
    new_top = std::max(new_top, points[i].y);
    new_bottom = std::min(new_bottom, points[i].y);
  }

  return CFX_FloatRect(new_left, new_bottom, new_right, new_top);
}
