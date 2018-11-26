// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/v8_initializer.h"
#include "base/rand_util.h"

#include "v8-platform.h"

#if defined(V8_USE_EXTERNAL_STARTUP_DATA)
#include "base/basictypes.h"
#include "base/debug/alias.h"
#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/files/memory_mapped_file.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/metrics/histogram.h"
#include "base/rand_util.h"
#include "base/strings/sys_string_conversions.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"
#include "crypto/sha2.h"
#endif  // V8_USE_EXTERNAL_STARTUP_DATA

#if defined(V8_USE_EXTERNAL_STARTUP_DATA)
#if defined(OS_ANDROID)
#include "base/android/apk_assets.h"
#endif
#if defined(OS_MACOSX)
#include "base/mac/foundation_util.h"
#endif  // OS_MACOSX
#include "base/path_service.h"
#endif  // V8_USE_EXTERNAL_STARTUP_DATA

namespace gin {

namespace {

#if defined(V8_USE_EXTERNAL_STARTUP_DATA)
// None of these globals are ever freed nor closed.
base::MemoryMappedFile* g_mapped_natives = nullptr;
base::MemoryMappedFile* g_mapped_snapshot = nullptr;

const base::PlatformFile kInvalidPlatformFile =
#if defined(OS_WIN)
    INVALID_HANDLE_VALUE;
#else
    -1;
#endif

// File handles intentionally never closed. Not using File here because its
// Windows implementation guards against two instances owning the same
// PlatformFile (which we allow since we know it is never freed).
base::PlatformFile g_natives_pf = kInvalidPlatformFile;
base::PlatformFile g_snapshot_pf = kInvalidPlatformFile;
base::MemoryMappedFile::Region g_natives_region;
base::MemoryMappedFile::Region g_snapshot_region;

const char kNativesFileName[] = "natives_blob.bin";
const char kSnapshotFileName[] = "snapshot_blob.bin";

void GetV8FilePath(const char* file_name, base::FilePath* path_out) {
#if !defined(OS_MACOSX)
  base::FilePath data_path;
#if defined(OS_ANDROID)
  // This is the path within the .apk.
  data_path = base::FilePath(FILE_PATH_LITERAL("assets"));
#elif defined(OS_POSIX)
  PathService::Get(base::DIR_EXE, &data_path);
#elif defined(OS_WIN)
  PathService::Get(base::DIR_MODULE, &data_path);
#endif
  DCHECK(!data_path.empty());

  *path_out = data_path.AppendASCII(file_name);
#else   // !defined(OS_MACOSX)
  base::ScopedCFTypeRef<CFStringRef> natives_file_name(
      base::SysUTF8ToCFStringRef(file_name));
  *path_out = base::mac::PathForFrameworkBundleResource(natives_file_name);
#endif  // !defined(OS_MACOSX)
  DCHECK(!path_out->empty());
}

static bool MapV8File(base::PlatformFile platform_file,
                      base::MemoryMappedFile::Region region,
                      base::MemoryMappedFile** mmapped_file_out) {
  DCHECK(*mmapped_file_out == NULL);
  scoped_ptr<base::MemoryMappedFile> mmapped_file(new base::MemoryMappedFile());
  if (mmapped_file->Initialize(base::File(platform_file), region)) {
    *mmapped_file_out = mmapped_file.release();
    return true;
  }
  return false;
}

base::PlatformFile OpenV8File(const char* file_name,
                              base::MemoryMappedFile::Region* region_out) {
  // Re-try logic here is motivated by http://crbug.com/479537
  // for A/V on Windows (https://support.microsoft.com/en-us/kb/316609).

  // These match tools/metrics/histograms.xml
  enum OpenV8FileResult {
    OPENED = 0,
    OPENED_RETRY,
    FAILED_IN_USE,
    FAILED_OTHER,
    MAX_VALUE
  };
  base::FilePath path;
  GetV8FilePath(file_name, &path);

#if defined(OS_ANDROID)
  base::File file(base::android::OpenApkAsset(path.value(), region_out));
  OpenV8FileResult result = file.IsValid() ? OpenV8FileResult::OPENED
                                           : OpenV8FileResult::FAILED_OTHER;
#else
  // Re-try logic here is motivated by http://crbug.com/479537
  // for A/V on Windows (https://support.microsoft.com/en-us/kb/316609).
  const int kMaxOpenAttempts = 5;
  const int kOpenRetryDelayMillis = 250;

  OpenV8FileResult result = OpenV8FileResult::FAILED_IN_USE;
  int flags = base::File::FLAG_OPEN | base::File::FLAG_READ;
  base::File file;
  for (int attempt = 0; attempt < kMaxOpenAttempts; attempt++) {
    file.Initialize(path, flags);
    if (file.IsValid()) {
      *region_out = base::MemoryMappedFile::Region::kWholeFile;
      if (attempt == 0) {
        result = OpenV8FileResult::OPENED;
        break;
      } else {
        result = OpenV8FileResult::OPENED_RETRY;
        break;
      }
    } else if (file.error_details() != base::File::FILE_ERROR_IN_USE) {
      result = OpenV8FileResult::FAILED_OTHER;
#ifdef OS_WIN
      // TODO(oth): temporary diagnostics for http://crbug.com/479537
      std::string narrow(kNativesFileName);
      base::FilePath::StringType nativesBlob(narrow.begin(), narrow.end());
      if (path.BaseName().value() == nativesBlob) {
        base::File::Error file_error = file.error_details();
        base::debug::Alias(&file_error);
        LOG(FATAL) << "Failed to open V8 file '" << path.value()
                   << "' (reason: " << file.error_details() << ")";
      }
#endif  // OS_WIN
      break;
    } else if (kMaxOpenAttempts - 1 != attempt) {
      base::PlatformThread::Sleep(
          base::TimeDelta::FromMilliseconds(kOpenRetryDelayMillis));
    }
  }
#endif  // defined(OS_ANDROID)

  UMA_HISTOGRAM_ENUMERATION("V8.Initializer.OpenV8File.Result",
                            result,
                            OpenV8FileResult::MAX_VALUE);
  return file.TakePlatformFile();
}

void OpenNativesFileIfNecessary() {
  if (g_natives_pf == kInvalidPlatformFile) {
    g_natives_pf = OpenV8File(kNativesFileName, &g_natives_region);
  }
}

void OpenSnapshotFileIfNecessary() {
  if (g_snapshot_pf == kInvalidPlatformFile) {
    g_snapshot_pf = OpenV8File(kSnapshotFileName, &g_snapshot_region);
  }
}

#if defined(V8_VERIFY_EXTERNAL_STARTUP_DATA)
bool VerifyV8StartupFile(base::MemoryMappedFile** file,
                         const unsigned char* fingerprint) {
  unsigned char output[crypto::kSHA256Length];
  crypto::SHA256HashString(
      base::StringPiece(reinterpret_cast<const char*>((*file)->data()),
                        (*file)->length()),
      output, sizeof(output));
  if (!memcmp(fingerprint, output, sizeof(output))) {
    return true;
  }

  // TODO(oth): Remove this temporary diagnostics for http://crbug.com/501799
  uint64_t input[sizeof(output)];
  memcpy(input, fingerprint, sizeof(input));

  base::debug::Alias(output);
  base::debug::Alias(input);

  const uint64_t* o64 = reinterpret_cast<const uint64_t*>(output);
  const uint64_t* f64 = reinterpret_cast<const uint64_t*>(fingerprint);
  LOG(FATAL) << "Natives length " << (*file)->length()
             << " H(computed) " << o64[0] << o64[1] << o64[2] << o64[3]
             << " H(expected) " << f64[0] << f64[1] << f64[2] << f64[3];

  delete *file;
  *file = NULL;
  return false;
}
#endif  // V8_VERIFY_EXTERNAL_STARTUP_DATA
#endif  // V8_USE_EXTERNAL_STARTUP_DATA

bool GenerateEntropy(unsigned char* buffer, size_t amount) {
  base::RandBytes(buffer, amount);
  return true;
}

}  // namespace

#if defined(V8_USE_EXTERNAL_STARTUP_DATA)
#if defined(V8_VERIFY_EXTERNAL_STARTUP_DATA)
// Defined in gen/gin/v8_snapshot_fingerprint.cc
extern const unsigned char g_natives_fingerprint[];
extern const unsigned char g_snapshot_fingerprint[];
#endif  // V8_VERIFY_EXTERNAL_STARTUP_DATA

enum LoadV8FileResult {
  V8_LOAD_SUCCESS = 0,
  V8_LOAD_FAILED_OPEN,
  V8_LOAD_FAILED_MAP,
  V8_LOAD_FAILED_VERIFY,
  V8_LOAD_MAX_VALUE
};

static LoadV8FileResult MapVerify(base::PlatformFile platform_file,
                                  const base::MemoryMappedFile::Region& region,
#if defined(V8_VERIFY_EXTERNAL_STARTUP_DATA)
                                  const unsigned char* fingerprint,
#endif
                                  base::MemoryMappedFile** mmapped_file_out) {
  if (platform_file == kInvalidPlatformFile)
    return V8_LOAD_FAILED_OPEN;
  if (!MapV8File(platform_file, region, mmapped_file_out))
    return V8_LOAD_FAILED_MAP;
#if defined(V8_VERIFY_EXTERNAL_STARTUP_DATA)
  if (!VerifyV8StartupFile(mmapped_file_out, fingerprint))
    return V8_LOAD_FAILED_VERIFY;
#endif  // V8_VERIFY_EXTERNAL_STARTUP_DATA
  return V8_LOAD_SUCCESS;
}

// static
void V8Initializer::LoadV8Snapshot() {
  if (g_mapped_snapshot)
    return;

  OpenSnapshotFileIfNecessary();
  LoadV8FileResult result = MapVerify(g_snapshot_pf, g_snapshot_region,
#if defined(V8_VERIFY_EXTERNAL_STARTUP_DATA)
                                      g_snapshot_fingerprint,
#endif
                                      &g_mapped_snapshot);
  // V8 can't start up without the source of the natives, but it can
  // start up (slower) without the snapshot.
  UMA_HISTOGRAM_ENUMERATION("V8.Initializer.LoadV8Snapshot.Result", result,
                            V8_LOAD_MAX_VALUE);
}

void V8Initializer::LoadV8Natives() {
  if (g_mapped_natives)
    return;

  OpenNativesFileIfNecessary();
  LoadV8FileResult result = MapVerify(g_natives_pf, g_natives_region,
#if defined(V8_VERIFY_EXTERNAL_STARTUP_DATA)
                                      g_natives_fingerprint,
#endif
                                      &g_mapped_natives);
  if (result != V8_LOAD_SUCCESS) {
    LOG(FATAL) << "Couldn't mmap v8 natives data file, status code is "
               << static_cast<int>(result);
  }
}

// static
void V8Initializer::LoadV8SnapshotFromFD(base::PlatformFile snapshot_pf,
                                         int64 snapshot_offset,
                                         int64 snapshot_size) {
  if (g_mapped_snapshot)
    return;

  if (snapshot_pf == kInvalidPlatformFile)
    return;

  base::MemoryMappedFile::Region snapshot_region =
      base::MemoryMappedFile::Region::kWholeFile;
  if (snapshot_size != 0 || snapshot_offset != 0) {
    snapshot_region.offset = snapshot_offset;
    snapshot_region.size = snapshot_size;
  }

  LoadV8FileResult result = V8_LOAD_SUCCESS;
  if (!MapV8File(snapshot_pf, snapshot_region, &g_mapped_snapshot))
    result = V8_LOAD_FAILED_MAP;
#if defined(V8_VERIFY_EXTERNAL_STARTUP_DATA)
  if (!VerifyV8StartupFile(&g_mapped_snapshot, g_snapshot_fingerprint))
    result = V8_LOAD_FAILED_VERIFY;
#endif  // V8_VERIFY_EXTERNAL_STARTUP_DATA
  UMA_HISTOGRAM_ENUMERATION("V8.Initializer.LoadV8Snapshot.Result", result,
                            V8_LOAD_MAX_VALUE);
}

// static
void V8Initializer::LoadV8NativesFromFD(base::PlatformFile natives_pf,
                                        int64 natives_offset,
                                        int64 natives_size) {
  if (g_mapped_natives)
    return;

  CHECK_NE(natives_pf, kInvalidPlatformFile);

  base::MemoryMappedFile::Region natives_region =
      base::MemoryMappedFile::Region::kWholeFile;
  if (natives_size != 0 || natives_offset != 0) {
    natives_region.offset = natives_offset;
    natives_region.size = natives_size;
  }

  if (!MapV8File(natives_pf, natives_region, &g_mapped_natives)) {
    LOG(FATAL) << "Couldn't mmap v8 natives data file";
  }
#if defined(V8_VERIFY_EXTERNAL_STARTUP_DATA)
  if (!VerifyV8StartupFile(&g_mapped_natives, g_natives_fingerprint)) {
    LOG(FATAL) << "Couldn't verify contents of v8 natives data file";
  }
#endif  // V8_VERIFY_EXTERNAL_STARTUP_DATA
}

// static
base::PlatformFile V8Initializer::GetOpenNativesFileForChildProcesses(
    base::MemoryMappedFile::Region* region_out) {
  OpenNativesFileIfNecessary();
  *region_out = g_natives_region;
  return g_natives_pf;
}

// static
base::PlatformFile V8Initializer::GetOpenSnapshotFileForChildProcesses(
    base::MemoryMappedFile::Region* region_out) {
  OpenSnapshotFileIfNecessary();
  *region_out = g_snapshot_region;
  return g_snapshot_pf;
}
#endif  // defined(V8_USE_EXTERNAL_STARTUP_DATA)

v8::Platform* V8Initializer::platform_ = nullptr;

// static
void V8Initializer::Initialize(gin::IsolateHolder::ScriptMode mode) {
  static bool v8_is_initialized = false;
  if (v8_is_initialized)
    return;

  if (!platform_)
      platform_ = V8Platform::Get();
  v8::V8::InitializePlatform(platform_);

  if (gin::IsolateHolder::kStrictMode == mode) {
    static const char use_strict[] = "--use_strict";
    v8::V8::SetFlagsFromString(use_strict, sizeof(use_strict) - 1);
  }

#if defined(V8_USE_EXTERNAL_STARTUP_DATA)
  v8::StartupData natives;
  natives.data = reinterpret_cast<const char*>(g_mapped_natives->data());
  natives.raw_size = static_cast<int>(g_mapped_natives->length());
  v8::V8::SetNativesDataBlob(&natives);

  if (g_mapped_snapshot != NULL) {
    v8::StartupData snapshot;
    snapshot.data = reinterpret_cast<const char*>(g_mapped_snapshot->data());
    snapshot.raw_size = static_cast<int>(g_mapped_snapshot->length());
    v8::V8::SetSnapshotDataBlob(&snapshot);
  }
#endif  // V8_USE_EXTERNAL_STARTUP_DATA

  v8::V8::SetEntropySource(&GenerateEntropy);
  v8::V8::Initialize();

  v8_is_initialized = true;
}

#ifdef MINIBLINK_NOT_IMPLEMENTED
// static
void V8Initializer::GetV8ExternalSnapshotData(const char** natives_data_out,
                                              int* natives_size_out,
                                              const char** snapshot_data_out,
                                              int* snapshot_size_out) {
  if (g_mapped_natives) {
    *natives_data_out = reinterpret_cast<const char*>(g_mapped_natives->data());
    *natives_size_out = static_cast<int>(g_mapped_natives->length());
  } else {
    *natives_data_out = NULL;
    *natives_size_out = 0;
  }
  if (g_mapped_snapshot) {
    *snapshot_data_out =
        reinterpret_cast<const char*>(g_mapped_snapshot->data());
    *snapshot_size_out = static_cast<int>(g_mapped_snapshot->length());
  } else {
    *snapshot_data_out = NULL;
    *snapshot_size_out = 0;
  }
}
#endif // MINIBLINK_NOT_IMPLEMENTED

}  // namespace gin
