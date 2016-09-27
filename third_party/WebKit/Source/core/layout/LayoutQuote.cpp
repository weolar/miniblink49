/**
 * Copyright (C) 2011 Nokia Inc.  All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
 *
 */

#include "config.h"
#include "core/layout/LayoutQuote.h"

#include "core/layout/LayoutTextFragment.h"
#include "core/layout/LayoutView.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/AtomicString.h"

#include <algorithm>

namespace blink {

LayoutQuote::LayoutQuote(Document* node, QuoteType quote)
    : LayoutInline(nullptr)
    , m_type(quote)
    , m_depth(0)
    , m_next(nullptr)
    , m_previous(nullptr)
    , m_attached(false)
{
    setDocumentForAnonymous(node);
}

LayoutQuote::~LayoutQuote()
{
    ASSERT(!m_attached);
    ASSERT(!m_next && !m_previous);
}

void LayoutQuote::willBeDestroyed()
{
    detachQuote();
    LayoutInline::willBeDestroyed();
}

void LayoutQuote::willBeRemovedFromTree()
{
    LayoutInline::willBeRemovedFromTree();
    detachQuote();
}

void LayoutQuote::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutInline::styleDidChange(diff, oldStyle);
    updateText();
}

struct Language {
    const char* lang;
    UChar open1;
    UChar close1;
    UChar open2;
    UChar close2;
    QuotesData* data;

    bool operator<(const Language& b) const { return strcmp(lang, b.lang) < 0; }
};

