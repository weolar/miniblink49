/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
 * Copyright (C) 2014 Opera Software ASA. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PluginView_h
#define PluginView_h

#include "platform/Widget.h"
#include "platform/scroll/ScrollTypes.h"
#include "wtf/text/WTFString.h"
#include <v8.h>

namespace blink { class WebLayer; }

namespace blink {

class ResourceError;
class ResourceResponse;

class PluginView : public Widget {
public:
    virtual bool isPluginView() const override final { return true; }

    virtual WebLayer* platformLayer() const { return 0; }
    virtual v8::Local<v8::Object> scriptableObject(v8::Isolate*) { return v8::Local<v8::Object>(); }
    virtual bool getFormValue(String&) { return false; }
    virtual bool wantsWheelEvents() { return false; }
    virtual bool supportsKeyboardFocus() const { return false; }
    virtual bool supportsInputMethod() const { return false; }
    virtual bool canProcessDrag() const { return false; }

    virtual void didReceiveResponse(const ResourceResponse&) { }
    virtual void didReceiveData(const char*, int) { }
    virtual void didFinishLoading() { }
    virtual void didFailLoading(const ResourceError&) { }

    virtual void layoutIfNeeded() { }
    virtual void invalidatePaintIfNeeded() { }

#if ENABLE(OILPAN)
    virtual LocalFrame* pluginFrame() const { return nullptr; }
    virtual void shouldDisposePlugin() { }
#endif

protected:
    PluginView() : Widget() { }
};

DEFINE_TYPE_CASTS(PluginView, Widget, widget, widget->isPluginView(), widget.isPluginView());

} // namespace blink

#endif // PluginView_h
