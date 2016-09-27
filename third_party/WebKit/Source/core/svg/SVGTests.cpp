/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
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
#include "core/svg/SVGTests.h"

#include "core/SVGNames.h"
#include "core/svg/SVGElement.h"
#include "platform/Language.h"

namespace blink {

SVGTests::SVGTests(SVGElement* contextElement)
    : m_requiredFeatures(SVGStaticStringList::create(contextElement, SVGNames::requiredFeaturesAttr))
    , m_requiredExtensions(SVGStaticStringList::create(contextElement, SVGNames::requiredExtensionsAttr))
    , m_systemLanguage(SVGStaticStringList::create(contextElement, SVGNames::systemLanguageAttr))
{
    ASSERT(contextElement);

    contextElement->addToPropertyMap(m_requiredFeatures);
    contextElement->addToPropertyMap(m_requiredExtensions);
    contextElement->addToPropertyMap(m_systemLanguage);
}

DEFINE_TRACE(SVGTests)
{
    visitor->trace(m_requiredFeatures);
    visitor->trace(m_requiredExtensions);
    visitor->trace(m_systemLanguage);
}

bool SVGTests::hasExtension(const String&)
{
    // FIXME: Implement me!
    return false;
}

bool SVGTests::isValid(Document& document) const
{
    // No need to check requiredFeatures since hasFeature always returns true.

    if (m_systemLanguage->isSpecified()) {
        bool matchFound = false;

        const Vector<String>& systemLanguage = m_systemLanguage->value()->values();
        for (const auto& value : systemLanguage) {
            if (value == defaultLanguage().string().substring(0, 2)) {
                matchFound = true;
                break;
            }
        }

        if (!matchFound)
            return false;
    }

    if (!m_requiredExtensions->value()->values().isEmpty())
        return false;

    return true;
}

bool SVGTests::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    // FIXME: Should handle exceptions here.
    // This is safe as of now, as the current impl of SVGStringList::setValueAsString never fails.
    SVGParsingError parseError = NoError;

    if (name == SVGNames::requiredFeaturesAttr)
        m_requiredFeatures->setBaseValueAsString(value, parseError);
    else if (name == SVGNames::requiredExtensionsAttr)
        m_requiredExtensions->setBaseValueAsString(value, parseError);
    else if (name == SVGNames::systemLanguageAttr)
        m_systemLanguage->setBaseValueAsString(value, parseError);
    else
        return false;

    ASSERT(parseError == NoError);

    return true;
}

bool SVGTests::isKnownAttribute(const QualifiedName& attrName)
{
    return attrName == SVGNames::requiredFeaturesAttr
        || attrName == SVGNames::requiredExtensionsAttr
        || attrName == SVGNames::systemLanguageAttr;
}

void SVGTests::addSupportedAttributes(HashSet<QualifiedName>& supportedAttributes)
{
    supportedAttributes.add(SVGNames::requiredFeaturesAttr);
    supportedAttributes.add(SVGNames::requiredExtensionsAttr);
    supportedAttributes.add(SVGNames::systemLanguageAttr);
}

}
