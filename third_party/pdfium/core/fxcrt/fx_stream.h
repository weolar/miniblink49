// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_FX_STREAM_H_
#define CORE_FXCRT_FX_STREAM_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

struct CFindFileDataA;
typedef CFindFileDataA FX_FileHandle;

#else  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

#include <dirent.h>
#include <sys/types.h>
typedef DIR FX_FileHandle;

#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

FX_FileHandle* FX_OpenFolder(const char* path);
bool FX_GetNextFile(FX_FileHandle* handle, ByteString* filename, bool* bFolder);
void FX_CloseFolder(FX_FileHandle* handle);

// Used with std::unique_ptr to automatically call FX_CloseFolder().
struct FxFolderHandleCloser {
  inline void operator()(FX_FileHandle* h) const { FX_CloseFolder(h); }
};

#define FX_FILEMODE_ReadOnly 1
#define FX_FILEMODE_Truncate 2

class IFX_WriteStream {
 public:
  virtual bool WriteBlock(const void* pData, size_t size) = 0;
  virtual bool WriteString(ByteStringView str) = 0;

 protected:
  virtual ~IFX_WriteStream() = default;
};

class IFX_ArchiveStream : public IFX_WriteStream {
 public:
  virtual bool WriteByte(uint8_t byte) = 0;
  virtual bool WriteDWord(uint32_t i) = 0;
  virtual FX_FILESIZE CurrentOffset() const = 0;
};

class IFX_StreamWithSize {
 public:
  virtual FX_FILESIZE GetSize() = 0;
};

class IFX_RetainableWriteStream : virtual public Retainable,
                                  public IFX_WriteStream {};

class IFX_SeekableWriteStream : virtual public IFX_StreamWithSize,
                                public IFX_RetainableWriteStream {
 public:
  // IFX_WriteStream:
  bool WriteBlock(const void* pData, size_t size) override;

  virtual bool Flush() = 0;
  virtual bool WriteBlockAtOffset(const void* pData,
                                  FX_FILESIZE offset,
                                  size_t size) = 0;
};

class IFX_SeekableReadStream : virtual public Retainable,
                               virtual public IFX_StreamWithSize {
 public:
  static RetainPtr<IFX_SeekableReadStream> CreateFromFilename(
      const char* filename);

  virtual bool IsEOF();
  virtual FX_FILESIZE GetPosition();
  virtual size_t ReadBlock(void* buffer, size_t size);

  virtual bool ReadBlockAtOffset(void* buffer,
                                 FX_FILESIZE offset,
                                 size_t size) = 0;
};

class IFX_SeekableStream : public IFX_SeekableReadStream,
                           public IFX_SeekableWriteStream {
 public:
  static RetainPtr<IFX_SeekableStream> CreateFromFilename(const char* filename,
                                                          uint32_t dwModes);

  static RetainPtr<IFX_SeekableStream> CreateFromFilename(
      const wchar_t* filename,
      uint32_t dwModes);

  // IFX_SeekableWriteStream:
  bool WriteBlock(const void* buffer, size_t size) override;
  bool WriteString(ByteStringView str) override;
};

#endif  // CORE_FXCRT_FX_STREAM_H_
