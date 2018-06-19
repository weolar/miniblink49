/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
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

#ifndef MHTMLArchive_h
#define MHTMLArchive_h

#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

namespace blink {

class ArchiveResource;
class KURL;
class MHTMLParser;
class SharedBuffer;

struct SerializedResource;

class PLATFORM_EXPORT MHTMLArchive final : public RefCountedWillBeGarbageCollectedFinalized<MHTMLArchive> {
public:
    static PassRefPtrWillBeRawPtr<MHTMLArchive> create();
    static PassRefPtrWillBeRawPtr<MHTMLArchive> create(const KURL&, SharedBuffer*);
    ~MHTMLArchive();

    // Binary encoding results in smaller MHTML files but they might not work in other browsers.
    enum EncodingPolicy {
        UseDefaultEncoding,
        UseBinaryEncoding
    };

    // Generates a random/unique boundary that can be used as a separator of
    // MHTML parts.
    static String generateMHTMLBoundary();

    // Generates an MHTML header and appends it to |outputBuffer|.
    //
    // Same |boundary| needs to used for all generateMHTMLHeader and
    // generateMHTMLPart and generateMHTMLFooter calls that belong to the same
    // MHTML document (see also generateMHTMLBoundary method).
    static void generateMHTMLHeader(
        const String& boundary, const String& title, const String& mimeType,
        SharedBuffer& outputBuffer);

    // Serializes SerializedResource as an MHTML part and appends it in
    // |outputBuffer|.
    //
    // Same |boundary| needs to used for all generateMHTMLHeader and
    // generateMHTMLPart and generateMHTMLFooter calls that belong to the same
    // MHTML document (see also generateMHTMLBoundary method).
    static void generateMHTMLPart(
        const String& boundary, EncodingPolicy, const SerializedResource&,
        SharedBuffer& outputBuffer);

    // Generates an MHTML footer and appends it to |outputBuffer|.
    //
    // Same |boundary| needs to used for all generateMHTMLHeader and
    // generateMHTMLPart and generateMHTMLFooter calls that belong to the same
    // MHTML document (see also generateMHTMLBoundary method).
    static void generateMHTMLFooter(
        const String& boundary,
        SharedBuffer& outputBuffer);

    // Generates and returns a full MHTML document.
    static PassRefPtr<SharedBuffer> generateMHTMLData(
        const Vector<SerializedResource>&, EncodingPolicy,
        const String& title, const String& mimeType);

    typedef WillBeHeapVector<RefPtrWillBeMember<ArchiveResource>> SubArchiveResources;
    typedef WillBeHeapVector<RefPtrWillBeMember<MHTMLArchive>> SubFrameArchives;

    ArchiveResource* mainResource() { return m_mainResource.get(); }
    const SubArchiveResources& subresources() const { return m_subresources; }
    const SubFrameArchives& subframeArchives() const { return m_subframeArchives; }

    DECLARE_TRACE();

private:
    friend class MHTMLParser;
    MHTMLArchive();

    void setMainResource(PassRefPtrWillBeRawPtr<ArchiveResource>);
    void addSubresource(PassRefPtrWillBeRawPtr<ArchiveResource>);
    void addSubframeArchive(PassRefPtrWillBeRawPtr<MHTMLArchive>);

#if !ENABLE(OILPAN)
    void clearAllSubframeArchives();
    void clearAllSubframeArchivesImpl(SubFrameArchives* clearedArchives);
#endif

    RefPtrWillBeMember<ArchiveResource> m_mainResource;
    SubArchiveResources m_subresources;
    SubFrameArchives m_subframeArchives;
};

}

#endif
