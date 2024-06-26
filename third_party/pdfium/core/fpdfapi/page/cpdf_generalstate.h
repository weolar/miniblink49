// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_GENERALSTATE_H_
#define CORE_FPDFAPI_PAGE_CPDF_GENERALSTATE_H_

#include "constants/transparency.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/shared_copy_on_write.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/fx_dib.h"

class CPDF_Object;
class CPDF_TransferFunc;

class CPDF_GeneralState {
 public:
  CPDF_GeneralState();
  CPDF_GeneralState(const CPDF_GeneralState& that);
  ~CPDF_GeneralState();

  void Emplace() { m_Ref.Emplace(); }
  bool HasRef() const { return !!m_Ref; }

  void SetRenderIntent(const ByteString& ri);

  ByteString GetBlendMode() const;
  BlendMode GetBlendType() const;
  void SetBlendType(BlendMode type);

  float GetFillAlpha() const;
  void SetFillAlpha(float alpha);

  float GetStrokeAlpha() const;
  void SetStrokeAlpha(float alpha);

  CPDF_Object* GetSoftMask() const;
  void SetSoftMask(CPDF_Object* pObject);

  const CPDF_Object* GetTR() const;
  void SetTR(CPDF_Object* pObject);

  RetainPtr<CPDF_TransferFunc> GetTransferFunc() const;
  void SetTransferFunc(const RetainPtr<CPDF_TransferFunc>& pFunc);

  void SetBlendMode(const ByteString& mode);

  const CFX_Matrix* GetSMaskMatrix() const;
  void SetSMaskMatrix(const CFX_Matrix& matrix);

  bool GetFillOP() const;
  void SetFillOP(bool op);

  bool GetStrokeOP() const;
  void SetStrokeOP(bool op);

  int GetOPMode() const;
  void SetOPMode(int mode);

  void SetBG(CPDF_Object* pObject);
  void SetUCR(CPDF_Object* pObject);
  void SetHT(CPDF_Object* pObject);

  void SetFlatness(float flatness);
  void SetSmoothness(float smoothness);

  bool GetStrokeAdjust() const;
  void SetStrokeAdjust(bool adjust);

  void SetAlphaSource(bool source);
  void SetTextKnockout(bool knockout);

  void SetMatrix(const CFX_Matrix& matrix);
  CFX_Matrix* GetMutableMatrix();

 private:
  class StateData final : public Retainable {
   public:
    StateData();
    StateData(const StateData& that);
    ~StateData() override;

    ByteString m_BlendMode = pdfium::transparency::kNormal;
    BlendMode m_BlendType = BlendMode::kNormal;
    UnownedPtr<CPDF_Object> m_pSoftMask;
    CFX_Matrix m_SMaskMatrix;
    float m_StrokeAlpha = 1.0f;
    float m_FillAlpha = 1.0f;
    UnownedPtr<const CPDF_Object> m_pTR;
    RetainPtr<CPDF_TransferFunc> m_pTransferFunc;
    CFX_Matrix m_Matrix;
    int m_RenderIntent = 0;
    bool m_StrokeAdjust = false;
    bool m_AlphaSource = false;
    bool m_TextKnockout = false;
    bool m_StrokeOP = false;
    bool m_FillOP = false;
    int m_OPMode = 0;
    UnownedPtr<const CPDF_Object> m_pBG;
    UnownedPtr<const CPDF_Object> m_pUCR;
    UnownedPtr<const CPDF_Object> m_pHT;
    float m_Flatness = 1.0f;
    float m_Smoothness = 0.0f;
  };

  SharedCopyOnWrite<StateData> m_Ref;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_GENERALSTATE_H_
