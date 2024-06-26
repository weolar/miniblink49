// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/cfx_fileaccess_windows.h"

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "third_party/base/ptr_util.h"

namespace {

void GetFileMode(uint32_t dwMode,
                 uint32_t& dwAccess,
                 uint32_t& dwShare,
                 uint32_t& dwCreation) {
  dwAccess = GENERIC_READ;
  dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
  if (!(dwMode & FX_FILEMODE_ReadOnly)) {
    dwAccess |= GENERIC_WRITE;
    dwCreation = (dwMode & FX_FILEMODE_Truncate) ? CREATE_ALWAYS : OPEN_ALWAYS;
  } else {
    dwCreation = OPEN_EXISTING;
  }
}

}  // namespace

// static
std::unique_ptr<FileAccessIface> FileAccessIface::Create() {
  return pdfium::MakeUnique<CFX_FileAccess_Windows>();
}

CFX_FileAccess_Windows::CFX_FileAccess_Windows() : m_hFile(nullptr) {}

CFX_FileAccess_Windows::~CFX_FileAccess_Windows() {
  Close();
}

bool CFX_FileAccess_Windows::Open(ByteStringView fileName, uint32_t dwMode) {
  if (m_hFile)
    return false;

  uint32_t dwAccess, dwShare, dwCreation;
  GetFileMode(dwMode, dwAccess, dwShare, dwCreation);
  m_hFile = ::CreateFileA(fileName.unterminated_c_str(), dwAccess, dwShare,
                          nullptr, dwCreation, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (m_hFile == INVALID_HANDLE_VALUE)
    m_hFile = nullptr;

  return !!m_hFile;
}

bool CFX_FileAccess_Windows::Open(WideStringView fileName, uint32_t dwMode) {
  if (m_hFile)
    return false;

  uint32_t dwAccess, dwShare, dwCreation;
  GetFileMode(dwMode, dwAccess, dwShare, dwCreation);
  m_hFile =
      ::CreateFileW((LPCWSTR)fileName.unterminated_c_str(), dwAccess, dwShare,
                    nullptr, dwCreation, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (m_hFile == INVALID_HANDLE_VALUE)
    m_hFile = nullptr;

  return !!m_hFile;
}

void CFX_FileAccess_Windows::Close() {
  if (!m_hFile)
    return;

  ::CloseHandle(m_hFile);
  m_hFile = nullptr;
}

FX_FILESIZE CFX_FileAccess_Windows::GetSize() const {
  if (!m_hFile)
    return 0;

  LARGE_INTEGER size = {};
  if (!::GetFileSizeEx(m_hFile, &size))
    return 0;

  return (FX_FILESIZE)size.QuadPart;
}

FX_FILESIZE CFX_FileAccess_Windows::GetPosition() const {
  if (!m_hFile)
    return (FX_FILESIZE)-1;

  LARGE_INTEGER dist = {};
  LARGE_INTEGER newPos = {};
  if (!::SetFilePointerEx(m_hFile, dist, &newPos, FILE_CURRENT))
    return (FX_FILESIZE)-1;

  return (FX_FILESIZE)newPos.QuadPart;
}

FX_FILESIZE CFX_FileAccess_Windows::SetPosition(FX_FILESIZE pos) {
  if (!m_hFile)
    return (FX_FILESIZE)-1;

  LARGE_INTEGER dist;
  dist.QuadPart = pos;
  LARGE_INTEGER newPos = {};
  if (!::SetFilePointerEx(m_hFile, dist, &newPos, FILE_BEGIN))
    return (FX_FILESIZE)-1;

  return (FX_FILESIZE)newPos.QuadPart;
}

size_t CFX_FileAccess_Windows::Read(void* pBuffer, size_t szBuffer) {
  if (!m_hFile)
    return 0;

  size_t szRead = 0;
  if (!::ReadFile(m_hFile, pBuffer, (DWORD)szBuffer, (LPDWORD)&szRead,
                  nullptr)) {
    return 0;
  }
  return szRead;
}

size_t CFX_FileAccess_Windows::Write(const void* pBuffer, size_t szBuffer) {
  if (!m_hFile)
    return 0;

  size_t szWrite = 0;
  if (!::WriteFile(m_hFile, pBuffer, (DWORD)szBuffer, (LPDWORD)&szWrite,
                   nullptr)) {
    return 0;
  }
  return szWrite;
}

size_t CFX_FileAccess_Windows::ReadPos(void* pBuffer,
                                       size_t szBuffer,
                                       FX_FILESIZE pos) {
  if (!m_hFile)
    return 0;

  if (pos >= GetSize())
    return 0;

  if (SetPosition(pos) == (FX_FILESIZE)-1)
    return 0;

  return Read(pBuffer, szBuffer);
}

size_t CFX_FileAccess_Windows::WritePos(const void* pBuffer,
                                        size_t szBuffer,
                                        FX_FILESIZE pos) {
  if (!m_hFile) {
    return 0;
  }
  if (SetPosition(pos) == (FX_FILESIZE)-1) {
    return 0;
  }
  return Write(pBuffer, szBuffer);
}

bool CFX_FileAccess_Windows::Flush() {
  if (!m_hFile)
    return false;

  return !!::FlushFileBuffers(m_hFile);
}

bool CFX_FileAccess_Windows::Truncate(FX_FILESIZE szFile) {
  if (SetPosition(szFile) == (FX_FILESIZE)-1)
    return false;

  return !!::SetEndOfFile(m_hFile);
}
