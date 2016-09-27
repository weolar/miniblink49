// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/paint/DisplayItemList.h"

#include "platform/NotImplemented.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "platform/graphics/paint/DrawingDisplayItem.h"

#if ENABLE(ASSERT)
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/skia/include/core/SkStream.h"
#endif

#ifndef NDEBUG
#include "platform/graphics/LoggingCanvas.h"
#include "wtf/text/StringBuilder.h"
#include <stdio.h>
#endif

namespace blink {

const DisplayItems& DisplayItemList::displayItems() const
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());
    ASSERT(m_newDisplayItems.empty());
    return m_currentDisplayItems;
}

bool DisplayItemList::lastDisplayItemIsNoopBegin() const
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());
    if (m_newDisplayItems.empty())
        return false;

    const auto& lastDisplayItem = *m_newDisplayItems.back();
    return lastDisplayItem.isBegin() && !lastDisplayItem.drawsContent();
}

void DisplayItemList::removeLastDisplayItem()
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());
    if (m_newDisplayItems.empty())
        return;

#if ENABLE(ASSERT)
    // Also remove the index pointing to the removed display item.
    DisplayItemIndicesByClientMap::iterator it = m_newDisplayItemIndicesByClient.find(m_newDisplayItems.back()->client());
    if (it != m_newDisplayItemIndicesByClient.end()) {
        Vector<size_t>& indices = it->value;
        if (!indices.isEmpty() && indices.last() == (m_newDisplayItems.size() - 1))
            indices.removeLast();
    }
#endif
    m_newDisplayItems.removeLast();
}

void DisplayItemList::processNewItem(DisplayItem* displayItem)
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());
    ASSERT(!m_constructionDisabled);
    ASSERT(!skippingCache() || !displayItem->isCached());

    if (DisplayItem::isCachedType(displayItem->type()))
        ++m_numCachedItems;

#if ENABLE(ASSERT)
    // Verify noop begin/end pairs have been removed.
    if (m_newDisplayItems.size() >= 2 && displayItem->isEnd()) {
        const auto& beginDisplayItem = *m_newDisplayItems.elementAt(m_newDisplayItems.size() - 2);
        if (beginDisplayItem.isBegin() && !beginDisplayItem.drawsContent())
            ASSERT(!displayItem->isEndAndPairedWith(beginDisplayItem.type()));
    }
#endif

    if (!m_scopeStack.isEmpty())
        displayItem->setScope(m_scopeStack.last().id, m_scopeStack.last().client);

#if ENABLE(ASSERT)
    size_t index = findMatchingItemFromIndex(displayItem->nonCachedId(), m_newDisplayItemIndicesByClient, m_newDisplayItems);
    if (index != kNotFound) {
#ifndef NDEBUG
        showDebugData();
        WTFLogAlways("DisplayItem %s has duplicated id with previous %s (index=%d)\n",
            displayItem->asDebugString().utf8().data(), m_newDisplayItems.elementAt(index)->asDebugString().utf8().data(), static_cast<int>(index));
#endif
        ASSERT_NOT_REACHED();
    }
    addItemToIndex(displayItem->client(), displayItem->type(), m_newDisplayItems.size() - 1, m_newDisplayItemIndicesByClient);
#endif // ENABLE(ASSERT)

    ASSERT(!displayItem->skippedCache()); // Only DisplayItemList can set the flag.
    if (skippingCache())
        displayItem->setSkippedCache();
}

void DisplayItemList::beginScope(DisplayItemClient client)
{
    ClientScopeIdMap::iterator it = m_clientScopeIdMap.find(client);
    int scopeId;
    if (it == m_clientScopeIdMap.end()) {
        m_clientScopeIdMap.add(client, 0);
        scopeId = 0;
    } else {
        scopeId = ++it->value;
    }
    m_scopeStack.append(Scope(client, scopeId));
    beginSkippingCache();
}

void DisplayItemList::endScope(DisplayItemClient client)
{
    m_scopeStack.removeLast();
    endSkippingCache();
}

void DisplayItemList::invalidate(DisplayItemClient client)
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());
    // Can only be called during layout/paintInvalidation, not during painting.
    ASSERT(m_newDisplayItems.empty());
    updateValidlyCachedClientsIfNeeded();
    m_validlyCachedClients.remove(client);
}

