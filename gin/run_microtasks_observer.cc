// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/run_microtasks_observer.h"

namespace gin {

RunMicrotasksObserver::RunMicrotasksObserver(v8::Isolate* isolate)
    : isolate_(isolate) {
}

void RunMicrotasksObserver::WillProcessTask(const base::PendingTask& task) {
}

void RunMicrotasksObserver::DidProcessTask(const base::PendingTask& task) {
  v8::Isolate::Scope scope(isolate_);
  isolate_->RunMicrotasks();
}

}  // namespace gin
