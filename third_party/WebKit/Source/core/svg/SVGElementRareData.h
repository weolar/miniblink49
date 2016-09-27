/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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

#ifndef SVGElementRareData_h
#define SVGElementRareData_h

#include "core/svg/SVGElement.h"
#include "platform/heap/Handle.h"
#include "wtf/HashSet.h"
#include "wtf/Noncopyable.h"
#include "wtf/StdLibExtras.h"

namespace blink {

class CSSCursorImageValue;
class SVGCursorElement;

class SVGElementRareData : public NoBaseWillBeGarbageCollectedFinalized<SVGElementRareData> {
    WTF_MAKE_NONCOPYABLE(SVGElementRareData); WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(SVGElementRareData);
public:
    SVGElementRareData(SVGElement* owner)
#if ENABLE(OILPAN)
        : m_owner(owner)
        , m_cursorElement(nullptr)
#else
        : m_cursorElement(nullptr)
#endif
        , m_cursorImageValue(nullptr)
        , m_correspondingElement(nullptr)
        , m_instancesUpdatesBlocked(false)
        , m_useOverrideComputedStyle(false)
        , m_needsOverrideComputedStyleUpdate(false)
    {
    }

    SVGElementSet& outgoingReferences() { return m_outgoingReferences; }
    const SVGElementSet& outgoingReferences() const { return m_outgoingReferences; }
    SVGElementSet& incomingReferences() { return m_incomingReferences; }
    const SVGElementSet& incomingReferences() const { return m_incomingReferences; }

    WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& elementInstances() { return m_elementInstances; }
    const WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& elementInstances() const { return m_elementInstances; }

    bool instanceUpdatesBlocked() const { return m_instancesUpdatesBlocked; }
    void setInstanceUpdatesBlocked(bool value) { m_instancesUpdatesBlocked = value; }

    SVGCursorElement* cursorElement() const { return m_cursorElement; }
    void setCursorElement(SVGCursorElement* cursorElement) { m_cursorElement = cursorElement; }

    SVGElement* correspondingElement() { return m_correspondingElement.get(); }
    void setCorrespondingElement(SVGElement* correspondingElement) { m_correspondingElement = correspondingElement; }

    CSSCursorImageValue* cursorImageValue() const { return m_cursorImageValue; }
    void setCursorImageValue(CSSCursorImageValue* cursorImageValue) { m_cursorImageValue = cursorImageValue; }

    MutableStylePropertySet* animatedSMILStyleProperties() const { return m_animatedSMILStyleProperties.get(); }
    MutableStylePropertySet* ensureAnimatedSMILStyleProperties();

    ComputedStyle* overrideComputedStyle(Element*, const ComputedStyle*);

    bool useOverrideComputedStyle() const { return m_useOverrideComputedStyle; }
    void setUseOverrideComputedStyle(bool value) { m_useOverrideComputedStyle = value; }
    void setNeedsOverrideComputedStyleUpdate() { m_needsOverrideComputedStyleUpdate = true; }

    AffineTransform* animateMotionTransform();

    DECLARE_TRACE();
    void processWeakMembers(Visitor*);

private:
#if ENABLE(OILPAN)
    Member<SVGElement> m_owner;
#endif
    SVGElementSet m_outgoingReferences;
    SVGElementSet m_incomingReferences;
    WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>> m_elementInstances;
    RawPtrWillBeWeakMember<SVGCursorElement> m_cursorElement;
    RawPtrWillBeWeakMember<CSSCursorImageValue> m_cursorImageValue;
    RefPtrWillBeMember<SVGElement> m_correspondingElement;
    bool m_instancesUpdatesBlocked : 1;
    bool m_useOverrideComputedStyle : 1;
    bool m_needsOverrideComputedStyleUpdate : 1;
    RefPtrWillBeMember<MutableStylePropertySet> m_animatedSMILStyleProperties;
    RefPtr<ComputedStyle> m_overrideComputedStyle;
    // Used by <animateMotion>
    OwnPtr<AffineTransform> m_animateMotionTransform;
};

}

#endif