void DisplayItemList::invalidateAll()
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());
    // Can only be called during layout/paintInvalidation, not during painting.
    ASSERT(m_newDisplayItems.empty());
    m_currentDisplayItems.clear();
    m_validlyCachedClients.clear();
    m_validlyCachedClientsDirty = false;
}

bool DisplayItemList::clientCacheIsValid(DisplayItemClient client) const
{
    if (skippingCache())
        return false;
    updateValidlyCachedClientsIfNeeded();
    return m_validlyCachedClients.contains(client);
}

size_t DisplayItemList::findMatchingItemFromIndex(const DisplayItem::Id& id, const DisplayItemIndicesByClientMap& displayItemIndicesByClient, const DisplayItems& list)
{
    DisplayItemIndicesByClientMap::const_iterator it = displayItemIndicesByClient.find(id.client);
    if (it == displayItemIndicesByClient.end())
        return kNotFound;

    const Vector<size_t>& indices = it->value;
    for (size_t index : indices) {
        // TODO(pdr): elementAt is not cheap so this should be refactored (See crbug.com/505965).
        const DisplayItem& existingItem = *list.elementAt(index);
        ASSERT(existingItem.ignoreFromDisplayList() || existingItem.client() == id.client);
        if (!existingItem.ignoreFromDisplayList() && id.matches(existingItem))
            return index;
    }

    return kNotFound;
}

void DisplayItemList::addItemToIndex(DisplayItemClient client, DisplayItem::Type type, size_t index, DisplayItemIndicesByClientMap& displayItemIndicesByClient)
{
    // Only need to index DrawingDisplayItems and FIXME: BeginSubtreeDisplayItems.
    if (!DisplayItem::isDrawingType(type))
        return;

    DisplayItemIndicesByClientMap::iterator it = displayItemIndicesByClient.find(client);
    Vector<size_t>& indices = it == displayItemIndicesByClient.end() ?
        displayItemIndicesByClient.add(client, Vector<size_t>()).storedValue->value : it->value;
    indices.append(index);
}

DisplayItems::Iterator DisplayItemList::findOutOfOrderCachedItem(DisplayItems::Iterator currentIt, const DisplayItem::Id& id, DisplayItemIndicesByClientMap& displayItemIndicesByClient)
{
    ASSERT(clientCacheIsValid(id.client));

    size_t foundIndex = findMatchingItemFromIndex(id, displayItemIndicesByClient, m_currentDisplayItems);
    if (foundIndex != kNotFound)
        return m_currentDisplayItems.iteratorAt(foundIndex);

    return findOutOfOrderCachedItemForward(currentIt, id, displayItemIndicesByClient);
}

// Find forward for the item and index all skipped indexable items.
DisplayItems::Iterator DisplayItemList::findOutOfOrderCachedItemForward(DisplayItems::Iterator currentIt, const DisplayItem::Id& id, DisplayItemIndicesByClientMap& displayItemIndicesByClient)
{
    DisplayItems::Iterator currentEnd = m_currentDisplayItems.end();
    for (; currentIt != currentEnd; ++currentIt) {
        const DisplayItem& item = **currentIt;
        if (!item.ignoreFromDisplayList()
            && DisplayItem::isDrawingType(item.type())
            && m_validlyCachedClients.contains(item.client())) {
            if (id.matches(item))
                return currentIt;

            addItemToIndex(item.client(), item.type(), currentIt.index(), displayItemIndicesByClient);
        }
    }
    return currentEnd;
}

