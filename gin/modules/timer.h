// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_MODULES_TIMER_H_
#define GIN_MODULES_TIMER_H_

#include "base/memory/weak_ptr.h"
#include "base/timer/timer.h"
#include "gin/gin_export.h"
#include "gin/handle.h"
#include "gin/runner.h"
#include "gin/wrappable.h"
#include "v8/include/v8.h"

namespace gin {

class ObjectTemplateBuilder;

// A simple scriptable timer that can work in one-shot or repeating mode.
class GIN_EXPORT Timer : public Wrappable<Timer> {
 public:
  enum TimerType {
    TYPE_ONE_SHOT,
    TYPE_REPEATING
  };

  static WrapperInfo kWrapperInfo;
  static Handle<Timer> Create(TimerType type, v8::Isolate* isolate,
                              int delay_ms, v8::Local<v8::Function> function);

  ObjectTemplateBuilder GetObjectTemplateBuilder(v8::Isolate* isolate) override;

 private:
  Timer(v8::Isolate* isolate, bool repeating, int delay_ms,
        v8::Local<v8::Function> function);
  ~Timer() override;
  void OnTimerFired();

  base::Timer timer_;
  base::WeakPtr<gin::Runner> runner_;
  base::WeakPtrFactory<Timer> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(Timer);
};


class GIN_EXPORT TimerModule : public Wrappable<TimerModule> {
 public:
  static const char kName[];
  static WrapperInfo kWrapperInfo;
  static Handle<TimerModule> Create(v8::Isolate* isolate);
  static v8::Local<v8::Value> GetModule(v8::Isolate* isolate);

 private:
  TimerModule();
  ~TimerModule() override;

  ObjectTemplateBuilder GetObjectTemplateBuilder(v8::Isolate* isolate) override;
};

}  // namespace gin

#endif  // GIN_MODULES_TIMER_H_
