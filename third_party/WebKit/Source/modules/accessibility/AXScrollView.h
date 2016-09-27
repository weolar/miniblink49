/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AXScrollView_h
#define AXScrollView_h

#include "modules/accessibility/AXObject.h"

namespace blink {

class AXObjectCacheImpl;
class AXScrollbar;
class Scrollbar;
class FrameView;

class AXScrollView final : public AXObject {
public:
    static PassRefPtrWillBeRawPtr<AXScrollView> create(FrameView*, AXObjectCacheImpl&);
    AccessibilityRole roleValue() const override { return ScrollAreaRole; }
    FrameView* scrollView() const { return m_scrollView; }

    ~AXScrollView() override;
    DECLARE_VIRTUAL_TRACE();
    void detach() override;

protected:
    ScrollableArea* getScrollableAreaIfScrollable() const override;

private:
    AXScrollView(FrameView*, AXObjectCacheImpl&);

    bool computeAccessibilityIsIgnored(IgnoredReasons* = nullptr) const override;
    bool isAXScrollView() const override { return true; }
    bool isEnabled() const override { return true; }

    bool isAttachment() const override;
    Widget* widgetForAttachmentView() const override;

    AXObject* scrollBar(AccessibilityOrientation) override;
    void addChildren() override;
    void clearChildren() override;
    AXObject* accessibilityHitTest(const IntPoint&) const override;
    void updateChildrenIfNecessary() override;
    void setNeedsToUpdateChildren() override { m_childrenDirty = true; }
    void updateScrollbars();

    FrameView* documentFrameView() const override;
    LayoutRect elementRect() const override;
    AXObject* computeParent() const override;
    AXObject* computeParentIfExists() const override;

    AXObject* webAreaObject() const;
    AXObject* firstChild() const override { return webAreaObject(); }
    AXScrollbar* addChildScrollbar(Scrollbar*);
    void removeChildScrollbar(AXObject*);

    RawPtrWillBeMember<FrameView> m_scrollView;
    RefPtrWillBeMember<AXObject> m_horizontalScrollbar;
    RefPtrWillBeMember<AXObject> m_verticalScrollbar;
    bool m_childrenDirty;
};

DEFINE_AX_OBJECT_TYPE_CASTS(AXScrollView, isAXScrollView());

} // namespace blink

#endif // AXScrollView_h
