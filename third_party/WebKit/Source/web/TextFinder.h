/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef TextFinder_h
#define TextFinder_h

#include "core/editing/FindOptions.h"
#include "platform/geometry/FloatRect.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebFloatPoint.h"
#include "public/platform/WebFloatRect.h"
#include "public/platform/WebRect.h"
#include "public/web/WebFindOptions.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Range;
class WebLocalFrameImpl;

template <typename T> class WebVector;

class TextFinder final : public NoBaseWillBeGarbageCollectedFinalized<TextFinder> {
public:
    static PassOwnPtrWillBeRawPtr<TextFinder> create(WebLocalFrameImpl& ownerFrame);

    bool find(
        int identifier, const WebString& searchText, const WebFindOptions&,
        bool wrapWithinFrame, WebRect* selectionRect);
    void stopFindingAndClearSelection();
    void scopeStringMatches(
        int identifier, const WebString& searchText, const WebFindOptions&,
        bool reset);
    void cancelPendingScopingEffort();
    void increaseMatchCount(int identifier, int count);
    void resetMatchCount();
    int findMatchMarkersVersion() const { return m_findMatchMarkersVersion; }
    WebFloatRect activeFindMatchRect();
    void findMatchRects(WebVector<WebFloatRect>&);
    int selectNearestFindMatch(const WebFloatPoint&, WebRect* selectionRect);

    // Returns which frame has an active match. This function should only be
    // called on the main frame, as it is the only frame keeping track. Returned
    // value can be 0 if no frame has an active match.
    WebLocalFrameImpl* activeMatchFrame() const { return m_currentActiveMatchFrame; }

    // Returns the active match in the current frame. Could be a null range if
    // the local frame has no active match.
    Range* activeMatch() const { return m_activeMatch.get(); }

    void flushCurrentScoping();

    void resetActiveMatch() { m_activeMatch = nullptr; }

    int totalMatchCount() const { return m_totalMatchCount; }
    bool scopingInProgress() const { return m_scopingInProgress; }
    void increaseMarkerVersion() { ++m_findMatchMarkersVersion; }

    ~TextFinder();

    class FindMatch {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        FindMatch(PassRefPtrWillBeRawPtr<Range>, int ordinal);

        DECLARE_TRACE();

        RefPtrWillBeMember<Range> m_range;

        // 1-based index within this frame.
        int m_ordinal;

        // In find-in-page coordinates.
        // Lazily calculated by updateFindMatchRects.
        FloatRect m_rect;
    };

    DECLARE_TRACE();

private:
    class DeferredScopeStringMatches;
    friend class DeferredScopeStringMatches;

    explicit TextFinder(WebLocalFrameImpl& ownerFrame);

    // Notifies the delegate about a new selection rect.
    void reportFindInPageSelection(
        const WebRect& selectionRect, int activeMatchOrdinal, int identifier);

    void reportFindInPageResultToAccessibility(int identifier);

    // Clear the find-in-page matches cache forcing rects to be fully
    // calculated again next time updateFindMatchRects is called.
    void clearFindMatchesCache();

    // Check if the activeMatchFrame still exists in the frame tree.
    bool isActiveMatchFrameValid() const;

    // Return the index in the find-in-page cache of the match closest to the
    // provided point in find-in-page coordinates, or -1 in case of error.
    // The squared distance to the closest match is returned in the distanceSquared parameter.
    int nearestFindMatch(const FloatPoint&, float& distanceSquared);

    // Select a find-in-page match marker in the current frame using a cache
    // match index returned by nearestFindMatch. Returns the ordinal of the new
    // selected match or -1 in case of error. Also provides the bounding box of
    // the marker in window coordinates if selectionRect is not null.
    int selectFindMatch(unsigned index, WebRect* selectionRect);

    // Compute and cache the rects for FindMatches if required.
    // Rects are automatically invalidated in case of content size changes,
    // propagating the invalidation to child frames.
    void updateFindMatchRects();

    // Append the find-in-page match rects of the current frame to the provided vector.
    void appendFindMatchRects(Vector<WebFloatRect>& frameRects);

    // Add a WebKit TextMatch-highlight marker to nodes in a range.
    void addMarker(Range*, bool activeMatch);

    // Sets the markers within a range as active or inactive.
    void setMarkerActive(Range*, bool active);

    // Returns the ordinal of the first match in the frame specified. This
    // function enumerates the frames, starting with the main frame and up to (but
    // not including) the frame passed in as a parameter and counts how many
    // matches have been found.
    int ordinalOfFirstMatchForFrame(WebLocalFrameImpl*) const;

