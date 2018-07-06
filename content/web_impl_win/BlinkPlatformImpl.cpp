
#include "config.h"
#include "base/rand_util.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/web_impl_win/WebThreadImpl.h"
#include "content/web_impl_win/WebURLLoaderImpl.h"
#include "content/web_impl_win/WebURLLoaderImplCurl.h"
#include "content/web_impl_win/CurrentTimeImpl.h"
#include "content/web_impl_win/WebThemeEngineImpl.h"
#include "content/web_impl_win/WebMimeRegistryImpl.h"
#include "content/web_impl_win/WebBlobRegistryImpl.h"
#include "content/web_impl_win/WebClipboardImpl.h"
#include "content/web_impl_win/WebFileUtilitiesImpl.h"
#include "content/web_impl_win/WebCryptoImpl.h"
#include "content/web_impl_win/WaitableEvent.h"
#include "content/web_impl_win/npapi/WebPluginImpl.h"
#include "content/web_impl_win/npapi/PluginDatabase.h"
#include "content/web_impl_win/WebMessagePortChannelImpl.h"
#include "content/resources/MissingImageData.h"
#include "content/resources/TextAreaResizeCornerData.h"
#include "content/resources/LocalizedString.h"
#include "content/resources/WebKitWebRes.h"

#include "content/browser/WebPage.h"
#include "content/browser/PlatformMessagePortChannel.h"
#include "cc/blink/WebCompositorSupportImpl.h"
#include "cc/raster/RasterTask.h"
#include "third_party/WebKit/public/web/WebKit.h"
#include "third_party/WebKit/Source/core/fetch/MemoryCache.h"
#include "third_party/WebKit/Source/web/WebStorageNamespaceImpl.h"
#include "third_party/WebKit/public/platform/WebScrollbarBehavior.h"
#include "third_party/WebKit/public/platform/WebPluginListBuilder.h"
#include "third_party/WebKit/Source/platform/plugins/PluginData.h"
#include "third_party/WebKit/Source/platform/PartitionAllocMemoryDumpProvider.h"
#include "third_party/WebKit/Source/platform/heap/BlinkGCMemoryDumpProvider.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8GCController.h"
#include "third_party/skia/include/core/SkGraphics.h"
#include "net/ActivatingLoaderCheck.h"
#include "gen/blink/core/UserAgentStyleSheets.h"
#include "gen/blink/platform/RuntimeEnabledFeatures.h"
#include "third_party/WebKit/Source/core/loader/ImageLoader.h" // TODO
#include "third_party/WebKit/Source/core/html/HTMLLinkElement.h" // TODO
#include "third_party/WebKit/Source/core/html/HTMLStyleElement.h" // TODO
#include "third_party/WebKit/Source/core/css/resolver/StyleResolver.h"
#include "third_party/skia/include/core/SkGraphics.h"
#include "ui/gfx/win/dpi.h"
#include "gin/public/isolate_holder.h"
#include "gin/array_buffer.h"
#include "net/WebURLLoaderManager.h"
#include "wke/wkeJsBindFreeTempObject.h"

DWORD g_paintToMemoryCanvasInUiThreadCount = 0;
DWORD g_rasterTaskCount = 0;
DWORD g_mouseCount = 0;
DWORD g_paintCount = 0;
DWORD g_scheduledActionCount = 0;
double g_autoRecordActionsTime = 0;

#ifdef _DEBUG

#include "base/process/InjectTool.h"

extern size_t g_v8MemSize;
extern size_t g_blinkMemSize;
extern size_t g_skiaMemSize;

class CallAddrsRecord;
CallAddrsRecord* g_callAddrsRecord = nullptr;

extern std::set<void*>* g_activatingStyleFetchedImage;

typedef void* (__cdecl * MyFree)(void*);
MyFree myFree = nullptr;

void* __cdecl newFree(void* p)
{
    if (p && 0x1122dd44 == *(size_t*)p) {
        DebugBreak();
    }
    return myFree(p);
}

typedef void *  (__cdecl* MyRealloc)(void*, size_t);
MyRealloc myRealloc = nullptr;

void* __cdecl newRealloc(void* p, size_t s)
{
    if (p && 0x1122dd44 == *(size_t*)p) {
        DebugBreak();
    }
    return myRealloc(p, s);
}

#endif

namespace blink {
#ifdef _DEBUG
extern std::set<void*>* g_activatingImageLoader;
extern std::set<void*>* g_activatingFontFallbackList;
extern int gStyleFetchedImageCreate;
extern int gStyleFetchedImageNotifyFinished;
#endif
}

