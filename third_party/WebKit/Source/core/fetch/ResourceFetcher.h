/*
    Copyright (C) 1998 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2001 Dirk Mueller <mueller@kde.org>
    Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
    Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

    This class provides all functionality needed for loading images, style sheets and html
    pages from the web. It has a memory cache for these objects.
*/

#ifndef ResourceFetcher_h
#define ResourceFetcher_h

#include "core/CoreExport.h"
#include "core/fetch/CachePolicy.h"
#include "core/fetch/FetchContext.h"
#include "core/fetch/FetchInitiatorInfo.h"
#include "core/fetch/FetchRequest.h"
#include "core/fetch/Resource.h"
#include "core/fetch/ResourceLoaderOptions.h"
#include "core/fetch/ResourcePtr.h"
#include "platform/Timer.h"
#include "platform/network/ResourceError.h"
#include "platform/network/ResourceLoadPriority.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/ListHashSet.h"
#include "wtf/text/StringHash.h"

namespace blink {

class ArchiveResourceCollection;
class CSSStyleSheetResource;
class DocumentResource;
class FontResource;
class ImageResource;
class MHTMLArchive;
class RawResource;
class ScriptResource;
class SubstituteData;
class XSLStyleSheetResource;
class KURL;
class ResourceTimingInfo;
class ResourceLoaderSet;

// The ResourceFetcher provides a per-context interface to the MemoryCache
// and enforces a bunch of security checks and rules for resource revalidation.
// Its lifetime is roughly per-DocumentLoader, in that it is generally created
// in the DocumentLoader constructor and loses its ability to generate network
// requests when the DocumentLoader is destroyed. Documents also hold a pointer
// to ResourceFetcher for their lifetime (and will create one if they
// are initialized without a LocalFrame), so a Document can keep a ResourceFetcher
// alive past detach if scripts still reference the Document.
class CORE_EXPORT ResourceFetcher : public GarbageCollectedFinalized<ResourceFetcher> {
    WTF_MAKE_NONCOPYABLE(ResourceFetcher);
public:
    static ResourceFetcher* create(FetchContext* context) { return new ResourceFetcher(context); }
    virtual ~ResourceFetcher();
    DECLARE_VIRTUAL_TRACE();

    ResourcePtr<Resource> requestResource(FetchRequest&, const ResourceFactory&);

    Resource* cachedResource(const KURL&) const;

    typedef HashMap<String, ResourcePtr<Resource>> DocumentResourceMap;
    const DocumentResourceMap& allResources() const { return m_documentResources; }

    bool autoLoadImages() const { return m_autoLoadImages; }
    void setAutoLoadImages(bool);

    void setImagesEnabled(bool);

    bool shouldDeferImageLoad(const KURL&) const;

    FetchContext& context() const { return m_context ? *m_context.get() : FetchContext::nullInstance(); }
    void clearContext() { m_context.clear(); }

    void garbageCollectDocumentResources();

    int requestCount() const;

    bool isPreloaded(const KURL&) const;
    void clearPreloads();
    void preloadStarted(Resource*);
    void printPreloadStats();

    void addAllArchiveResources(MHTMLArchive*);
    ArchiveResourceCollection* archiveResourceCollection() const { return m_archiveResourceCollection.get(); }

    void setDefersLoading(bool);
    void stopFetching();
    bool isFetching() const;

    void didLoadResource();
    void redirectReceived(Resource*, const ResourceResponse&);
    void didFinishLoading(Resource*, double finishTime, int64_t encodedDataLength);
    void didChangeLoadingPriority(const Resource*, ResourceLoadPriority, int intraPriorityValue);
    void didFailLoading(const Resource*, const ResourceError&);
    void willSendRequest(unsigned long identifier, ResourceRequest&, const ResourceResponse& redirectResponse, const FetchInitiatorInfo&);
    void didReceiveResponse(const Resource*, const ResourceResponse&);
    void didReceiveData(const Resource*, const char* data, int dataLength, int encodedDataLength);
    void didDownloadData(const Resource*, int dataLength, int encodedDataLength);
    void subresourceLoaderFinishedLoadingOnePart(ResourceLoader*);
    void didInitializeResourceLoader(ResourceLoader*);
    void willStartLoadingResource(Resource*, ResourceRequest&);
    bool defersLoading() const;
    bool isLoadedBy(ResourceFetcher*) const;

