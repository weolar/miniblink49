// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/modules/timer.h"

#include "base/bind.h"
#include "gin/object_template_builder.h"
#include "gin/per_context_data.h"

namespace gin {

namespace {

v8::Local<v8::String> GetHiddenPropertyName(v8::Isolate* isolate) {
  return gin::StringToSymbol(isolate, "::gin::Timer");
}

}  // namespace

// Timer =======================================================================

gin::WrapperInfo Timer::kWrapperInfo = { gin::kEmbedderNativeGin };

// static
Handle<Timer> Timer::Create(TimerType type, v8::Isolate* isolate, int delay_ms,
                            v8::Local<v8::Function> function) {
  return CreateHandle(isolate, new Timer(isolate, type == TYPE_REPEATING,
                                         delay_ms, function));
}

ObjectTemplateBuilder Timer::GetObjectTemplateBuilder(v8::Isolate* isolate) {
  // We use Unretained() here because we directly own timer_, so we know it will
  // be alive when these methods are called.
  return Wrappable<Timer>::GetObjectTemplateBuilder(isolate)
      .SetMethod("cancel",
                 base::Bind(&base::Timer::Stop, base::Unretained(&timer_)))
      .SetMethod("reset",
                 base::Bind(&base::Timer::Reset, base::Unretained(&timer_)));
}

Timer::Timer(v8::Isolate* isolate, bool repeating, int delay_ms,
             v8::Local<v8::Function> function)
    : timer_(false, repeating),
      runner_(PerContextData::From(
          isolate->GetCurrentContext())->runner()->GetWeakPtr()),
      weak_factory_(this) {
  GetWrapper(runner_->GetContextHolder()->isolate())->SetHiddenValue(
      GetHiddenPropertyName(isolate), function);
  timer_.Start(FROM_HERE, base::TimeDelta::FromMilliseconds(delay_ms),
               base::Bind(&Timer::OnTimerFired, weak_factory_.GetWeakPtr()));
}

Timer::~Timer() {
}

void Timer::OnTimerFired() {
  // This can happen in spite of the weak callback because it is possible for
  // a gin::Handle<> to keep this object alive past when the isolate it is part
  // of is destroyed.
  if (!runner_.get()) {
    return;
  }

  Runner::Scope scope(runner_.get());
  v8::Isolate* isolate = runner_->GetContextHolder()->isolate();
  v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(
      GetWrapper(isolate)->GetHiddenValue(GetHiddenPropertyName(isolate)));
  runner_->Call(function, v8::Undefined(isolate), 0, NULL);
}


// TimerModule =================================================================

const char TimerModule::kName[] = "timer";
WrapperInfo TimerModule::kWrapperInfo = { kEmbedderNativeGin };

// static
Handle<TimerModule> TimerModule::Create(v8::Isolate* isolate) {
  return CreateHandle(isolate, new TimerModule());
}

// static
v8::Local<v8::Value> TimerModule::GetModule(v8::Isolate* isolate) {
  return Create(isolate)->GetWrapper(isolate);
}

TimerModule::TimerModule() {
}

TimerModule::~TimerModule() {
}

ObjectTemplateBuilder TimerModule::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return Wrappable<TimerModule>::GetObjectTemplateBuilder(isolate)
      .SetMethod("createOneShot",
                 base::Bind(&Timer::Create, Timer::TYPE_ONE_SHOT))
      .SetMethod("createRepeating",
                 base::Bind(&Timer::Create, Timer::TYPE_REPEATING));
}

}  // namespace gin
