/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "public/web/WebKit.h"

#include "bindings/core/v8/ScriptStreamerThread.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8GCController.h"
#include "bindings/core/v8/V8Initializer.h"
#include "core/Init.h"
#include "core/animation/AnimationClock.h"
#include "core/dom/Microtask.h"
#include "core/frame/Settings.h"
#include "core/page/Page.h"
#include "core/workers/WorkerGlobalScopeProxy.h"
#include "gin/public/v8_platform.h"
#include "modules/InitModules.h"
#include "platform/LayoutTestSupport.h"
#include "platform/Logging.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/ImageDecodingStore.h"
#include "platform/graphics/media/MediaPlayer.h"
#include "platform/heap/Heap.h"
#include "platform/heap/glue/MessageLoopInterruptor.h"
#include "platform/heap/glue/PendingGCRunner.h"
#include "public/platform/Platform.h"
#include "public/platform/WebPrerenderingSupport.h"
#include "public/platform/WebThread.h"
#include "web/IndexedDBClientImpl.h"
#include "web/WebMediaPlayerClientImpl.h"
#include "wtf/Assertions.h"
#include "wtf/CryptographicallyRandomNumber.h"
#include "wtf/MainThread.h"
#include "wtf/Partitions.h"
#include "wtf/WTF.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/TextEncoding.h"
#include <v8.h>

namespace blink {

namespace {

class EndOfTaskRunner : public WebThread::TaskObserver {
public:
    void willProcessTask() override
    {
        AnimationClock::notifyTaskStart();
    }
    void didProcessTask() override
    {
        Microtask::performCheckpoint(mainThreadIsolate());
        V8GCController::reportDOMMemoryUsageToV8(mainThreadIsolate());
        V8Initializer::reportRejectedPromisesOnMainThread();
    }
};

class MainThreadTaskRunner: public WebThread::Task {
    WTF_MAKE_NONCOPYABLE(MainThreadTaskRunner);
public:
    MainThreadTaskRunner(WTF::MainThreadFunction* function, void* context)
        : m_function(function)
        , m_context(context) { }

