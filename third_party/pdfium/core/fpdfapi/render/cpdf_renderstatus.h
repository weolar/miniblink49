// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_RENDER_CPDF_RENDERSTATUS_H_
#define CORE_FPDFAPI_RENDER_CPDF_RENDERSTATUS_H_

#include <memory>
#include <vector>

#include "core/fpdfapi/page/cpdf_clippath.h"
#include "core/fpdfapi/page/cpdf_graphicstates.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fpdfapi/render/cpdf_transparency.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/fx_dib.h"

class CFX_PathData;
class CFX_RenderDevice;
class CPDF_Color;
class CPDF_Dictionary;
class CPDF_Font;
class CPDF_FormObject;
class CPDF_ImageCacheEntry;
class CPDF_ImageObject;
class CPDF_ImageRenderer;
class CPDF_Object;
class CPDF_PageObject;
class CPDF_PageObjectHolder;
class CPDF_PathObject;
class CPDF_RenderContext;
class CPDF_ShadingObject;
class CPDF_ShadingPattern;
class CPDF_TilingPattern;
class CPDF_TransferFunc;
class CPDF_Type3Cache;
class CPDF_Type3Char;
class CPDF_Type3Font;

class CPDF_RenderStatus {
 public:
  CPDF_RenderStatus(CPDF_RenderContext* pContext, CFX_RenderDevice* pDevice);
  ~CPDF_RenderStatus();

  // Called prior to Initialize().
  void SetOptions(const CPDF_RenderOptions& options) { m_Options = options; }
  void SetDeviceMatrix(const CFX_Matrix& matrix) { m_DeviceMatrix = matrix; }
  void SetStopObject(const CPDF_PageObject* pStopObj) { m_pStopObj = pStopObj; }
  void SetFormResource(const CPDF_Dictionary* pRes) { m_pFormResource = pRes; }
  void SetType3Char(CPDF_Type3Char* pType3Char) { m_pType3Char = pType3Char; }
  void SetFillColor(FX_ARGB color) { m_T3FillColor = color; }
  void SetDropObjects(bool bDropObjects) { m_bDropObjects = bDropObjects; }
  void SetLoadMask(bool bLoadMask) { m_bLoadMask = bLoadMask; }
  void SetStdCS(bool bStdCS) { m_bStdCS = bStdCS; }
  void SetGroupFamily(uint32_t family) { m_GroupFamily = family; }
  void SetTransparency(const CPDF_Transparency& transparency) {
    m_Transparency = transparency;
  }

  void Initialize(const CPDF_RenderStatus* pParentStatus,
                  const CPDF_GraphicStates* pInitialStates);

  void RenderObjectList(const CPDF_PageObjectHolder* pObjectHolder,
                        const CFX_Matrix& mtObj2Device);
  void RenderSingleObject(CPDF_PageObject* pObj,
                          const CFX_Matrix& mtObj2Device);
  bool ContinueSingleObject(CPDF_PageObject* pObj,
                            const CFX_Matrix& mtObj2Device,
                            PauseIndicatorIface* pPause);
  void ProcessClipPath(const CPDF_ClipPath& ClipPath,
                       const CFX_Matrix& mtObj2Device);

  uint32_t GetGroupFamily() const { return m_GroupFamily; }
  bool GetLoadMask() const { return m_bLoadMask; }
  bool GetDropObjects() const { return m_bDropObjects; }
  bool IsPrint() const { return m_bPrint; }
  bool IsStopped() const { return m_bStopped; }
  CPDF_RenderContext* GetContext() const { return m_pContext.Get(); }
  const CPDF_Dictionary* GetFormResource() const {
    return m_pFormResource.Get();
  }
  CPDF_Dictionary* GetPageResource() const { return m_pPageResource.Get(); }
  CFX_RenderDevice* GetRenderDevice() const { return m_pDevice; }
  const CPDF_RenderOptions& GetRenderOptions() const { return m_Options; }

#if defined _SKIA_SUPPORT_
  void DebugVerifyDeviceIsPreMultiplied() const;
#endif

  RetainPtr<CPDF_TransferFunc> GetTransferFunc(
      const CPDF_Object* pObject) const;

  FX_ARGB GetFillArgb(CPDF_PageObject* pObj) const {
    return GetFillArgbInternal(pObj, false);
  }
  FX_ARGB GetFillArgbForType3(CPDF_PageObject* pObj) const {
    return GetFillArgbInternal(pObj, true);
  }

  void DrawTilingPattern(CPDF_TilingPattern* pPattern,
                         CPDF_PageObject* pPageObj,
                         const CFX_Matrix& mtObj2Device,
                         bool bStroke);
  void DrawShadingPattern(CPDF_ShadingPattern* pPattern,
                          const CPDF_PageObject* pPageObj,
                          const CFX_Matrix& mtObj2Device,
                          bool bStroke);
  void CompositeDIBitmap(const RetainPtr<CFX_DIBitmap>& pDIBitmap,
                         int left,
                         int top,
                         FX_ARGB mask_argb,
                         int bitmap_alpha,
                         BlendMode blend_mode,
                         const CPDF_Transparency& transparency);

