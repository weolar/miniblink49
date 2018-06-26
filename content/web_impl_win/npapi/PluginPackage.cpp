/*
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Collabora Ltd.  All rights reserved.
 * Copyright (C) 2009 Holger Hans Peter Freyther
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
#include "content/web_impl_win/npapi/PluginPackage.h"

#include "content/web_impl_win/npapi/PluginDatabase.h"
#include "content/web_impl_win/npapi/WebPluginImpl.h"
#include "third_party/WebKit/Source/bindings/core/v8/npruntime_impl.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"

#define PURE = 0

#include <Shlwapi.h>
#include <string.h>

namespace content {

PluginPackage::~PluginPackage()
{
    // This destructor gets called during refresh() if PluginDatabase's
    // PluginSet hash is already populated, as it removes items from
    // the hash table. Calling the destructor on a loaded plug-in of
    // course would cause a crash, so we check to call unload before we
    // ASSERT.
    // FIXME: There is probably a better way to fix this.
    if (!m_loadCount)
        unloadWithoutShutdown();
    else
        unload();

    ASSERT(!m_isLoaded);
}

void PluginPackage::freeLibrarySoon()
{
    ASSERT(!m_freeLibraryTimer.isActive());
    if (!m_isVirtual)
        ASSERT(m_module);
    ASSERT(!m_loadCount);

    m_freeLibraryTimer.startOneShot(0, FROM_HERE);
}

void PluginPackage::freeLibraryTimerFired(blink::Timer<PluginPackage>*)
{
    if (!m_isVirtual)
        ASSERT(m_module);
    // Do nothing if the module got loaded again meanwhile
    if (!m_loadCount) {
        //::FreeLibrary(m_module); // by weolar: 不卸载模块，因为_NPN_IsAlive里的liveObjectMap还有NPObject，那个有模块里的虚函数表
        m_module = 0;
    }
}

int PluginPackage::compare(const PluginPackage& compareTo) const
{
    // Sort plug-ins that allow multiple instances first.
    bool AallowsMultipleInstances = !quirks().contains(PluginQuirkDontAllowMultipleInstances);
    bool BallowsMultipleInstances = !compareTo.quirks().contains(PluginQuirkDontAllowMultipleInstances);
    if (AallowsMultipleInstances != BallowsMultipleInstances)
        return AallowsMultipleInstances ? -1 : 1;

    // Sort plug-ins in a preferred path first.
    bool AisInPreferredDirectory = PluginDatabase::isPreferredPluginDirectory(parentDirectory());
    bool BisInPreferredDirectory = PluginDatabase::isPreferredPluginDirectory(compareTo.parentDirectory());
    if (AisInPreferredDirectory != BisInPreferredDirectory)
        return AisInPreferredDirectory ? -1 : 1;

    int diff = strcmp(name().utf8().data(), compareTo.name().utf8().data());
    if (diff)
        return diff;

    diff = compareFileVersion(compareTo.version());
    if (diff)
        return diff;

    return strcmp(parentDirectory().utf8().data(), compareTo.parentDirectory().utf8().data());
}

PluginPackage::PluginPackage(const String& path, const time_t& lastModified)
    : m_isEnabled(true)
    , m_isLoaded(false)
    , m_loadCount(0)
    , m_path(path)
    , m_moduleVersion(0)
    , m_module(0)
    , m_lastModified(lastModified)
    , m_freeLibraryTimer(this, &PluginPackage::freeLibraryTimerFired)
#if ENABLE(NETSCAPE_PLUGIN_METADATA_CACHE)
    , m_infoIsFromCache(true)
#endif
{
    m_isVirtual = false;
    m_NP_GetEntryPoints = nullptr;
    m_NP_Initialize = nullptr;

    m_fileName = String(::PathFindFileName(WTF::ensureUTF16UChar(m_path, true).data()));
    m_parentDirectory = m_path.left(m_path.length() - m_fileName.length() - 1);
}

void PluginPackage::unload()
{
    if (!m_isLoaded)
        return;

    if (--m_loadCount > 0)
        return;

    m_NPP_Shutdown();

    unloadWithoutShutdown();
}

void PluginPackage::unloadWithoutShutdown()
{
    if (!m_isLoaded)
        return;

    ASSERT(!m_loadCount);

    if (!m_isVirtual)
        ASSERT(m_module);

    // <rdar://5530519>: Crash when closing tab with pdf file (Reader 7 only)
    // If the plugin has subclassed its parent window, as with Reader 7, we may have
    // gotten here by way of the plugin's internal window proc forwarding a message to our
    // original window proc. If we free the plugin library from here, we will jump back
    // to code we just freed when we return, so delay calling FreeLibrary at least until
    // the next message loop
    freeLibrarySoon();

    m_isLoaded = false;
}

void PluginPackage::setEnabled(bool enabled)
{
    m_isEnabled = enabled;
}

PassRefPtr<PluginPackage> PluginPackage::createPackage(const String& path, const time_t& lastModified)
{
    RefPtr<PluginPackage> package = adoptRef(new PluginPackage(path, lastModified));

    if (!package->fetchInfo())
        return nullptr;

    return package.release();
}

PassRefPtr<PluginPackage> PluginPackage::createVirtualPackage(
    NP_InitializeFuncPtr NP_Initialize,
    NP_GetEntryPointsFuncPtr NP_GetEntryPoints,
    NPP_ShutdownProcPtr NPP_Shutdown,
    const time_t& lastModified,
    const String& name,
    const String& description,
    const String& mimeDescription
    )
{
    RefPtr<PluginPackage> package = adoptRef(new PluginPackage("", lastModified));
    package->m_isVirtual = true;
    package->m_NP_GetEntryPoints = NP_GetEntryPoints;
    package->m_NP_Initialize = NP_Initialize;
    package->m_NPP_Shutdown = NPP_Shutdown;
    package->m_name = name;
    package->m_description = description;

    Vector<String> extensionsVector;
    extensionsVector.append(description);

    package->m_mimeToExtensions.add(mimeDescription, extensionsVector);
    package->m_mimeToDescriptions.add(mimeDescription, description);
    return package.release();
}

#if ENABLE(NETSCAPE_PLUGIN_METADATA_CACHE)
PassRefPtr<PluginPackage> PluginPackage::createPackageFromCache(const String& path, const time_t& lastModified, const String& name, const String& description, const String& mimeDescription)
{
    RefPtr<PluginPackage> package = adoptRef(new PluginPackage(path, lastModified));
    package->m_name = name;
    package->m_description = description;
    package->determineModuleVersionFromDescription();
    package->setMIMEDescription(mimeDescription);
    package->m_infoIsFromCache = true;
    return package.release();
}
#endif

void PluginPackage::initializeBrowserFuncs()
{
    memset(&m_browserFuncs, 0, sizeof(m_browserFuncs));
    m_browserFuncs.size = sizeof(m_browserFuncs);
    m_browserFuncs.version = NPVersion();

    m_browserFuncs.geturl = NPN_GetURL;
    m_browserFuncs.posturl = NPN_PostURL;
    m_browserFuncs.requestread = NPN_RequestRead;
    m_browserFuncs.newstream = NPN_NewStream;
    m_browserFuncs.write = NPN_Write;
    m_browserFuncs.destroystream = NPN_DestroyStream;
    m_browserFuncs.status = NPN_Status;
    m_browserFuncs.uagent = NPN_UserAgent;
    m_browserFuncs.memalloc = NPN_MemAlloc;
    m_browserFuncs.memfree = NPN_MemFree;
    m_browserFuncs.memflush = NPN_MemFlush;
    m_browserFuncs.reloadplugins = NPN_ReloadPlugins;
    m_browserFuncs.geturlnotify = NPN_GetURLNotify;
    m_browserFuncs.posturlnotify = NPN_PostURLNotify;
    m_browserFuncs.getvalue = NPN_GetValue;
    m_browserFuncs.setvalue = NPN_SetValue;
    m_browserFuncs.invalidaterect = NPN_InvalidateRect;
    m_browserFuncs.invalidateregion = NPN_InvalidateRegion;
    m_browserFuncs.forceredraw = NPN_ForceRedraw;
    m_browserFuncs.getJavaEnv = NPN_GetJavaEnv;
    m_browserFuncs.getJavaPeer = NPN_GetJavaPeer;
    m_browserFuncs.pushpopupsenabledstate = NPN_PushPopupsEnabledState;
    m_browserFuncs.poppopupsenabledstate = NPN_PopPopupsEnabledState;
    m_browserFuncs.pluginthreadasynccall = NPN_PluginThreadAsyncCall;

    m_browserFuncs.releasevariantvalue = _NPN_ReleaseVariantValue;
    m_browserFuncs.getstringidentifier = _NPN_GetStringIdentifier;
    m_browserFuncs.getstringidentifiers = _NPN_GetStringIdentifiers;
    m_browserFuncs.getintidentifier = _NPN_GetIntIdentifier;
    m_browserFuncs.identifierisstring = _NPN_IdentifierIsString;
    m_browserFuncs.utf8fromidentifier = _NPN_UTF8FromIdentifier;
    m_browserFuncs.intfromidentifier = _NPN_IntFromIdentifier;
    m_browserFuncs.createobject = _NPN_CreateObject;
    m_browserFuncs.retainobject = _NPN_RetainObject;
    m_browserFuncs.releaseobject = _NPN_ReleaseObject;
    m_browserFuncs.invoke = _NPN_Invoke;
    m_browserFuncs.invokeDefault = _NPN_InvokeDefault;
    m_browserFuncs.evaluate = _NPN_Evaluate;
    m_browserFuncs.getproperty = _NPN_GetProperty;
    m_browserFuncs.setproperty = _NPN_SetProperty;
    m_browserFuncs.removeproperty = _NPN_RemoveProperty;
    m_browserFuncs.hasproperty = _NPN_HasProperty;
    m_browserFuncs.hasmethod = _NPN_HasMethod;
    m_browserFuncs.setexception = _NPN_SetException;
    m_browserFuncs.enumerate = _NPN_Enumerate;
    m_browserFuncs.construct = _NPN_Construct;
    m_browserFuncs.getvalueforurl = NPN_GetValueForURL;
    m_browserFuncs.setvalueforurl = NPN_SetValueForURL;
    m_browserFuncs.getauthenticationinfo = NPN_GetAuthenticationInfo;

    m_browserFuncs.popupcontextmenu = NPN_PopUpContextMenu;
}

int PluginPackage::compareFileVersion(const PlatformModuleVersion& compareVersion) const
{
    // return -1, 0, or 1 if plug-in version is less than, equal to, or greater than
    // the passed version

    if (m_moduleVersion.mostSig != compareVersion.mostSig)
        return m_moduleVersion.mostSig > compareVersion.mostSig ? 1 : -1;
    if (m_moduleVersion.leastSig != compareVersion.leastSig)
        return m_moduleVersion.leastSig > compareVersion.leastSig ? 1 : -1;

    return 0;
}

#if ENABLE(NETSCAPE_PLUGIN_METADATA_CACHE)
bool PluginPackage::ensurePluginLoaded()
{
    if (!m_infoIsFromCache)
        return m_isLoaded;

    m_quirks = PluginQuirkSet();
    m_name = String();
    m_description = String();
    m_fullMIMEDescription = String();
    m_moduleVersion = 0;

    return fetchInfo();
}
#endif

}
