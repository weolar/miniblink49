// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AsyncCallChain_h
#define AsyncCallChain_h

#include "bindings/core/v8/ScriptValue.h"
#include "wtf/Deque.h"
#include "wtf/Forward.h"

namespace blink {

class AsyncCallStack final : public RefCountedWillBeGarbageCollectedFinalized<AsyncCallStack> {
public:
    AsyncCallStack(const String&, const ScriptValue&);
    ~AsyncCallStack();
    DEFINE_INLINE_TRACE() { }
    String description() const { return m_description; }
    ScriptValue callFrames() const { return m_callFrames; }
private:
    String m_description;
    ScriptValue m_callFrames;
};

using AsyncCallStackVector = WillBeHeapDeque<RefPtrWillBeMember<AsyncCallStack>, 4>;

class AsyncCallChain final : public RefCountedWillBeGarbageCollectedFinalized<AsyncCallChain> {
public:
    static PassRefPtrWillBeRawPtr<AsyncCallChain> create(PassRefPtrWillBeRawPtr<AsyncCallStack>, AsyncCallChain* prevChain, unsigned asyncCallChainMaxLength);
    ~AsyncCallChain();
    const AsyncCallStackVector& callStacks() const { return m_callStacks; }
    DECLARE_TRACE();

private:
    AsyncCallChain(PassRefPtrWillBeRawPtr<AsyncCallStack>, AsyncCallChain* prevChain, unsigned asyncCallChainMaxLength);

    AsyncCallStackVector m_callStacks;
};

} // namespace blink


#endif // !defined(AsyncCallChain_h)