#if USING_VC6RT == 1
void scrt_initialize_thread_safe_statics();
#endif
extern "C" void x86_check_features(void);

namespace content {

class DOMStorageMapWrap {
public:
    DOMStorageMapWrap()
    {
    }
    ~DOMStorageMapWrap()
    {
    }
    blink::DOMStorageMap map;
};

DWORD sCurrentThreadTlsKey = -1;

static WebThreadImpl* currentTlsThread()
{
    //AtomicallyInitializedStaticReference(WTF::ThreadSpecific<WebThreadImpl>, sCurrentThread, new ThreadSpecific<WebThreadImpl>);
    //return sCurrentThread;
    if (-1 != sCurrentThreadTlsKey)
        return (WebThreadImpl*)TlsGetValue(sCurrentThreadTlsKey);

    return nullptr;
}

static void setRuntimeEnabledFeatures()
{
    blink::RuntimeEnabledFeatures::setSlimmingPaintEnabled(false);
    blink::RuntimeEnabledFeatures::setXSLTEnabled(false);
    blink::RuntimeEnabledFeatures::setExperimentalStreamEnabled(false);
    blink::RuntimeEnabledFeatures::setFrameTimingSupportEnabled(false);
    blink::RuntimeEnabledFeatures::setSharedWorkerEnabled(false);
    blink::RuntimeEnabledFeatures::setOverlayScrollbarsEnabled(false);
    blink::RuntimeEnabledFeatures::setTouchEnabled(false);
    blink::RuntimeEnabledFeatures::setMemoryCacheEnabled(true);
    blink::RuntimeEnabledFeatures::setCspCheckEnabled(true);
    blink::RuntimeEnabledFeatures::setNpapiPluginsEnabled(true);
}

void BlinkPlatformImpl::initialize()
{
#if USING_VC6RT == 1
    scrt_initialize_thread_safe_statics();
#endif
    x86_check_features();
    
    ::CoInitializeEx(nullptr, 0); // COINIT_MULTITHREADED
    ::OleInitialize(nullptr);

    setRuntimeEnabledFeatures();

    gfx::win::InitDeviceScaleFactor();
    BlinkPlatformImpl* platform = new BlinkPlatformImpl();
    blink::Platform::initialize(platform);
    gin::IsolateHolder::Initialize(gin::IsolateHolder::kNonStrictMode, gin::ArrayBufferAllocator::SharedInstance());
    blink::initialize(blink::Platform::current());
    //initializeOffScreenTimerWindow();

    // Maximum allocation size allowed for image scaling filters that
    // require pre-scaling. Skia will fallback to a filter that doesn't
    // require pre-scaling if the default filter would require an
    // allocation that exceeds this limit.
    const size_t kImageCacheSingleAllocationByteLimit = 64 * 1024 * 1024;
    SkGraphics::SetResourceCacheSingleAllocationByteLimit(kImageCacheSingleAllocationByteLimit);

    platform->m_defaultGcTimer = new blink::Timer<BlinkPlatformImpl>(platform, &BlinkPlatformImpl::garbageCollectedTimer);
    platform->m_defaultGcTimer->start(40, 40, FROM_HERE);

    platform->m_resTimer = new blink::Timer<BlinkPlatformImpl>(platform, &BlinkPlatformImpl::resourceGarbageCollectedTimer);
    platform->m_resTimer->start(120, 120, FROM_HERE);
    
//     platform->m_perfTimer = new blink::Timer<BlinkPlatformImpl>(platform, &BlinkPlatformImpl::perfTimer);
//     platform->m_perfTimer->start(2, 2, FROM_HERE);

    //OutputDebugStringW(L"BlinkPlatformImpl::initBlink\n");
}

BlinkPlatformImpl::BlinkPlatformImpl() 
{
    m_mainThreadId = -1;
    m_webThemeEngine = nullptr;
    m_mimeRegistry = nullptr;
    m_clipboardImpl = nullptr;
    m_webCompositorSupport = nullptr;
    m_webScrollbarBehavior = nullptr;
    m_localStorageStorageMap = nullptr;
    m_sessionStorageStorageMap = nullptr;
    m_webFileUtilitiesImpl = nullptr;
    m_blobRegistryImpl = nullptr;
    m_webCryptoImpl = nullptr;
    m_userAgent = nullptr;
    m_storageNamespaceIdCount = 1;
    m_lock = new CRITICAL_SECTION();
    m_threadNum = 0;
    m_gcTimer = nullptr;
    m_perfTimer = nullptr;
    m_ioThread = nullptr;
    m_firstMonotonicallyIncreasingTime = currentTimeImpl(); // (GetTickCount() / 1000.0);
    m_numberOfProcessors = 1;
    m_isDisableGC = false;

    ::InitializeCriticalSection(m_lock);

    setUserAgent(getUserAgent());

#ifdef _DEBUG
    myFree = (MyFree)ReplaceFuncAndCopy(free, newFree);
    myRealloc = (MyRealloc)ReplaceFuncAndCopy(realloc, newRealloc);
#endif
}

BlinkPlatformImpl::~BlinkPlatformImpl()
{
    if (m_gcTimer)
        delete m_gcTimer;
    delete m_defaultGcTimer;
    delete m_resTimer;

    ::DeleteCriticalSection(m_lock);
    delete m_lock;
    m_lock = nullptr;
}

void BlinkPlatformImpl::destroyWebInfo()
{
    if (m_webThemeEngine)
        delete m_webThemeEngine;
    m_webThemeEngine = nullptr;

    if (m_mimeRegistry)
        delete m_mimeRegistry;
    m_mimeRegistry = nullptr;

    if (m_webCompositorSupport)
        delete m_webCompositorSupport;
    m_webCompositorSupport = nullptr;

    if (m_webScrollbarBehavior)
        delete m_webScrollbarBehavior;
    m_webScrollbarBehavior = nullptr;

    if (m_localStorageStorageMap)
        delete m_localStorageStorageMap;
    m_localStorageStorageMap = nullptr;

    if (m_sessionStorageStorageMap)
        delete m_sessionStorageStorageMap;
    m_sessionStorageStorageMap = nullptr;

    if (m_clipboardImpl)
        delete m_clipboardImpl;
    m_clipboardImpl = nullptr;

    if (m_userAgent)
        delete m_userAgent;
    m_userAgent = nullptr;
}

void BlinkPlatformImpl::registerMemoryDumpProvider(blink::WebMemoryDumpProvider*) {}
void BlinkPlatformImpl::unregisterMemoryDumpProvider(blink::WebMemoryDumpProvider* provider)
{
    if (provider == blink::BlinkGCMemoryDumpProvider::instance()) {
        
    }

    if (provider == blink::PartitionAllocMemoryDumpProvider::instance()) {
        
    }
}

void BlinkPlatformImpl::preShutdown()
{
    WebPluginImpl::shutdown();
    destroyWebInfo();

    if (m_ioThread)
        delete m_ioThread;
    m_ioThread = nullptr;

    WebThread* mainThread = m_mainThread;
    delete mainThread;
}

void BlinkPlatformImpl::shutdown()
{
    wke::freeV8TempObejctOnOneFrameBefore();
    ((WebThreadImpl*)currentThread())->fire();

    net::WebURLLoaderManager::sharedInstance()->shutdown();
    WebPage::shutdown();

    ((WebThreadImpl*)currentThread())->fire();

    blink::memoryCache()->evictResources();
    v8::Isolate::GetCurrent()->LowMemoryNotification();

    cc::RasterTaskWorkerThreadPool* rasterPool = cc::RasterTaskWorkerThreadPool::shared();
    rasterPool->shutdown();

    SkGraphics::PurgeResourceCache();
    SkGraphics::PurgeFontCache();
    SkGraphics::Term();

    net::ActivatingLoaderCheck::inst()->shutdown();

    MemoryCache* memoryCache = MemoryCache::create();
    replaceMemoryCacheForTesting(memoryCache);

    ((WebThreadImpl*)currentThread())->fire();

    blink::Heap::collectAllGarbage();

    blink::ImageLoader::dispatchPendingLoadEvents();
    blink::ImageLoader::dispatchPendingErrorEvents();
    blink::HTMLLinkElement::dispatchPendingLoadEvents();
    blink::HTMLStyleElement::dispatchPendingLoadEvents();

    ((WebThreadImpl*)m_mainThread)->shutdown();

    if (blink::StyleResolver::styleNotYetAvailable())
        blink::StyleResolver::styleNotYetAvailable()->font().update(nullptr);

    blink::Heap::collectAllGarbage();

#ifdef _DEBUG
//     for (blink::ThreadState* state : blink::ThreadState::attachedThreads())
//         state->snapshot();
#endif

#ifdef _DEBUG
    if (blink::g_activatingImageLoader && !blink::g_activatingImageLoader->empty() ||
        // blink::g_activatingFontFallbackList && !blink::g_activatingFontFallbackList->empty() ||
        g_activatingStyleFetchedImage && !g_activatingStyleFetchedImage->empty() 
        // || g_activatingIncrementLoadEventDelayCount && !g_activatingIncrementLoadEventDelayCount->empty()
        )
        DebugBreak();
#endif

    preShutdown();
    blink::Heap::collectAllGarbage();

    blink::shutdown();
    closeThread();

    net::ActivatingLoaderCheck::inst()->destroy();

#ifdef _DEBUG
    size_t v8MemSize = g_v8MemSize;
    v8MemSize += g_blinkMemSize;
    v8MemSize += g_skiaMemSize;
    g_callAddrsRecord;
#endif
    delete this;
}

void BlinkPlatformImpl::perfTimer(blink::Timer<BlinkPlatformImpl>*)
{
    String output = String::format("perfTimer: paintToMemory:%d raster:%d, scheduled:%d, AutoRecordActions:%f\n",
        g_paintToMemoryCanvasInUiThreadCount, g_rasterTaskCount, g_scheduledActionCount, (float)g_autoRecordActionsTime);
    OutputDebugStringA(output.utf8().data());

    g_paintToMemoryCanvasInUiThreadCount = 0;
    g_rasterTaskCount = 0;
    g_mouseCount = 0;
    g_paintCount = 0;
    g_scheduledActionCount = 0;
    g_autoRecordActionsTime = 0;
}

BlinkPlatformImpl::AutoDisableGC::AutoDisableGC()
{
    BlinkPlatformImpl* platform = (BlinkPlatformImpl*)blink::Platform::current();
    platform->m_isDisableGC = true;
}

BlinkPlatformImpl::AutoDisableGC::~AutoDisableGC()
{
    BlinkPlatformImpl* platform = (BlinkPlatformImpl*)blink::Platform::current();
    platform->m_isDisableGC = false;
}

void BlinkPlatformImpl::resourceGarbageCollectedTimer(blink::Timer<BlinkPlatformImpl>*)
{
    blink::memoryCache()->evictResources();
}

void BlinkPlatformImpl::garbageCollectedTimer(blink::Timer<BlinkPlatformImpl>*)
{
    doGarbageCollected();
}

void BlinkPlatformImpl::doGarbageCollected()
{
    if (m_isDisableGC)
        return;
    
    v8::Isolate::GetCurrent()->LowMemoryNotification();
    SkGraphics::PurgeResourceCache();
    SkGraphics::PurgeFontCache();

    WebPage::gcAll();

#ifdef _DEBUG
//     String out = String::format("BlinkPlatformImpl::doGarbageCollected: %d %d %d\n", g_v8MemSize, g_blinkMemSize, g_skiaMemSize);
//     OutputDebugStringA(out.utf8().data());
#endif
}

void BlinkPlatformImpl::setGcTimer(double intervalSec)
{
    if (!m_gcTimer)
        m_gcTimer = new blink::Timer<BlinkPlatformImpl>(this, &BlinkPlatformImpl::garbageCollectedTimer);

    if (!m_gcTimer->isActive() || intervalSec < m_gcTimer->nextFireInterval())
        m_gcTimer->startOneShot(intervalSec, FROM_HERE);

    if (m_defaultGcTimer)
        delete m_defaultGcTimer;
    m_defaultGcTimer = nullptr;
}

void BlinkPlatformImpl::setResGcTimer(double intervalSec)
{
    m_resTimer->stop();
    m_resTimer->startOneShot(intervalSec, FROM_HERE);
}

void BlinkPlatformImpl::closeThread()
{   
    if (0 != m_threadNum)
        DebugBreak();

    ::EnterCriticalSection(m_lock);
    for (size_t i = 0; i < m_threads.size(); ++i) {
        if (nullptr != m_threads[i])
            DebugBreak();
    }
    ::LeaveCriticalSection(m_lock);
}

blink::WebThread* BlinkPlatformImpl::createThread(const char* name)
{
    if (0 != strcmp(name, "MainThread")) {
        RELEASE_ASSERT(-1 != sCurrentThreadTlsKey);
    }
    WebThreadImpl* threadImpl = new WebThreadImpl(name);

    ::EnterCriticalSection(m_lock);
    m_threads.push_back(threadImpl);
    ++m_threadNum;
    if (m_threadNum > m_maxThreadNum)
        DebugBreak();
    ::LeaveCriticalSection(m_lock);

    return threadImpl;
}

void BlinkPlatformImpl::onThreadExit(WebThreadImpl* threadImpl)
{
    ::EnterCriticalSection(m_lock);
    bool find = false;
    for (std::vector<WebThreadImpl*>::iterator i = m_threads.begin(); i != m_threads.end(); ++i) {
        if (*i != threadImpl)
            continue;
        m_threads.erase(i);
        --m_threadNum;
        find = true;
        break;
    }
    if (!find || m_threadNum < 0)
        DebugBreak();
    ::LeaveCriticalSection(m_lock);
}

void BlinkPlatformImpl::onCurrentThreadWhenWebThreadImplCreated(blink::WebThread* thread)
{
    RELEASE_ASSERT(-1 != sCurrentThreadTlsKey);
    TlsSetValue(sCurrentThreadTlsKey, thread);
}

blink::WebThread* BlinkPlatformImpl::currentThread()
{
    if (-1 == m_mainThreadId) {
        m_mainThreadId = WTF::currentThread();
        m_mainThread = createThread("MainThread");
        sCurrentThreadTlsKey = TlsAlloc();
        onCurrentThreadWhenWebThreadImplCreated(m_mainThread);
        return m_mainThread;
    }
    
    if (WTF::isMainThread())
        return m_mainThread;

    return currentTlsThread();
}

blink::WebThread* BlinkPlatformImpl::tryGetIoThread() const
{
    return m_ioThread;
}

blink::WebThread* BlinkPlatformImpl::ioThread()
{
    if (!m_ioThread)
        m_ioThread = createThread("ioThread");
    return m_ioThread;
}

void BlinkPlatformImpl::cryptographicallyRandomValues(unsigned char* buffer, size_t length)
{
    base::RandBytes(buffer, length);
}

blink::WebURLLoader* BlinkPlatformImpl::createURLLoader()
{
   //return new content::WebURLLoaderImpl();
   return new content::WebURLLoaderImplCurl();
}

const unsigned char* BlinkPlatformImpl::getTraceCategoryEnabledFlag(const char* categoryName)
{
    static const char* dummyCategoryEnabledFlag = "*";
    return reinterpret_cast<const unsigned char*>(dummyCategoryEnabledFlag);
}

blink::WebString BlinkPlatformImpl::defaultLocale()
{
    return blink::WebString::fromUTF8("zh-CN");
}

double BlinkPlatformImpl::currentTime()
{
    return currentTimeImpl();
}

double BlinkPlatformImpl::monotonicallyIncreasingTime() 
{
//     LARGE_INTEGER qpc;
//     QueryPerformanceCounter(&qpc);
//     return qpc.QuadPart / (1000 * 1000);

//     double timeInDouble = (double)GetTickCount();  
//     return (timeInDouble / 1000.0) - m_firstMonotonicallyIncreasingTime;

    double timeInDouble = (double)currentTimeImpl();
    return timeInDouble - m_firstMonotonicallyIncreasingTime;
}

double BlinkPlatformImpl::systemTraceTime() 
{
    DebugBreak();
    return 0; 
}

blink::WebString BlinkPlatformImpl::userAgent()
{
    return *m_userAgent;
}

const char* BlinkPlatformImpl::getUserAgent()
{
    const char* defaultUA = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.2171.99 Safari/537.36";
    BlinkPlatformImpl* self = (BlinkPlatformImpl*)blink::Platform::current();
    if (!self)
        return defaultUA;
    return self->m_userAgent->utf8().data();
}

void BlinkPlatformImpl::setUserAgent(const char* ua)
{
    if (m_userAgent)
        delete m_userAgent;
    m_userAgent = new String(ua);
}

void readJsFile(const wchar_t* path, std::vector<char>* buffer)
{
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        DebugBreak();
        return;
    }

