/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Stefan Schimanski (1Stein@gmx.de)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2011 Apple Inc. All rights reserved.
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
#include "core/html/HTMLObjectElement.h"

#include "bindings/core/v8/ScriptEventListener.h"
#include "core/HTMLNames.h"
#include "core/dom/Attribute.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/TagCollection.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/fetch/ImageResource.h"
#include "core/frame/Settings.h"
#include "core/html/FormDataList.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLImageLoader.h"
#include "core/html/HTMLMetaElement.h"
#include "core/html/HTMLParamElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/layout/LayoutEmbeddedObject.h"
#include "core/plugins/PluginView.h"
#include "platform/MIMETypeRegistry.h"
#include "platform/Widget.h"

namespace blink {

using namespace HTMLNames;

inline HTMLObjectElement::HTMLObjectElement(Document& document, HTMLFormElement* form, bool createdByParser)
    : HTMLPlugInElement(objectTag, document, createdByParser, ShouldNotPreferPlugInsForImages)
    , m_useFallbackContent(false)
{
    associateByParser(form);
}

inline HTMLObjectElement::~HTMLObjectElement()
{
#if !ENABLE(OILPAN)
    setForm(0);
#endif
}

PassRefPtrWillBeRawPtr<HTMLObjectElement> HTMLObjectElement::create(Document& document, HTMLFormElement* form, bool createdByParser)
{
    RefPtrWillBeRawPtr<HTMLObjectElement> element = adoptRefWillBeNoop(new HTMLObjectElement(document, form, createdByParser));
    element->ensureUserAgentShadowRoot();
    return element.release();
}

DEFINE_TRACE(HTMLObjectElement)
{
    FormAssociatedElement::trace(visitor);
    HTMLPlugInElement::trace(visitor);
}

LayoutPart* HTMLObjectElement::existingLayoutPart() const
{
    return layoutPart(); // This will return 0 if the layoutObject is not a LayoutPart.
}

bool HTMLObjectElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == borderAttr)
        return true;
    return HTMLPlugInElement::isPresentationAttribute(name);
}

void HTMLObjectElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == borderAttr)
        applyBorderAttributeToStyle(value, style);
    else
        HTMLPlugInElement::collectStyleForPresentationAttribute(name, value, style);
}

void HTMLObjectElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == formAttr) {
        formAttributeChanged();
    } else if (name == typeAttr) {
        m_serviceType = value.lower();
        size_t pos = m_serviceType.find(";");
        if (pos != kNotFound)
            m_serviceType = m_serviceType.left(pos);
        // FIXME: What is the right thing to do here? Should we supress the
        // reload stuff when a persistable widget-type is specified?
        reloadPluginOnAttributeChange(name);
        if (!layoutObject())
            requestPluginCreationWithoutLayoutObjectIfPossible();
    } else if (name == dataAttr) {
        m_url = stripLeadingAndTrailingHTMLSpaces(value);
        if (layoutObject() && isImageType()) {
            setNeedsWidgetUpdate(true);
            if (!m_imageLoader)
                m_imageLoader = HTMLImageLoader::create(this);
            m_imageLoader->updateFromElement(ImageLoader::UpdateIgnorePreviousError);
        } else {
            reloadPluginOnAttributeChange(name);
        }
    } else if (name == classidAttr) {
        m_classId = value;
        reloadPluginOnAttributeChange(name);
    } else {
        HTMLPlugInElement::parseAttribute(name, value);
    }
}

static void mapDataParamToSrc(Vector<String>* paramNames, Vector<String>* paramValues)
{
    // Some plugins don't understand the "data" attribute of the OBJECT tag (i.e. Real and WMP
    // require "src" attribute).
    int srcIndex = -1, dataIndex = -1;
    for (unsigned i = 0; i < paramNames->size(); ++i) {
        if (equalIgnoringCase((*paramNames)[i], "src"))
            srcIndex = i;
        else if (equalIgnoringCase((*paramNames)[i], "data"))
            dataIndex = i;
    }

    if (srcIndex == -1 && dataIndex != -1) {
        paramNames->append("src");
        paramValues->append((*paramValues)[dataIndex]);
    }
}

