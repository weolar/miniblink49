// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_manager.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/command_line.h"
#include "base/debug/alias.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/power_monitor/power_monitor.h"
#include "build/build_config.h"
#include "media/audio/audio_manager_factory.h"
#include "media/audio/fake_audio_log_factory.h"
#include "media/base/media_resources.h"
#include "media/base/media_switches.h"

#if defined(OS_WIN)
#include "base/win/scoped_com_initializer.h"
#endif

namespace media {
namespace {

    // The singleton instance of AudioManager. This is set when Create() is called.
    AudioManager* g_last_created = nullptr;

    // The singleton instance of AudioManagerFactory. This is only set if
    // SetFactory() is called. If it is set when Create() is called, its
    // CreateInstance() function is used to set |g_last_created|. Otherwise, the
    // linked implementation of media::CreateAudioManager is used to set
    // |g_last_created|.
    AudioManagerFactory* g_audio_manager_factory = nullptr;

    // Maximum number of failed pings to the audio thread allowed. A crash will be
    // issued once this count is reached.  We require at least two pings before
    // crashing to ensure unobservable power events aren't mistakenly caught (e.g.,
    // the system suspends before a OnSuspend() event can be fired.).
    const int kMaxHangFailureCount = 2;

    // Helper class for managing global AudioManager data and hang timers. If the
    // audio thread is unresponsive for more than two minutes we want to crash the
    // process so we can catch offenders quickly in the field.
    class AudioManagerHelper : public base::PowerObserver {
    public:
        AudioManagerHelper()
            : max_hung_task_time_(base::TimeDelta::FromMinutes(1))
            , hang_detection_enabled_(true)
            , io_task_running_(false)
            , audio_task_running_(false)
        {
        }
        ~AudioManagerHelper() override { }

        void StartHangTimer(
            const scoped_refptr<base::SingleThreadTaskRunner>& monitor_task_runner)
        {
            //             CHECK(!monitor_task_runner_);
            //             monitor_task_runner_ = monitor_task_runner;
            //             base::PowerMonitor::Get()->AddObserver(this);
            //             hang_failures_ = 0;
            //             io_task_running_ = audio_task_running_ = true;
            //             UpdateLastAudioThreadTimeTick();
            //             CrashOnAudioThreadHang();
            DebugBreak();
        }

        // Disable hang detection when the system goes into the suspend state.
        void OnSuspend() override
        {
            base::AutoLock lock(hang_lock_);
            hang_detection_enabled_ = false;
            hang_failures_ = 0;
        }

        // Reenable hang detection once the system comes out of the suspend state.
        void OnResume() override
        {
            base::AutoLock lock(hang_lock_);
            hang_detection_enabled_ = true;
            last_audio_thread_timer_tick_ = base::TimeTicks::Now();
            hang_failures_ = 0;

            // If either of the tasks were stopped during suspend, start them now.
            if (!audio_task_running_) {
                audio_task_running_ = true;

                base::AutoUnlock unlock(hang_lock_);
                UpdateLastAudioThreadTimeTick();
            }

            if (!io_task_running_) {
                io_task_running_ = true;

                base::AutoUnlock unlock(hang_lock_);
                CrashOnAudioThreadHang();
            }
        }

        // Runs on |monitor_task_runner| typically, but may be started on any thread.
        void CrashOnAudioThreadHang()
        {
            {
                base::AutoLock lock(hang_lock_);

                // Don't attempt to verify the tick time or post our task if the system is
                // in the process of suspending or resuming.
                if (!hang_detection_enabled_) {
                    io_task_running_ = false;
                    return;
                }

                DCHECK(io_task_running_);
                const base::TimeTicks now = base::TimeTicks::Now();
                const base::TimeDelta tick_delta = now - last_audio_thread_timer_tick_;
                if (tick_delta > max_hung_task_time_) {
                    CHECK_LT(++hang_failures_, kMaxHangFailureCount);
                } else {
                    hang_failures_ = 0;
                }
            }

            // Don't hold the lock while posting the next task.
            monitor_task_runner_->PostDelayedTask(
                FROM_HERE, base::Bind(&AudioManagerHelper::CrashOnAudioThreadHang, base::Unretained(this)),
                max_hung_task_time_);
        }

        // Runs on the audio thread typically, but may be started on any thread.
        void UpdateLastAudioThreadTimeTick()
        {
            {
                base::AutoLock lock(hang_lock_);
                last_audio_thread_timer_tick_ = base::TimeTicks::Now();
                hang_failures_ = 0;

                // Don't post our task if the system is or will be suspended.
                if (!hang_detection_enabled_) {
                    audio_task_running_ = false;
                    return;
                }

                DCHECK(audio_task_running_);
            }

            // Don't hold the lock while posting the next task.
            g_last_created->GetTaskRunner()->PostDelayedTask(
                FROM_HERE,
                base::Bind(&AudioManagerHelper::UpdateLastAudioThreadTimeTick,
                    base::Unretained(this)),
                max_hung_task_time_ / 5);
        }