    DWORD fileSizeHigh;
    const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);

    DWORD numberOfBytesRead = 0;
    buffer->resize(bufferSize);
    BOOL b = ::ReadFile(hFile, &buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
    ::CloseHandle(hFile);
    b = b;
}

blink::WebData BlinkPlatformImpl::loadResource(const char* name)
{
    if (0 == strcmp("html.css", name))
        return blink::WebData(blink::htmlUserAgentStyleSheet, sizeof(blink::htmlUserAgentStyleSheet));
    else if (0 == strcmp("quirks.css", name))
        return blink::WebData(blink::quirksUserAgentStyleSheet, sizeof(blink::quirksUserAgentStyleSheet));
    else if (0 == strcmp("themeWin.css", name))
        return blink::WebData(blink::themeWinUserAgentStyleSheet, sizeof(blink::themeWinUserAgentStyleSheet));
    else if (0 == strcmp("svg.css", name))
        return blink::WebData(blink::svgUserAgentStyleSheet, sizeof(blink::svgUserAgentStyleSheet));
    else if (0 == strcmp("themeChromiumSkia.css", name))
        return blink::WebData(blink::themeChromiumSkiaUserAgentStyleSheet, sizeof(blink::themeChromiumSkiaUserAgentStyleSheet));
    else if (0 == strcmp("themeChromium.css", name))
        return blink::WebData(blink::themeChromiumUserAgentStyleSheet, sizeof(blink::themeChromiumUserAgentStyleSheet));
    else if (0 == strcmp("themeWinQuirks.css", name))
        return blink::WebData(blink::themeWinQuirksUserAgentStyleSheet, sizeof(blink::themeWinQuirksUserAgentStyleSheet));
    else if (0 == strcmp("missingImage", name) || 0 == strcmp("nullPlugin", name))
        return blink::WebData((const char*)content::gMissingImageData, sizeof(content::gMissingImageData));
    else if (0 == strcmp("textAreaResizeCorner", name))
        return blink::WebData((const char*)content::gTextAreaResizeCornerData, sizeof(content::gTextAreaResizeCornerData));
    else if (0 == strcmp("textAreaResizeCorner@2x", name))
        return blink::WebData((const char*)content::gTextAreaResizeCornerData, sizeof(content::gTextAreaResizeCornerData));
    else if (0 == strcmp("mediaControls.css", name))
        return blink::WebData((const char*)blink::mediaControlsAndroidUserAgentStyleSheet, sizeof(blink::mediaControlsAndroidUserAgentStyleSheet));
    //////////////////////////////////////////////////////////////////////////
    else if (0 == strcmp("calendarPicker.css", name))
        return blink::WebData((const char*)content::calendarPickerCss, sizeof(content::calendarPickerCss));
    else if (0 == strcmp("calendarPicker.js", name))
        return blink::WebData((const char*)content::calendarPickerJs, sizeof(content::calendarPickerJs));
    else if (0 == strcmp("colorSuggestionPicker.css", name))
        return blink::WebData((const char*)content::colorSuggestionPickerCss, sizeof(content::colorSuggestionPickerCss));
    else if (0 == strcmp("colorSuggestionPicker.js", name))
        return blink::WebData((const char*)content::colorSuggestionPickerJs, sizeof(content::colorSuggestionPickerJs));
    else if (0 == strcmp("listPicker.css", name))
        return blink::WebData((const char*)content::listPickerCss, sizeof(content::listPickerCss));
    else if (0 == strcmp("listPicker.js", name))
        return blink::WebData((const char*)content::listPickerJs, sizeof(content::listPickerJs));
    else if (0 == strcmp("pickerButton.css", name))
        return blink::WebData((const char*)content::pickerButtonCss, sizeof(content::pickerButtonCss));
    else if (0 == strcmp("pickerCommon.css", name))
        return blink::WebData((const char*)content::pickerCommonCss, sizeof(content::pickerCommonCss));
    else if (0 == strcmp("pickerCommon.js", name))
        return blink::WebData((const char*)content::pickerCommonJs, sizeof(content::pickerCommonJs));
    else if (0 == strcmp("suggestionPicker.css", name))
        return blink::WebData((const char*)content::suggestionPickerCss, sizeof(content::suggestionPickerCss));
    else if (0 == strcmp("suggestionPicker.js", name))
        return blink::WebData((const char*)content::suggestionPickerJs, sizeof(content::suggestionPickerJs));
    else if (0 == strcmp("PrivateScriptRunner.js", name))
        return blink::WebData((const char*)content::PrivateScriptRunnerJs, sizeof(content::PrivateScriptRunnerJs));
    else if (0 == strcmp("HTMLMarqueeElement.js", name))
        return blink::WebData((const char*)content::HTMLMarqueeElementJs, sizeof(content::HTMLMarqueeElementJs));
    else if (0 == strcmp("PluginPlaceholderElement.js", name))
        return blink::WebData((const char*)content::PluginPlaceholderElementJs, sizeof(content::PluginPlaceholderElementJs));
    else if (0 == strcmp("DebuggerScriptSource.js", name)) {
//         std::vector<char> buffer;
//         readJsFile(L"E:\\mycode\\miniblink49\\trunk\\third_party\\WebKit\\Source\\core\\inspector\\DebuggerScript.js", &buffer);
//         return blink::WebData(&buffer[0], buffer.size());
        return blink::WebData((const char*)content::DebuggerScriptSourceJs, sizeof(content::DebuggerScriptSourceJs));
    } else if (0 == strcmp("InjectedScriptSource.js", name))
        return blink::WebData((const char*)content::InjectedScriptSourceJs, sizeof(content::InjectedScriptSourceJs));
    else if (0 == strcmp("InspectorOverlayPage.html", name))
        return blink::WebData((const char*)content::InspectorOverlayPageHtml, sizeof(content::InspectorOverlayPageHtml));
    else if (0 == strcmp("xhtmlmp.css", name)) {
        char xhtmlmpCss[] = "@viewport {width: auto;min-zoom: 0.25;max-zoom: 5;}";
        return blink::WebData(xhtmlmpCss, sizeof(xhtmlmpCss));
    }

    notImplemented();
    return blink::WebData(" ", 1);
}