// FIXME: This function should not deal with url or serviceType!
void HTMLObjectElement::parametersForPlugin(Vector<String>& paramNames, Vector<String>& paramValues, String& url, String& serviceType)
{
    HashSet<StringImpl*, CaseFoldingHash> uniqueParamNames;
    String urlParameter;

    // Scan the PARAM children and store their name/value pairs.
    // Get the URL and type from the params if we don't already have them.
    for (HTMLParamElement* p = Traversal<HTMLParamElement>::firstChild(*this); p; p = Traversal<HTMLParamElement>::nextSibling(*p)) {
        String name = p->name();
        if (name.isEmpty())
            continue;

        uniqueParamNames.add(name.impl());
        paramNames.append(p->name());
        paramValues.append(p->value());

        // FIXME: url adjustment does not belong in this function.
        if (url.isEmpty() && urlParameter.isEmpty() && (equalIgnoringCase(name, "src") || equalIgnoringCase(name, "movie") || equalIgnoringCase(name, "code") || equalIgnoringCase(name, "url")))
            urlParameter = stripLeadingAndTrailingHTMLSpaces(p->value());
        // FIXME: serviceType calculation does not belong in this function.
        if (serviceType.isEmpty() && equalIgnoringCase(name, "type")) {
            serviceType = p->value();
            size_t pos = serviceType.find(";");
            if (pos != kNotFound)
                serviceType = serviceType.left(pos);
        }
    }

    // When OBJECT is used for an applet via Sun's Java plugin, the CODEBASE attribute in the tag
    // points to the Java plugin itself (an ActiveX component) while the actual applet CODEBASE is
    // in a PARAM tag. See <http://java.sun.com/products/plugin/1.2/docs/tags.html>. This means
    // we have to explicitly suppress the tag's CODEBASE attribute if there is none in a PARAM,
    // else our Java plugin will misinterpret it. [4004531]
    String codebase;
    if (MIMETypeRegistry::isJavaAppletMIMEType(serviceType)) {
        codebase = "codebase";
        uniqueParamNames.add(codebase.impl()); // pretend we found it in a PARAM already
    }

    // Turn the attributes of the <object> element into arrays, but don't override <param> values.
    AttributeCollection attributes = this->attributes();
    for (const Attribute& attribute : attributes) {
        const AtomicString& name = attribute.name().localName();
        if (!uniqueParamNames.contains(name.impl())) {
            paramNames.append(name.string());
            paramValues.append(attribute.value().string());
        }
    }

    mapDataParamToSrc(&paramNames, &paramValues);

    // HTML5 says that an object resource's URL is specified by the object's data
    // attribute, not by a param element. However, for compatibility, allow the
    // resource's URL to be given by a param named "src", "movie", "code" or "url"
    // if we know that resource points to a plugin.
    if (url.isEmpty() && !urlParameter.isEmpty()) {
        KURL completedURL = document().completeURL(urlParameter);
        bool useFallback;
        if (shouldUsePlugin(completedURL, serviceType, false, useFallback))
            url = urlParameter;
    }
}


bool HTMLObjectElement::hasFallbackContent() const
{
    for (Node* child = firstChild(); child; child = child->nextSibling()) {
        // Ignore whitespace-only text, and <param> tags, any other content is fallback content.
        if (child->isTextNode()) {
            if (!toText(child)->containsOnlyWhitespace())
                return true;
        } else if (!isHTMLParamElement(*child)) {
            return true;
        }
    }
    return false;
}

