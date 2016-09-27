/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * (C) 2002-2003 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2002, 2005, 2006, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/css/StyleRuleImport.h"

#include "core/css/StyleSheetContents.h"
#include "core/dom/Document.h"
#include "core/fetch/CSSStyleSheetResource.h"
#include "core/fetch/FetchInitiatorTypeNames.h"
#include "core/fetch/FetchRequest.h"
#include "core/fetch/ResourceFetcher.h"

namespace blink {

PassRefPtrWillBeRawPtr<StyleRuleImport> StyleRuleImport::create(const String& href, PassRefPtrWillBeRawPtr<MediaQuerySet> media)
{
    return adoptRefWillBeNoop(new StyleRuleImport(href, media));
}

StyleRuleImport::StyleRuleImport(const String& href, PassRefPtrWillBeRawPtr<MediaQuerySet> media)
    : StyleRuleBase(Import)
    , m_parentStyleSheet(nullptr)
    , m_styleSheetClient(this)
    , m_strHref(href)
    , m_mediaQueries(media)
    , m_resource(0)
    , m_loading(false)
{
    if (!m_mediaQueries)
        m_mediaQueries = MediaQuerySet::create(String());
}

StyleRuleImport::~StyleRuleImport()
{
#if !ENABLE(OILPAN)
    if (m_styleSheet)
        m_styleSheet->clearOwnerRule();
#endif
    if (m_resource)
        m_resource->removeClient(&m_styleSheetClient);
}

DEFINE_TRACE_AFTER_DISPATCH(StyleRuleImport)
{
    visitor->trace(m_parentStyleSheet);
    visitor->trace(m_mediaQueries);
    visitor->trace(m_styleSheet);
    StyleRuleBase::traceAfterDispatch(visitor);
}

void StyleRuleImport::setCSSStyleSheet(const String& href, const KURL& baseURL, const String& charset, const CSSStyleSheetResource* cachedStyleSheet)
{
    if (m_styleSheet)
        m_styleSheet->clearOwnerRule();

    CSSParserContext context = m_parentStyleSheet ? m_parentStyleSheet->parserContext() : strictCSSParserContext();
    context.setCharset(charset);
    Document* document = m_parentStyleSheet ? m_parentStyleSheet->singleOwnerDocument() : 0;
    if (!baseURL.isNull()) {
        context.setBaseURL(baseURL);
        if (document)
            context.setReferrer(Referrer(baseURL.strippedForUseAsReferrer(), document->referrerPolicy()));
    }

    m_styleSheet = StyleSheetContents::create(this, href, context);

    m_styleSheet->parseAuthorStyleSheet(cachedStyleSheet, document ? document->securityOrigin() : 0);

    m_loading = false;

    if (m_parentStyleSheet) {
        m_parentStyleSheet->notifyLoadedSheet(cachedStyleSheet);
        m_parentStyleSheet->checkLoaded();
    }
}

bool StyleRuleImport::isLoading() const
{
    return m_loading || (m_styleSheet && m_styleSheet->isLoading());
}

void StyleRuleImport::requestStyleSheet()
{
    if (!m_parentStyleSheet)
        return;
    Document* document = m_parentStyleSheet->singleOwnerDocument();
    if (!document)
        return;

    ResourceFetcher* fetcher = document->fetcher();
    if (!fetcher)
        return;

    KURL absURL;
    if (!m_parentStyleSheet->baseURL().isNull()) {
        // use parent styleheet's URL as the base URL
        absURL = KURL(m_parentStyleSheet->baseURL(), m_strHref);
    } else {
        absURL = document->completeURL(m_strHref);
    }

    // Check for a cycle in our import chain.  If we encounter a stylesheet
    // in our parent chain with the same URL, then just bail.
    StyleSheetContents* rootSheet = m_parentStyleSheet;
    for (StyleSheetContents* sheet = m_parentStyleSheet; sheet; sheet = sheet->parentStyleSheet()) {
        if (equalIgnoringFragmentIdentifier(absURL, sheet->baseURL())
            || equalIgnoringFragmentIdentifier(absURL, document->completeURL(sheet->originalURL())))
            return;
        rootSheet = sheet;
    }

    FetchRequest request(ResourceRequest(absURL), FetchInitiatorTypeNames::css, m_parentStyleSheet->charset());
    m_resource = CSSStyleSheetResource::fetch(request, fetcher);
    if (m_resource) {
        // if the import rule is issued dynamically, the sheet may be
        // removed from the pending sheet count, so let the doc know
        // the sheet being imported is pending.
        if (m_parentStyleSheet && m_parentStyleSheet->loadCompleted() && rootSheet == m_parentStyleSheet)
            m_parentStyleSheet->startLoadingDynamicSheet();
        m_loading = true;
        m_resource->addClient(&m_styleSheetClient);
    }
}

} // namespace blink
