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


#include "config.h"
#include "web/TextFinder.h"

#include "core/dom/DocumentMarker.h"
#include "core/dom/DocumentMarkerController.h"
#include "core/dom/Range.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/Editor.h"
#include "core/editing/VisibleSelection.h"
#include "core/editing/iterators/CharacterIterator.h"
#include "core/frame/FrameView.h"
#include "core/layout/LayoutObject.h"
#include "core/page/Page.h"
#include "modules/accessibility/AXObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "platform/Timer.h"
#include "public/platform/WebVector.h"
#include "public/web/WebAXObject.h"
#include "public/web/WebFindOptions.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebViewClient.h"
#include "web/FindInPageCoordinates.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"
#include "wtf/CurrentTime.h"

namespace blink {

TextFinder::FindMatch::FindMatch(PassRefPtrWillBeRawPtr<Range> range, int ordinal)
    : m_range(range)
    , m_ordinal(ordinal)
{
}

DEFINE_TRACE(TextFinder::FindMatch)
{
    visitor->trace(m_range);
}

class TextFinder::DeferredScopeStringMatches : public NoBaseWillBeGarbageCollectedFinalized<TextFinder::DeferredScopeStringMatches> {
public:
    static PassOwnPtrWillBeRawPtr<DeferredScopeStringMatches> create(TextFinder* textFinder, int identifier, const WebString& searchText, const WebFindOptions& options, bool reset)
    {
        return adoptPtrWillBeNoop(new DeferredScopeStringMatches(textFinder, identifier, searchText, options, reset));
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_textFinder);
    }

    void dispose()
    {
        if (m_timer.isActive())
            m_timer.stop();
    }

private:
    DeferredScopeStringMatches(TextFinder* textFinder, int identifier, const WebString& searchText, const WebFindOptions& options, bool reset)
        : m_timer(this, &DeferredScopeStringMatches::doTimeout)
        , m_textFinder(textFinder)
        , m_identifier(identifier)
        , m_searchText(searchText)
        , m_options(options)
        , m_reset(reset)
    {
        m_timer.startOneShot(0.0, FROM_HERE);
    }

    void doTimeout(Timer<DeferredScopeStringMatches>*)
    {
        m_textFinder->callScopeStringMatches(this, m_identifier, m_searchText, m_options, m_reset);
    }

    Timer<DeferredScopeStringMatches> m_timer;
    RawPtrWillBeMember<TextFinder> m_textFinder;
    const int m_identifier;
    const WebString m_searchText;
    const WebFindOptions m_options;
    const bool m_reset;
};

