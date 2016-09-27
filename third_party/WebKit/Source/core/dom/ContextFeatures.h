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

#ifndef ContextFeatures_h
#define ContextFeatures_h

#include "core/CoreExport.h"
#include "core/page/Page.h"
#include "platform/RefCountedSupplement.h"

namespace blink {

class ContextFeaturesClient;
class Document;
class Page;

#if ENABLE(OILPAN)
class ContextFeatures final : public GarbageCollectedFinalized<ContextFeatures>, public HeapSupplement<Page> {
    USING_GARBAGE_COLLECTED_MIXIN(ContextFeatures);
public:
    typedef HeapSupplement<Page> SupplementType;
#else
class ContextFeatures : public RefCountedSupplement<Page, ContextFeatures> {
public:
    typedef RefCountedSupplement<Page, ContextFeatures> SupplementType;
#endif
    enum FeatureType {
        PagePopup = 0,
        MutationEvents,
        FeatureTypeSize // Should be the last entry.
    };

    static const char* supplementName();
    static ContextFeatures* defaultSwitch();
    static PassRefPtrWillBeRawPtr<ContextFeatures> create(PassOwnPtr<ContextFeaturesClient>);

    static bool pagePopupEnabled(Document*);
    static bool mutationEventsEnabled(Document*);

    bool isEnabled(Document*, FeatureType, bool) const;
    void urlDidChange(Document*);

#if ENABLE(OILPAN)
    DEFINE_INLINE_VIRTUAL_TRACE() { HeapSupplement<Page>::trace(visitor); }
#endif

private:
    explicit ContextFeatures(PassOwnPtr<ContextFeaturesClient> client)
        : m_client(client)
    { }

    OwnPtr<ContextFeaturesClient> m_client;
};

class ContextFeaturesClient {
    WTF_MAKE_FAST_ALLOCATED(ContextFeaturesClient);
public:
    static PassOwnPtr<ContextFeaturesClient> empty();

    virtual ~ContextFeaturesClient() { }
    virtual bool isEnabled(Document*, ContextFeatures::FeatureType, bool defaultValue) { return defaultValue; }
    virtual void urlDidChange(Document*) { }
};

CORE_EXPORT void provideContextFeaturesTo(Page&, PassOwnPtr<ContextFeaturesClient>);
void provideContextFeaturesToDocumentFrom(Document&, Page&);

inline PassRefPtrWillBeRawPtr<ContextFeatures> ContextFeatures::create(PassOwnPtr<ContextFeaturesClient> client)
{
    return adoptRefWillBeNoop(new ContextFeatures(client));
}

inline bool ContextFeatures::isEnabled(Document* document, FeatureType type, bool defaultValue) const
{
    if (!m_client)
        return defaultValue;
    return m_client->isEnabled(document, type, defaultValue);
}

inline void ContextFeatures::urlDidChange(Document* document)
{
    // FIXME: The original code, commented out below, is obviously
    // wrong, but the seemingly correct fix of negating the test to
    // the more logical 'if (!m_client)' crashes the renderer.
    // See issue 294180
    //
    // if (m_client)
    //     return;
    // m_client->urlDidChange(document);
}

} // namespace blink

#endif // ContextFeatures_h
