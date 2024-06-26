// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_input_controller.h"

#include "base/bind.h"
#include "base/metrics/histogram_macros.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/thread_task_runner_handle.h"
#include "base/threading/thread_restrictions.h"
#include "base/time/time.h"
#include "media/audio/audio_input_writer.h"
#include "media/base/user_input_monitor.h"

using base::TimeDelta;

namespace {

const int kMaxInputChannels = 3;

// TODO(henrika): remove usage of timers and add support for proper
// notification of when the input device is removed.  This was originally added
// to resolve http://crbug.com/79936 for Windows platforms.  This then caused
// breakage (very hard to repro bugs!) on other platforms: See
// http://crbug.com/226327 and http://crbug.com/230972.
// See also that the timer has been disabled on Mac now due to
// crbug.com/357501.
const int kTimerResetIntervalSeconds = 1;
// We have received reports that the timer can be too trigger happy on some
// Mac devices and the initial timer interval has therefore been increased
// from 1 second to 5 seconds.
const int kTimerInitialIntervalSeconds = 5;

#if defined(AUDIO_POWER_MONITORING)
// Time in seconds between two successive measurements of audio power levels.
const int kPowerMonitorLogIntervalSeconds = 15;

// A warning will be logged when the microphone audio volume is below this
// threshold.
const int kLowLevelMicrophoneLevelPercent = 10;

// Logs if the user has enabled the microphone mute or not. This is normally
// done by marking a checkbox in an audio-settings UI which is unique for each
// platform. Elements in this enum should not be added, deleted or rearranged.
enum MicrophoneMuteResult {
    MICROPHONE_IS_MUTED = 0,
    MICROPHONE_IS_NOT_MUTED = 1,
    MICROPHONE_MUTE_MAX = MICROPHONE_IS_NOT_MUTED
};

void LogMicrophoneMuteResult(MicrophoneMuteResult result)
{
    UMA_HISTOGRAM_ENUMERATION("Media.MicrophoneMuted",
        result,
        MICROPHONE_MUTE_MAX + 1);
}

// Helper method which calculates the average power of an audio bus. Unit is in
// dBFS, where 0 dBFS corresponds to all channels and samples equal to 1.0.
float AveragePower(const media::AudioBus& buffer)
{
    const int frames = buffer.frames();
    const int channels = buffer.channels();
    if (frames <= 0 || channels <= 0)
        return 0.0f;

    // Scan all channels and accumulate the sum of squares for all samples.
    float sum_power = 0.0f;
    for (int ch = 0; ch < channels; ++ch) {
        const float* channel_data = buffer.channel(ch);
        for (int i = 0; i < frames; i++) {
            const float sample = channel_data[i];
            sum_power += sample * sample;
        }
    }

    // Update accumulated average results, with clamping for sanity.
    const float average_power = std::max(0.0f, std::min(1.0f, sum_power / (frames * channels)));

    // Convert average power level to dBFS units, and pin it down to zero if it
    // is insignificantly small.
    const float kInsignificantPower = 1.0e-10f; // -100 dBFS
    const float power_dbfs = average_power < kInsignificantPower ? -std::numeric_limits<float>::infinity() : 10.0f * log10f(average_power);

    return power_dbfs;
}
#endif // AUDIO_POWER_MONITORING

}

// Used to log the result of capture startup.
// This was previously logged as a boolean with only the no callback and OK
// options. The enum order is kept to ensure backwards compatibility.
// Elements in this enum should not be deleted or rearranged; the only
// permitted operation is to add new elements before CAPTURE_STARTUP_RESULT_MAX
// and update CAPTURE_STARTUP_RESULT_MAX.
enum CaptureStartupResult {
    CAPTURE_STARTUP_NO_DATA_CALLBACK = 0,
    CAPTURE_STARTUP_OK = 1,
    CAPTURE_STARTUP_CREATE_STREAM_FAILED = 2,
    CAPTURE_STARTUP_OPEN_STREAM_FAILED = 3,
    CAPTURE_STARTUP_RESULT_MAX = CAPTURE_STARTUP_OPEN_STREAM_FAILED
};

void LogCaptureStartupResult(CaptureStartupResult result)
{
    UMA_HISTOGRAM_ENUMERATION("Media.AudioInputControllerCaptureStartupSuccess",
        result,
        CAPTURE_STARTUP_RESULT_MAX + 1);
}

