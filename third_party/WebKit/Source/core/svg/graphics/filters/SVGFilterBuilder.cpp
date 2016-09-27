/*
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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

#include "config.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"

#include "platform/graphics/filters/SourceAlpha.h"
#include "platform/graphics/filters/SourceGraphic.h"

namespace blink {

SVGFilterBuilder::SVGFilterBuilder(PassRefPtrWillBeRawPtr<FilterEffect> sourceGraphic)
{
    RefPtrWillBeRawPtr<FilterEffect> sourceGraphicRef = sourceGraphic;
    m_builtinEffects.add(SourceGraphic::effectName(), sourceGraphicRef);
    m_builtinEffects.add(SourceAlpha::effectName(), SourceAlpha::create(sourceGraphicRef.get()));
    addBuiltinEffects();
}

DEFINE_TRACE(SVGFilterBuilder)
{
#if ENABLE(OILPAN)
    visitor->trace(m_builtinEffects);
    visitor->trace(m_namedEffects);
    visitor->trace(m_effectRenderer);
    visitor->trace(m_effectReferences);
    visitor->trace(m_lastEffect);
#endif
}

void SVGFilterBuilder::add(const AtomicString& id, PassRefPtrWillBeRawPtr<FilterEffect> effect)
{
    if (id.isEmpty()) {
        m_lastEffect = effect;
        return;
    }

    if (m_builtinEffects.contains(id))
        return;

    m_lastEffect = effect;
    m_namedEffects.set(id, m_lastEffect);
}

FilterEffect* SVGFilterBuilder::getEffectById(const AtomicString& id) const
{
    if (!id.isEmpty()) {
        if (FilterEffect* builtinEffect = m_builtinEffects.get(id))
            return builtinEffect;

        if (FilterEffect* namedEffect = m_namedEffects.get(id))
            return namedEffect;
    }

    if (m_lastEffect)
        return m_lastEffect.get();

    return m_builtinEffects.get(SourceGraphic::effectName());
}

void SVGFilterBuilder::appendEffectToEffectReferences(PassRefPtrWillBeRawPtr<FilterEffect> prpEffect, LayoutObject* object)
{
    RefPtrWillBeRawPtr<FilterEffect> effect = prpEffect;

    // The effect must be a newly created filter effect.
    ASSERT(!m_effectReferences.contains(effect));
    ASSERT(object && !m_effectRenderer.contains(object));
    m_effectReferences.add(effect, FilterEffectSet());

    unsigned numberOfInputEffects = effect->inputEffects().size();

    // It is not possible to add the same value to a set twice.
    for (unsigned i = 0; i < numberOfInputEffects; ++i)
        effectReferences(effect->inputEffect(i)).add(effect.get());
    m_effectRenderer.add(object, effect.get());
}

void SVGFilterBuilder::clearEffects()
{
    m_lastEffect = nullptr;
    m_namedEffects.clear();
    m_effectReferences.clear();
    m_effectRenderer.clear();
    addBuiltinEffects();
}

void SVGFilterBuilder::clearResultsRecursive(FilterEffect* effect)
{
    if (!effect->hasImageFilter())
        return;

    effect->clearResult();

    FilterEffectSet& effectReferences = this->effectReferences(effect);
    for (FilterEffect* effectReference : effectReferences)
        clearResultsRecursive(effectReference);
}

} // namespace blink
