// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform\v8_inspector\protocol/Console.h"

#include "platform/inspector_protocol/DispatcherBase.h"
#include "platform/inspector_protocol/Parser.h"

namespace blink {
namespace protocol {
namespace Console {

// ------------- Enum values from types.

const char Metainfo::domainName[] = "Console";
const char Metainfo::commandPrefix[] = "Console.";

const char* ConsoleMessage::SourceEnum::Xml = "xml";
const char* ConsoleMessage::SourceEnum::Javascript = "javascript";
const char* ConsoleMessage::SourceEnum::Network = "network";
const char* ConsoleMessage::SourceEnum::ConsoleApi = "console-api";
const char* ConsoleMessage::SourceEnum::Storage = "storage";
const char* ConsoleMessage::SourceEnum::Appcache = "appcache";
const char* ConsoleMessage::SourceEnum::Rendering = "rendering";
const char* ConsoleMessage::SourceEnum::Security = "security";
const char* ConsoleMessage::SourceEnum::Other = "other";
const char* ConsoleMessage::SourceEnum::Deprecation = "deprecation";
const char* ConsoleMessage::SourceEnum::Worker = "worker";

const char* ConsoleMessage::LevelEnum::Log = "log";
const char* ConsoleMessage::LevelEnum::Warning = "warning";
const char* ConsoleMessage::LevelEnum::Error = "error";
const char* ConsoleMessage::LevelEnum::Debug = "debug";
const char* ConsoleMessage::LevelEnum::Info = "info";

std::unique_ptr<ConsoleMessage> ConsoleMessage::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<ConsoleMessage> result(new ConsoleMessage());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* sourceValue = object->get("source");
    errors->setName("source");
    result->m_source = ValueConversions<String16>::parse(sourceValue, errors);
    protocol::Value* levelValue = object->get("level");
    errors->setName("level");
    result->m_level = ValueConversions<String16>::parse(levelValue, errors);
    protocol::Value* textValue = object->get("text");
    errors->setName("text");
    result->m_text = ValueConversions<String16>::parse(textValue, errors);
    protocol::Value* urlValue = object->get("url");
    if (urlValue) {
        errors->setName("url");
        result->m_url = ValueConversions<String16>::parse(urlValue, errors);
    }
    protocol::Value* lineValue = object->get("line");
    if (lineValue) {
        errors->setName("line");
        result->m_line = ValueConversions<int>::parse(lineValue, errors);
    }
    protocol::Value* columnValue = object->get("column");
    if (columnValue) {
        errors->setName("column");
        result->m_column = ValueConversions<int>::parse(columnValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> ConsoleMessage::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("source", ValueConversions<String16>::serialize(m_source));
    result->setValue("level", ValueConversions<String16>::serialize(m_level));
    result->setValue("text", ValueConversions<String16>::serialize(m_text));
    if (m_url.isJust())
        result->setValue("url", ValueConversions<String16>::serialize(m_url.fromJust()));
    if (m_line.isJust())
        result->setValue("line", ValueConversions<int>::serialize(m_line.fromJust()));
    if (m_column.isJust())
        result->setValue("column", ValueConversions<int>::serialize(m_column.fromJust()));
    return result;
}

std::unique_ptr<ConsoleMessage> ConsoleMessage::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

// ------------- Enum values from params.


// ------------- Frontend notifications.

void Frontend::messageAdded(std::unique_ptr<protocol::Console::ConsoleMessage> message)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Console.messageAdded");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("message", ValueConversions<protocol::Console::ConsoleMessage>::serialize(message.get()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::messageRepeatCountUpdated(int count, double timestamp)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Console.messageRepeatCountUpdated");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("count", ValueConversions<int>::serialize(count));
    paramsObject->setValue("timestamp", ValueConversions<double>::serialize(timestamp));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::messagesCleared()
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Console.messagesCleared");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

// --------------------- Dispatcher.

class DispatcherImpl : public protocol::DispatcherBase {
public:
    DispatcherImpl(FrontendChannel* frontendChannel, Backend* backend)
        : DispatcherBase(frontendChannel)
        , m_backend(backend) {
        m_dispatchMap["Console.enable"] = &DispatcherImpl::enable;
        m_dispatchMap["Console.disable"] = &DispatcherImpl::disable;
        m_dispatchMap["Console.clearMessages"] = &DispatcherImpl::clearMessages;
    }
    ~DispatcherImpl() override { }
    void dispatch(int callId, const String16& method, std::unique_ptr<protocol::DictionaryValue> messageObject) override;

protected:
    using CallHandler = void (DispatcherImpl::*)(int callId, std::unique_ptr<DictionaryValue> messageObject, ErrorSupport* errors);
    using DispatchMap = protocol::HashMap<String16, CallHandler>;
    DispatchMap m_dispatchMap;

    void enable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void disable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void clearMessages(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);

    Backend* m_backend;
};

void DispatcherImpl::dispatch(int callId, const String16& method, std::unique_ptr<protocol::DictionaryValue> messageObject)
{
    protocol::HashMap<String16, CallHandler>::iterator it = m_dispatchMap.find(method);
    if (it == m_dispatchMap.end()) {
        reportProtocolError(callId, MethodNotFound, "'" + method + "' wasn't found", nullptr);
        return;
    }

    protocol::ErrorSupport errors;
    (this->*(it->second))(callId, std::move(messageObject), &errors);
}


void DispatcherImpl::enable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->enable(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::disable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->disable(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::clearMessages(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->clearMessages(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

// static
void Dispatcher::wire(UberDispatcher* dispatcher, Backend* backend)
{
    dispatcher->registerBackend("Console", wrapUnique(new DispatcherImpl(dispatcher->channel(), backend)));
}

} // Console
} // namespace protocol
} // namespace blink