// Update the existing display items by removing invalidated entries, updating
// repainted ones, and appending new items.
// - For CachedDisplayItem, copy the corresponding cached DrawingDisplayItem;
// - FIXME: Re-enable SubtreeCachedDisplayItem:
//   For SubtreeCachedDisplayItem, copy the cached display items between the
//   corresponding BeginSubtreeDisplayItem and EndSubtreeDisplayItem (incl.);
// - Otherwise, copy the new display item.
//
// The algorithm is O(|m_currentDisplayItems| + |m_newDisplayItems|).
// Coefficients are related to the ratio of out-of-order [Subtree]CachedDisplayItems
// and the average number of (Drawing|BeginSubtree)DisplayItems per client.
void DisplayItemList::commitNewDisplayItems()
{
    TRACE_EVENT2("blink,benchmark", "DisplayItemList::commitNewDisplayItems", "current_display_list_size", (int)m_currentDisplayItems.size(),
        "num_non_cached_new_items", (int)m_newDisplayItems.size() - m_numCachedItems);

    // These data structures are used during painting only.
    m_clientScopeIdMap.clear();
    ASSERT(m_scopeStack.isEmpty());
    m_scopeStack.clear();
    ASSERT(!skippingCache());
#if ENABLE(ASSERT)
    m_newDisplayItemIndicesByClient.clear();
#endif

    if (m_currentDisplayItems.empty()) {
#if ENABLE(ASSERT)
        for (const auto& item : m_newDisplayItems) {
            ASSERT(!DisplayItem::isCachedType(item->type())
                && !DisplayItem::isSubtreeCachedType(item->type()));
        }
#endif
        m_currentDisplayItems.swap(m_newDisplayItems);
        m_validlyCachedClientsDirty = true;
        m_numCachedItems = 0;
        return;
    }

    updateValidlyCachedClientsIfNeeded();

    // Stores indices to valid DrawingDisplayItems in m_currentDisplayItems that have not been matched
    // by CachedDisplayItems during synchronized matching. The indexed items will be matched
    // by later out-of-order CachedDisplayItems in m_newDisplayItems. This ensures that when
    // out-of-order CachedDisplayItems occur, we only traverse at most once over m_currentDisplayItems
    // looking for potential matches. Thus we can ensure that the algorithm runs in linear time.
    DisplayItemIndicesByClientMap displayItemIndicesByClient;

#if ENABLE(ASSERT)
    if (RuntimeEnabledFeatures::slimmingPaintUnderInvalidationCheckingEnabled()) {
        // Under-invalidation checking requires a full index of m_currentDisplayItems.
        size_t i = 0;
        for (const auto& item : m_currentDisplayItems) {
            addItemToIndex(item->client(), item->type(), i, displayItemIndicesByClient);
            ++i;
        }
    }
#endif // ENABLE(ASSERT)

    DisplayItems updatedList(kMaximumDisplayItemSize, std::max(m_currentDisplayItems.size(), m_newDisplayItems.size()));
    DisplayItems::Iterator currentIt = m_currentDisplayItems.begin();
    DisplayItems::Iterator currentEnd = m_currentDisplayItems.end();
    for (DisplayItems::Iterator newIt = m_newDisplayItems.begin(); newIt != m_newDisplayItems.end(); ++newIt) {
        const DisplayItem& newDisplayItem = **newIt;
        const DisplayItem::Id newDisplayItemId = newDisplayItem.nonCachedId();
        bool newDisplayItemHasCachedType = newDisplayItem.type() != newDisplayItemId.type;

        bool isSynchronized = currentIt != currentEnd
            && !currentIt->ignoreFromDisplayList()
            && newDisplayItemId.matches(**currentIt);

        if (newDisplayItemHasCachedType) {
            ASSERT(!RuntimeEnabledFeatures::slimmingPaintUnderInvalidationCheckingEnabled());
            ASSERT(DisplayItem::isCachedType(newDisplayItem.type()));
            ASSERT(clientCacheIsValid(newDisplayItem.client()));
            if (isSynchronized) {
                updatedList.appendByMoving(*currentIt);
            } else {
                DisplayItems::Iterator foundIt = findOutOfOrderCachedItem(currentIt, newDisplayItemId, displayItemIndicesByClient);
                isSynchronized = (foundIt == currentIt);

#ifndef NDEBUG
                if (foundIt == currentEnd) {
                    showDebugData();
                    WTFLogAlways("CachedDisplayItem %s not found in m_currentDisplayItems\n",
                        newDisplayItem.asDebugString().utf8().data());
                    ASSERT_NOT_REACHED();
                }
#endif
                // If foundIt == currentEnd, it means that we did not find the cached display item. This should be impossible, but may occur
                // if there is a bug in the system, such as under-invalidation, incorrect cache checking or duplicate display ids. In this case,
                // attempt to recover rather than crashing or bailing on display of the rest of the display list.
                if (foundIt == currentEnd)
                    continue;

                currentIt = foundIt;

                updatedList.appendByMoving(*foundIt);
            }
        } else {
#if ENABLE(ASSERT)
            if (RuntimeEnabledFeatures::slimmingPaintUnderInvalidationCheckingEnabled())
                checkCachedDisplayItemIsUnchanged(newDisplayItem, displayItemIndicesByClient);
            else
                ASSERT(!DisplayItem::isDrawingType(newDisplayItem.type()) || newDisplayItem.skippedCache() || !clientCacheIsValid(newDisplayItem.client()));
#endif // ENABLE(ASSERT)
            updatedList.appendByMoving(*newIt);
        }

        if (isSynchronized)
            ++currentIt;
    }

#if ENABLE(ASSERT)
    if (RuntimeEnabledFeatures::slimmingPaintUnderInvalidationCheckingEnabled())
        checkNoRemainingCachedDisplayItems();
#endif // ENABLE(ASSERT)

    m_newDisplayItems.clear();
    m_validlyCachedClientsDirty = true;
    m_currentDisplayItems.swap(updatedList);
    m_numCachedItems = 0;
}

