// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_MIDI_MIDI_MANAGER_WIN_H_
#define MEDIA_MIDI_MIDI_MANAGER_WIN_H_

#include <vector>

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/threading/thread.h"
#include "base/time/time.h"
#include "media/midi/midi_manager.h"

namespace media {
namespace midi {

    class MidiServiceWinDelegate {
    public:
        virtual ~MidiServiceWinDelegate() { }
        virtual void OnCompleteInitialization(Result result) = 0;
        virtual void OnAddInputPort(MidiPortInfo info) = 0;
        virtual void OnAddOutputPort(MidiPortInfo info) = 0;
        virtual void OnSetInputPortState(uint32 port_index, MidiPortState state) = 0;
        virtual void OnSetOutputPortState(uint32 port_index, MidiPortState state) = 0;
        virtual void OnReceiveMidiData(uint32 port_index,
            const std::vector<uint8>& data,
            base::TimeTicks time)
            = 0;
    };

    class MidiServiceWin {
    public:
        virtual ~MidiServiceWin() { }
        // This method may return before the initialization is completed.
        virtual void InitializeAsync(MidiServiceWinDelegate* delegate) = 0;
        // This method may return before the specified data is actually sent.
        virtual void SendMidiDataAsync(uint32 port_number,
            const std::vector<uint8>& data,
            base::TimeTicks time)
            = 0;
    };

    class MidiManagerWin final : public MidiManager, public MidiServiceWinDelegate {
    public:
        MidiManagerWin();
        ~MidiManagerWin() override;

        // MidiManager overrides:
        void StartInitialization() final;
        void Finalize() final;
        void DispatchSendMidiData(MidiManagerClient* client,
            uint32 port_index,
            const std::vector<uint8>& data,
            double timestamp) final;

        // MidiServiceWinDelegate overrides:
        void OnCompleteInitialization(Result result) final;
        void OnAddInputPort(MidiPortInfo info) final;
        void OnAddOutputPort(MidiPortInfo info) final;
        void OnSetInputPortState(uint32 port_index, MidiPortState state) final;
        void OnSetOutputPortState(uint32 port_index, MidiPortState state) final;
        void OnReceiveMidiData(uint32 port_index,
            const std::vector<uint8>& data,
            base::TimeTicks time) final;

    private:
        scoped_ptr<MidiServiceWin> midi_service_;
        DISALLOW_COPY_AND_ASSIGN(MidiManagerWin);
    };

} // namespace midi
} // namespace media

#endif // MEDIA_MIDI_MIDI_MANAGER_WIN_H_
