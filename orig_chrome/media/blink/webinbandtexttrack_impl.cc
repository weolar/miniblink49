// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/webinbandtexttrack_impl.h"

#include "base/logging.h"

namespace media {

WebInbandTextTrackImpl::WebInbandTextTrackImpl(
    Kind kind,
    const blink::WebString& label,
    const blink::WebString& language,
    const blink::WebString& id)
    : client_(NULL)
    , kind_(kind)
    , label_(label)
    , language_(language)
    , id_(id)
{
}

WebInbandTextTrackImpl::~WebInbandTextTrackImpl()
{
    DCHECK(!client_);
}

void WebInbandTextTrackImpl::setClient(
    blink::WebInbandTextTrackClient* client)
{
    client_ = client;
}

blink::WebInbandTextTrackClient* WebInbandTextTrackImpl::client()
{
    return client_;
}

WebInbandTextTrackImpl::Kind WebInbandTextTrackImpl::kind() const
{
    return kind_;
}

blink::WebString WebInbandTextTrackImpl::label() const
{
    return label_;
}

blink::WebString WebInbandTextTrackImpl::language() const
{
    return language_;
}

blink::WebString WebInbandTextTrackImpl::id() const
{
    return id_;
}

} // namespace media
