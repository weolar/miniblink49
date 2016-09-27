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

#include "config.h"
#include "web/WebHelperPluginImpl.h"

#include "core/html/HTMLObjectElement.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "public/web/WebPlugin.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebPluginContainerImpl.h"

namespace blink {

DEFINE_TYPE_CASTS(WebHelperPluginImpl, WebHelperPlugin, plugin, true, true);

WebHelperPlugin* WebHelperPlugin::create(const WebString& pluginType, WebLocalFrame* frame)
{
    OwnPtr<WebHelperPlugin> plugin = adoptPtr<WebHelperPlugin>(new WebHelperPluginImpl());
    if (!toWebHelperPluginImpl(plugin.get())->initialize(pluginType, toWebLocalFrameImpl(frame)))
        return 0;
    return plugin.leakPtr();
}

WebHelperPluginImpl::WebHelperPluginImpl()
    : m_destructionTimer(this, &WebHelperPluginImpl::reallyDestroy)
{
}

WebHelperPluginImpl::~WebHelperPluginImpl()
{
#if ENABLE(OILPAN)
    // FIXME: Oilpan: it is potentially problematic to support plugin
    // disposal during an Oilpan GC. If it happens, we need to know
    // and evaluate possible ways to handle it.
    ASSERT(!ThreadState::current()->sweepForbidden());
    if (m_pluginContainer)
        m_pluginContainer->dispose();
#endif
}

bool WebHelperPluginImpl::initialize(const String& pluginType, WebLocalFrameImpl* frame)
{
    ASSERT(!m_objectElement && !m_pluginContainer);
    if (!frame->frame()->loader().client())
        return false;

    m_objectElement = HTMLObjectElement::create(*frame->frame()->document(), 0, false);
    Vector<String> attributeNames;
    Vector<String> attributeValues;
    ASSERT(frame->frame()->document()->url().isValid());
    m_pluginContainer = adoptRefWillBeNoop(toWebPluginContainerImpl(frame->frame()->loader().client()->createPlugin(
        m_objectElement.get(),
        frame->frame()->document()->url(),
        attributeNames,
        attributeValues,
        pluginType,
        false,
        FrameLoaderClient::AllowDetachedPlugin).leakRef()));

    if (!m_pluginContainer)
        return false;

    // Getting a placeholder plugin is also failure, since it's not the plugin the caller needed.
    return !getPlugin()->isPlaceholder();
}

void WebHelperPluginImpl::reallyDestroy(Timer<WebHelperPluginImpl>*)
{
    delete this;
}

void WebHelperPluginImpl::destroy()
{
    // Defer deletion so we don't do too much work when called via stopActiveDOMObjects().
    // FIXME: It's not clear why we still need this. The original code held a Page and a
    // WebFrame, and destroying it would cause JavaScript triggered by frame detach to run,
    // which isn't allowed inside stopActiveDOMObjects(). Removing this causes one Chrome test
    // to fail with a timeout.
    m_destructionTimer.startOneShot(0, FROM_HERE);
}

WebPlugin* WebHelperPluginImpl::getPlugin()
{
    ASSERT(m_pluginContainer);
    ASSERT(m_pluginContainer->plugin());
    return m_pluginContainer->plugin();
}

} // namespace blink
