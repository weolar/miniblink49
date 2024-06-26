// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/active_loader.h"

#include "media/blink/buffered_resource_loader.h"
#include "third_party/WebKit/public/platform/WebURLLoader.h"

namespace media {

ActiveLoader::ActiveLoader(scoped_ptr<blink::WebURLLoader> loader)
    : loader_(loader.Pass())
    , deferred_(false)
{
}

ActiveLoader::~ActiveLoader()
{
    loader_->cancel();
}

void ActiveLoader::SetDeferred(bool deferred)
{
    deferred_ = deferred;
    loader_->setDefersLoading(deferred);
}

} // namespace media
