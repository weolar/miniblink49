// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RejectedPromises_h
#define RejectedPromises_h

#include "platform/heap/Handle.h"

namespace v8 {
class PromiseRejectMessage;
};

namespace blink {

class ScriptCallStack;
class ScriptState;

class RejectedPromises final : public NoBaseWillBeGarbageCollected<RejectedPromises> {
    DECLARE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(RejectedPromises);
public:
    static PassOwnPtrWillBeRawPtr<RejectedPromises> create()
    {
        return adoptPtrWillBeNoop(new RejectedPromises);
    }

    RejectedPromises();
    void dispose();
    DECLARE_TRACE();

    void rejectedWithNoHandler(ScriptState*, v8::PromiseRejectMessage, const String& errorMessage, const String& resourceName, int scriptId, int lineNumber, int columnNumber, PassRefPtrWillBeRawPtr<ScriptCallStack>);
    void handlerAdded(v8::PromiseRejectMessage);

    void processQueue();

private:
    class Message;

    WillBeHeapDeque<OwnPtrWillBeMember<Message>> m_queue;
    WillBeHeapVector<OwnPtrWillBeMember<Message>> m_reportedAsErrors;
};

} // namespace blink

#endif // RejectedPromises_h
