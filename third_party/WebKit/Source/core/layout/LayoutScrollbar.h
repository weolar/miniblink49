/*
 * Copyright (C) 2008, 2009 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LayoutScrollbar_h
#define LayoutScrollbar_h

#include "core/style/ComputedStyleConstants.h"
#include "platform/heap/Handle.h"
#include "platform/scroll/Scrollbar.h"
#include "wtf/HashMap.h"

namespace blink {

class LocalFrame;
class Node;
class LayoutBox;
class LayoutScrollbarPart;
class ComputedStyle;

class LayoutScrollbar final : public Scrollbar {
public:
    static PassRefPtrWillBeRawPtr<Scrollbar> createCustomScrollbar(ScrollableArea*, ScrollbarOrientation, Node*, LocalFrame* owningFrame = nullptr);
    virtual ~LayoutScrollbar();

    LayoutBox* owningLayoutObject() const;

    IntRect buttonRect(ScrollbarPart);
    IntRect trackRect(int startLength, int endLength);
    IntRect trackPieceRectWithMargins(ScrollbarPart, const IntRect&);

    int minimumThumbLength();

    virtual bool isOverlayScrollbar() const override { return false; }

    LayoutScrollbarPart* getPart(ScrollbarPart partType) { return m_parts.get(partType); }

    virtual void invalidateRect(const IntRect&) override;

    DECLARE_VIRTUAL_TRACE();

protected:
    LayoutScrollbar(ScrollableArea*, ScrollbarOrientation, Node*, LocalFrame*);

private:
    friend class Scrollbar;

    virtual void setParent(Widget*) override;
    virtual void setEnabled(bool) override;

    virtual void setHoveredPart(ScrollbarPart) override;
    virtual void setPressedPart(ScrollbarPart) override;

    virtual void styleChanged() override;

    virtual bool isCustomScrollbar() const override { return true; }

    void updateScrollbarParts(bool destroy = false);

    PassRefPtr<ComputedStyle> getScrollbarPseudoStyle(ScrollbarPart, PseudoId);
    void updateScrollbarPart(ScrollbarPart, bool destroy = false);

    // This Scrollbar(Widget) may outlive the DOM which created it (during tear down),
    // so we keep a reference to the Node which caused this custom scrollbar creation.
    // This will not create a reference cycle as the Widget tree is owned by our containing
    // FrameView which this Node pointer can in no way keep alive. See webkit bug 80610.
    RefPtrWillBeMember<Node> m_owner;

    RawPtrWillBeMember<LocalFrame> m_owningFrame;

    HashMap<unsigned, LayoutScrollbarPart*> m_parts;
};

DEFINE_TYPE_CASTS(LayoutScrollbar, ScrollbarThemeClient, scrollbar, scrollbar->isCustomScrollbar(), scrollbar.isCustomScrollbar());

} // namespace blink

#endif // LayoutScrollbar_h