    // Determines whether the scoping effort is required for a particular frame.
    // It is not necessary if the frame is invisible, for example, or if this
    // is a repeat search that already returned nothing last time the same prefix
    // was searched.
    bool shouldScopeMatches(const WTF::String& searchText);

    // Removes the current frame from the global scoping effort and triggers any
    // updates if appropriate. This method does not mark the scoping operation
    // as finished.
    void flushCurrentScopingEffort(int identifier);

    // Finishes the current scoping effort and triggers any updates if appropriate.
    void finishCurrentScopingEffort(int identifier);

    // Queue up a deferred call to scopeStringMatches.
    void scopeStringMatchesSoon(
        int identifier, const WebString& searchText, const WebFindOptions&,
        bool reset);

    // Called by a DeferredScopeStringMatches instance.
    void callScopeStringMatches(
        DeferredScopeStringMatches*, int identifier, const WebString& searchText,
        const WebFindOptions&, bool reset);

    // Determines whether to invalidate the content area and scrollbar.
    void invalidateIfNecessary();

    // Sets the markers within a current match range as active or inactive.
    void setMatchMarkerActive(bool);

    void decrementFramesScopingCount(int identifier);

    WebLocalFrameImpl& ownerFrame() const
    {
        ASSERT(m_ownerFrame);
        return *m_ownerFrame;
    }

    // Returns the ordinal of the first match in the owner frame.
    int ordinalOfFirstMatch() const;

    RawPtrWillBeMember<WebLocalFrameImpl> m_ownerFrame;

    // A way for the main frame to keep track of which frame has an active
    // match. Should be 0 for all other frames.
    RawPtrWillBeMember<WebLocalFrameImpl> m_currentActiveMatchFrame;

    // The range of the active match for the current frame.
    RefPtrWillBeMember<Range> m_activeMatch;

    // The index of the active match for the current frame.
    int m_activeMatchIndexInCurrentFrame;

    // The scoping effort can time out and we need to keep track of where we
    // ended our last search so we can continue from where we left of.
    //
    // This range is collapsed to the end position of the last successful
    // search; the new search should start from this position.
    RefPtrWillBeMember<Range> m_resumeScopingFromRange;

    // Keeps track of the last string this frame searched for. This is used for
    // short-circuiting searches in the following scenarios: When a frame has
    // been searched and returned 0 results, we don't need to search that frame
    // again if the user is just adding to the search (making it more specific).
    WTF::String m_lastSearchString;

    // Keeps track of how many matches this frame has found so far, so that we
    // don't lose count between scoping efforts, and is also used (in conjunction
    // with m_lastSearchString) to figure out if we need to search the frame again.
    int m_lastMatchCount;

    // This variable keeps a cumulative total of matches found so far for ALL the
    // frames on the page, and is only incremented by calling IncreaseMatchCount
    // (on the main frame only). It should be -1 for all other frames.
    int m_totalMatchCount;

    // This variable keeps a cumulative total of how many frames are currently
    // scoping, and is incremented/decremented on the main frame only.
    // It should be -1 for all other frames.
    int m_framesScopingCount;

    // Identifier of the latest find-in-page request. Required to be stored in
    // the frame in order to reply if required in case the frame is detached.
    int m_findRequestIdentifier;

    // Keeps track of when the scoping effort should next invalidate the scrollbar
    // and the frame area.
    int m_nextInvalidateAfter;

    // A list of all of the pending calls to scopeStringMatches.
    WillBeHeapVector<OwnPtrWillBeMember<DeferredScopeStringMatches>> m_deferredScopingWork;

    // Version number incremented on the main frame only whenever the document
    // find-in-page match markers change. It should be 0 for all other frames.
    int m_findMatchMarkersVersion;

    // Local cache of the find match markers currently displayed for this frame.
    WillBeHeapVector<FindMatch> m_findMatchesCache;

    // Contents size when find-in-page match rects were last computed for this
    // frame's cache.
    IntSize m_contentsSizeForCurrentFindMatchRects;

    // This flag is used by the scoping effort to determine if we need to figure
    // out which rectangle is the active match. Once we find the active
    // rectangle we clear this flag.
    bool m_locatingActiveRect;

    // Keeps track of whether there is an scoping effort ongoing in the frame.
    bool m_scopingInProgress;

    // Keeps track of whether the last find request completed its scoping effort
    // without finding any matches in this frame.
    bool m_lastFindRequestCompletedWithNoMatches;

    // Determines if the rects in the find-in-page matches cache of this frame
    // are invalid and should be recomputed.
    bool m_findMatchRectsAreValid;
};

} // namespace blink

WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::TextFinder::FindMatch);

#endif // TextFinder_h
