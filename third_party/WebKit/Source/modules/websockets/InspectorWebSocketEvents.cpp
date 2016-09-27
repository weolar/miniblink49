// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/websockets/InspectorWebSocketEvents.h"

#include "core/dom/Document.h"
#include "platform/TracedValue.h"
#include "platform/weborigin/KURL.h"

namespace blink {

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorWebSocketCreateEvent::data(Document* document, unsigned long identifier, const KURL& url, const String& protocol)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setInteger("identifier", identifier);
    value->setString("url", url.string());
    value->setString("frame", toHexString(document->frame()));
    if (!protocol.isNull())
        value->setString("webSocketProtocol", protocol);
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorWebSocketEvent::data(Document* document, unsigned long identifier)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setInteger("identifier", identifier);
    value->setString("frame", toHexString(document->frame()));
    setCallStack(value.get());
    return value.release();
}

} // namespace blink
