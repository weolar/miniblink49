// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_ffwidget.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fxcodec/codec/ccodec_progressivedecoder.h"
#include "core/fxcodec/fx_codec.h"
#include "core/fxcrt/maybe_owned.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "xfa/fwl/fwl_widgethit.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_imagerenderer.h"
#include "xfa/fxfa/parser/cxfa_border.h"
#include "xfa/fxfa/parser/cxfa_box.h"
#include "xfa/fxfa/parser/cxfa_image.h"
#include "xfa/fxfa/parser/cxfa_layoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_margin.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxgraphics/cxfa_graphics.h"

namespace {

FXDIB_Format XFA_GetDIBFormat(FXCODEC_IMAGE_TYPE type,
                              int32_t iComponents,
                              int32_t iBitsPerComponent) {
  FXDIB_Format dibFormat = FXDIB_Argb;
  switch (type) {
    case FXCODEC_IMAGE_JPG:
#ifdef PDF_ENABLE_XFA_BMP
    case FXCODEC_IMAGE_BMP:
#endif  // PDF_ENABLE_XFA_BMP
#ifdef PDF_ENABLE_XFA_TIFF
    case FXCODEC_IMAGE_TIFF:
#endif  // PDF_ENABLE_XFA_TIFF
    {
      dibFormat = FXDIB_Rgb32;
      int32_t bpp = iComponents * iBitsPerComponent;
      if (bpp <= 24) {
        dibFormat = FXDIB_Rgb;
      }
    } break;
#ifdef PDF_ENABLE_XFA_PNG
    case FXCODEC_IMAGE_PNG:
#endif  // PDF_ENABLE_XFA_PNG
    default:
      break;
  }
  return dibFormat;
}

bool IsFXCodecErrorStatus(FXCODEC_STATUS status) {
  return (status == FXCODEC_STATUS_ERROR ||
          status == FXCODEC_STATUS_ERR_MEMORY ||
          status == FXCODEC_STATUS_ERR_READ ||
          status == FXCODEC_STATUS_ERR_FLUSH ||
          status == FXCODEC_STATUS_ERR_FORMAT ||
          status == FXCODEC_STATUS_ERR_PARAMS);
}

}  // namespace

void XFA_DrawImage(CXFA_Graphics* pGS,
                   const CFX_RectF& rtImage,
                   const CFX_Matrix& matrix,
                   const RetainPtr<CFX_DIBitmap>& pDIBitmap,
                   XFA_AttributeValue iAspect,
                   const CFX_Size& dpi,
                   XFA_AttributeValue iHorzAlign,
                   XFA_AttributeValue iVertAlign) {
  if (rtImage.IsEmpty())
    return;
  if (!pDIBitmap || !pDIBitmap->GetBuffer())
    return;

  CFX_RectF rtFit(rtImage.TopLeft(),
                  XFA_UnitPx2Pt(pDIBitmap->GetWidth(), dpi.width),
                  XFA_UnitPx2Pt(pDIBitmap->GetHeight(), dpi.height));
  switch (iAspect) {
    case XFA_AttributeValue::Fit: {
      float f1 = rtImage.height / rtFit.height;
      float f2 = rtImage.width / rtFit.width;
      f1 = std::min(f1, f2);
      rtFit.height = rtFit.height * f1;
      rtFit.width = rtFit.width * f1;
      break;
    }
    case XFA_AttributeValue::Height: {
      float f1 = rtImage.height / rtFit.height;
      rtFit.height = rtImage.height;
      rtFit.width = f1 * rtFit.width;
      break;
    }
    case XFA_AttributeValue::None:
      rtFit.height = rtImage.height;
      rtFit.width = rtImage.width;
      break;
    case XFA_AttributeValue::Width: {
      float f1 = rtImage.width / rtFit.width;
      rtFit.width = rtImage.width;
      rtFit.height = rtFit.height * f1;
      break;
    }
    case XFA_AttributeValue::Actual:
    default:
      break;
  }

  if (iHorzAlign == XFA_AttributeValue::Center)
    rtFit.left += (rtImage.width - rtFit.width) / 2;
  else if (iHorzAlign == XFA_AttributeValue::Right)
    rtFit.left = rtImage.right() - rtFit.width;

  if (iVertAlign == XFA_AttributeValue::Middle)
    rtFit.top += (rtImage.height - rtFit.height) / 2;
  else if (iVertAlign == XFA_AttributeValue::Bottom)
    rtFit.top = rtImage.bottom() - rtImage.height;

  CFX_RenderDevice* pRenderDevice = pGS->GetRenderDevice();
  CFX_RenderDevice::StateRestorer restorer(pRenderDevice);
  CFX_PathData path;
  path.AppendRect(rtImage.left, rtImage.bottom(), rtImage.right(), rtImage.top);
  pRenderDevice->SetClip_PathFill(&path, &matrix, FXFILL_WINDING);

  CFX_Matrix mtImage(1, 0, 0, -1, 0, 1);
  mtImage.Concat(
      CFX_Matrix(rtFit.width, 0, 0, rtFit.height, rtFit.left, rtFit.top));
  mtImage.Concat(matrix);

  CXFA_ImageRenderer imageRender(pRenderDevice, pDIBitmap, &mtImage);
  if (!imageRender.Start()) {
    return;
  }
  while (imageRender.Continue())
    continue;
}

