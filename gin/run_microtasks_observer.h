// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_RUN_MICROTASKS_OBSERVER_H_
#define GIN_RUN_MICROTASKS_OBSERVER_H_

#include "base/message_loop/message_loop.h"
#include "v8/include/v8.h"

namespace gin {

// Runs any pending v8 Microtasks each time a task is completed.
// TODO(hansmuller); At some point perhaps this can be replaced with
// the (currently experimental) Isolate::SetAutorunMicrotasks() method.

class RunMicrotasksObserver : public base::MessageLoop::TaskObserver {
 public:
  RunMicrotasksObserver(v8::Isolate* isolate);

  void WillProcessTask(const base::PendingTask& pending_task) override;
  void DidProcessTask(const base::PendingTask& pending_task) override;

 private:
  v8::Isolate* isolate_;
};

}  // namespace gin

#endif  // GIN_RUN_MICROTASKS_OBSERVER_H_
