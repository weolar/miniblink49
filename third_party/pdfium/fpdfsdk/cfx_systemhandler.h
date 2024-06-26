// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_CFX_SYSTEMHANDLER_H_
#define FPDFSDK_CFX_SYSTEMHANDLER_H_

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CFFL_FormFiller;
class CPDF_Document;
class CPDF_Font;
class CPDFSDK_FormFillEnvironment;
class CPDFSDK_Widget;

class CFX_SystemHandler {
 public:
  static constexpr int32_t kInvalidTimerID = 0;
  using TimerCallback = void (*)(int32_t idEvent);

  explicit CFX_SystemHandler(CPDFSDK_FormFillEnvironment* pFormFillEnv);
  ~CFX_SystemHandler();

  void InvalidateRect(CPDFSDK_Widget* widget, const CFX_FloatRect& rect);
  void OutputSelectedRect(CFFL_FormFiller* pFormFiller,
                          const CFX_FloatRect& rect);
  bool IsSelectionImplemented() const;
  void SetCursor(int32_t nCursorType);
  bool FindNativeTrueTypeFont(ByteString sFontFaceName);
  CPDF_Font* AddNativeTrueTypeFontToPDF(CPDF_Document* pDoc,
                                        ByteString sFontFaceName,
                                        uint8_t nCharset);

  int32_t SetTimer(int32_t uElapse, TimerCallback lpTimerFunc);
  void KillTimer(int32_t nID);

 private:
  UnownedPtr<CPDFSDK_FormFillEnvironment> const m_pFormFillEnv;
};

#endif  // FPDFSDK_CFX_SYSTEMHANDLER_H_
