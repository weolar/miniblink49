// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <windows.h>

#include "core/fxcrt/fx_system.h"
#include "core/fxge/cfx_gemodule.h"
#include "core/fxge/win32/cfx_windowsdib.h"
#include "core/fxge/win32/win32_int.h"

ByteString CFX_WindowsDIB::GetBitmapInfo(
    const RetainPtr<CFX_DIBitmap>& pBitmap) {
  int len = sizeof(BITMAPINFOHEADER);
  if (pBitmap->GetBPP() == 1 || pBitmap->GetBPP() == 8)
    len += sizeof(DWORD) * (int)(1 << pBitmap->GetBPP());

  ByteString result;
  {
    // Span's lifetime must end before ReleaseBuffer() below.
    pdfium::span<char> cspan = result.GetBuffer(len);
    BITMAPINFOHEADER* pbmih = reinterpret_cast<BITMAPINFOHEADER*>(cspan.data());
    memset(pbmih, 0, sizeof(BITMAPINFOHEADER));
    pbmih->biSize = sizeof(BITMAPINFOHEADER);
    pbmih->biBitCount = pBitmap->GetBPP();
    pbmih->biCompression = BI_RGB;
    pbmih->biHeight = -(int)pBitmap->GetHeight();
    pbmih->biPlanes = 1;
    pbmih->biWidth = pBitmap->GetWidth();
    if (pBitmap->GetBPP() == 8) {
      uint32_t* pPalette = (uint32_t*)(pbmih + 1);
      if (pBitmap->GetPalette()) {
        for (int i = 0; i < 256; i++) {
          pPalette[i] = pBitmap->GetPalette()[i];
        }
      } else {
        for (int i = 0; i < 256; i++) {
          pPalette[i] = i * 0x010101;
        }
      }
    }
    if (pBitmap->GetBPP() == 1) {
      uint32_t* pPalette = (uint32_t*)(pbmih + 1);
      if (pBitmap->GetPalette()) {
        pPalette[0] = pBitmap->GetPalette()[0];
        pPalette[1] = pBitmap->GetPalette()[1];
      } else {
        pPalette[0] = 0;
        pPalette[1] = 0xffffff;
      }
    }
  }
  result.ReleaseBuffer(len);
  return result;
}

RetainPtr<CFX_DIBitmap> FX_WindowsDIB_LoadFromBuf(BITMAPINFO* pbmi,
                                                  LPVOID pData,
                                                  bool bAlpha) {
  int width = pbmi->bmiHeader.biWidth;
  int height = pbmi->bmiHeader.biHeight;
  BOOL bBottomUp = true;
  if (height < 0) {
    height = -height;
    bBottomUp = false;
  }
  int pitch = (width * pbmi->bmiHeader.biBitCount + 31) / 32 * 4;
  auto pBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  FXDIB_Format format = bAlpha
                            ? (FXDIB_Format)(pbmi->bmiHeader.biBitCount + 0x200)
                            : (FXDIB_Format)pbmi->bmiHeader.biBitCount;
  if (!pBitmap->Create(width, height, format))
    return nullptr;

  memcpy(pBitmap->GetBuffer(), pData, pitch * height);
  if (bBottomUp) {
    uint8_t* temp_buf = FX_Alloc(uint8_t, pitch);
    int top = 0, bottom = height - 1;
    while (top < bottom) {
      memcpy(temp_buf, pBitmap->GetBuffer() + top * pitch, pitch);
      memcpy(pBitmap->GetBuffer() + top * pitch,
             pBitmap->GetBuffer() + bottom * pitch, pitch);
      memcpy(pBitmap->GetBuffer() + bottom * pitch, temp_buf, pitch);
      top++;
      bottom--;
    }
    FX_Free(temp_buf);
    temp_buf = nullptr;
  }
  if (pbmi->bmiHeader.biBitCount == 1) {
    for (int i = 0; i < 2; i++)
      pBitmap->SetPaletteArgb(i, ((uint32_t*)pbmi->bmiColors)[i] | 0xff000000);
  } else if (pbmi->bmiHeader.biBitCount == 8) {
    for (int i = 0; i < 256; i++)
      pBitmap->SetPaletteArgb(i, ((uint32_t*)pbmi->bmiColors)[i] | 0xff000000);
  }
  return pBitmap;
}

RetainPtr<CFX_DIBitmap> CFX_WindowsDIB::LoadFromBuf(BITMAPINFO* pbmi,
                                                    LPVOID pData) {
  return FX_WindowsDIB_LoadFromBuf(pbmi, pData, false);
}

HBITMAP CFX_WindowsDIB::GetDDBitmap(const RetainPtr<CFX_DIBitmap>& pBitmap,
                                    HDC hDC) {
  ByteString info = GetBitmapInfo(pBitmap);
  return CreateDIBitmap(hDC, (BITMAPINFOHEADER*)info.c_str(), CBM_INIT,
                        pBitmap->GetBuffer(), (BITMAPINFO*)info.c_str(),
                        DIB_RGB_COLORS);
}

