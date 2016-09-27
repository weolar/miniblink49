// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKIA_EXT_SKIA_TRACE_SHIM_H_
#define SKIA_EXT_SKIA_TRACE_SHIM_H_

#include "base/debug/trace_event.h"

#define SK_TRACE_EVENT0(name) \
    TRACE_EVENT0("skia", name)
#define SK_TRACE_EVENT1(name, arg1_name, arg1_val) \
    TRACE_EVENT1("skia", name, arg1_name, arg1_val)
#define SK_TRACE_EVENT2(name, arg1_name, arg1_val, arg2_name, arg2_val) \
    TRACE_EVENT1("skia", name, arg1_name, arg1_val, arg2_name, arg2_val)

#endif
