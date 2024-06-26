// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/ModuleRecord.h"
#include "bindings/core/v8/Modulator.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "wtf/StdLibExtras.h"

namespace blink {

ModuleRecord::ModuleRecord(const KURL& url, v8::Isolate* isolate, v8::Local<v8::Module> module, ScriptPromiseResolver* resolver)
{
    m_url = url;
    m_hasEvaluate = false;
    m_parentResolver = resolver;

//     char* output = (char*)malloc(0x100);
//     sprintf_s(output, 0x99, "ModuleRecord::ModuleRecord: this:%p, ScriptPromiseResolver:%p\n", this, m_parentResolver);
//     OutputDebugStringA(output);
//     free(output);

    v8::Isolate::Scope isoldateScope(isolate);
    v8::HandleScope handleScope(isolate);

    m_idHash = -1;
    if (!module.IsEmpty())
        m_idHash = module->GetIdentityHash();
    m_module.set(isolate, module);
}

ModuleRecord::~ModuleRecord()
{
//     char* output = (char*)malloc(0x100);
//     sprintf_s(output, 0x99, "ModuleRecord::~ModuleRecord: this:%p, ScriptPromiseResolver:%p\n", this, m_parentResolver);
//     OutputDebugStringA(output);
//     free(output);

    m_module.clear();

    if (m_parentResolver.get())
        m_parentResolver->reject();
}

void ModuleRecord::setModule(v8::Isolate* isolate, v8::Local<v8::Module> module)
{
    ASSERT(m_module.isEmpty() && !module.IsEmpty());
    m_idHash = module->GetIdentityHash();
    m_module.set(isolate, module);
}

void ModuleRecord::clearModule()
{
    m_module.clear();
}

static v8::MaybeLocal<v8::Module> onResolveCallback(v8::Local<v8::Context> context, v8::Local<v8::String> specifier, v8::Local<v8::Module> referrer)
{
    Modulator* modulator = Modulator::from(context);
    if (!modulator)
        return v8::MaybeLocal<v8::Module>();

    ModuleRecord* referrerRecord = modulator->getModuleRecordById(referrer->GetIdentityHash());
    if (!referrerRecord)
        return v8::MaybeLocal<v8::Module>();

    String specifierUrl = toCoreString(specifier);
    KURL baseUrl = referrerRecord->url();
    KURL url(baseUrl, specifierUrl);

    ModuleRecord* record = modulator->getModuleRecordByUrl(url);
    if (!record)
        return v8::MaybeLocal<v8::Module>();

    return record->getModule(context->GetIsolate());
}

void ModuleRecord::executeModuleScript(v8::Isolate* isolate, v8::Local<v8::Context> context)
{
    if (m_hasEvaluate)
        return;
    v8::Isolate::Scope isoldateScope(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Module> v8module = m_module.newLocal(isolate);
    if (v8module.IsEmpty()) {
#if V8_MAJOR_VERSION >= 7
        if (m_parentResolver.get()) 
            m_parentResolver->reject();
#endif
        m_parentResolver = nullptr;
        return;
    }

    v8::Maybe<bool> b = v8module->InstantiateModule(context, onResolveCallback);
    if (b.IsNothing() || !b.ToChecked()) {
#if V8_MAJOR_VERSION >= 7
        if (m_parentResolver.get())
            m_parentResolver->reject();
#endif
        m_parentResolver = nullptr;
        return;
    }

    if (m_parentResolver.get()) {
#if V8_MAJOR_VERSION >= 7
        v8::Local<v8::Value> moduleNamespace = v8module->GetModuleNamespace();
        m_parentResolver->resolve(moduleNamespace);
#endif
        m_parentResolver = nullptr;
    }

    m_hasEvaluate = true;
    v8module->Evaluate(context);
}

DEFINE_TRACE(ModuleRecord)
{
#if ENABLE(OILPAN)
    visitor->trace(m_parentResolver);
#endif
}

} // namespace blink