RetainPtr<CFX_DIBitmap> XFA_LoadImageFromBuffer(
    const RetainPtr<IFX_SeekableReadStream>& pImageFileRead,
    FXCODEC_IMAGE_TYPE type,
    int32_t& iImageXDpi,
    int32_t& iImageYDpi) {
  CCodec_ModuleMgr* pCodecMgr = CPDF_ModuleMgr::Get()->GetCodecModule();
  std::unique_ptr<CCodec_ProgressiveDecoder> pProgressiveDecoder =
      pCodecMgr->CreateProgressiveDecoder();

  CFX_DIBAttribute dibAttr;
  pProgressiveDecoder->LoadImageInfo(pImageFileRead, type, &dibAttr, false);
  switch (dibAttr.m_wDPIUnit) {
    case FXCODEC_RESUNIT_CENTIMETER:
      dibAttr.m_nXDPI = (int32_t)(dibAttr.m_nXDPI * 2.54f);
      dibAttr.m_nYDPI = (int32_t)(dibAttr.m_nYDPI * 2.54f);
      break;
    case FXCODEC_RESUNIT_METER:
      dibAttr.m_nXDPI = (int32_t)(dibAttr.m_nXDPI / (float)100 * 2.54f);
      dibAttr.m_nYDPI = (int32_t)(dibAttr.m_nYDPI / (float)100 * 2.54f);
      break;
    default:
      break;
  }
  iImageXDpi = dibAttr.m_nXDPI > 1 ? dibAttr.m_nXDPI : (96);
  iImageYDpi = dibAttr.m_nYDPI > 1 ? dibAttr.m_nYDPI : (96);
  if (pProgressiveDecoder->GetWidth() <= 0 ||
      pProgressiveDecoder->GetHeight() <= 0) {
    return nullptr;
  }

  type = pProgressiveDecoder->GetType();
  int32_t iComponents = pProgressiveDecoder->GetNumComponents();
  int32_t iBpc = pProgressiveDecoder->GetBPC();
  FXDIB_Format dibFormat = XFA_GetDIBFormat(type, iComponents, iBpc);
  RetainPtr<CFX_DIBitmap> pBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  pBitmap->Create(pProgressiveDecoder->GetWidth(),
                  pProgressiveDecoder->GetHeight(), dibFormat);
  pBitmap->Clear(0xffffffff);

  size_t nFrames;
  FXCODEC_STATUS status;
  std::tie(status, nFrames) = pProgressiveDecoder->GetFrames();
  if (status != FXCODEC_STATUS_DECODE_READY || nFrames == 0) {
    pBitmap = nullptr;
    return pBitmap;
  }

  status = pProgressiveDecoder->StartDecode(pBitmap, 0, 0, pBitmap->GetWidth(),
                                            pBitmap->GetHeight());
  if (IsFXCodecErrorStatus(status)) {
    pBitmap = nullptr;
    return pBitmap;
  }

  while (status == FXCODEC_STATUS_DECODE_TOBECONTINUE) {
    status = pProgressiveDecoder->ContinueDecode();
    if (IsFXCodecErrorStatus(status)) {
      pBitmap = nullptr;
      return pBitmap;
    }
  }

  return pBitmap;
}

void XFA_RectWithoutMargin(CFX_RectF* rt, const CXFA_Margin* margin) {
  if (!margin)
    return;

  rt->Deflate(margin->GetLeftInset(), margin->GetTopInset(),
              margin->GetRightInset(), margin->GetBottomInset());
}

