/*
    Copyright (C) 2010 Rob Buis <rwlbuis@gmail.com>
    Copyright (C) 2011 Cosmin Truta <ctruta@gmail.com>
    Copyright (C) 2012 University of Szeged
    Copyright (C) 2012 Renata Hodovan <reni@webkit.org>

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
*/

#include "config.h"

#include "core/fetch/DocumentResource.h"

#include "core/dom/XMLDocument.h"
#include "core/fetch/FetchRequest.h"
#include "core/fetch/ResourceFetcher.h"
#include "platform/SharedBuffer.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

ResourcePtr<DocumentResource> DocumentResource::fetchSVGDocument(FetchRequest& request, ResourceFetcher* fetcher)
{
    ASSERT(request.resourceRequest().frameType() == WebURLRequest::FrameTypeNone);
    request.mutableResourceRequest().setRequestContext(WebURLRequest::RequestContextImage);
    return toDocumentResource(fetcher->requestResource(request, SVGDocumentResourceFactory()));
}

DocumentResource::DocumentResource(const ResourceRequest& request, Type type)
    : Resource(request, type)
    , m_decoder(TextResourceDecoder::create("application/xml"))
{
    // FIXME: We'll support more types to support HTMLImports.
    ASSERT(type == SVGDocument);
}

DocumentResource::~DocumentResource()
{
}

DEFINE_TRACE(DocumentResource)
{
    visitor->trace(m_document);
    Resource::trace(visitor);
}

void DocumentResource::setEncoding(const String& chs)
{
    m_decoder->setEncoding(chs, TextResourceDecoder::EncodingFromHTTPHeader);
}

String DocumentResource::encoding() const
{
    return m_decoder->encoding().name();
}

void DocumentResource::checkNotify()
{
    if (m_data) {
        StringBuilder decodedText;
        decodedText.append(m_decoder->decode(m_data->data(), m_data->size()));
        decodedText.append(m_decoder->flush());
        // We don't need to create a new frame because the new document belongs to the parent UseElement.
        m_document = createDocument(response().url());
        m_document->setContent(decodedText.toString());
    }
    Resource::checkNotify();
}

PassRefPtrWillBeRawPtr<Document> DocumentResource::createDocument(const KURL& url)
{
    switch (type()) {
    case SVGDocument:
        return XMLDocument::createSVG(DocumentInit(url));
    default:
        // FIXME: We'll add more types to support HTMLImports.
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

}
