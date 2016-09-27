/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(WEB_AUDIO)

#include "platform/audio/HRTFDatabaseLoader.h"

#include "platform/Task.h"
#include "platform/TaskSynchronizer.h"
#include "platform/ThreadSafeFunctional.h"
#include "public/platform/Platform.h"
#include "public/platform/WebTraceLocation.h"
#include "wtf/MainThread.h"

namespace blink {

using LoaderMap = HashMap<double, HRTFDatabaseLoader*>;

static LoaderMap& loaderMap()
{
    DEFINE_STATIC_LOCAL(LoaderMap*, map, (new LoaderMap));
    return *map;
}

PassRefPtr<HRTFDatabaseLoader> HRTFDatabaseLoader::createAndLoadAsynchronouslyIfNecessary(float sampleRate)
{
    ASSERT(isMainThread());

    RefPtr<HRTFDatabaseLoader> loader = loaderMap().get(sampleRate);
    if (loader) {
        ASSERT(sampleRate == loader->databaseSampleRate());
        return loader.release();
    }

    loader = adoptRef(new HRTFDatabaseLoader(sampleRate));
    loaderMap().add(sampleRate, loader.get());
    loader->loadAsynchronously();
    return loader.release();
}

HRTFDatabaseLoader::HRTFDatabaseLoader(float sampleRate)
    : m_databaseSampleRate(sampleRate)
{
    ASSERT(isMainThread());
}

HRTFDatabaseLoader::~HRTFDatabaseLoader()
{
    ASSERT(isMainThread());
    ASSERT(!m_thread);
    loaderMap().remove(m_databaseSampleRate);
}

void HRTFDatabaseLoader::loadTask()
{
    ASSERT(!isMainThread());

    {
        MutexLocker locker(m_lock);
        if (!m_hrtfDatabase) {
            // Load the default HRTF database.
            m_hrtfDatabase = HRTFDatabase::create(m_databaseSampleRate);
        }
    }
}

void HRTFDatabaseLoader::loadAsynchronously()
{
    ASSERT(isMainThread());

    MutexLocker locker(m_lock);
    if (!m_hrtfDatabase && !m_thread) {
        // Start the asynchronous database loading process.
        m_thread = adoptPtr(Platform::current()->createThread("HRTF database loader"));
        m_thread->postTask(FROM_HERE, new Task(threadSafeBind(&HRTFDatabaseLoader::loadTask, AllowCrossThreadAccess(this))));
    }
}

bool HRTFDatabaseLoader::isLoaded()
{
    MutexLocker locker(m_lock);
    return m_hrtfDatabase;
}

// This cleanup task is needed just to make sure that the loader thread finishes
// the load task and thus the loader thread doesn't touch m_thread any more.
void HRTFDatabaseLoader::cleanupTask(TaskSynchronizer* sync)
{
    sync->taskCompleted();
}

void HRTFDatabaseLoader::waitForLoaderThreadCompletion()
{
    if (!m_thread)
        return;

    TaskSynchronizer sync;
    m_thread->postTask(FROM_HERE, new Task(threadSafeBind(&HRTFDatabaseLoader::cleanupTask, AllowCrossThreadAccess(this), AllowCrossThreadAccess(&sync))));
    sync.waitForTaskCompletion();
    m_thread.clear();
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
