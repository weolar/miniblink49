#include "base/compiler_specific.h"
#include "third_party/WebKit/public/platform/Platform.h"

namespace WTF {
class Mutex;
}

namespace cc_blink {
class WebCompositorSupportImpl;
}

namespace content {

class DOMStorageMapWrap;
class WebThreadImpl;
class WebMimeRegistryImpl;

class BlinkPlatformImpl : NON_EXPORTED_BASE(public blink::Platform) {
public:
    BlinkPlatformImpl();
    virtual ~BlinkPlatformImpl();
    void shutdown();
    void preShutdown();

    void startGarbageCollectedThread();
   
    virtual void cryptographicallyRandomValues(unsigned char* buffer, size_t length) OVERRIDE;

    // Threads -------------------------------------------------------

    // Creates an embedder-defined thread.
    virtual blink::WebThread* createThread(const char* name) OVERRIDE;
    void onThreadExit(WebThreadImpl* threadImpl);

    // Returns an interface to the current thread. This is owned by the
    // embedder.
    virtual blink::WebThread* currentThread() OVERRIDE;
    static void onCurrentThreadWhenWebThreadImplCreated(blink::WebThread* thread);    

    virtual const unsigned char* getTraceCategoryEnabledFlag(const char* categoryName) OVERRIDE;

    // System --------------------------------------------------------------
    virtual blink::WebString defaultLocale() OVERRIDE;
    virtual double currentTime() OVERRIDE;
    virtual double monotonicallyIncreasingTime() OVERRIDE;
    virtual double systemTraceTime() OVERRIDE;

    virtual blink::WebString userAgent() OVERRIDE;

    virtual blink::WebData BlinkPlatformImpl::loadResource(const char* name) OVERRIDE;

    virtual blink::WebThemeEngine* BlinkPlatformImpl::themeEngine() OVERRIDE;

    virtual blink::WebMimeRegistry* mimeRegistry() OVERRIDE;

    virtual blink::WebCompositorSupport* compositorSupport() OVERRIDE;

    // Scrollbar ----------------------------------------------------------
    virtual blink::WebScrollbarBehavior* scrollbarBehavior() OVERRIDE;

    // Network -------------------------------------------------------------
    blink::WebURLLoader* createURLLoader() OVERRIDE;
    virtual blink::WebURLError cancelledError(const blink::WebURL&) const OVERRIDE;

    // DOM Storage --------------------------------------------------
    virtual blink::WebStorageNamespace* createLocalStorageNamespace() OVERRIDE;
    blink::WebStorageNamespace* createSessionStorageNamespace();

    // Resources -----------------------------------------------------------
    virtual blink::WebString queryLocalizedString(blink::WebLocalizedString::Name) OVERRIDE;
    virtual blink::WebString queryLocalizedString(blink::WebLocalizedString::Name, const blink::WebString& parameter) OVERRIDE;
    virtual blink::WebString queryLocalizedString(blink::WebLocalizedString::Name, const blink::WebString& parameter1, const blink::WebString& parameter2) OVERRIDE;

    // Blob ----------------------------------------------------------------

    // Must return non-null.
    virtual blink::WebBlobRegistry* blobRegistry() OVERRIDE;

    //////////////////////////////////////////////////////////////////////////
    virtual void registerMemoryDumpProvider(blink::WebMemoryDumpProvider*) OVERRIDE;
    virtual void unregisterMemoryDumpProvider(blink::WebMemoryDumpProvider*) OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	blink::WebThread* tryGetIoThread() const;
	blink::WebThread* ioThread();

private:
    void destroyWebInfo();
    void closeThread();
    void doGarbageCollected();

    CRITICAL_SECTION* m_lock;
    static const int m_maxThreadNum = 20;
    WebThreadImpl* m_threads[m_maxThreadNum];
    int m_threadNum;

	blink::WebThread* m_ioThread;

    ThreadIdentifier m_mainThreadId;
    blink::WebThemeEngine* m_webThemeEngine;
    WebMimeRegistryImpl* m_mimeRegistry;
    cc_blink::WebCompositorSupportImpl* m_webCompositorSupport;
    blink::WebScrollbarBehavior* m_webScrollbarBehavior;
    DOMStorageMapWrap* m_localStorageStorageMap;
    DOMStorageMapWrap* m_sessionStorageStorageMap;
    int64 m_storageNamespaceIdCount;
    double m_firstMonotonicallyIncreasingTime;
};

} // namespace content