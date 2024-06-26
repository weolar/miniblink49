// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Modulator_h
#define Modulator_h

#include "third_party/WebKit/Source/bindings/core/v8/ModuleRecord.h"
#include "third_party/WebKit/Source/bindings/core/v8/ScopedPersistent.h"
#include "third_party/WebKit/Source/core/html/parser/HTMLScriptRunner.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/platform/heap/HeapAllocator.h"
#include <v8.h>

namespace blink {

class ScriptPromiseResolver;
class HTMLScriptRunner;
class ScriptState;

// class MyScriptPromiseResolver {
// public:
//     MyScriptPromiseResolver(v8::Local<v8::Context> context, v8::Local<v8::Promise::Resolver> resolver);
//     void reject();
//     void resolve(v8::Local<v8::Value> val);
// 
//     v8::Persistent<v8::Promise::Resolver>* m_resolver;
//     v8::Persistent<v8::Context>* m_context;
// };

class Modulator : public GarbageCollectedFinalized<Modulator> {
public:
    static Modulator* create()
    {
        return (new Modulator());
    }
    ~Modulator();

    ModuleRecord* getModuleRecordById(int id);
    ModuleRecord* getModuleRecordByUrl(const KURL& url);

    void resolveDynamically(
        ScriptState* scriptState,
        const String& specifier,
        const KURL& url,
        //const ReferrerScriptInfo&,
        ScriptPromiseResolver*);

    void add(ModuleRecord* record);
    static Modulator* from(v8::Local<v8::Context> context);

    void shutdown();
    void setScriptRunner(HTMLScriptRunner* scriptRunner);
    void onModuleScriptLoader();

private:
    Modulator();

    void resolveDynamicallyDelay(const String& specifier, const KURL& url,
        //const ReferrerScriptInfo&,
        ScriptPromiseResolver*);

    DECLARE_TRACE();

    Member<HTMLScriptRunner> m_scriptRunner;

    ScriptState* m_scriptState;

    HeapHashMap<int, Member<ModuleRecord> > m_moduleRecordIdMap;
    HeapHashMap<String, Member<ModuleRecord> > m_moduleRecordUrlMap; // 这两个map是重复的，为了查询方便才这样搞
};

} // namespace blink

#endif // Modulator_h

