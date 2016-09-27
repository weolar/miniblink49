/*
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2007 Apple Inc.
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

#ifndef PrintContext_h
#define PrintContext_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

class SkCanvas;

namespace blink {

class Element;
class LocalFrame;
class FloatRect;
class FloatSize;
class IntRect;
class Node;

class CORE_EXPORT PrintContext : public NoBaseWillBeGarbageCollectedFinalized<PrintContext> {
public:
    explicit PrintContext(LocalFrame*);
    virtual ~PrintContext();

    LocalFrame* frame() const { return m_frame; }

    // Break up a page into rects without relayout.
    // FIXME: This means that CSS page breaks won't be on page boundary if the size is different than what was passed to begin(). That's probably not always desirable.
    // FIXME: Header and footer height should be applied before layout, not after.
    // FIXME: The printRect argument is only used to determine page aspect ratio, it would be better to pass a FloatSize with page dimensions instead.
    virtual void computePageRects(const FloatRect& printRect, float headerHeight, float footerHeight, float userScaleFactor, float& outPageHeight);

    // Deprecated. Page size computation is already in this class, clients shouldn't be copying it.
    // FIXME: Everyone passes |false| for the second paramer. We should remove the second parameter.
    virtual void computePageRectsWithPageSize(const FloatSize& pageSizeInPixels);

    // These are only valid after page rects are computed.
    size_t pageCount() const { return m_pageRects.size(); }
    const IntRect& pageRect(size_t pageNumber) const { return m_pageRects[pageNumber]; }
    const Vector<IntRect>& pageRects() const { return m_pageRects; }

    // Enter print mode, updating layout for new page size.
    // This function can be called multiple times to apply new print options without going back to screen mode.
    virtual void begin(float width, float height = 0);

    // Return to screen mode.
    virtual void end();

    // Used by layout tests.
    static int pageNumberForElement(Element*, const FloatSize& pageSizeInPixels); // Returns -1 if page isn't found.
    static String pageProperty(LocalFrame* frame, const char* propertyName, int pageNumber);
    static bool isPageBoxVisible(LocalFrame* frame, int pageNumber);
    static String pageSizeAndMarginsInPixels(LocalFrame* frame, int pageNumber, int width, int height, int marginTop, int marginRight, int marginBottom, int marginLeft);
    static int numberOfPages(LocalFrame*, const FloatSize& pageSizeInPixels);

    DECLARE_VIRTUAL_TRACE();

protected:
    void outputLinkedDestinations(SkCanvas*, const IntRect& pageRect);

    RawPtrWillBeMember<LocalFrame> m_frame;
    Vector<IntRect> m_pageRects;

private:
    void computePageRectsWithPageSizeInternal(const FloatSize& pageSizeInPixels);
    void collectLinkedDestinations(Node*);

    // Used to prevent misuses of begin() and end() (e.g., call end without begin).
    bool m_isPrinting;

    WillBeHeapHashMap<String, RawPtrWillBeMember<Element>> m_linkedDestinations;
    bool m_linkedDestinationsValid;
};

}

#endif