bool HTMLObjectElement::hasValidClassId()
{
    if (MIMETypeRegistry::isJavaAppletMIMEType(m_serviceType) && classId().startsWith("java:", TextCaseInsensitive))
        return true;

    // HTML5 says that fallback content should be rendered if a non-empty
    // classid is specified for which the UA can't find a suitable plugin.
    return classId().isEmpty();
}

void HTMLObjectElement::reloadPluginOnAttributeChange(const QualifiedName& name)
{
    // Following,
    //   http://www.whatwg.org/specs/web-apps/current-work/#the-object-element
    //   (Enumerated list below "Whenever one of the following conditions occur:")
    //
    // the updating of certain attributes should bring about "redetermination"
    // of what the element contains.
    bool needsInvalidation;
    if (name == typeAttr) {
        needsInvalidation = !fastHasAttribute(classidAttr) && !fastHasAttribute(dataAttr);
    } else if (name == dataAttr) {
        needsInvalidation = !fastHasAttribute(classidAttr);
    } else if (name == classidAttr) {
        needsInvalidation = true;
    } else {
        ASSERT_NOT_REACHED();
        needsInvalidation = false;
    }
    setNeedsWidgetUpdate(true);
    if (needsInvalidation)
        lazyReattachIfNeeded();
}

// FIXME: This should be unified with HTMLEmbedElement::updateWidget and
// moved down into HTMLPluginElement.cpp
void HTMLObjectElement::updateWidgetInternal()
{
    ASSERT(!layoutEmbeddedObject()->showsUnavailablePluginIndicator());
    ASSERT(needsWidgetUpdate());
    setNeedsWidgetUpdate(false);
    // FIXME: This should ASSERT isFinishedParsingChildren() instead.
    if (!isFinishedParsingChildren()) {
        dispatchErrorEvent();
        return;
    }

    // FIXME: I'm not sure it's ever possible to get into updateWidget during a
    // removal, but just in case we should avoid loading the frame to prevent
    // security bugs.
    if (!SubframeLoadingDisabler::canLoadFrame(*this)) {
        dispatchErrorEvent();
        return;
    }

    String url = this->url();
    String serviceType = m_serviceType;

    // FIXME: These should be joined into a PluginParameters class.
    Vector<String> paramNames;
    Vector<String> paramValues;
    parametersForPlugin(paramNames, paramValues, url, serviceType);

    // Note: url is modified above by parametersForPlugin.
    if (!allowedToLoadFrameURL(url)) {
        dispatchErrorEvent();
        return;
    }

    // FIXME: Is it possible to get here without a layoutObject now that we don't have beforeload events?
    if (!layoutObject())
        return;

    bool canRequest = true;
    if (classId().isEmpty() || !requestObject(url, classId(), paramNames, paramValues)) {
        canRequest = false;
    }

    if (!canRequest)
        canRequest = hasValidClassId() && requestObject(url, serviceType, paramNames, paramValues);

    if (!canRequest) {
        if (!url.isEmpty())
            dispatchErrorEvent();
        if (hasFallbackContent())
            renderFallbackContent();
    }
}

Node::InsertionNotificationRequest HTMLObjectElement::insertedInto(ContainerNode* insertionPoint)
{
    HTMLPlugInElement::insertedInto(insertionPoint);
    FormAssociatedElement::insertedInto(insertionPoint);
    return InsertionDone;
}

void HTMLObjectElement::removedFrom(ContainerNode* insertionPoint)
{
    HTMLPlugInElement::removedFrom(insertionPoint);
    FormAssociatedElement::removedFrom(insertionPoint);
}

void HTMLObjectElement::childrenChanged(const ChildrenChange& change)
{
    if (inDocument() && !useFallbackContent()) {
        setNeedsWidgetUpdate(true);
        lazyReattachIfNeeded();
    }
    HTMLPlugInElement::childrenChanged(change);
}

bool HTMLObjectElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == codebaseAttr || attribute.name() == dataAttr
        || (attribute.name() == usemapAttr && attribute.value()[0] != '#')
        || HTMLPlugInElement::isURLAttribute(attribute);
}

