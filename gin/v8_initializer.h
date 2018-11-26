// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_V8_INITIALIZER_H_
#define GIN_V8_INITIALIZER_H_

#if defined(V8_USE_EXTERNAL_STARTUP_DATA)
#include "base/files/file.h"
#include "base/files/memory_mapped_file.h"
#include "gin/array_buffer.h"
#endif // V8_USE_EXTERNAL_STARTUP_DATA
#include "gin/gin_export.h"
#include "gin/public/isolate_holder.h"
#include "gin/public/v8_platform.h"
//#include "v8/include/v8.h"
#include "v8.h"

namespace gin {

class GIN_EXPORT V8Initializer {
 public:
  // This should be called by IsolateHolder::Initialize().
  static void Initialize(gin::IsolateHolder::ScriptMode mode);

  // Get address and size information for currently loaded snapshot.
  // If no snapshot is loaded, the return values are null for addresses
  // and 0 for sizes.
  static void GetV8ExternalSnapshotData(const char** natives_data_out,
                                        int* natives_size_out,
                                        const char** snapshot_data_out,
                                        int* snapshot_size_out);

#if defined(V8_USE_EXTERNAL_STARTUP_DATA)

  // Load V8 snapshot from user provided platform file descriptors.
  // The offset and size arguments, if non-zero, specify the portions
  // of the files to be loaded. Since the VM can boot with or without
  // the snapshot, this function does not return a status.
  static void LoadV8SnapshotFromFD(base::PlatformFile snapshot_fd,
                                   int64 snapshot_offset,
                                   int64 snapshot_size);
  // Similar to LoadV8SnapshotFromFD, but for the source of the natives.
  // Without the natives we cannot continue, so this function contains
  // release mode asserts and won't return if it fails.
  static void LoadV8NativesFromFD(base::PlatformFile natives_fd,
                                  int64 natives_offset,
                                  int64 natives_size);

  // Load V8 snapshot from default resources, if they are available.
  static void LoadV8Snapshot();

  // Load V8 natives source from default resources. Contains asserts
  // so that it will not return if natives cannot be loaded.
  static void LoadV8Natives();

  // Opens (unless already cached) and returns the V8 natives file.
  // Use with LoadV8NativesFromFD().
  // Asserts if the file does not exist.
  static base::PlatformFile GetOpenNativesFileForChildProcesses(
      base::MemoryMappedFile::Region* region_out);

  // Opens (unless already cached) and returns the V8 snapshot file.
  // Use with LoadV8SnapshotFromFD().
  // Will return -1 if the file does not exist.
  static base::PlatformFile GetOpenSnapshotFileForChildProcesses(
      base::MemoryMappedFile::Region* region_out);
#endif  // V8_USE_EXTERNAL_STARTUP_DATA
  static void SetV8Platform(v8::Platform* platform) {
    platform_ = platform;
  }
private:
  static v8::Platform* platform_;
};

}  // namespace gin

#endif  // GIN_V8_INITIALIZER_H_
