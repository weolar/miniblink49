// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_renderstatus.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "constants/transparency.h"
#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/font/cpdf_type3char.h"
#include "core/fpdfapi/font/cpdf_type3font.h"
#include "core/fpdfapi/page/cpdf_docpagedata.h"
#include "core/fpdfapi/page/cpdf_form.h"
#include "core/fpdfapi/page/cpdf_formobject.h"
#include "core/fpdfapi/page/cpdf_function.h"
#include "core/fpdfapi/page/cpdf_graphicstates.h"
#include "core/fpdfapi/page/cpdf_image.h"
#include "core/fpdfapi/page/cpdf_imageobject.h"
#include "core/fpdfapi/page/cpdf_meshstream.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/page/cpdf_pathobject.h"
#include "core/fpdfapi/page/cpdf_shadingobject.h"
#include "core/fpdfapi/page/cpdf_shadingpattern.h"
#include "core/fpdfapi/page/cpdf_textobject.h"
#include "core/fpdfapi/page/cpdf_tilingpattern.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/render/cpdf_charposlist.h"
#include "core/fpdfapi/render/cpdf_devicebuffer.h"
#include "core/fpdfapi/render/cpdf_dibbase.h"
#include "core/fpdfapi/render/cpdf_docrenderdata.h"
#include "core/fpdfapi/render/cpdf_imagerenderer.h"
#include "core/fpdfapi/render/cpdf_pagerendercache.h"
#include "core/fpdfapi/render/cpdf_rendercontext.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fpdfapi/render/cpdf_scaledrenderbuffer.h"
#include "core/fpdfapi/render/cpdf_textrenderer.h"
#include "core/fpdfapi/render/cpdf_transferfunc.h"
#include "core/fpdfapi/render/cpdf_type3cache.h"
#include "core/fpdfdoc/cpdf_occontext.h"
#include "core/fxcrt/autorestorer.h"
#include "core/fxcrt/fx_safe_types.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/maybe_owned.h"
#include "core/fxge/cfx_defaultrenderdevice.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_renderdevice.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/renderdevicedriver_iface.h"
#include "third_party/base/compiler_specific.h"
#include "third_party/base/logging.h"
#include "third_party/base/numerics/safe_math.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

#ifdef _SKIA_SUPPORT_
#include "core/fxge/skia/fx_skia_device.h"
#endif

namespace {

constexpr int kShadingSteps = 256;
constexpr int kRenderMaxRecursionDepth = 64;
int g_CurrentRecursionDepth = 0;

void ReleaseCachedType3(CPDF_Type3Font* pFont) {
  CPDF_Document* pDoc = pFont->GetDocument();
  if (!pDoc)
    return;

  pDoc->GetRenderData()->MaybePurgeCachedType3(pFont);
  pDoc->GetPageData()->ReleaseFont(pFont->GetFontDict());
}

class CPDF_RefType3Cache {
 public:
  explicit CPDF_RefType3Cache(CPDF_Type3Font* pType3Font)
      : m_dwCount(0), m_pType3Font(pType3Font) {}

  ~CPDF_RefType3Cache() {
    while (m_dwCount--)
      ReleaseCachedType3(m_pType3Font.Get());
  }

  uint32_t m_dwCount;
  UnownedPtr<CPDF_Type3Font> const m_pType3Font;
};

uint32_t CountOutputsFromFunctions(
    const std::vector<std::unique_ptr<CPDF_Function>>& funcs) {
  FX_SAFE_UINT32 total = 0;
  for (const auto& func : funcs) {
    if (func)
      total += func->CountOutputs();
  }
  return total.ValueOrDefault(0);
}

uint32_t GetValidatedOutputsCount(
    const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
    const CPDF_ColorSpace* pCS) {
  uint32_t funcs_outputs = CountOutputsFromFunctions(funcs);
  return funcs_outputs ? std::max(funcs_outputs, pCS->CountComponents()) : 0;
}

void GetShadingSteps(float t_min,
                     float t_max,
                     const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
                     const CPDF_ColorSpace* pCS,
                     int alpha,
                     size_t results_count,
                     uint32_t* rgb_array) {
  ASSERT(results_count >= CountOutputsFromFunctions(funcs));
  ASSERT(results_count >= pCS->CountComponents());
  std::vector<float> result_array(results_count);
  float diff = t_max - t_min;
  for (int i = 0; i < kShadingSteps; ++i) {
    float input = diff * i / kShadingSteps + t_min;
    int offset = 0;
    for (const auto& func : funcs) {
      if (func) {
        int nresults = 0;
        if (func->Call(&input, 1, &result_array[offset], &nresults))
          offset += nresults;
      }
    }
    float R = 0.0f;
    float G = 0.0f;
    float B = 0.0f;
    pCS->GetRGB(result_array.data(), &R, &G, &B);
    rgb_array[i] =
        FXARGB_TODIB(ArgbEncode(alpha, FXSYS_round(R * 255),
                                FXSYS_round(G * 255), FXSYS_round(B * 255)));
  }
}

void DrawAxialShading(const RetainPtr<CFX_DIBitmap>& pBitmap,
                      const CFX_Matrix& mtObject2Bitmap,
                      const CPDF_Dictionary* pDict,
                      const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
                      const CPDF_ColorSpace* pCS,
                      int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);

  const uint32_t total_results = GetValidatedOutputsCount(funcs, pCS);
  if (total_results == 0)
    return;

  const CPDF_Array* pCoords = pDict->GetArrayFor("Coords");
  if (!pCoords)
    return;

  float start_x = pCoords->GetNumberAt(0);
  float start_y = pCoords->GetNumberAt(1);
  float end_x = pCoords->GetNumberAt(2);
  float end_y = pCoords->GetNumberAt(3);
  float t_min = 0;
  float t_max = 1.0f;
  const CPDF_Array* pArray = pDict->GetArrayFor("Domain");
  if (pArray) {
    t_min = pArray->GetNumberAt(0);
    t_max = pArray->GetNumberAt(1);
  }
  bool bStartExtend = false;
  bool bEndExtend = false;
  pArray = pDict->GetArrayFor("Extend");
  if (pArray) {
    bStartExtend = !!pArray->GetIntegerAt(0);
    bEndExtend = !!pArray->GetIntegerAt(1);
  }
  int width = pBitmap->GetWidth();
  int height = pBitmap->GetHeight();
  float x_span = end_x - start_x;
  float y_span = end_y - start_y;
  float axis_len_square = (x_span * x_span) + (y_span * y_span);

  uint32_t rgb_array[kShadingSteps];
  GetShadingSteps(t_min, t_max, funcs, pCS, alpha, total_results, rgb_array);

  int pitch = pBitmap->GetPitch();
  CFX_Matrix matrix = mtObject2Bitmap.GetInverse();
  for (int row = 0; row < height; row++) {
    uint32_t* dib_buf =
        reinterpret_cast<uint32_t*>(pBitmap->GetBuffer() + row * pitch);
    for (int column = 0; column < width; column++) {
      CFX_PointF pos = matrix.Transform(
          CFX_PointF(static_cast<float>(column), static_cast<float>(row)));
      float scale =
          (((pos.x - start_x) * x_span) + ((pos.y - start_y) * y_span)) /
          axis_len_square;
      int index = (int32_t)(scale * (kShadingSteps - 1));
      if (index < 0) {
        if (!bStartExtend)
          continue;

        index = 0;
      } else if (index >= kShadingSteps) {
        if (!bEndExtend)
          continue;

        index = kShadingSteps - 1;
      }
      dib_buf[column] = rgb_array[index];
    }
  }
}

void DrawRadialShading(const RetainPtr<CFX_DIBitmap>& pBitmap,
                       const CFX_Matrix& mtObject2Bitmap,
                       const CPDF_Dictionary* pDict,
                       const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
                       const CPDF_ColorSpace* pCS,
                       int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);

  const uint32_t total_results = GetValidatedOutputsCount(funcs, pCS);
  if (total_results == 0)
    return;

  const CPDF_Array* pCoords = pDict->GetArrayFor("Coords");
  if (!pCoords)
    return;

  float start_x = pCoords->GetNumberAt(0);
  float start_y = pCoords->GetNumberAt(1);
  float start_r = pCoords->GetNumberAt(2);
  float end_x = pCoords->GetNumberAt(3);
  float end_y = pCoords->GetNumberAt(4);
  float end_r = pCoords->GetNumberAt(5);
  float t_min = 0;
  float t_max = 1.0f;
  const CPDF_Array* pArray = pDict->GetArrayFor("Domain");
  if (pArray) {
    t_min = pArray->GetNumberAt(0);
    t_max = pArray->GetNumberAt(1);
  }
  bool bStartExtend = false;
  bool bEndExtend = false;
  pArray = pDict->GetArrayFor("Extend");
  if (pArray) {
    bStartExtend = !!pArray->GetIntegerAt(0);
    bEndExtend = !!pArray->GetIntegerAt(1);
  }

  uint32_t rgb_array[kShadingSteps];
  GetShadingSteps(t_min, t_max, funcs, pCS, alpha, total_results, rgb_array);

  const float dx = end_x - start_x;
  const float dy = end_y - start_y;
  const float dr = end_r - start_r;
  const float a = dx * dx + dy * dy - dr * dr;
  const bool a_is_float_zero = IsFloatZero(a);

  int width = pBitmap->GetWidth();
  int height = pBitmap->GetHeight();
  int pitch = pBitmap->GetPitch();

  bool bDecreasing =
      (dr < 0 && static_cast<int>(sqrt(dx * dx + dy * dy)) < -dr);

  CFX_Matrix matrix = mtObject2Bitmap.GetInverse();
  for (int row = 0; row < height; row++) {
    uint32_t* dib_buf =
        reinterpret_cast<uint32_t*>(pBitmap->GetBuffer() + row * pitch);
    for (int column = 0; column < width; column++) {
      CFX_PointF pos = matrix.Transform(
          CFX_PointF(static_cast<float>(column), static_cast<float>(row)));
      float pos_dx = pos.x - start_x;
      float pos_dy = pos.y - start_y;
      float b = -2 * (pos_dx * dx + pos_dy * dy + start_r * dr);
      float c = pos_dx * pos_dx + pos_dy * pos_dy - start_r * start_r;
      float s;
      if (IsFloatZero(b)) {
        s = sqrt(-c / a);
      } else if (a_is_float_zero) {
        s = -c / b;
      } else {
        float b2_4ac = (b * b) - 4 * (a * c);
        if (b2_4ac < 0)
          continue;

        float root = sqrt(b2_4ac);
        float s1 = (-b - root) / (2 * a);
        float s2 = (-b + root) / (2 * a);
        if (a <= 0)
          std::swap(s1, s2);
        if (bDecreasing)
          s = (s1 >= 0 || bStartExtend) ? s1 : s2;
        else
          s = (s2 <= 1.0f || bEndExtend) ? s2 : s1;

        if (start_r + s * dr < 0)
          continue;
      }

      int index = static_cast<int32_t>(s * (kShadingSteps - 1));
      if (index < 0) {
        if (!bStartExtend)
          continue;
        index = 0;
      } else if (index >= kShadingSteps) {
        if (!bEndExtend)
          continue;
        index = kShadingSteps - 1;
      }
      dib_buf[column] = rgb_array[index];
    }
  }
}

void DrawFuncShading(const RetainPtr<CFX_DIBitmap>& pBitmap,
                     const CFX_Matrix& mtObject2Bitmap,
                     const CPDF_Dictionary* pDict,
                     const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
                     const CPDF_ColorSpace* pCS,
                     int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);

  const uint32_t total_results = GetValidatedOutputsCount(funcs, pCS);
  if (total_results == 0)
    return;

  const CPDF_Array* pDomain = pDict->GetArrayFor("Domain");
  float xmin = 0.0f;
  float ymin = 0.0f;
  float xmax = 1.0f;
  float ymax = 1.0f;
  if (pDomain) {
    xmin = pDomain->GetNumberAt(0);
    xmax = pDomain->GetNumberAt(1);
    ymin = pDomain->GetNumberAt(2);
    ymax = pDomain->GetNumberAt(3);
  }
  CFX_Matrix mtDomain2Target = pDict->GetMatrixFor("Matrix");
  CFX_Matrix matrix =
      mtObject2Bitmap.GetInverse() * mtDomain2Target.GetInverse();
  int width = pBitmap->GetWidth();
  int height = pBitmap->GetHeight();
  int pitch = pBitmap->GetPitch();

  ASSERT(total_results >= CountOutputsFromFunctions(funcs));
  ASSERT(total_results >= pCS->CountComponents());
  std::vector<float> result_array(total_results);
  for (int row = 0; row < height; ++row) {
    uint32_t* dib_buf = (uint32_t*)(pBitmap->GetBuffer() + row * pitch);
    for (int column = 0; column < width; column++) {
      CFX_PointF pos = matrix.Transform(
          CFX_PointF(static_cast<float>(column), static_cast<float>(row)));
      if (pos.x < xmin || pos.x > xmax || pos.y < ymin || pos.y > ymax)
        continue;

      float input[] = {pos.x, pos.y};
      int offset = 0;
      for (const auto& func : funcs) {
        if (func) {
          int nresults;
          if (func->Call(input, 2, &result_array[offset], &nresults))
            offset += nresults;
        }
      }

      float R = 0.0f;
      float G = 0.0f;
      float B = 0.0f;
      pCS->GetRGB(result_array.data(), &R, &G, &B);
      dib_buf[column] = FXARGB_TODIB(ArgbEncode(
          alpha, (int32_t)(R * 255), (int32_t)(G * 255), (int32_t)(B * 255)));
    }
  }
}

