// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_PLAYER_TRACKER_H_
#define MEDIA_BASE_PLAYER_TRACKER_H_

#include "base/basictypes.h"
#include "base/callback.h"
#include "media/base/media_export.h"

namespace media {

// An interface for players to register to be notified when a new decryption key
// becomes available or when the CDM is unset.
class MEDIA_EXPORT PlayerTracker {
public:
    virtual ~PlayerTracker();

    // Registers player callbacks with the CDM.
    // - |new_key_cb| is fired when a new decryption key becomes available.
    // - |cdm_unset_cb| is fired when the CDM is detached from the player. The
    //   player should stop using the CDM and release any ref-count to the CDM.
    // Returns a registration ID which can be used to unregister a player.
    virtual int RegisterPlayer(const base::Closure& new_key_cb,
        const base::Closure& cdm_unset_cb)
        = 0;

    // Unregisters a previously registered player. This should be called when
    // the CDM is detached from the player (e.g. setMediaKeys(0)), or when the
    // player is destroyed.
    virtual void UnregisterPlayer(int registration_id) = 0;

protected:
    PlayerTracker();

private:
    DISALLOW_COPY_AND_ASSIGN(PlayerTracker);
};

} // namespace media

#endif // MEDIA_BASE_PLAYER_TRACKER_H_
