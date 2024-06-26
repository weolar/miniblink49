// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_MIDI_MIDI_SCHEDULER_H_
#define MEDIA_MIDI_MIDI_SCHEDULER_H_

#include "base/callback.h"
#include "base/memory/weak_ptr.h"
#include "media/midi/midi_export.h"

namespace media {
namespace midi {

    class MidiManager;
    class MidiManagerClient;

    // TODO(crbug.com/467442): Make tasks cancelable per client.
    class MIDI_EXPORT MidiScheduler final {
    public:
        explicit MidiScheduler(MidiManager* manager);
        ~MidiScheduler();

        // Post |closure| to the current message loop safely. The |closure| will not
        // be invoked after MidiScheduler is deleted. AccumulateMidiBytesSent() of
        // |client| is called internally.
        void PostSendDataTask(MidiManagerClient* client,
            size_t length,
            double timestamp,
            const base::Closure& closure);

    private:
        void InvokeClosure(MidiManagerClient* client,
            size_t length,
            const base::Closure& closure);

        // MidiManager should own the MidiScheduler and be alive longer.
        MidiManager* manager_;
        base::WeakPtrFactory<MidiScheduler> weak_factory_;

        DISALLOW_COPY_AND_ASSIGN(MidiScheduler);
    };

} // namespace midi
} // namespace media

#endif // MEDIA_MIDI_MIDI_SCHEDULER_H_
