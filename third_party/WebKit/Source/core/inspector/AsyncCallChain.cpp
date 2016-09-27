// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/AsyncCallChain.h"

#include "wtf/text/WTFString.h"

namespace blink {

DEFINE_TRACE(AsyncCallChain)
{
    visitor->trace(m_callStacks);
}

AsyncCallStack::AsyncCallStack(const String& description, const ScriptValue& callFrames)
    : m_description(description)
    , m_callFrames(callFrames)
{
}

AsyncCallStack::~AsyncCallStack()
{
}

PassRefPtrWillBeRawPtr<AsyncCallChain> AsyncCallChain::create(PassRefPtrWillBeRawPtr<AsyncCallStack> stack, AsyncCallChain* prevChain, unsigned asyncCallChainMaxLength)
{
    return adoptRefWillBeNoop(new AsyncCallChain(stack, prevChain, asyncCallChainMaxLength));
}

AsyncCallChain::AsyncCallChain(PassRefPtrWillBeRawPtr<AsyncCallStack> stack, AsyncCallChain* prevChain, unsigned asyncCallChainMaxLength)
{
    if (stack)
        m_callStacks.append(stack);
    if (prevChain) {
        const AsyncCallStackVector& other = prevChain->m_callStacks;
        for (size_t i = 0; i < other.size() && m_callStacks.size() < asyncCallChainMaxLength; i++)
            m_callStacks.append(other[i]);
    }
}

AsyncCallChain::~AsyncCallChain()
{
}

} // namespace blink
