// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FPDFAPI_RENDER_CPDF_TRANSPARENCY_H_
#define CORE_FPDFAPI_RENDER_CPDF_TRANSPARENCY_H_

class CPDF_Transparency {
 public:
  CPDF_Transparency();

  CPDF_Transparency(const CPDF_Transparency& other);

  bool IsGroup() const { return m_bGroup; }
  bool IsIsolated() const { return m_bIsolated; }

  void SetGroup() { m_bGroup = true; }
  void SetIsolated() { m_bIsolated = true; }

 private:
  bool m_bGroup = false;
  bool m_bIsolated = false;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_TRANSPARENCY_H_
