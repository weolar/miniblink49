#include "base/compiler_specific.h"
#include "third_party/WebKit/public/platform/Platform.h"

namespace WTF {
class Mutex;
}

namespace blink {

template <typename T> class Timer;
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
class WebBlobRegistryImpl;
class WebCryptoImpl;

class BlinkPlatformImpl : NON_EXPORTED_BASE(public blink::Platform) {
public:
    BlinkPlatformImpl();
    virtual ~BlinkPlatformImpl();
    void shutdown();
    void preShutdown();

    static void initialize();

    void setGcTimer(double intervalSec);
    void setResGcTimer(double intervalSec);
   
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
    static const char* getUserAgent();
    void setUserAgent(const char* ua);

    virtual blink::WebData loadResource(const char* name) override;

    virtual blink::WebThemeEngine* themeEngine() override;

    virtual blink::WebMimeRegistry* mimeRegistry() override;

    virtual blink::WebCompositorSupport* compositorSupport() override;

    // Process -------------------------------------------------------------

    // Returns a unique identifier for a process. This may not necessarily be
    // the process's process ID.
    virtual uint32_t getUniqueIdForProcess() override;

    // Scrollbar ----------------------------------------------------------
    virtual blink::WebScrollbarBehavior* scrollbarBehavior() override;

    // Message Ports -------------------------------------------------------
    virtual void createMessageChannel(blink::WebMessagePortChannel** channel1, blink::WebMessagePortChannel** channel2) override;

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

    // WaitableEvent------------------------------------------------------ -
    virtual blink::WebWaitableEvent* createWaitableEvent(blink::WebWaitableEvent::ResetPolicy, blink::WebWaitableEvent::InitialState) override;
    virtual blink::WebWaitableEvent* waitMultipleEvents(const blink::WebVector<blink::WebWaitableEvent*>& events) override;

    // Blob ----------------------------------------------------------------

    // Must return non-null.
    virtual blink::WebBlobRegistry* blobRegistry() override;

    // clipboard -----------------------------------------------------------
    virtual blink::WebClipboard* clipboard() override;

    // Plugin --------------------------------------------------------------
    void getPluginList(bool refresh, blink::WebPluginListBuilder* builder) override;

    // fileUtilities -------------------------------------------------------
    virtual blink::WebFileUtilities* fileUtilities() override;

    // WebCrypto ----------------------------------------------------------

    virtual blink::WebCrypto* crypto() override;

    //////////////////////////////////////////////////////////////////////////
    virtual void registerMemoryDumpProvider(blink::WebMemoryDumpProvider*) override;
    virtual void unregisterMemoryDumpProvider(blink::WebMemoryDumpProvider*) override;

    //////////////////////////////////////////////////////////////////////////
    blink::WebThread* tryGetIoThread() const;
    blink::WebThread* ioThread();
    void doGarbageCollected();

    //////////////////////////////////////////////////////////////////////////
    virtual size_t numberOfProcessors() override;
    void setNumberOfProcessors(size_t num);

    //////////////////////////////////////////////////////////////////////////
    class AutoDisableGC {
    public:
        AutoDisableGC();
        ~AutoDisableGC();
    };

private:
    void destroyWebInfo();
    void closeThread();
    void resourceGarbageCollectedTimer(blink::Timer<BlinkPlatformImpl>*);
    void garbageCollectedTimer(blink::Timer<BlinkPlatformImpl>*);
    void perfTimer(blink::Timer<BlinkPlatformImpl>*);
    bool m_isDisableGC;

    CRITICAL_SECTION* m_lock;
    static const int m_maxThreadNum = 1000;
    std::vector<WebThreadImpl*> m_threads;
    int m_threadNum;

    blink::Timer<BlinkPlatformImpl>* m_gcTimer;
    blink::Timer<BlinkPlatformImpl>* m_defaultGcTimer;
    blink::Timer<BlinkPlatformImpl>* m_perfTimer;
    blink::Timer<BlinkPlatformImpl>* m_resTimer; // 资源单独一个定时器

    blink::WebThread* m_ioThread;

    ThreadIdentifier m_mainThreadId;
    blink::WebThemeEngine* m_webThemeEngine;
    WebMimeRegistryImpl* m_mimeRegistry;
    WebClipboardImpl* m_clipboardImpl;
    WebBlobRegistryImpl* m_blobRegistryImpl;
    WebFileUtilitiesImpl* m_webFileUtilitiesImpl;
    WebCryptoImpl* m_webCryptoImpl;
    cc_blink::WebCompositorSupportImpl* m_webCompositorSupport;
    blink::WebScrollbarBehavior* m_webScrollbarBehavior;
    DOMStorageMapWrap* m_localStorageStorageMap;
    DOMStorageMapWrap* m_sessionStorageStorageMap;
    int64 m_storageNamespaceIdCount;
    double m_firstMonotonicallyIncreasingTime;

    WTF::String* m_userAgent;

    size_t m_numberOfProcessors;
};

} // namespace content