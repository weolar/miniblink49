// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_FONT_CFX_FONTSOURCEENUM_FILE_H_
#define XFA_FGAS_FONT_CFX_FONTSOURCEENUM_FILE_H_

#include <vector>

#include "core/fxcrt/fx_stream.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
#error "Not used on Windows"
#endif

class CFX_FontSourceEnum_File {
 public:
  CFX_FontSourceEnum_File();
  ~CFX_FontSourceEnum_File();

  void GetNext();
  bool HasNext() const;
  RetainPtr<IFX_SeekableStream> GetStream() const;

 private:
  struct HandleParentPath {
    HandleParentPath() = default;
    HandleParentPath(const HandleParentPath& x) {
      pFileHandle = x.pFileHandle;
      bsParentPath = x.bsParentPath;
    }
    FX_FileHandle* pFileHandle;
    ByteString bsParentPath;
  };

  ByteString GetNextFile();

  WideString m_wsNext;
  std::vector<HandleParentPath> m_FolderQueue;
  std::vector<ByteString> m_FolderPaths;
};

#endif  // XFA_FGAS_FONT_CFX_FONTSOURCEENUM_FILE_H_
