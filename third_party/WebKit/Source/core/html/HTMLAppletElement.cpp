/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Stefan Schimanski (1Stein@gmx.de)
 * Copyright (C) 2004, 2005, 2006, 2008, 2009, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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
#include "core/html/HTMLAppletElement.h"

#include "core/HTMLNames.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/HTMLParamElement.h"
#include "core/layout/LayoutApplet.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/plugins/PluginPlaceholder.h"
#include "platform/Widget.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

using namespace HTMLNames;

HTMLAppletElement::HTMLAppletElement(Document& document, bool createdByParser)
    : HTMLPlugInElement(appletTag, document, createdByParser, ShouldNotPreferPlugInsForImages)
{
    m_serviceType = "application/x-java-applet";
}

PassRefPtrWillBeRawPtr<HTMLAppletElement> HTMLAppletElement::create(Document& document, bool createdByParser)
{
    RefPtrWillBeRawPtr<HTMLAppletElement> element = adoptRefWillBeNoop(new HTMLAppletElement(document, createdByParser));
    element->ensureUserAgentShadowRoot();
    return element.release();
}

void HTMLAppletElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == altAttr
        || name == archiveAttr
        || name == codeAttr
        || name == codebaseAttr
        || name == mayscriptAttr
        || name == objectAttr) {
        // Do nothing.
        return;
    }

    HTMLPlugInElement::parseAttribute(name, value);
}

bool HTMLAppletElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == codebaseAttr || attribute.name() == objectAttr
        || HTMLPlugInElement::isURLAttribute(attribute);
}

bool HTMLAppletElement::hasLegalLinkAttribute(const QualifiedName& name) const
{
    return name == codebaseAttr || HTMLPlugInElement::hasLegalLinkAttribute(name);
}

bool HTMLAppletElement::layoutObjectIsNeeded(const ComputedStyle& style)
{
    if (!fastHasAttribute(codeAttr) && !hasOpenShadowRoot())
        return false;
    return HTMLPlugInElement::layoutObjectIsNeeded(style);
}

LayoutObject* HTMLAppletElement::createLayoutObject(const ComputedStyle& style)
{
    if (!canEmbedJava() || hasOpenShadowRoot())
        return LayoutObject::createObject(this, style);

    if (usePlaceholderContent())
        return new LayoutBlockFlow(this);

    return new LayoutApplet(this);
}

LayoutPart* HTMLAppletElement::layoutPartForJSBindings() const
{
    if (!canEmbedJava())
        return nullptr;
    return HTMLPlugInElement::layoutPartForJSBindings();
}

LayoutPart* HTMLAppletElement::existingLayoutPart() const
{
    return layoutPart();
}

void HTMLAppletElement::updateWidgetInternal()
{
    setNeedsWidgetUpdate(false);
    // FIXME: This should ASSERT isFinishedParsingChildren() instead.
    if (!isFinishedParsingChildren())
        return;

    LayoutEmbeddedObject* layoutObject = layoutEmbeddedObject();

    LocalFrame* frame = document().frame();
    ASSERT(frame);

    Vector<String> paramNames;
    Vector<String> paramValues;

    const AtomicString& codeBase = getAttribute(codebaseAttr);
    if (!codeBase.isNull()) {
        KURL codeBaseURL = document().completeURL(codeBase);
        paramNames.append("codeBase");
        paramValues.append(codeBase.string());
    }

    const AtomicString& archive = getAttribute(archiveAttr);
    if (!archive.isNull()) {
        paramNames.append("archive");
        paramValues.append(archive.string());
    }

    const AtomicString& code = getAttribute(codeAttr);
    paramNames.append("code");
    paramValues.append(code.string());

    // If the 'codebase' attribute is set, it serves as a relative root for the file that the Java
    // plugin will load. If the 'code' attribute is set, and the 'archive' is not set, then we need
    // to check the url generated by resolving 'code' against 'codebase'. If the 'archive'
    // attribute is set, then 'code' points to a class inside the archive, so we need to check the
    // url generated by resolving 'archive' against 'codebase'.
    KURL urlToCheck;
    KURL rootURL;
    if (!codeBase.isNull())
        rootURL = document().completeURL(codeBase);
    if (rootURL.isNull() || !rootURL.isValid())
        rootURL = document().url();

    if (!archive.isNull())
        urlToCheck = KURL(rootURL, archive);
    else if (!code.isNull())
        urlToCheck = KURL(rootURL, code);
    if (!canEmbedURL(urlToCheck))
        return;

    const AtomicString& name = document().isHTMLDocument() ? getNameAttribute() : getIdAttribute();
    if (!name.isNull()) {
        paramNames.append("name");
        paramValues.append(name.string());
    }

    paramNames.append("baseURL");
    KURL baseURL = document().baseURL();
    paramValues.append(baseURL.string());

    const AtomicString& mayScript = getAttribute(mayscriptAttr);
    if (!mayScript.isNull()) {
        paramNames.append("mayScript");
        paramValues.append(mayScript.string());
    }

    for (HTMLParamElement* param = Traversal<HTMLParamElement>::firstChild(*this); param; param = Traversal<HTMLParamElement>::nextSibling(*param)) {
        if (param->name().isEmpty())
            continue;

        paramNames.append(param->name());
        paramValues.append(param->value());
    }

    OwnPtrWillBeRawPtr<PluginPlaceholder> placeholder = nullptr;
    RefPtrWillBeRawPtr<Widget> widget = nullptr;
    if (frame->loader().allowPlugins(AboutToInstantiatePlugin)) {
        placeholder = frame->loader().client()->createPluginPlaceholder(document(), KURL(), paramNames, paramValues, m_serviceType, false);
        if (!placeholder)
            widget = frame->loader().client()->createJavaAppletWidget(this, baseURL, paramNames, paramValues);
    }

    if (!placeholder && !widget) {
        if (!layoutObject->showsUnavailablePluginIndicator())
            layoutObject->setPluginUnavailabilityReason(LayoutEmbeddedObject::PluginMissing);
        setPlaceholder(nullptr);
    } else if (placeholder) {
        setPlaceholder(placeholder.release());
    } else if (widget) {
        document().setContainsPlugins();
        setWidget(widget);
        setPlaceholder(nullptr);
    }
}

bool HTMLAppletElement::canEmbedJava() const
{
    if (document().isSandboxed(SandboxPlugins))
        return false;

    Settings* settings = document().settings();
    if (!settings)
        return false;

    if (!settings->javaEnabled())
        return false;

    return true;
}

bool HTMLAppletElement::canEmbedURL(const KURL& url) const
{
    if (!document().securityOrigin()->canDisplay(url)) {
        FrameLoader::reportLocalLoadFailed(document().frame(), url.string());
        return false;
    }

    if (!document().contentSecurityPolicy()->allowObjectFromSource(url)
        || !document().contentSecurityPolicy()->allowPluginTypeForDocument(document(), m_serviceType, m_serviceType, url)) {
        layoutEmbeddedObject()->setPluginUnavailabilityReason(LayoutEmbeddedObject::PluginBlockedByContentSecurityPolicy);
        return false;
    }
    return true;
}

}
