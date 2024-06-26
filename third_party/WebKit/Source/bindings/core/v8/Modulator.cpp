// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/Modulator.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/html/parser/HTMLScriptRunner.h"
#include "public/platform/Platform.h"
#include "wtf/StdLibExtras.h"

namespace blink {

Modulator::Modulator()
{

}

Modulator::~Modulator()
{

}

// ParsedSpecifier represents a parsed specifier, either
// - a non-bare specifier, parsed as a KURL as specced by
//   https://html.spec.whatwg.org/#resolve-a-module-specifier or
// - a bare specifier, stored as a String as-is.

// Non-import-maps cases:
// Bare specifiers should be rejected by callers as resolution errors.
// Then ParsedSpecifier represents the result of
// https://html.spec.whatwg.org/#resolve-a-module-specifier
// and behaves just like a KURL via GetUrl().

// Import-maps cases:
// In the import map spec, specifiers are handled mostly as strings,
// occasionally converted to/from URLs.
// In Blink, we pass ParsedSpecifier throughout the import map resolution,
// instead of passing String with occasionally converting to KURL.
// This avoid duplicated URL parsing.
class ParsedSpecifier final {
public:
    // Parse |specifier|, which may be a non-bare or bare specifier.
    // This implements
    // https://html.spec.whatwg.org/#resolve-a-module-specifier
    // but doesn't reject bare specifiers, which should be rejected by callers
    // if needed.
    static ParsedSpecifier create(const String& specifier, const KURL& base_url);

    enum class Type { kInvalid, kBare, kURL };

    Type getType() const
    {
        return m_type;
    }

    // Returns the string to be used as the key of import maps.
    // This is the bare specifier itself if type is kBare, or
    // serialized URL if type is kURL.
    String getImportMapKeyString() const;

    // Returns the URL, if type is kURL, or an null URL otherwise.
    KURL getUrl() const;

    bool isValid() const
    {
        return getType() != Type::kInvalid;
    }

private:
    // Invalid specifier.
    ParsedSpecifier()
        : m_type(Type::kInvalid)
    {
    }
    // Non-bare specifier.
    explicit ParsedSpecifier(const KURL& url)
        : m_type(Type::kURL)
        , m_url(url)
    {
    }
    // Bare specifier.
    explicit ParsedSpecifier(const String& bareSpecifier)
        : m_type(Type::kBare)
        , m_bareSpecifier(bareSpecifier)
    {
    }