blink::WebThemeEngine* BlinkPlatformImpl::themeEngine()
{
    if (nullptr == m_webThemeEngine)
        m_webThemeEngine = new WebThemeEngineImpl();
    return m_webThemeEngine;
}

blink::WebMimeRegistry* BlinkPlatformImpl::mimeRegistry()
{
    if (nullptr == m_mimeRegistry)
        m_mimeRegistry = new WebMimeRegistryImpl();
    return m_mimeRegistry;
}

blink::WebCompositorSupport* BlinkPlatformImpl::compositorSupport()
{
    if (!m_webCompositorSupport)
        m_webCompositorSupport = new cc_blink::WebCompositorSupportImpl();
    return m_webCompositorSupport;
}

blink::WebScrollbarBehavior* BlinkPlatformImpl::scrollbarBehavior()
{
    if (!m_webScrollbarBehavior)
        m_webScrollbarBehavior = new blink::WebScrollbarBehavior();
    return m_webScrollbarBehavior;
}

uint32_t BlinkPlatformImpl::getUniqueIdForProcess()
{
    return ::GetCurrentProcessId();
}

void BlinkPlatformImpl::createMessageChannel(blink::WebMessagePortChannel** channel1, blink::WebMessagePortChannel** channel2)
{
    PlatformMessagePortChannel::MessagePortQueue* queue1 = PlatformMessagePortChannel::MessagePortQueue::create();
    PlatformMessagePortChannel::MessagePortQueue* queue2 = PlatformMessagePortChannel::MessagePortQueue::create();

    WebMessagePortChannelImpl* channelImpl1 = new WebMessagePortChannelImpl(queue1, queue2);
    WebMessagePortChannelImpl* channelImpl2 = new WebMessagePortChannelImpl(queue2, queue1);

    channelImpl1->getChannel()->m_entangledChannel = channelImpl2->getChannel();
    channelImpl2->getChannel()->m_entangledChannel = channelImpl1->getChannel();

    *channel1 = channelImpl1;
    *channel2 = channelImpl2;
}

