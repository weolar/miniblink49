
#include "config.h"
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
#include "content/resources/MediaPlayerData.h"
#include "content/resources/ViewSource.h"
#include "content/browser/WebPage.h"
#include "content/browser/PlatformMessagePortChannel.h"
#include "content/browser/PostTaskHelper.h"
#include "mc/blink/WebCompositorSupportImpl.h"
#include "mc/raster/RasterTask.h"
#ifndef NO_USE_ORIG_CHROME
#include "orig_chrome/content/OrigChromeMgr.h"
#endif
#include "third_party/WebKit/public/web/WebKit.h"
#include "third_party/WebKit/Source/core/fetch/MemoryCache.h"
#include "third_party/WebKit/public/platform/WebScrollbarBehavior.h"
#include "third_party/WebKit/public/platform/WebPluginListBuilder.h"
#include "third_party/WebKit/Source/platform/WebThreadSupportingGC.h"
#include "third_party/WebKit/Source/platform/plugins/PluginData.h"
#include "third_party/WebKit/Source/platform/PartitionAllocMemoryDumpProvider.h"
#include "third_party/WebKit/Source/platform/fonts/FontCache.h"
#include "third_party/WebKit/Source/platform/heap/BlinkGCMemoryDumpProvider.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8GCController.h"
#include "third_party/WebKit/Source/core/loader/ImageLoader.h" // TODO
#include "third_party/WebKit/Source/core/html/HTMLLinkElement.h" // TODO
#include "third_party/WebKit/Source/core/html/HTMLStyleElement.h" // TODO
#include "third_party/WebKit/Source/core/css/resolver/StyleResolver.h"
#include "third_party/WebKit/Source/core/timing/GcTimeScheduler.h"
#include "third_party/skia/include/core/SkGraphics.h"
#include "third_party/skia/include/core/SkGraphics.h"
#include "gen/blink/core/UserAgentStyleSheets.h"
#include "gen/blink/platform/RuntimeEnabledFeatures.h"
#include "ui/gfx/win/dpi.h"
#include "gin/public/isolate_holder.h"
#include "gin/array_buffer.h"
#include "net/ActivatingObjCheck.h"
#include "net/WebURLLoaderManager.h"
#include "net/WebStorageNamespaceImpl.h"
#include "net/DownloadFileBlobCache.h"
#include "net/WebURLLoaderManagerUtil.h"
#include "net/DataURL.h"
#include "wke/wkeUtil.h"
#include "wke/wkeGlobalVar.h"
#include "wke/wkeWebView.h"
#include "base/rand_util.h"
#include "base/values.h"
#include "base/time/time.h"
#include "base/WindowsVersion.h"
#include <crtdbg.h>
#include <iosfwd>
#include <sstream>

// #define VLD_FORCE_ENABLE 1
// #include "C:\\Program Files (x86)\\Visual Leak Detector\\include\\vld.h"

DWORD g_paintToMemoryCanvasInUiThreadCount = 0;
DWORD g_rasterTaskCount = 0;
DWORD g_mouseCount = 0;
DWORD g_paintCount = 0;
DWORD g_scheduledActionCount = 0;
double g_autoRecordActionsTime = 0;

#ifdef _DEBUG

//#include "base/process/InjectTool.h"

size_t g_v8MemSize = 0;
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

typedef void*  (__cdecl* MyRealloc)(void*, size_t);
MyRealloc myRealloc = nullptr;

void* __cdecl newRealloc(void* p, size_t s)
{
    if (p && 0x1122dd44 == *(size_t*)p) {
        DebugBreak();
    }
    return myRealloc(p, s);
}

typedef void* (__cdecl* MyMalloc)(size_t);
MyMalloc myMalloc = nullptr;

void* __cdecl newMalloc(size_t s)
{
    if (s == 256)
        s = s;
    return myMalloc(s);
}

#endif

static void onAllocationHook(void* address, size_t, const char* typeName)
{
//     char* output = (char*)malloc(0x100);
//     sprintf_s(output, 0x99, "onAllocationHook: %p\n", address);
//     OutputDebugStringA(output);
//     free(output);
    //net::ActivatingObjCheck::inst()->add((intptr_t)address);
}

static void onFreeHook(void* address)
{
//     char* output = (char*)malloc(0x100);
//     sprintf_s(output, 0x99, "onFreeHook: %p\n", address);
//     OutputDebugStringA(output);
//     free(output);
// 
//     if (!net::ActivatingObjCheck::inst()->isActivating((intptr_t)address))
//         DebugBreak();
//     net::ActivatingObjCheck::inst()->remove((intptr_t)address);
}

