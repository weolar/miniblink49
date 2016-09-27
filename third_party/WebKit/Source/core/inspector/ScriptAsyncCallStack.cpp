// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/ScriptAsyncCallStack.h"

namespace blink {

PassRefPtrWillBeRawPtr<ScriptAsyncCallStack> ScriptAsyncCallStack::create(const String& description, PassRefPtrWillBeRawPtr<ScriptCallStack> callStack, PassRefPtrWillBeRawPtr<ScriptAsyncCallStack> asyncStackTrace)
{
    return adoptRefWillBeNoop(new ScriptAsyncCallStack(description, callStack, asyncStackTrace));
}

ScriptAsyncCallStack::ScriptAsyncCallStack(const String& description, PassRefPtrWillBeRawPtr<ScriptCallStack> callStack, PassRefPtrWillBeRawPtr<ScriptAsyncCallStack> asyncStackTrace)
    : m_description(description)
    , m_callStack(callStack)
    , m_asyncStackTrace(asyncStackTrace)
{
    ASSERT(m_callStack);
}

ScriptAsyncCallStack::~ScriptAsyncCallStack()
{
}

PassRefPtr<TypeBuilder::Console::AsyncStackTrace> ScriptAsyncCallStack::buildInspectorObject() const
{
    RefPtr<TypeBuilder::Console::AsyncStackTrace> result = TypeBuilder::Console::AsyncStackTrace::create()
        .setCallFrames(m_callStack->buildInspectorArray())
        .release();
    result->setDescription(m_description);
    if (m_asyncStackTrace)
        result->setAsyncStackTrace(m_asyncStackTrace->buildInspectorObject());
    return result.release();
}

DEFINE_TRACE(ScriptAsyncCallStack)
{
    visitor->trace(m_callStack);
    visitor->trace(m_asyncStackTrace);
}

} // namespace blink
