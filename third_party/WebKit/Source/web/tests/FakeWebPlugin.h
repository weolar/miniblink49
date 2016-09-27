/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
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

#ifndef FakeWebPlugin_h
#define FakeWebPlugin_h

#include "public/web/WebPlugin.h"

namespace blink {

class WebDragData;
class WebFrame;
class WebInputEvent;
class WebPluginContainer;
class WebURL;
class WebURLResponse;
struct WebPluginParams;

class FakeWebPlugin : public WebPlugin {
public:
    FakeWebPlugin(WebFrame*, const WebPluginParams&);

    // WebPlugin methods:
    bool initialize(WebPluginContainer*) override;
    void destroy() override;
    NPObject* scriptableObject() override { return 0; }
    bool canProcessDrag() const override { return false; }
    void layoutIfNeeded() override { }
    void paint(WebCanvas*, const WebRect&) override { }
    void updateGeometry(const WebRect& clientRect, const WebRect& clipRect, const WebRect& windowClipRect, const WebVector<WebRect>& cutOutsRects, bool isVisible) override { }
    void updateFocus(bool, WebFocusType) override { }
    void updateVisibility(bool) override { }
    bool acceptsInputEvents() override { return true; }
    bool handleInputEvent(const WebInputEvent&, WebCursorInfo&) override { return false; }
    bool handleDragStatusUpdate(WebDragStatus, const WebDragData&, WebDragOperationsMask, const WebPoint& position, const WebPoint& screenPosition) override { return false; }
    void didReceiveResponse(const WebURLResponse&) override { }
    void didReceiveData(const char* data, int dataLength) override { }
    void didFinishLoading() override { }
    void didFailLoading(const WebURLError&) override { }
    void didFinishLoadingFrameRequest(const WebURL&, void* notifyData) override { }
    void didFailLoadingFrameRequest(const WebURL&, void* notifyData, const WebURLError&) override { }
    bool isPlaceholder() override { return false; }

protected:
    virtual ~FakeWebPlugin();

    WebPluginContainer* container() const { return m_container; }

private:
    WebFrame* m_frame;
    WebPluginContainer* m_container;
};

} // namespace blink

#endif // FakeWebPlugin_h