blink::WebURLError BlinkPlatformImpl::cancelledError(const blink::WebURL& url) const
{
    blink::WebURLError error;
    error.reason = -3; // net::ERR_ABORTED
    error.domain = blink::WebString(url.string());
    error.localizedDescription = blink::WebString::fromUTF8("url cancelledError\n");
    error.isCancellation = true;
    error.staleCopyInCache = false;
    WTF::String outError = "url cancelledError:";
    outError.append((WTF::String)url.string());
    outError.append("\n");
    OutputDebugStringW(outError.charactersWithNullTermination().data());

    return error;
}

blink::WebStorageNamespace* BlinkPlatformImpl::createLocalStorageNamespace()
{
    if (!m_localStorageStorageMap)
        m_localStorageStorageMap = new DOMStorageMapWrap();
    return new blink::WebStorageNamespaceImpl(blink::kLocalStorageNamespaceId, &m_localStorageStorageMap->map, true);
}

blink::WebStorageNamespace* BlinkPlatformImpl::createSessionStorageNamespace()
{
    if (!m_sessionStorageStorageMap)
        m_sessionStorageStorageMap = new DOMStorageMapWrap();
    return new blink::WebStorageNamespaceImpl(m_storageNamespaceIdCount++, &m_sessionStorageStorageMap->map, false);
}