    enum AccessControlLoggingDecision {
        ShouldLogAccessControlErrors,
        ShouldNotLogAccessControlErrors
    };
    bool canAccessRedirect(Resource*, ResourceRequest&, const ResourceResponse&, ResourceLoaderOptions&);
    bool canAccessResource(Resource*, SecurityOrigin*, const KURL&, AccessControlLoggingDecision) const;
    bool isControlledByServiceWorker() const;

    void acceptDataFromThreadedReceiver(unsigned long identifier, const char* data, int dataLength, int encodedDataLength);

    ResourceLoadPriority loadPriority(Resource::Type, const FetchRequest&);

    enum ResourceLoadStartType {
        ResourceLoadingFromNetwork,
        ResourceLoadingFromCache
    };
    void requestLoadStarted(Resource*, const FetchRequest&, ResourceLoadStartType);
    static const ResourceLoaderOptions& defaultResourceOptions();

    String getCacheIdentifier() const;

    void scheduleDocumentResourcesGC();
    bool clientDefersImage(const KURL&) const;
    void determineRequestContext(ResourceRequest&, Resource::Type);

private:
    friend class ResourceCacheValidationSuppressor;

    explicit ResourceFetcher(FetchContext*);

    ResourcePtr<Resource> createResourceForRevalidation(const FetchRequest&, Resource*, const ResourceFactory&);
    ResourcePtr<Resource> createResourceForLoading(FetchRequest&, const String& charset, const ResourceFactory&);
    void storeResourceTimingInitiatorInformation(Resource*);
    bool scheduleArchiveLoad(Resource*, const ResourceRequest&);

    enum RevalidationPolicy { Use, Revalidate, Reload, Load };
    RevalidationPolicy determineRevalidationPolicy(Resource::Type, const FetchRequest&, Resource* existingResource) const;

    void addAdditionalRequestHeaders(ResourceRequest&, Resource::Type);

    static bool resourceNeedsLoad(Resource*, const FetchRequest&, RevalidationPolicy);

    void notifyLoadedFromMemoryCache(Resource*);

    void garbageCollectDocumentResourcesTimerFired(Timer<ResourceFetcher>*);

    void resourceTimingReportTimerFired(Timer<ResourceFetcher>*);

    void reloadImagesIfNotDeferred();

    void willTerminateResourceLoader(ResourceLoader*);

    Member<FetchContext> m_context;

    HashSet<String> m_validatedURLs;
    mutable DocumentResourceMap m_documentResources;

    OwnPtr<ListHashSet<Resource*>> m_preloads;
    OwnPtrWillBeMember<ArchiveResourceCollection> m_archiveResourceCollection;

    Timer<ResourceFetcher> m_garbageCollectDocumentResourcesTimer;
    Timer<ResourceFetcher> m_resourceTimingReportTimer;

    typedef HashMap<Resource*, OwnPtr<ResourceTimingInfo>> ResourceTimingInfoMap;
    ResourceTimingInfoMap m_resourceTimingInfoMap;

    Vector<OwnPtr<ResourceTimingInfo>> m_scheduledResourceTimingReports;

    Member<ResourceLoaderSet> m_loaders;
    Member<ResourceLoaderSet> m_nonBlockingLoaders;

    // Used in hit rate histograms.
    class DeadResourceStatsRecorder {
    public:
        DeadResourceStatsRecorder();
        ~DeadResourceStatsRecorder();

        void update(RevalidationPolicy);

    private:
        int m_useCount;
        int m_revalidateCount;
        int m_loadCount;
    };
    DeadResourceStatsRecorder m_deadStatsRecorder;

    // 29 bits left
    bool m_autoLoadImages : 1;
    bool m_imagesEnabled : 1;
    bool m_allowStaleResources : 1;
};

class ResourceCacheValidationSuppressor {
    WTF_MAKE_NONCOPYABLE(ResourceCacheValidationSuppressor);
    STACK_ALLOCATED();
public:
    ResourceCacheValidationSuppressor(ResourceFetcher* loader)
        : m_loader(loader)
        , m_previousState(false)
    {
        if (m_loader) {
            m_previousState = m_loader->m_allowStaleResources;
            m_loader->m_allowStaleResources = true;
        }
    }
    ~ResourceCacheValidationSuppressor()
    {
        if (m_loader)
            m_loader->m_allowStaleResources = m_previousState;
    }
private:
    Member<ResourceFetcher> m_loader;
    bool m_previousState;
};

} // namespace blink

#endif // ResourceFetcher_h
