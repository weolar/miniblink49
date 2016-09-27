// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_STREAM_IMPL_H_
#define CEF_LIBCEF_BROWSER_STREAM_IMPL_H_
#pragma once

#include "include/cef_stream.h"

#include <stdio.h>
#include <string>

#include "cef/include/base/cef_lock.h"

// Implementation of CefStreamReader for files.
class CefFileReader : public CefStreamReader {
 public:
  CefFileReader(FILE* file, bool close);
  ~CefFileReader() override;

  size_t Read(void* ptr, size_t size, size_t n) override;
  int Seek(int64 offset, int whence) override;
  int64 Tell() override;
  int Eof() override;
  bool MayBlock() override { return true; }

 protected:
  bool close_;
  FILE* file_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(CefFileReader);
};

// Implementation of CefStreamWriter for files.
class CefFileWriter : public CefStreamWriter {
 public:
  CefFileWriter(FILE* file, bool close);
  ~CefFileWriter() override;

  size_t Write(const void* ptr, size_t size, size_t n) override;
  int Seek(int64 offset, int whence) override;
  int64 Tell() override;
  int Flush() override;
  bool MayBlock() override { return true; }

 protected:
  FILE* file_;
  bool close_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(CefFileWriter);
};

// Implementation of CefStreamReader for byte buffers.
class CefBytesReader : public CefStreamReader {
 public:
  CefBytesReader(void* data, int64 datasize, bool copy);
  ~CefBytesReader() override;

  size_t Read(void* ptr, size_t size, size_t n) override;
  int Seek(int64 offset, int whence) override;
  int64 Tell() override;
  int Eof() override;
  bool MayBlock() override { return false; }

  void SetData(void* data, int64 datasize, bool copy);

  void* GetData() { return data_; }
  size_t GetDataSize() { return offset_; }

 protected:
  void* data_;
  int64 datasize_;
  bool copy_;
  int64 offset_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(CefBytesReader);
};

// Implementation of CefStreamWriter for byte buffers.
class CefBytesWriter : public CefStreamWriter {
 public:
  explicit CefBytesWriter(size_t grow);
  ~CefBytesWriter() override;

  size_t Write(const void* ptr, size_t size, size_t n) override;
  int Seek(int64 offset, int whence) override;
  int64 Tell() override;
  int Flush() override;
  bool MayBlock() override { return false; }

  void* GetData() { return data_; }
  int64 GetDataSize() { return offset_; }
  std::string GetDataString();

 protected:
  size_t Grow(size_t size);

  size_t grow_;
  void* data_;
  int64 datasize_;
  int64 offset_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(CefBytesWriter);
};

// Implementation of CefStreamReader for handlers.
class CefHandlerReader : public CefStreamReader {
 public:
  explicit CefHandlerReader(CefRefPtr<CefReadHandler> handler)
      : handler_(handler) {}

  size_t Read(void* ptr, size_t size, size_t n) override {
    return handler_->Read(ptr, size, n);
  }
  int Seek(int64 offset, int whence) override {
    return handler_->Seek(offset, whence);
  }
  int64 Tell() override {
    return handler_->Tell();
  }
  int Eof() override {
    return handler_->Eof();
  }
  bool MayBlock() override {
    return handler_->MayBlock();
  }

 protected:
  CefRefPtr<CefReadHandler> handler_;

  IMPLEMENT_REFCOUNTING(CefHandlerReader);
};

// Implementation of CefStreamWriter for handlers.
class CefHandlerWriter : public CefStreamWriter {
 public:
  explicit CefHandlerWriter(CefRefPtr<CefWriteHandler> handler)
      : handler_(handler) {}

  size_t Write(const void* ptr, size_t size, size_t n) override {
    return handler_->Write(ptr, size, n);
  }
  int Seek(int64 offset, int whence) override {
    return handler_->Seek(offset, whence);
  }
  int64 Tell() override {
    return handler_->Tell();
  }
  int Flush() override {
    return handler_->Flush();
  }
  bool MayBlock() override {
    return handler_->MayBlock();
  }

 protected:
  CefRefPtr<CefWriteHandler> handler_;

  IMPLEMENT_REFCOUNTING(CefHandlerWriter);
};

#endif  // CEF_LIBCEF_BROWSER_STREAM_IMPL_H_
