/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "core/dom/CSSSelectorWatch.h"

#include "core/css/StylePropertySet.h"
#include "core/css/parser/CSSParser.h"
#include "core/dom/Document.h"
#include "core/dom/ExecutionContext.h"
#include "core/frame/LocalFrame.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/style/StyleRareNonInheritedData.h"

namespace blink {

// The address of this string is important; its value is just documentation.
static const char kSupplementName[] = "CSSSelectorWatch";

CSSSelectorWatch::CSSSelectorWatch(Document& document)
    : m_document(document)
    , m_callbackSelectorChangeTimer(this, &CSSSelectorWatch::callbackSelectorChangeTimerFired)
    , m_timerExpirations(0)
{
}

CSSSelectorWatch& CSSSelectorWatch::from(Document& document)
{
    CSSSelectorWatch* watch = fromIfExists(document);
    if (!watch) {
        watch = new CSSSelectorWatch(document);
        WillBeHeapSupplement<Document>::provideTo(document, kSupplementName, adoptPtrWillBeNoop(watch));
    }
    return *watch;
}

CSSSelectorWatch* CSSSelectorWatch::fromIfExists(Document& document)
{
    return static_cast<CSSSelectorWatch*>(WillBeHeapSupplement<Document>::from(document, kSupplementName));
}

void CSSSelectorWatch::callbackSelectorChangeTimerFired(Timer<CSSSelectorWatch>*)
{
    // Should be ensured by updateSelectorMatches():
    ASSERT(!m_addedSelectors.isEmpty() || !m_removedSelectors.isEmpty());

    if (m_timerExpirations < 1) {
        m_timerExpirations++;
        m_callbackSelectorChangeTimer.startOneShot(0, FROM_HERE);
        return;
    }
    if (document().frame()) {
        Vector<String> addedSelectors;
        Vector<String> removedSelectors;
        copyToVector(m_addedSelectors, addedSelectors);
        copyToVector(m_removedSelectors, removedSelectors);
        document().frame()->loader().client()->selectorMatchChanged(addedSelectors, removedSelectors);
    }
    m_addedSelectors.clear();
    m_removedSelectors.clear();
    m_timerExpirations = 0;
}

void CSSSelectorWatch::updateSelectorMatches(const Vector<String>& removedSelectors, const Vector<String>& addedSelectors)
{
    bool shouldUpdateTimer = false;

    for (unsigned i = 0; i < removedSelectors.size(); ++i) {
        const String& selector = removedSelectors[i];
        if (!m_matchingCallbackSelectors.remove(selector))
            continue;

        // Count reached 0.
        shouldUpdateTimer = true;
        if (m_addedSelectors.contains(selector))
            m_addedSelectors.remove(selector);
        else
            m_removedSelectors.add(selector);
    }

    for (unsigned i = 0; i < addedSelectors.size(); ++i) {
        const String& selector = addedSelectors[i];
        HashCountedSet<String>::AddResult result = m_matchingCallbackSelectors.add(selector);
        if (!result.isNewEntry)
            continue;

        shouldUpdateTimer = true;
        if (m_removedSelectors.contains(selector))
            m_removedSelectors.remove(selector);
        else
            m_addedSelectors.add(selector);
    }

    if (!shouldUpdateTimer)
        return;

    if (m_removedSelectors.isEmpty() && m_addedSelectors.isEmpty()) {
        if (m_callbackSelectorChangeTimer.isActive()) {
            m_timerExpirations = 0;
            m_callbackSelectorChangeTimer.stop();
        }
    } else {
        m_timerExpirations = 0;
        if (!m_callbackSelectorChangeTimer.isActive())
            m_callbackSelectorChangeTimer.startOneShot(0, FROM_HERE);
    }
}

static bool allCompound(const CSSSelectorList& selectorList)
{
    for (const CSSSelector* selector = selectorList.first(); selector; selector = selectorList.next(*selector)) {
        if (!selector->isCompound())
            return false;
    }
    return true;
}

void CSSSelectorWatch::watchCSSSelectors(const Vector<String>& selectors)
{
    m_watchedCallbackSelectors.clear();

    const RefPtrWillBeRawPtr<StylePropertySet> callbackPropertySet = ImmutableStylePropertySet::create(nullptr, 0, UASheetMode);

    CSSSelectorList selectorList;
    for (unsigned i = 0; i < selectors.size(); ++i) {
        CSSParser::parseSelector(CSSParserContext(UASheetMode, 0), selectors[i], selectorList);
        if (!selectorList.isValid())
            continue;

        // Only accept Compound Selectors, since they're cheaper to match.
        if (!allCompound(selectorList))
            continue;

        m_watchedCallbackSelectors.append(StyleRule::create(selectorList, callbackPropertySet));
    }
    document().changedSelectorWatch();
}

DEFINE_TRACE(CSSSelectorWatch)
{
    visitor->trace(m_watchedCallbackSelectors);
    visitor->trace(m_document);
    WillBeHeapSupplement<Document>::trace(visitor);
}

} // namespace blink