bool GetScanlineIntersect(int y,
                          const CFX_PointF& first,
                          const CFX_PointF& second,
                          float* x) {
  if (first.y == second.y)
    return false;

  if (first.y < second.y) {
    if (y < first.y || y > second.y)
      return false;
  } else if (y < second.y || y > first.y) {
    return false;
  }
  *x = first.x + ((second.x - first.x) * (y - first.y) / (second.y - first.y));
  return true;
}

void DrawGouraud(const RetainPtr<CFX_DIBitmap>& pBitmap,
                 int alpha,
                 CPDF_MeshVertex triangle[3]) {
  float min_y = triangle[0].position.y;
  float max_y = triangle[0].position.y;
  for (int i = 1; i < 3; i++) {
    min_y = std::min(min_y, triangle[i].position.y);
    max_y = std::max(max_y, triangle[i].position.y);
  }
  if (min_y == max_y)
    return;

  int min_yi = std::max(static_cast<int>(floor(min_y)), 0);
  int max_yi = static_cast<int>(ceil(max_y));

  if (max_yi >= pBitmap->GetHeight())
    max_yi = pBitmap->GetHeight() - 1;

  for (int y = min_yi; y <= max_yi; y++) {
    int nIntersects = 0;
    float inter_x[3];
    float r[3];
    float g[3];
    float b[3];
    for (int i = 0; i < 3; i++) {
      CPDF_MeshVertex& vertex1 = triangle[i];
      CPDF_MeshVertex& vertex2 = triangle[(i + 1) % 3];
      CFX_PointF& position1 = vertex1.position;
      CFX_PointF& position2 = vertex2.position;
      bool bIntersect =
          GetScanlineIntersect(y, position1, position2, &inter_x[nIntersects]);
      if (!bIntersect)
        continue;

      float y_dist = (y - position1.y) / (position2.y - position1.y);
      r[nIntersects] = vertex1.r + ((vertex2.r - vertex1.r) * y_dist);
      g[nIntersects] = vertex1.g + ((vertex2.g - vertex1.g) * y_dist);
      b[nIntersects] = vertex1.b + ((vertex2.b - vertex1.b) * y_dist);
      nIntersects++;
    }
    if (nIntersects != 2)
      continue;

    int min_x, max_x, start_index, end_index;
    if (inter_x[0] < inter_x[1]) {
      min_x = (int)floor(inter_x[0]);
      max_x = (int)ceil(inter_x[1]);
      start_index = 0;
      end_index = 1;
    } else {
      min_x = (int)floor(inter_x[1]);
      max_x = (int)ceil(inter_x[0]);
      start_index = 1;
      end_index = 0;
    }

    int start_x = std::max(min_x, 0);
    int end_x = max_x;
    if (end_x > pBitmap->GetWidth())
      end_x = pBitmap->GetWidth();

    uint8_t* dib_buf =
        pBitmap->GetBuffer() + y * pBitmap->GetPitch() + start_x * 4;
    float r_unit = (r[end_index] - r[start_index]) / (max_x - min_x);
    float g_unit = (g[end_index] - g[start_index]) / (max_x - min_x);
    float b_unit = (b[end_index] - b[start_index]) / (max_x - min_x);
    float R = r[start_index] + (start_x - min_x) * r_unit;
    float G = g[start_index] + (start_x - min_x) * g_unit;
    float B = b[start_index] + (start_x - min_x) * b_unit;
    for (int x = start_x; x < end_x; x++) {
      R += r_unit;
      G += g_unit;
      B += b_unit;
      FXARGB_SETDIB(dib_buf,
                    ArgbEncode(alpha, (int32_t)(R * 255), (int32_t)(G * 255),
                               (int32_t)(B * 255)));
      dib_buf += 4;
    }
  }
}

void DrawFreeGouraudShading(
    const RetainPtr<CFX_DIBitmap>& pBitmap,
    const CFX_Matrix& mtObject2Bitmap,
    const CPDF_Stream* pShadingStream,
    const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
    const CPDF_ColorSpace* pCS,
    int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);

  CPDF_MeshStream stream(kFreeFormGouraudTriangleMeshShading, funcs,
                         pShadingStream, pCS);
  if (!stream.Load())
    return;

  CPDF_MeshVertex triangle[3];
  memset(triangle, 0, sizeof(triangle));

  while (!stream.BitStream()->IsEOF()) {
    CPDF_MeshVertex vertex;
    uint32_t flag;
    if (!stream.ReadVertex(mtObject2Bitmap, &vertex, &flag))
      return;

    if (flag == 0) {
      triangle[0] = vertex;
      for (int j = 1; j < 3; j++) {
        uint32_t tflag;
        if (!stream.ReadVertex(mtObject2Bitmap, &triangle[j], &tflag))
          return;
      }
    } else {
      if (flag == 1)
        triangle[0] = triangle[1];

      triangle[1] = triangle[2];
      triangle[2] = vertex;
    }
    DrawGouraud(pBitmap, alpha, triangle);
  }
}

void DrawLatticeGouraudShading(
    const RetainPtr<CFX_DIBitmap>& pBitmap,
    const CFX_Matrix& mtObject2Bitmap,
    const CPDF_Stream* pShadingStream,
    const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
    const CPDF_ColorSpace* pCS,
    int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);

  int row_verts = pShadingStream->GetDict()->GetIntegerFor("VerticesPerRow");
  if (row_verts < 2)
    return;

  CPDF_MeshStream stream(kLatticeFormGouraudTriangleMeshShading, funcs,
                         pShadingStream, pCS);
  if (!stream.Load())
    return;

  std::vector<CPDF_MeshVertex> vertices[2];
  vertices[0] = stream.ReadVertexRow(mtObject2Bitmap, row_verts);
  if (vertices[0].empty())
    return;

  int last_index = 0;
  while (1) {
    vertices[1 - last_index] = stream.ReadVertexRow(mtObject2Bitmap, row_verts);
    if (vertices[1 - last_index].empty())
      return;

    CPDF_MeshVertex triangle[3];
    for (int i = 1; i < row_verts; ++i) {
      triangle[0] = vertices[last_index][i];
      triangle[1] = vertices[1 - last_index][i - 1];
      triangle[2] = vertices[last_index][i - 1];
      DrawGouraud(pBitmap, alpha, triangle);
      triangle[2] = vertices[1 - last_index][i];
      DrawGouraud(pBitmap, alpha, triangle);
    }
    last_index = 1 - last_index;
  }
}

struct Coon_BezierCoeff {
  float a, b, c, d;
  void FromPoints(float p0, float p1, float p2, float p3) {
    a = -p0 + 3 * p1 - 3 * p2 + p3;
    b = 3 * p0 - 6 * p1 + 3 * p2;
    c = -3 * p0 + 3 * p1;
    d = p0;
  }
  Coon_BezierCoeff first_half() {
    Coon_BezierCoeff result;
    result.a = a / 8;
    result.b = b / 4;
    result.c = c / 2;
    result.d = d;
    return result;
  }
  Coon_BezierCoeff second_half() {
    Coon_BezierCoeff result;
    result.a = a / 8;
    result.b = 3 * a / 8 + b / 4;
    result.c = 3 * a / 8 + b / 2 + c / 2;
    result.d = a / 8 + b / 4 + c / 2 + d;
    return result;
  }
  void GetPoints(float p[4]) {
    p[0] = d;
    p[1] = c / 3 + p[0];
    p[2] = b / 3 - p[0] + 2 * p[1];
    p[3] = a + p[0] - 3 * p[1] + 3 * p[2];
  }
  void GetPointsReverse(float p[4]) {
    p[3] = d;
    p[2] = c / 3 + p[3];
    p[1] = b / 3 - p[3] + 2 * p[2];
    p[0] = a + p[3] - 3 * p[2] + 3 * p[1];
  }
  void BezierInterpol(Coon_BezierCoeff& C1,
                      Coon_BezierCoeff& C2,
                      Coon_BezierCoeff& D1,
                      Coon_BezierCoeff& D2) {
    a = (D1.a + D2.a) / 2;
    b = (D1.b + D2.b) / 2;
    c = (D1.c + D2.c) / 2 - (C1.a / 8 + C1.b / 4 + C1.c / 2) +
        (C2.a / 8 + C2.b / 4) + (-C1.d + D2.d) / 2 - (C2.a + C2.b) / 2;
    d = C1.a / 8 + C1.b / 4 + C1.c / 2 + C1.d;
  }
  float Distance() {
    float dis = a + b + c;
    return dis < 0 ? -dis : dis;
  }
};

struct Coon_Bezier {
  Coon_BezierCoeff x, y;
  void FromPoints(float x0,
                  float y0,
                  float x1,
                  float y1,
                  float x2,
                  float y2,
                  float x3,
                  float y3) {
    x.FromPoints(x0, x1, x2, x3);
    y.FromPoints(y0, y1, y2, y3);
  }

  Coon_Bezier first_half() {
    Coon_Bezier result;
    result.x = x.first_half();
    result.y = y.first_half();
    return result;
  }

  Coon_Bezier second_half() {
    Coon_Bezier result;
    result.x = x.second_half();
    result.y = y.second_half();
    return result;
  }

  void BezierInterpol(Coon_Bezier& C1,
                      Coon_Bezier& C2,
                      Coon_Bezier& D1,
                      Coon_Bezier& D2) {
    x.BezierInterpol(C1.x, C2.x, D1.x, D2.x);
    y.BezierInterpol(C1.y, C2.y, D1.y, D2.y);
  }

  void GetPoints(std::vector<FX_PATHPOINT>& pPoints, size_t start_idx) {
    float p[4];
    int i;
    x.GetPoints(p);
    for (i = 0; i < 4; i++)
      pPoints[start_idx + i].m_Point.x = p[i];

    y.GetPoints(p);
    for (i = 0; i < 4; i++)
      pPoints[start_idx + i].m_Point.y = p[i];
  }

  void GetPointsReverse(std::vector<FX_PATHPOINT>& pPoints, size_t start_idx) {
    float p[4];
    int i;
    x.GetPointsReverse(p);
    for (i = 0; i < 4; i++)
      pPoints[i + start_idx].m_Point.x = p[i];

    y.GetPointsReverse(p);
    for (i = 0; i < 4; i++)
      pPoints[i + start_idx].m_Point.y = p[i];
  }

  float Distance() { return x.Distance() + y.Distance(); }
};

int Interpolate(int p1, int p2, int delta1, int delta2, bool* overflow) {
  pdfium::base::CheckedNumeric<int> p = p2;
  p -= p1;
  p *= delta1;
  p /= delta2;
  p += p1;
  if (!p.IsValid())
    *overflow = true;
  return p.ValueOrDefault(0);
}

int BiInterpolImpl(int c0,
                   int c1,
                   int c2,
                   int c3,
                   int x,
                   int y,
                   int x_scale,
                   int y_scale,
                   bool* overflow) {
  int x1 = Interpolate(c0, c3, x, x_scale, overflow);
  int x2 = Interpolate(c1, c2, x, x_scale, overflow);
  return Interpolate(x1, x2, y, y_scale, overflow);
}

struct Coon_Color {
  Coon_Color() { memset(comp, 0, sizeof(int) * 3); }

  // Returns true if successful, false if overflow detected.
  bool BiInterpol(Coon_Color colors[4],
                  int x,
                  int y,
                  int x_scale,
                  int y_scale) {
    bool overflow = false;
    for (int i = 0; i < 3; i++) {
      comp[i] = BiInterpolImpl(colors[0].comp[i], colors[1].comp[i],
                               colors[2].comp[i], colors[3].comp[i], x, y,
                               x_scale, y_scale, &overflow);
    }
    return !overflow;
  }

  int Distance(Coon_Color& o) {
    return std::max({abs(comp[0] - o.comp[0]), abs(comp[1] - o.comp[1]),
                     abs(comp[2] - o.comp[2])});
  }

  int comp[3];
};

