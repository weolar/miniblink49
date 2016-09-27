/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#ifndef WebScrollbarThemeClientImpl_h
#define WebScrollbarThemeClientImpl_h

#include "platform/PlatformExport.h"
#include "platform/scroll/ScrollbarThemeClient.h"
#include "public/platform/WebScrollbar.h"
#include "wtf/Noncopyable.h"

namespace blink {

// Adapts a WebScrollbar to the ScrollbarThemeClient interface
class PLATFORM_EXPORT WebScrollbarThemeClientImpl : public ScrollbarThemeClient {
    WTF_MAKE_NONCOPYABLE(WebScrollbarThemeClientImpl);
public:
    // Caller must retain ownership of this pointer and ensure that its lifetime
    // exceeds this instance.
    WebScrollbarThemeClientImpl(WebScrollbar*);
    ~WebScrollbarThemeClientImpl() override;

    // Implement ScrollbarThemeClient interface
    int x() const override;
    int y() const override;
    int width() const override;
    int height() const override;
    IntSize size() const override;
    IntPoint location() const override;
    Widget* parent() const override;
    Widget* root() const override;
    void setFrameRect(const IntRect&) override;
    IntRect frameRect() const override;
    void invalidate() override;
    void invalidateRect(const IntRect&) override;
    ScrollbarOverlayStyle scrollbarOverlayStyle() const override;
    void getTickmarks(Vector<IntRect>&) const override;
    bool isScrollableAreaActive() const override;
    IntPoint convertFromContainingWindow(const IntPoint&) override;
    bool isCustomScrollbar() const override;
    ScrollbarOrientation orientation() const override;
    bool isLeftSideVerticalScrollbar() const override;
    int value() const override;
    float currentPos() const override;
    int visibleSize() const override;
    int totalSize() const override;
    int maximum() const override;
    ScrollbarControlSize controlSize() const override;
    ScrollbarPart pressedPart() const override;
    ScrollbarPart hoveredPart() const override;
    void styleChanged() override;
    bool enabled() const override;
    void setEnabled(bool) override;
    bool isOverlayScrollbar() const override;
    bool isAlphaLocked() const override;
    void setIsAlphaLocked(bool) override;
    float elasticOverscroll() const override;
    void setElasticOverscroll(float) override;
    DisplayItemClient displayItemClient() const override;
    String debugName() const override;

private:
    WebScrollbar* m_scrollbar;
};

} // namespace blink

#endif // WebScrollbarThemeClientImpl_h
