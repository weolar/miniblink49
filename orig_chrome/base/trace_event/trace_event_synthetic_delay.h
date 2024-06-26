// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// The synthetic delay framework makes it possible to dynamically inject
// arbitrary delays into into different parts of the codebase. This can be used,
// for instance, for testing various task scheduling algorithms.
//
// The delays are specified in terms of a target duration for a given block of
// code. If the code executes faster than the duration, the thread is made to
// sleep until the deadline is met.
//
// Code can be instrumented for delays with two sets of macros. First, for
// delays that should apply within a scope, use the following macro:
//
//   TRACE_EVENT_SYNTHETIC_DELAY("cc.LayerTreeHost.DrawAndSwap");
//
// For delaying operations that span multiple scopes, use:
//
//   TRACE_EVENT_SYNTHETIC_DELAY_BEGIN("cc.Scheduler.BeginMainFrame");
//   ...
//   TRACE_EVENT_SYNTHETIC_DELAY_END("cc.Scheduler.BeginMainFrame");
//
// Here BEGIN establishes the start time for the delay and END executes the
// delay based on the remaining time. If BEGIN is called multiple times in a
// row, END should be called a corresponding number of times. Only the last
// call to END will have an effect.
//
// Note that a single delay may begin on one thread and end on another. This
// implies that a single delay cannot not be applied in several threads at once.

#ifndef BASE_TRACE_EVENT_TRACE_EVENT_SYNTHETIC_DELAY_H_
#define BASE_TRACE_EVENT_TRACE_EVENT_SYNTHETIC_DELAY_H_

// Apply a named delay in the current scope.
#define TRACE_EVENT_SYNTHETIC_DELAY(name)

// Begin a named delay, establishing its timing start point. May be called
// multiple times as long as the calls to TRACE_EVENT_SYNTHETIC_DELAY_END are
// balanced. Only the first call records the timing start point.
#define TRACE_EVENT_SYNTHETIC_DELAY_BEGIN(name)

// End a named delay. The delay is applied only if this call matches the
// first corresponding call to TRACE_EVENT_SYNTHETIC_DELAY_BEGIN with the
// same delay.
#define TRACE_EVENT_SYNTHETIC_DELAY_END(name)

#endif // BASE_TRACE_EVENT_TRACE_EVENT_SYNTHETIC_DELAY_H_