#define COONCOLOR_THRESHOLD 4
struct CPDF_PatchDrawer {
  void Draw(int x_scale,
            int y_scale,
            int left,
            int bottom,
            Coon_Bezier C1,
            Coon_Bezier C2,
            Coon_Bezier D1,
            Coon_Bezier D2) {
    bool bSmall = C1.Distance() < 2 && C2.Distance() < 2 && D1.Distance() < 2 &&
                  D2.Distance() < 2;
    Coon_Color div_colors[4];
    int d_bottom = 0;
    int d_left = 0;
    int d_top = 0;
    int d_right = 0;
    if (!div_colors[0].BiInterpol(patch_colors, left, bottom, x_scale,
                                  y_scale)) {
      return;
    }
    if (!bSmall) {
      if (!div_colors[1].BiInterpol(patch_colors, left, bottom + 1, x_scale,
                                    y_scale)) {
        return;
      }
      if (!div_colors[2].BiInterpol(patch_colors, left + 1, bottom + 1, x_scale,
                                    y_scale)) {
        return;
      }
      if (!div_colors[3].BiInterpol(patch_colors, left + 1, bottom, x_scale,
                                    y_scale)) {
        return;
      }
      d_bottom = div_colors[3].Distance(div_colors[0]);
      d_left = div_colors[1].Distance(div_colors[0]);
      d_top = div_colors[1].Distance(div_colors[2]);
      d_right = div_colors[2].Distance(div_colors[3]);
    }

    if (bSmall ||
        (d_bottom < COONCOLOR_THRESHOLD && d_left < COONCOLOR_THRESHOLD &&
         d_top < COONCOLOR_THRESHOLD && d_right < COONCOLOR_THRESHOLD)) {
      std::vector<FX_PATHPOINT>& pPoints = path.GetPoints();
      C1.GetPoints(pPoints, 0);
      D2.GetPoints(pPoints, 3);
      C2.GetPointsReverse(pPoints, 6);
      D1.GetPointsReverse(pPoints, 9);
      int fillFlags = FXFILL_WINDING | FXFILL_FULLCOVER;
      if (bNoPathSmooth)
        fillFlags |= FXFILL_NOPATHSMOOTH;
      pDevice->DrawPath(
          &path, nullptr, nullptr,
          ArgbEncode(alpha, div_colors[0].comp[0], div_colors[0].comp[1],
                     div_colors[0].comp[2]),
          0, fillFlags);
    } else {
      if (d_bottom < COONCOLOR_THRESHOLD && d_top < COONCOLOR_THRESHOLD) {
        Coon_Bezier m1;
        m1.BezierInterpol(D1, D2, C1, C2);
        y_scale *= 2;
        bottom *= 2;
        Draw(x_scale, y_scale, left, bottom, C1, m1, D1.first_half(),
             D2.first_half());
        Draw(x_scale, y_scale, left, bottom + 1, m1, C2, D1.second_half(),
             D2.second_half());
      } else if (d_left < COONCOLOR_THRESHOLD &&
                 d_right < COONCOLOR_THRESHOLD) {
        Coon_Bezier m2;
        m2.BezierInterpol(C1, C2, D1, D2);
        x_scale *= 2;
        left *= 2;
        Draw(x_scale, y_scale, left, bottom, C1.first_half(), C2.first_half(),
             D1, m2);
        Draw(x_scale, y_scale, left + 1, bottom, C1.second_half(),
             C2.second_half(), m2, D2);
      } else {
        Coon_Bezier m1, m2;
        m1.BezierInterpol(D1, D2, C1, C2);
        m2.BezierInterpol(C1, C2, D1, D2);
        Coon_Bezier m1f = m1.first_half();
        Coon_Bezier m1s = m1.second_half();
        Coon_Bezier m2f = m2.first_half();
        Coon_Bezier m2s = m2.second_half();
        x_scale *= 2;
        y_scale *= 2;
        left *= 2;
        bottom *= 2;
        Draw(x_scale, y_scale, left, bottom, C1.first_half(), m1f,
             D1.first_half(), m2f);
        Draw(x_scale, y_scale, left, bottom + 1, m1f, C2.first_half(),
             D1.second_half(), m2s);
        Draw(x_scale, y_scale, left + 1, bottom, C1.second_half(), m1s, m2f,
             D2.first_half());
        Draw(x_scale, y_scale, left + 1, bottom + 1, m1s, C2.second_half(), m2s,
             D2.second_half());
      }
    }
  }

  int max_delta;
  CFX_PathData path;
  CFX_RenderDevice* pDevice;
  int bNoPathSmooth;
  int alpha;
  Coon_Color patch_colors[4];
};

void DrawCoonPatchMeshes(
    ShadingType type,
    const RetainPtr<CFX_DIBitmap>& pBitmap,
    const CFX_Matrix& mtObject2Bitmap,
    const CPDF_Stream* pShadingStream,
    const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
    const CPDF_ColorSpace* pCS,
    bool bNoPathSmooth,
    int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);
  ASSERT(type == kCoonsPatchMeshShading ||
         type == kTensorProductPatchMeshShading);

  CFX_DefaultRenderDevice device;
  device.Attach(pBitmap, false, nullptr, false);
  CPDF_MeshStream stream(type, funcs, pShadingStream, pCS);
  if (!stream.Load())
    return;

  CPDF_PatchDrawer patch;
  patch.alpha = alpha;
  patch.pDevice = &device;
  patch.bNoPathSmooth = bNoPathSmooth;

  for (int i = 0; i < 13; i++) {
    patch.path.AppendPoint(
        CFX_PointF(), i == 0 ? FXPT_TYPE::MoveTo : FXPT_TYPE::BezierTo, false);
  }

  CFX_PointF coords[16];
  int point_count = type == kTensorProductPatchMeshShading ? 16 : 12;
  while (!stream.BitStream()->IsEOF()) {
    if (!stream.CanReadFlag())
      break;
    uint32_t flag = stream.ReadFlag();
    int iStartPoint = 0, iStartColor = 0, i = 0;
    if (flag) {
      iStartPoint = 4;
      iStartColor = 2;
      CFX_PointF tempCoords[4];
      for (i = 0; i < 4; i++) {
        tempCoords[i] = coords[(flag * 3 + i) % 12];
      }
      memcpy(coords, tempCoords, sizeof(tempCoords));
      Coon_Color tempColors[2];
      tempColors[0] = patch.patch_colors[flag];
      tempColors[1] = patch.patch_colors[(flag + 1) % 4];
      memcpy(patch.patch_colors, tempColors, sizeof(Coon_Color) * 2);
    }
    for (i = iStartPoint; i < point_count; i++) {
      if (!stream.CanReadCoords())
        break;
      coords[i] = mtObject2Bitmap.Transform(stream.ReadCoords());
    }

    for (i = iStartColor; i < 4; i++) {
      if (!stream.CanReadColor())
        break;

      float r;
      float g;
      float b;
      std::tie(r, g, b) = stream.ReadColor();

      patch.patch_colors[i].comp[0] = (int32_t)(r * 255);
      patch.patch_colors[i].comp[1] = (int32_t)(g * 255);
      patch.patch_colors[i].comp[2] = (int32_t)(b * 255);
    }
    CFX_FloatRect bbox = CFX_FloatRect::GetBBox(coords, point_count);
    if (bbox.right <= 0 || bbox.left >= (float)pBitmap->GetWidth() ||
        bbox.top <= 0 || bbox.bottom >= (float)pBitmap->GetHeight()) {
      continue;
    }
    Coon_Bezier C1, C2, D1, D2;
    C1.FromPoints(coords[0].x, coords[0].y, coords[11].x, coords[11].y,
                  coords[10].x, coords[10].y, coords[9].x, coords[9].y);
    C2.FromPoints(coords[3].x, coords[3].y, coords[4].x, coords[4].y,
                  coords[5].x, coords[5].y, coords[6].x, coords[6].y);
    D1.FromPoints(coords[0].x, coords[0].y, coords[1].x, coords[1].y,
                  coords[2].x, coords[2].y, coords[3].x, coords[3].y);
    D2.FromPoints(coords[9].x, coords[9].y, coords[8].x, coords[8].y,
                  coords[7].x, coords[7].y, coords[6].x, coords[6].y);
    patch.Draw(1, 1, 0, 0, C1, C2, D1, D2);
  }
}

RetainPtr<CFX_DIBitmap> DrawPatternBitmap(
    CPDF_Document* pDoc,
    CPDF_PageRenderCache* pCache,
    CPDF_TilingPattern* pPattern,
    const CFX_Matrix& mtObject2Device,
    int width,
    int height,
    const CPDF_RenderOptions::Options& draw_options) {
  auto pBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!pBitmap->Create(width, height,
                       pPattern->colored() ? FXDIB_Argb : FXDIB_8bppMask)) {
    return nullptr;
  }
  CFX_DefaultRenderDevice bitmap_device;
  bitmap_device.Attach(pBitmap, false, nullptr, false);
  pBitmap->Clear(0);
  CFX_FloatRect cell_bbox =
      pPattern->pattern_to_form()->TransformRect(pPattern->bbox());
  cell_bbox = mtObject2Device.TransformRect(cell_bbox);
  CFX_FloatRect bitmap_rect(0.0f, 0.0f, (float)width, (float)height);
  CFX_Matrix mtAdjust;
  mtAdjust.MatchRect(bitmap_rect, cell_bbox);

  CFX_Matrix mtPattern2Bitmap = mtObject2Device * mtAdjust;
  CPDF_RenderOptions options;
  if (!pPattern->colored())
    options.SetColorMode(CPDF_RenderOptions::kAlpha);

  options.GetOptions() = draw_options;
  options.GetOptions().bForceHalftone = true;

  CPDF_RenderContext context(pDoc, pCache);
  context.AppendLayer(pPattern->form(), &mtPattern2Bitmap);
  context.Render(&bitmap_device, &options, nullptr);
#if defined _SKIA_SUPPORT_PATHS_
  bitmap_device.Flush(true);
  pBitmap->UnPreMultiply();
#endif
  return pBitmap;
}

bool IsAvailableMatrix(const CFX_Matrix& matrix) {
  if (matrix.a == 0 || matrix.d == 0)
    return matrix.b != 0 && matrix.c != 0;

  if (matrix.b == 0 || matrix.c == 0)
    return matrix.a != 0 && matrix.d != 0;

  return true;
}

bool MissingFillColor(const CPDF_ColorState* pColorState) {
  return !pColorState->HasRef() || pColorState->GetFillColor()->IsNull();
}

bool MissingStrokeColor(const CPDF_ColorState* pColorState) {
  return !pColorState->HasRef() || pColorState->GetStrokeColor()->IsNull();
}

bool Type3CharMissingFillColor(const CPDF_Type3Char* pChar,
                               const CPDF_ColorState* pColorState) {
  return pChar && (!pChar->colored() ||
                   (pChar->colored() && MissingFillColor(pColorState)));
}

bool Type3CharMissingStrokeColor(const CPDF_Type3Char* pChar,
                                 const CPDF_ColorState* pColorState) {
  return pChar && (!pChar->colored() ||
                   (pChar->colored() && MissingStrokeColor(pColorState)));
}

}  // namespace

CPDF_RenderStatus::CPDF_RenderStatus(CPDF_RenderContext* pContext,
                                     CFX_RenderDevice* pDevice)
    : m_pContext(pContext), m_pDevice(pDevice) {}

CPDF_RenderStatus::~CPDF_RenderStatus() {}

void CPDF_RenderStatus::Initialize(const CPDF_RenderStatus* pParentState,
                                   const CPDF_GraphicStates* pInitialStates) {
  m_bPrint = m_pDevice->GetDeviceClass() != FXDC_DISPLAY;
  m_pPageResource = m_pContext->GetPageResources();
  if (pInitialStates && !m_pType3Char) {
    m_InitialStates.CopyStates(*pInitialStates);
    if (pParentState) {
      if (!m_InitialStates.m_ColorState.HasFillColor()) {
        m_InitialStates.m_ColorState.SetFillColorRef(
            pParentState->m_InitialStates.m_ColorState.GetFillColorRef());
        *m_InitialStates.m_ColorState.GetMutableFillColor() =
            *pParentState->m_InitialStates.m_ColorState.GetFillColor();
      }
      if (!m_InitialStates.m_ColorState.HasStrokeColor()) {
        m_InitialStates.m_ColorState.SetStrokeColorRef(
            pParentState->m_InitialStates.m_ColorState.GetFillColorRef());
        *m_InitialStates.m_ColorState.GetMutableStrokeColor() =
            *pParentState->m_InitialStates.m_ColorState.GetStrokeColor();
      }
    }
  } else {
    m_InitialStates.DefaultStates();
  }
}

void CPDF_RenderStatus::RenderObjectList(
    const CPDF_PageObjectHolder* pObjectHolder,
    const CFX_Matrix& mtObj2Device) {
#if defined _SKIA_SUPPORT_
  DebugVerifyDeviceIsPreMultiplied();
#endif
  CFX_FloatRect clip_rect = mtObj2Device.GetInverse().TransformRect(
      CFX_FloatRect(m_pDevice->GetClipBox()));
  for (const auto& pCurObj : *pObjectHolder->GetPageObjectList()) {
    if (pCurObj.get() == m_pStopObj) {
      m_bStopped = true;
      return;
    }
    if (!pCurObj)
      continue;

    if (pCurObj->GetRect().left > clip_rect.right ||
        pCurObj->GetRect().right < clip_rect.left ||
        pCurObj->GetRect().bottom > clip_rect.top ||
        pCurObj->GetRect().top < clip_rect.bottom) {
      continue;
    }
    RenderSingleObject(pCurObj.get(), mtObj2Device);
    if (m_bStopped)
      return;
  }
#if defined _SKIA_SUPPORT_
  DebugVerifyDeviceIsPreMultiplied();
#endif
}

void CPDF_RenderStatus::RenderSingleObject(CPDF_PageObject* pObj,
                                           const CFX_Matrix& mtObj2Device) {
#if defined _SKIA_SUPPORT_
  DebugVerifyDeviceIsPreMultiplied();
#endif
  AutoRestorer<int> restorer(&g_CurrentRecursionDepth);
  if (++g_CurrentRecursionDepth > kRenderMaxRecursionDepth) {
    return;
  }
  m_pCurObj = pObj;
  if (m_Options.GetOCContext() &&
      !m_Options.GetOCContext()->CheckObjectVisible(pObj)) {
    return;
  }
  ProcessClipPath(pObj->m_ClipPath, mtObj2Device);
  if (ProcessTransparency(pObj, mtObj2Device)) {
    return;
  }
  ProcessObjectNoClip(pObj, mtObj2Device);
#if defined _SKIA_SUPPORT_
  DebugVerifyDeviceIsPreMultiplied();
#endif
}

