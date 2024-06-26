// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DynamicImportScriptLoader_h
#define DynamicImportScriptLoader_h

#include "core/dom/ScriptLoaderClient.h"
#include "core/dom/PendingScript.h"
#include "core/fetch/ScriptResource.h"
#include "core/fetch/FetchRequest.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "platform/heap/Handle.h"

namespace blink {

class Document;

class DynamicImportScriptLoader
    : public NoBaseWillBeGarbageCollectedFinalized<DynamicImportScriptLoader>
    , private ScriptResourceClient {
public:
    DynamicImportScriptLoader(ScriptPromiseResolver* resolver);
    ~DynamicImportScriptLoader();
    
    bool fetchScript(Document* doc, const String& sourceUrl, FetchRequest::DeferOption);

private:
    // ResourceClient
    void notifyFinished(Resource*) override;

    ResourcePtr<ScriptResource> m_resource;
    String m_characterEncoding;
    String m_fallbackCharacterEncoding;
    PendingScript m_pendingScript;
    ScriptPromiseResolver* m_resolver;
};

} // namespace blink

#endif // DynamicImportScriptLoader