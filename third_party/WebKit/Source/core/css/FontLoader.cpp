// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/FontLoader.h"

#include "core/css/CSSFontSelector.h"
#include "core/dom/Document.h"
#include "core/dom/IncrementLoadEventDelayCount.h"
#include "core/fetch/FontResource.h"
#include "core/inspector/ConsoleMessage.h"

namespace blink {

struct FontLoader::FontToLoad {
public:
    static PassOwnPtr<FontToLoad> create(FontResource* fontResource, Document& document)
    {
        return adoptPtr(new FontToLoad(fontResource, document));
    }

    ResourcePtr<FontResource> fontResource;
    OwnPtr<IncrementLoadEventDelayCount> delay;

private:
    FontToLoad(FontResource* resource, Document& document)
        : fontResource(resource)
        , delay(IncrementLoadEventDelayCount::create(document, this))
    {
    }
};

FontLoader::FontLoader(CSSFontSelector* fontSelector, Document* document)
    : m_beginLoadingTimer(this, &FontLoader::beginLoadTimerFired)
    , m_fontSelector(fontSelector)
    , m_document(document)
{
}

FontLoader::~FontLoader()
{
#if ENABLE(OILPAN)
    if (!m_document) {
        ASSERT(m_fontsToBeginLoading.isEmpty());
        return;
    }
    m_beginLoadingTimer.stop();
    // This will decrement the request counts on the ResourceFetcher for all the
    // fonts that were pending at the time the FontLoader dies.
    clearPendingFonts();
#endif
}

void FontLoader::addFontToBeginLoading(FontResource* fontResource)
{
    if (!m_document || !fontResource->stillNeedsLoad() || fontResource->loadScheduled())
        return;

    m_fontsToBeginLoading.append(FontToLoad::create(fontResource, *m_document));
    fontResource->didScheduleLoad();
    if (!m_beginLoadingTimer.isActive())
        m_beginLoadingTimer.startOneShot(0, FROM_HERE);
}

void FontLoader::beginLoadTimerFired(Timer<blink::FontLoader>*)
{
    loadPendingFonts();
}

void FontLoader::loadPendingFonts()
{
    ASSERT(m_document);

    FontsToLoadVector fontsToBeginLoading;
    fontsToBeginLoading.swap(m_fontsToBeginLoading);
    for (const auto& fontToLoad : fontsToBeginLoading) {
        if (m_document->frame())
            fontToLoad->fontResource->beginLoadIfNeeded(m_document->fetcher());
        else
            fontToLoad->fontResource->error(Resource::LoadError);
    }

    // When the local fontsToBeginLoading vector goes out of scope it will
    // decrement the request counts on the ResourceFetcher for all the fonts
    // that were just loaded.
}

void FontLoader::fontFaceInvalidated()
{
    if (m_fontSelector)
        m_fontSelector->fontFaceInvalidated();
}

void FontLoader::didFailToDecode(FontResource* fontResource)
{
    // FIXME: Provide more useful message such as OTS rejection reason.
    // See crbug.com/97467
    if (m_fontSelector && m_fontSelector->document())
        m_fontSelector->document()->addConsoleMessage(ConsoleMessage::create(OtherMessageSource, WarningMessageLevel, "Failed to decode downloaded font: " + fontResource->url().elidedString()));
        if (fontResource->otsParsingMessage().length() > 1)
            m_fontSelector->document()->addConsoleMessage(ConsoleMessage::create(OtherMessageSource, WarningMessageLevel, "OTS parsing error: " + fontResource->otsParsingMessage()));
}

#if !ENABLE(OILPAN)
void FontLoader::clearDocumentAndFontSelector()
{
    if (!m_document) {
        ASSERT(m_fontsToBeginLoading.isEmpty());
        return;
    }

    m_beginLoadingTimer.stop();
    clearPendingFonts();
    m_document = nullptr;
    m_fontSelector = nullptr;
}
#endif

void FontLoader::clearPendingFonts()
{
    for (const auto& fontToLoad : m_fontsToBeginLoading)
        fontToLoad->fontResource->didUnscheduleLoad();
    m_fontsToBeginLoading.clear();
}

DEFINE_TRACE(FontLoader)
{
    visitor->trace(m_document);
    visitor->trace(m_fontSelector);
}

} // namespace blink
