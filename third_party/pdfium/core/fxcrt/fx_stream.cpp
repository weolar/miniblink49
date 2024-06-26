// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/fx_stream.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/fileaccess_iface.h"
#include "core/fxcrt/fx_safe_types.h"
#include "third_party/base/ptr_util.h"

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
#include <direct.h>

struct CFindFileDataA {
  HANDLE m_Handle;
  bool m_bEnd;
  WIN32_FIND_DATAA m_FindData;
};
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

namespace {

class CFX_CRTFileStream final : public IFX_SeekableStream {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  // IFX_SeekableStream:
  FX_FILESIZE GetSize() override { return m_pFile->GetSize(); }
  bool IsEOF() override { return GetPosition() >= GetSize(); }
  FX_FILESIZE GetPosition() override { return m_pFile->GetPosition(); }
  bool ReadBlockAtOffset(void* buffer,
                         FX_FILESIZE offset,
                         size_t size) override {
    return m_pFile->ReadPos(buffer, size, offset) > 0;
  }
  size_t ReadBlock(void* buffer, size_t size) override {
    return m_pFile->Read(buffer, size);
  }
  bool WriteBlockAtOffset(const void* buffer,
                          FX_FILESIZE offset,
                          size_t size) override {
    return !!m_pFile->WritePos(buffer, size, offset);
  }
  bool Flush() override { return m_pFile->Flush(); }

 private:
  explicit CFX_CRTFileStream(std::unique_ptr<FileAccessIface> pFA)
      : m_pFile(std::move(pFA)) {}
  ~CFX_CRTFileStream() override {}

  std::unique_ptr<FileAccessIface> m_pFile;
};

}  // namespace

// static
RetainPtr<IFX_SeekableStream> IFX_SeekableStream::CreateFromFilename(
    const char* filename,
    uint32_t dwModes) {
  std::unique_ptr<FileAccessIface> pFA = FileAccessIface::Create();
  if (!pFA->Open(filename, dwModes))
    return nullptr;
  return pdfium::MakeRetain<CFX_CRTFileStream>(std::move(pFA));
}

// static
RetainPtr<IFX_SeekableStream> IFX_SeekableStream::CreateFromFilename(
    const wchar_t* filename,
    uint32_t dwModes) {
  std::unique_ptr<FileAccessIface> pFA = FileAccessIface::Create();
  if (!pFA->Open(filename, dwModes))
    return nullptr;
  return pdfium::MakeRetain<CFX_CRTFileStream>(std::move(pFA));
}

// static
RetainPtr<IFX_SeekableReadStream> IFX_SeekableReadStream::CreateFromFilename(
    const char* filename) {
  return IFX_SeekableStream::CreateFromFilename(filename, FX_FILEMODE_ReadOnly);
}

bool IFX_SeekableWriteStream::WriteBlock(const void* pData, size_t size) {
  return WriteBlockAtOffset(pData, GetSize(), size);
}

bool IFX_SeekableReadStream::IsEOF() {
  return false;
}

FX_FILESIZE IFX_SeekableReadStream::GetPosition() {
  return 0;
}

size_t IFX_SeekableReadStream::ReadBlock(void* buffer, size_t size) {
  return 0;
}

bool IFX_SeekableStream::WriteBlock(const void* buffer, size_t size) {
  return WriteBlockAtOffset(buffer, GetSize(), size);
}

bool IFX_SeekableStream::WriteString(ByteStringView str) {
  return WriteBlock(str.unterminated_c_str(), str.GetLength());
}

FX_FileHandle* FX_OpenFolder(const char* path) {
#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  auto pData = pdfium::MakeUnique<CFindFileDataA>();
  pData->m_Handle =
      FindFirstFileExA((ByteString(path) + "/*.*").c_str(), FindExInfoStandard,
                       &pData->m_FindData, FindExSearchNameMatch, nullptr, 0);
  if (pData->m_Handle == INVALID_HANDLE_VALUE)
    return nullptr;

  pData->m_bEnd = false;
  return pData.release();
#else
  return opendir(path);
#endif
}

bool FX_GetNextFile(FX_FileHandle* handle,
                    ByteString* filename,
                    bool* bFolder) {
  if (!handle)
    return false;

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  if (handle->m_bEnd)
    return false;

  *filename = handle->m_FindData.cFileName;
  *bFolder =
      (handle->m_FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
  if (!FindNextFileA(handle->m_Handle, &handle->m_FindData))
    handle->m_bEnd = true;
  return true;
#else
  struct dirent* de = readdir(handle);
  if (!de)
    return false;
  *filename = de->d_name;
  *bFolder = de->d_type == DT_DIR;
  return true;
#endif
}

void FX_CloseFolder(FX_FileHandle* handle) {
  if (!handle)
    return;

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  FindClose(handle->m_Handle);
  delete handle;
#else
  closedir(handle);
#endif
}