namespace media {

// static
AudioInputController::Factory* AudioInputController::factory_ = NULL;

AudioInputController::AudioInputController(EventHandler* handler,
    SyncWriter* sync_writer,
    UserInputMonitor* user_input_monitor,
    const bool agc_is_enabled)
    : creator_task_runner_(base::ThreadTaskRunnerHandle::Get())
    , handler_(handler)
    , stream_(NULL)
    , data_is_active_(false)
    , state_(CLOSED)
    , sync_writer_(sync_writer)
    , max_volume_(0.0)
    , user_input_monitor_(user_input_monitor)
    , agc_is_enabled_(agc_is_enabled)
    ,
#if defined(AUDIO_POWER_MONITORING)
    power_measurement_is_enabled_(false)
    , log_silence_state_(false)
    , silence_state_(SILENCE_STATE_NO_MEASUREMENT)
    ,
#endif
    prev_key_down_count_(0)
    , input_writer_(nullptr)
{
    DCHECK(creator_task_runner_.get());
}

AudioInputController::~AudioInputController()
{
    DCHECK_EQ(state_, CLOSED);
}

// static
scoped_refptr<AudioInputController> AudioInputController::Create(
    AudioManager* audio_manager,
    EventHandler* event_handler,
    const AudioParameters& params,
    const std::string& device_id,
    UserInputMonitor* user_input_monitor)
{
    DCHECK(audio_manager);

    if (!params.IsValid() || (params.channels() > kMaxInputChannels))
        return NULL;

    if (factory_) {
        return factory_->Create(
            audio_manager, event_handler, params, user_input_monitor);
    }
    scoped_refptr<AudioInputController> controller(
        new AudioInputController(event_handler, NULL, user_input_monitor, false));

    controller->task_runner_ = audio_manager->GetTaskRunner();

    // Create and open a new audio input stream from the existing
    // audio-device thread.
    if (!controller->task_runner_->PostTask(
            FROM_HERE,
            base::Bind(&AudioInputController::DoCreate,
                controller,
                base::Unretained(audio_manager),
                params,
                device_id))) {
        controller = NULL;
    }

    return controller;
}

// static
scoped_refptr<AudioInputController> AudioInputController::CreateLowLatency(
    AudioManager* audio_manager,
    EventHandler* event_handler,
    const AudioParameters& params,
    const std::string& device_id,
    SyncWriter* sync_writer,
    UserInputMonitor* user_input_monitor,
    const bool agc_is_enabled)
{
    DCHECK(audio_manager);
    DCHECK(sync_writer);

    if (!params.IsValid() || (params.channels() > kMaxInputChannels))
        return NULL;

    // Create the AudioInputController object and ensure that it runs on
    // the audio-manager thread.
    scoped_refptr<AudioInputController> controller(new AudioInputController(
        event_handler, sync_writer, user_input_monitor, agc_is_enabled));
    controller->task_runner_ = audio_manager->GetTaskRunner();

    // Create and open a new audio input stream from the existing
    // audio-device thread. Use the provided audio-input device.
    if (!controller->task_runner_->PostTask(
            FROM_HERE,
            base::Bind(&AudioInputController::DoCreateForLowLatency,
                controller,
                base::Unretained(audio_manager),
                params,
                device_id))) {
        controller = NULL;
    }

    return controller;
}

// static
scoped_refptr<AudioInputController> AudioInputController::CreateForStream(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    EventHandler* event_handler,
    AudioInputStream* stream,
    SyncWriter* sync_writer,
    UserInputMonitor* user_input_monitor)
{
    DCHECK(sync_writer);
    DCHECK(stream);

    // Create the AudioInputController object and ensure that it runs on
    // the audio-manager thread.
    scoped_refptr<AudioInputController> controller(new AudioInputController(
        event_handler, sync_writer, user_input_monitor, false));
    controller->task_runner_ = task_runner;

    // TODO(miu): See TODO at top of file.  Until that's resolved, we need to
    // disable the error auto-detection here (since the audio mirroring
    // implementation will reliably report error and close events).  Note, of
    // course, that we're assuming CreateForStream() has been called for the audio
    // mirroring use case only.
    if (!controller->task_runner_->PostTask(
            FROM_HERE,
            base::Bind(&AudioInputController::DoCreateForStream,
                controller,
                stream))) {
        controller = NULL;
    }

    return controller;
}

void AudioInputController::Record()
{
    task_runner_->PostTask(FROM_HERE, base::Bind(&AudioInputController::DoRecord, this));
}

void AudioInputController::Close(const base::Closure& closed_task)
{
    DCHECK(!closed_task.is_null());
    DCHECK(creator_task_runner_->BelongsToCurrentThread());

    task_runner_->PostTaskAndReply(
        FROM_HERE, base::Bind(&AudioInputController::DoClose, this), closed_task);
}

void AudioInputController::SetVolume(double volume)
{
    task_runner_->PostTask(FROM_HERE, base::Bind(&AudioInputController::DoSetVolume, this, volume));
}

void AudioInputController::DoCreate(AudioManager* audio_manager,
    const AudioParameters& params,
    const std::string& device_id)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    SCOPED_UMA_HISTOGRAM_TIMER("Media.AudioInputController.CreateTime");
    if (handler_)
        handler_->OnLog(this, "AIC::DoCreate");

#if defined(AUDIO_POWER_MONITORING)
    // Disable power monitoring for streams that run without AGC enabled to
    // avoid adding logs and UMA for non-WebRTC clients.
    power_measurement_is_enabled_ = agc_is_enabled_;
    last_audio_level_log_time_ = base::TimeTicks::Now();
    silence_state_ = SILENCE_STATE_NO_MEASUREMENT;
#endif

