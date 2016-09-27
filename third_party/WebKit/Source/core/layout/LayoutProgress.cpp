/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"

#include "core/layout/LayoutProgress.h"

#include "core/html/HTMLProgressElement.h"
#include "core/layout/LayoutTheme.h"
#include "wtf/CurrentTime.h"
#include "wtf/RefPtr.h"

namespace blink {

LayoutProgress::LayoutProgress(HTMLElement* element)
    : LayoutBlockFlow(element)
    , m_position(HTMLProgressElement::InvalidPosition)
    , m_animationStartTime(0)
    , m_animationRepeatInterval(0)
    , m_animationDuration(0)
    , m_animating(false)
    , m_animationTimer(this, &LayoutProgress::animationTimerFired)
{
}

LayoutProgress::~LayoutProgress()
{
}

void LayoutProgress::willBeDestroyed()
{
    if (m_animating) {
        m_animationTimer.stop();
        m_animating = false;
    }
    LayoutBlockFlow::willBeDestroyed();
}

void LayoutProgress::updateFromElement()
{
    HTMLProgressElement* element = progressElement();
    if (m_position == element->position())
        return;
    m_position = element->position();

    updateAnimationState();
    setShouldDoFullPaintInvalidation();
    LayoutBlockFlow::updateFromElement();
}

double LayoutProgress::animationProgress() const
{
    return m_animating ? (fmod((currentTime() - m_animationStartTime), m_animationDuration) / m_animationDuration) : 0;
}

bool LayoutProgress::isDeterminate() const
{
    return (HTMLProgressElement::IndeterminatePosition != position()
        && HTMLProgressElement::InvalidPosition != position());
}

void LayoutProgress::animationTimerFired(Timer<LayoutProgress>*)
{
    setShouldDoFullPaintInvalidation();
    if (!m_animationTimer.isActive() && m_animating)
        m_animationTimer.startOneShot(m_animationRepeatInterval, FROM_HERE);
}

void LayoutProgress::updateAnimationState()
{
    m_animationDuration = LayoutTheme::theme().animationDurationForProgressBar();
    m_animationRepeatInterval = LayoutTheme::theme().animationRepeatIntervalForProgressBar();

    bool animating = style()->hasAppearance() && m_animationDuration > 0;
    if (animating == m_animating)
        return;

    m_animating = animating;
    if (m_animating) {
        m_animationStartTime = currentTime();
        m_animationTimer.startOneShot(m_animationRepeatInterval, FROM_HERE);
    } else {
        m_animationTimer.stop();
    }
}

HTMLProgressElement* LayoutProgress::progressElement() const
{
    if (!node())
        return nullptr;

    if (isHTMLProgressElement(*node()))
        return toHTMLProgressElement(node());

    ASSERT(node()->shadowHost());
    return toHTMLProgressElement(node()->shadowHost());
}

} // namespace blink
