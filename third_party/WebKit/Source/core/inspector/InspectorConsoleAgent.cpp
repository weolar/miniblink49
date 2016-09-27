/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "config.h"
#include "core/inspector/InspectorConsoleAgent.h"

#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/ConsoleMessageStorage.h"
#include "core/inspector/IdentifiersFactory.h"
#include "core/inspector/InjectedScript.h"
#include "core/inspector/InjectedScriptManager.h"
#include "core/inspector/InspectorDebuggerAgent.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/inspector/ScriptArguments.h"
#include "core/inspector/ScriptAsyncCallStack.h"
#include "core/inspector/V8Debugger.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace ConsoleAgentState {
static const char consoleMessagesEnabled[] = "consoleMessagesEnabled";
}

InspectorConsoleAgent::InspectorConsoleAgent(InjectedScriptManager* injectedScriptManager)
    : InspectorBaseAgent<InspectorConsoleAgent, InspectorFrontend::Console>("Console")
    , m_injectedScriptManager(injectedScriptManager)
    , m_debuggerAgent(nullptr)
    , m_enabled(false)
{
}

InspectorConsoleAgent::~InspectorConsoleAgent()
{
#if !ENABLE(OILPAN)
    m_instrumentingAgents->setInspectorConsoleAgent(0);
#endif
}

DEFINE_TRACE(InspectorConsoleAgent)
{
    visitor->trace(m_injectedScriptManager);
    visitor->trace(m_debuggerAgent);
    InspectorBaseAgent::trace(visitor);
}

void InspectorConsoleAgent::enable(ErrorString*)
{
    if (m_enabled)
        return;
    m_instrumentingAgents->setInspectorConsoleAgent(this);
    m_enabled = true;
    enableStackCapturingIfNeeded();

    m_state->setBoolean(ConsoleAgentState::consoleMessagesEnabled, true);

    ConsoleMessageStorage* storage = messageStorage();
    if (storage->expiredCount()) {
        RefPtrWillBeRawPtr<ConsoleMessage> expiredMessage = ConsoleMessage::create(OtherMessageSource, WarningMessageLevel, String::format("%d console messages are not shown.", storage->expiredCount()));
        expiredMessage->setTimestamp(0);
        sendConsoleMessageToFrontend(expiredMessage.get(), false);
    }

    size_t messageCount = storage->size();
    for (size_t i = 0; i < messageCount; ++i)
        sendConsoleMessageToFrontend(storage->at(i), false);
}

void InspectorConsoleAgent::disable(ErrorString*)
{
    if (!m_enabled)
        return;
    m_instrumentingAgents->setInspectorConsoleAgent(nullptr);
    m_enabled = false;
    disableStackCapturingIfNeeded();

    m_state->setBoolean(ConsoleAgentState::consoleMessagesEnabled, false);
}

void InspectorConsoleAgent::restore()
{
    if (m_state->getBoolean(ConsoleAgentState::consoleMessagesEnabled)) {
        frontend()->messagesCleared();
        ErrorString error;
        enable(&error);
    }
}

void InspectorConsoleAgent::addMessageToConsole(ConsoleMessage* consoleMessage)
{
    sendConsoleMessageToFrontend(consoleMessage, true);
    if (consoleMessage->type() != AssertMessageType)
        return;
    if (!m_debuggerAgent || !m_debuggerAgent->enabled())
        return;
    if (m_debuggerAgent->debugger().pauseOnExceptionsState() != V8Debugger::DontPauseOnExceptions)
        m_debuggerAgent->breakProgram(InspectorFrontend::Debugger::Reason::Assert, nullptr);
}

void InspectorConsoleAgent::consoleMessagesCleared()
{
    m_injectedScriptManager->releaseObjectGroup("console");
    frontend()->messagesCleared();
}

static TypeBuilder::Console::ConsoleMessage::Source::Enum messageSourceValue(MessageSource source)
{
    switch (source) {
    case XMLMessageSource: return TypeBuilder::Console::ConsoleMessage::Source::Xml;
    case JSMessageSource: return TypeBuilder::Console::ConsoleMessage::Source::Javascript;
    case NetworkMessageSource: return TypeBuilder::Console::ConsoleMessage::Source::Network;
    case ConsoleAPIMessageSource: return TypeBuilder::Console::ConsoleMessage::Source::Console_api;
    case StorageMessageSource: return TypeBuilder::Console::ConsoleMessage::Source::Storage;
    case AppCacheMessageSource: return TypeBuilder::Console::ConsoleMessage::Source::Appcache;
    case RenderingMessageSource: return TypeBuilder::Console::ConsoleMessage::Source::Rendering;
    case SecurityMessageSource: return TypeBuilder::Console::ConsoleMessage::Source::Security;
    case OtherMessageSource: return TypeBuilder::Console::ConsoleMessage::Source::Other;
    case DeprecationMessageSource: return TypeBuilder::Console::ConsoleMessage::Source::Deprecation;
    }
    return TypeBuilder::Console::ConsoleMessage::Source::Other;
}