void DisplayItemList::updateValidlyCachedClientsIfNeeded() const
{
    if (!m_validlyCachedClientsDirty)
        return;

    m_validlyCachedClients.clear();
    m_validlyCachedClientsDirty = false;

    DisplayItemClient lastClient = nullptr;
    for (const auto& displayItem : m_currentDisplayItems) {
        if (displayItem->client() == lastClient)
            continue;
        lastClient = displayItem->client();
        if (!displayItem->skippedCache())
            m_validlyCachedClients.add(lastClient);
    }
}

void DisplayItemList::commitNewDisplayItemsAndAppendToWebDisplayItemList(WebDisplayItemList* list)
{
    commitNewDisplayItems();
    for (const auto& item : m_currentDisplayItems)
        item->appendToWebDisplayItemList(list);
}

#if ENABLE(ASSERT)

static void showUnderInvalidationError(const char* reason, const DisplayItem& displayItem)
{
#ifndef NDEBUG
    WTFLogAlways("%s: %s\nSee http://crbug.com/450725.", reason, displayItem.asDebugString().utf8().data());
#else
    WTFLogAlways("%s. Run debug build to get more details\nSee http://crbug.com/450725.", reason);
#endif // NDEBUG
}

static bool bitmapIsAllZero(const SkBitmap& bitmap)
{
    bitmap.lockPixels();
    bool result = true;
    for (int x = 0; result && x < bitmap.width(); ++x) {
        for (int y = 0; result && y < bitmap.height(); ++y) {
            if (SkColorSetA(bitmap.getColor(x, y), 0) != SK_ColorTRANSPARENT)
                result = false;
        }
    }
    bitmap.unlockPixels();
    return result;
}

