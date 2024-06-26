// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/sounds/sounds_manager.h"

#include "base/compiler_specific.h"
#include "base/logging.h"
#include "base/memory/linked_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/single_thread_task_runner.h"
#include "media/audio/audio_manager.h"
#include "media/audio/sounds/audio_stream_handler.h"

namespace media {

namespace {

    SoundsManager* g_instance = NULL;
    bool g_initialized_for_testing = false;

    // SoundsManagerImpl ---------------------------------------------------

    class SoundsManagerImpl : public SoundsManager {
    public:
        SoundsManagerImpl();
        ~SoundsManagerImpl() override;

        // SoundsManager implementation:
        bool Initialize(SoundKey key, const base::StringPiece& data) override;
        bool Play(SoundKey key) override;
        bool Stop(SoundKey key) override;
        base::TimeDelta GetDuration(SoundKey key) override;

    private:
        linked_ptr<AudioStreamHandler> GetHandler(SoundKey key);

        base::hash_map<SoundKey, linked_ptr<AudioStreamHandler>> handlers_;
        scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

        DISALLOW_COPY_AND_ASSIGN(SoundsManagerImpl);
    };

    SoundsManagerImpl::SoundsManagerImpl()
        : task_runner_(AudioManager::Get()->GetTaskRunner())
    {
    }

    SoundsManagerImpl::~SoundsManagerImpl() { DCHECK(CalledOnValidThread()); }

    bool SoundsManagerImpl::Initialize(SoundKey key,
        const base::StringPiece& data)
    {
        linked_ptr<AudioStreamHandler> current_handler = GetHandler(key);
        if (current_handler.get() && current_handler->IsInitialized())
            return true;
        linked_ptr<AudioStreamHandler> new_handler(new AudioStreamHandler(data));
        if (!new_handler->IsInitialized()) {
            LOG(WARNING) << "Can't initialize AudioStreamHandler for key=" << key;
            return false;
        }
        handlers_[key] = new_handler;
        return true;
    }

    bool SoundsManagerImpl::Play(SoundKey key)
    {
        DCHECK(CalledOnValidThread());
        linked_ptr<AudioStreamHandler> handler = GetHandler(key);
        if (!handler.get())
            return false;
        if (!handler->IsInitialized())
            return false;
        return handler->Play();
    }

    bool SoundsManagerImpl::Stop(SoundKey key)
    {
        DCHECK(CalledOnValidThread());
        linked_ptr<AudioStreamHandler> handler = GetHandler(key);
        if (!handler.get())
            return false;
        if (!handler->IsInitialized())
            return false;
        handler->Stop();
        return true;
    }

    base::TimeDelta SoundsManagerImpl::GetDuration(SoundKey key)
    {
        DCHECK(CalledOnValidThread());
        linked_ptr<AudioStreamHandler> handler = GetHandler(key);
        if (!handler.get())
            return base::TimeDelta();
        if (!handler->IsInitialized())
            return base::TimeDelta();
        const WavAudioHandler& wav_audio = handler->wav_audio_handler();
        return wav_audio.GetDuration();
    }

    linked_ptr<AudioStreamHandler> SoundsManagerImpl::GetHandler(SoundKey key)
    {
        auto key_handler_pair_iter = handlers_.find(key);
        return key_handler_pair_iter == handlers_.end() ? linked_ptr<AudioStreamHandler>() : key_handler_pair_iter->second;
    }

} // namespace

SoundsManager::SoundsManager() { }

SoundsManager::~SoundsManager() { DCHECK(CalledOnValidThread()); }

// static
void SoundsManager::Create()
{
    CHECK(!g_instance || g_initialized_for_testing)
        << "SoundsManager::Create() is called twice";
    if (g_initialized_for_testing)
        return;
    g_instance = new SoundsManagerImpl();
}

// static
void SoundsManager::Shutdown()
{
    CHECK(g_instance) << "SoundsManager::Shutdown() is called "
                      << "without previous call to Create()";
    delete g_instance;
    g_instance = NULL;
}

// static
SoundsManager* SoundsManager::Get()
{
    CHECK(g_instance) << "SoundsManager::Get() is called before Create()";
    return g_instance;
}

// static
void SoundsManager::InitializeForTesting(SoundsManager* manager)
{
    CHECK(!g_instance) << "SoundsManager is already initialized.";
    CHECK(manager);
    g_instance = manager;
    g_initialized_for_testing = true;
}

} // namespace media
