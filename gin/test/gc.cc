// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/test/gc.h"

#include "base/bind.h"
#include "base/logging.h"
#include "gin/arguments.h"
#include "gin/converter.h"
#include "gin/function_template.h"
#include "gin/object_template_builder.h"
#include "gin/per_isolate_data.h"
#include "gin/public/wrapper_info.h"
#include "gin/wrappable.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gin {

namespace {
WrapperInfo g_wrapper_info = { kEmbedderNativeGin };
}  // namespace

const char GC::kModuleName[] = "gc";

v8::Local<v8::Value> GC::GetModule(v8::Isolate* isolate) {
  PerIsolateData* data = PerIsolateData::From(isolate);
  v8::Local<v8::ObjectTemplate> templ =
      data->GetObjectTemplate(&g_wrapper_info);
  if (templ.IsEmpty()) {
    templ = ObjectTemplateBuilder(isolate)
        .SetMethod("collectGarbage",
                   base::Bind(&v8::Isolate::LowMemoryNotification,
                              base::Unretained(isolate)))
        .Build();
    data->SetObjectTemplate(&g_wrapper_info, templ);
  }
  return templ->NewInstance();
}

}  // namespace gin
