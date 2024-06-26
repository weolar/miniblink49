// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_INPUT_IPC_H_
#define MEDIA_AUDIO_AUDIO_INPUT_IPC_H_

#include "base/memory/shared_memory.h"
#include "base/sync_socket.h"
#include "media/audio/audio_parameters.h"
#include "media/base/media_export.h"

namespace media {

enum AudioInputIPCDelegateState {
    AUDIO_INPUT_IPC_DELEGATE_STATE_RECORDING,
    AUDIO_INPUT_IPC_DELEGATE_STATE_STOPPED,
    AUDIO_INPUT_IPC_DELEGATE_STATE_ERROR,
    AUDIO_INPUT_IPC_DELEGATE_STATE_LAST = AUDIO_INPUT_IPC_DELEGATE_STATE_ERROR,
};

// Contains IPC notifications for the state of the server side
// (AudioInputController) audio state changes and when an AudioInputController
// has been created.  Implemented by AudioInputDevice.
class MEDIA_EXPORT AudioInputIPCDelegate {
public:
    // Called when an AudioInputController has been created.
    // The shared memory |handle| points to a memory section that's used to
    // transfer data between the AudioInputDevice and AudioInputController
    // objects.  The implementation of OnStreamCreated takes ownership.
    // The |socket_handle| is used by the AudioInputController to signal
    // notifications that more data is available and can optionally provide
    // parameter changes back.  The AudioInputDevice must read from this socket
    // and process the shared memory whenever data is read from the socket.
    virtual void OnStreamCreated(base::SharedMemoryHandle handle,
        base::SyncSocket::Handle socket_handle,
        int length,
        int total_segments)
        = 0;

    // Called when state of an audio stream has changed.
    virtual void OnStateChanged(AudioInputIPCDelegateState state) = 0;

    // Called when the input stream volume has changed.
    virtual void OnVolume(double volume) = 0;

    // Called when the AudioInputIPC object is going away and/or when the
    // IPC channel has been closed and no more IPC requests can be made.
    // Implementations should delete their owned AudioInputIPC instance
    // immediately.
    virtual void OnIPCClosed() = 0;

protected:
    virtual ~AudioInputIPCDelegate();
};

// Provides IPC functionality for an AudioInputIPCDelegate (e.g., an
// AudioInputDevice).  The implementation should asynchronously deliver the
// messages to an AudioInputController object (or create one in the case of
// CreateStream()), that may live in a separate process.
class MEDIA_EXPORT AudioInputIPC {
public:
    virtual ~AudioInputIPC();

    // Sends a request to create an AudioInputController object in the peer
    // process, and configures it to use the specified audio |params|.  The
    // |total_segments| indidates number of equal-lengthed segments in the shared
    // memory buffer.  Once the stream has been created, the implementation will
    // notify |delegate| by calling OnStreamCreated().
    virtual void CreateStream(AudioInputIPCDelegate* delegate,
        int session_id,
        const AudioParameters& params,
        bool automatic_gain_control,
        uint32 total_segments)
        = 0;

    // Corresponds to a call to AudioInputController::Record() on the server side.
    virtual void RecordStream() = 0;

    // Sets the volume of the audio stream.
    virtual void SetVolume(double volume) = 0;

    // Closes the audio stream, which should shut down the corresponding
    // AudioInputController in the peer process.
    virtual void CloseStream() = 0;
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_INPUT_IPC_H_
