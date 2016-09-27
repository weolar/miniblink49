// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/page/scrolling/ScrollState.h"

#include "core/dom/Element.h"
#include "core/dom/ExceptionCode.h"

namespace blink {

PassRefPtrWillBeRawPtr<ScrollState> ScrollState::create(double deltaX, double deltaY,
    double deltaGranularity, double velocityX, double velocityY, bool inInertialPhase,
    bool isBeginning, bool isEnding, bool fromUserInput, bool shouldPropagate,
    bool deltaConsumedForScrollSequence)
{
    return adoptRefWillBeNoop(new ScrollState(
        deltaX, deltaY, deltaGranularity, velocityX, velocityY,
        inInertialPhase, isBeginning, isEnding, fromUserInput, shouldPropagate,
        deltaConsumedForScrollSequence));
}

ScrollState::ScrollState(double deltaX, double deltaY, double deltaGranularity,
    double velocityX, double velocityY, bool inInertialPhase, bool isBeginning,
    bool isEnding, bool fromUserInput, bool shouldPropagate,
    bool deltaConsumedForScrollSequence)
    : m_deltaX(deltaX)
    , m_deltaY(deltaY)
    , m_deltaGranularity(deltaGranularity)
    , m_velocityX(velocityX)
    , m_velocityY(velocityY)
    , m_inInertialPhase(inInertialPhase)
    , m_isBeginning(isBeginning)
    , m_isEnding(isEnding)
    , m_fromUserInput(fromUserInput)
    , m_shouldPropagate(shouldPropagate)
    , m_deltaConsumedForScrollSequence(deltaConsumedForScrollSequence)
{
}

void ScrollState::consumeDelta(double x, double y, ExceptionState& exceptionState)
{
    if ((m_deltaX > 0 && 0 > x) || (m_deltaX < 0 && 0 < x) || (m_deltaY > 0 && 0 > y) || (m_deltaY < 0 && 0 < y)) {
        exceptionState.throwDOMException(InvalidModificationError, "Can't increase delta using consumeDelta");
        return;
    }
    if (fabs(x) > fabs(m_deltaX) || fabs(y) > fabs(m_deltaY)) {
        exceptionState.throwDOMException(InvalidModificationError, "Can't change direction of delta using consumeDelta");
        return;
    }
    consumeDeltaNative(x, y);
}

void ScrollState::distributeToScrollChainDescendant()
{
    if (!m_scrollChain.isEmpty())
        m_scrollChain.takeFirst()->distributeScroll(*this);
}

void ScrollState::consumeDeltaNative(double x, double y)
{
    m_deltaX -= x;
    m_deltaY -= y;

    if (x || y)
        m_deltaConsumedForScrollSequence = true;
}

} // namespace blink