// Table of quotes from http://www.whatwg.org/specs/web-apps/current-work/multipage/rendering.html#quote
Language languages[] = {
    { "af",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "agq",           0x201e, 0x201d, 0x201a, 0x2019, 0 },
    { "ak",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "am",            0x00ab, 0x00bb, 0x2039, 0x203a, 0 },
    { "ar",            0x201d, 0x201c, 0x2019, 0x2018, 0 },
    { "asa",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "az-cyrl",       0x00ab, 0x00bb, 0x2039, 0x203a, 0 },
    { "bas",           0x00ab, 0x00bb, 0x201e, 0x201c, 0 },
    { "bem",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "bez",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "bg",            0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "bm",            0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "bn",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "br",            0x00ab, 0x00bb, 0x2039, 0x203a, 0 },
    { "brx",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "bs-cyrl" ,      0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "ca",            0x201c, 0x201d, 0x00ab, 0x00bb, 0 },
    { "cgg",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "chr",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "cs",            0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "da",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "dav",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "de",            0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "de-ch",         0x00ab, 0x00bb, 0x2039, 0x203a, 0 },
    { "dje",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "dua",           0x00ab, 0x00bb, 0x2018, 0x2019, 0 },
    { "dyo",           0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "dz",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ebu",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ee",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "el",            0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "en",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "en-gb",         0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "es",            0x201c, 0x201d, 0x00ab, 0x00bb, 0 },
    { "et",            0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "eu",            0x201c, 0x201d, 0x00ab, 0x00bb, 0 },
    { "ewo",           0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "fa",            0x00ab, 0x00bb, 0x2039, 0x203a, 0 },
    { "ff",            0x201e, 0x201d, 0x201a, 0x2019, 0 },
    { "fi",            0x201d, 0x201d, 0x2019, 0x2019, 0 },
    { "fr",            0x00ab, 0x00bb, 0x00ab, 0x00bb, 0 },
    { "fr-ca",         0x00ab, 0x00bb, 0x2039, 0x203a, 0 },
    { "fr-ch",         0x00ab, 0x00bb, 0x2039, 0x203a, 0 },
    { "gsw",           0x00ab, 0x00bb, 0x2039, 0x203a, 0 },
    { "gu",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "guz",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ha",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "he",            0x0022, 0x0022, 0x0027, 0x0027, 0 },
    { "hi",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "hr",            0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "hu",            0x201e, 0x201d, 0x00bb, 0x00ab, 0 },
    { "id",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ig",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "it",            0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "ja",            0x300c, 0x300d, 0x300e, 0x300f, 0 },
    { "jgo",           0x00ab, 0x00bb, 0x2039, 0x203a, 0 },
    { "jmc",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "kab",           0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "kam",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "kde",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "kea",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "khq",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ki",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "kkj",           0x00ab, 0x00bb, 0x2039, 0x203a, 0 },
    { "kln",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "km",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "kn",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ko",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ksb",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ksf",           0x00ab, 0x00bb, 0x2018, 0x2019, 0 },
    { "lag",           0x201d, 0x201d, 0x2019, 0x2019, 0 },
    { "lg",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ln",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "lo",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "lt",            0x201e, 0x201c, 0x201e, 0x201c, 0 },
    { "lu",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "luo",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "luy",           0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "lv",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "mas",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "mer",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "mfe",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "mg",            0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "mgo",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "mk",            0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "ml",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "mr",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ms",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "mua",           0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "my",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "naq",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "nb",            0x00ab, 0x00bb, 0x2018, 0x2019, 0 },
    { "nd",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "nl",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "nmg",           0x201e, 0x201d, 0x00ab, 0x00bb, 0 },
    { "nn",            0x00ab, 0x00bb, 0x2018, 0x2019, 0 },
    { "nnh",           0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "nus",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "nyn",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "pl",            0x201e, 0x201d, 0x00ab, 0x00bb, 0 },
    { "pt",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "pt-pt",         0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "rn",            0x201d, 0x201d, 0x2019, 0x2019, 0 },
    { "ro",            0x201e, 0x201d, 0x00ab, 0x00bb, 0 },
    { "rof",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ru",            0x00ab, 0x00bb, 0x201e, 0x201c, 0 },
    { "rw",            0x00ab, 0x00bb, 0x2018, 0x2019, 0 },
    { "rwk",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "saq",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "sbp",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "seh",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ses",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "sg",            0x00ab, 0x00bb, 0x201c, 0x201d, 0 },
    { "shi",           0x00ab, 0x00bb, 0x201e, 0x201d, 0 },
    { "shi-tfng",      0x00ab, 0x00bb, 0x201e, 0x201d, 0 },
    { "si",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "sk",            0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "sl",            0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "sn",            0x201d, 0x201d, 0x2019, 0x2019, 0 },
    { "so",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "sq",            0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "sr",            0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "sr-latn",       0x201e, 0x201c, 0x201a, 0x2018, 0 },
    { "sv",            0x201d, 0x201d, 0x2019, 0x2019, 0 },
    { "sw",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "swc",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ta",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "te",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "teo",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "th",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "ti-er",         0x2018, 0x2019, 0x201c, 0x201d, 0 },
    { "to",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "tr",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "twq",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "tzm",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "uk",            0x00ab, 0x00bb, 0x201e, 0x201c, 0 },
    { "ur",            0x201d, 0x201c, 0x2019, 0x2018, 0 },
    { "vai",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "vai-latn",      0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "vi",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "vun",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "xh",            0x2018, 0x2019, 0x201c, 0x201d, 0 },
    { "xog",           0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "yav",           0x00ab, 0x00bb, 0x00ab, 0x00bb, 0 },
    { "yo",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "zh",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
    { "zh-hant",       0x300c, 0x300d, 0x300e, 0x300f, 0 },
    { "zu",            0x201c, 0x201d, 0x2018, 0x2019, 0 },
};

const QuotesData* quotesDataForLanguage(const AtomicString& lang)
{
    if (lang.isNull())
        return nullptr;

    // This could be just a hash table, but doing that adds 200k to LayoutQuote.o
    Language* languagesEnd = languages + WTF_ARRAY_LENGTH(languages);
    CString lowercaseLang = lang.lower().utf8();
    Language key = { lowercaseLang.data(), 0, 0, 0, 0, 0 };
    Language* match = std::lower_bound(languages, languagesEnd, key);
    if (match == languagesEnd || strcmp(match->lang, key.lang))
        return nullptr;

    if (!match->data)
        match->data = QuotesData::create(match->open1, match->close1, match->open2, match->close2).leakRef();

    return match->data;
}

static const QuotesData* basicQuotesData()
{
    // FIXME: The default quotes should be the fancy quotes for "en".
    DEFINE_STATIC_REF(QuotesData, staticBasicQuotes, (QuotesData::create('"', '"', '\'', '\'')));
    return staticBasicQuotes;
}

void LayoutQuote::updateText()
{
    String text = computeText();
    if (m_text == text)
        return;

    m_text = text;

    LayoutTextFragment* fragment = findFragmentChild();
    if (fragment) {
        fragment->setStyle(mutableStyle());
        fragment->setContentString(m_text.impl());
    } else {
        fragment = new LayoutTextFragment(&document(), m_text.impl());
        fragment->setStyle(mutableStyle());
        addChild(fragment);
    }
}

LayoutTextFragment* LayoutQuote::findFragmentChild() const
{
    // We walk from the end of the child list because, if we've had a first-letter
    // LayoutObject inserted then the remaining text will be at the end.
    while (LayoutObject* child = lastChild()) {
        if (child->isText() && toLayoutText(child)->isTextFragment())
            return toLayoutTextFragment(child);
    }

    return nullptr;
}

String LayoutQuote::computeText() const
{
    switch (m_type) {
    case NO_OPEN_QUOTE:
    case NO_CLOSE_QUOTE:
        return emptyString();
    case CLOSE_QUOTE:
        return quotesData()->getCloseQuote(m_depth - 1).impl();
    case OPEN_QUOTE:
        return quotesData()->getOpenQuote(m_depth).impl();
    }
    ASSERT_NOT_REACHED();
    return emptyString();
}

const QuotesData* LayoutQuote::quotesData() const
{
    if (const QuotesData* customQuotes = style()->quotes())
        return customQuotes;

    if (const QuotesData* quotes = quotesDataForLanguage(style()->locale()))
        return quotes;

    return basicQuotesData();
}

void LayoutQuote::attachQuote()
{
    ASSERT(view());
    ASSERT(!m_attached);
    ASSERT(!m_next && !m_previous);
    ASSERT(isRooted());

    if (!view()->layoutQuoteHead()) {
        view()->setLayoutQuoteHead(this);
        m_attached = true;
        return;
    }

    for (LayoutObject* predecessor = previousInPreOrder(); predecessor; predecessor = predecessor->previousInPreOrder()) {
        // Skip unattached predecessors to avoid having stale m_previous pointers
        // if the previous node is never attached and is then destroyed.
        if (!predecessor->isQuote() || !toLayoutQuote(predecessor)->isAttached())
            continue;
        m_previous = toLayoutQuote(predecessor);
        m_next = m_previous->m_next;
        m_previous->m_next = this;
        if (m_next)
            m_next->m_previous = this;
        break;
    }

    if (!m_previous) {
        m_next = view()->layoutQuoteHead();
        view()->setLayoutQuoteHead(this);
        if (m_next)
            m_next->m_previous = this;
    }
    m_attached = true;

    for (LayoutQuote* quote = this; quote; quote = quote->m_next)
        quote->updateDepth();

    ASSERT(!m_next || m_next->m_attached);
    ASSERT(!m_next || m_next->m_previous == this);
    ASSERT(!m_previous || m_previous->m_attached);
    ASSERT(!m_previous || m_previous->m_next == this);
}

void LayoutQuote::detachQuote()
{
    ASSERT(!m_next || m_next->m_attached);
    ASSERT(!m_previous || m_previous->m_attached);
    if (!m_attached)
        return;

    // Reset our attached status at this point because it's possible for
    // updateDepth() to call into attachQuote(). Attach quote walks the layout
    // tree looking for quotes that are attached and does work on them.
    m_attached = false;

    if (m_previous)
        m_previous->m_next = m_next;
    else if (view())
        view()->setLayoutQuoteHead(m_next);
    if (m_next)
        m_next->m_previous = m_previous;
    if (!documentBeingDestroyed()) {
        for (LayoutQuote* quote = m_next; quote; quote = quote->m_next)
            quote->updateDepth();
    }
    m_next = nullptr;
    m_previous = nullptr;
    m_depth = 0;
}

void LayoutQuote::updateDepth()
{
    ASSERT(m_attached);
    int oldDepth = m_depth;
    m_depth = 0;
    if (m_previous) {
        m_depth = m_previous->m_depth;
        switch (m_previous->m_type) {
        case OPEN_QUOTE:
        case NO_OPEN_QUOTE:
            m_depth++;
            break;
        case CLOSE_QUOTE:
        case NO_CLOSE_QUOTE:
            if (m_depth)
                m_depth--;
            break;
        }
    }
    if (oldDepth != m_depth)
        updateText();
}

} // namespace blink
