/*
 * Copyright (C) 2014, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AXObjectCacheImpl_h
#define AXObjectCacheImpl_h

#include "core/dom/AXObjectCache.h"
#include "core/layout/LayoutText.h"
#include "modules/ModulesExport.h"
#include "modules/accessibility/AXObject.h"
#include "platform/Timer.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/RefPtr.h"

namespace blink {

class AbstractInlineTextBox;
class HTMLAreaElement;
class FrameView;
class Widget;

struct TextMarkerData {
    AXID axID;
    Node* node;
    int offset;
    EAffinity affinity;
};

// This class should only be used from inside the accessibility directory.
class MODULES_EXPORT AXObjectCacheImpl : public AXObjectCache {
    WTF_MAKE_NONCOPYABLE(AXObjectCacheImpl);
public:
    static PassOwnPtrWillBeRawPtr<AXObjectCache> create(Document&);

    explicit AXObjectCacheImpl(Document&);
    ~AXObjectCacheImpl();
    DECLARE_VIRTUAL_TRACE();

    AXObject* focusedUIElementForPage(const Page*);

    void dispose() override;

    void selectionChanged(Node*) override;
    void childrenChanged(Node*) override;
    void childrenChanged(LayoutObject*) override;
    void checkedStateChanged(Node*) override;
    virtual void listboxOptionStateChanged(HTMLOptionElement*);
    virtual void listboxSelectedChildrenChanged(HTMLSelectElement*);
    virtual void listboxActiveIndexChanged(HTMLSelectElement*);

    void remove(LayoutObject*) override;
    void remove(Node*) override;
    void remove(Widget*) override;
    void remove(AbstractInlineTextBox*) override;

    const Element* rootAXEditableElement(const Node*) override;

    // Called by a node when text or a text equivalent (e.g. alt) attribute is changed.
    void textChanged(LayoutObject*) override;
    // Called when a node has just been attached, so we can make sure we have the right subclass of AXObject.
    void updateCacheAfterNodeIsAttached(Node*) override;

    void handleAttributeChanged(const QualifiedName& attrName, Element*) override;
    void handleFocusedUIElementChanged(Node* oldFocusedNode, Node* newFocusedNode) override;
    void handleInitialFocus() override;
    void handleTextFormControlChanged(Node*) override;
    void handleEditableTextContentChanged(Node*) override;
    void handleValueChanged(Node*) override;
    void handleUpdateActiveMenuOption(LayoutMenuList*, int optionIndex) override;
    void didShowMenuListPopup(LayoutMenuList*) override;
    void didHideMenuListPopup(LayoutMenuList*) override;
    void handleLoadComplete(Document*) override;
    void handleLayoutComplete(Document*) override;

    void setCanvasObjectBounds(Element*, const LayoutRect&) override;

    void inlineTextBoxesUpdated(LayoutObject*) override;

    // Called when the scroll offset changes.
    void handleScrollPositionChanged(FrameView*) override;
    void handleScrollPositionChanged(LayoutObject*) override;

    // Called when scroll bars are added / removed (as the view resizes).
    void handleScrollbarUpdate(FrameView*) override;
    void handleLayoutComplete(LayoutObject*) override;
    void handleScrolledToAnchor(const Node* anchorNode) override;

    const AtomicString& computedRoleForNode(Node*) override;
    String computedNameForNode(Node*) override;

    // Returns the root object for the entire document.
    AXObject* rootObject();

    AXObject* objectFromAXID(AXID id) const { return m_objects.get(id); }
    AXObject* root();

    // used for objects without backing elements
    AXObject* getOrCreate(AccessibilityRole);
    AXObject* getOrCreate(LayoutObject*);
    AXObject* getOrCreate(Widget*);
    AXObject* getOrCreate(Node*);
    AXObject* getOrCreate(AbstractInlineTextBox*);

    // will only return the AXObject if it already exists
    AXObject* get(Node*);
    AXObject* get(LayoutObject*);
    AXObject* get(Widget*);
    AXObject* get(AbstractInlineTextBox*);

    AXObject* firstAccessibleObjectFromNode(const Node*);

    void remove(AXID);

    void childrenChanged(AXObject*);

    void handleActiveDescendantChanged(Node*);
    void handleAriaRoleChanged(Node*);
    void handleAriaExpandedChange(Node*);
    void handleAriaSelectedChanged(Node*);

    void recomputeIsIgnored(LayoutObject*);

    bool accessibilityEnabled();
    bool inlineTextBoxAccessibilityEnabled();

    void removeAXID(AXObject*);
    bool isIDinUse(AXID id) const { return m_idsInUse.contains(id); }

    AXID platformGenerateAXID() const;

    // Counts the number of times the document has been modified. Some attribute values are cached
    // as long as the modification count hasn't changed.
    int modificationCount() const { return m_modificationCount; }

    void postNotification(LayoutObject*, AXNotification);
    void postNotification(Node*, AXNotification);
    void postNotification(AXObject*, AXNotification);

    //
    // Aria-owns support.
    //

    // Returns true if the given object's position in the tree was due to aria-owns.
    bool isAriaOwned(const AXObject*) const;

    // Returns the parent of the given object due to aria-owns.
    AXObject* getAriaOwnedParent(const AXObject*) const;

    // Given an object that has an aria-owns attributes, and a vector of ids from the value of
    // that attribute, updates the internal state to reflect the new set of children owned by
    // this object, returning the result in |ownedChildren|. The result is validated - illegal,
    // duplicate, or cyclical references have been removed.
    //
    // If one or more ids aren't found, they're added to a lookup table so that if an
    // element with that id appears later, it can be added when you call updateTreeIfElementIdIsAriaOwned.
    void updateAriaOwns(const AXObject* owner, const Vector<String>& idVector, Vector<AXObject*>& ownedChildren);

    // Given an element in the DOM tree that was either just added or whose id just changed,
    // check to see if another object wants to be its parent due to aria-owns. If so, update
    // the tree by calling childrenChanged() on the potential owner, possibly reparenting
    // this element.
    void updateTreeIfElementIdIsAriaOwned(Element*);

protected:
    void postPlatformNotification(AXObject*, AXNotification);
    void textChanged(AXObject*);
    void labelChanged(Element*);

    // This is a weak reference cache for knowing if Nodes used by TextMarkers are valid.
    void setNodeInUse(Node* n) { m_textMarkerNodes.add(n); }
    void removeNodeForUse(Node* n) { m_textMarkerNodes.remove(n); }
    bool isNodeInUse(Node* n) { return m_textMarkerNodes.contains(n); }

    PassRefPtrWillBeRawPtr<AXObject> createFromRenderer(LayoutObject*);
    PassRefPtrWillBeRawPtr<AXObject> createFromNode(Node*);
    PassRefPtrWillBeRawPtr<AXObject> createFromInlineTextBox(AbstractInlineTextBox*);

private:

    RawPtrWillBeMember<Document> m_document;
    WillBeHeapHashMap<AXID, RefPtrWillBeMember<AXObject>> m_objects;
    // LayoutObject and AbstractInlineTextBox are not on the Oilpan heap so we
    // do not use HeapHashMap for those mappings.
    HashMap<LayoutObject*, AXID> m_layoutObjectMapping;
    WillBeHeapHashMap<RawPtrWillBeMember<Widget>, AXID> m_widgetObjectMapping;
    WillBeHeapHashMap<RawPtrWillBeMember<Node>, AXID> m_nodeObjectMapping;
    HashMap<AbstractInlineTextBox*, AXID> m_inlineTextBoxObjectMapping;
    WillBeHeapHashSet<RawPtrWillBeMember<Node>> m_textMarkerNodes;
    int m_modificationCount;

    HashSet<AXID> m_idsInUse;

#if ENABLE(ASSERT)
    // Verified when finalizing.
    bool m_hasBeenDisposed;
#endif

    //
    // Aria-owns
    //

    // Map from the AXID of the owner to the AXIDs of the children.
    // This is a validated map, it doesn't contain illegal, duplicate,
    // or cyclical matches, or references to IDs that don't exist.
    HashMap<AXID, Vector<AXID>> m_ariaOwnerToChildrenMapping;

    // Map from the AXID of a child to the AXID of the parent that owns it.
    HashMap<AXID, AXID> m_ariaOwnedChildToOwnerMapping;

    // Map from the AXID of a child to the AXID of its real parent in the tree if
    // we ignored aria-owns. This is needed in case the owner no longer wants to own it.
    HashMap<AXID, AXID> m_ariaOwnedChildToRealParentMapping;

    // Map from the AXID of any object with an aria-owns attribute to the set of ids
    // of its children. This is *unvalidated*, it includes ids that may not currently
    // exist in the tree.
    HashMap<AXID, HashSet<String>> m_ariaOwnerToIdsMapping;

    // Map from an ID (the ID attribute of a DOM element) to the set of elements that
    // want to own that ID. This is *unvalidated*, it includes possible duplicates.
    // This is used so that when an element with an ID is added to the tree or changes
    // its ID, we can quickly determine if it affects an aria-owns relationship.
    HashMap<String, OwnPtr<HashSet<AXID>>> m_idToAriaOwnersMapping;

    Timer<AXObjectCacheImpl> m_notificationPostTimer;
    WillBeHeapVector<pair<RefPtrWillBeMember<AXObject>, AXNotification>> m_notificationsToPost;
    void notificationPostTimerFired(Timer<AXObjectCacheImpl>*);

    AXObject* focusedImageMapUIElement(HTMLAreaElement*);

    AXID getAXID(AXObject*);

    void textChanged(Node*);
    bool nodeIsTextControl(const Node*);

    Settings* settings();
};

// This is the only subclass of AXObjectCache.
DEFINE_TYPE_CASTS(AXObjectCacheImpl, AXObjectCache, cache, true, true);

bool nodeHasRole(Node*, const String& role);
// This will let you know if aria-hidden was explicitly set to false.
bool isNodeAriaVisible(Node*);

}

#endif