bool TextFinder::find(int identifier, const WebString& searchText, const WebFindOptions& options, bool wrapWithinFrame, WebRect* selectionRect)
{
    if (!ownerFrame().frame() || !ownerFrame().frame()->page())
        return false;

    WebLocalFrameImpl* mainFrameImpl = ownerFrame().viewImpl()->mainFrameImpl();

    if (!options.findNext)
        ownerFrame().frame()->page()->unmarkAllTextMatches();
    else
        setMarkerActive(m_activeMatch.get(), false);

    if (m_activeMatch && &m_activeMatch->ownerDocument() != ownerFrame().frame()->document())
        m_activeMatch = nullptr;

    // If the user has selected something since the last Find operation we want
    // to start from there. Otherwise, we start searching from where the last Find
    // operation left off (either a Find or a FindNext operation).
    VisibleSelection selection(ownerFrame().frame()->selection().selection());
    bool activeSelection = !selection.isNone();
    if (activeSelection) {
        m_activeMatch = selection.firstRange().get();
        ownerFrame().frame()->selection().clear();
    }

    ASSERT(ownerFrame().frame() && ownerFrame().frame()->view());
    const FindOptions findOptions = (options.forward ? 0 : Backwards)
        | (options.matchCase ? 0 : CaseInsensitive)
        | (wrapWithinFrame ? WrapAround : 0)
        | (options.wordStart ? AtWordStarts : 0)
        | (options.medialCapitalAsWordStart ? TreatMedialCapitalAsWordStart : 0)
        | (options.findNext ? 0 : StartInSelection);
    m_activeMatch = ownerFrame().frame()->editor().findStringAndScrollToVisible(searchText, m_activeMatch.get(), findOptions);

    if (!m_activeMatch) {
        // If we're finding next the next active match might not be in the current frame.
        // In this case we don't want to clear the matches cache.
        if (!options.findNext)
            clearFindMatchesCache();

        ownerFrame().frameView()->invalidatePaintForTickmarks();
        return false;
    }

#if OS(ANDROID)
    ownerFrame().viewImpl()->zoomToFindInPageRect(ownerFrame().frameView()->contentsToRootFrame(enclosingIntRect(LayoutObject::absoluteBoundingBoxRectForRange(m_activeMatch.get()))));
#endif

    setMarkerActive(m_activeMatch.get(), true);
    WebLocalFrameImpl* oldActiveFrame = mainFrameImpl->ensureTextFinder().m_currentActiveMatchFrame;
    mainFrameImpl->ensureTextFinder().m_currentActiveMatchFrame = &ownerFrame();

    // Make sure no node is focused. See http://crbug.com/38700.
    ownerFrame().frame()->document()->setFocusedElement(nullptr);

    if (!options.findNext || activeSelection) {
        // This is either a Find operation or a Find-next from a new start point
        // due to a selection, so we set the flag to ask the scoping effort
        // to find the active rect for us and report it back to the UI.
        m_locatingActiveRect = true;
    } else {
        if (oldActiveFrame != &ownerFrame()) {
            if (options.forward)
                m_activeMatchIndexInCurrentFrame = 0;
            else
                m_activeMatchIndexInCurrentFrame = m_lastMatchCount - 1;
        } else {
            if (options.forward)
                ++m_activeMatchIndexInCurrentFrame;
            else
                --m_activeMatchIndexInCurrentFrame;

            if (m_activeMatchIndexInCurrentFrame + 1 > m_lastMatchCount)
                m_activeMatchIndexInCurrentFrame = 0;
            if (m_activeMatchIndexInCurrentFrame == -1)
                m_activeMatchIndexInCurrentFrame = m_lastMatchCount - 1;
        }
        if (selectionRect) {
            *selectionRect = ownerFrame().frameView()->contentsToRootFrame(m_activeMatch->boundingBox());
            reportFindInPageSelection(*selectionRect, m_activeMatchIndexInCurrentFrame + 1, identifier);
        }
    }

    return true;
}

void TextFinder::stopFindingAndClearSelection()
{
    cancelPendingScopingEffort();

    // Remove all markers for matches found and turn off the highlighting.
    ownerFrame().frame()->document()->markers().removeMarkers(DocumentMarker::TextMatch);
    ownerFrame().frame()->editor().setMarkedTextMatchesAreHighlighted(false);
    clearFindMatchesCache();

    // Let the frame know that we don't want tickmarks anymore.
    ownerFrame().frameView()->invalidatePaintForTickmarks();
}

void TextFinder::reportFindInPageResultToAccessibility(int identifier)
{
    AXObjectCacheImpl* axObjectCache = toAXObjectCacheImpl(ownerFrame().frame()->document()->existingAXObjectCache());
    if (!axObjectCache)
        return;

    AXObject* startObject = axObjectCache->get(m_activeMatch->startContainer());
    AXObject* endObject = axObjectCache->get(m_activeMatch->endContainer());
    if (!startObject || !endObject)
        return;

    WebLocalFrameImpl* mainFrameImpl = ownerFrame().viewImpl()->mainFrameImpl();
    if (mainFrameImpl && mainFrameImpl->client()) {
        mainFrameImpl->client()->handleAccessibilityFindInPageResult(
            identifier, m_activeMatchIndexInCurrentFrame + 1,
            WebAXObject(startObject), m_activeMatch->startOffset(),
            WebAXObject(endObject), m_activeMatch->endOffset());
    }
}