bool BlinkPlatformImpl::portAllowed(const blink::WebURL&) const
{
    return true;
}

// Resources -----------------------------------------------------------
blink::WebString BlinkPlatformImpl::queryLocalizedString(blink::WebLocalizedString::Name name)
{
    return queryLocalizedStringFromResources(name);
}

blink::WebString BlinkPlatformImpl::queryLocalizedString(blink::WebLocalizedString::Name, const blink::WebString& parameter)
{
    return blink::WebString();
}

blink::WebString BlinkPlatformImpl::queryLocalizedString(blink::WebLocalizedString::Name, const blink::WebString& parameter1, const blink::WebString& parameter2)
{
    return blink::WebString();
}

blink::WebCrypto* BlinkPlatformImpl::crypto() {
    
    if (!m_webCryptoImpl)
        m_webCryptoImpl = new WebCryptoImpl();
    return m_webCryptoImpl;
}

// Blob ----------------------------------------------------------------
blink::WebBlobRegistry* BlinkPlatformImpl::blobRegistry()
{
    if (!m_blobRegistryImpl)
        m_blobRegistryImpl = new WebBlobRegistryImpl();
    return m_blobRegistryImpl;
}

// clipboard ----------------------------------------------------------------

blink::WebClipboard* BlinkPlatformImpl::clipboard()
{
    if (!m_clipboardImpl)
        m_clipboardImpl = new WebClipboardImpl();
    return m_clipboardImpl;
}

