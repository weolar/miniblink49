// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/ScriptSourceCode.h"

namespace blink {

ScriptSourceCode::ScriptSourceCode()
    : m_resource(0)
    , m_startPosition(TextPosition::minimumPosition())
{
}

ScriptSourceCode::ScriptSourceCode(const String& source, const KURL& url, const TextPosition& startPosition)
    : m_source(source)
    , m_resource(0)
    , m_url(url)
    , m_startPosition(startPosition)
{
    treatNullSourceAsEmpty();
    if (!m_url.isEmpty())
        m_url.removeFragmentIdentifier();
}

ScriptSourceCode::ScriptSourceCode(ScriptResource* resource)
    : m_source(resource->script())
    , m_resource(resource)
    , m_startPosition(TextPosition::minimumPosition())
{
    treatNullSourceAsEmpty();
}

ScriptSourceCode::ScriptSourceCode(PassRefPtrWillBeRawPtr<ScriptStreamer> streamer, ScriptResource* resource)
    : m_source(resource->script())
    , m_resource(resource)
    , m_streamer(streamer)
    , m_startPosition(TextPosition::minimumPosition())
{
    treatNullSourceAsEmpty();
}

ScriptSourceCode::~ScriptSourceCode()
{
}

DEFINE_TRACE(ScriptSourceCode)
{
    visitor->trace(m_streamer);
}

const KURL& ScriptSourceCode::url() const
{
    if (m_url.isEmpty() && m_resource) {
        m_url = m_resource->response().url();
        if (!m_url.isEmpty())
            m_url.removeFragmentIdentifier();
    }
    return m_url;
}

String ScriptSourceCode::sourceMapUrl() const
{
    if (!m_resource)
        return String();
    const ResourceResponse& response = m_resource->response();
    String sourceMapUrl = response.httpHeaderField("SourceMap");
    if (sourceMapUrl.isEmpty()) {
        // Try to get deprecated header.
        sourceMapUrl = response.httpHeaderField("X-SourceMap");
    }
    return sourceMapUrl;
}

void ScriptSourceCode::treatNullSourceAsEmpty()
{
    // ScriptSourceCode allows for the representation of the null/not-there-really ScriptSourceCode value.
    // Encoded by way of a m_source.isNull() being true, with the nullary constructor to be used to
    // construct such a value.
    //
    // Should the other constructors be passed a null string, that is interpreted as representing
    // the empty script. Consequently, we need to disambiguate between such null string occurrences.
    // Do that by converting the latter case's null strings into empty ones.
    if (m_source.isNull())
        m_source = "";
}

} // namespace blink
