/*
 * Copyright (C) 2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2005 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Nicholas Shanks <webkit@nickshanks.com>
 * Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2012 Adobe Systems Incorporated. All rights reserved.
 * Copyright (C) 2012 Intel Corporation. All rights reserved.
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
#include "core/css/parser/CSSParserMode.h"

#include "core/dom/Document.h"
#include "core/frame/Settings.h"
#include "core/frame/csp/ContentSecurityPolicy.h"

namespace blink {

CSSParserContext::CSSParserContext(CSSParserMode mode, UseCounter* useCounter)
    : m_mode(mode)
    , m_isHTMLDocument(false)
    , m_useLegacyBackgroundSizeShorthandBehavior(false)
    , m_shouldCheckContentSecurityPolicy(DoNotCheckContentSecurityPolicy)
    , m_useCounter(useCounter)
{
}

CSSParserContext::CSSParserContext(const Document& document, UseCounter* useCounter, const KURL& baseURL, const String& charset)
    : m_baseURL(baseURL.isNull() ? document.baseURL() : baseURL)
    , m_charset(charset)
    , m_mode(document.inQuirksMode() ? HTMLQuirksMode : HTMLStandardMode)
    , m_referrer(m_baseURL.strippedForUseAsReferrer(), document.referrerPolicy())
    , m_isHTMLDocument(document.isHTMLDocument())
    , m_useLegacyBackgroundSizeShorthandBehavior(document.settings() ? document.settings()->useLegacyBackgroundSizeShorthandBehavior() : false)
    , m_shouldCheckContentSecurityPolicy(DoNotCheckContentSecurityPolicy)
    , m_useCounter(useCounter)
{
    if (ContentSecurityPolicy::shouldBypassMainWorld(&document))
        m_shouldCheckContentSecurityPolicy = DoNotCheckContentSecurityPolicy;
    else
        m_shouldCheckContentSecurityPolicy = CheckContentSecurityPolicy;
}

CSSParserContext::CSSParserContext(const CSSParserContext& other, UseCounter* useCounter)
    : m_baseURL(other.m_baseURL)
    , m_charset(other.m_charset)
    , m_mode(other.m_mode)
    , m_referrer(other.m_referrer)
    , m_isHTMLDocument(other.m_isHTMLDocument)
    , m_useLegacyBackgroundSizeShorthandBehavior(other.m_useLegacyBackgroundSizeShorthandBehavior)
    , m_shouldCheckContentSecurityPolicy(other.m_shouldCheckContentSecurityPolicy)
    , m_useCounter(useCounter)
{
}

bool CSSParserContext::operator==(const CSSParserContext& other) const
{
    return m_baseURL == other.m_baseURL
        && m_charset == other.m_charset
        && m_mode == other.m_mode
        && m_isHTMLDocument == other.m_isHTMLDocument
        && m_useLegacyBackgroundSizeShorthandBehavior == other.m_useLegacyBackgroundSizeShorthandBehavior;
}

const CSSParserContext& strictCSSParserContext()
{
    DEFINE_STATIC_LOCAL(CSSParserContext, strictContext, (HTMLStandardMode, 0));
    return strictContext;
}

KURL CSSParserContext::completeURL(const String& url) const
{
    if (url.isNull())
        return KURL();
    if (charset().isEmpty())
        return KURL(baseURL(), url);
    return KURL(baseURL(), url, charset());
}

} // namespace blink