    // TODO(miu): See TODO at top of file.  Until that's resolved, assume all
    // platform audio input requires the |no_data_timer_| be used to auto-detect
    // errors.  In reality, probably only Windows needs to be treated as
    // unreliable here.
    DoCreateForStream(audio_manager->MakeAudioInputStream(params, device_id));
}

void AudioInputController::DoCreateForLowLatency(AudioManager* audio_manager,
    const AudioParameters& params,
    const std::string& device_id)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

#if defined(AUDIO_POWER_MONITORING)
    // We only log silence state UMA stats for low latency mode and if we use a
    // real device.
    if (params.format() != AudioParameters::AUDIO_FAKE)
        log_silence_state_ = true;
#endif

    low_latency_create_time_ = base::TimeTicks::Now();
    DoCreate(audio_manager, params, device_id);
}

void AudioInputController::DoCreateForStream(
    AudioInputStream* stream_to_control)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    DCHECK(!stream_);
    stream_ = stream_to_control;

    if (!stream_) {
        if (handler_)
            handler_->OnError(this, STREAM_CREATE_ERROR);
        LogCaptureStartupResult(CAPTURE_STARTUP_CREATE_STREAM_FAILED);
        return;
    }

    if (stream_ && !stream_->Open()) {
        stream_->Close();
        stream_ = NULL;
        if (handler_)
            handler_->OnError(this, STREAM_OPEN_ERROR);
        LogCaptureStartupResult(CAPTURE_STARTUP_OPEN_STREAM_FAILED);
        return;
    }

    DCHECK(!no_data_timer_.get());

    // Set AGC state using mode in |agc_is_enabled_| which can only be enabled in
    // CreateLowLatency().
#if defined(AUDIO_POWER_MONITORING)
    bool agc_is_supported = false;
    agc_is_supported = stream_->SetAutomaticGainControl(agc_is_enabled_);
    // Disable power measurements on platforms that does not support AGC at a
    // lower level. AGC can fail on platforms where we don't support the
    // functionality to modify the input volume slider. One such example is
    // Windows XP.
    power_measurement_is_enabled_ &= agc_is_supported;
#else
    stream_->SetAutomaticGainControl(agc_is_enabled_);
#endif

    // Create the data timer which will call FirstCheckForNoData(). The timer
    // is started in DoRecord() and restarted in each DoCheckForNoData()
    // callback.
    // The timer is enabled for logging purposes. The NO_DATA_ERROR triggered
    // from the timer must be ignored by the EventHandler.
    // TODO(henrika): remove usage of timer when it has been verified on Canary
    // that we are safe doing so. Goal is to get rid of |no_data_timer_| and
    // everything that is tied to it. crbug.com/357569.
    no_data_timer_.reset(new base::Timer(
        FROM_HERE, base::TimeDelta::FromSeconds(kTimerInitialIntervalSeconds),
        base::Bind(&AudioInputController::FirstCheckForNoData,
            base::Unretained(this)),
        false));

    state_ = CREATED;
    if (handler_)
        handler_->OnCreated(this);

    if (user_input_monitor_) {
        user_input_monitor_->EnableKeyPressMonitoring();
        prev_key_down_count_ = user_input_monitor_->GetKeyPressCount();
    }
}