        AudioLogFactory* fake_log_factory() { return &fake_log_factory_; }

#if defined(OS_WIN)
        // This should be called before creating an AudioManager in tests to ensure
        // that the creating thread is COM initialized.
        void InitializeCOMForTesting()
        {
            com_initializer_for_testing_.reset(new base::win::ScopedCOMInitializer());
        }
#endif

#if defined(OS_LINUX)
        void set_app_name(const std::string& app_name)
        {
            app_name_ = app_name;
        }

        const std::string& app_name() const
        {
            return app_name_;
        }
#endif

    private:
        FakeAudioLogFactory fake_log_factory_;

        const base::TimeDelta max_hung_task_time_;
        scoped_refptr<base::SingleThreadTaskRunner> monitor_task_runner_;

        base::Lock hang_lock_;
        bool hang_detection_enabled_;
        base::TimeTicks last_audio_thread_timer_tick_;
        int hang_failures_;
        bool io_task_running_;
        bool audio_task_running_;

#if defined(OS_WIN)
        scoped_ptr<base::win::ScopedCOMInitializer> com_initializer_for_testing_;
#endif

#if defined(OS_LINUX)
        std::string app_name_;
#endif

        DISALLOW_COPY_AND_ASSIGN(AudioManagerHelper);
    };

    bool g_hang_monitor_enabled = false;

    base::LazyInstance<AudioManagerHelper>::Leaky g_helper = LAZY_INSTANCE_INITIALIZER;

} // namespace

// Forward declaration of the platform specific AudioManager factory function.
AudioManager* CreateAudioManager(AudioLogFactory* audio_log_factory);

AudioManager::AudioManager() { }

AudioManager::~AudioManager()
{
    CHECK(!g_last_created || g_last_created == this);
    g_last_created = nullptr;
}

// static
void AudioManager::SetFactory(AudioManagerFactory* factory)
{
    CHECK(factory);
    CHECK(!g_last_created);
    CHECK(!g_audio_manager_factory);
    g_audio_manager_factory = factory;
}

// static
void AudioManager::ResetFactoryForTesting()
{
    if (g_audio_manager_factory) {
        delete g_audio_manager_factory;
        g_audio_manager_factory = nullptr;
    }
}

// static
AudioManager* AudioManager::Create(AudioLogFactory* audio_log_factory)
{
    CHECK(!g_last_created);
    if (g_audio_manager_factory)
        g_last_created = g_audio_manager_factory->CreateInstance(audio_log_factory);
    else
        g_last_created = CreateAudioManager(audio_log_factory);

    return g_last_created;
}

// static
AudioManager* AudioManager::CreateWithHangTimer(
    AudioLogFactory* audio_log_factory,
    const scoped_refptr<base::SingleThreadTaskRunner>& monitor_task_runner)
{
    AudioManager* manager = Create(audio_log_factory);
    if (g_hang_monitor_enabled || base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableAudioHangMonitor)) {
        g_helper.Pointer()->StartHangTimer(monitor_task_runner);
    }
    return manager;
}

// static
AudioManager* AudioManager::CreateForTesting()
{
#if defined(OS_WIN)
    g_helper.Pointer()->InitializeCOMForTesting();
#endif
    return Create(g_helper.Pointer()->fake_log_factory());
}

// static
void AudioManager::EnableHangMonitor()
{
    CHECK(!g_last_created);
// On OSX the audio thread is the UI thread, for which a hang monitor is not
// necessary or recommended.  If it's manually requested, we should allow it
// to start though.
#if !defined(OS_MACOSX)
    g_hang_monitor_enabled = true;
#endif
}

#if defined(OS_LINUX)
// static
void AudioManager::SetGlobalAppName(const std::string& app_name)
{
    g_helper.Pointer()->set_app_name(app_name);
}

// static
const std::string& AudioManager::GetGlobalAppName()
{
    return g_helper.Pointer()->app_name();
}
#endif

// static
AudioManager* AudioManager::Get()
{
    return g_last_created;
}

// static
std::string AudioManager::GetDefaultDeviceName()
{
    return GetLocalizedStringUTF8(DEFAULT_AUDIO_DEVICE_NAME);
}

// static
std::string AudioManager::GetCommunicationsDeviceName()
{
#if defined(OS_WIN)
    return GetLocalizedStringUTF8(COMMUNICATIONS_AUDIO_DEVICE_NAME);
#else
    NOTREACHED();
    return "";
#endif
}

} // namespace media
