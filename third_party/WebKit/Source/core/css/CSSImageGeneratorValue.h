/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CSSImageGeneratorValue_h
#define CSSImageGeneratorValue_h

#include "core/CoreExport.h"
#include "core/css/CSSValue.h"
#include "platform/geometry/IntSizeHash.h"
#include "wtf/HashCountedSet.h"
#include "wtf/RefPtr.h"

namespace blink {

class Document;
class Image;
class LayoutObject;

struct SizeAndCount {
    SizeAndCount(IntSize newSize = IntSize(), int newCount = 0)
        : size(newSize)
        , count(newCount)
    {
    }

    IntSize size;
    int count;
};

typedef HashMap<const LayoutObject*, SizeAndCount> LayoutObjectSizeCountMap;

class CORE_EXPORT CSSImageGeneratorValue : public CSSValue {
public:
    ~CSSImageGeneratorValue();

    void addClient(LayoutObject*, const IntSize&);
    void removeClient(LayoutObject*);
    PassRefPtr<Image> image(LayoutObject*, const IntSize&);

    bool isFixedSize() const;
    IntSize fixedSize(const LayoutObject*);

    bool isPending() const;
    bool knownToBeOpaque(const LayoutObject*) const;

    void loadSubimages(Document*);

    DEFINE_INLINE_TRACE_AFTER_DISPATCH() { CSSValue::traceAfterDispatch(visitor); }

protected:
    explicit CSSImageGeneratorValue(ClassType);

    Image* getImage(LayoutObject*, const IntSize&);
    void putImage(const IntSize&, PassRefPtr<Image>);
    const LayoutObjectSizeCountMap& clients() const { return m_clients; }

    HashCountedSet<IntSize> m_sizes; // A count of how many times a given image size is in use.
    LayoutObjectSizeCountMap m_clients; // A map from LayoutObjects (with entry count) to image sizes.
    HashMap<IntSize, RefPtr<Image>> m_images; // A cache of Image objects by image size.

#if ENABLE(OILPAN)
    // FIXME: Oilpan: when/if we can make the layoutObject point directly to the CSSImageGenerator value using
    // a member we don't need to have this hack where we keep a persistent to the instance as long as
    // there are clients in the LayoutObjectSizeCountMap.
    GC_PLUGIN_IGNORE("366546")
    OwnPtr<Persistent<CSSImageGeneratorValue>> m_keepAlive;
#endif
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSImageGeneratorValue, isImageGeneratorValue());

} // namespace blink

#endif // CSSImageGeneratorValue_h
