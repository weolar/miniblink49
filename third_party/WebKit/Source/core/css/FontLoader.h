// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FontLoader_h
#define FontLoader_h

#include "core/fetch/ResourceLoader.h"
#include "core/fetch/ResourcePtr.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "wtf/Vector.h"

namespace blink {

class CSSFontSelector;
class Document;
class FontResource;

class FontLoader : public RefCountedWillBeGarbageCollectedFinalized<FontLoader> {
public:
    static PassRefPtrWillBeRawPtr<FontLoader> create(CSSFontSelector* fontSelector, Document* document)
    {
        return adoptRefWillBeNoop(new FontLoader(fontSelector, document));
    }
    ~FontLoader();

    void addFontToBeginLoading(FontResource*);
    void loadPendingFonts();
    void fontFaceInvalidated();
    void didFailToDecode(FontResource*);

#if !ENABLE(OILPAN)
    void clearDocumentAndFontSelector();
#endif

    DECLARE_TRACE();

private:
    FontLoader(CSSFontSelector*, Document*);
    void beginLoadTimerFired(Timer<FontLoader>*);
    void clearPendingFonts();

    Timer<FontLoader> m_beginLoadingTimer;

    struct FontToLoad;
    typedef Vector<OwnPtr<FontToLoad>> FontsToLoadVector;
    FontsToLoadVector m_fontsToBeginLoading;
    RawPtrWillBeMember<CSSFontSelector> m_fontSelector;
    RawPtrWillBeWeakMember<Document> m_document;
};

} // namespace blink

#endif // FontLoader_h
