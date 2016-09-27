// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebTestInterfaceFactory_h
#define WebTestInterfaceFactory_h

#include <v8.h>

namespace blink {

class WebTestInterfaceFactory {
public:
    virtual ~WebTestInterfaceFactory() { }
    virtual v8::Local<v8::Value> createInstance(v8::Local<v8::Context>) = 0;
};

} // namespace blink

#endif
