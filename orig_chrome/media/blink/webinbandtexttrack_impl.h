// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_WEBINBANDTEXTTRACK_IMPL_H_
#define MEDIA_BLINK_WEBINBANDTEXTTRACK_IMPL_H_

#include "third_party/WebKit/public/platform/WebInbandTextTrack.h"
#include "third_party/WebKit/public/platform/WebString.h"

namespace media {

class WebInbandTextTrackImpl : public blink::WebInbandTextTrack {
public:
    WebInbandTextTrackImpl(Kind kind,
        const blink::WebString& label,
        const blink::WebString& language,
        const blink::WebString& id);
    ~WebInbandTextTrackImpl() override;

    void setClient(blink::WebInbandTextTrackClient* client) override;
    blink::WebInbandTextTrackClient* client() override;

    Kind kind() const override;

    blink::WebString label() const override;
    blink::WebString language() const override;
    blink::WebString id() const override;

private:
    blink::WebInbandTextTrackClient* client_;
    Kind kind_;
    blink::WebString label_;
    blink::WebString language_;
    blink::WebString id_;
    DISALLOW_COPY_AND_ASSIGN(WebInbandTextTrackImpl);
};

} // namespace media

#endif // MEDIA_BLINK_WEBINBANDTEXTTRACK_IMPL_H_
