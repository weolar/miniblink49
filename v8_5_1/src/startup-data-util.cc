// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#define _CRT_SECURE_NO_WARNINGS
#include "src/startup-data-util.h"

#include <stdlib.h>
#include <string.h>

#include "src/base/logging.h"
#include "src/base/platform/platform.h"
#include "src/flags.h"
#include "src/utils.h"


namespace v8 {
namespace internal {

#ifdef V8_USE_EXTERNAL_STARTUP_DATA

namespace {

v8::StartupData g_natives;
v8::StartupData g_snapshot;


void ClearStartupData(v8::StartupData* data) {
  data->data = nullptr;
  data->raw_size = 0;
}


void DeleteStartupData(v8::StartupData* data) {
  delete[] data->data;
  ClearStartupData(data);
}


void FreeStartupData() {
  DeleteStartupData(&g_natives);
  DeleteStartupData(&g_snapshot);
}


void Load(const char* blob_file, v8::StartupData* startup_data,
          void (*setter_fn)(v8::StartupData*)) {
  ClearStartupData(startup_data);

  CHECK(blob_file);

  FILE* file = fopen(blob_file, "rb");
  if (!file) {
    PrintF(stderr, "Failed to open startup resource '%s'.\n", blob_file);
    return;
  }

  fseek(file, 0, SEEK_END);
  startup_data->raw_size = static_cast<int>(ftell(file));
  rewind(file);

  startup_data->data = new char[startup_data->raw_size];
  int read_size = static_cast<int>(fread(const_cast<char*>(startup_data->data),
                                         1, startup_data->raw_size, file));
  fclose(file);

  if (startup_data->raw_size == read_size) {
    (*setter_fn)(startup_data);
  } else {
    PrintF(stderr, "Corrupted startup resource '%s'.\n", blob_file);
  }
}


void LoadFromFiles(const char* natives_blob, const char* snapshot_blob) {
  Load(natives_blob, &g_natives, v8::V8::SetNativesDataBlob);
  Load(snapshot_blob, &g_snapshot, v8::V8::SetSnapshotDataBlob);

  atexit(&FreeStartupData);
}


char* RelativePath(char** buffer, const char* exec_path, const char* name) {
  DCHECK(exec_path);
  int path_separator = static_cast<int>(strlen(exec_path)) - 1;
  while (path_separator >= 0 &&
         !base::OS::isDirectorySeparator(exec_path[path_separator])) {
    path_separator--;
  }
  if (path_separator >= 0) {
    int name_length = static_cast<int>(strlen(name));
    *buffer =
        reinterpret_cast<char*>(calloc(path_separator + name_length + 2, 1));
    *buffer[0] = '\0';
    strncat(*buffer, exec_path, path_separator + 1);
    strncat(*buffer, name, name_length);
  } else {
    *buffer = _strdup(name);
  }
  return *buffer;
}

}  // namespace
#endif  // V8_USE_EXTERNAL_STARTUP_DATA


void InitializeExternalStartupData(const char* directory_path) {
#ifdef V8_USE_EXTERNAL_STARTUP_DATA
  char* natives;
  char* snapshot;
  LoadFromFiles(RelativePath(&natives, directory_path, "natives_blob.bin"),
                RelativePath(&snapshot, directory_path,
                             FLAG_ignition ? "snapshot_blob_ignition.bin"
                                           : "snapshot_blob.bin"));
  free(natives);
  free(snapshot);
#endif  // V8_USE_EXTERNAL_STARTUP_DATA
}


void InitializeExternalStartupData(const char* natives_blob,
                                   const char* snapshot_blob) {
#ifdef V8_USE_EXTERNAL_STARTUP_DATA
  LoadFromFiles(natives_blob, snapshot_blob);
#endif  // V8_USE_EXTERNAL_STARTUP_DATA
}

}  // namespace internal
}  // namespace v8
