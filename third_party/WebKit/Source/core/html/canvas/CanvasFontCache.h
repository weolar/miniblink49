// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CanvasFontCache_h
#define CanvasFontCache_h

#include "core/CoreExport.h"
#include "core/css/StylePropertySet.h"
#include "platform/fonts/Font.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebThread.h"
#include "wtf/HashMap.h"
#include "wtf/ListHashSet.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Document;
class FontCachePurgePreventer;

class CORE_EXPORT CanvasFontCache final : public GarbageCollectedFinalized<CanvasFontCache>, public WebThread::TaskObserver {
public:
    static CanvasFontCache* create(Document& document)
    {
        return new CanvasFontCache(document);
    }

    MutableStylePropertySet* parseFont(const String&);
    void pruneAll();
    unsigned size();

    DECLARE_VIRTUAL_TRACE();

    static unsigned maxFonts();
    unsigned hardMaxFonts();

    void willUseCurrentFont() { schedulePruningIfNeeded(); }
    bool getFontUsingDefaultStyle(const String&, Font&);

    // TaskObserver implementation
    virtual void didProcessTask();
    virtual void willProcessTask() { }

    // For testing
    bool isInCache(const String&);

    ~CanvasFontCache();

private:
    explicit CanvasFontCache(Document&);
    void schedulePruningIfNeeded();
    typedef WillBeHeapHashMap<String, RefPtrWillBeMember<MutableStylePropertySet>> MutableStylePropertyMap;

    HashMap<String, Font> m_fontsResolvedUsingDefaultStyle;
    MutableStylePropertyMap m_fetchedFonts;
    ListHashSet<String> m_fontLRUList;
    OwnPtr<FontCachePurgePreventer> m_mainCachePurgePreventer;
    RawPtrWillBeMember<Document> m_document;
    RefPtr<ComputedStyle> m_defaultFontStyle;
    bool m_pruningScheduled;
};

} // blink

#endif
