// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/fpdf_edit.h"

#include <utility>
#include <vector>

#include "core/fpdfapi/page/cpdf_path.h"
#include "core/fpdfapi/page/cpdf_pathobject.h"
#include "core/fxcrt/fx_system.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

// These checks are here because core/ and public/ cannot depend on each other.
static_assert(CFX_GraphStateData::LineCapButt == FPDF_LINECAP_BUTT,
              "CFX_GraphStateData::LineCapButt value mismatch");
static_assert(CFX_GraphStateData::LineCapRound == FPDF_LINECAP_ROUND,
              "CFX_GraphStateData::LineCapRound value mismatch");
static_assert(CFX_GraphStateData::LineCapSquare ==
                  FPDF_LINECAP_PROJECTING_SQUARE,
              "CFX_GraphStateData::LineCapSquare value mismatch");

static_assert(CFX_GraphStateData::LineJoinMiter == FPDF_LINEJOIN_MITER,
              "CFX_GraphStateData::LineJoinMiter value mismatch");
static_assert(CFX_GraphStateData::LineJoinRound == FPDF_LINEJOIN_ROUND,
              "CFX_GraphStateData::LineJoinRound value mismatch");
static_assert(CFX_GraphStateData::LineJoinBevel == FPDF_LINEJOIN_BEVEL,
              "CFX_GraphStateData::LineJoinBevel value mismatch");

static_assert(static_cast<int>(FXPT_TYPE::LineTo) == FPDF_SEGMENT_LINETO,
              "FXPT_TYPE::LineTo value mismatch");
static_assert(static_cast<int>(FXPT_TYPE::BezierTo) == FPDF_SEGMENT_BEZIERTO,
              "FXPT_TYPE::BezierTo value mismatch");
static_assert(static_cast<int>(FXPT_TYPE::MoveTo) == FPDF_SEGMENT_MOVETO,
              "FXPT_TYPE::MoveTo value mismatch");

namespace {

CPDF_PathObject* CPDFPathObjectFromFPDFPageObject(FPDF_PAGEOBJECT page_object) {
  auto* obj = CPDFPageObjectFromFPDFPageObject(page_object);
  return obj ? obj->AsPath() : nullptr;
}

}  // namespace

FPDF_EXPORT FPDF_PAGEOBJECT FPDF_CALLCONV FPDFPageObj_CreateNewPath(float x,
                                                                    float y) {
  auto pPathObj = pdfium::MakeUnique<CPDF_PathObject>();
  pPathObj->path().AppendPoint(CFX_PointF(x, y), FXPT_TYPE::MoveTo, false);
  pPathObj->DefaultStates();

  // Caller takes ownership.
  return FPDFPageObjectFromCPDFPageObject(pPathObj.release());
}

FPDF_EXPORT FPDF_PAGEOBJECT FPDF_CALLCONV FPDFPageObj_CreateNewRect(float x,
                                                                    float y,
                                                                    float w,
                                                                    float h) {
  auto pPathObj = pdfium::MakeUnique<CPDF_PathObject>();
  pPathObj->path().AppendRect(x, y, x + w, y + h);
  pPathObj->DefaultStates();

  // Caller takes ownership.
  return FPDFPageObjectFromCPDFPageObject(pPathObj.release());
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFPath_SetStrokeColor(FPDF_PAGEOBJECT path,
                        unsigned int R,
                        unsigned int G,
                        unsigned int B,
                        unsigned int A) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  return FPDFPageObj_SetStrokeColor(path, R, G, B, A);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFPath_GetStrokeColor(FPDF_PAGEOBJECT path,
                        unsigned int* R,
                        unsigned int* G,
                        unsigned int* B,
                        unsigned int* A) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  return FPDFPageObj_GetStrokeColor(path, R, G, B, A);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFPath_SetStrokeWidth(FPDF_PAGEOBJECT path, float width) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  return FPDFPageObj_SetStrokeWidth(path, width);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFPath_SetFillColor(FPDF_PAGEOBJECT path,
                                                          unsigned int R,
                                                          unsigned int G,
                                                          unsigned int B,
                                                          unsigned int A) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  return FPDFPageObj_SetFillColor(path, R, G, B, A);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFPath_GetFillColor(FPDF_PAGEOBJECT path,
                                                          unsigned int* R,
                                                          unsigned int* G,
                                                          unsigned int* B,
                                                          unsigned int* A) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  return FPDFPageObj_GetFillColor(path, R, G, B, A);
}

FPDF_EXPORT int FPDF_CALLCONV FPDFPath_CountSegments(FPDF_PAGEOBJECT path) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return -1;
  return pdfium::CollectionSize<int>(pPathObj->path().GetPoints());
}