// hook 这种类型的：76121030    FF 25 2C 90 18 76    jmp dword ptr [__imp__NtUserBeginPaint@8 (7618902Ch)] 
BOOL HookAddrByJmp(void* addr, PROC pfnNew, PROC* pfnOld)
{
    FARPROC pFunc = (FARPROC)addr;
    DWORD dwOldProtect = 0;
    DWORD dwAddress = 0;

    BYTE pBuf[5] = { 0xE9, 0, };
    BYTE pBuf2[2] = { 0xEB, 0xF9 };

    PBYTE pByte = (PBYTE)pFunc;
    if ((pByte[0] != 0xFF || pByte[1] != 0x25))
        return FALSE;

    PBYTE pNopByte = pByte - 5;
    if ((pNopByte[0] != 0xcc && pNopByte[0] != 0x90) || 
        (pNopByte[1] != 0xcc && pNopByte[1] != 0x90) ||
        (pNopByte[2] != 0xcc && pNopByte[2] != 0x90) ||
        (pNopByte[3] != 0xcc && pNopByte[3] != 0x90) ||
        (pNopByte[4] != 0xcc && pNopByte[4] != 0x90)
        )
        return FALSE;

    if (pfnOld)
        *pfnOld = **(PROC**)(&pByte[2]);

    VirtualProtect((LPVOID)((DWORD)pFunc - 5), 7, PAGE_EXECUTE_READWRITE, &dwOldProtect);

    dwAddress = (DWORD)pfnNew - (DWORD)pFunc; //计算Hook函数和被Hook函数的地址偏移

    memcpy(&pBuf[1], &dwAddress, 4); //将偏移地址拼凑到“JMP XXXXx"中

    memcpy((LPVOID)((DWORD)pFunc - 5), pBuf, 5); //将“JMP pfnNew”写入pFunc-5的位置，也就是五个NOP的位置

    // 2. MOV EDI, EDI (0x8BFF)
    // 将“JMP-7”写入pFunc的位置，也就是MOV EDI, EDI的位置
    memcpy(pFunc, pBuf2, 2);

    VirtualProtect((LPVOID)((DWORD)pFunc - 5), 7, dwOldProtect, &dwOldProtect);
    return TRUE;
}

BOOL HookAddrByHotpatch(void* addr, PROC pfnNew, PROC* pfnOld)
{
    FARPROC pFunc = (FARPROC)addr;
    DWORD dwOldProtect = 0;
    DWORD dwAddress = 0;

    BYTE pBuf[5] = { 0xE9, 0, };
    BYTE pBuf2[2] = { 0xEB, 0xF9 };

    PBYTE pByte = (PBYTE)pFunc;

    if ((pByte[0] == 0xFF && pByte[1] == 0x25)) // jmp dword ptr [__imp__NtUserBeginPaint@8 (7618902Ch)] 
        return HookAddrByJmp(addr, pfnNew, pfnOld); 

    if ((pByte[0] != 0x8B || pByte[1] != 0xff) && (pByte[0] != 0xFF || pByte[1] != 0x25))
        return FALSE;

    if (pfnOld)
        *pfnOld = (PROC)&pByte[2];

    VirtualProtect((LPVOID)((DWORD)pFunc - 5), 7, PAGE_EXECUTE_READWRITE, &dwOldProtect);

    dwAddress = (DWORD)pfnNew - (DWORD)pFunc; //计算Hook函数和被Hook函数的地址偏移

    memcpy(&pBuf[1], &dwAddress, 4); //将偏移地址拼凑到“JMP XXXXx"中

    memcpy((LPVOID)((DWORD)pFunc - 5), pBuf, 5); //将“JMP pfnNew”写入pFunc-5的位置，也就是五个NOP的位置

    // 2. MOV EDI, EDI (0x8BFF)
    // 将“JMP-7”写入pFunc的位置，也就是MOV EDI, EDI的位置
    memcpy(pFunc, pBuf2, 2);

    VirtualProtect((LPVOID)((DWORD)pFunc - 5), 7, dwOldProtect, &dwOldProtect);
    return TRUE;
}

BOOL HookByHotpatch(LPCWSTR szDllName, LPCSTR szFuncName, PROC pfnNew, PROC* pfnOld)
{
    FARPROC pFunc = (FARPROC)GetProcAddress(LoadLibraryW(szDllName), szFuncName);
    return HookAddrByHotpatch(pFunc, pfnNew, pfnOld);
}

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

DWORD sCurrentThreadTlsKey = -1;

void traceEventSamplingState(const char* category, const char* name, const char* funcName)
{
//     std::string funcitonName(funcName);
//     if (funcitonName == "blink::TimerBase::runInternal")
//         return;
//     if (funcitonName == "blink::MouseEventV8Internal::xAttributeGetterCallback")
//         return;
//     if (funcitonName == "blink::MouseEventV8Internal::yAttributeGetterCallback")
//         return;
//     if (funcitonName == "blink::MouseEventV8Internal::yAttributeGetterCallback")
//         return;
// 
//     funcitonName.insert(0, "trace:");
//     funcitonName += "\n";
//     OutputDebugStringA(funcitonName.c_str());
}

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
    blink::RuntimeEnabledFeatures::setCspCheckEnabled(false);
    blink::RuntimeEnabledFeatures::setNpapiPluginsEnabled(true);
    blink::RuntimeEnabledFeatures::setDOMConvenienceAPIEnabled(true);
    blink::RuntimeEnabledFeatures::setTextBlobEnabled(true);
    blink::RuntimeEnabledFeatures::setCssVariablesEnabled(true);
    blink::RuntimeEnabledFeatures::setCSSMotionPathEnabled(true);
    blink::RuntimeEnabledFeatures::setKeyboardEventKeyEnabled(true);
    blink::RuntimeEnabledFeatures::setKeyboardEventCodeEnabled(true);
    blink::RuntimeEnabledFeatures::setCSSOMSmoothScrollEnabled(true);
    blink::RuntimeEnabledFeatures::setCompositorAnimationTimelinesEnabled(true);

    blink::RuntimeEnabledFeatures::setAlwaysUseComplexTextEnabled(true);
    blink::FontCache::setUseDirectWrite(base::getWindowsVersion(nullptr, nullptr) >= base::WindowsVista);
}

