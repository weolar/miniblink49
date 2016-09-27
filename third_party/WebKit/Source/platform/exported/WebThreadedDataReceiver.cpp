// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebThreadedDataReceiver.h"

#include "platform/ThreadedDataReceiver.h"

namespace blink {

WebThreadedDataReceiver::WebThreadedDataReceiver(const PassRefPtrWillBeRawPtr<ThreadedDataReceiver>& threadedDataReceiver)
    : m_private(threadedDataReceiver)
{
}

void WebThreadedDataReceiver::reset()
{
    m_private.reset();
}

void WebThreadedDataReceiver::assign(const WebThreadedDataReceiver& other)
{
    m_private = other.m_private;
}

void WebThreadedDataReceiver::acceptData(const char* data, int dataLength)
{
    return m_private->acceptData(data, dataLength);
}

WebThread* WebThreadedDataReceiver::backgroundThread()
{
    return m_private->backgroundThread();
}

bool WebThreadedDataReceiver::needsMainthreadDataCopy()
{
    return m_private->needsMainthreadDataCopy();
}

void WebThreadedDataReceiver::acceptMainthreadDataNotification(const char* data, int dataLength, int encodedDataLength)
{
    return m_private->acceptMainthreadDataNotification(data, dataLength, encodedDataLength);
}

} // namespace blink