void AudioInputController::DoRecord()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    SCOPED_UMA_HISTOGRAM_TIMER("Media.AudioInputController.RecordTime");

    if (state_ != CREATED)
        return;

    {
        base::AutoLock auto_lock(lock_);
        state_ = RECORDING;
    }

    if (handler_)
        handler_->OnLog(this, "AIC::DoRecord");

    if (no_data_timer_) {
        // Start the data timer. Once |kTimerResetIntervalSeconds| have passed,
        // a callback to FirstCheckForNoData() is made.
        no_data_timer_->Reset();
    }

    stream_->Start(this);
    if (handler_)
        handler_->OnRecording(this);
}

void AudioInputController::DoClose()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    SCOPED_UMA_HISTOGRAM_TIMER("Media.AudioInputController.CloseTime");

    if (state_ == CLOSED)
        return;

    // If this is a low-latency stream, log the total duration (since DoCreate)
    // and add it to a UMA histogram.
    if (!low_latency_create_time_.is_null()) {
        base::TimeDelta duration = base::TimeTicks::Now() - low_latency_create_time_;
        //UMA_HISTOGRAM_LONG_TIMES("Media.InputStreamDuration", duration);
        if (handler_) {
            std::string log_string = base::StringPrintf("AIC::DoClose: stream duration=");
            log_string += base::Int64ToString(duration.InSeconds());
            log_string += " seconds";
            handler_->OnLog(this, log_string);
        }
    }

    // Delete the timer on the same thread that created it.
    no_data_timer_.reset();

    DoStopCloseAndClearStream();
    SetDataIsActive(false);

    if (SharedMemoryAndSyncSocketMode())
        sync_writer_->Close();

    if (user_input_monitor_)
        user_input_monitor_->DisableKeyPressMonitoring();

#if defined(AUDIO_POWER_MONITORING)
    // Send UMA stats if enabled.
    if (log_silence_state_)
        LogSilenceState(silence_state_);
    log_silence_state_ = false;
#endif

    input_writer_ = nullptr;

    state_ = CLOSED;
}

void AudioInputController::DoReportError()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    if (handler_)
        handler_->OnError(this, STREAM_ERROR);
}

void AudioInputController::DoSetVolume(double volume)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_GE(volume, 0);
    DCHECK_LE(volume, 1.0);

    if (state_ != CREATED && state_ != RECORDING)
        return;

    // Only ask for the maximum volume at first call and use cached value
    // for remaining function calls.
    if (!max_volume_) {
        max_volume_ = stream_->GetMaxVolume();
    }

    if (max_volume_ == 0.0) {
        DLOG(WARNING) << "Failed to access input volume control";
        return;
    }

    // Set the stream volume and scale to a range matched to the platform.
    stream_->SetVolume(max_volume_ * volume);
}

void AudioInputController::FirstCheckForNoData()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    LogCaptureStartupResult(GetDataIsActive() ? CAPTURE_STARTUP_OK : CAPTURE_STARTUP_NO_DATA_CALLBACK);
    if (handler_) {
        handler_->OnLog(this, GetDataIsActive() ? "AIC::FirstCheckForNoData => data is active" : "AIC::FirstCheckForNoData => data is NOT active");
    }
    DoCheckForNoData();
}

void AudioInputController::DoCheckForNoData()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (!GetDataIsActive()) {
        // The data-is-active marker will be false only if it has been more than
        // one second since a data packet was recorded. This can happen if a
        // capture device has been removed or disabled.
        if (handler_)
            handler_->OnError(this, NO_DATA_ERROR);
    }

    // Mark data as non-active. The flag will be re-enabled in OnData() each
    // time a data packet is received. Hence, under normal conditions, the
    // flag will only be disabled during a very short period.
    SetDataIsActive(false);

    // Restart the timer to ensure that we check the flag again in
    // |kTimerResetIntervalSeconds|.
    no_data_timer_->Start(
        FROM_HERE, base::TimeDelta::FromSeconds(kTimerResetIntervalSeconds),
        base::Bind(&AudioInputController::DoCheckForNoData,
            base::Unretained(this)));
}