bool CPDF_RenderStatus::ContinueSingleObject(CPDF_PageObject* pObj,
                                             const CFX_Matrix& mtObj2Device,
                                             PauseIndicatorIface* pPause) {
  if (m_pImageRenderer) {
    if (m_pImageRenderer->Continue(pPause))
      return true;

    if (!m_pImageRenderer->GetResult())
      DrawObjWithBackground(pObj, mtObj2Device);
    m_pImageRenderer.reset();
    return false;
  }

  m_pCurObj = pObj;
  if (m_Options.GetOCContext() &&
      !m_Options.GetOCContext()->CheckObjectVisible(pObj)) {
    return false;
  }

  ProcessClipPath(pObj->m_ClipPath, mtObj2Device);
  if (ProcessTransparency(pObj, mtObj2Device))
    return false;

  if (!pObj->IsImage()) {
    ProcessObjectNoClip(pObj, mtObj2Device);
    return false;
  }

  m_pImageRenderer = pdfium::MakeUnique<CPDF_ImageRenderer>();
  if (!m_pImageRenderer->Start(this, pObj->AsImage(), mtObj2Device, false,
                               BlendMode::kNormal)) {
    if (!m_pImageRenderer->GetResult())
      DrawObjWithBackground(pObj, mtObj2Device);
    m_pImageRenderer.reset();
    return false;
  }
  return ContinueSingleObject(pObj, mtObj2Device, pPause);
}

FX_RECT CPDF_RenderStatus::GetObjectClippedRect(
    const CPDF_PageObject* pObj,
    const CFX_Matrix& mtObj2Device) const {
  FX_RECT rect = pObj->GetTransformedBBox(mtObj2Device);
  rect.Intersect(m_pDevice->GetClipBox());
  return rect;
}

void CPDF_RenderStatus::ProcessObjectNoClip(CPDF_PageObject* pObj,
                                            const CFX_Matrix& mtObj2Device) {
#if defined _SKIA_SUPPORT_
  DebugVerifyDeviceIsPreMultiplied();
#endif
  bool bRet = false;
  switch (pObj->GetType()) {
    case CPDF_PageObject::TEXT:
      bRet = ProcessText(pObj->AsText(), mtObj2Device, nullptr);
      break;
    case CPDF_PageObject::PATH:
      bRet = ProcessPath(pObj->AsPath(), mtObj2Device);
      break;
    case CPDF_PageObject::IMAGE:
      bRet = ProcessImage(pObj->AsImage(), mtObj2Device);
      break;
    case CPDF_PageObject::SHADING:
      ProcessShading(pObj->AsShading(), mtObj2Device);
      return;
    case CPDF_PageObject::FORM:
      bRet = ProcessForm(pObj->AsForm(), mtObj2Device);
      break;
  }
  if (!bRet)
    DrawObjWithBackground(pObj, mtObj2Device);
#if defined _SKIA_SUPPORT_
  DebugVerifyDeviceIsPreMultiplied();
#endif
}

bool CPDF_RenderStatus::DrawObjWithBlend(CPDF_PageObject* pObj,
                                         const CFX_Matrix& mtObj2Device) {
  switch (pObj->GetType()) {
    case CPDF_PageObject::PATH:
      return ProcessPath(pObj->AsPath(), mtObj2Device);
    case CPDF_PageObject::IMAGE:
      return ProcessImage(pObj->AsImage(), mtObj2Device);
    case CPDF_PageObject::FORM:
      return ProcessForm(pObj->AsForm(), mtObj2Device);
    default:
      return false;
  }
}

void CPDF_RenderStatus::DrawObjWithBackground(CPDF_PageObject* pObj,
                                              const CFX_Matrix& mtObj2Device) {
  FX_RECT rect = GetObjectClippedRect(pObj, mtObj2Device);
  if (rect.IsEmpty())
    return;

  int res = 300;
  if (pObj->IsImage() &&
      m_pDevice->GetDeviceCaps(FXDC_DEVICE_CLASS) == FXDC_PRINTER) {
    res = 0;
  }
  CPDF_ScaledRenderBuffer buffer;
  if (!buffer.Initialize(m_pContext.Get(), m_pDevice, rect, pObj, &m_Options,
                         res)) {
    return;
  }
  CFX_Matrix matrix = mtObj2Device * buffer.GetMatrix();
  const CPDF_Dictionary* pFormResource = nullptr;
  const CPDF_FormObject* pFormObj = pObj->AsForm();
  if (pFormObj) {
    const CPDF_Dictionary* pFormDict = pFormObj->form()->GetDict();
    if (pFormDict)
      pFormResource = pFormDict->GetDictFor("Resources");
  }
  CPDF_RenderStatus status(m_pContext.Get(), buffer.GetDevice());
  status.SetOptions(m_Options);
  status.SetDeviceMatrix(buffer.GetMatrix());
  status.SetTransparency(m_Transparency);
  status.SetDropObjects(m_bDropObjects);
  status.SetFormResource(pFormResource);
  status.Initialize(nullptr, nullptr);
  status.RenderSingleObject(pObj, matrix);
  buffer.OutputToDevice();
}

bool CPDF_RenderStatus::ProcessForm(const CPDF_FormObject* pFormObj,
                                    const CFX_Matrix& mtObj2Device) {
#if defined _SKIA_SUPPORT_
  DebugVerifyDeviceIsPreMultiplied();
#endif
  const CPDF_Dictionary* pOC = pFormObj->form()->GetDict()->GetDictFor("OC");
  if (pOC && m_Options.GetOCContext() &&
      !m_Options.GetOCContext()->CheckOCGVisible(pOC)) {
    return true;
  }
  CFX_Matrix matrix = pFormObj->form_matrix() * mtObj2Device;
  const CPDF_Dictionary* pFormDict = pFormObj->form()->GetDict();
  const CPDF_Dictionary* pResources =
      pFormDict ? pFormDict->GetDictFor("Resources") : nullptr;
  CPDF_RenderStatus status(m_pContext.Get(), m_pDevice);
  status.SetOptions(m_Options);
  status.SetStopObject(m_pStopObj.Get());
  status.SetTransparency(m_Transparency);
  status.SetDropObjects(m_bDropObjects);
  status.SetFormResource(pResources);
  status.Initialize(this, pFormObj);
  status.m_curBlend = m_curBlend;
  {
    CFX_RenderDevice::StateRestorer restorer(m_pDevice);
    status.RenderObjectList(pFormObj->form(), matrix);
    m_bStopped = status.m_bStopped;
  }
#if defined _SKIA_SUPPORT_
  DebugVerifyDeviceIsPreMultiplied();
#endif
  return true;
}

bool CPDF_RenderStatus::ProcessPath(CPDF_PathObject* pPathObj,
                                    const CFX_Matrix& mtObj2Device) {
  int FillType = pPathObj->filltype();
  bool bStroke = pPathObj->stroke();
  ProcessPathPattern(pPathObj, mtObj2Device, &FillType, &bStroke);
  if (FillType == 0 && !bStroke)
    return true;

  uint32_t fill_argb = FillType ? GetFillArgb(pPathObj) : 0;
  uint32_t stroke_argb = bStroke ? GetStrokeArgb(pPathObj) : 0;
  CFX_Matrix path_matrix = pPathObj->matrix() * mtObj2Device;
  if (!IsAvailableMatrix(path_matrix))
    return true;

  if (FillType && m_Options.GetOptions().bRectAA)
    FillType |= FXFILL_RECT_AA;
  if (m_Options.GetOptions().bFillFullcover)
    FillType |= FXFILL_FULLCOVER;
  if (m_Options.GetOptions().bNoPathSmooth)
    FillType |= FXFILL_NOPATHSMOOTH;
  if (bStroke)
    FillType |= FX_FILL_STROKE;

  const CPDF_PageObject* pPageObj =
      static_cast<const CPDF_PageObject*>(pPathObj);
  if (pPageObj->m_GeneralState.GetStrokeAdjust())
    FillType |= FX_STROKE_ADJUST;
  if (m_pType3Char)
    FillType |= FX_FILL_TEXT_MODE;

  CFX_GraphState graphState = pPathObj->m_GraphState;
  if (m_Options.GetOptions().bThinLine)
    graphState.SetLineWidth(0);
  return m_pDevice->DrawPathWithBlend(
      pPathObj->path().GetObject(), &path_matrix, graphState.GetObject(),
      fill_argb, stroke_argb, FillType, m_curBlend);
}

RetainPtr<CPDF_TransferFunc> CPDF_RenderStatus::GetTransferFunc(
    const CPDF_Object* pObj) const {
  ASSERT(pObj);
  CPDF_DocRenderData* pDocCache = m_pContext->GetDocument()->GetRenderData();
  return pDocCache ? pDocCache->GetTransferFunc(pObj) : nullptr;
}

FX_ARGB CPDF_RenderStatus::GetFillArgbInternal(CPDF_PageObject* pObj,
                                               bool bType3) const {
  const CPDF_ColorState* pColorState = &pObj->m_ColorState;
  if (!bType3 && Type3CharMissingFillColor(m_pType3Char.Get(), pColorState))
    return m_T3FillColor;

  if (MissingFillColor(pColorState))
    pColorState = &m_InitialStates.m_ColorState;

  FX_COLORREF colorref = pColorState->GetFillColorRef();
  if (colorref == 0xFFFFFFFF)
    return 0;

  int32_t alpha =
      static_cast<int32_t>((pObj->m_GeneralState.GetFillAlpha() * 255));
  if (pObj->m_GeneralState.GetTR()) {
    if (!pObj->m_GeneralState.GetTransferFunc()) {
      pObj->m_GeneralState.SetTransferFunc(
          GetTransferFunc(pObj->m_GeneralState.GetTR()));
    }
    if (pObj->m_GeneralState.GetTransferFunc()) {
      colorref =
          pObj->m_GeneralState.GetTransferFunc()->TranslateColor(colorref);
    }
  }
  return m_Options.TranslateColor(AlphaAndColorRefToArgb(alpha, colorref));
}

FX_ARGB CPDF_RenderStatus::GetStrokeArgb(CPDF_PageObject* pObj) const {
  const CPDF_ColorState* pColorState = &pObj->m_ColorState;
  if (Type3CharMissingStrokeColor(m_pType3Char.Get(), pColorState))
    return m_T3FillColor;

  if (MissingStrokeColor(pColorState))
    pColorState = &m_InitialStates.m_ColorState;

  FX_COLORREF colorref = pColorState->GetStrokeColorRef();
  if (colorref == 0xFFFFFFFF)
    return 0;

  int32_t alpha = static_cast<int32_t>(pObj->m_GeneralState.GetStrokeAlpha() *
                                       255);  // not rounded.
  if (pObj->m_GeneralState.GetTR()) {
    if (!pObj->m_GeneralState.GetTransferFunc()) {
      pObj->m_GeneralState.SetTransferFunc(
          GetTransferFunc(pObj->m_GeneralState.GetTR()));
    }
    if (pObj->m_GeneralState.GetTransferFunc()) {
      colorref =
          pObj->m_GeneralState.GetTransferFunc()->TranslateColor(colorref);
    }
  }
  return m_Options.TranslateColor(AlphaAndColorRefToArgb(alpha, colorref));
}

void CPDF_RenderStatus::ProcessClipPath(const CPDF_ClipPath& ClipPath,
                                        const CFX_Matrix& mtObj2Device) {
  if (!ClipPath.HasRef()) {
    if (m_LastClipPath.HasRef()) {
      m_pDevice->RestoreState(true);
      m_LastClipPath.SetNull();
    }
    return;
  }
  if (m_LastClipPath == ClipPath)
    return;

  m_LastClipPath = ClipPath;
  m_pDevice->RestoreState(true);
  for (size_t i = 0; i < ClipPath.GetPathCount(); ++i) {
    const CFX_PathData* pPathData = ClipPath.GetPath(i).GetObject();
    if (!pPathData)
      continue;

    if (pPathData->GetPoints().empty()) {
      CFX_PathData EmptyPath;
      EmptyPath.AppendRect(-1, -1, 0, 0);
      m_pDevice->SetClip_PathFill(&EmptyPath, nullptr, FXFILL_WINDING);
    } else {
      m_pDevice->SetClip_PathFill(pPathData, &mtObj2Device,
                                  ClipPath.GetClipType(i));
    }
  }

  if (ClipPath.GetTextCount() == 0)
    return;

  if (m_pDevice->GetDeviceClass() == FXDC_DISPLAY &&
      !(m_pDevice->GetDeviceCaps(FXDC_RENDER_CAPS) & FXRC_SOFT_CLIP)) {
    return;
  }

  std::unique_ptr<CFX_PathData> pTextClippingPath;
  for (size_t i = 0; i < ClipPath.GetTextCount(); ++i) {
    CPDF_TextObject* pText = ClipPath.GetText(i);
    if (pText) {
      if (!pTextClippingPath)
        pTextClippingPath = pdfium::MakeUnique<CFX_PathData>();
      ProcessText(pText, mtObj2Device, pTextClippingPath.get());
      continue;
    }

    if (!pTextClippingPath)
      continue;

    int fill_mode = FXFILL_WINDING;
    if (m_Options.GetOptions().bNoTextSmooth)
      fill_mode |= FXFILL_NOPATHSMOOTH;
    m_pDevice->SetClip_PathFill(pTextClippingPath.get(), nullptr, fill_mode);
    pTextClippingPath.reset();
  }
}

bool CPDF_RenderStatus::ClipPattern(const CPDF_PageObject* pPageObj,
                                    const CFX_Matrix& mtObj2Device,
                                    bool bStroke) {
  if (pPageObj->IsPath())
    return SelectClipPath(pPageObj->AsPath(), mtObj2Device, bStroke);
  if (pPageObj->IsImage()) {
    m_pDevice->SetClip_Rect(pPageObj->GetTransformedBBox(mtObj2Device));
    return true;
  }
  return false;
}

