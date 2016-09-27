/*
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LayoutProgress_h
#define LayoutProgress_h

#include "core/CoreExport.h"
#include "core/layout/LayoutBlockFlow.h"

namespace blink {

class HTMLProgressElement;

class CORE_EXPORT LayoutProgress final : public LayoutBlockFlow {
public:
    explicit LayoutProgress(HTMLElement*);
    virtual ~LayoutProgress();

    double position() const { return m_position; }
    double animationProgress() const;

    bool isDeterminate() const;
    virtual void updateFromElement() override;

    HTMLProgressElement* progressElement() const;

    virtual const char* name() const override { return "LayoutProgress"; }

protected:
    virtual void willBeDestroyed() override;

private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectProgress || LayoutBlockFlow::isOfType(type); }

    void animationTimerFired(Timer<LayoutProgress>*);
    void updateAnimationState();

    double m_position;
    double m_animationStartTime;
    double m_animationRepeatInterval;
    double m_animationDuration;
    bool m_animating;
    Timer<LayoutProgress> m_animationTimer;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutProgress, isProgress());

} // namespace blink

#endif // LayoutProgress_h
