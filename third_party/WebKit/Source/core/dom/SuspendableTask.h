// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SuspendableTask_h
#define SuspendableTask_h

namespace blink {

class SuspendableTask {
public:
    SuspendableTask() { }
    virtual void run() = 0;
    virtual void contextDestroyed() { }
    virtual ~SuspendableTask() { }
};

} // namespace blink

#endif // SuspendableTask_h
