// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/test/file.h"

#include <iostream>

#include "base/bind.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "gin/arguments.h"
#include "gin/converter.h"
#include "gin/object_template_builder.h"
#include "gin/per_isolate_data.h"
#include "gin/public/wrapper_info.h"

using v8::ObjectTemplate;

namespace gin {

namespace {

v8::Local<v8::Value> ReadFileToString(gin::Arguments* args) {
  std::string filename;
  if (!args->GetNext(&filename))
    return v8::Null(args->isolate());

  const base::FilePath& path = base::FilePath::FromUTF8Unsafe(filename);
  std::string contents;
  if (!ReadFileToString(path, &contents))
    return v8::Null(args->isolate());

  return gin::Converter<std::string>::ToV8(args->isolate(), contents);
}

v8::Local<v8::Value> GetSourceRootDirectory(gin::Arguments* args) {
  base::FilePath path;
  if (!PathService::Get(base::DIR_SOURCE_ROOT, &path))
    return v8::Null(args->isolate());
  return gin::Converter<std::string>::ToV8(args->isolate(),
                                           path.AsUTF8Unsafe());
}

v8::Local<v8::Value> GetFilesInDirectory(gin::Arguments* args) {
  std::string filename;
  if (!args->GetNext(&filename))
    return v8::Null(args->isolate());

  const base::FilePath& path = base::FilePath::FromUTF8Unsafe(filename);
  if (!base::DirectoryExists(path))
    return v8::Null(args->isolate());

  std::vector<std::string> names;
  base::FileEnumerator e(path, false, base::FileEnumerator::FILES);
  for (base::FilePath name = e.Next(); !name.empty(); name = e.Next()) {
    names.push_back(name.BaseName().AsUTF8Unsafe());
  }

  v8::Local<v8::Value> v8_names;
  if (!TryConvertToV8(args->isolate(), names, &v8_names))
    return v8::Null(args->isolate());
  return v8_names;
}

gin::WrapperInfo g_wrapper_info = { gin::kEmbedderNativeGin };

}  // namespace

const char File::kModuleName[] = "file";

v8::Local<v8::Value> File::GetModule(v8::Isolate* isolate) {
  gin::PerIsolateData* data = gin::PerIsolateData::From(isolate);
  v8::Local<ObjectTemplate> templ = data->GetObjectTemplate(&g_wrapper_info);
  if (templ.IsEmpty()) {
    templ = gin::ObjectTemplateBuilder(isolate)
        .SetMethod("readFileToString", ReadFileToString)
        .SetMethod("getFilesInDirectory", GetFilesInDirectory)
        .SetMethod("getSourceRootDirectory", GetSourceRootDirectory)
        .Build();
    data->SetObjectTemplate(&g_wrapper_info, templ);
  }
  return templ->NewInstance();
}

}  // namespace gin
