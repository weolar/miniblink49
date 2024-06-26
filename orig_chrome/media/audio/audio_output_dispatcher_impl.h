// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// AudioOutputDispatcherImpl is an implementation of AudioOutputDispatcher.
//
// To avoid opening and closing audio devices more frequently than necessary,
// each dispatcher has a pool of inactive physical streams. A stream is closed
// only if it hasn't been used for a certain period of time (specified via the
// constructor).
//

#ifndef MEDIA_AUDIO_AUDIO_OUTPUT_DISPATCHER_IMPL_H_
#define MEDIA_AUDIO_AUDIO_OUTPUT_DISPATCHER_IMPL_H_

#include <map>
#include <vector>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "base/timer/timer.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_logging.h"
#include "media/audio/audio_manager.h"
#include "media/audio/audio_output_dispatcher.h"
#include "media/audio/audio_parameters.h"

namespace media {

class AudioOutputProxy;

class MEDIA_EXPORT AudioOutputDispatcherImpl : public AudioOutputDispatcher {
public:
    // |close_delay| specifies delay after the stream is idle until the audio
    // device is closed.
    AudioOutputDispatcherImpl(AudioManager* audio_manager,
        const AudioParameters& params,
        const std::string& output_device_id,
        const base::TimeDelta& close_delay);

    // Opens a new physical stream if there are no pending streams in
    // |idle_streams_|.  Do not call Close() or Stop() if this method fails.
    bool OpenStream() override;

    // If there are pending streams in |idle_streams_| then it reuses one of
    // them, otherwise creates a new one.
    bool StartStream(AudioOutputStream::AudioSourceCallback* callback,
        AudioOutputProxy* stream_proxy) override;

    // Stops the stream assigned to the specified proxy and moves it into
    // |idle_streams_| for reuse by other proxies.
    void StopStream(AudioOutputProxy* stream_proxy) override;

    void StreamVolumeSet(AudioOutputProxy* stream_proxy, double volume) override;

    // Closes |idle_streams_| until the number of |idle_streams_| is equal to the
    // |idle_proxies_| count.  If there are no |idle_proxies_| a single stream is
    // kept alive until |close_timer_| fires.
    void CloseStream(AudioOutputProxy* stream_proxy) override;

    void Shutdown() override;

    // Returns true if there are any open AudioOutputProxy objects.
    bool HasOutputProxies() const;

    // Closes all |idle_streams_|.
    void CloseAllIdleStreams();

private:
    friend class base::RefCountedThreadSafe<AudioOutputDispatcherImpl>;
    ~AudioOutputDispatcherImpl() override;

    // Creates a new physical output stream, opens it and pushes to
    // |idle_streams_|.  Returns false if the stream couldn't be created or
    // opened.
    bool CreateAndOpenStream();

    // Similar to CloseAllIdleStreams(), but keeps |keep_alive| streams alive.
    void CloseIdleStreams(size_t keep_alive);

    size_t idle_proxies_;
    std::vector<AudioOutputStream*> idle_streams_;

    // When streams are stopped they're added to |idle_streams_|, if no stream is
    // reused before |close_delay_| elapses |close_timer_| will run
    // CloseIdleStreams().
    base::DelayTimer close_timer_;

    typedef std::map<AudioOutputProxy*, AudioOutputStream*> AudioStreamMap;
    AudioStreamMap proxy_to_physical_map_;

    scoped_ptr<AudioLog> audio_log_;
    typedef std::map<AudioOutputStream*, int> AudioStreamIDMap;
    AudioStreamIDMap audio_stream_ids_;
    int audio_stream_id_;

    DISALLOW_COPY_AND_ASSIGN(AudioOutputDispatcherImpl);
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_OUTPUT_DISPATCHER_IMPL_H_
