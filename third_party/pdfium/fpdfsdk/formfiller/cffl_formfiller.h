// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FORMFILLER_CFFL_FORMFILLER_H_
#define FPDFSDK_FORMFILLER_CFFL_FORMFILLER_H_

#include <map>
#include <memory>

#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/cpdfsdk_fieldaction.h"
#include "fpdfsdk/formfiller/cba_fontmap.h"
#include "fpdfsdk/formfiller/cffl_interactiveformfiller.h"

class CPDFSDK_Annot;
class CPDFSDK_FormFillEnvironment;
class CPDFSDK_PageView;
class CPDFSDK_Widget;

class CFFL_FormFiller : public CPWL_Wnd::ProviderIface,
                        public CPWL_TimerHandler {
 public:
  CFFL_FormFiller(CPDFSDK_FormFillEnvironment* pFormFillEnv,
                  CPDFSDK_Widget* pWidget);
  ~CFFL_FormFiller() override;

  virtual FX_RECT GetViewBBox(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot);
  virtual void OnDraw(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      CFX_RenderDevice* pDevice,
                      const CFX_Matrix& mtUser2Device);
  virtual void OnDrawDeactive(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot,
                              CFX_RenderDevice* pDevice,
                              const CFX_Matrix& mtUser2Device);

  virtual void OnMouseEnter(CPDFSDK_PageView* pPageView, CPDFSDK_Annot* pAnnot);
  virtual void OnMouseExit(CPDFSDK_PageView* pPageView, CPDFSDK_Annot* pAnnot);

  virtual bool OnLButtonDown(CPDFSDK_PageView* pPageView,
                             CPDFSDK_Annot* pAnnot,
                             uint32_t nFlags,
                             const CFX_PointF& point);
  virtual bool OnLButtonUp(CPDFSDK_PageView* pPageView,
                           CPDFSDK_Annot* pAnnot,
                           uint32_t nFlags,
                           const CFX_PointF& point);
  virtual bool OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                               CPDFSDK_Annot* pAnnot,
                               uint32_t nFlags,
                               const CFX_PointF& point);
  virtual bool OnMouseMove(CPDFSDK_PageView* pPageView,
                           CPDFSDK_Annot* pAnnot,
                           uint32_t nFlags,
                           const CFX_PointF& point);
  virtual bool OnMouseWheel(CPDFSDK_PageView* pPageView,
                            CPDFSDK_Annot* pAnnot,
                            uint32_t nFlags,
                            short zDelta,
                            const CFX_PointF& point);
  virtual bool OnRButtonDown(CPDFSDK_PageView* pPageView,
                             CPDFSDK_Annot* pAnnot,
                             uint32_t nFlags,
                             const CFX_PointF& point);
  virtual bool OnRButtonUp(CPDFSDK_PageView* pPageView,
                           CPDFSDK_Annot* pAnnot,
                           uint32_t nFlags,
                           const CFX_PointF& point);

  virtual bool OnKeyDown(CPDFSDK_Annot* pAnnot,
                         uint32_t nKeyCode,
                         uint32_t nFlags);
  virtual bool OnChar(CPDFSDK_Annot* pAnnot, uint32_t nChar, uint32_t nFlags);

  WideString GetText(CPDFSDK_Annot* pAnnot);
  WideString GetSelectedText(CPDFSDK_Annot* pAnnot);
  void ReplaceSelection(CPDFSDK_Annot* pAnnot, const WideString& text);

  bool CanUndo(CPDFSDK_Annot* pAnnot);
  bool CanRedo(CPDFSDK_Annot* pAnnot);
  bool Undo(CPDFSDK_Annot* pAnnot);
  bool Redo(CPDFSDK_Annot* pAnnot);

  void SetFocusForAnnot(CPDFSDK_Annot* pAnnot, uint32_t nFlag);
  void KillFocusForAnnot(CPDFSDK_Annot* pAnnot, uint32_t nFlag);

  // CPWL_TimerHandler
  void TimerProc() override;
  CFX_SystemHandler* GetSystemHandler() const override;

  // CPWL_Wnd::ProviderIface:
  CFX_Matrix GetWindowMatrix(const CPWL_Wnd::PrivateData* pAttached) override;

  virtual void GetActionData(CPDFSDK_PageView* pPageView,
                             CPDF_AAction::AActionType type,
                             CPDFSDK_FieldAction& fa);
  virtual void SetActionData(CPDFSDK_PageView* pPageView,
                             CPDF_AAction::AActionType type,
                             const CPDFSDK_FieldAction& fa);
  virtual bool IsActionDataChanged(CPDF_AAction::AActionType type,
                                   const CPDFSDK_FieldAction& faOld,
                                   const CPDFSDK_FieldAction& faNew);

  virtual CPWL_Wnd::CreateParams GetCreateParam();
  virtual std::unique_ptr<CPWL_Wnd> NewPWLWindow(
      const CPWL_Wnd::CreateParams& cp,
      std::unique_ptr<CPWL_Wnd::PrivateData> pAttachedData) = 0;
  virtual CPWL_Wnd* ResetPDFWindow(CPDFSDK_PageView* pPageView,
                                   bool bRestoreValue);
  virtual void SaveState(CPDFSDK_PageView* pPageView);
  virtual void RestoreState(CPDFSDK_PageView* pPageView);
  virtual CFX_FloatRect GetFocusBox(CPDFSDK_PageView* pPageView);

  CFX_Matrix GetCurMatrix();
  CFX_FloatRect FFLtoPWL(const CFX_FloatRect& rect);
  CFX_FloatRect PWLtoFFL(const CFX_FloatRect& rect);
  CFX_PointF FFLtoPWL(const CFX_PointF& point);
  CFX_PointF PWLtoFFL(const CFX_PointF& point);
  CFX_PointF WndtoPWL(CPDFSDK_PageView* pPageView, const CFX_PointF& pt);
  CFX_FloatRect FFLtoWnd(CPDFSDK_PageView* pPageView,
                         const CFX_FloatRect& rect);

  bool CommitData(CPDFSDK_PageView* pPageView, uint32_t nFlag);
  virtual bool IsDataChanged(CPDFSDK_PageView* pPageView);
  virtual void SaveData(CPDFSDK_PageView* pPageView);