FPDF_EXPORT FPDF_PATHSEGMENT FPDF_CALLCONV
FPDFPath_GetPathSegment(FPDF_PAGEOBJECT path, int index) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return nullptr;

  const std::vector<FX_PATHPOINT>& points = pPathObj->path().GetPoints();
  if (!pdfium::IndexInBounds(points, index))
    return nullptr;

  return FPDFPathSegmentFromFXPathPoint(&points[index]);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFPath_MoveTo(FPDF_PAGEOBJECT path,
                                                    float x,
                                                    float y) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  pPathObj->path().AppendPoint(CFX_PointF(x, y), FXPT_TYPE::MoveTo, false);
  pPathObj->SetDirty(true);
  return true;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFPath_LineTo(FPDF_PAGEOBJECT path,
                                                    float x,
                                                    float y) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  pPathObj->path().AppendPoint(CFX_PointF(x, y), FXPT_TYPE::LineTo, false);
  pPathObj->SetDirty(true);
  return true;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFPath_BezierTo(FPDF_PAGEOBJECT path,
                                                      float x1,
                                                      float y1,
                                                      float x2,
                                                      float y2,
                                                      float x3,
                                                      float y3) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  CPDF_Path& cpath = pPathObj->path();
  cpath.AppendPoint(CFX_PointF(x1, y1), FXPT_TYPE::BezierTo, false);
  cpath.AppendPoint(CFX_PointF(x2, y2), FXPT_TYPE::BezierTo, false);
  cpath.AppendPoint(CFX_PointF(x3, y3), FXPT_TYPE::BezierTo, false);
  pPathObj->SetDirty(true);
  return true;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFPath_Close(FPDF_PAGEOBJECT path) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  CPDF_Path& cpath = pPathObj->path();
  if (cpath.GetPoints().empty())
    return false;

  cpath.ClosePath();
  pPathObj->SetDirty(true);
  return true;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFPath_SetDrawMode(FPDF_PAGEOBJECT path,
                                                         int fillmode,
                                                         FPDF_BOOL stroke) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  pPathObj->set_stroke(!!stroke);
  if (fillmode == FPDF_FILLMODE_ALTERNATE)
    pPathObj->set_filltype(FXFILL_ALTERNATE);
  else if (fillmode == FPDF_FILLMODE_WINDING)
    pPathObj->set_filltype(FXFILL_WINDING);
  else
    pPathObj->set_filltype(0);
  pPathObj->SetDirty(true);
  return true;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFPath_GetDrawMode(FPDF_PAGEOBJECT path,
                                                         int* fillmode,
                                                         FPDF_BOOL* stroke) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj || !fillmode || !stroke)
    return false;

  if (pPathObj->filltype() == FXFILL_ALTERNATE)
    *fillmode = FPDF_FILLMODE_ALTERNATE;
  else if (pPathObj->filltype() == FXFILL_WINDING)
    *fillmode = FPDF_FILLMODE_WINDING;
  else
    *fillmode = FPDF_FILLMODE_NONE;

  *stroke = pPathObj->stroke();
  return true;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFPath_GetMatrix(FPDF_PAGEOBJECT path,
                                                       double* a,
                                                       double* b,
                                                       double* c,
                                                       double* d,
                                                       double* e,
                                                       double* f) {
  if (!path || !a || !b || !c || !d || !e || !f)
    return false;

  CPDF_PathObject* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  std::tie(*a, *b, *c, *d, *e, *f) = pPathObj->matrix().AsTuple();
  return true;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFPath_SetMatrix(FPDF_PAGEOBJECT path,
                                                       double a,
                                                       double b,
                                                       double c,
                                                       double d,
                                                       double e,
                                                       double f) {
  CPDF_PathObject* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return false;

  pPathObj->set_matrix(CFX_Matrix(a, b, c, d, e, f));
  pPathObj->SetDirty(true);
  return true;
}

FPDF_EXPORT void FPDF_CALLCONV FPDFPath_SetLineJoin(FPDF_PAGEOBJECT path,
                                                    int line_join) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return;

  FPDFPageObj_SetLineJoin(path, line_join);
}

FPDF_EXPORT void FPDF_CALLCONV FPDFPath_SetLineCap(FPDF_PAGEOBJECT path,
                                                   int line_cap) {
  auto* pPathObj = CPDFPathObjectFromFPDFPageObject(path);
  if (!pPathObj)
    return;

  FPDFPageObj_SetLineCap(path, line_cap);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFPathSegment_GetPoint(FPDF_PATHSEGMENT segment, float* x, float* y) {
  auto* pPathPoint = FXPathPointFromFPDFPathSegment(segment);
  if (!pPathPoint || !x || !y)
    return false;

  *x = pPathPoint->m_Point.x;
  *y = pPathPoint->m_Point.y;
  return true;
}

FPDF_EXPORT int FPDF_CALLCONV
FPDFPathSegment_GetType(FPDF_PATHSEGMENT segment) {
  auto* pPathPoint = FXPathPointFromFPDFPathSegment(segment);
  return pPathPoint ? static_cast<int>(pPathPoint->m_Type)
                    : FPDF_SEGMENT_UNKNOWN;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFPathSegment_GetClose(FPDF_PATHSEGMENT segment) {
  auto* pPathPoint = FXPathPointFromFPDFPathSegment(segment);
  return pPathPoint ? pPathPoint->m_CloseFigure : false;
}
