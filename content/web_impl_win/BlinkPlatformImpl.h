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
class WebClipboardImpl;
class WebFileUtilitiesImpl;

class BlinkPlatformImpl : NON_EXPORTED_BASE(public blink::Platform) {
public:
    BlinkPlatformImpl();
    virtual ~BlinkPlatformImpl();
    void shutdown();
    void preShutdown();

    static void initialize();

    void startGarbageCollectedThread();
   
    virtual void cryptographicallyRandomValues(unsigned char* buffer, size_t length) override;

    // Threads -------------------------------------------------------

    // Creates an embedder-defined thread.
    virtual blink::WebThread* createThread(const char* name) override;
    void onThreadExit(WebThreadImpl* threadImpl);

    // Returns an interface to the current thread. This is owned by the
    // embedder.
    virtual blink::WebThread* currentThread() override;
    static void onCurrentThreadWhenWebThreadImplCreated(blink::WebThread* thread);    

    virtual const unsigned char* getTraceCategoryEnabledFlag(const char* categoryName) override;

    // System --------------------------------------------------------------
    virtual blink::WebString defaultLocale() override;
    virtual double currentTime() override;
    virtual double monotonicallyIncreasingTime() override;
    virtual double systemTraceTime() override;

    virtual blink::WebString userAgent() override;
	void setUserAgent(char* ua);

    virtual blink::WebData BlinkPlatformImpl::loadResource(const char* name) override;

    virtual blink::WebThemeEngine* BlinkPlatformImpl::themeEngine() override;

    virtual blink::WebMimeRegistry* mimeRegistry() override;

    virtual blink::WebCompositorSupport* compositorSupport() override;

    // Scrollbar ----------------------------------------------------------
    virtual blink::WebScrollbarBehavior* scrollbarBehavior() override;

    // Network -------------------------------------------------------------
    blink::WebURLLoader* createURLLoader() override;
    virtual blink::WebURLError cancelledError(const blink::WebURL&) const override;

    // DOM Storage --------------------------------------------------
    virtual blink::WebStorageNamespace* createLocalStorageNamespace() override;
    blink::WebStorageNamespace* createSessionStorageNamespace();
    virtual bool portAllowed(const blink::WebURL&) const override;

    // Resources -----------------------------------------------------------
    virtual blink::WebString queryLocalizedString(blink::WebLocalizedString::Name) override;
    virtual blink::WebString queryLocalizedString(blink::WebLocalizedString::Name, const blink::WebString& parameter) override;
    virtual blink::WebString queryLocalizedString(blink::WebLocalizedString::Name, const blink::WebString& parameter1, const blink::WebString& parameter2) override;

    // Blob ----------------------------------------------------------------

    // Must return non-null.
    virtual blink::WebBlobRegistry* blobRegistry() override;

    // clipboard -----------------------------------------------------------
    virtual blink::WebClipboard* clipboard() override;

    // Plugin --------------------------------------------------------------
    void BlinkPlatformImpl::getPluginList(bool refresh, blink::WebPluginListBuilder* builder) override;

    // fileUtilities -------------------------------------------------------
    virtual blink::WebFileUtilities* fileUtilities() override;

    //////////////////////////////////////////////////////////////////////////
    virtual void registerMemoryDumpProvider(blink::WebMemoryDumpProvider*) override;
    virtual void unregisterMemoryDumpProvider(blink::WebMemoryDumpProvider*) override;

	//////////////////////////////////////////////////////////////////////////
	blink::WebThread* tryGetIoThread() const;
	blink::WebThread* ioThread();

private:
    void destroyWebInfo();
    void closeThread();
    void doGarbageCollected();

    CRITICAL_SECTION* m_lock;
    static const int m_maxThreadNum = 1000;
    std::vector<WebThreadImpl*> m_threads;
    int m_threadNum;

	blink::WebThread* m_ioThread;

    ThreadIdentifier m_mainThreadId;
    blink::WebThemeEngine* m_webThemeEngine;
    WebMimeRegistryImpl* m_mimeRegistry;
    WebClipboardImpl* m_clipboardImpl;
    WebFileUtilitiesImpl* m_webFileUtilitiesImpl;
    cc_blink::WebCompositorSupportImpl* m_webCompositorSupport;
    blink::WebScrollbarBehavior* m_webScrollbarBehavior;
    DOMStorageMapWrap* m_localStorageStorageMap;
    DOMStorageMapWrap* m_sessionStorageStorageMap;
    int64 m_storageNamespaceIdCount;
    double m_firstMonotonicallyIncreasingTime;

    WTF::String* m_userAgent;
};

} // namespace content