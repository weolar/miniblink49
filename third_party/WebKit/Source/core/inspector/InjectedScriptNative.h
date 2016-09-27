// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InjectedScriptNative_h
#define InjectedScriptNative_h

#include "bindings/core/v8/V8GlobalValueMap.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/RefCounted.h"
#include <v8.h>

namespace blink {

class InjectedScriptNative final : public RefCounted<InjectedScriptNative> {
public:
    explicit InjectedScriptNative(v8::Isolate*);
    ~InjectedScriptNative();

    void setOnInjectedScriptHost(v8::Local<v8::Object>);
    static InjectedScriptNative* fromInjectedScriptHost(v8::Local<v8::Object>);

    int bind(v8::Local<v8::Value>, const String& groupName);
    void unbind(int id);
    v8::Local<v8::Value> objectForId(int id);

    void releaseObjectGroup(const String& groupName);
    String groupName(int objectId) const;

private:
    void addObjectToGroup(int objectId, const String& groupName);

    int m_lastBoundObjectId;
    v8::Isolate* m_isolate;
    V8GlobalValueMap<int, v8::Value, v8::kNotWeak> m_idToWrappedObject;
    typedef HashMap<int, String> IdToObjectGroupName;
    IdToObjectGroupName m_idToObjectGroupName;
    typedef HashMap<String, Vector<int>> NameToObjectGroup;
    NameToObjectGroup m_nameToObjectGroup;
};

} // namespace blink

#endif
