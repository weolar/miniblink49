// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NativeValueTraits_h
#define NativeValueTraits_h

#include <v8.h>

namespace blink {

class ExceptionState;

template <typename T, typename... Arguments>
struct NativeValueTraits {
    static T nativeValue(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&, Arguments... args);
};

} // namespace blink

#endif // NativeValueTraits_h
