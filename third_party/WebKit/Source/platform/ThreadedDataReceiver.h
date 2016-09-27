// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ThreadedDataReceiver_h
#define ThreadedDataReceiver_h

#include "platform/heap/Handle.h"

namespace blink {

class WebThread;

class ThreadedDataReceiver : public WillBeGarbageCollectedMixin {
public:
    virtual ~ThreadedDataReceiver() { }

#if !ENABLE(OILPAN)
    virtual void ref() = 0;
    virtual void deref() = 0;
#endif

    virtual void acceptData(const char* data, int dataLength) = 0;
    virtual WebThread* backgroundThread() = 0;

    virtual bool needsMainthreadDataCopy() = 0;
    virtual void acceptMainthreadDataNotification(const char* data, int dataLength, int encodedDataLength) = 0;
};

} // namespace blink

#endif // ThreadedDataReceiver_h
