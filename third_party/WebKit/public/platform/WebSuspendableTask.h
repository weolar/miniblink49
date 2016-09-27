// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebSuspendableTask_h
#define WebSuspendableTask_h

namespace blink {

class WebSuspendableTask {
public:
    virtual void run() = 0;
    virtual void contextDestroyed() { }
    virtual ~WebSuspendableTask() { }
};

} // namespace blink

#endif // WebSuspendableTask_h
