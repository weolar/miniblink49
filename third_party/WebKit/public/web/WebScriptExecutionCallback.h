// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebScriptExecutionCallback_h
#define WebScriptExecutionCallback_h

namespace v8 {
class Value;
template <class T> class Local;
}

namespace blink {

template <typename T> class WebVector;

class WebScriptExecutionCallback {
public:
    virtual ~WebScriptExecutionCallback() { }

    // Method to be invoked when the asynchronous script execution is complete.
    // After function call all objects in vector will be collected
    virtual void completed(const WebVector<v8::Local<v8::Value> >&) { }
};

} // namespace blink

#endif // WebScriptExecutionCallback_h
