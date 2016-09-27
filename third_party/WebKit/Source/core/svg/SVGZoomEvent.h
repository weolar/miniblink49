/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2006 Samuel Weinig <sam.weinig@gmail.com>
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
 */

#ifndef SVGZoomEvent_h
#define SVGZoomEvent_h

#include "core/events/UIEvent.h"
#include "core/svg/SVGPoint.h"
#include "core/svg/SVGRect.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGZoomEvent final : public UIEvent {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGZoomEvent> create()
    {
        return adoptRefWillBeNoop(new SVGZoomEvent);
    }

    // 'SVGZoomEvent' functions
    PassRefPtrWillBeRawPtr<SVGRectTearOff> zoomRectScreen() const;

    float previousScale() const;

    PassRefPtrWillBeRawPtr<SVGPointTearOff> previousTranslate() const;

    float newScale() const;

    PassRefPtrWillBeRawPtr<SVGPointTearOff> newTranslate() const;

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    SVGZoomEvent();

    float m_newScale;
    float m_previousScale;

    FloatPoint m_newTranslate;
    FloatPoint m_previousTranslate;
};

} // namespace blink

#endif // SVGZoomEvent_h