bool CPDF_RenderStatus::SelectClipPath(const CPDF_PathObject* pPathObj,
                                       const CFX_Matrix& mtObj2Device,
                                       bool bStroke) {
  CFX_Matrix path_matrix = pPathObj->matrix() * mtObj2Device;
  if (bStroke) {
    CFX_GraphState graphState = pPathObj->m_GraphState;
    if (m_Options.GetOptions().bThinLine)
      graphState.SetLineWidth(0);
    return m_pDevice->SetClip_PathStroke(pPathObj->path().GetObject(),
                                         &path_matrix, graphState.GetObject());
  }
  int fill_mode = pPathObj->filltype();
  if (m_Options.GetOptions().bNoPathSmooth) {
    fill_mode |= FXFILL_NOPATHSMOOTH;
  }
  return m_pDevice->SetClip_PathFill(pPathObj->path().GetObject(), &path_matrix,
                                     fill_mode);
}

bool CPDF_RenderStatus::ProcessTransparency(CPDF_PageObject* pPageObj,
                                            const CFX_Matrix& mtObj2Device) {
#if defined _SKIA_SUPPORT_
  DebugVerifyDeviceIsPreMultiplied();
#endif
  BlendMode blend_type = pPageObj->m_GeneralState.GetBlendType();
  CPDF_Dictionary* pSMaskDict =
      ToDictionary(pPageObj->m_GeneralState.GetSoftMask());
  if (pSMaskDict) {
    if (pPageObj->IsImage() &&
        pPageObj->AsImage()->GetImage()->GetDict()->KeyExist("SMask")) {
      pSMaskDict = nullptr;
    }
  }
  const CPDF_Dictionary* pFormResource = nullptr;
  float group_alpha = 1.0f;
  CPDF_Transparency transparency = m_Transparency;
  bool bGroupTransparent = false;
  const CPDF_FormObject* pFormObj = pPageObj->AsForm();
  if (pFormObj) {
    group_alpha = pFormObj->m_GeneralState.GetFillAlpha();
    transparency = pFormObj->form()->GetTransparency();
    bGroupTransparent = transparency.IsIsolated();
    const CPDF_Dictionary* pFormDict = pFormObj->form()->GetDict();
    if (pFormDict)
      pFormResource = pFormDict->GetDictFor("Resources");
  }
  bool bTextClip =
      (pPageObj->m_ClipPath.HasRef() &&
       pPageObj->m_ClipPath.GetTextCount() > 0 &&
       m_pDevice->GetDeviceClass() == FXDC_DISPLAY &&
       !(m_pDevice->GetDeviceCaps(FXDC_RENDER_CAPS) & FXRC_SOFT_CLIP));
  if (m_Options.GetOptions().bOverprint && pPageObj->IsImage() &&
      pPageObj->m_GeneralState.GetFillOP() &&
      pPageObj->m_GeneralState.GetStrokeOP()) {
    CPDF_Document* pDocument = nullptr;
    CPDF_Page* pPage = nullptr;
    if (m_pContext->GetPageCache()) {
      pPage = m_pContext->GetPageCache()->GetPage();
      pDocument = pPage->GetDocument();
    } else {
      pDocument = pPageObj->AsImage()->GetImage()->GetDocument();
    }
    const CPDF_Dictionary* pPageResources =
        pPage ? pPage->m_pPageResources.Get() : nullptr;
    const CPDF_Object* pCSObj = pPageObj->AsImage()
                                    ->GetImage()
                                    ->GetStream()
                                    ->GetDict()
                                    ->GetDirectObjectFor("ColorSpace");
    const CPDF_ColorSpace* pColorSpace =
        pDocument->LoadColorSpace(pCSObj, pPageResources);
    if (pColorSpace) {
      int format = pColorSpace->GetFamily();
      if (format == PDFCS_DEVICECMYK || format == PDFCS_SEPARATION ||
          format == PDFCS_DEVICEN) {
        blend_type = BlendMode::kDarken;
      }
      pDocument->GetPageData()->ReleaseColorSpace(pCSObj);
    }
  }
  if (!pSMaskDict && group_alpha == 1.0f && blend_type == BlendMode::kNormal &&
      !bTextClip && !bGroupTransparent) {
    return false;
  }
  if (m_bPrint) {
    bool bRet = false;
    int rendCaps = m_pDevice->GetRenderCaps();
    if (!(transparency.IsIsolated() || pSMaskDict || bTextClip) &&
        (rendCaps & FXRC_BLEND_MODE)) {
      BlendMode oldBlend = m_curBlend;
      m_curBlend = blend_type;
      bRet = DrawObjWithBlend(pPageObj, mtObj2Device);
      m_curBlend = oldBlend;
    }
    if (!bRet) {
      DrawObjWithBackground(pPageObj, mtObj2Device);
    }
    return true;
  }
  FX_RECT rect = pPageObj->GetTransformedBBox(mtObj2Device);
  rect.Intersect(m_pDevice->GetClipBox());
  if (rect.IsEmpty())
    return true;

  int width = rect.Width();
  int height = rect.Height();
  CFX_DefaultRenderDevice bitmap_device;
  RetainPtr<CFX_DIBitmap> backdrop;
  if (!transparency.IsIsolated() &&
      (m_pDevice->GetRenderCaps() & FXRC_GET_BITS)) {
    backdrop = pdfium::MakeRetain<CFX_DIBitmap>();
    if (!m_pDevice->CreateCompatibleBitmap(backdrop, width, height))
      return true;
    m_pDevice->GetDIBits(backdrop, rect.left, rect.top);
  }
  if (!bitmap_device.Create(width, height, FXDIB_Argb, backdrop))
    return true;

  RetainPtr<CFX_DIBitmap> bitmap = bitmap_device.GetBitmap();
  bitmap->Clear(0);

  CFX_Matrix new_matrix = mtObj2Device;
  new_matrix.Translate(-rect.left, -rect.top);

  RetainPtr<CFX_DIBitmap> pTextMask;
  if (bTextClip) {
    pTextMask = pdfium::MakeRetain<CFX_DIBitmap>();
    if (!pTextMask->Create(width, height, FXDIB_8bppMask))
      return true;

    pTextMask->Clear(0);
    CFX_DefaultRenderDevice text_device;
    text_device.Attach(pTextMask, false, nullptr, false);
    for (size_t i = 0; i < pPageObj->m_ClipPath.GetTextCount(); ++i) {
      CPDF_TextObject* textobj = pPageObj->m_ClipPath.GetText(i);
      if (!textobj)
        break;

      CFX_Matrix text_matrix = textobj->GetTextMatrix();
      CPDF_TextRenderer::DrawTextPath(
          &text_device, textobj->GetCharCodes(), textobj->GetCharPositions(),
          textobj->m_TextState.GetFont(), textobj->m_TextState.GetFontSize(),
          &text_matrix, &new_matrix, textobj->m_GraphState.GetObject(),
          (FX_ARGB)-1, 0, nullptr, 0);
    }
  }
  CPDF_RenderStatus bitmap_render(m_pContext.Get(), &bitmap_device);
  bitmap_render.SetOptions(m_Options);
  bitmap_render.SetStopObject(m_pStopObj.Get());
  bitmap_render.SetStdCS(true);
  bitmap_render.SetDropObjects(m_bDropObjects);
  bitmap_render.SetFormResource(pFormResource);
  bitmap_render.Initialize(nullptr, nullptr);
  bitmap_render.ProcessObjectNoClip(pPageObj, new_matrix);
#if defined _SKIA_SUPPORT_PATHS_
  bitmap_device.Flush(true);
  bitmap->UnPreMultiply();
#endif
  m_bStopped = bitmap_render.m_bStopped;
  if (pSMaskDict) {
    CFX_Matrix smask_matrix =
        *pPageObj->m_GeneralState.GetSMaskMatrix() * mtObj2Device;
    RetainPtr<CFX_DIBBase> pSMaskSource =
        LoadSMask(pSMaskDict, &rect, &smask_matrix);
    if (pSMaskSource)
      bitmap->MultiplyAlpha(pSMaskSource);
  }
  if (pTextMask) {
    bitmap->MultiplyAlpha(pTextMask);
    pTextMask.Reset();
  }
  int32_t blitAlpha = 255;
  if (group_alpha != 1.0f && transparency.IsGroup()) {
    blitAlpha = (int32_t)(group_alpha * 255);
#ifndef _SKIA_SUPPORT_
    bitmap->MultiplyAlpha(blitAlpha);
    blitAlpha = 255;
#endif
  }
  transparency = m_Transparency;
  if (pPageObj->IsForm()) {
    transparency.SetGroup();
  }
  CompositeDIBitmap(bitmap, rect.left, rect.top, 0, blitAlpha, blend_type,
                    transparency);
#if defined _SKIA_SUPPORT_
  DebugVerifyDeviceIsPreMultiplied();
#endif
  return true;
}

RetainPtr<CFX_DIBitmap> CPDF_RenderStatus::GetBackdrop(
    const CPDF_PageObject* pObj,
    const FX_RECT& rect,
    bool bBackAlphaRequired,
    int* left,
    int* top) {
  FX_RECT bbox = rect;
  bbox.Intersect(m_pDevice->GetClipBox());
  *left = bbox.left;
  *top = bbox.top;
  int width = bbox.Width();
  int height = bbox.Height();
  auto pBackdrop = pdfium::MakeRetain<CFX_DIBitmap>();
  if (bBackAlphaRequired && !m_bDropObjects)
    pBackdrop->Create(width, height, FXDIB_Argb);
  else
    m_pDevice->CreateCompatibleBitmap(pBackdrop, width, height);

  if (!pBackdrop->GetBuffer())
    return nullptr;

  bool bNeedDraw;
  if (pBackdrop->HasAlpha())
    bNeedDraw = !(m_pDevice->GetRenderCaps() & FXRC_ALPHA_OUTPUT);
  else
    bNeedDraw = !(m_pDevice->GetRenderCaps() & FXRC_GET_BITS);

  if (!bNeedDraw) {
    m_pDevice->GetDIBits(pBackdrop, *left, *top);
    return pBackdrop;
  }
  CFX_Matrix FinalMatrix = m_DeviceMatrix;
  FinalMatrix.Translate(-*left, -*top);
  pBackdrop->Clear(pBackdrop->HasAlpha() ? 0 : 0xffffffff);

  CFX_DefaultRenderDevice device;
  device.Attach(pBackdrop, false, nullptr, false);
  m_pContext->Render(&device, pObj, &m_Options, &FinalMatrix);
  return pBackdrop;
}

std::unique_ptr<CPDF_GraphicStates> CPDF_RenderStatus::CloneObjStates(
    const CPDF_GraphicStates* pSrcStates,
    bool bStroke) {
  if (!pSrcStates)
    return nullptr;

  auto pStates = pdfium::MakeUnique<CPDF_GraphicStates>();
  pStates->CopyStates(*pSrcStates);
  const CPDF_Color* pObjColor = bStroke
                                    ? pSrcStates->m_ColorState.GetStrokeColor()
                                    : pSrcStates->m_ColorState.GetFillColor();
  if (!pObjColor->IsNull()) {
    pStates->m_ColorState.SetFillColorRef(
        bStroke ? pSrcStates->m_ColorState.GetStrokeColorRef()
                : pSrcStates->m_ColorState.GetFillColorRef());
    pStates->m_ColorState.SetStrokeColorRef(
        pStates->m_ColorState.GetFillColorRef());
  }
  return pStates;
}

#if defined _SKIA_SUPPORT_
void CPDF_RenderStatus::DebugVerifyDeviceIsPreMultiplied() const {
  m_pDevice->DebugVerifyBitmapIsPreMultiplied();
}
#endif

