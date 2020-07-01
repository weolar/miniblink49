/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
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

#include "config.h"

#include "content/web_impl_win/npapi/PluginMainThreadScheduler.h"
#include "content/web_impl_win/npapi/WebPluginImpl.h"
#include "third_party/npapi/bindings/npruntime.h"
#include "third_party/WebKit/Source/core/page/Page.h"

using namespace blink;
using namespace content;

#if ENABLE_WKE
NPNetscapeFuncs s_wkeBrowserFuncs = { 0 };
#endif

// The plugin view is always the ndata of the instance,. Sometimes, plug-ins will call an instance-specific function
// with a NULL instance. To workaround this, call the last plug-in view that made a call to a plug-in.
// Currently, the current plug-in view is only set before NPP_New in PluginView::start.
// This specifically works around Flash and Shockwave. When we call NPP_New, they call NPN_Useragent with a NULL instance.
static WebPluginImpl* pluginViewForInstance(NPP instance)
{
    if (instance) {
        if (WebPluginImpl::isAlive(instance)) {
            if (instance->ndata) {
                return static_cast<WebPluginImpl*>(instance->ndata);
            } else
                return nullptr;
        } else
            return nullptr;
    }
    return WebPluginImpl::currentPluginView();
}

void* NPN_MemAlloc(uint32_t size)
{
    return malloc(size);
}

void NPN_MemFree(void* ptr)
{
    free(ptr);
}

uint32_t NPN_MemFlush(uint32_t)
{
    // Do nothing
    return 0;
}

void NPN_ReloadPlugins(NPBool reloadPages)
{
    Page::refreshPlugins();
}

NPError NPN_RequestRead(NPStream*, NPByteRange*)
{
    return NPERR_STREAM_NOT_SEEKABLE;
}

NPError NPN_GetURLNotify(NPP instance, const char* url, const char* target, void* notifyData)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.geturlnotify)
            return s_wkeBrowserFuncs.geturlnotify(instance, url, target, notifyData);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->getURLNotify(url, target, notifyData);
}

NPError NPN_GetURL(NPP instance, const char* url, const char* target)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.geturl)
            return s_wkeBrowserFuncs.geturl(instance, url, target);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->getURL(url, target);
}

NPError NPN_PostURLNotify(NPP instance, const char* url, const char* target, uint32_t len, const char* buf, NPBool file, void* notifyData)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.posturlnotify)
            return s_wkeBrowserFuncs.posturlnotify(instance, url, target, len, buf, file, notifyData);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->postURLNotify(url, target, len, buf, file, notifyData);
}

NPError NPN_PostURL(NPP instance, const char* url, const char* target, uint32_t len, const char* buf, NPBool file)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.posturl)
            return s_wkeBrowserFuncs.posturl(instance, url, target, len, buf, file);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->postURL(url, target, len, buf, file);
}

NPError NPN_NewStream(NPP instance, NPMIMEType type, const char* target, NPStream** stream)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.newstream)
            return s_wkeBrowserFuncs.newstream(instance, type, target, stream);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->newStream(type, target, stream);
}

int32_t NPN_Write(NPP instance, NPStream* stream, int32_t len, void* buffer)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.write)
            return s_wkeBrowserFuncs.write(instance, stream, len, buffer);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->write(stream, len, buffer);
}

NPError NPN_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.destroystream)
            return s_wkeBrowserFuncs.destroystream(instance, stream, reason);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->destroyStream(stream, reason);
}

const char* NPN_UserAgent(NPP instance)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        const char* ua = nullptr;
        if (s_wkeBrowserFuncs.uagent)
            ua = s_wkeBrowserFuncs.uagent(instance);
        if (!ua)
            return WebPluginImpl::userAgentStatic();
    }        
 
    return view->userAgent();
}

void NPN_Status(NPP instance, const char* message)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.status)
            return s_wkeBrowserFuncs.status(instance, message);
        return;
    }
    view->status(message);
}