// Plugins -------------------------------------------------------------
void BlinkPlatformImpl::getPluginList(bool refresh, blink::WebPluginListBuilder* builder)
{
//     builder->addPlugin(blink::WebString::fromUTF8("Shockwave Flash"), blink::WebString::fromUTF8("flashPlugin"), blink::WebString::fromUTF8(".swf"));
//     builder->addMediaTypeToLastPlugin(blink::WebString::fromUTF8("application/x-shockwave-flash"), blink::WebString::fromUTF8("flashPlugin"));
//     builder->addFileExtensionToLastMediaType(blink::WebString::fromUTF8(".swf"));

    const Vector<PluginPackage*>& plugins = PluginDatabase::installedPlugins()->plugins();

    for (size_t i = 0; i < plugins.size(); ++i) {
        PluginPackage* package = plugins[i];
        String name = package->name();
        String desc = package->description();
        String file = package->fileName();
        builder->addPlugin(name, desc, file);

        const MIMEToDescriptionsMap& mimeToDescriptions = package->mimeToDescriptions();
        MIMEToDescriptionsMap::const_iterator end = mimeToDescriptions.end();

        for (MIMEToDescriptionsMap::const_iterator it = mimeToDescriptions.begin(); it != end; ++it) {
            String type = it->key;
            String desc = it->value;
            builder->addMediaTypeToLastPlugin(type, desc);

            Vector<String> extensions = package->mimeToExtensions().get(type);
            for (size_t j = 0; j < extensions.size(); ++j) {
                builder->addFileExtensionToLastMediaType(extensions[j]);
            }
        }
    }
}

