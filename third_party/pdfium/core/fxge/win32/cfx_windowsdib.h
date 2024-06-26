// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_WIN32_CFX_WINDOWSDIB_H_
#define CORE_FXGE_WIN32_CFX_WINDOWSDIB_H_
#ifdef _WIN32
#ifndef _WINDOWS_
#include <windows.h>
#endif

#include "core/fxcrt/bytestring.h"
#include "core/fxge/dib/cfx_dibitmap.h"

#define WINDIB_OPEN_MEMORY 0x1
#define WINDIB_OPEN_PATHNAME 0x2

struct WINDIB_Open_Args_ {
  int flags;
  const uint8_t* memory_base;
  size_t memory_size;
  const wchar_t* path_name;
};

class CFX_WindowsDIB final : public CFX_DIBitmap {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  static ByteString GetBitmapInfo(const RetainPtr<CFX_DIBitmap>& pBitmap);
  static HBITMAP GetDDBitmap(const RetainPtr<CFX_DIBitmap>& pBitmap, HDC hDC);

  static RetainPtr<CFX_DIBitmap> LoadFromBuf(BITMAPINFO* pbmi, void* pData);
  static RetainPtr<CFX_DIBitmap> LoadFromFile(const wchar_t* filename);
  static RetainPtr<CFX_DIBitmap> LoadFromFile(const char* filename);
  static RetainPtr<CFX_DIBitmap> LoadDIBitmap(WINDIB_Open_Args_ args);

  HBITMAP GetWindowsBitmap() const { return m_hBitmap; }

  void LoadFromDevice(HDC hDC, int left, int top);
  void SetToDevice(HDC hDC, int left, int top);

 private:
  CFX_WindowsDIB(HDC hDC, int width, int height);
  ~CFX_WindowsDIB() override;

  HDC m_hMemDC;
  HBITMAP m_hBitmap;
  HBITMAP m_hOldBitmap;
};

#endif  // _WIN32

#endif  // CORE_FXGE_WIN32_CFX_WINDOWSDIB_H_