void TextFinder::scopeStringMatches(int identifier, const WebString& searchText, const WebFindOptions& options, bool reset)
{
    if (reset) {
        // This is a brand new search, so we need to reset everything.
        // Scoping is just about to begin.
        m_scopingInProgress = true;

        // Need to keep the current identifier locally in order to finish the
        // request in case the frame is detached during the process.
        m_findRequestIdentifier = identifier;

        // Clear highlighting for this frame.
        LocalFrame* frame = ownerFrame().frame();
        if (frame && frame->page() && frame->editor().markedTextMatchesAreHighlighted())
            frame->page()->unmarkAllTextMatches();

        // Clear the tickmarks and results cache.
        clearFindMatchesCache();

        // Clear the counters from last operation.
        m_lastMatchCount = 0;
        m_nextInvalidateAfter = 0;
        m_resumeScopingFromRange = nullptr;

        // The view might be null on detached frames.
        if (frame && frame->page())
            ownerFrame().viewImpl()->mainFrameImpl()->ensureTextFinder().m_framesScopingCount++;

        // Now, defer scoping until later to allow find operation to finish quickly.
        scopeStringMatchesSoon(identifier, searchText, options, false); // false means just reset, so don't do it again.
        return;
    }

    if (!shouldScopeMatches(searchText)) {
        // Note that we want to defer the final update when resetting even if shouldScopeMatches returns false.
        // This is done in order to prevent sending a final message based only on the results of the first frame
        // since m_framesScopingCount would be 0 as other frames have yet to reset.
        finishCurrentScopingEffort(identifier);
        return;
    }

    WebLocalFrameImpl* mainFrameImpl = ownerFrame().viewImpl()->mainFrameImpl();
    Position searchStart = firstPositionInNode(ownerFrame().frame()->document());
    Position searchEnd = lastPositionInNode(ownerFrame().frame()->document());
    ASSERT(searchStart.document() == searchEnd.document());

    if (m_resumeScopingFromRange) {
        // This is a continuation of a scoping operation that timed out and didn't
        // complete last time around, so we should start from where we left off.
        ASSERT(m_resumeScopingFromRange->collapsed());
        searchStart = m_resumeScopingFromRange->endPosition();
        if (searchStart.document() != searchEnd.document())
            return;
    }

    // This timeout controls how long we scope before releasing control. This
    // value does not prevent us from running for longer than this, but it is
    // periodically checked to see if we have exceeded our allocated time.
    const double maxScopingDuration = 0.1; // seconds

    int matchCount = 0;
    bool timedOut = false;
    double startTime = currentTime();
    do {
        // Find next occurrence of the search string.
        // FIXME: (http://crbug.com/6818) This WebKit operation may run for longer
        // than the timeout value, and is not interruptible as it is currently
        // written. We may need to rewrite it with interruptibility in mind, or
        // find an alternative.
        EphemeralRange result = findPlainText(EphemeralRange(searchStart, searchEnd), searchText, options.matchCase ? 0 : CaseInsensitive);
        if (result.isCollapsed()) {
            // Not found.
            break;
        }

        RefPtrWillBeRawPtr<Range> resultRange = Range::create(result.document(), result.startPosition(), result.endPosition());
        if (resultRange->collapsed()) {
            // resultRange will be collapsed if the matched text spans over multiple TreeScopes.
            // FIXME: Show such matches to users.
            searchStart = result.endPosition();
            continue;
        }

        ++matchCount;

        // Catch a special case where Find found something but doesn't know what
        // the bounding box for it is. In this case we set the first match we find
        // as the active rect.
        IntRect resultBounds = resultRange->boundingBox();
        IntRect activeSelectionRect;
        if (m_locatingActiveRect) {
            activeSelectionRect = m_activeMatch.get() ?
                m_activeMatch->boundingBox() : resultBounds;
        }

        // If the Find function found a match it will have stored where the
        // match was found in m_activeSelectionRect on the current frame. If we
        // find this rect during scoping it means we have found the active
        // tickmark.
        bool foundActiveMatch = false;
        if (m_locatingActiveRect && (activeSelectionRect == resultBounds)) {
            // We have found the active tickmark frame.
            mainFrameImpl->ensureTextFinder().m_currentActiveMatchFrame = &ownerFrame();
            foundActiveMatch = true;
            // We also know which tickmark is active now.
            m_activeMatchIndexInCurrentFrame = matchCount - 1;
            // To stop looking for the active tickmark, we set this flag.
            m_locatingActiveRect = false;

            // Notify browser of new location for the selected rectangle.
            reportFindInPageSelection(
                ownerFrame().frameView()->contentsToRootFrame(resultBounds),
                m_activeMatchIndexInCurrentFrame + 1,
                identifier);
        }

        addMarker(resultRange.get(), foundActiveMatch);

        m_findMatchesCache.append(FindMatch(resultRange.get(), m_lastMatchCount + matchCount));

        // Set the new start for the search range to be the end of the previous
        // result range. There is no need to use a VisiblePosition here,
        // since findPlainText will use a TextIterator to go over the visible
        // text nodes.
        searchStart = result.endPosition();

        m_resumeScopingFromRange = Range::create(result.document(), result.endPosition(), result.endPosition());
        timedOut = (currentTime() - startTime) >= maxScopingDuration;
    } while (!timedOut);

    // Remember what we search for last time, so we can skip searching if more
    // letters are added to the search string (and last outcome was 0).
    m_lastSearchString = searchText;

    if (matchCount > 0) {
        ownerFrame().frame()->editor().setMarkedTextMatchesAreHighlighted(true);

        m_lastMatchCount += matchCount;

        // Let the mainframe know how much we found during this pass.
        mainFrameImpl->increaseMatchCount(matchCount, identifier);
    }

    if (timedOut) {
        // If we found anything during this pass, we should redraw. However, we
        // don't want to spam too much if the page is extremely long, so if we
        // reach a certain point we start throttling the redraw requests.
        if (matchCount > 0)
            invalidateIfNecessary();

        // Scoping effort ran out of time, lets ask for another time-slice.
        scopeStringMatchesSoon(
            identifier,
            searchText,
            options,
            false); // don't reset.
        return; // Done for now, resume work later.
    }

    finishCurrentScopingEffort(identifier);
}

