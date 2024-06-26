// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_dest.h"

#include <algorithm>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"

namespace {

// These arrays are indexed by the PDFDEST_VIEW_* constants.

// Last element is a sentinel.
const char* const g_sZoomModes[] = {"Unknown", "XYZ",  "Fit",  "FitH",
                                    "FitV",    "FitR", "FitB", "FitBH",
                                    "FitBV",   nullptr};

const uint8_t g_sZoomModeMaxParamCount[] = {0, 3, 0, 1, 1, 4, 0, 1, 1, 0};

static_assert(FX_ArraySize(g_sZoomModes) ==
                  FX_ArraySize(g_sZoomModeMaxParamCount),
              "Zoom mode count Mismatch");

}  // namespace

CPDF_Dest::CPDF_Dest() {}

CPDF_Dest::CPDF_Dest(const CPDF_Dest& that) = default;

CPDF_Dest::CPDF_Dest(const CPDF_Array* pArray) : m_pArray(pArray) {}

CPDF_Dest::~CPDF_Dest() {}

int CPDF_Dest::GetDestPageIndex(CPDF_Document* pDoc) const {
  if (!m_pArray)
    return -1;

  const CPDF_Object* pPage = m_pArray->GetDirectObjectAt(0);
  if (!pPage)
    return -1;

  if (pPage->IsNumber())
    return pPage->GetInteger();

  if (!pPage->IsDictionary())
    return -1;

  return pDoc->GetPageIndex(pPage->GetObjNum());
}

int CPDF_Dest::GetZoomMode() const {
  if (!m_pArray)
    return 0;

  const CPDF_Object* pArray = m_pArray->GetDirectObjectAt(1);
  if (!pArray)
    return 0;

  ByteString mode = pArray->GetString();
  for (int i = 1; g_sZoomModes[i]; ++i) {
    if (mode == g_sZoomModes[i])
      return i;
  }

  return 0;
}

bool CPDF_Dest::GetXYZ(bool* pHasX,
                       bool* pHasY,
                       bool* pHasZoom,
                       float* pX,
                       float* pY,
                       float* pZoom) const {
  *pHasX = false;
  *pHasY = false;
  *pHasZoom = false;

  if (!m_pArray)
    return false;

  if (m_pArray->size() < 5)
    return false;

  const CPDF_Name* xyz = ToName(m_pArray->GetDirectObjectAt(1));
  if (!xyz || xyz->GetString() != "XYZ")
    return false;

  const CPDF_Number* numX = ToNumber(m_pArray->GetDirectObjectAt(2));
  const CPDF_Number* numY = ToNumber(m_pArray->GetDirectObjectAt(3));
  const CPDF_Number* numZoom = ToNumber(m_pArray->GetDirectObjectAt(4));

  // If the value is a CPDF_Null then ToNumber will return nullptr.
  *pHasX = !!numX;
  *pHasY = !!numY;
  *pHasZoom = !!numZoom;

  if (numX)
    *pX = numX->GetNumber();
  if (numY)
    *pY = numY->GetNumber();

  // A zoom value of 0 is equivalent to a null value, so treat it as a null.
  if (numZoom) {
    float num = numZoom->GetNumber();
    if (num == 0.0)
      *pHasZoom = false;
    else
      *pZoom = num;
  }

  return true;
}

unsigned long CPDF_Dest::GetNumParams() const {
  if (!m_pArray || m_pArray->size() < 2)
    return 0;

  unsigned long maxParamsForFitType = g_sZoomModeMaxParamCount[GetZoomMode()];
  unsigned long numParamsInArray = m_pArray->size() - 2;
  return std::min(maxParamsForFitType, numParamsInArray);
}

float CPDF_Dest::GetParam(int index) const {
  return m_pArray ? m_pArray->GetNumberAt(2 + index) : 0;
}

ByteString CPDF_Dest::GetRemoteName() const {
  return m_pArray ? m_pArray->GetString() : ByteString();
}
