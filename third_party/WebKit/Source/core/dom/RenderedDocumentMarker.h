/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RenderedDocumentMarker_h
#define RenderedDocumentMarker_h

#include "core/dom/DocumentMarker.h"
#include "platform/geometry/LayoutRect.h"

namespace blink {

class RenderedDocumentMarker final : public DocumentMarker {
public:
    static PassOwnPtrWillBeRawPtr<RenderedDocumentMarker> create(const DocumentMarker& marker)
    {
        return adoptPtrWillBeNoop(new RenderedDocumentMarker(marker));
    }

    bool isRendered() const { return invalidMarkerRect() != m_renderedRect; }
    bool contains(const LayoutPoint& point) const { return isRendered() && m_renderedRect.contains(point); }
    bool setRenderedRect(const LayoutRect& r)
    {
        const LayoutRect& rect = r.isEmpty() ? invalidMarkerRect() : r;
        if (rect == m_renderedRect)
            return false;
        m_renderedRect = rect;
        return true;
    }

    const LayoutRect& renderedRect() const { return m_renderedRect; }

    bool invalidateRenderedRect() { return setRenderedRect(invalidMarkerRect()); }

private:
    explicit RenderedDocumentMarker(const DocumentMarker& marker)
        : DocumentMarker(marker)
        , m_renderedRect(invalidMarkerRect())
    {
    }

    static const LayoutRect& invalidMarkerRect()
    {
        static const LayoutRect rect = LayoutRect(-1, -1, -1, -1);
        return rect;
    }

    LayoutRect m_renderedRect;
};

DEFINE_TYPE_CASTS(RenderedDocumentMarker, DocumentMarker, marker, true, true);

} // namespace blink

WTF_ALLOW_MOVE_INIT_AND_COMPARE_WITH_MEM_FUNCTIONS(blink::RenderedDocumentMarker);

#endif
