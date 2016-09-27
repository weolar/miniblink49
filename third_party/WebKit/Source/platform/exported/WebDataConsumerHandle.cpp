// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebDataConsumerHandle.h"

#include "platform/heap/Handle.h"

namespace blink {

WebDataConsumerHandle::WebDataConsumerHandle()
{
    ASSERT(ThreadState::current());
}

WebDataConsumerHandle::~WebDataConsumerHandle()
{
    ASSERT(ThreadState::current());
}

PassOwnPtr<WebDataConsumerHandle::Reader> WebDataConsumerHandle::obtainReader(WebDataConsumerHandle::Client* client)
{
    ASSERT(ThreadState::current());
    return adoptPtr(obtainReaderInternal(client));
}

} // namespace blink

