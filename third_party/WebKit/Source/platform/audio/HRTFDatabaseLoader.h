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

#ifndef HRTFDatabaseLoader_h
#define HRTFDatabaseLoader_h

#include "platform/audio/HRTFDatabase.h"
#include "public/platform/WebThread.h"
#include "wtf/HashMap.h"
#include "wtf/RefCounted.h"
#include "wtf/ThreadingPrimitives.h"

namespace blink {

class TaskSynchronizer;

// HRTFDatabaseLoader will asynchronously load the default HRTFDatabase in a new thread.

class PLATFORM_EXPORT HRTFDatabaseLoader final : public RefCounted<HRTFDatabaseLoader> {
public:
    // Lazily creates a HRTFDatabaseLoader (if not already created) for the given sample-rate
    // and starts loading asynchronously (when created the first time).
    // Returns the HRTFDatabaseLoader.
    // Must be called from the main thread.
    static PassRefPtr<HRTFDatabaseLoader> createAndLoadAsynchronouslyIfNecessary(float sampleRate);

    // Both constructor and destructor must be called from the main thread.
    ~HRTFDatabaseLoader();

    // Returns true once the default database has been completely loaded.
    bool isLoaded();

    // waitForLoaderThreadCompletion() may be called more than once and is thread-safe.
    void waitForLoaderThreadCompletion();

    HRTFDatabase* database() { return m_hrtfDatabase.get(); }

    float databaseSampleRate() const { return m_databaseSampleRate; }

private:
    // Both constructor and destructor must be called from the main thread.
    explicit HRTFDatabaseLoader(float sampleRate);

    // If it hasn't already been loaded, creates a new thread and initiates asynchronous loading of the default database.
    // This must be called from the main thread.
    void loadAsynchronously();

    // Called in asynchronous loading thread.
    void loadTask();
    void cleanupTask(TaskSynchronizer*);

    // Holding a m_lock is required when accessing m_hrtfDatabase since we access it from multiple threads.
    Mutex m_lock;
    OwnPtr<HRTFDatabase> m_hrtfDatabase;

    OwnPtr<WebThread> m_thread;

    float m_databaseSampleRate;
};

} // namespace blink

#endif // HRTFDatabaseLoader_h
