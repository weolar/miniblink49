#include "base/compiler_specific.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "net/StorageDef.h"

namespace WTF {
class Mutex;
}

namespace blink {
template <typename T> class Timer;
class WebThreadSupportingGC;
class GcTimeScheduler;
}

namespace mc_blink {
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

    static void initialize(bool ocEnable);

    void setGcTimer(double intervalSec);
    void setResGcTimer(double intervalSec);

    float getZoom() const { return m_zoom; }
    void setZoom(float f) { m_zoom = f; }
   
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

    blink::WebData parseDataURL(const  blink::WebURL&, blink::WebString& mimetype, blink::WebString& charset) override;

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
    //blink::WebStorageNamespace* createSessionStorageNamespace();
    int64 genStorageNamespaceId();
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

    // media

    virtual double audioHardwareSampleRate() override;
    virtual size_t audioHardwareBufferSize() override;
    virtual unsigned audioHardwareOutputChannels() override;

    // Creates a device for audio I/O.
    // Pass in (numberOfInputChannels > 0) if live/local audio input is desired.
    virtual blink::WebAudioDevice* createAudioDevice(size_t bufferSize, unsigned numberOfInputChannels, unsigned numberOfChannels, double sampleRate, blink::WebAudioDevice::RenderCallback*, const blink::WebString& deviceId) override;

    //////////////////////////////////////////////////////////////////////////
    virtual void registerMemoryDumpProvider(blink::WebMemoryDumpProvider*) override;
    virtual void unregisterMemoryDumpProvider(blink::WebMemoryDumpProvider*) override;

    // Platform events -----------------------------------------------------
    virtual blink::WebString domCodeStringFromEnum(int domCode) override;
    virtual blink::WebString domKeyStringFromEnum(int domKey) override;

    // GPU ----------------------------------------------------------------
    virtual blink::WebGraphicsContext3D* createOffscreenGraphicsContext3D(const blink::WebGraphicsContext3D::Attributes&, blink::WebGraphicsContext3D* shareContext) override;
    virtual blink::WebGraphicsContext3D* createOffscreenGraphicsContext3D(const blink::WebGraphicsContext3D::Attributes&, blink::WebGraphicsContext3D* shareContext, blink::WebGLInfo* glInfo) override;
    virtual blink::WebGraphicsContext3D* createOffscreenGraphicsContext3D(const blink::WebGraphicsContext3D::Attributes&) override;

    virtual blink::WebGraphicsContext3DProvider* createSharedOffscreenGraphicsContext3DProvider() override;
    virtual bool canAccelerate2dCanvas() override;
    virtual bool isThreadedCompositingEnabled() override;
    virtual blink::WebFlingAnimator* createFlingAnimator() override;
    virtual blink::WebGestureCurve* createFlingAnimationCurve(blink::WebGestureDevice deviceSource, const blink::WebFloatPoint& velocity, const blink::WebSize& cumulativeScroll) override;

    //////////////////////////////////////////////////////////////////////////
    //blink::WebThread* tryGetIoThread() const;
    WTF::Vector<blink::WebThread*> getIoThreads();
    blink::WebThread* createIoThread(const char* threadName);
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

    float m_zoom;

    CRITICAL_SECTION* m_lock;
    static const int m_maxThreadNum = 1000;
    std::vector<WebThreadImpl*> m_threads;
    int m_threadNum;

//     blink::Timer<BlinkPlatformImpl>* m_gcTimer;
//     blink::Timer<BlinkPlatformImpl>* m_defaultGcTimer;
//     blink::Timer<BlinkPlatformImpl>* m_perfTimer;
//     blink::Timer<BlinkPlatformImpl>* m_resTimer; // 资源单独一个定时器

    blink::GcTimeScheduler* m_gcTimeScheduler;

    //WTF::OwnPtr<blink::WebThreadSupportingGC> m_ioThread;
    std::vector<blink::WebThreadSupportingGC*> m_ioThreads;
    //blink::WebThreadSupportingGC* m_ioThread;

    ThreadIdentifier m_mainThreadId;
    blink::WebThemeEngine* m_webThemeEngine;
    WebMimeRegistryImpl* m_mimeRegistry;
    WebClipboardImpl* m_clipboardImpl;
    WebBlobRegistryImpl* m_blobRegistryImpl;
    WebFileUtilitiesImpl* m_webFileUtilitiesImpl;
    WebCryptoImpl* m_webCryptoImpl;
    blink::WebCompositorSupport* m_mcCompositorSupport;
    blink::WebCompositorSupport* m_ccCompositorSupport;

    blink::WebScrollbarBehavior* m_webScrollbarBehavior;
#ifdef MINIBLINK_NO_PAGE_LOCALSTORAGE
    DOMStorageMap* m_localStorageStorageMap;
#endif
//  DOMStorageMap* m_sessionStorageStorageMap;
    int64 m_storageNamespaceIdCount;
    double m_firstMonotonicallyIncreasingTime;

    std::string* m_userAgent;

    size_t m_numberOfProcessors;
};

} // namespace content