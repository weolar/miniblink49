// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/inspector/InjectedScriptNative.h"

#include "bindings/core/v8/V8HiddenValue.h"
#include "platform/JSONValues.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

InjectedScriptNative::InjectedScriptNative(v8::Isolate* isolate)
    : m_lastBoundObjectId(1)
    , m_isolate(isolate)
    , m_idToWrappedObject(m_isolate)
{
}

InjectedScriptNative::~InjectedScriptNative() { }

void InjectedScriptNative::setOnInjectedScriptHost(v8::Local<v8::Object> injectedScriptHost)
{
    v8::HandleScope handleScope(m_isolate);
    v8::Local<v8::External> external = v8::External::New(m_isolate, this);
    V8HiddenValue::setHiddenValue(m_isolate, injectedScriptHost, V8HiddenValue::injectedScriptNative(m_isolate), external);
}

InjectedScriptNative* InjectedScriptNative::fromInjectedScriptHost(v8::Local<v8::Object> injectedScriptObject)
{
    v8::Isolate* isolate = injectedScriptObject->GetIsolate();
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Value> value = V8HiddenValue::getHiddenValue(isolate, injectedScriptObject, V8HiddenValue::injectedScriptNative(isolate));
    ASSERT(!value.IsEmpty());
    v8::Local<v8::External> external = value.As<v8::External>();
    void* ptr = external->Value();
    ASSERT(ptr);
    return static_cast<InjectedScriptNative*>(ptr);
}

int InjectedScriptNative::bind(v8::Local<v8::Value> value, const String& groupName)
{
    if (m_lastBoundObjectId <= 0)
        m_lastBoundObjectId = 1;
    int id = m_lastBoundObjectId++;
    m_idToWrappedObject.Set(id, value);
    addObjectToGroup(id, groupName);
    return id;
}

void InjectedScriptNative::unbind(int id)
{
    m_idToWrappedObject.Remove(id);
    m_idToObjectGroupName.remove(id);
}

v8::Local<v8::Value> InjectedScriptNative::objectForId(int id)
{
    return m_idToWrappedObject.Get(id);
}

void InjectedScriptNative::addObjectToGroup(int objectId, const String& groupName)
{
    if (groupName.isEmpty())
        return;
    if (objectId <= 0)
        return;
    m_idToObjectGroupName.set(objectId, groupName);
    NameToObjectGroup::iterator groupIt = m_nameToObjectGroup.find(groupName);
    if (groupIt == m_nameToObjectGroup.end())
        m_nameToObjectGroup.set(groupName, Vector<int>()).storedValue->value.append(objectId);
    else
        groupIt->value.append(objectId);
}

void InjectedScriptNative::releaseObjectGroup(const String& groupName)
{
    if (groupName.isEmpty())
        return;
    NameToObjectGroup::iterator groupIt = m_nameToObjectGroup.find(groupName);
    if (groupIt == m_nameToObjectGroup.end())
        return;
    for (int id : groupIt->value)
        unbind(id);
    m_nameToObjectGroup.remove(groupIt);
}

String InjectedScriptNative::groupName(int objectId) const
{
    if (objectId <= 0)
        return String();
    return m_idToObjectGroupName.get(objectId);
}

} // namespace blink