    const Type m_type;
    const KURL m_url;
    const String m_bareSpecifier;
};

// <specdef href="https://html.spec.whatwg.org/#resolve-a-module-specifier">
// <specdef label="import-specifier"
// href="https://wicg.github.io/import-maps/#parse-a-url-like-import-specifier">
// This can return a kBare ParsedSpecifier for cases where the spec concepts
// listed above should return failure/null. The users of ParsedSpecifier should
// handle kBare cases properly, depending on contexts and whether import maps
// are enabled.
ParsedSpecifier ParsedSpecifier::create(const String& specifier, const KURL& base_url)
{
    // <spec step="1">Apply the URL parser to specifier. If the result is not
    // failure, return the result.</spec>
    //
    // <spec label="import-specifier" step="2">Let url be the result of parsing
    // specifier (with no base URL).</spec>
    KURL url(ParsedURLString, specifier);
    if (WTF::kNotFound != specifier.find("://") && url.isValid()) {
        // <spec label="import-specifier" step="4">If url¡¯s scheme is either a fetch
        // scheme or "std", then return url.</spec>
        //
        // TODO(hiroshige): This check is done in the callers of ParsedSpecifier.
        return ParsedSpecifier(url);
    }

    // <spec step="2">If specifier does not start with the character U+002F
    // SOLIDUS (/), the two-character sequence U+002E FULL STOP, U+002F SOLIDUS
    // (./), or the three-character sequence U+002E FULL STOP, U+002E FULL STOP,
    // U+002F SOLIDUS (../), return failure.</spec>
    //
    // <spec label="import-specifier" step="1">If specifier starts with "/", "./",
    // or "../", then:</spec>
    if (!specifier.startsWith("/") && !specifier.startsWith("./") && !specifier.startsWith("../")) {
        // Do not consider an empty specifier as a valid bare specifier.
        //
        // <spec
        // href="https://wicg.github.io/import-maps/#normalize-a-specifier-key"
        // step="1">If specifierKey is the empty string, then:</spec>
        if (specifier.isEmpty())
            return ParsedSpecifier();

        // <spec label="import-specifier" step="3">If url is failure, then return
        // null.</spec>
        return ParsedSpecifier(specifier);
    }

    // <spec step="3">Return the result of applying the URL parser to specifier
    // with base URL as the base URL.</spec>
    //
    // <spec label="import-specifier" step="1.1">Let url be the result of parsing
    // specifier with baseURL as the base URL.</spec>
    ASSERT(base_url.isValid());
    KURL absolute_url(base_url, specifier);
    // <spec label="import-specifier" step="1.3">Return url.</spec>
    if (absolute_url.isValid())
        return ParsedSpecifier(absolute_url);

    // <spec label="import-specifier" step="1.2">If url is failure, then return
    // null.</spec>
    return ParsedSpecifier();
}

String ParsedSpecifier::getImportMapKeyString() const
{
    switch (getType()) {
    case Type::kInvalid:
        return String();
    case Type::kBare:
        return m_bareSpecifier;
    case Type::kURL:
        return m_url.getUTF8String();
    }
    return String();
}

KURL ParsedSpecifier::getUrl() const
{
    switch (getType()) {
    case Type::kInvalid:
    case Type::kBare:
        return KURL();
    case Type::kURL:
        return m_url;
    }
    return KURL();
}


void Modulator::resolveDynamically(
    ScriptState* scriptState,
    const String& specifier,
    const KURL& url,
    //const ReferrerScriptInfo&,
    ScriptPromiseResolver* resolver)
{
    RELEASE_ASSERT(scriptState);
    if (!m_scriptState) {
        m_scriptState = scriptState;
    } else {
        RELEASE_ASSERT(scriptState == m_scriptState);
    }
    m_scriptState->ref();

//     char* output = (char*)malloc(0x100);
//     sprintf_s(output, 0x99, "Modulator::resolveDynamically: this:%p, m_scriptState:%p\n", this, m_scriptState);
//     OutputDebugStringA(output);
//     free(output);

    Platform::current()->mainThread()->postTask(FROM_HERE,
        WTF::bind(&Modulator::resolveDynamicallyDelay, this, specifier, url, resolver));
}

void Modulator::resolveDynamicallyDelay(
    const String& specifier,
    const KURL& url,
    //const ReferrerScriptInfo&,
    ScriptPromiseResolver* resolver)
{
    do {
        if (!m_scriptRunner.get())
            break;

//         char* output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "Modulator::resolveDynamicallyDelay: this:%p, m_scriptState:%p ScriptPromiseResolver:%p\n", this, m_scriptState, resolver);
//         OutputDebugStringA(output);
//         free(output);

        LocalDOMWindow* domWin = m_scriptState->domWindow();
        if (!domWin)
            break;
        Document* document = domWin->document();
        if (!document)
            break;

        ParsedSpecifier parsedSpecifier = ParsedSpecifier::create(specifier, url);
        if (!parsedSpecifier.isValid())
            break;

        if (parsedSpecifier.getType() == ParsedSpecifier::Type::kBare)
            break;
        KURL urlFull = parsedSpecifier.getUrl();

        bool b = m_scriptRunner->requestPendingModuleScript(document, /*parentModuleRecord*/nullptr, urlFull.getUTF8String(), resolver);
        if (b)
            resolver = nullptr;
    } while (false);

    m_scriptState->deref();

    if (resolver)
        resolver->reject();
}

void Modulator::shutdown()
{
    HeapHashMap<String, Member<ModuleRecord>>::iterator it = m_moduleRecordUrlMap.begin();
    for (; it != m_moduleRecordUrlMap.end(); ++it) {
        ModuleRecord* rec = it->value.get();
        rec->clearModule();
    }
//     if (m_scriptState) // weolar: !!!!!!!!!!
//         m_scriptState->deref();
}

void Modulator::setScriptRunner(HTMLScriptRunner* scriptRunner)
{
    ASSERT(!m_scriptRunner || m_scriptRunner == scriptRunner);
    m_scriptRunner = scriptRunner;
}

void Modulator::onModuleScriptLoader()
{
    if (m_scriptRunner.get())
        m_scriptRunner->executeScriptsWaitingForParsing();
}

void Modulator::add(ModuleRecord* record)
{
    if (-1 != record->getIdHash())
        m_moduleRecordIdMap.add(record->getIdHash(), record);

    m_moduleRecordUrlMap.add(record->url().getUTF8String(), record);
}

ModuleRecord* Modulator::getModuleRecordById(int id)
{
    HeapHashMap<int, Member<ModuleRecord>>::iterator it = m_moduleRecordIdMap.find(id);
    if (it != m_moduleRecordIdMap.end())
        return it->value.get();
    return nullptr;
}

ModuleRecord* Modulator::getModuleRecordByUrl(const KURL& url)
{
    HeapHashMap<String, Member<ModuleRecord>>::iterator it = m_moduleRecordUrlMap.find(url.getUTF8String());
    if (it != m_moduleRecordUrlMap.end())
        return it->value.get();
    return nullptr;
}

Modulator* Modulator::from(v8::Local<v8::Context> context)
{
    ScriptState* scriptState = ScriptState::from(context);
    if (!scriptState)
        return nullptr;
    return scriptState->modulator();
}

DEFINE_TRACE(Modulator)
{
#if ENABLE(OILPAN)
    visitor->trace(m_moduleRecordIdMap);
    visitor->trace(m_moduleRecordUrlMap);
    visitor->trace(m_scriptRunner); 
#endif
}

} // namespace blink
