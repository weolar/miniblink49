// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "InternalsNavigatorContentUtils.h"

#include "core/dom/Document.h"
#include "core/testing/Internals.h"
#include "modules/navigatorcontentutils/NavigatorContentUtils.h"
#include "modules/navigatorcontentutils/testing/NavigatorContentUtilsClientMock.h"

namespace blink {

void InternalsNavigatorContentUtils::setNavigatorContentUtilsClientMock(Internals&, Document* document)
{
    ASSERT(document && document->page());
    NavigatorContentUtils* navigatorContentUtils = NavigatorContentUtils::from(*document->frame());
    navigatorContentUtils->setClientForTest(adoptPtr(new NavigatorContentUtilsClientMock()));
}

} // namespace blink