void TextFinder::flushCurrentScopingEffort(int identifier)
{
    if (!ownerFrame().frame() || !ownerFrame().frame()->page())
        return;

    WebLocalFrameImpl* mainFrameImpl = ownerFrame().viewImpl()->mainFrameImpl();
    mainFrameImpl->ensureTextFinder().decrementFramesScopingCount(identifier);
}

void TextFinder::finishCurrentScopingEffort(int identifier)
{
    flushCurrentScopingEffort(identifier);

    m_scopingInProgress = false;
    m_lastFindRequestCompletedWithNoMatches = !m_lastMatchCount;

    // This frame is done, so show any scrollbar tickmarks we haven't drawn yet.
    ownerFrame().frameView()->invalidatePaintForTickmarks();
}

void TextFinder::cancelPendingScopingEffort()
{
#if ENABLE(OILPAN)
    for (DeferredScopeStringMatches* deferredWork : m_deferredScopingWork)
        deferredWork->dispose();
#endif
    m_deferredScopingWork.clear();

    m_activeMatchIndexInCurrentFrame = -1;

    // Last request didn't complete.
    if (m_scopingInProgress)
        m_lastFindRequestCompletedWithNoMatches = false;

    m_scopingInProgress = false;
}

void TextFinder::increaseMatchCount(int identifier, int count)
{
    if (count)
        ++m_findMatchMarkersVersion;

    m_totalMatchCount += count;

    // Update the UI with the latest findings.
    if (ownerFrame().client())
        ownerFrame().client()->reportFindInPageMatchCount(identifier, m_totalMatchCount, !m_framesScopingCount);
}

void TextFinder::reportFindInPageSelection(const WebRect& selectionRect, int activeMatchOrdinal, int identifier)
{
    // Update the UI with the latest selection rect.
    if (ownerFrame().client())
        ownerFrame().client()->reportFindInPageSelection(identifier, ordinalOfFirstMatch() + activeMatchOrdinal, selectionRect);

    // Update accessibility too, so if the user commits to this query
    // we can move accessibility focus to this result.
    reportFindInPageResultToAccessibility(identifier);
}

void TextFinder::resetMatchCount()
{
    if (m_totalMatchCount > 0)
        ++m_findMatchMarkersVersion;

    m_totalMatchCount = 0;
    m_framesScopingCount = 0;
}

void TextFinder::clearFindMatchesCache()
{
    if (!m_findMatchesCache.isEmpty())
        ownerFrame().viewImpl()->mainFrameImpl()->ensureTextFinder().m_findMatchMarkersVersion++;

    m_findMatchesCache.clear();
    m_findMatchRectsAreValid = false;
}

bool TextFinder::isActiveMatchFrameValid() const
{
    WebLocalFrameImpl* mainFrameImpl = ownerFrame().viewImpl()->mainFrameImpl();
    WebLocalFrameImpl* activeMatchFrame = mainFrameImpl->activeMatchFrame();
    return activeMatchFrame && activeMatchFrame->activeMatch() && activeMatchFrame->frame()->tree().isDescendantOf(mainFrameImpl->frame());
}