CXFA_FFWidget* XFA_GetWidgetFromLayoutItem(CXFA_LayoutItem* pLayoutItem) {
  if (!pLayoutItem->GetFormNode()->HasCreatedUIWidget())
    return nullptr;

  return ToFFWidget(ToContentLayoutItem(pLayoutItem));
}

CXFA_CalcData::CXFA_CalcData() : m_iRefCount(0) {}

CXFA_CalcData::~CXFA_CalcData() {}

CXFA_FFWidget::CXFA_FFWidget(CXFA_Node* node)
    : CXFA_ContentLayoutItem(node), m_pNode(node) {}

CXFA_FFWidget::~CXFA_FFWidget() = default;

CXFA_FFWidget* CXFA_FFWidget::AsFFWidget() {
  return this;
}

const CFWL_App* CXFA_FFWidget::GetFWLApp() {
  return GetPageView()->GetDocView()->GetDoc()->GetApp()->GetFWLApp();
}

const CFX_RectF& CXFA_FFWidget::GetWidgetRect() const {
  if ((m_dwStatus & XFA_WidgetStatus_RectCached) == 0)
    RecacheWidgetRect();
  return m_rtWidget;
}

const CFX_RectF& CXFA_FFWidget::RecacheWidgetRect() const {
  m_dwStatus |= XFA_WidgetStatus_RectCached;
  m_rtWidget = GetRect(false);
  return m_rtWidget;
}

CFX_RectF CXFA_FFWidget::GetRectWithoutRotate() {
  CFX_RectF rtWidget = GetWidgetRect();
  float fValue = 0;
  switch (m_pNode->GetRotate()) {
    case 90:
      rtWidget.top = rtWidget.bottom();
      fValue = rtWidget.width;
      rtWidget.width = rtWidget.height;
      rtWidget.height = fValue;
      break;
    case 180:
      rtWidget.left = rtWidget.right();
      rtWidget.top = rtWidget.bottom();
      break;
    case 270:
      rtWidget.left = rtWidget.right();
      fValue = rtWidget.width;
      rtWidget.width = rtWidget.height;
      rtWidget.height = fValue;
      break;
  }
  return rtWidget;
}

uint32_t CXFA_FFWidget::GetStatus() {
  return m_dwStatus;
}

void CXFA_FFWidget::ModifyStatus(uint32_t dwAdded, uint32_t dwRemoved) {
  m_dwStatus = (m_dwStatus & ~dwRemoved) | dwAdded;
}

CFX_RectF CXFA_FFWidget::GetBBox(uint32_t dwStatus, FocusOption focus) {
  if (focus == kDrawFocus || !m_pPageView)
    return CFX_RectF();
  return m_pPageView->GetPageViewRect();
}

void CXFA_FFWidget::RenderWidget(CXFA_Graphics* pGS,
                                 const CFX_Matrix& matrix,
                                 uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus))
    return;

  CXFA_Border* border = m_pNode->GetBorderIfExists();
  if (!border)
    return;

  CFX_RectF rtBorder = GetRectWithoutRotate();
  CXFA_Margin* margin = border->GetMarginIfExists();
  XFA_RectWithoutMargin(&rtBorder, margin);

  rtBorder.Normalize();
  DrawBorder(pGS, border, rtBorder, matrix);
}

bool CXFA_FFWidget::IsLoaded() {
  return !!m_pPageView;
}

bool CXFA_FFWidget::LoadWidget() {
  PerformLayout();
  return true;
}

bool CXFA_FFWidget::PerformLayout() {
  RecacheWidgetRect();
  return true;
}

bool CXFA_FFWidget::UpdateFWLData() {
  return false;
}

void CXFA_FFWidget::UpdateWidgetProperty() {}

void CXFA_FFWidget::DrawBorder(CXFA_Graphics* pGS,
                               CXFA_Box* box,
                               const CFX_RectF& rtBorder,
                               const CFX_Matrix& matrix) {
  if (box)
    box->Draw(pGS, rtBorder, matrix, false);
}

void CXFA_FFWidget::DrawBorderWithFlag(CXFA_Graphics* pGS,
                                       CXFA_Box* box,
                                       const CFX_RectF& rtBorder,
                                       const CFX_Matrix& matrix,
                                       bool forceRound) {
  if (box)
    box->Draw(pGS, rtBorder, matrix, forceRound);
}

