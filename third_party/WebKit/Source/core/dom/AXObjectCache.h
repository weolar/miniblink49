/*
 * Copyright (C) 2003, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
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

#ifndef AXObjectCache_h
#define AXObjectCache_h

#include "core/CoreExport.h"
#include "core/dom/Document.h"

typedef unsigned AXID;

namespace blink {

class AbstractInlineTextBox;
class AXObject;
class FrameView;
class HTMLOptionElement;
class HTMLSelectElement;
class LayoutMenuList;
class Page;
class Widget;

class CORE_EXPORT AXObjectCache : public NoBaseWillBeGarbageCollectedFinalized<AXObjectCache> {
    WTF_MAKE_NONCOPYABLE(AXObjectCache);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(AXObjectCache);
public:
    static PassOwnPtrWillBeRawPtr<AXObjectCache> create(Document&);

    static AXObject* focusedUIElementForPage(const Page*);

    virtual ~AXObjectCache();
    DEFINE_INLINE_VIRTUAL_TRACE() { }

    enum AXNotification {
        AXActiveDescendantChanged,
        AXAlert,
        AXAriaAttributeChanged,
        AXAutocorrectionOccured,
        AXBlur,
        AXCheckedStateChanged,
        AXChildrenChanged,
        AXFocusedUIElementChanged,
        AXHide,
        AXInvalidStatusChanged,
        AXLayoutComplete,
        AXLiveRegionChanged,
        AXLoadComplete,
        AXLocationChanged,
        AXMenuListItemSelected,
        AXMenuListItemUnselected,
        AXMenuListValueChanged,
        AXRowCollapsed,
        AXRowCountChanged,
        AXRowExpanded,
        AXScrollPositionChanged,
        AXScrolledToAnchor,
        AXSelectedChildrenChanged,
        AXSelectedTextChanged,
        AXShow,
        AXTextChanged,
        AXTextInserted,
        AXTextRemoved,
        AXValueChanged
    };

    virtual void dispose() = 0;

    virtual void selectionChanged(Node*) = 0;
    virtual void childrenChanged(Node*) = 0;
    virtual void childrenChanged(LayoutObject*) = 0;
    virtual void checkedStateChanged(Node*) = 0;
    virtual void listboxOptionStateChanged(HTMLOptionElement*) = 0;
    virtual void listboxSelectedChildrenChanged(HTMLSelectElement*) = 0;
    virtual void listboxActiveIndexChanged(HTMLSelectElement*) = 0;

    virtual void remove(LayoutObject*) = 0;
    virtual void remove(Node*) = 0;
    virtual void remove(Widget*) = 0;
    virtual void remove(AbstractInlineTextBox*) = 0;

    virtual const Element* rootAXEditableElement(const Node*) = 0;

    // Called by a node when text or a text equivalent (e.g. alt) attribute is changed.
    virtual void textChanged(LayoutObject*) = 0;
    // Called when a node has just been attached, so we can make sure we have the right subclass of AXObject.
    virtual void updateCacheAfterNodeIsAttached(Node*) = 0;

    virtual void handleAttributeChanged(const QualifiedName& attrName, Element*) = 0;
    virtual void handleFocusedUIElementChanged(Node* oldFocusedNode, Node* newFocusedNode) = 0;
    virtual void handleInitialFocus() = 0;
    virtual void handleEditableTextContentChanged(Node*) = 0;
    virtual void handleTextFormControlChanged(Node*) = 0;
    virtual void handleValueChanged(Node*) = 0;
    virtual void handleUpdateActiveMenuOption(LayoutMenuList*, int optionIndex) = 0;
    virtual void didShowMenuListPopup(LayoutMenuList*) = 0;
    virtual void didHideMenuListPopup(LayoutMenuList*) = 0;
    virtual void handleLoadComplete(Document*) = 0;
    virtual void handleLayoutComplete(Document*) = 0;


    virtual void setCanvasObjectBounds(Element*, const LayoutRect&) = 0;

    virtual void inlineTextBoxesUpdated(LayoutObject*) = 0;

    // Called when the scroll offset changes.
    virtual void handleScrollPositionChanged(FrameView*) = 0;
    virtual void handleScrollPositionChanged(LayoutObject*) = 0;

    // Called when scroll bars are added / removed (as the view resizes).
    virtual void handleScrollbarUpdate(FrameView*) = 0;
    virtual void handleLayoutComplete(LayoutObject*) = 0;
    virtual void handleScrolledToAnchor(const Node* anchorNode) = 0;

    virtual const AtomicString& computedRoleForNode(Node*) = 0;
    virtual String computedNameForNode(Node*) = 0;

    typedef PassOwnPtrWillBeRawPtr<AXObjectCache> (*AXObjectCacheCreateFunction)(Document&);
    static void init(AXObjectCacheCreateFunction);

protected:
    AXObjectCache();

private:
    static AXObjectCacheCreateFunction m_createFunction;
};

class CORE_EXPORT ScopedAXObjectCache {
    WTF_MAKE_NONCOPYABLE(ScopedAXObjectCache);
public:
    static PassOwnPtr<ScopedAXObjectCache> create(Document&);
    ~ScopedAXObjectCache();

    AXObjectCache* get();

private:
    explicit ScopedAXObjectCache(Document&);

    RefPtrWillBePersistent<Document> m_document;
    OwnPtrWillBePersistent<AXObjectCache> m_cache;
};

}

#endif