bool CPDF_RenderStatus::ProcessText(CPDF_TextObject* textobj,
                                    const CFX_Matrix& mtObj2Device,
                                    CFX_PathData* pClippingPath) {
  if (textobj->GetCharCodes().empty())
    return true;

  const TextRenderingMode text_render_mode = textobj->m_TextState.GetTextMode();
  if (text_render_mode == TextRenderingMode::MODE_INVISIBLE)
    return true;

  CPDF_Font* pFont = textobj->m_TextState.GetFont();
  if (pFont->IsType3Font())
    return ProcessType3Text(textobj, mtObj2Device);

  bool bFill = false;
  bool bStroke = false;
  bool bClip = false;
  if (pClippingPath) {
    bClip = true;
  } else {
    switch (text_render_mode) {
      case TextRenderingMode::MODE_FILL:
      case TextRenderingMode::MODE_FILL_CLIP:
        bFill = true;
        break;
      case TextRenderingMode::MODE_STROKE:
      case TextRenderingMode::MODE_STROKE_CLIP:
        if (pFont->HasFace())
          bStroke = true;
        else
          bFill = true;
        break;
      case TextRenderingMode::MODE_FILL_STROKE:
      case TextRenderingMode::MODE_FILL_STROKE_CLIP:
        bFill = true;
        if (pFont->HasFace())
          bStroke = true;
        break;
      case TextRenderingMode::MODE_INVISIBLE:
        // Already handled above, but the compiler is not smart enough to
        // realize it. Fall through.
        NOTREACHED();
        return true;
      case TextRenderingMode::MODE_CLIP:
        return true;
    }
  }
  FX_ARGB stroke_argb = 0;
  FX_ARGB fill_argb = 0;
  bool bPattern = false;
  if (bStroke) {
    if (textobj->m_ColorState.GetStrokeColor()->IsPattern()) {
      bPattern = true;
    } else {
      stroke_argb = GetStrokeArgb(textobj);
    }
  }
  if (bFill) {
    if (textobj->m_ColorState.GetFillColor()->IsPattern()) {
      bPattern = true;
    } else {
      fill_argb = GetFillArgb(textobj);
    }
  }
  CFX_Matrix text_matrix = textobj->GetTextMatrix();
  if (!IsAvailableMatrix(text_matrix))
    return true;

  float font_size = textobj->m_TextState.GetFontSize();
  if (bPattern) {
    DrawTextPathWithPattern(textobj, mtObj2Device, pFont, font_size,
                            &text_matrix, bFill, bStroke);
    return true;
  }
  if (bClip || bStroke) {
    const CFX_Matrix* pDeviceMatrix = &mtObj2Device;
    CFX_Matrix device_matrix;
    if (bStroke) {
      const float* pCTM = textobj->m_TextState.GetCTM();
      if (pCTM[0] != 1.0f || pCTM[3] != 1.0f) {
        CFX_Matrix ctm(pCTM[0], pCTM[1], pCTM[2], pCTM[3], 0, 0);
        text_matrix *= ctm.GetInverse();
        device_matrix = ctm * mtObj2Device;
        pDeviceMatrix = &device_matrix;
      }
    }
    int flag = 0;
    if (bStroke && bFill) {
      flag |= FX_FILL_STROKE;
      flag |= FX_STROKE_TEXT_MODE;
    }
    if (textobj->m_GeneralState.GetStrokeAdjust())
      flag |= FX_STROKE_ADJUST;
    if (m_Options.GetOptions().bNoTextSmooth)
      flag |= FXFILL_NOPATHSMOOTH;
    return CPDF_TextRenderer::DrawTextPath(
        m_pDevice, textobj->GetCharCodes(), textobj->GetCharPositions(), pFont,
        font_size, &text_matrix, pDeviceMatrix,
        textobj->m_GraphState.GetObject(), fill_argb, stroke_argb,
        pClippingPath, flag);
  }
  text_matrix.Concat(mtObj2Device);
  return CPDF_TextRenderer::DrawNormalText(
      m_pDevice, textobj->GetCharCodes(), textobj->GetCharPositions(), pFont,
      font_size, &text_matrix, fill_argb, &m_Options);
}

RetainPtr<CPDF_Type3Cache> CPDF_RenderStatus::GetCachedType3(
    CPDF_Type3Font* pFont) {
  CPDF_Document* pDoc = pFont->GetDocument();
  if (!pDoc)
    return nullptr;

  pDoc->GetPageData()->GetFont(pFont->GetFontDict());
  return pDoc->GetRenderData()->GetCachedType3(pFont);
}

// TODO(npm): Font fallback for type 3 fonts? (Completely separate code!!)
bool CPDF_RenderStatus::ProcessType3Text(CPDF_TextObject* textobj,
                                         const CFX_Matrix& mtObj2Device) {
  CPDF_Type3Font* pType3Font = textobj->m_TextState.GetFont()->AsType3Font();
  if (pdfium::ContainsValue(m_Type3FontCache, pType3Font))
    return true;

  CFX_Matrix text_matrix = textobj->GetTextMatrix();
  CFX_Matrix char_matrix = pType3Font->GetFontMatrix();
  float font_size = textobj->m_TextState.GetFontSize();
  char_matrix.Scale(font_size, font_size);
  FX_ARGB fill_argb = GetFillArgbForType3(textobj);
  int fill_alpha = FXARGB_A(fill_argb);
  int device_class = m_pDevice->GetDeviceClass();
  std::vector<FXTEXT_GLYPHPOS> glyphs;
  if (device_class == FXDC_DISPLAY)
    glyphs.resize(textobj->GetCharCodes().size());
  else if (fill_alpha < 255)
    return false;

  CPDF_RefType3Cache refTypeCache(pType3Font);
  for (size_t iChar = 0; iChar < textobj->GetCharCodes().size(); ++iChar) {
    uint32_t charcode = textobj->GetCharCodes()[iChar];
    if (charcode == static_cast<uint32_t>(-1))
      continue;

    CPDF_Type3Char* pType3Char = pType3Font->LoadChar(charcode);
    if (!pType3Char)
      continue;

    CFX_Matrix matrix = char_matrix;
    matrix.e += iChar > 0 ? textobj->GetCharPositions()[iChar - 1] : 0;
    matrix.Concat(text_matrix);
    matrix.Concat(mtObj2Device);
    if (!pType3Char->LoadBitmap(m_pContext.Get())) {
      if (!glyphs.empty()) {
        for (size_t i = 0; i < iChar; ++i) {
          const FXTEXT_GLYPHPOS& glyph = glyphs[i];
          if (!glyph.m_pGlyph)
            continue;

          m_pDevice->SetBitMask(glyph.m_pGlyph->m_pBitmap,
                                glyph.m_Origin.x + glyph.m_pGlyph->m_Left,
                                glyph.m_Origin.y - glyph.m_pGlyph->m_Top,
                                fill_argb);
        }
        glyphs.clear();
      }

      std::unique_ptr<CPDF_GraphicStates> pStates =
          CloneObjStates(textobj, false);
      CPDF_RenderOptions options = m_Options;
      options.GetOptions().bForceHalftone = true;
      options.GetOptions().bRectAA = true;
      options.GetOptions().bForceDownsample = false;

      const CPDF_Dictionary* pFormResource = nullptr;
      if (pType3Char->form() && pType3Char->form()->GetDict()) {
        pFormResource = pType3Char->form()->GetDict()->GetDictFor("Resources");
      }
      if (fill_alpha == 255) {
        CPDF_RenderStatus status(m_pContext.Get(), m_pDevice);
        status.SetOptions(options);
        status.SetTransparency(pType3Char->form()->GetTransparency());
        status.SetType3Char(pType3Char);
        status.SetFillColor(fill_argb);
        status.SetDropObjects(m_bDropObjects);
        status.SetFormResource(pFormResource);
        status.Initialize(this, pStates.get());
        status.m_Type3FontCache = m_Type3FontCache;
        status.m_Type3FontCache.push_back(pType3Font);

        CFX_RenderDevice::StateRestorer restorer(m_pDevice);
        status.RenderObjectList(pType3Char->form(), matrix);
      } else {
        FX_RECT rect =
            matrix.TransformRect(pType3Char->form()->CalcBoundingBox())
                .GetOuterRect();
        CFX_DefaultRenderDevice bitmap_device;
        if (!bitmap_device.Create(rect.Width(), rect.Height(), FXDIB_Argb,
                                  nullptr)) {
          return true;
        }
        bitmap_device.GetBitmap()->Clear(0);
        CPDF_RenderStatus status(m_pContext.Get(), &bitmap_device);
        status.SetOptions(options);
        status.SetTransparency(pType3Char->form()->GetTransparency());
        status.SetType3Char(pType3Char);
        status.SetFillColor(fill_argb);
        status.SetDropObjects(m_bDropObjects);
        status.SetFormResource(pFormResource);
        status.Initialize(this, pStates.get());
        status.m_Type3FontCache = m_Type3FontCache;
        status.m_Type3FontCache.push_back(pType3Font);
        matrix.Translate(-rect.left, -rect.top);
        status.RenderObjectList(pType3Char->form(), matrix);
        m_pDevice->SetDIBits(bitmap_device.GetBitmap(), rect.left, rect.top);
      }
    } else if (pType3Char->GetBitmap()) {
      if (device_class == FXDC_DISPLAY) {
        RetainPtr<CPDF_Type3Cache> pCache = GetCachedType3(pType3Font);
        refTypeCache.m_dwCount++;
        CFX_GlyphBitmap* pBitmap = pCache->LoadGlyph(charcode, &matrix);
        if (!pBitmap)
          continue;

        CFX_Point origin(FXSYS_round(matrix.e), FXSYS_round(matrix.f));
        if (glyphs.empty()) {
          m_pDevice->SetBitMask(pBitmap->m_pBitmap, origin.x + pBitmap->m_Left,
                                origin.y - pBitmap->m_Top, fill_argb);
        } else {
          glyphs[iChar].m_pGlyph = pBitmap;
          glyphs[iChar].m_Origin = origin;
        }
      } else {
        CFX_Matrix image_matrix = pType3Char->matrix() * matrix;
        CPDF_ImageRenderer renderer;
        if (renderer.Start(this, pType3Char->GetBitmap(), fill_argb, 255,
                           image_matrix, FXDIB_ResampleOptions(), false,
                           BlendMode::kNormal)) {
          renderer.Continue(nullptr);
        }
        if (!renderer.GetResult())
          return false;
      }
    }
  }

  if (glyphs.empty())
    return true;

  FX_RECT rect = FXGE_GetGlyphsBBox(glyphs, 0);
  auto pBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!pBitmap->Create(rect.Width(), rect.Height(), FXDIB_8bppMask))
    return true;

  pBitmap->Clear(0);
  for (const FXTEXT_GLYPHPOS& glyph : glyphs) {
    if (!glyph.m_pGlyph)
      continue;

    pdfium::base::CheckedNumeric<int> left = glyph.m_Origin.x;
    left += glyph.m_pGlyph->m_Left;
    left -= rect.left;
    if (!left.IsValid())
      continue;

    pdfium::base::CheckedNumeric<int> top = glyph.m_Origin.y;
    top -= glyph.m_pGlyph->m_Top;
    top -= rect.top;
    if (!top.IsValid())
      continue;

    pBitmap->CompositeMask(left.ValueOrDie(), top.ValueOrDie(),
                           glyph.m_pGlyph->m_pBitmap->GetWidth(),
                           glyph.m_pGlyph->m_pBitmap->GetHeight(),
                           glyph.m_pGlyph->m_pBitmap, fill_argb, 0, 0,
                           BlendMode::kNormal, nullptr, false, 0);
  }
  m_pDevice->SetBitMask(pBitmap, rect.left, rect.top, fill_argb);
  return true;
}

void CPDF_RenderStatus::DrawTextPathWithPattern(const CPDF_TextObject* textobj,
                                                const CFX_Matrix& mtObj2Device,
                                                CPDF_Font* pFont,
                                                float font_size,
                                                const CFX_Matrix* pTextMatrix,
                                                bool bFill,
                                                bool bStroke) {
  if (!bStroke) {
    std::vector<std::unique_ptr<CPDF_TextObject>> pCopy;
    pCopy.push_back(std::unique_ptr<CPDF_TextObject>(textobj->Clone()));

    CPDF_PathObject path;
    path.set_filltype(FXFILL_WINDING);
    path.m_ClipPath.AppendTexts(&pCopy);
    path.m_ColorState = textobj->m_ColorState;
    path.m_GeneralState = textobj->m_GeneralState;
    path.path().AppendRect(textobj->GetRect().left, textobj->GetRect().bottom,
                           textobj->GetRect().right, textobj->GetRect().top);
    path.SetRect(textobj->GetRect());

    AutoRestorer<UnownedPtr<const CPDF_PageObject>> restorer2(&m_pCurObj);
    RenderSingleObject(&path, mtObj2Device);
    return;
  }
  CPDF_CharPosList CharPosList;
  CharPosList.Load(textobj->GetCharCodes(), textobj->GetCharPositions(), pFont,
                   font_size);
  for (uint32_t i = 0; i < CharPosList.m_nChars; i++) {
    FXTEXT_CHARPOS& charpos = CharPosList.m_pCharPos[i];
    auto* font = charpos.m_FallbackFontPosition == -1
                     ? pFont->GetFont()
                     : pFont->GetFontFallback(charpos.m_FallbackFontPosition);
    const CFX_PathData* pPath =
        font->LoadGlyphPath(charpos.m_GlyphIndex, charpos.m_FontCharWidth);
    if (!pPath)
      continue;

    CPDF_PathObject path;
    path.m_GraphState = textobj->m_GraphState;
    path.m_ColorState = textobj->m_ColorState;

    CFX_Matrix matrix;
    if (charpos.m_bGlyphAdjust) {
      matrix = CFX_Matrix(charpos.m_AdjustMatrix[0], charpos.m_AdjustMatrix[1],
                          charpos.m_AdjustMatrix[2], charpos.m_AdjustMatrix[3],
                          0, 0);
    }
    matrix.Concat(CFX_Matrix(font_size, 0, 0, font_size, charpos.m_Origin.x,
                             charpos.m_Origin.y));
    path.set_stroke(bStroke);
    path.set_filltype(bFill ? FXFILL_WINDING : 0);
    path.path().Append(pPath, &matrix);
    path.set_matrix(*pTextMatrix);
    path.CalcBoundingBox();
    ProcessPath(&path, mtObj2Device);
  }
}

