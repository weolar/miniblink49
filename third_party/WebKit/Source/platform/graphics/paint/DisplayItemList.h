// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DisplayItemList_h
#define DisplayItemList_h

#include "platform/PlatformExport.h"
#include "platform/graphics/ListContainer.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "platform/graphics/paint/Transform3DDisplayItem.h"
#include "wtf/HashMap.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Utility.h"
#include "wtf/Vector.h"

namespace blink {

class GraphicsContext;

using DisplayItems = ListContainer<DisplayItem>;

static const size_t kInitialDisplayItemsCapacity = 64;
static const size_t kMaximumDisplayItemSize = sizeof(BeginTransform3DDisplayItem);

class PLATFORM_EXPORT DisplayItemList {
    WTF_MAKE_NONCOPYABLE(DisplayItemList);
    WTF_MAKE_FAST_ALLOCATED(DisplayItemList);
public:
    static PassOwnPtr<DisplayItemList> create()
    {
        return adoptPtr(new DisplayItemList());
    }

    // These methods are called during paint invalidation.
    void invalidate(DisplayItemClient);
    void invalidateAll();

    // These methods are called during painting.
    template <typename DisplayItemClass, typename... Args>
    DisplayItemClass& createAndAppend(Args&&... args)
    {
        static_assert(WTF::IsSubclass<DisplayItemClass, DisplayItem>::value,
            "Can only createAndAppend subclasses of DisplayItem.");
        static_assert(sizeof(DisplayItemClass) <= kMaximumDisplayItemSize,
            "DisplayItem subclass is larger than kMaximumDisplayItemSize.");

        DisplayItemClass* displayItem = m_newDisplayItems.allocateAndConstruct<DisplayItemClass>(WTF::forward<Args>(args)...);
        processNewItem(displayItem);
        return *displayItem;
    }
    void beginScope(DisplayItemClient);
    void endScope(DisplayItemClient);

    // True if the last display item is a begin that doesn't draw content.
    bool lastDisplayItemIsNoopBegin() const;
    void removeLastDisplayItem();

    void beginSkippingCache() { ++m_skippingCacheCount; }
    void endSkippingCache() { ASSERT(m_skippingCacheCount > 0); --m_skippingCacheCount; }
    bool skippingCache() const { return m_skippingCacheCount; }

    // Must be called when a painting is finished.
    void commitNewDisplayItems();

    // Get the paint list generated after the last painting.
    const DisplayItems& displayItems() const;

    bool clientCacheIsValid(DisplayItemClient) const;

    // Commits the new display items and plays back the updated display items into the given context.
    void commitNewDisplayItemsAndReplay(GraphicsContext& context)
    {
        commitNewDisplayItems();
        replay(context);
    }

    void commitNewDisplayItemsAndAppendToWebDisplayItemList(WebDisplayItemList*);

    bool displayItemConstructionIsDisabled() const { return m_constructionDisabled; }
    void setDisplayItemConstructionIsDisabled(const bool disable) { m_constructionDisabled = disable; }

#if ENABLE(ASSERT)
    size_t newDisplayItemsSize() const { return m_newDisplayItems.size(); }
#endif

#ifndef NDEBUG
    void showDebugData() const;
#endif

protected:
    DisplayItemList()
        : m_currentDisplayItems(kMaximumDisplayItemSize, 0)
        , m_newDisplayItems(kMaximumDisplayItemSize, kInitialDisplayItemsCapacity)
        , m_validlyCachedClientsDirty(false)
        , m_constructionDisabled(false)
        , m_skippingCacheCount(0)
        , m_numCachedItems(0) { }

private:
    friend class DisplayItemListTest;
    friend class DisplayItemListPaintTest;
    friend class LayoutObjectDrawingRecorderTest;

    // Set new item state (scopes, cache skipping, etc) for a new item.
    // TODO(pdr): This only passes a pointer to make the patch easier to review. Change to a reference.
    void processNewItem(DisplayItem*);

    void updateValidlyCachedClientsIfNeeded() const;

#ifndef NDEBUG
    WTF::String displayItemsAsDebugString(const DisplayItems&) const;
#endif

    // Indices into PaintList of all DrawingDisplayItems and BeginSubtreeDisplayItems of each client.
    // Temporarily used during merge to find out-of-order display items.
    using DisplayItemIndicesByClientMap = HashMap<DisplayItemClient, Vector<size_t>>;

    static size_t findMatchingItemFromIndex(const DisplayItem::Id&, const DisplayItemIndicesByClientMap&, const DisplayItems&);
    static void addItemToIndex(DisplayItemClient, DisplayItem::Type, size_t index, DisplayItemIndicesByClientMap&);
    DisplayItems::Iterator findOutOfOrderCachedItem(DisplayItems::Iterator currentIt, const DisplayItem::Id&, DisplayItemIndicesByClientMap&);
    DisplayItems::Iterator findOutOfOrderCachedItemForward(DisplayItems::Iterator currentIt, const DisplayItem::Id&, DisplayItemIndicesByClientMap&);

#if ENABLE(ASSERT)
    // The following two methods are for checking under-invalidations
    // (when RuntimeEnabledFeatures::slimmingPaintUnderInvalidationCheckingEnabled).
    void checkCachedDisplayItemIsUnchanged(const DisplayItem&, DisplayItemIndicesByClientMap&);
    void checkNoRemainingCachedDisplayItems();
#endif

    void replay(GraphicsContext&);

    DisplayItems m_currentDisplayItems;
    DisplayItems m_newDisplayItems;

    // Contains all clients having valid cached paintings if updated.
    // It's lazily updated in updateValidlyCachedClientsIfNeeded().
    // FIXME: In the future we can replace this with client-side repaint flags
    // to avoid the cost of building and querying the hash table.
    mutable HashSet<DisplayItemClient> m_validlyCachedClients;
    mutable bool m_validlyCachedClientsDirty;

    // Allow display item construction to be disabled to isolate the costs of construction
    // in performance metrics.
    bool m_constructionDisabled;

    int m_skippingCacheCount;

    int m_numCachedItems;

    // Scope ids are allocated per client to ensure that the ids are stable for non-invalidated
    // clients between frames, so that we can use the id to match new display items to cached
    // display items.
    struct Scope {
        Scope(DisplayItemClient c, int i) : client(c), id(i) { }
        DisplayItemClient client;
        int id;
    };
    typedef HashMap<DisplayItemClient, int> ClientScopeIdMap;
    ClientScopeIdMap m_clientScopeIdMap;
    Vector<Scope> m_scopeStack;

#if ENABLE(ASSERT)
    // This is used to check duplicated ids during add(). We could also check during
    // updatePaintList(), but checking during add() helps developer easily find where
    // the duplicated ids are from.
    DisplayItemIndicesByClientMap m_newDisplayItemIndicesByClient;
#endif
};

} // namespace blink

#endif // DisplayItemList_h
