// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/LinkManifest.h"

#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLLinkElement.h"
#include "core/loader/FrameLoaderClient.h"

namespace blink {

PassOwnPtrWillBeRawPtr<LinkManifest> LinkManifest::create(HTMLLinkElement* owner)
{
    return adoptPtrWillBeNoop(new LinkManifest(owner));
}

LinkManifest::LinkManifest(HTMLLinkElement* owner)
    : LinkResource(owner)
{
}

LinkManifest::~LinkManifest()
{
}

void LinkManifest::process()
{
    if (!m_owner || !m_owner->document().frame())
        return;

    m_owner->document().frame()->loader().client()->dispatchDidChangeManifest();
}

bool LinkManifest::hasLoaded() const
{
    return false;
}

void LinkManifest::ownerRemoved()
{
    process();
}

} // namespace blink