void TextFinder::updateFindMatchRects()
{
    IntSize currentContentsSize = ownerFrame().contentsSize();
    if (m_contentsSizeForCurrentFindMatchRects != currentContentsSize) {
        m_contentsSizeForCurrentFindMatchRects = currentContentsSize;
        m_findMatchRectsAreValid = false;
    }

    size_t deadMatches = 0;
    for (FindMatch& match : m_findMatchesCache) {
        if (!match.m_range->boundaryPointsValid() || !match.m_range->startContainer()->inDocument())
            match.m_rect = FloatRect();
        else if (!m_findMatchRectsAreValid)
            match.m_rect = findInPageRectFromRange(match.m_range.get());

        if (match.m_rect.isEmpty())
            ++deadMatches;
    }

    // Remove any invalid matches from the cache.
    if (deadMatches) {
        WillBeHeapVector<FindMatch> filteredMatches;
        filteredMatches.reserveCapacity(m_findMatchesCache.size() - deadMatches);

        for (const FindMatch& match : m_findMatchesCache) {
            if (!match.m_rect.isEmpty())
                filteredMatches.append(match);
        }

        m_findMatchesCache.swap(filteredMatches);
    }

    // Invalidate the rects in child frames. Will be updated later during traversal.
    if (!m_findMatchRectsAreValid)
        for (WebFrame* child = ownerFrame().firstChild(); child; child = child->nextSibling())
            toWebLocalFrameImpl(child)->ensureTextFinder().m_findMatchRectsAreValid = false;

    m_findMatchRectsAreValid = true;
}

WebFloatRect TextFinder::activeFindMatchRect()
{
    if (!isActiveMatchFrameValid())
        return WebFloatRect();

    return WebFloatRect(findInPageRectFromRange(m_currentActiveMatchFrame->activeMatch()));
}

void TextFinder::findMatchRects(WebVector<WebFloatRect>& outputRects)
{
    Vector<WebFloatRect> matchRects;
    for (WebLocalFrameImpl* frame = &ownerFrame(); frame; frame = toWebLocalFrameImpl(frame->traverseNext(false)))
        frame->ensureTextFinder().appendFindMatchRects(matchRects);

    outputRects = matchRects;
}

void TextFinder::appendFindMatchRects(Vector<WebFloatRect>& frameRects)
{
    updateFindMatchRects();
    frameRects.reserveCapacity(frameRects.size() + m_findMatchesCache.size());
    for (const FindMatch& match : m_findMatchesCache) {
        ASSERT(!match.m_rect.isEmpty());
        frameRects.append(match.m_rect);
    }
}

int TextFinder::selectNearestFindMatch(const WebFloatPoint& point, WebRect* selectionRect)
{
    TextFinder* bestFinder = nullptr;
    int indexInBestFrame = -1;
    float distanceInBestFrame = FLT_MAX;

    for (WebLocalFrameImpl* frame = &ownerFrame(); frame; frame = toWebLocalFrameImpl(frame->traverseNext(false))) {
        float distanceInFrame;
        TextFinder& finder = frame->ensureTextFinder();
        int indexInFrame = finder.nearestFindMatch(point, distanceInFrame);
        if (distanceInFrame < distanceInBestFrame) {
            bestFinder = &finder;
            indexInBestFrame = indexInFrame;
            distanceInBestFrame = distanceInFrame;
        }
    }

    if (indexInBestFrame != -1)
        return bestFinder->selectFindMatch(static_cast<unsigned>(indexInBestFrame), selectionRect);

    return -1;
}

int TextFinder::nearestFindMatch(const FloatPoint& point, float& distanceSquared)
{
    updateFindMatchRects();

    int nearest = -1;
    distanceSquared = FLT_MAX;
    for (size_t i = 0; i < m_findMatchesCache.size(); ++i) {
        ASSERT(!m_findMatchesCache[i].m_rect.isEmpty());
        FloatSize offset = point - m_findMatchesCache[i].m_rect.center();
        float width = offset.width();
        float height = offset.height();
        float currentDistanceSquared = width * width + height * height;
        if (currentDistanceSquared < distanceSquared) {
            nearest = i;
            distanceSquared = currentDistanceSquared;
        }
    }
    return nearest;
}

