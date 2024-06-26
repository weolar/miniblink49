// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_FILEACCESS_IFACE_H_
#define CORE_FXCRT_FILEACCESS_IFACE_H_

#include <memory>

#include "core/fxcrt/fx_safe_types.h"
#include "core/fxcrt/fx_stream.h"
#include "core/fxcrt/fx_string.h"

class FileAccessIface {
 public:
  static std::unique_ptr<FileAccessIface> Create();
  virtual ~FileAccessIface() = default;

  virtual bool Open(ByteStringView fileName, uint32_t dwMode) = 0;
  virtual bool Open(WideStringView fileName, uint32_t dwMode) = 0;
  virtual void Close() = 0;
  virtual FX_FILESIZE GetSize() const = 0;
  virtual FX_FILESIZE GetPosition() const = 0;
  virtual FX_FILESIZE SetPosition(FX_FILESIZE pos) = 0;
  virtual size_t Read(void* pBuffer, size_t szBuffer) = 0;
  virtual size_t Write(const void* pBuffer, size_t szBuffer) = 0;
  virtual size_t ReadPos(void* pBuffer, size_t szBuffer, FX_FILESIZE pos) = 0;
  virtual size_t WritePos(const void* pBuffer,
                          size_t szBuffer,
                          FX_FILESIZE pos) = 0;
  virtual bool Flush() = 0;
  virtual bool Truncate(FX_FILESIZE szFile) = 0;
};

#endif  // CORE_FXCRT_FILEACCESS_IFACE_H_
