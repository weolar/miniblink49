// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_MIDI_MIDI_MANAGER_H_
#define MEDIA_MIDI_MIDI_MANAGER_H_

#include <set>
#include <vector>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"
#include "base/time/time.h"
#include "media/midi/midi_export.h"
#include "media/midi/midi_port_info.h"
#include "media/midi/result.h"

namespace base {
class SingleThreadTaskRunner;
} // namespace base

namespace media {
namespace midi {

    // A MidiManagerClient registers with the MidiManager to receive MIDI data.
    // See MidiManager::RequestAccess() and MidiManager::ReleaseAccess()
    // for details.
    class MIDI_EXPORT MidiManagerClient {
    public:
        virtual ~MidiManagerClient() { }

        // AddInputPort() and AddOutputPort() are called before CompleteStartSession()
        // is called to notify existing MIDI ports, and also called after that to
        // notify new MIDI ports are added.
        virtual void AddInputPort(const MidiPortInfo& info) = 0;
        virtual void AddOutputPort(const MidiPortInfo& info) = 0;

        // SetInputPortState() and SetOutputPortState() are called to notify a known
        // device gets disconnected, or connected again.
        virtual void SetInputPortState(uint32 port_index, MidiPortState state) = 0;
        virtual void SetOutputPortState(uint32 port_index, MidiPortState state) = 0;

        // CompleteStartSession() is called when platform dependent preparation is
        // finished.
        virtual void CompleteStartSession(Result result) = 0;

        // ReceiveMidiData() is called when MIDI data has been received from the
        // MIDI system.
        // |port_index| represents the specific input port from input_ports().
        // |data| represents a series of bytes encoding one or more MIDI messages.
        // |length| is the number of bytes in |data|.
        // |timestamp| is the time the data was received, in seconds.
        virtual void ReceiveMidiData(uint32 port_index,
            const uint8* data,
            size_t length,
            double timestamp)
            = 0;

        // AccumulateMidiBytesSent() is called to acknowledge when bytes have
        // successfully been sent to the hardware.
        // This happens as a result of the client having previously called
        // MidiManager::DispatchSendMidiData().
        virtual void AccumulateMidiBytesSent(size_t n) = 0;

        // Detach() is called when MidiManager is going to shutdown immediately.
        // Client should not touch MidiManager instance after Detach() is called.
        virtual void Detach() = 0;
    };

    // Manages access to all MIDI hardware.
    class MIDI_EXPORT MidiManager {
    public:
        static const size_t kMaxPendingClientCount = 128;

        MidiManager();
        virtual ~MidiManager();

        // The constructor and the destructor will be called on the CrBrowserMain
        // thread.
        static MidiManager* Create();

        // Called on the CrBrowserMain thread to notify the Chrome_IOThread will stop
        // and the instance will be destructed on the CrBrowserMain thread soon.
        void Shutdown();

        // A client calls StartSession() to receive and send MIDI data.
        // If the session is ready to start, the MIDI system is lazily initialized
        // and the client is registered to receive MIDI data.
        // CompleteStartSession() is called with Result::OK if the session is started.
        // Otherwise CompleteStartSession() is called with proper Result code.
        // StartSession() and EndSession() can be called on the Chrome_IOThread.
        // CompleteStartSession() will be invoked on the same Chrome_IOThread.
        void StartSession(MidiManagerClient* client);

        // A client calls EndSession() to stop receiving MIDI data.
        void EndSession(MidiManagerClient* client);

        // Invoke AccumulateMidiBytesSent() for |client| safely. If the session was
        // already closed, do nothing.
        void AccumulateMidiBytesSent(MidiManagerClient* client, size_t n);

        // DispatchSendMidiData() is called when MIDI data should be sent to the MIDI
        // system.
        // This method is supposed to return immediately and should not block.
        // |port_index| represents the specific output port from output_ports().
        // |data| represents a series of bytes encoding one or more MIDI messages.
        // |length| is the number of bytes in |data|.
        // |timestamp| is the time to send the data, in seconds. A value of 0
        // means send "now" or as soon as possible.
        // The default implementation is for unsupported platforms.
        virtual void DispatchSendMidiData(MidiManagerClient* client,
            uint32 port_index,
            const std::vector<uint8>& data,
            double timestamp);

