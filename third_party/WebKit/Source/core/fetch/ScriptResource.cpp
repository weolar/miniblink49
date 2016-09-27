/*
    Copyright (C) 1998 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2001 Dirk Mueller (mueller@kde.org)
    Copyright (C) 2002 Waldo Bastian (bastian@kde.org)
    Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
    Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

    This class provides all functionality needed for loading images, style sheets and html
    pages from the web. It has a memory cache for these objects.
*/

#include "config.h"
#include "core/fetch/ScriptResource.h"

#include "core/fetch/FetchRequest.h"
#include "core/fetch/ResourceClientWalker.h"
#include "core/fetch/ResourceFetcher.h"
#include "platform/MIMETypeRegistry.h"
#include "platform/SharedBuffer.h"
#include "platform/network/HTTPParsers.h"

namespace blink {

ResourcePtr<ScriptResource> ScriptResource::fetch(FetchRequest& request, ResourceFetcher* fetcher)
{
    ASSERT(request.resourceRequest().frameType() == WebURLRequest::FrameTypeNone);
    request.mutableResourceRequest().setRequestContext(WebURLRequest::RequestContextScript);
    return toScriptResource(fetcher->requestResource(request, ScriptResourceFactory()));
}

ScriptResource::ScriptResource(const ResourceRequest& resourceRequest, const String& charset)
    : TextResource(resourceRequest, Script, "application/javascript", charset)
{
    DEFINE_STATIC_LOCAL(const AtomicString, acceptScript, ("*/*", AtomicString::ConstructFromLiteral));

    // It's javascript we want.
    // But some websites think their scripts are <some wrong mimetype here>
    // and refuse to serve them if we only accept application/x-javascript.
    setAccept(acceptScript);
}

ScriptResource::~ScriptResource()
{
}

void ScriptResource::didAddClient(ResourceClient* client)
{
    ASSERT(client->resourceClientType() == ScriptResourceClient::expectedType());
    Resource::didAddClient(client);
}

void ScriptResource::appendData(const char* data, unsigned length)
{
    Resource::appendData(data, length);
    ResourceClientWalker<ScriptResourceClient> walker(m_clients);
    while (ScriptResourceClient* client = walker.next())
        client->notifyAppendData(this);
}

AtomicString ScriptResource::mimeType() const
{
    return extractMIMETypeFromMediaType(m_response.httpHeaderField("Content-Type")).lower();
}

const String& ScriptResource::script()
{
    ASSERT(!isPurgeable());
    ASSERT(isLoaded());

    if (!m_script && m_data) {
        String script = decodedText();
        m_data.clear();
        // We lie a it here and claim that script counts as encoded data (even though it's really decoded data).
        // That's because the MemoryCache thinks that it can clear out decoded data by calling destroyDecodedData(),
        // but we can't destroy script in destroyDecodedData because that's our only copy of the data!
        setEncodedSize(script.sizeInBytes());
        m_script = AtomicString(script);
    }

    return m_script.string();
}

bool ScriptResource::mimeTypeAllowedByNosniff() const
{
    return parseContentTypeOptionsHeader(m_response.httpHeaderField("X-Content-Type-Options")) != ContentTypeOptionsNosniff || MIMETypeRegistry::isSupportedJavaScriptMIMEType(mimeType());
}

} // namespace blink
