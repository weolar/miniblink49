// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_CFX_WINDOWSRENDERDEVICE_H_
#define CORE_FXGE_CFX_WINDOWSRENDERDEVICE_H_

#ifdef _WIN32
#ifndef _WINDOWS_
#include <windows.h>
#endif

#include "core/fxge/cfx_renderdevice.h"

enum WindowsPrintMode {
  kModeEmf = 0,
  kModeTextOnly = 1,
  kModePostScript2 = 2,
  kModePostScript3 = 3,
  kModePostScript2PassThrough = 4,
  kModePostScript3PassThrough = 5,
};

class RenderDeviceDriverIface;

#if defined(PDFIUM_PRINT_TEXT_WITH_GDI)
typedef void (*PDFiumEnsureTypefaceCharactersAccessible)(const LOGFONT* font,
                                                         const wchar_t* text,
                                                         size_t text_length);

extern bool g_pdfium_print_text_with_gdi;
extern PDFiumEnsureTypefaceCharactersAccessible
    g_pdfium_typeface_accessible_func;
#endif
extern WindowsPrintMode g_pdfium_print_mode;

class CFX_WindowsRenderDevice final : public CFX_RenderDevice {
 public:
  static RenderDeviceDriverIface* CreateDriver(HDC hDC);

  explicit CFX_WindowsRenderDevice(HDC hDC);
  ~CFX_WindowsRenderDevice() override;
};

#endif  // _WIN32

#endif  // CORE_FXGE_CFX_WINDOWSRENDERDEVICE_H_
