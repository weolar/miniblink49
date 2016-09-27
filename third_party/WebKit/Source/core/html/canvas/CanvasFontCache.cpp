// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/html/canvas/CanvasFontCache.h"

#include "core/css/parser/CSSParser.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/Document.h"
#include "core/style/ComputedStyle.h"
#include "platform/fonts/FontCache.h"
#include "public/platform/Platform.h"

namespace {

const unsigned CanvasFontCacheMaxFonts = 50;
const unsigned CanvasFontCacheHardMaxFonts = 250;
const unsigned CanvasFontCacheHiddenMaxFonts = 1;
const int defaultFontSize = 10;
const char defaultFontFamily[] = "sans-serif";

}

namespace blink {

CanvasFontCache::CanvasFontCache(Document& document)
    : m_document(&document)
    , m_pruningScheduled(false)
{
    FontFamily fontFamily;
    fontFamily.setFamily(defaultFontFamily);
    FontDescription defaultFontDescription;
    defaultFontDescription.setFamily(fontFamily);
    defaultFontDescription.setSpecifiedSize(defaultFontSize);
    defaultFontDescription.setComputedSize(defaultFontSize);
    m_defaultFontStyle = ComputedStyle::create();
    m_defaultFontStyle->setFontDescription(defaultFontDescription);
    m_defaultFontStyle->font().update(m_defaultFontStyle->font().fontSelector());
}

CanvasFontCache::~CanvasFontCache()
{
    m_mainCachePurgePreventer.clear();
    if (m_pruningScheduled) {
        Platform::current()->currentThread()->removeTaskObserver(this);
    }
}

unsigned CanvasFontCache::maxFonts()
{
    return CanvasFontCacheMaxFonts;
}

unsigned CanvasFontCache::hardMaxFonts()
{
    return m_document->hidden() ? CanvasFontCacheHiddenMaxFonts : CanvasFontCacheHardMaxFonts;
}

bool CanvasFontCache::getFontUsingDefaultStyle(const String& fontString, Font& resolvedFont)
{
    HashMap<String, Font>::iterator i = m_fontsResolvedUsingDefaultStyle.find(fontString);
    if (i != m_fontsResolvedUsingDefaultStyle.end()) {
        ASSERT(m_fontLRUList.contains(fontString));
        m_fontLRUList.remove(fontString);
        m_fontLRUList.add(fontString);
        resolvedFont = i->value;
        return true;
    }

    // Addition to LRU list taken care of inside parseFont
    MutableStylePropertySet* parsedStyle = parseFont(fontString);
    if (!parsedStyle)
        return false;

    RefPtr<ComputedStyle> fontStyle = ComputedStyle::clone(*m_defaultFontStyle.get());
    m_document->ensureStyleResolver().computeFont(fontStyle.get(), *parsedStyle);
    m_fontsResolvedUsingDefaultStyle.add(fontString, fontStyle->font());
    resolvedFont = m_fontsResolvedUsingDefaultStyle.find(fontString)->value;
    return true;
}

MutableStylePropertySet* CanvasFontCache::parseFont(const String& fontString)
{
    RefPtrWillBeRawPtr<MutableStylePropertySet> parsedStyle;
    MutableStylePropertyMap::iterator i = m_fetchedFonts.find(fontString);
    if (i != m_fetchedFonts.end()) {
        ASSERT(m_fontLRUList.contains(fontString));
        parsedStyle = i->value;
        m_fontLRUList.remove(fontString);
        m_fontLRUList.add(fontString);
    } else {
        parsedStyle = MutableStylePropertySet::create();
        CSSParser::parseValue(parsedStyle.get(), CSSPropertyFont, fontString, true, HTMLStandardMode, 0);
        if (parsedStyle->isEmpty())
            return nullptr;
        // According to http://lists.w3.org/Archives/Public/public-html/2009Jul/0947.html,
        // the "inherit" and "initial" values must be ignored.
        RefPtrWillBeRawPtr<CSSValue> fontValue = parsedStyle->getPropertyCSSValue(CSSPropertyFontSize);
        if (fontValue && (fontValue->isInitialValue() || fontValue->isInheritedValue()))
            return nullptr;
        m_fetchedFonts.add(fontString, parsedStyle);
        m_fontLRUList.add(fontString);
        // Hard limit is applied here, on the fly, while the soft limit is
        // applied at the end of the task.
        if (m_fetchedFonts.size() > hardMaxFonts()) {
            ASSERT(m_fetchedFonts.size() == hardMaxFonts() + 1);
            ASSERT(m_fontLRUList.size() == hardMaxFonts() + 1);
            m_fetchedFonts.remove(m_fontLRUList.first());
            m_fontsResolvedUsingDefaultStyle.remove(m_fontLRUList.first());
            m_fontLRUList.removeFirst();
        }
    }
    schedulePruningIfNeeded();

    return parsedStyle.get(); // In non-oilpan builds: ref in m_fetchedFonts keeps object alive after return.
}

void CanvasFontCache::didProcessTask()
{
    ASSERT(m_pruningScheduled);
    ASSERT(m_mainCachePurgePreventer);
    while (m_fetchedFonts.size() > maxFonts()) {
        m_fetchedFonts.remove(m_fontLRUList.first());
        m_fontsResolvedUsingDefaultStyle.remove(m_fontLRUList.first());
        m_fontLRUList.removeFirst();
    }
    m_mainCachePurgePreventer.clear();
    Platform::current()->currentThread()->removeTaskObserver(this);
    m_pruningScheduled = false;
}

void CanvasFontCache::schedulePruningIfNeeded()
{
    if (m_pruningScheduled)
        return;
    ASSERT(!m_mainCachePurgePreventer);
    m_mainCachePurgePreventer = adoptPtr(new FontCachePurgePreventer);
    Platform::current()->currentThread()->addTaskObserver(this);
    m_pruningScheduled = true;
}

bool CanvasFontCache::isInCache(const String& fontString)
{
    return m_fetchedFonts.find(fontString) != m_fetchedFonts.end();
}

void CanvasFontCache::pruneAll()
{
    m_fetchedFonts.clear();
    m_fontLRUList.clear();
}

DEFINE_TRACE(CanvasFontCache)
{
#if ENABLE(OILPAN)
    visitor->trace(m_fetchedFonts);
    visitor->trace(m_document);
#endif
}

} // blink
