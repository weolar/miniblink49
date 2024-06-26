// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ModuleRecord_h
#define ModuleRecord_h

#include <v8.h>
#include "third_party/WebKit/Source/bindings/core/v8/ScopedPersistent.h"
#include "third_party/WebKit/Source/bindings/core/v8/ScriptPromiseResolver.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/platform/heap/Visitor.h"
#include "third_party/WebKit/Source/platform/heap/Heap.h"

namespace blink {

class ScriptPromiseResolver;
class MyScriptPromiseResolver;

class ModuleRecord : public GarbageCollectedFinalized<ModuleRecord> {
public:
    ModuleRecord(const KURL& url, v8::Isolate* isolate, v8::Local<v8::Module> module, ScriptPromiseResolver* resolver);
    ~ModuleRecord();

    int getIdHash() const { return m_idHash; }

    void setModule(v8::Isolate* isolate, v8::Local<v8::Module> module);
    void clearModule();
    v8::Local<v8::Module> getModule(v8::Isolate* isolate) const { return m_module.newLocal(isolate); }

    KURL url() const { return m_url; }
    void executeModuleScript(v8::Isolate* isolate, v8::Local<v8::Context> context);

private:
    DECLARE_TRACE();
    int m_idHash;
    KURL m_url;
    bool m_hasEvaluate;
    Member<ScriptPromiseResolver> m_parentResolver;
    ScopedPersistent<v8::Module> m_module;
};

} // namespace blink

#endif // ModuleRecord_h
