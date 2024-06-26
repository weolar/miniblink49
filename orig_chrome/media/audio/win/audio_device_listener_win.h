// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_WIN_AUDIO_DEVICE_LISTENER_WIN_H_
#define MEDIA_AUDIO_WIN_AUDIO_DEVICE_LISTENER_WIN_H_

#include <MMDeviceAPI.h>
#include <string>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "base/win/scoped_comptr.h"
#include "media/base/media_export.h"

using base::win::ScopedComPtr;

namespace base {
class TickClock;
}

namespace media {

// IMMNotificationClient implementation for listening for default device changes
// and forwarding to AudioManagerWin so it can notify downstream clients.  Only
// output (eRender) device changes are supported currently.  Core Audio support
// is required to construct this object.  Must be constructed and destructed on
// a single COM initialized thread.
// TODO(dalecurtis, henrika): Support input device changes.
class MEDIA_EXPORT AudioDeviceListenerWin : public IMMNotificationClient {
public:
    // The listener callback will be called from a system level multimedia thread,
    // thus the callee must be thread safe.  |listener| is a permanent callback
    // and must outlive AudioDeviceListenerWin.
    explicit AudioDeviceListenerWin(const base::Closure& listener_cb);
    virtual ~AudioDeviceListenerWin();

private:
    friend class AudioDeviceListenerWinTest;

    // Minimum allowed time between device change notifications.
    static const int kDeviceChangeLimitMs = 250;

    // IMMNotificationClient implementation.
    STDMETHOD_(ULONG, AddRef)
    () override;
    STDMETHOD_(ULONG, Release)
    () override;
    STDMETHOD(QueryInterface)
    (REFIID iid, void** object) override;
    STDMETHOD(OnPropertyValueChanged)
    (LPCWSTR device_id,
        const PROPERTYKEY key) override;
    STDMETHOD(OnDeviceAdded)
    (LPCWSTR device_id) override;
    STDMETHOD(OnDeviceRemoved)
    (LPCWSTR device_id) override;
    STDMETHOD(OnDeviceStateChanged)
    (LPCWSTR device_id, DWORD new_state) override;
    STDMETHOD(OnDefaultDeviceChanged)
    (EDataFlow flow,
        ERole role,
        LPCWSTR new_default_device_id) override;

    base::Closure listener_cb_;
    ScopedComPtr<IMMDeviceEnumerator> device_enumerator_;

    // Used to rate limit device change events.
    base::TimeTicks last_device_change_time_;

    // AudioDeviceListenerWin must be constructed and destructed on one thread.
    base::ThreadChecker thread_checker_;

    scoped_ptr<base::TickClock> tick_clock_;

    DISALLOW_COPY_AND_ASSIGN(AudioDeviceListenerWin);
};

} // namespace media

#endif // MEDIA_AUDIO_WIN_AUDIO_DEVICE_LISTENER_WIN_H_