void CXFA_FFWidget::InvalidateRect() {
  CFX_RectF rtWidget = GetBBox(XFA_WidgetStatus_Focused, kDoNotDrawFocus);
  rtWidget.Inflate(2, 2);
  m_pDocView->InvalidateRect(m_pPageView.Get(), rtWidget);
}

bool CXFA_FFWidget::OnMouseEnter() {
  return false;
}

bool CXFA_FFWidget::OnMouseExit() {
  return false;
}

bool CXFA_FFWidget::AcceptsFocusOnButtonDown(uint32_t dwFlags,
                                             const CFX_PointF& point,
                                             FWL_MouseCommand command) {
  return false;
}

void CXFA_FFWidget::OnLButtonDown(uint32_t dwFlags, const CFX_PointF& point) {}

bool CXFA_FFWidget::OnLButtonUp(uint32_t dwFlags, const CFX_PointF& point) {
  return false;
}

bool CXFA_FFWidget::OnLButtonDblClk(uint32_t dwFlags, const CFX_PointF& point) {
  return false;
}

bool CXFA_FFWidget::OnMouseMove(uint32_t dwFlags, const CFX_PointF& point) {
  return false;
}

bool CXFA_FFWidget::OnMouseWheel(uint32_t dwFlags,
                                 int16_t zDelta,
                                 const CFX_PointF& point) {
  return false;
}

void CXFA_FFWidget::OnRButtonDown(uint32_t dwFlags, const CFX_PointF& point) {}

bool CXFA_FFWidget::OnRButtonUp(uint32_t dwFlags, const CFX_PointF& point) {
  return false;
}

bool CXFA_FFWidget::OnRButtonDblClk(uint32_t dwFlags, const CFX_PointF& point) {
  return false;
}

bool CXFA_FFWidget::OnSetFocus(CXFA_FFWidget* pOldWidget) {
  CXFA_FFWidget* pParent = ToFFWidget(ToContentLayoutItem(GetParent()));
  if (pParent && !pParent->IsAncestorOf(pOldWidget))
    pParent->OnSetFocus(pOldWidget);

  m_dwStatus |= XFA_WidgetStatus_Focused;
  CXFA_EventParam eParam;
  eParam.m_eType = XFA_EVENT_Enter;
  eParam.m_pTarget = m_pNode.Get();
  m_pNode->ProcessEvent(GetDocView(), XFA_AttributeValue::Enter, &eParam);
  return true;
}

bool CXFA_FFWidget::OnKillFocus(CXFA_FFWidget* pNewWidget) {
  m_dwStatus &= ~XFA_WidgetStatus_Focused;
  EventKillFocus();
  if (!pNewWidget)
    return true;

  CXFA_FFWidget* pParent = ToFFWidget(ToContentLayoutItem(GetParent()));
  if (pParent && !pParent->IsAncestorOf(pNewWidget))
    pParent->OnKillFocus(pNewWidget);

  return true;
}

bool CXFA_FFWidget::OnKeyDown(uint32_t dwKeyCode, uint32_t dwFlags) {
  return false;
}

bool CXFA_FFWidget::OnKeyUp(uint32_t dwKeyCode, uint32_t dwFlags) {
  return false;
}

bool CXFA_FFWidget::OnChar(uint32_t dwChar, uint32_t dwFlags) {
  return false;
}

FWL_WidgetHit CXFA_FFWidget::OnHitTest(const CFX_PointF& point) {
  return FWL_WidgetHit::Unknown;
}

bool CXFA_FFWidget::OnSetCursor(const CFX_PointF& point) {
  return false;
}

bool CXFA_FFWidget::CanUndo() {
  return false;
}

bool CXFA_FFWidget::CanRedo() {
  return false;
}

bool CXFA_FFWidget::Undo() {
  return false;
}

bool CXFA_FFWidget::Redo() {
  return false;
}

bool CXFA_FFWidget::CanCopy() {
  return false;
}

bool CXFA_FFWidget::CanCut() {
  return false;
}

bool CXFA_FFWidget::CanPaste() {
  return false;
}

bool CXFA_FFWidget::CanSelectAll() {
  return false;
}

bool CXFA_FFWidget::CanDelete() {
  return CanCut();
}

bool CXFA_FFWidget::CanDeSelect() {
  return CanCopy();
}

