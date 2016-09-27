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

#ifndef WebHelperPlugin_h
#define WebHelperPlugin_h

#include "../platform/WebCommon.h"
#include "WebFrame.h"

namespace blink {

class WebPlugin;
class WebString;

class WebHelperPlugin {
public:
    // May return null if initialization fails. If the returned pointer is
    // non-null, the caller must free it by calling destroy().
    BLINK_EXPORT static WebHelperPlugin* create(const WebString& PluginType, WebLocalFrame*);

    // Returns a WebPlugin corresponding to the instantiated plugin. This will
    // never return null.
    virtual WebPlugin* getPlugin() = 0;

    // Initiates destruction of the WebHelperPlugin.
    virtual void destroy() = 0;

protected:
    virtual ~WebHelperPlugin() { }
};

} // namespace blink

namespace WTF {

template<typename T> struct OwnedPtrDeleter;
template<> struct OwnedPtrDeleter<blink::WebHelperPlugin> {
    static void deletePtr(blink::WebHelperPlugin* plugin)
    {
        if (plugin)
            plugin->destroy();
    }
};

} // namespace WTF

#endif