 private:
  FX_ARGB GetFillArgbInternal(CPDF_PageObject* pObj, bool bType3) const;
  bool ProcessTransparency(CPDF_PageObject* PageObj,
                           const CFX_Matrix& mtObj2Device);
  void ProcessObjectNoClip(CPDF_PageObject* PageObj,
                           const CFX_Matrix& mtObj2Device);
  void DrawObjWithBackground(CPDF_PageObject* pObj,
                             const CFX_Matrix& mtObj2Device);
  bool DrawObjWithBlend(CPDF_PageObject* pObj, const CFX_Matrix& mtObj2Device);
  bool ProcessPath(CPDF_PathObject* pPathObj, const CFX_Matrix& mtObj2Device);
  void ProcessPathPattern(CPDF_PathObject* pPathObj,
                          const CFX_Matrix& mtObj2Device,
                          int* filltype,
                          bool* bStroke);
  void DrawPathWithPattern(CPDF_PathObject* pPathObj,
                           const CFX_Matrix& mtObj2Device,
                           const CPDF_Color* pColor,
                           bool bStroke);
  bool ClipPattern(const CPDF_PageObject* pPageObj,
                   const CFX_Matrix& mtObj2Device,
                   bool bStroke);
  bool SelectClipPath(const CPDF_PathObject* pPathObj,
                      const CFX_Matrix& mtObj2Device,
                      bool bStroke);
  bool ProcessImage(CPDF_ImageObject* pImageObj,
                    const CFX_Matrix& mtObj2Device);
  void ProcessShading(const CPDF_ShadingObject* pShadingObj,
                      const CFX_Matrix& mtObj2Device);
  void DrawShading(const CPDF_ShadingPattern* pPattern,
                   const CFX_Matrix& mtMatrix,
                   const FX_RECT& clip_rect,
                   int alpha,
                   bool bAlphaMode);
  bool ProcessType3Text(CPDF_TextObject* textobj,
                        const CFX_Matrix& mtObj2Device);
  bool ProcessText(CPDF_TextObject* textobj,
                   const CFX_Matrix& mtObj2Device,
                   CFX_PathData* pClippingPath);
  void DrawTextPathWithPattern(const CPDF_TextObject* textobj,
                               const CFX_Matrix& mtObj2Device,
                               CPDF_Font* pFont,
                               float font_size,
                               const CFX_Matrix* pTextMatrix,
                               bool bFill,
                               bool bStroke);
  bool ProcessForm(const CPDF_FormObject* pFormObj,
                   const CFX_Matrix& mtObj2Device);
  RetainPtr<CFX_DIBitmap> GetBackdrop(const CPDF_PageObject* pObj,
                                      const FX_RECT& rect,
                                      bool bBackAlphaRequired,
                                      int* left,
                                      int* top);
  RetainPtr<CFX_DIBitmap> LoadSMask(CPDF_Dictionary* pSMaskDict,
                                    FX_RECT* pClipRect,
                                    const CFX_Matrix* pMatrix);
  // Optionally write the colorspace family value into |pCSFamily|.
  FX_ARGB GetBackColor(const CPDF_Dictionary* pSMaskDict,
                       const CPDF_Dictionary* pGroupDict,
                       int* pCSFamily);
  static RetainPtr<CPDF_Type3Cache> GetCachedType3(CPDF_Type3Font* pFont);
  static std::unique_ptr<CPDF_GraphicStates> CloneObjStates(
      const CPDF_GraphicStates* pPathObj,
      bool bStroke);
  FX_ARGB GetStrokeArgb(CPDF_PageObject* pObj) const;
  FX_RECT GetObjectClippedRect(const CPDF_PageObject* pObj,
                               const CFX_Matrix& mtObj2Device) const;

  CPDF_RenderOptions m_Options;
  UnownedPtr<const CPDF_Dictionary> m_pFormResource;
  UnownedPtr<CPDF_Dictionary> m_pPageResource;
  std::vector<CPDF_Type3Font*> m_Type3FontCache;
  UnownedPtr<CPDF_RenderContext> const m_pContext;
  bool m_bStopped = false;
  CFX_RenderDevice* const m_pDevice;
  CFX_Matrix m_DeviceMatrix;
  CPDF_ClipPath m_LastClipPath;
  UnownedPtr<const CPDF_PageObject> m_pCurObj;
  UnownedPtr<const CPDF_PageObject> m_pStopObj;
  CPDF_GraphicStates m_InitialStates;
  std::unique_ptr<CPDF_ImageRenderer> m_pImageRenderer;
  CPDF_Transparency m_Transparency;
  bool m_bPrint = false;
  bool m_bDropObjects = false;
  bool m_bStdCS = false;
  bool m_bLoadMask = false;
  uint32_t m_GroupFamily = 0;
  UnownedPtr<CPDF_Type3Char> m_pType3Char;
  FX_ARGB m_T3FillColor = 0;
  BlendMode m_curBlend = BlendMode::kNormal;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_RENDERSTATUS_H_