void AudioInputController::OnData(AudioInputStream* stream,
    const AudioBus* source,
    uint32 hardware_delay_bytes,
    double volume)
{
    // |input_writer_| should only be accessed on the audio thread, but as a means
    // to avoid copying data and posting on the audio thread, we just check for
    // non-null here.
    if (input_writer_) {
        scoped_ptr<AudioBus> source_copy = AudioBus::Create(source->channels(), source->frames());
        source->CopyTo(source_copy.get());
        task_runner_->PostTask(
            FROM_HERE,
            base::Bind(
                &AudioInputController::WriteInputDataForDebugging,
                this,
                base::Passed(&source_copy)));
    }

    // Mark data as active to ensure that the periodic calls to
    // DoCheckForNoData() does not report an error to the event handler.
    SetDataIsActive(true);

    {
        base::AutoLock auto_lock(lock_);
        if (state_ != RECORDING)
            return;
    }

    bool key_pressed = false;
    if (user_input_monitor_) {
        size_t current_count = user_input_monitor_->GetKeyPressCount();
        key_pressed = current_count != prev_key_down_count_;
        prev_key_down_count_ = current_count;
        DVLOG_IF(6, key_pressed) << "Detected keypress.";
    }

    // Use SharedMemory and SyncSocket if the client has created a SyncWriter.
    // Used by all low-latency clients except WebSpeech.
    if (SharedMemoryAndSyncSocketMode()) {
        sync_writer_->Write(source, volume, key_pressed, hardware_delay_bytes);

#if defined(AUDIO_POWER_MONITORING)
        // Only do power-level measurements if DoCreate() has been called. It will
        // ensure that logging will mainly be done for WebRTC and WebSpeech
        // clients.
        if (!power_measurement_is_enabled_)
            return;

        // Perform periodic audio (power) level measurements.
        if ((base::TimeTicks::Now() - last_audio_level_log_time_).InSeconds() > kPowerMonitorLogIntervalSeconds) {
            // Calculate the average power of the signal, or the energy per sample.
            const float average_power_dbfs = AveragePower(*source);

            // Add current microphone volume to log and UMA histogram.
            const int mic_volume_percent = static_cast<int>(100.0 * volume);

            // Use event handler on the audio thread to relay a message to the ARIH
            // in content which does the actual logging on the IO thread.
            task_runner_->PostTask(FROM_HERE,
                base::Bind(&AudioInputController::DoLogAudioLevels,
                    this,
                    average_power_dbfs,
                    mic_volume_percent));

            last_audio_level_log_time_ = base::TimeTicks::Now();
        }
#endif
        return;
    }

    // TODO(henrika): Investigate if we can avoid the extra copy here.
    // (see http://crbug.com/249316 for details). AFAIK, this scope is only
    // active for WebSpeech clients.
    scoped_ptr<AudioBus> audio_data = AudioBus::Create(source->channels(), source->frames());
    source->CopyTo(audio_data.get());

    // Ownership of the audio buffer will be with the callback until it is run,
    // when ownership is passed to the callback function.
    task_runner_->PostTask(
        FROM_HERE,
        base::Bind(
            &AudioInputController::DoOnData, this, base::Passed(&audio_data)));
}

void AudioInputController::DoOnData(scoped_ptr<AudioBus> data)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    if (handler_)
        handler_->OnData(this, data.get());
}