int TextFinder::selectFindMatch(unsigned index, WebRect* selectionRect)
{
    ASSERT_WITH_SECURITY_IMPLICATION(index < m_findMatchesCache.size());

    RefPtrWillBeRawPtr<Range> range = m_findMatchesCache[index].m_range;
    if (!range->boundaryPointsValid() || !range->startContainer()->inDocument())
        return -1;

    // Check if the match is already selected.
    TextFinder& mainFrameTextFinder = ownerFrame().viewImpl()->mainFrameImpl()->ensureTextFinder();
    WebLocalFrameImpl* activeMatchFrame = mainFrameTextFinder.m_currentActiveMatchFrame;
    if (&ownerFrame() != activeMatchFrame || !m_activeMatch || !areRangesEqual(m_activeMatch.get(), range.get())) {
        if (isActiveMatchFrameValid())
            activeMatchFrame->ensureTextFinder().setMatchMarkerActive(false);

        m_activeMatchIndexInCurrentFrame = m_findMatchesCache[index].m_ordinal - 1;

        // Set this frame as the active frame (the one with the active highlight).
        mainFrameTextFinder.m_currentActiveMatchFrame = &ownerFrame();
        ownerFrame().viewImpl()->setFocusedFrame(&ownerFrame());

        m_activeMatch = range.release();
        setMarkerActive(m_activeMatch.get(), true);

        // Clear any user selection, to make sure Find Next continues on from the match we just activated.
        ownerFrame().frame()->selection().clear();

        // Make sure no node is focused. See http://crbug.com/38700.
        ownerFrame().frame()->document()->setFocusedElement(nullptr);
    }

    IntRect activeMatchRect;
    IntRect activeMatchBoundingBox = enclosingIntRect(LayoutObject::absoluteBoundingBoxRectForRange(m_activeMatch.get()));

    if (!activeMatchBoundingBox.isEmpty()) {
        if (m_activeMatch->firstNode() && m_activeMatch->firstNode()->layoutObject()) {
            m_activeMatch->firstNode()->layoutObject()->scrollRectToVisible(
                LayoutRect(activeMatchBoundingBox), ScrollAlignment::alignCenterIfNeeded, ScrollAlignment::alignCenterIfNeeded);
        }

        // Zoom to the active match.
        activeMatchRect = ownerFrame().frameView()->contentsToRootFrame(activeMatchBoundingBox);
        ownerFrame().viewImpl()->zoomToFindInPageRect(activeMatchRect);
    }

    if (selectionRect)
        *selectionRect = activeMatchRect;

    return ordinalOfFirstMatch() + m_activeMatchIndexInCurrentFrame + 1;
}

PassOwnPtrWillBeRawPtr<TextFinder> TextFinder::create(WebLocalFrameImpl& ownerFrame)
{
    return adoptPtrWillBeNoop(new TextFinder(ownerFrame));
}

TextFinder::TextFinder(WebLocalFrameImpl& ownerFrame)
    : m_ownerFrame(&ownerFrame)
    , m_currentActiveMatchFrame(nullptr)
    , m_activeMatchIndexInCurrentFrame(-1)
    , m_resumeScopingFromRange(nullptr)
    , m_lastMatchCount(-1)
    , m_totalMatchCount(-1)
    , m_framesScopingCount(-1)
    , m_findRequestIdentifier(-1)
    , m_nextInvalidateAfter(0)
    , m_findMatchMarkersVersion(0)
    , m_locatingActiveRect(false)
    , m_scopingInProgress(false)
    , m_lastFindRequestCompletedWithNoMatches(false)
    , m_findMatchRectsAreValid(false)
{
}

TextFinder::~TextFinder()
{
#if !ENABLE(OILPAN)
    cancelPendingScopingEffort();
#endif
}

void TextFinder::addMarker(Range* range, bool activeMatch)
{
    ownerFrame().frame()->document()->markers().addTextMatchMarker(range, activeMatch);
}

void TextFinder::setMarkerActive(Range* range, bool active)
{
    if (!range || range->collapsed())
        return;
    ownerFrame().frame()->document()->markers().setMarkersActive(range, active);
}

