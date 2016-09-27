// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/parser/ResourcePreloader.h"

namespace blink {

void ResourcePreloader::takeAndPreload(PreloadRequestStream& r)
{
    PreloadRequestStream requests;
    requests.swap(r);

    for (PreloadRequestStream::iterator it = requests.begin(); it != requests.end(); ++it)
        preload(it->release());
}

}