typedef BOOL (WINAPI* PFN_SetThreadStackGuarantee)(PULONG StackSizeInBytes);

typedef HANDLE (WINAPI* PFN_CreateFileW)(
    __in     LPCWSTR lpFileName,
    __in     DWORD dwDesiredAccess,
    __in     DWORD dwShareMode,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in     DWORD dwCreationDisposition,
    __in     DWORD dwFlagsAndAttributes,
    __in_opt HANDLE hTemplateFile
    );
PFN_CreateFileW s_CreateFileW = nullptr;

HANDLE WINAPI HookCreateFileW(
    __in     LPCWSTR lpFileName,
    __in     DWORD dwDesiredAccess,
    __in     DWORD dwShareMode,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in     DWORD dwCreationDisposition,
    __in     DWORD dwFlagsAndAttributes,
    __in_opt HANDLE hTemplateFile
    )
{
    return s_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

class IdleGcWorker : public blink::WebThread::Task {
public:
    IdleGcWorker(BlinkPlatformImpl* self)
    {
        m_self = self;
    }

    virtual ~IdleGcWorker() override {}

    virtual void run() override
    {
        m_self->doGarbageCollected();
    }
private:
    BlinkPlatformImpl* m_self;
};

void BlinkPlatformImpl::initialize(bool ocEnable)
{
#if USING_VC6RT == 1
    scrt_initialize_thread_safe_statics();
#endif
    x86_check_features();

    //_control87(0x133f, 0xffff);
//     unsigned int control_word_x87 = 0;
//     __control87_2(_PC_64, MCW_PC, &control_word_x87, 0);

    //control87(_PC_64, MCW_PC);

//     ULONG stackSizeInBytes = 894 * 1024;
//     PFN_SetThreadStackGuarantee pSetThreadStackGuarantee = (PFN_SetThreadStackGuarantee)::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "SetThreadStackGuarantee");
//     pSetThreadStackGuarantee(&stackSizeInBytes);

    //HookByHotpatch(L"C:\\Windows\\SysWOW64\\kernel32.dll", (LPCSTR)"CreateFileW", ((PROC)(HookCreateFileW)), (PROC*)&s_CreateFileW);
    
    ::CoInitializeEx(nullptr, 0); // COINIT_MULTITHREADED
    ::OleInitialize(nullptr);

#ifndef NO_USE_ORIG_CHROME
    if (ocEnable)
      OrigChromeMgr::init();
#endif

    setRuntimeEnabledFeatures();

    //     v8::V8::SetFlagsFromString("--turbo", strlen("--turbo"));
    //     v8::V8::SetFlagsFromString("--scavenge_reclaim_unmodified_objects", strlen("--scavenge_reclaim_unmodified_objects"));

    // 75版本开启V8_CONCURRENT_MARKING_BOOL会有崩溃。测试网站：
    // https://xsdt1.i-xinnuo.com/xqc_pc/xqc.html#/pdfPage?title=%E5%90%88%E5%90%8C%E6%AD%A3%E6%96%87%E6%B5%8B%E8%AF%95.pdf&fileId=60ff7c7be4b017f601627438
    // 记得设置ua："Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like XSD) Chrome/79.0.3945.130 Safari/537.36"
    // 57版本开启incremental_marking访问上面网址也有崩溃
#if V8_MAJOR_VERSION >= 7
    const char* v8flags[] = {
        "--harmony_dynamic_import",
        "--harmony_import_meta",
        "--no_future",
        "--liftoff",
        "--no_wasm_lazy_compilation",
        "--no_experimental_wasm_simd",
        "--harmony_sharedarraybuffer",
        "--experimental_wasm_threads",
        "--wasm_tier_up",
        "--no_wasm_trap_handler",
        "--no_untrusted_code_mitigations",
        "--no_single_threaded_gc",
        "--harmony_await_optimization",
        /////
        "--young_generation_large_objects",
        "--no_incremental_marking", // 暂不支持incremental_marking。主要是UnifiedHeapController::AdvanceTracing在独立线程，因为没有v8::context导致崩溃
        "--no_incremental_marking_wrappers",
        //       "--no_parallel_scavenge",
        "--no_concurrent_marking",
        "--no_parallel_marking",
        "--no_trace_concurrent_marking",
        
        //       "--no_concurrent_store_buffer",
        //       "--no_concurrent_sweeping",
        //       "--no_parallel_compaction",
        //       "--no_parallel_pointer_update",
        //       "--no_concurrent_array_buffer_freeing",
        // 
        //       "--block_concurrent_recompilation",
        nullptr
    };
#else
    const char* v8flags[] = {
        //"--block_concurrent_recompilation",
        "--no_incremental_marking",
        "--no_incremental_marking_wrappers",
        nullptr
    };
#endif
    for (size_t i = 0; v8flags[i]; i++) {
        v8::V8::SetFlagsFromString(v8flags[i], strlen(v8flags[i]));
    }

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

//     platform->m_defaultGcTimer = new blink::Timer<BlinkPlatformImpl>(platform, &BlinkPlatformImpl::garbageCollectedTimer);
//     platform->m_defaultGcTimer->start(40, 40, FROM_HERE);
// 
//     platform->m_resTimer = new blink::Timer<BlinkPlatformImpl>(platform, &BlinkPlatformImpl::resourceGarbageCollectedTimer);
//     platform->m_resTimer->start(120, 120, FROM_HERE);
    platform->m_gcTimeScheduler = new blink::GcTimeScheduler();
    platform->m_mainThread->postDelayedTask(FROM_HERE, new IdleGcWorker(platform), 3000);

    WTF::PartitionAllocHooks::setAllocationHook(onAllocationHook);
    WTF::PartitionAllocHooks::setFreeHook(onFreeHook);

    net::sharedResourceMutex(CURL_LOCK_DATA_COOKIE);
}

BlinkPlatformImpl::BlinkPlatformImpl() 
{
    content::g_uiThreadId = ::GetCurrentThreadId();
    m_mainThreadId = -1;
    m_zoom = 1;
    m_webThemeEngine = nullptr;
    m_mimeRegistry = nullptr;
    m_clipboardImpl = nullptr;
    m_ccCompositorSupport = nullptr;
    m_mcCompositorSupport = nullptr;
    m_webScrollbarBehavior = nullptr;
#ifdef MINIBLINK_NO_PAGE_LOCALSTORAGE
    m_localStorageStorageMap = nullptr;
#endif
    //m_sessionStorageStorageMap = nullptr;
    m_webFileUtilitiesImpl = nullptr;
    m_blobRegistryImpl = nullptr;
    m_webCryptoImpl = nullptr;
    m_userAgent = nullptr;
    m_storageNamespaceIdCount = 1;
    m_lock = new CRITICAL_SECTION();
    m_threadNum = 0;
//     m_gcTimer = nullptr;
//     m_perfTimer = nullptr;
    m_firstMonotonicallyIncreasingTime = currentTime(); // (GetTickCount() / 1000.0);
    m_numberOfProcessors = 1;
    m_isDisableGC = false;

    ::InitializeCriticalSection(m_lock);

    setUserAgent(getUserAgent());

#ifdef _DEBUG
    //myFree = (MyFree)ReplaceFuncAndCopy(free, newFree);
    //myRealloc = (MyRealloc)ReplaceFuncAndCopy(realloc, newRealloc);
    //myMalloc = (MyMalloc)ReplaceFuncAndCopy(malloc, newMalloc);
    //_CrtSetBreakAlloc(3584109);
#endif

    //VLDEnable();
}

BlinkPlatformImpl::~BlinkPlatformImpl()
{
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

#ifndef NO_USE_ORIG_CHROME
//     if (m_ccCompositorSupport)
//         delete m_ccCompositorSupport;
//     m_ccCompositorSupport = nullptr;

    OrigChromeMgr::shutdown();
#endif
//     if (m_mcCompositorSupport)
//         delete m_mcCompositorSupport;
//     m_mcCompositorSupport = nullptr;

    if (m_webScrollbarBehavior)
        delete m_webScrollbarBehavior;
    m_webScrollbarBehavior = nullptr;

#ifdef MINIBLINK_NO_PAGE_LOCALSTORAGE
    if (m_localStorageStorageMap)
        delete m_localStorageStorageMap;
    m_localStorageStorageMap = nullptr;
#endif

//     if (m_sessionStorageStorageMap)
//         delete m_sessionStorageStorageMap;
//     m_sessionStorageStorageMap = nullptr;

    if (m_clipboardImpl)
        delete m_clipboardImpl;
    m_clipboardImpl = nullptr;

    if (m_userAgent)
        delete m_userAgent;
    m_userAgent = nullptr;

    wke::releaseGlobalVar();
}

void BlinkPlatformImpl::registerMemoryDumpProvider(blink::WebMemoryDumpProvider*) {}
void BlinkPlatformImpl::unregisterMemoryDumpProvider(blink::WebMemoryDumpProvider* provider)
{
    if (provider == blink::BlinkGCMemoryDumpProvider::instance()) {
        
    }

    if (provider == blink::PartitionAllocMemoryDumpProvider::instance()) {
        
    }
}

void shutdownIoThread(blink::WebThreadSupportingGC* webThread, int* waitCount)
{
    webThread->shutdown();
    atomicDecrement(waitCount);
}

void BlinkPlatformImpl::preShutdown()
{
    WebPluginImpl::shutdown();
    destroyWebInfo();

    int waitCount = m_ioThreads.size();
    for (size_t i = 0; i < m_ioThreads.size(); ++i) {
        blink::WebThreadSupportingGC* webThread = m_ioThreads[i];
        webThread->platformThread().postTask(FROM_HERE, WTF::bind(&shutdownIoThread, webThread, &waitCount));
    }
    while (waitCount) { ::Sleep(10); }
    for (size_t i = 0; i < m_ioThreads.size(); ++i) {
        blink::WebThreadSupportingGC* webThread = m_ioThreads[i];
        delete webThread;
    }

//     if (m_gcTimer)
//         delete m_gcTimer;
//     delete m_defaultGcTimer;
//     delete m_resTimer;

    WebThread* mainThread = m_mainThread;
    delete mainThread;
}

void BlinkPlatformImpl::shutdown()
{
    wke::freeV8TempObejctOnOneFrameBefore();

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
#if V8_MAJOR_VERSION >= 7
    blink::V8PerIsolateData::from(isolate)->leakV8References();
#endif
    ((WebThreadImpl*)currentThread())->fire();

    net::WebURLLoaderManager::sharedInstance()->shutdown();
    wke::CWebView::shutdown();

    ((WebThreadImpl*)currentThread())->fire();

    blink::memoryCache()->evictResources();
    isolate->LowMemoryNotification();

    mc::RasterTaskWorkerThreadPool* rasterPool = mc::RasterTaskWorkerThreadPool::shared();
    rasterPool->shutdown();
 
    SkGraphics::PurgeResourceCache();
    SkGraphics::PurgeFontCache();
    SkGraphics::Term();

    net::ActivatingObjCheck::inst()->shutdown();

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
    blink::Heap::collectAllGarbage();

    net::DownloadFileBlobCache::inst()->shutdown();

    blink::shutdown();
    closeThread();

    net::ActivatingObjCheck::inst()->destroy();

#ifdef _DEBUG
    size_t v8MemSize = g_v8MemSize;
    v8MemSize += g_blinkMemSize;
    v8MemSize += g_skiaMemSize;
    g_callAddrsRecord;
#endif
    delete this;

    //_CrtDumpMemoryLeaks();
    //VLDDisable();
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
    //doGarbageCollected();
}

void BlinkPlatformImpl::garbageCollectedTimer(blink::Timer<BlinkPlatformImpl>*)
{
    //doGarbageCollected();
}

void BlinkPlatformImpl::doGarbageCollected()
{
    m_mainThread->postDelayedTask(FROM_HERE, new IdleGcWorker(this), 3000);
    if (m_isDisableGC)
        return;
    if (!m_gcTimeScheduler->needGc())
        return;

    v8::Isolate* isolate = v8::Isolate::GetCurrent();

#if V8_MAJOR_VERSION >= 7
    blink::V8PerIsolateData::from(isolate)->leakV8References();
#endif
    
    isolate->LowMemoryNotification();

    SkGraphics::PurgeResourceCache();
    SkGraphics::PurgeFontCache();

    WebPage::gcAll();

#if 0 // def _DEBUG
    String out = String::format("BlinkPlatformImpl::doGarbageCollected: %d %d %d\n", g_v8MemSize, g_blinkMemSize, g_skiaMemSize);
    OutputDebugStringA(out.utf8().data());
#endif
}

void BlinkPlatformImpl::setGcTimer(double intervalSec)
{
//     if (!m_gcTimer)
//         m_gcTimer = new blink::Timer<BlinkPlatformImpl>(this, &BlinkPlatformImpl::garbageCollectedTimer);
// 
//     if (!m_gcTimer->isActive() || intervalSec < m_gcTimer->nextFireInterval())
//         m_gcTimer->startOneShot(intervalSec, FROM_HERE);
// 
//     if (m_defaultGcTimer)
//         delete m_defaultGcTimer;
//     m_defaultGcTimer = nullptr;
    m_gcTimeScheduler->setNextFireInterval(intervalSec);
}

void BlinkPlatformImpl::setResGcTimer(double intervalSec)
{
//     m_resTimer->stop();
//     m_resTimer->startOneShot(intervalSec, FROM_HERE);
    m_gcTimeScheduler->setNextFireInterval(intervalSec);
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

static void initializeIoThread(blink::WebThreadSupportingGC* webThreadSupportingGC)
{
    webThreadSupportingGC->initialize();
}

WTF::Vector<blink::WebThread*> BlinkPlatformImpl::getIoThreads()
{
    WTF::Vector<blink::WebThread*> result;
    if (0 == m_ioThreads.size()) {
        for (size_t i = 0; i < 4; ++i) {
            blink::WebThread* threadPtr = createIoThread("ioThread");
            result.append(threadPtr);
        }
    }

    return result;
}

blink::WebThread* BlinkPlatformImpl::createIoThread(const char* threadName)
{
    blink::WebThreadSupportingGC* ioThread = blink::WebThreadSupportingGC::create(threadName).leakPtr();
    ioThread->platformThread().postTask(FROM_HERE, WTF::bind(&initializeIoThread, ioThread));
    m_ioThreads.push_back(ioThread);
    return &ioThread->platformThread();
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
    if (wke::g_defaultLocale.get())
        return blink::WebString::fromUTF8(wke::g_defaultLocale->c_str());
    return blink::WebString::fromUTF8("zh-CN");
}

double BlinkPlatformImpl::currentTime()
{
#if 1 // ndef NO_USE_ORIG_CHROME
    return base::Time::Now().ToDoubleT();
#else
    return currentTimeImpl();
#endif
}

double BlinkPlatformImpl::monotonicallyIncreasingTime() 
{
//     LARGE_INTEGER qpc;
//     QueryPerformanceCounter(&qpc);
//     return qpc.QuadPart / (1000 * 1000);

//     double timeInDouble = (double)GetTickCount();  
//     return (timeInDouble / 1000.0) - m_firstMonotonicallyIncreasingTime;

#if 1 // ndef NO_USE_ORIG_CHROME
    return base::TimeTicks::Now().ToInternalValue() / static_cast<double>(base::Time::kMicrosecondsPerSecond);
#else
    double timeInDouble = (double)currentTimeImpl();
    return timeInDouble - m_firstMonotonicallyIncreasingTime;
#endif
}

double BlinkPlatformImpl::systemTraceTime() 
{
    DebugBreak();
    return 0; 
}

blink::WebString BlinkPlatformImpl::userAgent()
{
    return blink::WebString::fromUTF8(m_userAgent->c_str());
}

const char* BlinkPlatformImpl::getUserAgent()
{
    const char* defaultUA = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3729.169 Safari/537.36";

    BlinkPlatformImpl* self = (BlinkPlatformImpl*)blink::Platform::current();
    if (!self)
        return defaultUA;
    return self->m_userAgent->c_str();
}

void BlinkPlatformImpl::setUserAgent(const char* ua)
{
    if (m_userAgent)
        delete m_userAgent;
    m_userAgent = new std::string(ua);
}

blink::WebData BlinkPlatformImpl::parseDataURL(const blink::WebURL& url, blink::WebString& mimeType, blink::WebString& charset)
{
    blink::WebData result;
    Vector<char> outData;
    WTF::String mimeTypeStr;
    WTF::String charsetStr;
    bool b = net::parseDataURL(url, mimeTypeStr, charsetStr, outData);
    if (!b)
        return result;

    mimeType = mimeTypeStr;
    charset = charsetStr;

    result.assign(outData.data(), outData.size());
    return result;
}

void readJsFile(const char* path, std::vector<char>* buffer)
{
    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
    if (0 == strcmp("html.css", name)) {
//         std::vector<char> buffer;
//         readJsFile("G:\\mycode\\mb\\third_party\\WebKit\\Source\\core\\css\\html.css", &buffer);
//         return blink::WebData(&buffer[0], buffer.size());
        return blink::WebData(blink::htmlUserAgentStyleSheet, sizeof(blink::htmlUserAgentStyleSheet));
    } else if (0 == strcmp("quirks.css", name))
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
    else if (0 == strcmp("mediaControls.css", name)) {
        std::string buffer(blink::mediaControlsUserAgentStyleSheet, sizeof(blink::mediaControlsUserAgentStyleSheet));;
        //buffer += "video::-webkit-media-controls {display: none;}";
        return blink::WebData(buffer.c_str(), buffer.size());
        //return blink::WebData((const char*)blink::mediaControlsUserAgentStyleSheet, sizeof(blink::mediaControlsUserAgentStyleSheet));
    } else if (0 == strcmp("fullscreen.css", name)) {
        return blink::WebData((const char*)content::fullscreenCss, sizeof(content::fullscreenCss));
    } else if (0 == strcmp("DocumentXMLTreeViewer.js", name)) {
        return blink::WebData((const char*)content::DocumentXMLTreeViewerJs, sizeof(content::DocumentXMLTreeViewerJs));
    } else if (0 == strcmp("DocumentXMLTreeViewer.css", name)) {
        return blink::WebData((const char*)content::DocumentXMLTreeViewerCss, sizeof(content::DocumentXMLTreeViewerCss));
    } else if (0 == strcmp("view-source.css", name)) {
//         std::vector<char> buffer;
//         readJsFile("E:\\mycode\\mb49-gee\\third_party\\WebKit\\Source\\core\\css\\view-source.css", &buffer);
//         return blink::WebData(&buffer[0], buffer.size());
        return blink::WebData((const char*)content::ViewSourceCss, sizeof(content::ViewSourceCss));
    }

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
        return blink::WebData((const char*)content::DebuggerScriptSourceJs, sizeof(content::DebuggerScriptSourceJs));
    } else if (0 == strcmp("InjectedScriptSource.js", name))
        return blink::WebData((const char*)content::InjectedScriptSourceJs, sizeof(content::InjectedScriptSourceJs));
    else if (0 == strcmp("InspectorOverlayPage.html", name))
        return blink::WebData((const char*)content::InspectorOverlayPageHtml, sizeof(content::InspectorOverlayPageHtml));
    else if (0 == strcmp("xhtmlmp.css", name)) {
        char xhtmlmpCss[] = "@viewport {width: auto;min-zoom: 0.25;max-zoom: 5;}";
        return blink::WebData(xhtmlmpCss, sizeof(xhtmlmpCss));
    }
    else if (0 == strcmp("mediaplayerSoundLevel0", name))
        return blink::WebData((const char*)content::MediaplayerSoundLevel0, sizeof(content::MediaplayerSoundLevel0));
    else if (0 == strcmp("mediaplayerSoundLevel1", name))
        return blink::WebData((const char*)content::MediaplayerSoundLevel1, sizeof(content::MediaplayerSoundLevel1));
    else if (0 == strcmp("mediaplayerSoundLevel2", name))
        return blink::WebData((const char*)content::MediaplayerSoundLevel2, sizeof(content::MediaplayerSoundLevel2));
    else if (0 == strcmp("mediaplayerSoundLevel3", name))
        return blink::WebData((const char*)content::MediaplayerSoundLevel3, sizeof(content::MediaplayerSoundLevel3));
    else if (0 == strcmp("mediaplayerSoundDisabled", name))
        return blink::WebData((const char*)content::MediaplayerSoundDisabled, sizeof(content::MediaplayerSoundDisabled));
    
    else if (0 == strcmp("mediaplayerPlay", name))
        return blink::WebData((const char*)content::MediaplayerPlay, sizeof(content::MediaplayerPlay));
    else if (0 == strcmp("mediaplayerPause", name))
        return blink::WebData((const char*)content::MediaplayerPause, sizeof(content::MediaplayerPause));
    else if (0 == strcmp("mediaplayerPlayDisabled", name))
        return blink::WebData((const char*)content::MediaplayerPlayDisabled, sizeof(content::MediaplayerPlayDisabled));

    else if (0 == strcmp("mediaplayerOverlayPlay", name))
        return blink::WebData((const char*)content::MediaplayerOverlayPlay, sizeof(content::MediaplayerOverlayPlay));
    else if (0 == strcmp("mediaplayerSliderThumb", name))
        return blink::WebData((const char*)content::MediaplayerSliderThumb, sizeof(content::MediaplayerSliderThumb));
    else if (0 == strcmp("mediaplayerVolumeSliderThumb", name))
        return blink::WebData((const char*)content::MediaplayerVolumeSliderThumb, sizeof(content::MediaplayerVolumeSliderThumb));

    else if (0 == strcmp("mediaplayerFullscreen", name))
        return blink::WebData((const char*)content::MediaplayerFullscreen, sizeof(content::MediaplayerFullscreen));
    else if (0 == strcmp("mediaplayerClosedCaption", name))
        return blink::WebData((const char*)content::MediaplayerClosedcaption, sizeof(content::MediaplayerClosedcaption));
    else if (0 == strcmp("mediaplayerClosedCaptionDisabled", name))
        return blink::WebData((const char*)content::MediaplayerClosedcaptionDisabled, sizeof(content::MediaplayerClosedcaptionDisabled));

    else if (0 == strcmp("mediaplayerCastOn", name))
        return blink::WebData((const char*)content::MediaplayerCastOn, sizeof(content::MediaplayerCastOn));
    else if (0 == strcmp("mediaplayerCastOff", name))
        return blink::WebData((const char*)content::MediaplayerCastOff, sizeof(content::MediaplayerCastOff));
    else if (0 == strcmp("mediaplayerOverlayCastOff", name))
        return blink::WebData((const char*)content::MediaplayerOverlayCastOff, sizeof(content::MediaplayerOverlayCastOff));

    else if (0 == strcmp("mediaplayerSliderThumb", name))
        return blink::WebData((const char*)content::MediaplayerSliderThumb, sizeof(content::MediaplayerSliderThumb));
    else if (0 == strcmp("mediaplayerVolumeSliderThumb", name))
        return blink::WebData((const char*)content::MediaplayerVolumeSliderThumb, sizeof(content::MediaplayerVolumeSliderThumb));

    else if (0 == strcmp("themeInputMultipleFields.css", name))
        return blink::WebData((const char*)content::ThemeInputMultipleFieldsCss, sizeof(content::ThemeInputMultipleFieldsCss));
    else if (0 == strcmp("searchCancelPressed", name))
        return blink::WebData((const char*)content::searchCancelPressedPng, sizeof(content::searchCancelPressedPng));
    else if (0 == strcmp("searchCancel", name))
        return blink::WebData((const char*)content::searchCancelPng, sizeof(content::searchCancelPng));

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
#ifndef NO_USE_ORIG_CHROME
    if (OrigChromeMgr::getInst()) {
        if (!m_ccCompositorSupport)
            m_ccCompositorSupport = OrigChromeMgr::createWebCompositorSupport();
        return m_ccCompositorSupport;
    }
#endif
    if (!m_mcCompositorSupport)
        m_mcCompositorSupport = new mc_blink::WebCompositorSupportImpl();
    return m_mcCompositorSupport;
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
#ifndef MINIBLINK_NO_PAGE_LOCALSTORAGE
    RELEASE_ASSERT(false);
    return nullptr;
#else
    if (!m_localStorageStorageMap)
        m_localStorageStorageMap = new net::DOMStorageMap();
    return new blink::WebStorageNamespaceImpl("", blink::kLocalStorageNamespaceId, m_localStorageStorageMap, true);
#endif
}

// blink::WebStorageNamespace* BlinkPlatformImpl::createSessionStorageNamespace()
// {
//     if (!m_sessionStorageStorageMap)
//         m_sessionStorageStorageMap = new net::DOMStorageMap();
//     return new net::WebStorageNamespaceImpl("", m_storageNamespaceIdCount++, m_sessionStorageStorageMap, false);
// }

int64 BlinkPlatformImpl::genStorageNamespaceId()
{
    ++m_storageNamespaceIdCount;
    return m_storageNamespaceIdCount;
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

double BlinkPlatformImpl::audioHardwareSampleRate() { return 48000.0000; }
size_t BlinkPlatformImpl::audioHardwareBufferSize() { return 2048; }
unsigned BlinkPlatformImpl::audioHardwareOutputChannels() { return 2; }

blink::WebAudioDevice* BlinkPlatformImpl::createAudioDevice(size_t bufferSize, unsigned numberOfInputChannels, unsigned numberOfChannels, double sampleRate, blink::WebAudioDevice::RenderCallback* cb, const blink::WebString& deviceId)
{
#ifndef NO_USE_ORIG_CHROME
    if (OrigChromeMgr::getInst())
        return OrigChromeMgr::createAudioDevice(bufferSize, numberOfInputChannels, numberOfChannels, sampleRate, cb, deviceId);
#endif
    return nullptr;
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

    if (!wke::g_navigatorPluginsEnable)
        return;

    if (wke::g_getPluginListCallback && !wke::g_getPluginListCallback(refresh, builder, nullptr))
        return;
    
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

            // third_party\WebKit\Source\core\dom\DOMImplementation.cpp会询问
            // third_party\WebKit\Source\platform\plugins\PluginData.cpp 里得到的插件mime是否有支持的，有的话就创建PluginDocument
            if (desc.startsWith("application/virtual-plugin-")) {
                if (!package->load())
                    continue;

                NPError npErr = package->pluginFuncs()->newp((NPMIMEType)"application/pdf", nullptr, 0, 0, nullptr, nullptr, nullptr);
                if (NPERR_NO_ERROR != npErr)
                    continue;
                type = "application/pdf";
                desc = "pdfviewer";
            }

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

blink::WebString BlinkPlatformImpl::domCodeStringFromEnum(int domCode)
{
    switch (domCode) {
    case 0x8:
        return "Backspace";
        break;
    case 0x9:
        return "Tab";
        break;
    case 0xD:
        return "Enter";
        break;
    case 0x1b:
        return "Escape";
        break;
    case 0x26:
        return "ArrowUp";
        break;
    case 0x28:
        return "ArrowDown";
        break;
    case 0x25:
        return "ArrowLeft";
        break;
    case 0x27:
        return "ArrowRight";
        break;
    case 0x29:
        return "Enter";
        break;
    default:
        break;
    }
    return "";
}

blink::WebString BlinkPlatformImpl::domKeyStringFromEnum(int domKey)
{
    return domCodeStringFromEnum(domKey);
}

blink::WebGraphicsContext3D* BlinkPlatformImpl::createOffscreenGraphicsContext3D(const blink::WebGraphicsContext3D::Attributes& attr, blink::WebGraphicsContext3D* shareContext)
{
    return createOffscreenGraphicsContext3D(attr, shareContext, nullptr);
}

blink::WebGraphicsContext3D* BlinkPlatformImpl::createOffscreenGraphicsContext3D(const blink::WebGraphicsContext3D::Attributes& attr)
{
    return createOffscreenGraphicsContext3D(attr, nullptr, nullptr);
}

blink::WebGraphicsContext3D* BlinkPlatformImpl::createOffscreenGraphicsContext3D(
    const blink::WebGraphicsContext3D::Attributes& attr,
    blink::WebGraphicsContext3D* shareContext,
    blink::WebGLInfo* glInfo
    )
{
#ifndef NO_USE_ORIG_CHROME
    if (OrigChromeMgr::getInst())
        return OrigChromeMgr::createOffscreenGraphicsContext3D(attr, shareContext, glInfo);
#endif
    return nullptr;
}

blink::WebGraphicsContext3DProvider* BlinkPlatformImpl::createSharedOffscreenGraphicsContext3DProvider() 
{
    return nullptr;
}

bool BlinkPlatformImpl::canAccelerate2dCanvas() 
{
    return false;
}

bool BlinkPlatformImpl::isThreadedCompositingEnabled() 
{
    return false;
}

blink::WebFlingAnimator* BlinkPlatformImpl::createFlingAnimator() 
{
    return nullptr;
}

blink::WebGestureCurve* BlinkPlatformImpl::createFlingAnimationCurve(blink::WebGestureDevice deviceSource, const blink::WebFloatPoint& velocity, const blink::WebSize& cumulativeScroll)
{
    return nullptr;
}

} // namespace content

HMODULE g_hModule;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ulReasonForCall,
    LPVOID lpReserved
)
{
    g_hModule = hModule;

    switch (ulReasonForCall) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}