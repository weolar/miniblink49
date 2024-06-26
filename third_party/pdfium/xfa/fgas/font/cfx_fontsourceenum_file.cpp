// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/font/cfx_fontsourceenum_file.h"

#include <iterator>

namespace {

constexpr char kFolderSeparator = '/';

constexpr const char* g_FontFolders[] = {
#if _FX_PLATFORM_ == _FX_PLATFORM_LINUX_
    "/usr/share/fonts", "/usr/share/X11/fonts/Type1",
    "/usr/share/X11/fonts/TTF", "/usr/local/share/fonts",
#elif _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
    "~/Library/Fonts", "/Library/Fonts", "/System/Library/Fonts",
#elif _FX_PLATFORM_ == _FX_PLATFORM_ANDROID_
    "/system/fonts",
#endif
};

}  // namespace

CFX_FontSourceEnum_File::CFX_FontSourceEnum_File()
    : m_FolderPaths(std::begin(g_FontFolders), std::end(g_FontFolders)) {}

CFX_FontSourceEnum_File::~CFX_FontSourceEnum_File() = default;

ByteString CFX_FontSourceEnum_File::GetNextFile() {
  FX_FileHandle* pCurHandle =
      !m_FolderQueue.empty() ? m_FolderQueue.back().pFileHandle : nullptr;
  if (!pCurHandle) {
    if (m_FolderPaths.empty())
      return ByteString();
    pCurHandle = FX_OpenFolder(m_FolderPaths.back().c_str());
    HandleParentPath hpp;
    hpp.pFileHandle = pCurHandle;
    hpp.bsParentPath = m_FolderPaths.back();
    m_FolderQueue.push_back(hpp);
  }
  ByteString bsName;
  bool bFolder;
  while (true) {
    if (!FX_GetNextFile(pCurHandle, &bsName, &bFolder)) {
      FX_CloseFolder(pCurHandle);
      if (!m_FolderQueue.empty())
        m_FolderQueue.pop_back();
      if (m_FolderQueue.empty()) {
        if (!m_FolderPaths.empty())
          m_FolderPaths.pop_back();
        return !m_FolderPaths.empty() ? GetNextFile() : ByteString();
      }
      pCurHandle = m_FolderQueue.back().pFileHandle;
      continue;
    }
    if (bsName == "." || bsName == "..")
      continue;
    if (bFolder) {
      HandleParentPath hpp;
      hpp.bsParentPath =
          m_FolderQueue.back().bsParentPath + kFolderSeparator + bsName;
      hpp.pFileHandle = FX_OpenFolder(hpp.bsParentPath.c_str());
      if (!hpp.pFileHandle)
        continue;
      m_FolderQueue.push_back(hpp);
      pCurHandle = hpp.pFileHandle;
      continue;
    }
    bsName = m_FolderQueue.back().bsParentPath + kFolderSeparator + bsName;
    break;
  }
  return bsName;
}

void CFX_FontSourceEnum_File::GetNext() {
  m_wsNext = WideString::FromUTF8(GetNextFile().AsStringView());
}

bool CFX_FontSourceEnum_File::HasNext() const {
  return !m_wsNext.IsEmpty();
}

RetainPtr<IFX_SeekableStream> CFX_FontSourceEnum_File::GetStream() const {
  ASSERT(HasNext());
  return IFX_SeekableStream::CreateFromFilename(m_wsNext.c_str(),
                                                FX_FILEMODE_ReadOnly);
}
