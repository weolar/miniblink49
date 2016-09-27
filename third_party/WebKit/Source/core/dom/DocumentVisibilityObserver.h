// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DocumentVisibilityObserver_h
#define DocumentVisibilityObserver_h

#include "core/CoreExport.h"
#include "core/page/PageVisibilityState.h"
#include "platform/heap/Handle.h"

namespace blink {

class Document;

class CORE_EXPORT DocumentVisibilityObserver : public WillBeGarbageCollectedMixin {
    WTF_MAKE_NONCOPYABLE(DocumentVisibilityObserver);
public:
    explicit DocumentVisibilityObserver(Document&);
    virtual ~DocumentVisibilityObserver();

    virtual void didChangeVisibilityState(PageVisibilityState) = 0;

    // Classes that inherit Node and DocumentVisibilityObserver must have a
    // virtual override of Node::didMoveToNewDocument that calls
    // DocumentVisibilityObserver::setDocument
    void setObservedDocument(Document&);

protected:
    DECLARE_VIRTUAL_TRACE();

private:
    void registerObserver(Document&);
    void unregisterObserver();

    RawPtrWillBeMember<Document> m_document;
};

} // namespace blink

#endif // DocumentVisibilityObserver_h
