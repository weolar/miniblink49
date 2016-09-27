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

#include "config.h"
#include "core/layout/svg/SVGTextLayoutEngineSpacing.h"

#include "platform/fonts/Character.h"
#include "platform/fonts/Font.h"

namespace blink {

SVGTextLayoutEngineSpacing::SVGTextLayoutEngineSpacing(const Font& font, float effectiveZoom)
    : m_font(font)
    , m_lastCharacter(0)
    , m_effectiveZoom(effectiveZoom)
{
    ASSERT(m_effectiveZoom);
}

float SVGTextLayoutEngineSpacing::calculateCSSSpacing(UChar currentCharacter)
{
    UChar lastCharacter = m_lastCharacter;
    m_lastCharacter = currentCharacter;

    if (!m_font.fontDescription().letterSpacing() && !m_font.fontDescription().wordSpacing())
        return 0;

    float spacing = m_font.fontDescription().letterSpacing();
    if (currentCharacter && lastCharacter && m_font.fontDescription().wordSpacing()) {
        if (Character::treatAsSpace(currentCharacter) && !Character::treatAsSpace(lastCharacter))
            spacing += m_font.fontDescription().wordSpacing();
    }

    if (m_effectiveZoom != 1)
        spacing = spacing / m_effectiveZoom;

    return spacing;
}

}