Optional<WideString> CXFA_FFWidget::Copy() {
  return {};
}

Optional<WideString> CXFA_FFWidget::Cut() {
  return {};
}

bool CXFA_FFWidget::Paste(const WideString& wsPaste) {
  return false;
}

void CXFA_FFWidget::SelectAll() {}

void CXFA_FFWidget::Delete() {}

void CXFA_FFWidget::DeSelect() {}

WideString CXFA_FFWidget::GetText() {
  return WideString();
}

FormFieldType CXFA_FFWidget::GetFormFieldType() {
  return FormFieldType::kXFA;
}

CFX_PointF CXFA_FFWidget::Rotate2Normal(const CFX_PointF& point) {
  CFX_Matrix mt = GetRotateMatrix();
  if (mt.IsIdentity())
    return point;

  return mt.GetInverse().Transform(point);
}

CFX_Matrix CXFA_FFWidget::GetRotateMatrix() {
  int32_t iRotate = m_pNode->GetRotate();
  if (!iRotate)
    return CFX_Matrix();

  CFX_RectF rcWidget = GetRectWithoutRotate();
  CFX_Matrix mt;
  switch (iRotate) {
    case 90:
      mt.a = 0;
      mt.b = -1;
      mt.c = 1;
      mt.d = 0;
      mt.e = rcWidget.left - rcWidget.top;
      mt.f = rcWidget.left + rcWidget.top;
      break;
    case 180:
      mt.a = -1;
      mt.b = 0;
      mt.c = 0;
      mt.d = -1;
      mt.e = rcWidget.left * 2;
      mt.f = rcWidget.top * 2;
      break;
    case 270:
      mt.a = 0;
      mt.b = 1;
      mt.c = -1;
      mt.d = 0;
      mt.e = rcWidget.left + rcWidget.top;
      mt.f = rcWidget.top - rcWidget.left;
      break;
  }
  return mt;
}

bool CXFA_FFWidget::IsLayoutRectEmpty() {
  CFX_RectF rtLayout = GetRectWithoutRotate();
  return rtLayout.width < 0.1f && rtLayout.height < 0.1f;
}

CXFA_LayoutItem* CXFA_FFWidget::GetParent() {
  CXFA_Node* pParentNode = m_pNode->GetParent();
  if (!pParentNode)
    return nullptr;

  CXFA_LayoutProcessor* layout = GetDocView()->GetXFALayout();
  return layout->GetLayoutItem(pParentNode);
}

bool CXFA_FFWidget::IsAncestorOf(CXFA_FFWidget* pWidget) {
  if (!pWidget)
    return false;

  CXFA_Node* pChildNode = pWidget->GetNode();
  while (pChildNode) {
    if (pChildNode == m_pNode)
      return true;

    pChildNode = pChildNode->GetParent();
  }
  return false;
}

bool CXFA_FFWidget::PtInActiveRect(const CFX_PointF& point) {
  return GetWidgetRect().Contains(point);
}

CXFA_FFDoc* CXFA_FFWidget::GetDoc() {
  return m_pDocView->GetDoc();
}

CXFA_FFApp* CXFA_FFWidget::GetApp() {
  return GetDoc()->GetApp();
}

IXFA_AppProvider* CXFA_FFWidget::GetAppProvider() {
  return GetApp()->GetAppProvider();
}

bool CXFA_FFWidget::IsMatchVisibleStatus(uint32_t dwStatus) {
  return !!(m_dwStatus & XFA_WidgetStatus_Visible);
}

void CXFA_FFWidget::EventKillFocus() {
  if (m_dwStatus & XFA_WidgetStatus_Access) {
    m_dwStatus &= ~XFA_WidgetStatus_Access;
    return;
  }
  CXFA_EventParam eParam;
  eParam.m_eType = XFA_EVENT_Exit;
  eParam.m_pTarget = m_pNode.Get();
  m_pNode->ProcessEvent(GetDocView(), XFA_AttributeValue::Exit, &eParam);
}

bool CXFA_FFWidget::IsButtonDown() {
  return (m_dwStatus & XFA_WidgetStatus_ButtonDown) != 0;
}

void CXFA_FFWidget::SetButtonDown(bool bSet) {
  bSet ? m_dwStatus |= XFA_WidgetStatus_ButtonDown
       : m_dwStatus &= ~XFA_WidgetStatus_ButtonDown;
}
