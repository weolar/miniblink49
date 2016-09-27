// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/modules/console.h"

#include <iostream>

#include "base/strings/string_util.h"
#include "gin/arguments.h"
#include "gin/converter.h"
#include "gin/object_template_builder.h"
#include "gin/per_isolate_data.h"
#include "gin/public/wrapper_info.h"

using v8::ObjectTemplate;

namespace gin {

namespace {

void Log(Arguments* args) {
  std::vector<std::string> messages;
  if (!args->GetRemaining(&messages)) {
    args->ThrowError();
    return;
  }
  std::cout << JoinString(messages, ' ') << std::endl;
}

WrapperInfo g_wrapper_info = { kEmbedderNativeGin };

}  // namespace

const char Console::kModuleName[] = "console";

v8::Local<v8::Value> Console::GetModule(v8::Isolate* isolate) {
  PerIsolateData* data = PerIsolateData::From(isolate);
  v8::Local<ObjectTemplate> templ = data->GetObjectTemplate(&g_wrapper_info);
  if (templ.IsEmpty()) {
    templ = ObjectTemplateBuilder(isolate)
        .SetMethod("log", Log)
        .Build();
    data->SetObjectTemplate(&g_wrapper_info, templ);
  }
  return templ->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
}

}  // namespace gin