void CPDF_RenderStatus::DrawShading(const CPDF_ShadingPattern* pPattern,
                                    const CFX_Matrix& mtMatrix,
                                    const FX_RECT& clip_rect,
                                    int alpha,
                                    bool bAlphaMode) {
  const auto& funcs = pPattern->GetFuncs();
  const CPDF_Dictionary* pDict = pPattern->GetShadingObject()->GetDict();
  const CPDF_ColorSpace* pColorSpace = pPattern->GetCS();
  if (!pColorSpace)
    return;

  FX_ARGB background = 0;
  if (!pPattern->IsShadingObject() && pDict->KeyExist("Background")) {
    const CPDF_Array* pBackColor = pDict->GetArrayFor("Background");
    if (pBackColor && pBackColor->size() >= pColorSpace->CountComponents()) {
      std::vector<float> comps(pColorSpace->CountComponents());
      for (uint32_t i = 0; i < pColorSpace->CountComponents(); i++)
        comps[i] = pBackColor->GetNumberAt(i);
      float R = 0.0f;
      float G = 0.0f;
      float B = 0.0f;
      pColorSpace->GetRGB(comps.data(), &R, &G, &B);
      background = ArgbEncode(255, (int32_t)(R * 255), (int32_t)(G * 255),
                              (int32_t)(B * 255));
    }
  }
  FX_RECT clip_rect_bbox = clip_rect;
  if (pDict->KeyExist("BBox")) {
    clip_rect_bbox.Intersect(
        mtMatrix.TransformRect(pDict->GetRectFor("BBox")).GetOuterRect());
  }
  if (m_pDevice->GetDeviceCaps(FXDC_RENDER_CAPS) & FXRC_SHADING &&
      m_pDevice->GetDeviceDriver()->DrawShading(
          pPattern, &mtMatrix, clip_rect_bbox, alpha, bAlphaMode)) {
    return;
  }
  CPDF_DeviceBuffer buffer;
  buffer.Initialize(m_pContext.Get(), m_pDevice, clip_rect_bbox,
                    m_pCurObj.Get(), 150);
  CFX_Matrix FinalMatrix = mtMatrix * buffer.GetMatrix();
  RetainPtr<CFX_DIBitmap> pBitmap = buffer.GetBitmap();
  if (!pBitmap->GetBuffer())
    return;

  pBitmap->Clear(background);
  switch (pPattern->GetShadingType()) {
    case kInvalidShading:
    case kMaxShading:
      return;
    case kFunctionBasedShading:
      DrawFuncShading(pBitmap, FinalMatrix, pDict, funcs, pColorSpace, alpha);
      break;
    case kAxialShading:
      DrawAxialShading(pBitmap, FinalMatrix, pDict, funcs, pColorSpace, alpha);
      break;
    case kRadialShading:
      DrawRadialShading(pBitmap, FinalMatrix, pDict, funcs, pColorSpace, alpha);
      break;
    case kFreeFormGouraudTriangleMeshShading: {
      // The shading object can be a stream or a dictionary. We do not handle
      // the case of dictionary at the moment.
      if (const CPDF_Stream* pStream = ToStream(pPattern->GetShadingObject())) {
        DrawFreeGouraudShading(pBitmap, FinalMatrix, pStream, funcs,
                               pColorSpace, alpha);
      }
    } break;
    case kLatticeFormGouraudTriangleMeshShading: {
      // The shading object can be a stream or a dictionary. We do not handle
      // the case of dictionary at the moment.
      if (const CPDF_Stream* pStream = ToStream(pPattern->GetShadingObject())) {
        DrawLatticeGouraudShading(pBitmap, FinalMatrix, pStream, funcs,
                                  pColorSpace, alpha);
      }
    } break;
    case kCoonsPatchMeshShading:
    case kTensorProductPatchMeshShading: {
      // The shading object can be a stream or a dictionary. We do not handle
      // the case of dictionary at the moment.
      if (const CPDF_Stream* pStream = ToStream(pPattern->GetShadingObject())) {
        DrawCoonPatchMeshes(pPattern->GetShadingType(), pBitmap, FinalMatrix,
                            pStream, funcs, pColorSpace,
                            m_Options.GetOptions().bNoPathSmooth, alpha);
      }
    } break;
  }
  if (bAlphaMode)
    pBitmap->LoadChannelFromAlpha(FXDIB_Red, pBitmap);

  if (m_Options.ColorModeIs(CPDF_RenderOptions::kGray))
    pBitmap->ConvertColorScale(0, 0xffffff);
  buffer.OutputToDevice();
}

void CPDF_RenderStatus::DrawShadingPattern(CPDF_ShadingPattern* pattern,
                                           const CPDF_PageObject* pPageObj,
                                           const CFX_Matrix& mtObj2Device,
                                           bool bStroke) {
  if (!pattern->Load())
    return;

  CFX_RenderDevice::StateRestorer restorer(m_pDevice);
  if (!ClipPattern(pPageObj, mtObj2Device, bStroke))
    return;

  FX_RECT rect = GetObjectClippedRect(pPageObj, mtObj2Device);
  if (rect.IsEmpty())
    return;

  CFX_Matrix matrix = *pattern->pattern_to_form() * mtObj2Device;
  int alpha =
      FXSYS_round(255 * (bStroke ? pPageObj->m_GeneralState.GetStrokeAlpha()
                                 : pPageObj->m_GeneralState.GetFillAlpha()));
  DrawShading(pattern, matrix, rect, alpha,
              m_Options.ColorModeIs(CPDF_RenderOptions::kAlpha));
}

void CPDF_RenderStatus::ProcessShading(const CPDF_ShadingObject* pShadingObj,
                                       const CFX_Matrix& mtObj2Device) {
  FX_RECT rect = pShadingObj->GetTransformedBBox(mtObj2Device);
  FX_RECT clip_box = m_pDevice->GetClipBox();
  rect.Intersect(clip_box);
  if (rect.IsEmpty())
    return;

  CFX_Matrix matrix = pShadingObj->matrix() * mtObj2Device;
  DrawShading(pShadingObj->pattern(), matrix, rect,
              FXSYS_round(255 * pShadingObj->m_GeneralState.GetFillAlpha()),
              m_Options.ColorModeIs(CPDF_RenderOptions::kAlpha));
}

void CPDF_RenderStatus::DrawTilingPattern(CPDF_TilingPattern* pPattern,
                                          CPDF_PageObject* pPageObj,
                                          const CFX_Matrix& mtObj2Device,
                                          bool bStroke) {
  if (!pPattern->Load())
    return;

  CFX_RenderDevice::StateRestorer restorer(m_pDevice);
  if (!ClipPattern(pPageObj, mtObj2Device, bStroke))
    return;

  FX_RECT clip_box = m_pDevice->GetClipBox();
  if (clip_box.IsEmpty())
    return;

  CFX_Matrix mtPattern2Device = *pPattern->pattern_to_form() * mtObj2Device;

  bool bAligned =
      pPattern->bbox().left == 0 && pPattern->bbox().bottom == 0 &&
      pPattern->bbox().right == pPattern->x_step() &&
      pPattern->bbox().top == pPattern->y_step() &&
      (mtPattern2Device.IsScaled() || mtPattern2Device.Is90Rotated());

  CFX_FloatRect cell_bbox = mtPattern2Device.TransformRect(pPattern->bbox());

  float ceil_height = std::ceil(cell_bbox.Height());
  float ceil_width = std::ceil(cell_bbox.Width());

  // Validate the float will fit into the int when the conversion is done.
  if (!pdfium::base::IsValueInRangeForNumericType<int>(ceil_height) ||
      !pdfium::base::IsValueInRangeForNumericType<int>(ceil_width)) {
    return;
  }

  int width = static_cast<int>(ceil_width);
  int height = static_cast<int>(ceil_height);
  if (width <= 0)
    width = 1;
  if (height <= 0)
    height = 1;

  CFX_FloatRect clip_box_p =
      mtPattern2Device.GetInverse().TransformRect(CFX_FloatRect(clip_box));
  int min_col = (int)ceil((clip_box_p.left - pPattern->bbox().right) /
                          pPattern->x_step());
  int max_col = (int)floor((clip_box_p.right - pPattern->bbox().left) /
                           pPattern->x_step());
  int min_row = (int)ceil((clip_box_p.bottom - pPattern->bbox().top) /
                          pPattern->y_step());
  int max_row = (int)floor((clip_box_p.top - pPattern->bbox().bottom) /
                           pPattern->y_step());

  // Make sure we can fit the needed width * height into an int.
  if (height > std::numeric_limits<int>::max() / width)
    return;

  if (width > clip_box.Width() || height > clip_box.Height() ||
      width * height > clip_box.Width() * clip_box.Height()) {
    std::unique_ptr<CPDF_GraphicStates> pStates;
    if (!pPattern->colored())
      pStates = CloneObjStates(pPageObj, bStroke);

    const CPDF_Dictionary* pFormDict = pPattern->form()->GetDict();
    const CPDF_Dictionary* pFormResource =
        pFormDict ? pFormDict->GetDictFor("Resources") : nullptr;
    for (int col = min_col; col <= max_col; col++) {
      for (int row = min_row; row <= max_row; row++) {
        CFX_PointF original = mtPattern2Device.Transform(
            CFX_PointF(col * pPattern->x_step(), row * pPattern->y_step()));
        CFX_Matrix matrix = mtObj2Device;
        matrix.Translate(original.x - mtPattern2Device.e,
                         original.y - mtPattern2Device.f);
        CFX_RenderDevice::StateRestorer restorer2(m_pDevice);
        CPDF_RenderStatus status(m_pContext.Get(), m_pDevice);
        status.SetOptions(m_Options);
        status.SetTransparency(pPattern->form()->GetTransparency());
        status.SetFormResource(pFormResource);
        status.SetDropObjects(m_bDropObjects);
        status.Initialize(this, pStates.get());
        status.RenderObjectList(pPattern->form(), matrix);
      }
    }
    return;
  }
  if (bAligned) {
    int orig_x = FXSYS_round(mtPattern2Device.e);
    int orig_y = FXSYS_round(mtPattern2Device.f);
    min_col = (clip_box.left - orig_x) / width;
    if (clip_box.left < orig_x)
      min_col--;

    max_col = (clip_box.right - orig_x) / width;
    if (clip_box.right <= orig_x)
      max_col--;

    min_row = (clip_box.top - orig_y) / height;
    if (clip_box.top < orig_y)
      min_row--;

    max_row = (clip_box.bottom - orig_y) / height;
    if (clip_box.bottom <= orig_y)
      max_row--;
  }
  float left_offset = cell_bbox.left - mtPattern2Device.e;
  float top_offset = cell_bbox.bottom - mtPattern2Device.f;
  RetainPtr<CFX_DIBitmap> pPatternBitmap;
  if (width * height < 16) {
    RetainPtr<CFX_DIBitmap> pEnlargedBitmap =
        DrawPatternBitmap(m_pContext->GetDocument(), m_pContext->GetPageCache(),
                          pPattern, mtObj2Device, 8, 8, m_Options.GetOptions());
    pPatternBitmap = pEnlargedBitmap->StretchTo(
        width, height, FXDIB_ResampleOptions(), nullptr);
  } else {
    pPatternBitmap = DrawPatternBitmap(
        m_pContext->GetDocument(), m_pContext->GetPageCache(), pPattern,
        mtObj2Device, width, height, m_Options.GetOptions());
  }
  if (!pPatternBitmap)
    return;

  if (m_Options.ColorModeIs(CPDF_RenderOptions::kGray))
    pPatternBitmap->ConvertColorScale(0, 0xffffff);

  FX_ARGB fill_argb = GetFillArgb(pPageObj);
  int clip_width = clip_box.right - clip_box.left;
  int clip_height = clip_box.bottom - clip_box.top;
  auto pScreen = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!pScreen->Create(clip_width, clip_height, FXDIB_Argb))
    return;

  pScreen->Clear(0);
  uint32_t* src_buf = (uint32_t*)pPatternBitmap->GetBuffer();
  for (int col = min_col; col <= max_col; col++) {
    for (int row = min_row; row <= max_row; row++) {
      int start_x, start_y;
      if (bAligned) {
        start_x = FXSYS_round(mtPattern2Device.e) + col * width - clip_box.left;
        start_y = FXSYS_round(mtPattern2Device.f) + row * height - clip_box.top;
      } else {
        CFX_PointF original = mtPattern2Device.Transform(
            CFX_PointF(col * pPattern->x_step(), row * pPattern->y_step()));

        pdfium::base::CheckedNumeric<int> safeStartX =
            FXSYS_round(original.x + left_offset);
        pdfium::base::CheckedNumeric<int> safeStartY =
            FXSYS_round(original.y + top_offset);

        safeStartX -= clip_box.left;
        safeStartY -= clip_box.top;
        if (!safeStartX.IsValid() || !safeStartY.IsValid())
          return;

        start_x = safeStartX.ValueOrDie();
        start_y = safeStartY.ValueOrDie();
      }
      if (width == 1 && height == 1) {
        if (start_x < 0 || start_x >= clip_box.Width() || start_y < 0 ||
            start_y >= clip_box.Height()) {
          continue;
        }
        uint32_t* dest_buf =
            (uint32_t*)(pScreen->GetBuffer() + pScreen->GetPitch() * start_y +
                        start_x * 4);
        if (pPattern->colored())
          *dest_buf = *src_buf;
        else
          *dest_buf = (*(uint8_t*)src_buf << 24) | (fill_argb & 0xffffff);
      } else {
        if (pPattern->colored()) {
          pScreen->CompositeBitmap(start_x, start_y, width, height,
                                   pPatternBitmap, 0, 0, BlendMode::kNormal,
                                   nullptr, false);
        } else {
          pScreen->CompositeMask(start_x, start_y, width, height,
                                 pPatternBitmap, fill_argb, 0, 0,
                                 BlendMode::kNormal, nullptr, false, 0);
        }
      }
    }
  }
  CompositeDIBitmap(pScreen, clip_box.left, clip_box.top, 0, 255,
                    BlendMode::kNormal, CPDF_Transparency());
}

void CPDF_RenderStatus::DrawPathWithPattern(CPDF_PathObject* pPathObj,
                                            const CFX_Matrix& mtObj2Device,
                                            const CPDF_Color* pColor,
                                            bool bStroke) {
  CPDF_Pattern* pattern = pColor->GetPattern();
  if (!pattern)
    return;

  if (CPDF_TilingPattern* pTilingPattern = pattern->AsTilingPattern())
    DrawTilingPattern(pTilingPattern, pPathObj, mtObj2Device, bStroke);
  else if (CPDF_ShadingPattern* pShadingPattern = pattern->AsShadingPattern())
    DrawShadingPattern(pShadingPattern, pPathObj, mtObj2Device, bStroke);
}

