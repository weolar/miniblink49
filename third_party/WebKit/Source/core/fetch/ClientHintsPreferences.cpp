// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/fetch/ClientHintsPreferences.h"

#include "core/fetch/ResourceFetcher.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/network/HTTPParsers.h"

namespace blink {

void handleAcceptClientHintsHeader(const String& headerValue, ClientHintsPreferences& preferences, ResourceFetcher* fetcher)
{
    if (!RuntimeEnabledFeatures::clientHintsEnabled() || headerValue.isEmpty())
        return;
    CommaDelimitedHeaderSet acceptCH;
    parseCommaDelimitedHeader(headerValue, acceptCH);
    if (acceptCH.contains("dpr")) {
        if (fetcher)
            fetcher->context().countClientHintsDPR();
        preferences.setShouldSendDPR(true);
    }

    if (acceptCH.contains("width")) {
        if (fetcher)
            fetcher->context().countClientHintsResourceWidth();
        preferences.setShouldSendResourceWidth(true);
    }

    if (acceptCH.contains("viewport-width")) {
        if (fetcher)
            fetcher->context().countClientHintsViewportWidth();
        preferences.setShouldSendViewportWidth(true);
    }
}

}
