/*
 * Copyright (C) 2006, 2010 Apple Inc. All rights reserved.
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

#ifndef FontFallbackList_h
#define FontFallbackList_h

#include "platform/fonts/FontSelector.h"
#include "platform/fonts/SimpleFontData.h"
#include "platform/fonts/shaping/CachingWordShaper.h"
#include "wtf/Forward.h"
#include "wtf/MainThread.h"

namespace blink {

class GlyphPageTreeNodeBase;
class FontDescription;

const int cAllFamiliesScanned = -1;

class PLATFORM_EXPORT FontFallbackList : public RefCounted<FontFallbackList> {
    WTF_MAKE_NONCOPYABLE(FontFallbackList);
public:
    typedef HashMap<int, GlyphPageTreeNodeBase*, DefaultHash<int>::Hash> GlyphPages;

    class GlyphPagesStateSaver {
    public:
        GlyphPagesStateSaver(FontFallbackList& fallbackList)
            : m_fallbackList(fallbackList)
            , m_pages(fallbackList.m_pages)
            , m_pageZero(fallbackList.m_pageZero)
        {
        }

        ~GlyphPagesStateSaver()
        {
            m_fallbackList.m_pages = m_pages;
            m_fallbackList.m_pageZero = m_pageZero;
        }

    private:
        FontFallbackList& m_fallbackList;
        GlyphPages& m_pages;
        GlyphPageTreeNodeBase* m_pageZero;
    };

    static PassRefPtr<FontFallbackList> create() { return adoptRef(new FontFallbackList()); }

    ~FontFallbackList();
    bool isValid() const;
    void invalidate(PassRefPtrWillBeRawPtr<FontSelector>);

    bool loadingCustomFonts() const;
    bool shouldSkipDrawing() const;

    FontSelector* fontSelector() const { return m_fontSelector.get(); }
    // FIXME: It should be possible to combine fontSelectorVersion and generation.
    unsigned fontSelectorVersion() const { return m_fontSelectorVersion; }
    unsigned generation() const { return m_generation; }

    CachingWordShaper& cachingWordShaper() const { return m_cachingWordShaper; }

    const SimpleFontData* primarySimpleFontData(const FontDescription& fontDescription)
    {
        ASSERT(isMainThread());
        if (!m_cachedPrimarySimpleFontData)
            m_cachedPrimarySimpleFontData = determinePrimarySimpleFontData(fontDescription);
        return m_cachedPrimarySimpleFontData;
    }
    const FontData* fontDataAt(const FontDescription&, unsigned index) const;

    GlyphPageTreeNodeBase* getPageNode(unsigned pageNumber) const
    {
        return pageNumber ? m_pages.get(pageNumber) : m_pageZero;
    }

    void setPageNode(unsigned pageNumber, GlyphPageTreeNodeBase* node)
    {
        if (pageNumber)
            m_pages.set(pageNumber, node);
        else
            m_pageZero = node;
    }

private:
    FontFallbackList();

    PassRefPtr<FontData> getFontData(const FontDescription&, int& familyIndex) const;

    const SimpleFontData* determinePrimarySimpleFontData(const FontDescription&) const;

    void releaseFontData();

    mutable Vector<RefPtr<FontData>, 1> m_fontList;
    GlyphPages m_pages;
    GlyphPageTreeNodeBase* m_pageZero;
    mutable const SimpleFontData* m_cachedPrimarySimpleFontData;
    RefPtrWillBePersistent<FontSelector> m_fontSelector;
    mutable CachingWordShaper m_cachingWordShaper;
    unsigned m_fontSelectorVersion;
    mutable int m_familyIndex;
    unsigned short m_generation;
    mutable bool m_hasLoadingFallback : 1;
};

} // namespace blink

#endif