#ifdef PDF_ENABLE_XFA
  virtual bool IsFieldFull(CPDFSDK_PageView* pPageView);
#endif  // PDF_ENABLE_XFA

  CPWL_Wnd* GetPDFWindow(CPDFSDK_PageView* pPageView, bool bNew);
  void DestroyPDFWindow(CPDFSDK_PageView* pPageView);
  void EscapeFiller(CPDFSDK_PageView* pPageView, bool bDestroyPDFWindow);


  bool IsValid() const;
  CFX_FloatRect GetPDFWindowRect() const;

  CPDFSDK_PageView* GetCurPageView(bool renew);
  void SetChangeMark();

  CPDFSDK_Annot* GetSDKAnnot() const { return m_pWidget.Get(); }

 protected:
  // If the inheriting widget has its own fontmap and a PWL_Edit widget that
  // access that fontmap then you have to call DestroyWindows before destroying
  // the font map in order to not get a use-after-free.
  //
  // The font map should be stored somewhere more appropriate so it will live
  // until the PWL_Edit is done with it. pdfium:566
  void DestroyWindows();

  void InvalidateRect(const FX_RECT& rect);

  bool m_bValid = false;
  UnownedPtr<CPDFSDK_FormFillEnvironment> const m_pFormFillEnv;
  UnownedPtr<CPDFSDK_Widget> m_pWidget;
  std::map<CPDFSDK_PageView*, std::unique_ptr<CPWL_Wnd>> m_Maps;
};

#endif  // FPDFSDK_FORMFILLER_CFFL_FORMFILLER_H_
