// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/canvas2d/HitRegion.h"

#include "core/dom/AXObjectCache.h"
#include "core/layout/LayoutBoxModelObject.h"

namespace blink {

HitRegion::HitRegion(const Path& path, const HitRegionOptions& options)
    : m_id(options.id())
    , m_control(options.control())
    , m_path(path)
{
    if (options.fillRule() != "evenodd")
        m_fillRule = RULE_NONZERO;
    else
        m_fillRule = RULE_EVENODD;
}

void HitRegion::updateAccessibility(Element* canvas)
{
    if (!m_control || !canvas || !canvas->layoutObject() || !m_control->isDescendantOf(canvas))
        return;

    AXObjectCache* axObjectCache = m_control->document().existingAXObjectCache();
    if (!axObjectCache)
        return;

    FloatRect boundingRect = m_path.boundingRect();

    // Offset by the canvas rect, taking border and padding into account.
    LayoutBoxModelObject* rbmo = canvas->layoutBoxModelObject();
    IntRect canvasRect = canvas->layoutObject()->absoluteBoundingBoxRect();
    canvasRect.move(rbmo->borderLeft() + rbmo->paddingLeft(),
        rbmo->borderTop() + rbmo->paddingTop());
    LayoutRect elementRect = enclosingLayoutRect(boundingRect);
    elementRect.moveBy(canvasRect.location());

    axObjectCache->setCanvasObjectBounds(m_control.get(), elementRect);
}

bool HitRegion::contains(const LayoutPoint& point) const
{
    return m_path.contains(FloatPoint(point), m_fillRule);
}

bool HitRegion::contains(const FloatPoint& point) const
{
    return m_path.contains(point, m_fillRule);
}

void HitRegion::removePixels(const Path& clearArea)
{
    m_path.subtractPath(clearArea);
}

DEFINE_TRACE(HitRegion)
{
    visitor->trace(m_control);
}

void HitRegionManager::addHitRegion(PassRefPtrWillBeRawPtr<HitRegion> passHitRegion)
{
    RefPtrWillBeRawPtr<HitRegion> hitRegion = passHitRegion;

    m_hitRegionList.add(hitRegion);

    if (!hitRegion->id().isEmpty())
        m_hitRegionIdMap.set(hitRegion->id(), hitRegion);

    if (hitRegion->control())
        m_hitRegionControlMap.set(hitRegion->control(), hitRegion);
}

void HitRegionManager::removeHitRegion(HitRegion* hitRegion)
{
    if (!hitRegion)
        return;

    if (!hitRegion->id().isEmpty())
        m_hitRegionIdMap.remove(hitRegion->id());

    if (hitRegion->control())
        m_hitRegionControlMap.remove(hitRegion->control());

    m_hitRegionList.remove(hitRegion);
}

void HitRegionManager::removeHitRegionById(const String& id)
{
    if (!id.isEmpty())
        removeHitRegion(getHitRegionById(id));
}

void HitRegionManager::removeHitRegionByControl(Element* control)
{
    removeHitRegion(getHitRegionByControl(control));
}

void HitRegionManager::removeHitRegionsInRect(const FloatRect& rect, const AffineTransform& ctm)
{
    Path clearArea;
    clearArea.addRect(rect);
    clearArea.transform(ctm);

    HitRegionIterator itEnd = m_hitRegionList.rend();
    HitRegionList toBeRemoved;

    for (HitRegionIterator it = m_hitRegionList.rbegin(); it != itEnd; ++it) {
        RefPtrWillBeRawPtr<HitRegion> hitRegion = *it;
        hitRegion->removePixels(clearArea);
        if (hitRegion->path().isEmpty())
            toBeRemoved.add(hitRegion);
    }

    itEnd = toBeRemoved.rend();
    for (HitRegionIterator it = toBeRemoved.rbegin(); it != itEnd; ++it)
        removeHitRegion(it->get());
}

void HitRegionManager::removeAllHitRegions()
{
    m_hitRegionList.clear();
    m_hitRegionIdMap.clear();
    m_hitRegionControlMap.clear();
}

HitRegion* HitRegionManager::getHitRegionById(const String& id) const
{
    return m_hitRegionIdMap.get(id);
}

HitRegion* HitRegionManager::getHitRegionByControl(Element* control) const
{
    if (control)
        return m_hitRegionControlMap.get(control);

    return nullptr;
}

HitRegion* HitRegionManager::getHitRegionAtPoint(const LayoutPoint& point) const
{
    HitRegionIterator itEnd = m_hitRegionList.rend();

    for (HitRegionIterator it = m_hitRegionList.rbegin(); it != itEnd; ++it) {
        RefPtrWillBeRawPtr<HitRegion> hitRegion = *it;
        if (hitRegion->contains(point))
            return hitRegion.get();
    }

    return nullptr;
}

unsigned HitRegionManager::getHitRegionsCount() const
{
    return m_hitRegionList.size();
}

DEFINE_TRACE(HitRegionManager)
{
#if ENABLE(OILPAN)
    visitor->trace(m_hitRegionList);
    visitor->trace(m_hitRegionIdMap);
    visitor->trace(m_hitRegionControlMap);
#endif
}

} // namespace blink
