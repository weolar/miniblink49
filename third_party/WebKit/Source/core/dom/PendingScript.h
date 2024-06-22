/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PendingScript_h
#define PendingScript_h

#include "core/CoreExport.h"
#include "core/fetch/ResourceOwner.h"
#include "core/fetch/ScriptResource.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/TextPosition.h"

#include "v8.h"
#include "third_party/WebKit/Source/bindings/core/v8/ScopedPersistent.h"

namespace blink {

class Element;
class Document;
class ScriptSourceCode;
class ScriptStreamer;
class ModuleScriptLoader;
class ModuleRecord;
class HTMLScriptRunner;
class ScriptPromiseResolver;
class MyScriptPromiseResolver;

// A container for an external script which may be loaded and executed.
//
// A ResourcePtr alone does not prevent the underlying Resource
// from purging its data buffer. This class holds a dummy client open for its
// lifetime in order to guarantee that the data buffer will not be purged.
class CORE_EXPORT PendingScript final 
    : public GarbageCollectedFinalized<PendingScript>
    , public ResourceOwner<ScriptResource> {
    //ALLOW_ONLY_INLINE_ALLOCATION();
    //USING_GARBAGE_COLLECTED_MIXIN(PendingScript);
    //USING_PRE_FINALIZER(PendingScript, dispose);
    WTF_MAKE_NONCOPYABLE(PendingScript);
public:
    enum Type {
        ParsingBlocking,
        Deferred,
        Async
    };

    static PendingScript* create(Element*, ScriptResource*);

    //PendingScript(const PendingScript&);
    ~PendingScript();

    //PendingScript& operator=(const PendingScript&);

    TextPosition startingPosition() const { return m_startingPosition; }
    void setStartingPosition(const TextPosition& position) { m_startingPosition = position; }

    void watchForLoad(ScriptResourceClient*);
    void stopWatchingForLoad(ScriptResourceClient*);

    Element* element() const { return m_element.get(); }
    void setElement(Element*);
    //PassRefPtrWillBeRawPtr<Element> releaseElementAndClear();

    void setScriptResource(ScriptResource*);

    virtual void notifyFinished(Resource*);
    virtual void notifyAppendData(ScriptResource*);

    DECLARE_TRACE();

    ScriptSourceCode getSource(const KURL& documentURL, bool& errorOccurred) const;

    void setStreamer(PassRefPtrWillBeRawPtr<ScriptStreamer>);

    bool isReady() const;

    //////////////////////////////////////////////////////////////////////////
    void dispose(ScriptResourceClient* client);
    bool isModule() const;
    bool hadGetModuleDepend() const;
    bool requesetModuleScript(Document* document, const ModuleRecord* parentModuleRecord, const String& sourceUrl, ScriptPromiseResolver* resolver);
    Vector<String> compileModuleAndRequestDepend(HTMLScriptRunner* scriptRunner, Document* document);
    //v8::Local<v8::Module> getModule(v8::Isolate* isolate) const { return m_module.newLocal(isolate); }
    ModuleRecord* getModuleRecord() const { return m_moduleRecord; }
    bool isWatchingForLoad() const { return m_watchingForLoad; }
//     bool copyIfNeeded(const PendingScript& other);
//     void setCopying();
    bool hasModuleScriptString() const;
    void setModuleScriptString(const String& str);
private:
    void setGetModuleDepend();
    //////////////////////////////////////////////////////////////////////////

private:
    //PendingScript();
    PendingScript(Element*, ScriptResource*);
    bool m_watchingForLoad;

    //////////////////////////////////////////////////////////////////////////模块相关实现
    bool m_isModule;
    bool m_hadGetModuleDepend;
    bool m_isCopying; // 临时给executeScriptsWaitingForParsing用，用来查找哪个拷贝是m_scriptsToExecuteAfterParsing里面的
    Member<ModuleScriptLoader> m_moduleScriptLoader; // 没有<script>标签而是js里面import引入的脚本
    Member<ModuleRecord> m_moduleRecord;
    //Member<ScriptResourceClient> m_client;
    String m_moduleScriptString;
    //////////////////////////////////////////////////////////////////////////

    RefPtrWillBeMember<Element> m_element;
    TextPosition m_startingPosition; // Only used for inline script tags.

    RefPtrWillBeMember<ScriptStreamer> m_streamer;
};

} // namespace blink

#endif // PendingScript_h