void CPDF_RenderStatus::ProcessPathPattern(CPDF_PathObject* pPathObj,
                                           const CFX_Matrix& mtObj2Device,
                                           int* filltype,
                                           bool* bStroke) {
  ASSERT(filltype);
  ASSERT(bStroke);

  if (*filltype) {
    const CPDF_Color& FillColor = *pPathObj->m_ColorState.GetFillColor();
    if (FillColor.IsPattern()) {
      DrawPathWithPattern(pPathObj, mtObj2Device, &FillColor, false);
      *filltype = 0;
    }
  }
  if (*bStroke) {
    const CPDF_Color& StrokeColor = *pPathObj->m_ColorState.GetStrokeColor();
    if (StrokeColor.IsPattern()) {
      DrawPathWithPattern(pPathObj, mtObj2Device, &StrokeColor, true);
      *bStroke = false;
    }
  }
}

bool CPDF_RenderStatus::ProcessImage(CPDF_ImageObject* pImageObj,
                                     const CFX_Matrix& mtObj2Device) {
  CPDF_ImageRenderer render;
  if (render.Start(this, pImageObj, mtObj2Device, m_bStdCS, m_curBlend))
    render.Continue(nullptr);
  return render.GetResult();
}

void CPDF_RenderStatus::CompositeDIBitmap(
    const RetainPtr<CFX_DIBitmap>& pDIBitmap,
    int left,
    int top,
    FX_ARGB mask_argb,
    int bitmap_alpha,
    BlendMode blend_mode,
    const CPDF_Transparency& transparency) {
  if (!pDIBitmap)
    return;

  if (blend_mode == BlendMode::kNormal) {
    if (!pDIBitmap->IsAlphaMask()) {
      if (bitmap_alpha < 255) {
#ifdef _SKIA_SUPPORT_
        std::unique_ptr<CFX_ImageRenderer> dummy;
        CFX_Matrix m = CFX_RenderDevice::GetFlipMatrix(
            pDIBitmap->GetWidth(), pDIBitmap->GetHeight(), left, top);
        m_pDevice->StartDIBits(pDIBitmap, bitmap_alpha, 0, m,
                               FXDIB_ResampleOptions(), &dummy);
        return;
#else
        pDIBitmap->MultiplyAlpha(bitmap_alpha);
#endif
      }
#ifdef _SKIA_SUPPORT_
      CFX_SkiaDeviceDriver::PreMultiply(pDIBitmap);
#endif
      if (m_pDevice->SetDIBits(pDIBitmap, left, top)) {
        return;
      }
    } else {
      uint32_t fill_argb = m_Options.TranslateColor(mask_argb);
      if (bitmap_alpha < 255) {
        uint8_t* fill_argb8 = reinterpret_cast<uint8_t*>(&fill_argb);
        fill_argb8[3] *= bitmap_alpha / 255;
      }
      if (m_pDevice->SetBitMask(pDIBitmap, left, top, fill_argb)) {
        return;
      }
    }
  }
  bool bIsolated = transparency.IsIsolated();
  bool bBackAlphaRequired =
      blend_mode != BlendMode::kNormal && bIsolated && !m_bDropObjects;
  bool bGetBackGround =
      ((m_pDevice->GetRenderCaps() & FXRC_ALPHA_OUTPUT)) ||
      (!(m_pDevice->GetRenderCaps() & FXRC_ALPHA_OUTPUT) &&
       (m_pDevice->GetRenderCaps() & FXRC_GET_BITS) && !bBackAlphaRequired);
  if (bGetBackGround) {
    if (bIsolated || !transparency.IsGroup()) {
      if (!pDIBitmap->IsAlphaMask())
        m_pDevice->SetDIBitsWithBlend(pDIBitmap, left, top, blend_mode);
      return;
    }

    FX_RECT rect(left, top, left + pDIBitmap->GetWidth(),
                 top + pDIBitmap->GetHeight());
    rect.Intersect(m_pDevice->GetClipBox());
    RetainPtr<CFX_DIBitmap> pClone;
    if (m_pDevice->GetBackDrop() && m_pDevice->GetBitmap()) {
      pClone = m_pDevice->GetBackDrop()->Clone(&rect);
      if (!pClone)
        return;

      RetainPtr<CFX_DIBitmap> pForeBitmap = m_pDevice->GetBitmap();
      pClone->CompositeBitmap(0, 0, pClone->GetWidth(), pClone->GetHeight(),
                              pForeBitmap, rect.left, rect.top,
                              BlendMode::kNormal, nullptr, false);
      left = std::min(left, 0);
      top = std::min(top, 0);
      if (pDIBitmap->IsAlphaMask()) {
        pClone->CompositeMask(0, 0, pClone->GetWidth(), pClone->GetHeight(),
                              pDIBitmap, mask_argb, left, top, blend_mode,
                              nullptr, false, 0);
      } else {
        pClone->CompositeBitmap(0, 0, pClone->GetWidth(), pClone->GetHeight(),
                                pDIBitmap, left, top, blend_mode, nullptr,
                                false);
      }
    } else {
      pClone = pDIBitmap;
    }
    if (m_pDevice->GetBackDrop()) {
      m_pDevice->SetDIBits(pClone, rect.left, rect.top);
    } else {
      if (!pDIBitmap->IsAlphaMask()) {
        m_pDevice->SetDIBitsWithBlend(pDIBitmap, rect.left, rect.top,
                                      blend_mode);
      }
    }
    return;
  }
  int back_left;
  int back_top;
  FX_RECT rect(left, top, left + pDIBitmap->GetWidth(),
               top + pDIBitmap->GetHeight());
  RetainPtr<CFX_DIBitmap> pBackdrop = GetBackdrop(
      m_pCurObj.Get(), rect, blend_mode != BlendMode::kNormal && bIsolated,
      &back_left, &back_top);
  if (!pBackdrop)
    return;

  if (pDIBitmap->IsAlphaMask()) {
    pBackdrop->CompositeMask(left - back_left, top - back_top,
                             pDIBitmap->GetWidth(), pDIBitmap->GetHeight(),
                             pDIBitmap, mask_argb, 0, 0, blend_mode, nullptr,
                             false, 0);
  } else {
    pBackdrop->CompositeBitmap(left - back_left, top - back_top,
                               pDIBitmap->GetWidth(), pDIBitmap->GetHeight(),
                               pDIBitmap, 0, 0, blend_mode, nullptr, false);
  }

  auto pBackdrop1 = pdfium::MakeRetain<CFX_DIBitmap>();
  pBackdrop1->Create(pBackdrop->GetWidth(), pBackdrop->GetHeight(),
                     FXDIB_Rgb32);
  pBackdrop1->Clear((uint32_t)-1);
  pBackdrop1->CompositeBitmap(0, 0, pBackdrop->GetWidth(),
                              pBackdrop->GetHeight(), pBackdrop, 0, 0,
                              BlendMode::kNormal, nullptr, false);
  pBackdrop = std::move(pBackdrop1);
  m_pDevice->SetDIBits(pBackdrop, back_left, back_top);
}

RetainPtr<CFX_DIBitmap> CPDF_RenderStatus::LoadSMask(
    CPDF_Dictionary* pSMaskDict,
    FX_RECT* pClipRect,
    const CFX_Matrix* pMatrix) {
  if (!pSMaskDict)
    return nullptr;

  CPDF_Stream* pGroup = pSMaskDict->GetStreamFor(pdfium::transparency::kG);
  if (!pGroup)
    return nullptr;

  std::unique_ptr<CPDF_Function> pFunc;
  const CPDF_Object* pFuncObj =
      pSMaskDict->GetDirectObjectFor(pdfium::transparency::kTR);
  if (pFuncObj && (pFuncObj->IsDictionary() || pFuncObj->IsStream()))
    pFunc = CPDF_Function::Load(pFuncObj);

  CFX_Matrix matrix = *pMatrix;
  matrix.Translate(-pClipRect->left, -pClipRect->top);

  CPDF_Form form(m_pContext->GetDocument(), m_pContext->GetPageResources(),
                 pGroup);
  form.ParseContent(nullptr, nullptr, nullptr, nullptr);

  CFX_DefaultRenderDevice bitmap_device;
  bool bLuminosity =
      pSMaskDict->GetStringFor(pdfium::transparency::kSoftMaskSubType) !=
      pdfium::transparency::kAlpha;
  int width = pClipRect->right - pClipRect->left;
  int height = pClipRect->bottom - pClipRect->top;
  FXDIB_Format format;
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_ || defined _SKIA_SUPPORT_ || \
    defined _SKIA_SUPPORT_PATHS_
  format = bLuminosity ? FXDIB_Rgb32 : FXDIB_8bppMask;
#else
  format = bLuminosity ? FXDIB_Rgb : FXDIB_8bppMask;
#endif
  if (!bitmap_device.Create(width, height, format, nullptr))
    return nullptr;

  CFX_DIBitmap& bitmap = *bitmap_device.GetBitmap();
  int nCSFamily = 0;
  if (bLuminosity) {
    FX_ARGB back_color =
        GetBackColor(pSMaskDict, pGroup->GetDict(), &nCSFamily);
    bitmap.Clear(back_color);
  } else {
    bitmap.Clear(0);
  }

  const CPDF_Dictionary* pFormResource = nullptr;
  if (form.GetDict())
    pFormResource = form.GetDict()->GetDictFor("Resources");
  CPDF_RenderOptions options;
  options.SetColorMode(bLuminosity ? CPDF_RenderOptions::kNormal
                                   : CPDF_RenderOptions::kAlpha);
  CPDF_RenderStatus status(m_pContext.Get(), &bitmap_device);
  status.SetOptions(options);
  status.SetGroupFamily(nCSFamily);
  status.SetLoadMask(bLuminosity);
  status.SetStdCS(true);
  status.SetFormResource(pFormResource);
  status.SetDropObjects(m_bDropObjects);
  status.Initialize(nullptr, nullptr);
  status.RenderObjectList(&form, matrix);

  auto pMask = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!pMask->Create(width, height, FXDIB_8bppMask))
    return nullptr;

  uint8_t* dest_buf = pMask->GetBuffer();
  int dest_pitch = pMask->GetPitch();
  uint8_t* src_buf = bitmap.GetBuffer();
  int src_pitch = bitmap.GetPitch();
  std::vector<uint8_t> transfers(256);
  if (pFunc) {
    std::vector<float> results(pFunc->CountOutputs());
    for (int i = 0; i < 256; i++) {
      float input = (float)i / 255.0f;
      int nresult;
      pFunc->Call(&input, 1, results.data(), &nresult);
      transfers[i] = FXSYS_round(results[0] * 255);
    }
  } else {
    for (int i = 0; i < 256; i++) {
      transfers[i] = i;
    }
  }
  if (bLuminosity) {
    int Bpp = bitmap.GetBPP() / 8;
    for (int row = 0; row < height; row++) {
      uint8_t* dest_pos = dest_buf + row * dest_pitch;
      uint8_t* src_pos = src_buf + row * src_pitch;
      for (int col = 0; col < width; col++) {
        *dest_pos++ = transfers[FXRGB2GRAY(src_pos[2], src_pos[1], *src_pos)];
        src_pos += Bpp;
      }
    }
  } else if (pFunc) {
    int size = dest_pitch * height;
    for (int i = 0; i < size; i++) {
      dest_buf[i] = transfers[src_buf[i]];
    }
  } else {
    memcpy(dest_buf, src_buf, dest_pitch * height);
  }
  return pMask;
}

FX_ARGB CPDF_RenderStatus::GetBackColor(const CPDF_Dictionary* pSMaskDict,
                                        const CPDF_Dictionary* pGroupDict,
                                        int* pCSFamily) {
  static constexpr FX_ARGB kDefaultColor = ArgbEncode(255, 0, 0, 0);
  const CPDF_Array* pBC = pSMaskDict->GetArrayFor(pdfium::transparency::kBC);
  if (!pBC)
    return kDefaultColor;

  const CPDF_Object* pCSObj = nullptr;
  const CPDF_Dictionary* pGroup =
      pGroupDict ? pGroupDict->GetDictFor("Group") : nullptr;
  if (pGroup)
    pCSObj = pGroup->GetDirectObjectFor(pdfium::transparency::kCS);
  const CPDF_ColorSpace* pCS =
      m_pContext->GetDocument()->LoadColorSpace(pCSObj, nullptr);
  if (!pCS)
    return kDefaultColor;

  int family = pCS->GetFamily();
  if (family == PDFCS_LAB || pCS->IsSpecial() ||
      (family == PDFCS_ICCBASED && !pCS->IsNormal())) {
    return kDefaultColor;
  }

  // Store Color Space Family to use in CPDF_RenderStatus::Initialize().
  *pCSFamily = family;

  uint32_t comps = std::max(8u, pCS->CountComponents());
  std::vector<float> floats(comps);
  size_t count = std::min<size_t>(8, pBC->size());
  for (size_t i = 0; i < count; i++)
    floats[i] = pBC->GetNumberAt(i);

  float R;
  float G;
  float B;
  pCS->GetRGB(floats.data(), &R, &G, &B);
  m_pContext->GetDocument()->GetPageData()->ReleaseColorSpace(pCSObj);
  return ArgbEncode(255, static_cast<int>(R * 255), static_cast<int>(G * 255),
                    static_cast<int>(B * 255));
}