void DisplayItemList::checkCachedDisplayItemIsUnchanged(const DisplayItem& displayItem, DisplayItemIndicesByClientMap& displayItemIndicesByClient)
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintUnderInvalidationCheckingEnabled());

    if (!DisplayItem::isDrawingType(displayItem.type()) || displayItem.skippedCache() || !clientCacheIsValid(displayItem.client()))
        return;

    // If checking under-invalidation, we always generate new display item even if the client is not invalidated.
    // Checks if the new picture is the same as the cached old picture. If the new picture is different but
    // the client is not invalidated, issue error about under-invalidation.
    size_t index = findMatchingItemFromIndex(displayItem.nonCachedId(), displayItemIndicesByClient, m_currentDisplayItems);
    if (index == kNotFound) {
        showUnderInvalidationError("ERROR: under-invalidation: no cached display item", displayItem);
        ASSERT_NOT_REACHED();
        return;
    }

    DisplayItems::Iterator foundItem = m_currentDisplayItems.iteratorAt(index);
    RefPtr<const SkPicture> newPicture = static_cast<const DrawingDisplayItem&>(displayItem).picture();
    RefPtr<const SkPicture> oldPicture = static_cast<const DrawingDisplayItem*>(*foundItem)->picture();
    // Mark the display item as ignored so that we can check if there are any remaining cached display items after merging.
    foundItem->setIgnoredFromDisplayList();

    if (!newPicture && !oldPicture)
        return;
    if (newPicture && oldPicture) {
        switch (static_cast<const DrawingDisplayItem&>(displayItem).underInvalidationCheckingMode()) {
        case DrawingDisplayItem::CheckPicture:
            if (newPicture->approximateOpCount() == oldPicture->approximateOpCount()) {
                SkDynamicMemoryWStream newPictureSerialized;
                newPicture->serialize(&newPictureSerialized);
                SkDynamicMemoryWStream oldPictureSerialized;
                oldPicture->serialize(&oldPictureSerialized);

                if (newPictureSerialized.bytesWritten() == oldPictureSerialized.bytesWritten()) {
                    RefPtr<SkData> oldData = adoptRef(oldPictureSerialized.copyToData());
                    RefPtr<SkData> newData = adoptRef(newPictureSerialized.copyToData());
                    if (oldData->equals(newData.get()))
                        return;
                }
            }
            break;
        case DrawingDisplayItem::CheckBitmap:
            if (newPicture->cullRect() == oldPicture->cullRect()) {
                SkBitmap bitmap;
                SkRect rect = newPicture->cullRect();
                bitmap.allocPixels(SkImageInfo::MakeN32Premul(rect.width(), rect.height()));
                SkCanvas canvas(bitmap);
                canvas.translate(-rect.x(), -rect.y());
                canvas.drawPicture(oldPicture.get());
                SkPaint diffPaint;
                diffPaint.setXfermodeMode(SkXfermode::kDifference_Mode);
                canvas.drawPicture(newPicture.get(), nullptr, &diffPaint);
                if (bitmapIsAllZero(bitmap)) // Contents are the same.
                    return;
            }
        default:
            ASSERT_NOT_REACHED();
        }
    }

    showUnderInvalidationError("ERROR: under-invalidation: display item changed", displayItem);
#ifndef NDEBUG
    String oldPictureDebugString = oldPicture ? pictureAsDebugString(oldPicture.get()) : "None";
    String newPictureDebugString = newPicture ? pictureAsDebugString(newPicture.get()) : "None";
    WTFLogAlways("old picture:\n%s\n", oldPictureDebugString.utf8().data());
    WTFLogAlways("new picture:\n%s\n", newPictureDebugString.utf8().data());
#endif // NDEBUG

    ASSERT_NOT_REACHED();
}

void DisplayItemList::checkNoRemainingCachedDisplayItems()
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintUnderInvalidationCheckingEnabled());

    for (const auto& displayItem : m_currentDisplayItems) {
        if (displayItem->ignoreFromDisplayList() || !DisplayItem::isDrawingType(displayItem->type()) || !clientCacheIsValid(displayItem->client()))
            continue;
        showUnderInvalidationError("May be under-invalidation: no new display item", *displayItem);
    }
}

#endif // ENABLE(ASSERT)

#ifndef NDEBUG

WTF::String DisplayItemList::displayItemsAsDebugString(const DisplayItems& list) const
{
    StringBuilder stringBuilder;
    size_t i = 0;
    for (auto it = list.begin(); it != list.end(); ++it, ++i) {
        const DisplayItem& displayItem = *list.elementAt(i);
        if (i)
            stringBuilder.append(",\n");
        if (displayItem.ignoreFromDisplayList()) {
            stringBuilder.append("null");
            continue;
        }
        stringBuilder.append(String::format("{index: %d, ", (int)i));
        displayItem.dumpPropertiesAsDebugString(stringBuilder);
        stringBuilder.append(", cacheIsValid: ");
        stringBuilder.append(clientCacheIsValid(displayItem.client()) ? "true" : "false");
        stringBuilder.append('}');
    }
    return stringBuilder.toString();
}

void DisplayItemList::showDebugData() const
{
    WTFLogAlways("current display items: [%s]\n", displayItemsAsDebugString(m_currentDisplayItems).utf8().data());
    WTFLogAlways("new display items: [%s]\n", displayItemsAsDebugString(m_newDisplayItems).utf8().data());
}

#endif // ifndef NDEBUG

void DisplayItemList::replay(GraphicsContext& context)
{
    TRACE_EVENT0("blink,benchmark", "DisplayItemList::replay");
    ASSERT(m_newDisplayItems.empty());
    for (auto displayItem : m_currentDisplayItems)
        displayItem->replay(context);
}

} // namespace blink
