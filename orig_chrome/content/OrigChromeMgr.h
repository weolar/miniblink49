
#ifndef content_browser_OrigChromeMgr_h
#define content_browser_OrigChromeMgr_h

#include "third_party/WebKit/public/platform/WebGraphicsContext3D.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/WebAudioDevice.h"
#include <map>

namespace base {
class MessageLoop;
class Thread;
}

namespace cc {
class TaskGraphRunner;
}

namespace blink {
class WebCompositorSupport;
class WebTraceLocation;
class WebLocalFrame;
class WebMediaPlayer;
class WebMediaPlayerClient;
class WebURL;
}

namespace media {
class AudioManager;
class AudioHardwareConfig;
}

typedef void(__stdcall* OrigTaskType)();

namespace content {

class RasterWorkerPool;
class WebSharedBitmapManager;
class ChildGpuMemoryBufferManager;
class TaskObserverAdapter;
class MediaPermissionDispatcherImpl;
class AudioRendererMixerManager;

enum GLImplType {
    kGLImplTypeNone,
    kGLImplTypeDesktopGL,
    kGLImplTypeDesktopGLCoreProfile,
    kGLImplTypeOSMesaGL,
    kGLImplTypeAppleGL,
    kGLImplTypeEGLGLES2,
    kGLImplTypeEgles2Swiftshader,
    kGLImplTypeMockGL
};

class OrigChromeMgr {
public:
    static void init();
    static OrigChromeMgr* getInst();
    static void shutdown();
    void initUiThread();
    void initBlinkThread();

    void setGLImplType(GLImplType type);

    bool uesSoftwareOutput();

    static blink::WebGraphicsContext3D* createOffscreenGraphicsContext3D(
        const blink::WebGraphicsContext3D::Attributes& attr,
        blink::WebGraphicsContext3D* shareContext,
        blink::WebGLInfo* glInfo);

    static blink::WebAudioDevice* createAudioDevice(size_t bufferSize, unsigned numberOfInputChannels, unsigned numberOfChannels, double sampleRate, blink::WebAudioDevice::RenderCallback*, const blink::WebString& deviceId);

    static blink::WebCompositorSupport* createWebCompositorSupport();
    static blink::WebMediaPlayer* createWebMediaPlayer(blink::WebLocalFrame* frame, const blink::WebURL& url, blink::WebMediaPlayerClient* client);

    static void runUntilIdle();
    static void runUntilIdleWithoutMsgPeek();
    static void postBlinkTask(OrigTaskType task);
    static void postUiTask(OrigTaskType task);

    static void postWebTask(const blink::WebTraceLocation&, blink::WebThread::Task*);
    static void postWebDelayedTask(const blink::WebTraceLocation&, blink::WebThread::Task*, long long delayMs);
    static void addTaskObserver(blink::WebThread::TaskObserver* observer);
    static void removeTaskObserver(blink::WebThread::TaskObserver* observer);

    void onCreateWebview();

    cc::TaskGraphRunner* getTaskGraphRunner();

    base::MessageLoop* getUiLoop() { return m_uiLoop; }
    base::MessageLoop* getBlinkLoop() { return m_blinkLoop; }

    GLImplType getGlImplType() const { return m_glImplType; }

    WebSharedBitmapManager* getSharedBitmapManager() { return m_sharedBitmapManager; }
    ChildGpuMemoryBufferManager* getChildGpuMemoryBufferManager() { return m_childGpuMemoryBufferManager; }

    MediaPermissionDispatcherImpl* getMediaPermission();

    base::Thread* getMediaThread() const { return m_mediaThread; }

    base::Thread* getMediaIoThread() const { return m_mediaIoThread; }

    media::AudioManager* getAudioManager() const { return m_audioManager; }

    base::Thread* getOrCreateCompositorThread();

private:
    OrigChromeMgr();

    void createMediaThreadIfNeeded();
    AudioRendererMixerManager* getAudioRendererMixerManager();

    static OrigChromeMgr* m_inst;
    base::MessageLoop* m_uiLoop;
    base::MessageLoop* m_blinkLoop;
    RasterWorkerPool* m_rasterWorkerPool;

    WebSharedBitmapManager* m_sharedBitmapManager;
    ChildGpuMemoryBufferManager* m_childGpuMemoryBufferManager;

    GLImplType m_glImplType;
    bool m_webglReady; // 至少有一个gl的dll准备好了，可以实现webgl

    typedef std::map<blink::WebThread::TaskObserver*, TaskObserverAdapter*> TaskObserverMap;
    TaskObserverMap m_taskObserverMap;

    MediaPermissionDispatcherImpl* m_mediaPermissionDispatcherImpl;
    media::AudioManager* m_audioManager;
    media::AudioHardwareConfig* m_audioHardwareConfig;
    AudioRendererMixerManager* m_audioRendererMixerManager;
    void* m_hFfmpeg;

    base::Thread* m_mediaThread;
    base::Thread* m_mediaIoThread;
    base::Thread* m_compositorThread;
};

}

#endif // content_browser_OrigChromeMgr_h