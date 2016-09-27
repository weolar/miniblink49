// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPresentationAvailabilityObserver_h
#define WebPresentationAvailabilityObserver_h

namespace blink {

// WebPresentationAvailabilityObserver is an interface that is implemented by
// objects that wish to be notified when there is a presentation display
// availability change.
class WebPresentationAvailabilityObserver {
public:
    virtual void availabilityChanged(bool) = 0;
};

} // namespace blink

#endif // WebPresentationAvailabilityObserver_h