static TypeBuilder::Console::ConsoleMessage::Type::Enum messageTypeValue(MessageType type)
{
    switch (type) {
    case LogMessageType: return TypeBuilder::Console::ConsoleMessage::Type::Log;
    case ClearMessageType: return TypeBuilder::Console::ConsoleMessage::Type::Clear;
    case DirMessageType: return TypeBuilder::Console::ConsoleMessage::Type::Dir;
    case DirXMLMessageType: return TypeBuilder::Console::ConsoleMessage::Type::Dirxml;
    case TableMessageType: return TypeBuilder::Console::ConsoleMessage::Type::Table;
    case TraceMessageType: return TypeBuilder::Console::ConsoleMessage::Type::Trace;
    case StartGroupMessageType: return TypeBuilder::Console::ConsoleMessage::Type::StartGroup;
    case StartGroupCollapsedMessageType: return TypeBuilder::Console::ConsoleMessage::Type::StartGroupCollapsed;
    case EndGroupMessageType: return TypeBuilder::Console::ConsoleMessage::Type::EndGroup;
    case AssertMessageType: return TypeBuilder::Console::ConsoleMessage::Type::Assert;
    case TimeEndMessageType: return TypeBuilder::Console::ConsoleMessage::Type::Log;
    case CountMessageType: return TypeBuilder::Console::ConsoleMessage::Type::Log;
    }
    return TypeBuilder::Console::ConsoleMessage::Type::Log;
}

static TypeBuilder::Console::ConsoleMessage::Level::Enum messageLevelValue(MessageLevel level)
{
    switch (level) {
    case DebugMessageLevel: return TypeBuilder::Console::ConsoleMessage::Level::Debug;
    case LogMessageLevel: return TypeBuilder::Console::ConsoleMessage::Level::Log;
    case WarningMessageLevel: return TypeBuilder::Console::ConsoleMessage::Level::Warning;
    case ErrorMessageLevel: return TypeBuilder::Console::ConsoleMessage::Level::Error;
    case InfoMessageLevel: return TypeBuilder::Console::ConsoleMessage::Level::Info;
    case RevokedErrorMessageLevel: return TypeBuilder::Console::ConsoleMessage::Level::RevokedError;
    }
    return TypeBuilder::Console::ConsoleMessage::Level::Log;
}

void InspectorConsoleAgent::sendConsoleMessageToFrontend(ConsoleMessage* consoleMessage, bool generatePreview)
{
    if (consoleMessage->workerGlobalScopeProxy())
        return;

    RefPtr<TypeBuilder::Console::ConsoleMessage> jsonObj = TypeBuilder::Console::ConsoleMessage::create()
        .setSource(messageSourceValue(consoleMessage->source()))
        .setLevel(messageLevelValue(consoleMessage->level()))
        .setText(consoleMessage->message())
        .setTimestamp(consoleMessage->timestamp());
    // FIXME: only send out type for ConsoleAPI source messages.
    jsonObj->setType(messageTypeValue(consoleMessage->type()));
    jsonObj->setLine(static_cast<int>(consoleMessage->lineNumber()));
    jsonObj->setColumn(static_cast<int>(consoleMessage->columnNumber()));
    if (consoleMessage->scriptId())
        jsonObj->setScriptId(String::number(consoleMessage->scriptId()));
    jsonObj->setUrl(consoleMessage->url());
    ScriptState* scriptState = consoleMessage->scriptState();
    if (scriptState)
        jsonObj->setExecutionContextId(m_injectedScriptManager->injectedScriptIdFor(scriptState));
    if (consoleMessage->source() == NetworkMessageSource && consoleMessage->requestIdentifier())
        jsonObj->setNetworkRequestId(IdentifiersFactory::requestId(consoleMessage->requestIdentifier()));
    RefPtrWillBeRawPtr<ScriptArguments> arguments = consoleMessage->scriptArguments();
    if (arguments && arguments->argumentCount()) {
        InjectedScript injectedScript = m_injectedScriptManager->injectedScriptFor(arguments->scriptState());
        if (!injectedScript.isEmpty()) {
            RefPtr<TypeBuilder::Array<TypeBuilder::Runtime::RemoteObject> > jsonArgs = TypeBuilder::Array<TypeBuilder::Runtime::RemoteObject>::create();
            if (consoleMessage->type() == TableMessageType && generatePreview && arguments->argumentCount()) {
                ScriptValue table = arguments->argumentAt(0);
                ScriptValue columns = arguments->argumentCount() > 1 ? arguments->argumentAt(1) : ScriptValue();
                RefPtr<TypeBuilder::Runtime::RemoteObject> inspectorValue = injectedScript.wrapTable(table, columns);
                if (!inspectorValue) {
                    ASSERT_NOT_REACHED();
                    return;
                }
                jsonArgs->addItem(inspectorValue);
            } else {
                for (unsigned i = 0; i < arguments->argumentCount(); ++i) {
                    RefPtr<TypeBuilder::Runtime::RemoteObject> inspectorValue = injectedScript.wrapObject(arguments->argumentAt(i), "console", generatePreview);
                    if (!inspectorValue) {
                        ASSERT_NOT_REACHED();
                        return;
                    }
                    jsonArgs->addItem(inspectorValue);
                }
            }
            jsonObj->setParameters(jsonArgs);
        }
    }
    if (consoleMessage->callStack()) {
        jsonObj->setStackTrace(consoleMessage->callStack()->buildInspectorArray());
        RefPtrWillBeRawPtr<ScriptAsyncCallStack> asyncCallStack = consoleMessage->callStack()->asyncCallStack();
        if (asyncCallStack)
            jsonObj->setAsyncStackTrace(asyncCallStack->buildInspectorObject());
    }
    if (consoleMessage->messageId())
        jsonObj->setMessageId(consoleMessage->messageId());
    if (consoleMessage->relatedMessageId())
        jsonObj->setRelatedMessageId(consoleMessage->relatedMessageId());
    frontend()->messageAdded(jsonObj);
    frontend()->flush();
}

} // namespace blink
