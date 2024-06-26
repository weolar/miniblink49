// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if 0

#include "core/dom/DynamicImportScriptLoader.h"
#include "core/dom/Document.h"
#include "core/html/parser/HTMLParserIdioms.h"

namespace blink {

DynamicImportScriptLoader::DynamicImportScriptLoader(ScriptPromiseResolver* resolver)
    : m_resolver(resolver)
{

}

DynamicImportScriptLoader::~DynamicImportScriptLoader()
{

}

bool DynamicImportScriptLoader::fetchScript(Document* doc, const String& sourceUrl, FetchRequest::DeferOption defer)
{
    if (!stripLeadingAndTrailingHTMLSpaces(sourceUrl).isEmpty()) {
        FetchRequest request(ResourceRequest(doc->completeURL(sourceUrl)), "script");
        request.setCharset("UTF-8");
        request.setDefer(defer);

        m_resource = ScriptResource::fetch(request, doc->fetcher());
    }

    if (m_resource)
        return true;
    return false;
}

void DynamicImportScriptLoader::notifyFinished(Resource* resource)
{

}

} // namespace blink

#endif