void NPN_InvalidateRect(NPP instance, NPRect* invalidRect)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.invalidaterect)
            s_wkeBrowserFuncs.invalidaterect(instance, invalidRect);
        return;
    }
    view->invalidateRect(invalidRect);
}

void NPN_InvalidateRegion(NPP instance, NPRegion invalidRegion)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.invalidateregion)
            s_wkeBrowserFuncs.invalidateregion(instance, invalidRegion);
        return;
    }
    view->invalidateRegion(invalidRegion);
}

void NPN_ForceRedraw(NPP instance)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.forceredraw)
            s_wkeBrowserFuncs.forceredraw(instance);
        return;
    }
    view->forceRedraw();
}

NPError NPN_GetValue(NPP instance, NPNVariable variable, void* value)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.getvalue)
            return s_wkeBrowserFuncs.getvalue(instance, variable, value);
        return WebPluginImpl::getValueStatic(variable, value);
    }        

    return view->getValue(variable, value);
}

NPError NPN_SetValue(NPP instance, NPPVariable variable, void* value)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.setvalue)
            return s_wkeBrowserFuncs.setvalue(instance, variable, value);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->setValue(variable, value);
}

void* NPN_GetJavaEnv()
{
    // Unsupported
    if (s_wkeBrowserFuncs.getJavaEnv)
        return s_wkeBrowserFuncs.getJavaEnv();
    return 0;
}

void* NPN_GetJavaPeer(NPP instance)
{
    // Unsupported
    if (s_wkeBrowserFuncs.getJavaPeer)
        return s_wkeBrowserFuncs.getJavaPeer(instance);
    return 0;
}

void NPN_PushPopupsEnabledState(NPP instance, NPBool enabled)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.pushpopupsenabledstate)
            s_wkeBrowserFuncs.pushpopupsenabledstate(instance, enabled);
        return;
    }
    view->pushPopupsEnabledState(enabled);
}

void NPN_PopPopupsEnabledState(NPP instance)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.poppopupsenabledstate)
            s_wkeBrowserFuncs.poppopupsenabledstate(instance);
        return;
    }
    view->popPopupsEnabledState();
}

extern "C" typedef void PluginThreadAsyncCallFunction(void*);
void NPN_PluginThreadAsyncCall(NPP instance, PluginThreadAsyncCallFunction func, void* userData)
{
    // Callback function type only differs from MainThreadFunction by being extern "C", which doesn't affect calling convention on any compilers we use.
    PluginMainThreadScheduler::scheduler().scheduleCall(instance, reinterpret_cast<PluginMainThreadScheduler::MainThreadFunction*>(func), userData);
}

NPError NPN_GetValueForURL(NPP instance, NPNURLVariable variable, const char* url, char** value, uint32_t* len)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.getvalueforurl)
            return s_wkeBrowserFuncs.getvalueforurl(instance, variable, url, value, len);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->getValueForURL(variable, url, value, len);
}

NPError NPN_SetValueForURL(NPP instance, NPNURLVariable variable, const char* url, const char* value, uint32_t len)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.setvalueforurl)
            return s_wkeBrowserFuncs.setvalueforurl(instance, variable, url, value, len);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->setValueForURL(variable, url, value, len);
}

NPError NPN_GetAuthenticationInfo(NPP instance, const char* protocol, const char* host, int32_t port, const char* scheme, const char* realm, char** username, uint32_t* ulen, char** password, uint32_t* plen)
{
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.getauthenticationinfo)
            return s_wkeBrowserFuncs.getauthenticationinfo(instance, protocol, host, port, scheme, realm, username, ulen, password, plen);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return view->getAuthenticationInfo(protocol, host, port, scheme, realm, username, ulen, password, plen);
}

NPError NPN_PopUpContextMenu(NPP instance, NPMenu* menu)
{
//     UNUSED_PARAM(instance);
//     UNUSED_PARAM(menu);
    WebPluginImpl* view = pluginViewForInstance(instance);
    if (!view) {
        if (s_wkeBrowserFuncs.popupcontextmenu)
            return s_wkeBrowserFuncs.popupcontextmenu(instance, menu);
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    return NPERR_NO_ERROR;
}
