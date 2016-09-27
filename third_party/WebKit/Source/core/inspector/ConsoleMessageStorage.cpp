// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/ConsoleMessageStorage.h"

#include "core/frame/FrameHost.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/InspectorConsoleInstrumentation.h"

namespace blink {

static const unsigned maxConsoleMessageCount = 1000;

ConsoleMessageStorage::ConsoleMessageStorage()
    : m_expiredCount(0)
{
}

void ConsoleMessageStorage::reportMessage(ExecutionContext* context, PassRefPtrWillBeRawPtr<ConsoleMessage> prpMessage)
{
    RefPtrWillBeRawPtr<ConsoleMessage> message = prpMessage;
    message->collectCallStack();

    if (message->type() == ClearMessageType)
        clear(context);

    InspectorInstrumentation::addMessageToConsole(context, message.get());

    ASSERT(m_messages.size() <= maxConsoleMessageCount);
    if (m_messages.size() == maxConsoleMessageCount) {
        ++m_expiredCount;
        m_messages.removeFirst();
    }
    m_messages.append(message);
}

void ConsoleMessageStorage::clear(ExecutionContext* context)
{
    InspectorInstrumentation::consoleMessagesCleared(context);
    m_messages.clear();
    m_expiredCount = 0;
}

Vector<unsigned> ConsoleMessageStorage::argumentCounts() const
{
    Vector<unsigned> result(m_messages.size());
    for (size_t i = 0; i < m_messages.size(); ++i)
        result[i] = m_messages[i]->argumentCount();
    return result;
}

void ConsoleMessageStorage::adoptWorkerMessagesAfterTermination(WorkerGlobalScopeProxy* workerGlobalScopeProxy)
{
    for (size_t i = 0; i < m_messages.size(); ++i) {
        if (m_messages[i]->workerGlobalScopeProxy() == workerGlobalScopeProxy)
            m_messages[i]->setWorkerGlobalScopeProxy(nullptr);
    }
}

void ConsoleMessageStorage::frameWindowDiscarded(LocalDOMWindow* window)
{
    for (size_t i = 0; i < m_messages.size(); ++i)
        m_messages[i]->frameWindowDiscarded(window);
}

size_t ConsoleMessageStorage::size() const
{
    return m_messages.size();
}

ConsoleMessage* ConsoleMessageStorage::at(size_t index) const
{
    return m_messages[index].get();
}

int ConsoleMessageStorage::expiredCount() const
{
    return m_expiredCount;
}

DEFINE_TRACE(ConsoleMessageStorage)
{
    visitor->trace(m_messages);
}

} // namespace blink
