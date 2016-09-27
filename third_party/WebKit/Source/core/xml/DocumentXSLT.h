// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DocumentXSLT_h
#define DocumentXSLT_h

#include "core/dom/Document.h"
#include "platform/heap/Handle.h"
#include "wtf/RefPtr.h"

namespace blink {

class Document;
class ProcessingInstruction;

class DocumentXSLT final : public NoBaseWillBeGarbageCollected<DocumentXSLT>, public WillBeHeapSupplement<Document> {
    WTF_MAKE_NONCOPYABLE(DocumentXSLT);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DocumentXSLT);
public:
    Document* transformSourceDocument()
    {
        return m_transformSourceDocument.get();
    }

    void setTransformSourceDocument(Document* document)
    {
        ASSERT(document);
        m_transformSourceDocument = document;
    }

    static DocumentXSLT& from(WillBeHeapSupplementable<Document>&);
    static const char* supplementName();

    // The following static methods don't use any instance of DocumentXSLT.
    // They are just using DocumentXSLT namespace.
    static void applyXSLTransform(Document&, ProcessingInstruction*);
    static ProcessingInstruction* findXSLStyleSheet(Document&);
    static bool processingInstructionInsertedIntoDocument(Document&, ProcessingInstruction*);
    static bool processingInstructionRemovedFromDocument(Document&, ProcessingInstruction*);
    static bool sheetLoaded(Document&, ProcessingInstruction*);
    static bool hasTransformSourceDocument(Document&);

    DECLARE_VIRTUAL_TRACE();

private:
    DocumentXSLT();

    RefPtrWillBeMember<Document> m_transformSourceDocument;
};

} // namespace blink

#endif
