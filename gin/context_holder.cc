// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/public/context_holder.h"

#include "base/logging.h"
#include "gin/per_context_data.h"

namespace gin {

ContextHolder::ContextHolder(v8::Isolate* isolate)
    : isolate_(isolate) {
}

ContextHolder::~ContextHolder() {
  // PerContextData needs to be destroyed before the context.
  //data_.reset();
    if (data_)
        delete data_;
    data_ = nullptr;
}

void ContextHolder::SetContext(v8::Local<v8::Context> context) {
  DCHECK(context_.IsEmpty());
  context_.Reset(isolate_, context);
  //data_.reset(new PerContextData(this, context));
  if (data_)
      delete data_;
  data_ = new PerContextData(this, context);
}

}  // namespace gin