void GetBitmapSize(HBITMAP hBitmap, int& w, int& h) {
  BITMAP bmp;
  GetObject(hBitmap, sizeof bmp, &bmp);
  w = bmp.bmWidth;
  h = bmp.bmHeight;
}

RetainPtr<CFX_DIBitmap> CFX_WindowsDIB::LoadFromFile(const wchar_t* filename) {
  CWin32Platform* pPlatform =
      (CWin32Platform*)CFX_GEModule::Get()->GetPlatformData();
  if (pPlatform->m_GdiplusExt.IsAvailable()) {
    WINDIB_Open_Args_ args;
    args.flags = WINDIB_OPEN_PATHNAME;
    args.path_name = filename;
    return pPlatform->m_GdiplusExt.LoadDIBitmap(args);
  }
  HBITMAP hBitmap = (HBITMAP)LoadImageW(nullptr, (wchar_t*)filename,
                                        IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
  if (!hBitmap) {
    return nullptr;
  }
  HDC hDC = CreateCompatibleDC(nullptr);
  int width;
  int height;
  GetBitmapSize(hBitmap, width, height);
  auto pDIBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!pDIBitmap->Create(width, height, FXDIB_Rgb)) {
    DeleteDC(hDC);
    return nullptr;
  }
  ByteString info = GetBitmapInfo(pDIBitmap);
  int ret = GetDIBits(hDC, hBitmap, 0, height, pDIBitmap->GetBuffer(),
                      (BITMAPINFO*)info.c_str(), DIB_RGB_COLORS);
  DeleteDC(hDC);
  if (!ret)
    return nullptr;
  return pDIBitmap;
}

RetainPtr<CFX_DIBitmap> CFX_WindowsDIB::LoadFromFile(const char* filename) {
  return LoadFromFile(WideString::FromDefANSI(filename).c_str());
}

RetainPtr<CFX_DIBitmap> CFX_WindowsDIB::LoadDIBitmap(WINDIB_Open_Args_ args) {
  CWin32Platform* pPlatform =
      (CWin32Platform*)CFX_GEModule::Get()->GetPlatformData();
  if (pPlatform->m_GdiplusExt.IsAvailable()) {
    return pPlatform->m_GdiplusExt.LoadDIBitmap(args);
  }
  if (args.flags == WINDIB_OPEN_MEMORY) {
    return nullptr;
  }
  HBITMAP hBitmap = (HBITMAP)LoadImageW(nullptr, (wchar_t*)args.path_name,
                                        IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
  if (!hBitmap) {
    return nullptr;
  }
  HDC hDC = CreateCompatibleDC(nullptr);
  int width, height;
  GetBitmapSize(hBitmap, width, height);
  auto pDIBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!pDIBitmap->Create(width, height, FXDIB_Rgb)) {
    DeleteDC(hDC);
    return nullptr;
  }
  ByteString info = GetBitmapInfo(pDIBitmap);
  int ret = GetDIBits(hDC, hBitmap, 0, height, pDIBitmap->GetBuffer(),
                      (BITMAPINFO*)info.c_str(), DIB_RGB_COLORS);
  DeleteDC(hDC);
  if (!ret)
    return nullptr;
  return pDIBitmap;
}

CFX_WindowsDIB::CFX_WindowsDIB(HDC hDC, int width, int height) {
  Create(width, height, FXDIB_Rgb, (uint8_t*)1, 0);
  BITMAPINFOHEADER bmih;
  memset(&bmih, 0, sizeof bmih);
  bmih.biSize = sizeof bmih;
  bmih.biBitCount = 24;
  bmih.biHeight = -height;
  bmih.biPlanes = 1;
  bmih.biWidth = width;
  LPVOID pData = nullptr;
  m_hBitmap = CreateDIBSection(hDC, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, &pData,
                               nullptr, 0);
  m_pBuffer.Reset(static_cast<uint8_t*>(pData));
  m_hMemDC = CreateCompatibleDC(hDC);
  m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);
}

CFX_WindowsDIB::~CFX_WindowsDIB() {
  SelectObject(m_hMemDC, m_hOldBitmap);
  DeleteDC(m_hMemDC);
  DeleteObject(m_hBitmap);
}

void CFX_WindowsDIB::LoadFromDevice(HDC hDC, int left, int top) {
  ::BitBlt(m_hMemDC, 0, 0, m_Width, m_Height, hDC, left, top, SRCCOPY);
}

void CFX_WindowsDIB::SetToDevice(HDC hDC, int left, int top) {
  ::BitBlt(hDC, left, top, m_Width, m_Height, m_hMemDC, 0, 0, SRCCOPY);
}
