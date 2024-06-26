
#include "content/OrigChromeMgr.h"

#include "cc/blink/web_compositor_support_impl.h"
#include "cc/blink/web_external_bitmap_impl.h"
#include "content/RasterWorkerPool.h"
#include "content/WebSharedBitmapManager.h"
#include "content/gpu/ChildGpuMemoryBufferManager.h"
#include "content/media/audio_renderer_mixer_manager.h"
#include "content/media/renderer_webaudiodevice_impl.h"
#include "gpu/blink/webgraphicscontext3d_in_process_command_buffer_impl.h"
#include "media/audio/audio_manager.h"
#include "media/audio/audio_manager_base.h"
#include "media/audio/fake_audio_log_factory.h"
#include "media/base/channel_layout.h"
#include "media/base/audio_hardware_config.h"
#include "media/base/media_log.h"
#include "media/base/media_permission.h"
#include "media/blink/webmediaplayer_impl.h"
#include "media/blink/webmediaplayer_params.h"
#include "media/renderers/default_renderer_factory.h"
#include "third_party/WebKit/public/platform/WebContentDecryptionModule.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebSecurityOrigin.h"
#include "ui/gl/gl_surface.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/at_exit.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/message_loop/message_loop.h"
#include <Shlwapi.h>

#ifdef _WIN64
# pragma comment(lib, "G:/mycode/mb/out/x64Release/plugins/ffmpeg/ffmpeg_x64.dll.lib")
#else
# if _DEBUG
#  pragma comment(lib, "M:\\chromium\\chromium.bb\\src\\out\\Debug\\ffmpeg.dll.lib")
# else
#  pragma comment(lib, "M:\\chromium\\chromium.bb\\src\\out\\release\\ffmpeg.dll.lib")
# endif
#endif

extern HMODULE g_hModule;

namespace content {

OrigChromeMgr* OrigChromeMgr::m_inst = nullptr;

base::AtExitManager* g_exitManager = nullptr;

OrigChromeMgr::OrigChromeMgr()
{
    m_uiLoop = nullptr;
    m_blinkLoop = nullptr;
    m_rasterWorkerPool = nullptr;
    m_mediaThread = nullptr;
    m_mediaIoThread = nullptr;
    m_compositorThread = nullptr;
    m_sharedBitmapManager = nullptr;
    m_childGpuMemoryBufferManager = nullptr;
    m_audioRendererMixerManager = nullptr;
    m_hFfmpeg = nullptr;
    m_glImplType = kGLImplTypeNone;
    m_webglReady = false;
}

void OrigChromeMgr::init()
{
    if (m_inst)
        return;

    g_exitManager = new base::AtExitManager();
    m_inst = new OrigChromeMgr();

    base::CommandLine::Init(0, nullptr);
}

void OrigChromeMgr::setGLImplType(GLImplType type)
{
    m_glImplType = type;
}

OrigChromeMgr* OrigChromeMgr::getInst()
{
    return m_inst;
}

void OrigChromeMgr::shutdown()
{
    delete g_exitManager;
}

void OrigChromeMgr::runUntilIdleWithoutMsgPeek()
{
    if (!m_inst)
        return;
    base::MessageLoop::current()->RunUntilIdleWithoutMsgPeek();
}

void OrigChromeMgr::runUntilIdle()
{
    if (!m_inst)
        return;
    base::MessageLoop::current()->RunUntilIdle();
}

static int s_blinkThreadRunnerCount = 0;
static int s_uiThreadRunnerCount = 0;

static void blinkRunner(OrigTaskType task)
{
    InterlockedDecrement((LONG*)&s_blinkThreadRunnerCount);
    task();
}

static void uiRunner(OrigTaskType task)
{
    InterlockedDecrement((LONG*)&s_uiThreadRunnerCount);
    task();
}

void OrigChromeMgr::postBlinkTask(OrigTaskType task)
{
    if (!m_inst || s_blinkThreadRunnerCount > 0)
        return;

    InterlockedIncrement((LONG*)&s_blinkThreadRunnerCount);
    m_inst->m_blinkLoop->PostTask(FROM_HERE, base::Bind(&blinkRunner, task));
}

void OrigChromeMgr::postUiTask(OrigTaskType task)
{
    if (!m_inst || s_uiThreadRunnerCount > 0)
        return;
    InterlockedIncrement((LONG*)&s_uiThreadRunnerCount);
    m_inst->m_uiLoop->PostTask(FROM_HERE, base::Bind(&uiRunner, task));
}

static void webRunner(blink::WebThread::Task* task)
{
    task->run();
    delete task;
}

void OrigChromeMgr::postWebTask(const blink::WebTraceLocation& from, blink::WebThread::Task* task)
{
    base::MessageLoop* messageLoop = m_inst->m_blinkLoop;
    tracked_objects::Location location(from.functionName(), from.fileName(), -1, nullptr);

    messageLoop->PostTask(location, base::Bind(&webRunner, task));
}

void OrigChromeMgr::postWebDelayedTask(const blink::WebTraceLocation& from, blink::WebThread::Task* task, long long delayMs)
{
    base::MessageLoop* messageLoop = m_inst->m_blinkLoop;
    tracked_objects::Location location(from.functionName(), from.fileName(), -1, nullptr);

    messageLoop->PostDelayedTask(location, base::Bind(&webRunner, task), base::TimeDelta::FromMilliseconds(delayMs));
}

class TaskObserverAdapter : public base::MessageLoop::TaskObserver {
public:
    TaskObserverAdapter(blink::WebThread::TaskObserver* observer)
        : m_observer(observer)
    {
    }