int TextFinder::ordinalOfFirstMatchForFrame(WebLocalFrameImpl* frame) const
{
    int ordinal = 0;
    WebLocalFrameImpl* mainFrameImpl = ownerFrame().viewImpl()->mainFrameImpl();
    // Iterate from the main frame up to (but not including) |frame| and
    // add up the number of matches found so far.
    for (WebLocalFrameImpl* it = mainFrameImpl; it != frame; it = toWebLocalFrameImpl(it->traverseNext(true))) {
        TextFinder& finder = it->ensureTextFinder();
        if (finder.m_lastMatchCount > 0)
            ordinal += finder.m_lastMatchCount;
    }
    return ordinal;
}

bool TextFinder::shouldScopeMatches(const String& searchText)
{
    // Don't scope if we can't find a frame or a view.
    // The user may have closed the tab/application, so abort.
    // Also ignore detached frames, as many find operations report to the main frame.
    LocalFrame* frame = ownerFrame().frame();
    if (!frame || !frame->view() || !frame->page() || !ownerFrame().hasVisibleContent())
        return false;

    ASSERT(frame->document() && frame->view());

    // If the frame completed the scoping operation and found 0 matches the last
    // time it was searched, then we don't have to search it again if the user is
    // just adding to the search string or sending the same search string again.
    if (m_lastFindRequestCompletedWithNoMatches && !m_lastSearchString.isEmpty()) {
        // Check to see if the search string prefixes match.
        String previousSearchPrefix =
            searchText.substring(0, m_lastSearchString.length());

        if (previousSearchPrefix == m_lastSearchString)
            return false; // Don't search this frame, it will be fruitless.
    }

    return true;
}

void TextFinder::scopeStringMatchesSoon(int identifier, const WebString& searchText, const WebFindOptions& options, bool reset)
{
    m_deferredScopingWork.append(DeferredScopeStringMatches::create(this, identifier, searchText, options, reset));
}

void TextFinder::callScopeStringMatches(DeferredScopeStringMatches* caller, int identifier, const WebString& searchText, const WebFindOptions& options, bool reset)
{
    size_t index = m_deferredScopingWork.find(caller);
#if !ENABLE(OILPAN)
    // Finalization needs to be delayed as (m_)searchText is passed by reference.
    OwnPtr<DeferredScopeStringMatches> item = index != kNotFound ? m_deferredScopingWork[index].release() : nullptr;
#endif
    m_deferredScopingWork.remove(index);

    scopeStringMatches(identifier, searchText, options, reset);
}

void TextFinder::invalidateIfNecessary()
{
    if (m_lastMatchCount <= m_nextInvalidateAfter)
        return;

    // FIXME: (http://crbug.com/6819) Optimize the drawing of the tickmarks and
    // remove this. This calculation sets a milestone for when next to
    // invalidate the scrollbar and the content area. We do this so that we
    // don't spend too much time drawing the scrollbar over and over again.
    // Basically, up until the first 500 matches there is no throttle.
    // After the first 500 matches, we set set the milestone further and
    // further out (750, 1125, 1688, 2K, 3K).
    static const int startSlowingDownAfter = 500;
    static const int slowdown = 750;

    int i = m_lastMatchCount / startSlowingDownAfter;
    m_nextInvalidateAfter += i * slowdown;
    ownerFrame().frameView()->invalidatePaintForTickmarks();
}

void TextFinder::flushCurrentScoping()
{
    flushCurrentScopingEffort(m_findRequestIdentifier);
}

void TextFinder::setMatchMarkerActive(bool active)
{
    setMarkerActive(m_activeMatch.get(), active);
}

void TextFinder::decrementFramesScopingCount(int identifier)
{
    // This frame has no further scoping left, so it is done. Other frames might,
    // of course, continue to scope matches.
    --m_framesScopingCount;

    // If this is the last frame to finish scoping we need to trigger the final
    // update to be sent.
    if (!m_framesScopingCount)
        ownerFrame().increaseMatchCount(0, identifier);
}

int TextFinder::ordinalOfFirstMatch() const
{
    return ordinalOfFirstMatchForFrame(m_ownerFrame.get());
}

DEFINE_TRACE(TextFinder)
{
#if ENABLE(OILPAN)
    visitor->trace(m_ownerFrame);
    visitor->trace(m_currentActiveMatchFrame);
    visitor->trace(m_activeMatch);
    visitor->trace(m_resumeScopingFromRange);
    visitor->trace(m_deferredScopingWork);
    visitor->trace(m_findMatchesCache);
#endif
}

} // namespace blink
