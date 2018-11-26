/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Collabora, Ltd.  All rights reserved.
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

#ifndef PluginPackage_h
#define PluginPackage_h

#include "content/web_impl_win/npapi/PluginQuirkSet.h"

#include "third_party/WebKit/Source/platform/Timer.h"
#include "third_party/npapi/bindings/npfunctions.h"
#include "third_party/WebKit/Source/wtf/HashMap.h"
#include "third_party/WebKit/Source/wtf/RefCounted.h"
#include "third_party/WebKit/Source/wtf/text/StringHash.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"

namespace content {

typedef HashMap<String, String> MIMEToDescriptionsMap;
typedef HashMap<String, Vector<String> > MIMEToExtensionsMap;
typedef HMODULE PlatformModule;

struct PlatformModuleVersion {
    unsigned leastSig;
    unsigned mostSig;

    PlatformModuleVersion(unsigned)
        : leastSig(0)
        , mostSig(0)
    {
    }

    PlatformModuleVersion(unsigned lsb, unsigned msb)
        : leastSig(lsb)
        , mostSig(msb)
    {
    }

};

class PluginPackage : public RefCounted<PluginPackage> {
public:
    ~PluginPackage();
    static PassRefPtr<PluginPackage> createPackage(const String& path, const time_t& lastModified);
#if ENABLE(NETSCAPE_PLUGIN_METADATA_CACHE)
    static PassRefPtr<PluginPackage> createPackageFromCache(const String& path, const time_t& lastModified, const String& name, const String& description, const String& mimeDescription);
#endif
    
    static PassRefPtr<PluginPackage> createVirtualPackage(
        NP_InitializeFuncPtr NP_Initialize,
        NP_GetEntryPointsFuncPtr NP_GetEntryPoints,
        NPP_ShutdownProcPtr NPP_Shutdown
        );

    const String& name() const { return m_name; }
    const String& description() const { return m_description; }
    const String& path() const { return m_path; }
    const String& fileName() const { return m_fileName; }
    const String& parentDirectory() const { return m_parentDirectory; }
    PlatformModule module() const { return m_module; }
    uint16_t NPVersion() const;
    time_t lastModified() const { return m_lastModified; }

    const MIMEToDescriptionsMap& mimeToDescriptions() const { return m_mimeToDescriptions; }
    const MIMEToExtensionsMap& mimeToExtensions() const { return m_mimeToExtensions; }

    unsigned hash() const;
    static bool equal(const PluginPackage& a, const PluginPackage& b);

    bool load();
    void unload();
    void unloadWithoutShutdown();

    bool isEnabled() const { return m_isEnabled; }
    void setEnabled(bool);

    bool isVirtual() const { return m_isVirtual; }

    const NPPluginFuncs* pluginFuncs() const { return &m_pluginFuncs; }
    const NPNetscapeFuncs* browserFuncs() const { return &m_browserFuncs; }

    int compareFileVersion(const PlatformModuleVersion&) const;
    int compare(const PluginPackage&) const;
    PluginQuirkSet quirks() const { return m_quirks; }
    const PlatformModuleVersion& version() const { return m_moduleVersion; }

#if ENABLE(NETSCAPE_PLUGIN_METADATA_CACHE)
    bool ensurePluginLoaded();
    void setMIMEDescription(const String& mimeDescription);
    String fullMIMEDescription() const { return m_fullMIMEDescription;}
#endif

private:
    PluginPackage(const String& path, const time_t& lastModified);

    bool doLoad();

    bool fetchInfo();
    bool isPluginBlacklisted();
    void determineQuirks(const String& mimeType);

    void determineModuleVersionFromDescription();
    void initializeBrowserFuncs();

    bool m_isEnabled;
    bool m_isLoaded;
    int m_loadCount;

    String m_description;
    String m_path;
    String m_fileName;
    String m_name;
    String m_parentDirectory;

    PlatformModuleVersion m_moduleVersion;

    MIMEToDescriptionsMap m_mimeToDescriptions;
    MIMEToExtensionsMap m_mimeToExtensions;

    PlatformModule m_module;
    time_t m_lastModified;
    
    bool m_isVirtual;
    NP_GetEntryPointsFuncPtr m_NP_GetEntryPoints;
    NP_InitializeFuncPtr m_NP_Initialize;

    NPP_ShutdownProcPtr m_NPP_Shutdown;
    NPPluginFuncs m_pluginFuncs;
    NPNetscapeFuncs m_browserFuncs;

    void freeLibrarySoon();
    void freeLibraryTimerFired(blink::Timer<PluginPackage>*);
    blink::Timer<PluginPackage> m_freeLibraryTimer;

    PluginQuirkSet m_quirks;
#if ENABLE(NETSCAPE_PLUGIN_METADATA_CACHE)
    String m_fullMIMEDescription;
    bool m_infoIsFromCache;
#endif
};

// FIXME: This is a workaround because PluginPackageHash is broken and may consider keys with different hashes as equal.
struct PluginPackageHashTraits : HashTraits<RefPtr<PluginPackage> > {
    static const int minimumTableSize = 64;
};

struct PluginPackageHash {
    static unsigned hash(const WTF::RefPtrValuePeeker<PluginPackage>& key) { return (*key).hash();}
    static bool equal(const WTF::RefPtrValuePeeker<PluginPackage>& a, const WTF::RefPtrValuePeeker<PluginPackage>& b) { return PluginPackage::equal(*a, *b); }

    static const bool safeToCompareToEmptyOrDeleted = false;
};

} // namespace content

namespace WTF {

template<> struct HashTraits<content::PluginPackage> : SimpleClassHashTraits<content::PluginPackage> { };

template<> struct DefaultHash<content::PluginPackage> {
    typedef content::PluginPackageHash Hash;
};

} // namespace WTF

#endif
