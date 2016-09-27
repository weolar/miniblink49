/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/dom/ContextFeatures.h"

#include "core/dom/Document.h"
#include "core/page/Page.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

PassOwnPtr<ContextFeaturesClient> ContextFeaturesClient::empty()
{
    return adoptPtr(new ContextFeaturesClient());
}

const char* ContextFeatures::supplementName()
{
    return "ContextFeatures";
}

ContextFeatures* ContextFeatures::defaultSwitch()
{
    DEFINE_STATIC_REF_WILL_BE_PERSISTENT(ContextFeatures, instance, (ContextFeatures::create(ContextFeaturesClient::empty())));
    return instance;
}

bool ContextFeatures::pagePopupEnabled(Document* document)
{
    if (!document)
        return false;
    return document->contextFeatures().isEnabled(document, PagePopup, false);
}

bool ContextFeatures::mutationEventsEnabled(Document* document)
{
    ASSERT(document);
    if (!document)
        return true;
    return document->contextFeatures().isEnabled(document, MutationEvents, true);
}

void provideContextFeaturesTo(Page& page, PassOwnPtr<ContextFeaturesClient> client)
{
    ContextFeatures::SupplementType::provideTo(page, ContextFeatures::supplementName(), ContextFeatures::create(client));
}

void provideContextFeaturesToDocumentFrom(Document& document, Page& page)
{
    ContextFeatures* provided = static_cast<ContextFeatures*>(ContextFeatures::SupplementType::from(page, ContextFeatures::supplementName()));
    if (!provided)
        return;
    document.setContextFeatures(*provided);
}

} // namespace blink