    void WillProcessTask(const base::PendingTask& pending_task) override
    {
        m_observer->willProcessTask();
    }

    void DidProcessTask(const base::PendingTask& pending_task) override
    {
        m_observer->didProcessTask();
    }

private:
    blink::WebThread::TaskObserver* m_observer;
};

void OrigChromeMgr::addTaskObserver(blink::WebThread::TaskObserver* observer)
{
    if (!m_inst->m_blinkLoop) {
        m_inst->m_blinkLoop = new base::MessageLoop(base::MessageLoop::TYPE_UI);

        const char* kThreadName = "MbBlinkThread";
        m_inst->m_blinkLoop->set_thread_name(kThreadName);
    }
    base::MessageLoop* messageLoop = m_inst->m_blinkLoop;

    std::pair<TaskObserverMap::iterator, bool> result = m_inst->m_taskObserverMap.insert(std::make_pair(observer, nullptr));
    if (result.second)
        result.first->second = new TaskObserverAdapter(observer);

    messageLoop->AddTaskObserver(result.first->second);
}

void OrigChromeMgr::removeTaskObserver(blink::WebThread::TaskObserver* observer)
{
    base::MessageLoop* messageLoop = m_inst->m_blinkLoop;
    TaskObserverMap::iterator iter = m_inst->m_taskObserverMap.find(observer);
    if (iter == m_inst->m_taskObserverMap.end())
        return;
    messageLoop->RemoveTaskObserver(iter->second);
    delete iter->second;
    m_inst->m_taskObserverMap.erase(iter);
}

void OrigChromeMgr::onCreateWebview()
{
    ;
}

cc::TaskGraphRunner* OrigChromeMgr::getTaskGraphRunner()
{
    return m_rasterWorkerPool->GetTaskGraphRunner();
}

void OrigChromeMgr::initUiThread()
{
    m_uiLoop = new base::MessageLoop(base::MessageLoop::TYPE_UI);

    const char* kThreadName = "MbUiThread";
    m_uiLoop->set_thread_name(kThreadName);
}

static scoped_ptr<cc::SharedBitmap> SharedBitmapAllocation(const gfx::Size& size)
{
    return OrigChromeMgr::getInst()->getSharedBitmapManager()->AllocateSharedBitmap(size);
}

void OrigChromeMgr::initBlinkThread()
{
    m_rasterWorkerPool = new RasterWorkerPool();
    m_rasterWorkerPool->Start(1, base::SimpleThread::Options());

    if (kGLImplTypeEGLGLES2 == m_glImplType) {
        if (!gfx::GLSurface::InitializeForce(gfx::kGLImplementationEGLGLES2)) {
            OutputDebugStringA("gfx::GLSurface::Initialize EGLGLES2 failed\n");
            m_glImplType = kGLImplTypeNone;
        } else
            m_webglReady = true;
    } else if (kGLImplTypeEgles2Swiftshader == m_glImplType || kGLImplTypeNone == m_glImplType) {
        if (!gfx::GLSurface::InitializeForce(gfx::kGLImplementationEGLGLES2SwiftShader)) {
            OutputDebugStringA("gfx::GLSurface::Initialize SwiftShader failed\n");
            m_glImplType = kGLImplTypeNone;
        } else
            m_webglReady = true;
    }

    m_sharedBitmapManager = new WebSharedBitmapManager();
    m_childGpuMemoryBufferManager = nullptr;

    if (m_webglReady)
        cc_blink::SetSharedBitmapAllocationFunction(&SharedBitmapAllocation);

    if (kGLImplTypeEGLGLES2 == m_glImplType || kGLImplTypeEgles2Swiftshader == m_glImplType) {
        m_childGpuMemoryBufferManager = new ChildGpuMemoryBufferManager();
    }
}

base::Thread* OrigChromeMgr::getOrCreateCompositorThread()
{
    if (!m_compositorThread) {
        m_compositorThread = (new base::Thread("CompositorThread"));
        m_compositorThread->Start();
    }
    return m_compositorThread;
}

void OrigChromeMgr::createMediaThreadIfNeeded()
{
    //DCHECK(message_loop() == base::MessageLoop::current());
    if (m_mediaThread)
        return;

    std::vector<WCHAR> fullpath;
    fullpath.resize(MAX_PATH + 1);
    memset(fullpath.data(), 0, sizeof(wchar_t) * (MAX_PATH + 1));
    ::GetModuleFileNameW(g_hModule, fullpath.data(), MAX_PATH);
    ::PathRemoveFileSpecW(fullpath.data());

    std::wstring name = fullpath.data();
#ifdef _WIN64
    name += L"\\plugins\\ffmpeg\\ffmpeg_x64.dll";
#else
    name += L"\\plugins\\ffmpeg\\ffmpeg.dll";
#endif
    m_hFfmpeg = LoadLibraryW(name.c_str());
    if (!m_hFfmpeg)
        return;

    m_mediaThread = (new base::Thread("MediaThread"));
    m_mediaThread->Start();

    m_mediaIoThread = (new base::Thread("MediaIoThread"));
    m_mediaIoThread->Start();

    m_audioManager = (media::AudioManager::CreateWithHangTimer(new media::FakeAudioLogFactory(), /*io_thread_->task_runner()*/ m_mediaThread->task_runner()));

    media::AudioParameters outputParams = m_audioManager->GetDefaultOutputStreamParameters();
    media::AudioParameters inputParams = m_audioManager->GetInputStreamParameters(media::AudioManagerBase::kDefaultDeviceId);
    m_audioHardwareConfig = new media::AudioHardwareConfig(inputParams, outputParams);
}

blink::WebCompositorSupport* OrigChromeMgr::createWebCompositorSupport()
{
    return new cc_blink::WebCompositorSupportImpl();
}

class MediaPermissionDispatcherImpl : public media::MediaPermission {
public:
    explicit MediaPermissionDispatcherImpl() {}
    ~MediaPermissionDispatcherImpl() override {}