void AudioInputController::DoLogAudioLevels(float level_dbfs,
    int microphone_volume_percent)
{
#if defined(AUDIO_POWER_MONITORING)
    DCHECK(task_runner_->BelongsToCurrentThread());
    if (!handler_)
        return;

    // Detect if the user has enabled hardware mute by pressing the mute
    // button in audio settings for the selected microphone.
    const bool microphone_is_muted = stream_->IsMuted();
    if (microphone_is_muted) {
        LogMicrophoneMuteResult(MICROPHONE_IS_MUTED);
        handler_->OnLog(this, "AIC::OnData: microphone is muted!");
        // Return early if microphone is muted. No need to adding logs and UMA stats
        // of audio levels if we know that the micropone is muted.
        return;
    }

    LogMicrophoneMuteResult(MICROPHONE_IS_NOT_MUTED);

    std::string log_string = base::StringPrintf(
        "AIC::OnData: average audio level=%.2f dBFS", level_dbfs);
    static const float kSilenceThresholdDBFS = -72.24719896f;
    if (level_dbfs < kSilenceThresholdDBFS)
        log_string += " <=> low audio input level!";
    handler_->OnLog(this, log_string);

    UpdateSilenceState(level_dbfs < kSilenceThresholdDBFS);

    //UMA_HISTOGRAM_PERCENTAGE("Media.MicrophoneVolume", microphone_volume_percent);
    log_string = base::StringPrintf(
        "AIC::OnData: microphone volume=%d%%", microphone_volume_percent);
    if (microphone_volume_percent < kLowLevelMicrophoneLevelPercent)
        log_string += " <=> low microphone level!";
    handler_->OnLog(this, log_string);
#endif
}

void AudioInputController::OnError(AudioInputStream* stream)
{
    // Handle error on the audio-manager thread.
    task_runner_->PostTask(FROM_HERE, base::Bind(&AudioInputController::DoReportError, this));
}

void AudioInputController::EnableDebugRecording(
    AudioInputWriter* input_writer)
{
    task_runner_->PostTask(FROM_HERE, base::Bind(&AudioInputController::DoEnableDebugRecording, this, input_writer));
}

void AudioInputController::DisableDebugRecording(
    const base::Closure& callback)
{
    DCHECK(creator_task_runner_->BelongsToCurrentThread());
    DCHECK(!callback.is_null());

    task_runner_->PostTaskAndReply(
        FROM_HERE,
        base::Bind(&AudioInputController::DoDisableDebugRecording,
            this),
        callback);
}

void AudioInputController::DoStopCloseAndClearStream()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // Allow calling unconditionally and bail if we don't have a stream to close.
    if (stream_ != NULL) {
        stream_->Stop();
        stream_->Close();
        stream_ = NULL;
    }

    // The event handler should not be touched after the stream has been closed.
    handler_ = NULL;
}

void AudioInputController::SetDataIsActive(bool enabled)
{
    base::subtle::Release_Store(&data_is_active_, enabled);
}

bool AudioInputController::GetDataIsActive()
{
    return (base::subtle::Acquire_Load(&data_is_active_) != false);
}

#if defined(AUDIO_POWER_MONITORING)
void AudioInputController::UpdateSilenceState(bool silence)
{
    if (silence) {
        if (silence_state_ == SILENCE_STATE_NO_MEASUREMENT) {
            silence_state_ = SILENCE_STATE_ONLY_SILENCE;
        } else if (silence_state_ == SILENCE_STATE_ONLY_AUDIO) {
            silence_state_ = SILENCE_STATE_AUDIO_AND_SILENCE;
        } else {
            DCHECK(silence_state_ == SILENCE_STATE_ONLY_SILENCE || silence_state_ == SILENCE_STATE_AUDIO_AND_SILENCE);
        }
    } else {
        if (silence_state_ == SILENCE_STATE_NO_MEASUREMENT) {
            silence_state_ = SILENCE_STATE_ONLY_AUDIO;
        } else if (silence_state_ == SILENCE_STATE_ONLY_SILENCE) {
            silence_state_ = SILENCE_STATE_AUDIO_AND_SILENCE;
        } else {
            DCHECK(silence_state_ == SILENCE_STATE_ONLY_AUDIO || silence_state_ == SILENCE_STATE_AUDIO_AND_SILENCE);
        }
    }
}

void AudioInputController::LogSilenceState(SilenceState value)
{
    UMA_HISTOGRAM_ENUMERATION("Media.AudioInputControllerSessionSilenceReport",
        value,
        SILENCE_STATE_MAX + 1);
}
#endif

void AudioInputController::DoEnableDebugRecording(
    AudioInputWriter* input_writer)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(!input_writer_);
    input_writer_ = input_writer;
}

void AudioInputController::DoDisableDebugRecording()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    input_writer_ = nullptr;
}

void AudioInputController::WriteInputDataForDebugging(
    scoped_ptr<AudioBus> data)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    if (input_writer_)
        input_writer_->Write(data.Pass());
}

} // namespace media