    void run() override
    {
        m_function(m_context);
    }
private:
    WTF::MainThreadFunction* m_function;
    void* m_context;
};

} // namespace

static WebThread::TaskObserver* s_endOfTaskRunner = 0;
static WebThread::TaskObserver* s_pendingGCRunner = 0;
static ThreadState::Interruptor* s_messageLoopInterruptor = 0;
static ThreadState::Interruptor* s_isolateInterruptor = 0;

// Make sure we are not re-initialized in the same address space.
// Doing so may cause hard to reproduce crashes.
static bool s_webKitInitialized = false;

void initialize(Platform* platform)
{
    initializeWithoutV8(platform);

    V8Initializer::initializeMainThreadIfNeeded();

    s_isolateInterruptor = new V8IsolateInterruptor(V8PerIsolateData::mainThreadIsolate());
    ThreadState::current()->addInterruptor(s_isolateInterruptor);
    ThreadState::current()->registerTraceDOMWrappers(V8PerIsolateData::mainThreadIsolate(), V8GCController::traceDOMWrappers);

    // currentThread is null if we are running on a thread without a message loop.
    if (WebThread* currentThread = platform->currentThread()) {
        ASSERT(!s_endOfTaskRunner);
        s_endOfTaskRunner = new EndOfTaskRunner;
        currentThread->addTaskObserver(s_endOfTaskRunner);
    }
}

v8::Isolate* mainThreadIsolate()
{
    return V8PerIsolateData::mainThreadIsolate();
}

static double currentTimeFunction()
{
    return Platform::current()->currentTime();
}

static double monotonicallyIncreasingTimeFunction()
{
    return Platform::current()->monotonicallyIncreasingTime();
}

static double systemTraceTimeFunction()
{
    return Platform::current()->systemTraceTime();
}

static void histogramEnumerationFunction(const char* name, int sample, int boundaryValue)
{
    Platform::current()->histogramEnumeration(name, sample, boundaryValue);
}

static void cryptographicallyRandomValues(unsigned char* buffer, size_t length)
{
    Platform::current()->cryptographicallyRandomValues(buffer, length);
}

static void callOnMainThreadFunction(WTF::MainThreadFunction function, void* context)
{
    Platform::current()->mainThread()->postTask(FROM_HERE, new MainThreadTaskRunner(function, context));
}

static void mainThreadadjustAmountOfExternalAllocatedMemory(void* sizePtr)
{
    v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory((int)sizePtr);
}

static void adjustAmountOfExternalAllocatedMemory(int size)
{
    if (!v8::Isolate::GetCurrent()) {
        callOnMainThreadFunction(mainThreadadjustAmountOfExternalAllocatedMemory, (void*)size);
        return;
    }
    v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(size);
}

void blinkAdjustAmountOfExternalAllocatedMemory(int size)
{
    return adjustAmountOfExternalAllocatedMemory(size);
}

void initializeWithoutV8(Platform* platform)
{
    ASSERT(!s_webKitInitialized);
    s_webKitInitialized = true;

    ASSERT(platform);
    //Platform::initialize(platform); // weolar:因为Platform::initialize需要创建mainthread，所以等初始化好了main thread再调用，放到后面去。

    WTF::setRandomSource(cryptographicallyRandomValues);
    WTF::initialize(currentTimeFunction, monotonicallyIncreasingTimeFunction, systemTraceTimeFunction, histogramEnumerationFunction, adjustAmountOfExternalAllocatedMemory);
    WTF::initializeMainThread(callOnMainThreadFunction);
    Platform::initialize(platform);
    Heap::init();

    ThreadState::attachMainThread();
    // currentThread() is null if we are running on a thread without a message loop.
    if (WebThread* currentThread = platform->currentThread()) {
        ASSERT(!s_pendingGCRunner);
        s_pendingGCRunner = new PendingGCRunner;
        currentThread->addTaskObserver(s_pendingGCRunner);

        ASSERT(!s_messageLoopInterruptor);
        s_messageLoopInterruptor = new MessageLoopInterruptor(currentThread);
        ThreadState::current()->addInterruptor(s_messageLoopInterruptor);
    }

    DEFINE_STATIC_LOCAL(ModulesInitializer, initializer, ());
    initializer.init();
#ifdef MINIBLINK_NOT_IMPLEMENTED
    setIndexedDBClientCreateFunction(IndexedDBClientImpl::create);
#endif // MINIBLINK_NOT_IMPLEMENTED
    MediaPlayer::setMediaEngineCreateFunction(WebMediaPlayerClientImpl::create);
}

void shutdown()
{
    // currentThread() is null if we are running on a thread without a message loop.
    if (Platform::current()->currentThread()) {
        // We don't need to (cannot) remove s_endOfTaskRunner from the current
        // message loop, because the message loop is already destructed before
        // the shutdown() is called.
        delete s_endOfTaskRunner;
        s_endOfTaskRunner = 0;
    }

    ASSERT(s_isolateInterruptor);
    ThreadState::current()->removeInterruptor(s_isolateInterruptor);

    // currentThread() is null if we are running on a thread without a message loop.
    if (Platform::current()->currentThread()) {
        ASSERT(s_pendingGCRunner);
        delete s_pendingGCRunner;
        s_pendingGCRunner = 0;

        ASSERT(s_messageLoopInterruptor);
        ThreadState::current()->removeInterruptor(s_messageLoopInterruptor);
        delete s_messageLoopInterruptor;
        s_messageLoopInterruptor = 0;
    }

    // Shutdown V8-related background threads before V8 is ramped down. Note
    // that this will wait the thread to stop its operations.
    ScriptStreamerThread::shutdown();

    v8::Isolate* isolate = V8PerIsolateData::mainThreadIsolate();
    V8PerIsolateData::willBeDestroyed(isolate);

    // Make sure we stop WorkerThreads before the main thread's ThreadState
    // and later shutdown steps starts freeing up resources needed during
    // worker termination.
#ifdef MINIBLINK_NOT_IMPLEMENTED
    WorkerThread::terminateAndWaitForAllWorkers();
#endif // MINIBLINK_NOT_IMPLEMENTED
    ModulesInitializer::terminateThreads();

    // Detach the main thread before starting the shutdown sequence
    // so that the main thread won't get involved in a GC during the shutdown.
    ThreadState::detachMainThread();

    V8PerIsolateData::destroy(isolate);

    shutdownWithoutV8();
}

void shutdownWithoutV8()
{
    ASSERT(!s_endOfTaskRunner);
    CoreInitializer::shutdown();
    Heap::shutdown();
    WTF::shutdown();
    Platform::shutdown();
    WebPrerenderingSupport::shutdown();
}

void setLayoutTestMode(bool value)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    LayoutTestSupport::setIsRunningLayoutTest(value);
#endif // MINIBLINK_NOT_IMPLEMENTED
}

bool layoutTestMode()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    return LayoutTestSupport::isRunningLayoutTest();
#endif // MINIBLINK_NOT_IMPLEMENTED
    return false;
}

void setFontAntialiasingEnabledForTest(bool value)
{
    LayoutTestSupport::setFontAntialiasingEnabledForTest(value);
}

bool fontAntialiasingEnabledForTest()
{
    return LayoutTestSupport::isFontAntialiasingEnabledForTest();
}

void enableLogChannel(const char* name)
{
#if !LOG_DISABLED
    WTFLogChannel* channel = getChannelFromName(name);
    if (channel)
        channel->state = WTFLogChannelOn;
#endif // !LOG_DISABLED
}

void resetPluginCache(bool reloadPages)
{
    ASSERT(!reloadPages);
    Page::refreshPlugins();
}

void decommitFreeableMemory()
{
    WTF::Partitions::decommitFreeableMemory();
}

} // namespace blink