bool HTMLObjectElement::hasLegalLinkAttribute(const QualifiedName& name) const
{
    return name == classidAttr || name == dataAttr || name == codebaseAttr || HTMLPlugInElement::hasLegalLinkAttribute(name);
}

const QualifiedName& HTMLObjectElement::subResourceAttributeName() const
{
    return dataAttr;
}

const AtomicString HTMLObjectElement::imageSourceURL() const
{
    return getAttribute(dataAttr);
}

// FIXME: Remove this hack.
void HTMLObjectElement::reattachFallbackContent()
{
    // This can happen inside of attach() in the middle of a recalcStyle so we need to
    // reattach synchronously here.
    if (document().inStyleRecalc())
        reattach();
    else
        lazyReattachIfAttached();
}

void HTMLObjectElement::renderFallbackContent()
{
    if (useFallbackContent())
        return;

    if (!inDocument())
        return;

    // Before we give up and use fallback content, check to see if this is a MIME type issue.
    if (m_imageLoader && m_imageLoader->image() && m_imageLoader->image()->status() != Resource::LoadError) {
        m_serviceType = m_imageLoader->image()->response().mimeType();
        if (!isImageType()) {
            // If we don't think we have an image type anymore, then clear the image from the loader.
            m_imageLoader->setImage(0);
            reattachFallbackContent();
            return;
        }
    }

    m_useFallbackContent = true;

    // FIXME: Style gets recalculated which is suboptimal.
    reattachFallbackContent();
}

bool HTMLObjectElement::isExposed() const
{
    // http://www.whatwg.org/specs/web-apps/current-work/#exposed
    for (HTMLObjectElement* ancestor = Traversal<HTMLObjectElement>::firstAncestor(*this); ancestor; ancestor = Traversal<HTMLObjectElement>::firstAncestor(*ancestor)) {
        if (ancestor->isExposed())
            return false;
    }
    for (HTMLElement& element : Traversal<HTMLElement>::descendantsOf(*this)) {
        if (isHTMLObjectElement(element) || isHTMLEmbedElement(element))
            return false;
    }
    return true;
}

bool HTMLObjectElement::containsJavaApplet() const
{
    if (MIMETypeRegistry::isJavaAppletMIMEType(getAttribute(typeAttr)))
        return true;

    for (HTMLElement& child : Traversal<HTMLElement>::childrenOf(*this)) {
        if (isHTMLParamElement(child)
            && equalIgnoringCase(child.getNameAttribute(), "type")
            && MIMETypeRegistry::isJavaAppletMIMEType(child.getAttribute(valueAttr).string()))
            return true;
        if (isHTMLObjectElement(child) && toHTMLObjectElement(child).containsJavaApplet())
            return true;
        if (isHTMLAppletElement(child))
            return true;
    }

    return false;
}

void HTMLObjectElement::didMoveToNewDocument(Document& oldDocument)
{
    FormAssociatedElement::didMoveToNewDocument(oldDocument);
    HTMLPlugInElement::didMoveToNewDocument(oldDocument);
}

bool HTMLObjectElement::appendFormData(FormDataList& encoding, bool)
{
    if (name().isEmpty())
        return false;

    // Widget is needed immediately to satisfy cases like
    // LayoutTests/plugins/form-value.html.
    Widget* widget = pluginWidgetForJSBindings();
    if (!widget || !widget->isPluginView())
        return false;
    String value;
    if (!toPluginView(widget)->getFormValue(value))
        return false;
    encoding.appendData(name(), value);
    return true;
}

HTMLFormElement* HTMLObjectElement::formOwner() const
{
    return FormAssociatedElement::form();
}

bool HTMLObjectElement::isInteractiveContent() const
{
    return fastHasAttribute(usemapAttr);
}

bool HTMLObjectElement::useFallbackContent() const
{
    return HTMLPlugInElement::useFallbackContent() || m_useFallbackContent;
}

}