    protected:
        friend class MidiManagerUsb;

        // Initializes the platform dependent MIDI system. MidiManager class has a
        // default implementation that synchronously calls CompleteInitialization()
        // with Result::NOT_SUPPORTED on the caller thread. A derived class for a
        // specific platform should override this method correctly.
        // This method is called on Chrome_IOThread thread inside StartSession().
        // Platform dependent initialization can be processed synchronously or
        // asynchronously. When the initialization is completed,
        // CompleteInitialization() should be called with |result|.
        // |result| should be Result::OK on success, otherwise a proper Result.
        virtual void StartInitialization();

        // Finalizes the platform dependent MIDI system. Called on Chrome_IOThread
        // thread and the thread will stop immediately after this call.
        // Platform dependent resources that were allocated on the Chrome_IOThread
        // should be disposed inside this method.
        virtual void Finalize() { }

        // Called from a platform dependent implementation of StartInitialization().
        // It invokes CompleteInitializationInternal() on the thread that calls
        // StartSession() and distributes |result| to MIDIManagerClient objects in
        // |pending_clients_|.
        void CompleteInitialization(Result result);

        void AddInputPort(const MidiPortInfo& info);
        void AddOutputPort(const MidiPortInfo& info);
        void SetInputPortState(uint32 port_index, MidiPortState state);
        void SetOutputPortState(uint32 port_index, MidiPortState state);

        // Dispatches to all clients.
        // TODO(toyoshim): Fix the mac implementation to use
        // |ReceiveMidiData(..., base::TimeTicks)|.
        void ReceiveMidiData(uint32 port_index,
            const uint8* data,
            size_t length,
            double timestamp);

        void ReceiveMidiData(uint32 port_index,
            const uint8* data,
            size_t length,
            base::TimeTicks time)
        {
            ReceiveMidiData(port_index, data, length,
                (time - base::TimeTicks()).InSecondsF());
        }

        size_t clients_size_for_testing() const { return clients_.size(); }
        size_t pending_clients_size_for_testing() const
        {
            return pending_clients_.size();
        }

        const MidiPortInfoList& input_ports() const { return input_ports_; }
        const MidiPortInfoList& output_ports() const { return output_ports_; }

    private:
        void CompleteInitializationInternal(Result result);
        void AddInitialPorts(MidiManagerClient* client);
        void ShutdownOnSessionThread();

        // Keeps track of all clients who wish to receive MIDI data.
        typedef std::set<MidiManagerClient*> ClientSet;
        ClientSet clients_;

        // Keeps track of all clients who are waiting for CompleteStartSession().
        ClientSet pending_clients_;

        // Keeps a SingleThreadTaskRunner of the thread that calls StartSession in
        // order to invoke CompleteStartSession() on the thread.
        scoped_refptr<base::SingleThreadTaskRunner> session_thread_runner_;

        // Keeps true if platform dependent initialization is already completed.
        bool initialized_;

        // Keeps false until Finalize() is called.
        bool finalized_;

        // Keeps the platform dependent initialization result if initialization is
        // completed. Otherwise keeps Result::NOT_INITIALIZED.
        Result result_;

        // Keeps all MidiPortInfo.
        MidiPortInfoList input_ports_;
        MidiPortInfoList output_ports_;

        // Protects access to |clients_|, |pending_clients_|,
        // |session_thread_runner_|, |initialized_|, |finalize_|, |result_|,
        // |input_ports_| and |output_ports_|.
        base::Lock lock_;

        DISALLOW_COPY_AND_ASSIGN(MidiManager);
    };

} // namespace midi
} // namespace media

#endif // MEDIA_MIDI_MIDI_MANAGER_H_