    // media::MediaPermission implementation.
    void HasPermission(media::MediaPermission::Type type, const blink::WebURL& security_origin, const media::MediaPermission::PermissionStatusCB& permission_status_cb) override
    {
    }

    // MediaStreamDevicePermissionContext doesn't support RequestPermission yet
    // and will always return CONTENT_SETTING_BLOCK.
    void RequestPermission(media::MediaPermission::Type type, const blink::WebURL& security_origin, const media::MediaPermission::PermissionStatusCB& permission_status_cb) override
    {
    }
};

MediaPermissionDispatcherImpl* OrigChromeMgr::getMediaPermission()
{
    if (!m_mediaPermissionDispatcherImpl)
        m_mediaPermissionDispatcherImpl = new MediaPermissionDispatcherImpl();
    return m_mediaPermissionDispatcherImpl;
}

int64_t adjustAmountOfExternalAllocatedMemory(int64_t)
{
    return 0;
}

AudioRendererMixerManager* OrigChromeMgr::getAudioRendererMixerManager()
{
    if (!m_audioRendererMixerManager)
        m_audioRendererMixerManager = (new AudioRendererMixerManager());
    return m_audioRendererMixerManager;
}

blink::WebMediaPlayer* OrigChromeMgr::createWebMediaPlayer(blink::WebLocalFrame* frame, const blink::WebURL& url, blink::WebMediaPlayerClient* client)
{
    if (!m_inst)
        return nullptr;
    AudioRendererMixerManager* audioRendererMixerManager = m_inst->getAudioRendererMixerManager();
    std::string origin = frame->securityOrigin().toString().utf8();
    media::RestartableAudioRendererSink* sink = reinterpret_cast<media::RestartableAudioRendererSink*>(audioRendererMixerManager->CreateInput(0, "", origin));

    scoped_refptr<media::RestartableAudioRendererSink> audio_renderer_sink(sink);
    media::WebMediaPlayerParams::Context3DCB context_3d_cb;

    m_inst->createMediaThreadIfNeeded();
    if (!m_inst->m_hFfmpeg)
        return nullptr;

    scoped_refptr<base::SingleThreadTaskRunner> mediaThreadTaskRunner = m_inst->m_mediaThread->task_runner();
    scoped_refptr<base::SingleThreadTaskRunner> compositorTaskRunner = m_inst->m_uiLoop->task_runner();

    blink::WebContentDecryptionModule* initialCdm = nullptr;
    scoped_refptr<media::MediaLog> media_log(new media::MediaLog());

    m_inst->m_rasterWorkerPool->AddRef(); // WebMediaPlayerImpl::~WebMediaPlayerImpl will release

    media::WebMediaPlayerParams params(
        media::WebMediaPlayerParams::DeferLoadCB(),
        audio_renderer_sink, media_log, mediaThreadTaskRunner,
        m_inst->m_rasterWorkerPool,
        compositorTaskRunner, context_3d_cb,
        base::Bind(&adjustAmountOfExternalAllocatedMemory), // base::Bind(&v8::Isolate::AdjustAmountOfExternalAllocatedMemory, base::Unretained(blink::mainThreadIsolate())),
        m_inst->getMediaPermission(), initialCdm);

    scoped_ptr<media::RendererFactory> media_renderer_factory;
    media_renderer_factory.reset(new media::DefaultRendererFactory(media_log, /*render_thread->GetGpuFactories()*/ nullptr, *(m_inst->m_audioHardwareConfig)));

    return new media::WebMediaPlayerImpl(frame, client, nullptr, base::WeakPtr<media::WebMediaPlayerDelegate>(), media_renderer_factory.Pass(), nullptr, params);
}

blink::WebGraphicsContext3D* OrigChromeMgr::createOffscreenGraphicsContext3D(
    const blink::WebGraphicsContext3D::Attributes& attr,
    blink::WebGraphicsContext3D* shareCxt,
    blink::WebGLInfo* glInfo)
{
    if (!m_inst->m_webglReady)
        return nullptr;

    bool loseContextWhenOutOfMemory = false;
    scoped_ptr<gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl> commandBuffer;
    commandBuffer = gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl::CreateOffscreenContext(attr, loseContextWhenOutOfMemory);
    commandBuffer->InitializeOnCurrentThread();

    return commandBuffer.release();
}

blink::WebAudioDevice* OrigChromeMgr::createAudioDevice(
    size_t bufferSize, 
    unsigned numberOfInputChannels, 
    unsigned numberOfChannels, 
    double sampleRate, 
    blink::WebAudioDevice::RenderCallback* callback, 
    const blink::WebString& deviceId)
{
    m_inst->createMediaThreadIfNeeded();
    if (!m_inst->m_hFfmpeg)
        return nullptr;

    // The |channels| does not exactly identify the channel layout of the
    // device. The switch statement below assigns a best guess to the channel
    // layout based on number of channels.
    media::ChannelLayout layout = media::CHANNEL_LAYOUT_UNSUPPORTED;
    switch (numberOfChannels) {
    case 1:
        layout = media::CHANNEL_LAYOUT_MONO;
        break;
    case 2:
        layout = media::CHANNEL_LAYOUT_STEREO;
        break;
    case 3:
        layout = media::CHANNEL_LAYOUT_2_1;
        break;
    case 4:
        layout = media::CHANNEL_LAYOUT_4_0;
        break;
    case 5:
        layout = media::CHANNEL_LAYOUT_5_0;
        break;
    case 6:
        layout = media::CHANNEL_LAYOUT_5_1;
        break;
    case 7:
        layout = media::CHANNEL_LAYOUT_7_0;
        break;
    case 8:
        layout = media::CHANNEL_LAYOUT_7_1;
        break;
    default:
        // If the layout is not supported (more than 9 channels), falls back to
        // discrete mode.
        layout = media::CHANNEL_LAYOUT_DISCRETE;
    }

    int sessionId = 0;
    if (deviceId.isNull() || !base::StringToInt(base::UTF16ToUTF8(base::StringPiece16(deviceId)), &sessionId)) {
        if (numberOfInputChannels > 0)
            DLOG(WARNING) << "createAudioDevice(): request for audio input ignored";

        numberOfInputChannels = 0;
    }

    // For CHANNEL_LAYOUT_DISCRETE, pass the explicit channel count along with
    // the channel layout when creating an |AudioParameters| object.
    media::AudioParameters params(media::AudioParameters::AUDIO_PCM_LOW_LATENCY, layout, static_cast<int>(sampleRate), 16, bufferSize);
    params.set_channels_for_discrete(numberOfChannels);

    return new RendererWebAudioDeviceImpl(params, callback, sessionId);
}

}