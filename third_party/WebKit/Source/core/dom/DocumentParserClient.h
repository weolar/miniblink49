// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DocumentParserClient_h
#define DocumentParserClient_h

#include "platform/heap/Handle.h"

namespace blink {

class DocumentParserClient : public WillBeGarbageCollectedMixin {
public:
    // This callback is called when all data pushed to parser has been consumed.
    virtual void notifyParserStopped() = 0;

    DEFINE_INLINE_VIRTUAL_TRACE() { }

protected:
    DocumentParserClient() { }
};

} // namespace blink

#endif // DocumentParserClient_h