blink::WebFileUtilities* BlinkPlatformImpl::fileUtilities()
{
    if (!m_webFileUtilitiesImpl)
        m_webFileUtilitiesImpl = new WebFileUtilitiesImpl();
    return m_webFileUtilitiesImpl;
}

// WaitableEvent -------------------------------------------------------

WebWaitableEvent* BlinkPlatformImpl::createWaitableEvent(blink::WebWaitableEvent::ResetPolicy policy, blink::WebWaitableEvent::InitialState state)
{
    return new WaitableEvent(policy == blink::WebWaitableEvent::ResetPolicy::Manual, state == blink::WebWaitableEvent::InitialState::Signaled);
}

WebWaitableEvent* BlinkPlatformImpl::waitMultipleEvents(const blink::WebVector<blink::WebWaitableEvent*>& webEvents)
{
    Vector<WaitableEvent*> events;
    for (size_t i = 0; i < webEvents.size(); ++i)
        events.append((WaitableEvent*)(webEvents[i]));

    size_t idx = WaitableEvent::waitMany(events.data(), events.size());
    ASSERT(idx < webEvents.size());
    return webEvents[idx];
}

size_t BlinkPlatformImpl::numberOfProcessors() { return m_numberOfProcessors; }
void BlinkPlatformImpl::setNumberOfProcessors(size_t num) { m_numberOfProcessors = num; }

} // namespace content