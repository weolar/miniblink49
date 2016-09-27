// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
// FIXME(dominicc): Poor confused check-webkit-style demands Attribute.h here.
#include "core/dom/Attribute.h"

#include "core/HTMLNames.h"
#include "core/SVGNames.h"
#include "core/XLinkNames.h"
#include "core/clipboard/Pasteboard.h"
#include "core/dom/QualifiedName.h"
#include "core/editing/Editor.h"
#include "core/editing/SelectionType.h"
#include "core/editing/VisibleSelection.h"
#include "core/html/HTMLElement.h"
#include "core/svg/SVGAElement.h"
#include "core/svg/SVGAnimateElement.h"
#include "core/svg/SVGDiscardElement.h"
#include "core/svg/SVGSetElement.h"
#include "core/svg/animation/SVGSMILElement.h"
#include "core/svg/properties/SVGPropertyInfo.h"
#include "core/testing/DummyPageHolder.h"
#include "platform/geometry/IntSize.h"
#include "platform/weborigin/KURL.h"
#include "wtf/Vector.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/WTFString.h"
#include <gtest/gtest.h>

// Test that SVG content with JavaScript URLs is sanitized by removing
// the URLs. This sanitization happens when the content is pasted or
// drag-dropped into an editable element.
//
// There are two vectors for JavaScript URLs in SVG content:
//
// 1. Attributes, for example xlink:href in an <svg:a> element.
// 2. Animations which set those attributes, for example
//    <animate attributeName="xlink:href" values="javascript:...
//
// The following SVG elements, although related to animation, cannot
// set JavaScript URLs:
//
// - 'discard' can only remove elements, not set their attributes
// - 'animateMotion' does not use attribute name and produces floats
// - 'animateTransform' can only animate transform lists

namespace blink {

// Pastes htmlToPaste into the body of pageHolder's document, and
// returns the new content of the body.
String contentAfterPastingHTML(
    DummyPageHolder* pageHolder,
    const char* htmlToPaste)
{
    LocalFrame& frame = pageHolder->frame();
    HTMLElement* body = pageHolder->document().body();

    // Make the body editable, and put the caret in it.
    body->setAttribute(HTMLNames::contenteditableAttr, "true");
    frame.selection().setSelection(VisibleSelection::selectionFromContentsOfNode(body));
    EXPECT_EQ(CaretSelection, frame.selection().selectionType());
    EXPECT_TRUE(frame.selection().isContentEditable()) <<
        "We should be pasting into something editable.";

    Pasteboard* pasteboard = Pasteboard::generalPasteboard();
    pasteboard->writeHTML(htmlToPaste, blankURL(), "", false);
    EXPECT_TRUE(frame.editor().executeCommand("Paste"));

    return body->innerHTML();
}

// Integration tests.

TEST(
    UnsafeSVGAttributeSanitizationTest,
    pasteAnchor_javaScriptHrefIsStripped)
{
    OwnPtr<DummyPageHolder> pageHolder = DummyPageHolder::create(IntSize(1, 1));
    static const char unsafeContent[] =
        "<svg xmlns='http://www.w3.org/2000/svg' "
        "     xmlns:xlink='http://www.w3.org/1999/xlink'"
        "     width='1cm' height='1cm'>"
        "  <a xlink:href='javascript:alert()'></a>"
        "</svg>";
    String sanitizedContent =
        contentAfterPastingHTML(pageHolder.get(), unsafeContent);

    EXPECT_TRUE(sanitizedContent.contains("</a>")) <<
        "We should have pasted *something*; the document is: " <<
        sanitizedContent.utf8().data();
    EXPECT_FALSE(sanitizedContent.contains(":alert()")) <<
        "The JavaScript URL is unsafe and should have been stripped; "
        "instead: " <<
        sanitizedContent.utf8().data();
}

TEST(
    UnsafeSVGAttributeSanitizationTest,
    pasteAnchor_javaScriptHrefIsStripped_caseAndEntityInProtocol)
{
    OwnPtr<DummyPageHolder> pageHolder = DummyPageHolder::create(IntSize(1, 1));
    static const char unsafeContent[] =
        "<svg xmlns='http://www.w3.org/2000/svg' "
        "     xmlns:xlink='http://www.w3.org/1999/xlink'"
        "     width='1cm' height='1cm'>"
        "  <a xlink:href='j&#x41;vascriPT:alert()'></a>"
        "</svg>";
    String sanitizedContent =
        contentAfterPastingHTML(pageHolder.get(), unsafeContent);

    EXPECT_TRUE(sanitizedContent.contains("</a>")) <<
        "We should have pasted *something*; the document is: " <<
        sanitizedContent.utf8().data();
    EXPECT_FALSE(sanitizedContent.contains(":alert()")) <<
        "The JavaScript URL is unsafe and should have been stripped; "
        "instead: " <<
        sanitizedContent.utf8().data();
}

TEST(
    UnsafeSVGAttributeSanitizationTest,
    pasteAnchor_javaScriptHrefIsStripped_entityWithoutSemicolonInProtocol)
{
    OwnPtr<DummyPageHolder> pageHolder = DummyPageHolder::create(IntSize(1, 1));
    static const char unsafeContent[] =
        "<svg xmlns='http://www.w3.org/2000/svg' "
        "     xmlns:xlink='http://www.w3.org/1999/xlink'"
        "     width='1cm' height='1cm'>"
        "  <a xlink:href='jav&#x61script:alert()'></a>"
        "</svg>";
    String sanitizedContent =
        contentAfterPastingHTML(pageHolder.get(), unsafeContent);

    EXPECT_TRUE(sanitizedContent.contains("</a>")) <<
        "We should have pasted *something*; the document is: " <<
        sanitizedContent.utf8().data();
    EXPECT_FALSE(sanitizedContent.contains(":alert()")) <<
        "The JavaScript URL is unsafe and should have been stripped; "
        "instead: " <<
        sanitizedContent.utf8().data();
}

// Other sanitization integration tests are layout tests that use
// document.execCommand('Copy') to source content that they later
// paste. However SVG animation elements are not serialized when
// copying, which means we can't test sanitizing these attributes in
// layout tests: there is nowhere to source the unsafe content from.
TEST(
    UnsafeSVGAttributeSanitizationTest,
    pasteAnimatedAnchor_javaScriptHrefIsStripped_caseAndEntityInProtocol)
{
    OwnPtr<DummyPageHolder> pageHolder = DummyPageHolder::create(IntSize(1, 1));
    static const char unsafeContent[] =
        "<svg xmlns='http://www.w3.org/2000/svg' "
        "     xmlns:xlink='http://www.w3.org/1999/xlink'"
        "     width='1cm' height='1cm'>"
        "  <a xlink:href='https://www.google.com/'>"
        "    <animate xmlns:ng='http://www.w3.org/1999/xlink' "
        "             attributeName='ng:href' values='evil;J&#x61VaSCRIpT:alert()'>"
        "  </a>"
        "</svg>";
    String sanitizedContent =
        contentAfterPastingHTML(pageHolder.get(), unsafeContent);

    EXPECT_TRUE(sanitizedContent.contains("<a xlink:href=\"https://www.goo")) <<
        "We should have pasted *something*; the document is: " <<
        sanitizedContent.utf8().data();
    EXPECT_FALSE(sanitizedContent.contains(":alert()")) <<
        "The JavaScript URL is unsafe and should have been stripped; "
        "instead: " <<
        sanitizedContent.utf8().data();
}

// Unit tests

// stripScriptingAttributes inspects animation attributes for
// javascript: URLs. This check could be defeated if strings supported
// addition. If this test starts failing you must strengthen
// Element::stripScriptingAttributes, perhaps to strip all
// SVG animation attributes.
TEST(UnsafeSVGAttributeSanitizationTest, stringsShouldNotSupportAddition)
{
    RefPtrWillBeRawPtr<Document> document = Document::create();
    RefPtrWillBeRawPtr<SVGElement> target = SVGAElement::create(*document);
    RefPtrWillBeRawPtr<SVGAnimateElement> element = SVGAnimateElement::create(*document);
    element->setTargetElement(target.get());
    element->setAttributeName(XLinkNames::hrefAttr);

    // Sanity check that xlink:href was identified as a "string" attribute
    EXPECT_EQ(AnimatedString, element->animatedPropertyType());

    EXPECT_FALSE(element->animatedPropertyTypeSupportsAddition());
}

TEST(
    UnsafeSVGAttributeSanitizationTest,
    stripScriptingAttributes_animateElement)
{
    Vector<Attribute> attributes;
    attributes.append(Attribute(XLinkNames::hrefAttr, "javascript:alert()"));
    attributes.append(Attribute(SVGNames::fromAttr, "/home"));
    attributes.append(Attribute(SVGNames::toAttr, "javascript:own3d()"));

    RefPtrWillBeRawPtr<Document> document = Document::create();
    RefPtrWillBeRawPtr<Element> element = SVGAnimateElement::create(*document);
    element->stripScriptingAttributes(attributes);

    EXPECT_EQ(2ul, attributes.size()) <<
        "One of the attributes should have been stripped.";
    EXPECT_EQ(XLinkNames::hrefAttr, attributes[0].name()) <<
        "The 'xlink:href' attribute should not have been stripped from "
        "<animate> because it is not a URL attribute of <animate>.";
    EXPECT_EQ(SVGNames::fromAttr, attributes[1].name()) <<
        "The 'from' attribute should not have been strippef from <animate> "
        "because its value is innocuous.";
}

TEST(
    UnsafeSVGAttributeSanitizationTest,
    isJavaScriptURLAttribute_xlinkHrefContainingJavascriptURL)
{
    Attribute attribute(XLinkNames::hrefAttr, "javascript:alert()");
    RefPtrWillBeRawPtr<Document> document = Document::create();
    RefPtrWillBeRawPtr<Element> element = SVGAElement::create(*document);
    EXPECT_TRUE(
        element->isJavaScriptURLAttribute(attribute)) <<
        "The 'a' element should identify an 'xlink:href' attribute with a "
        "JavaScript URL value as a JavaScript URL attribute";
}

TEST(
    UnsafeSVGAttributeSanitizationTest,
    isJavaScriptURLAttribute_xlinkHrefContainingJavascriptURL_alternatePrefix)
{
    QualifiedName hrefAlternatePrefix(
        "foo", "href", XLinkNames::xlinkNamespaceURI);
    Attribute evilAttribute(hrefAlternatePrefix, "javascript:alert()");
    RefPtrWillBeRawPtr<Document> document = Document::create();
    RefPtrWillBeRawPtr<Element> element = SVGAElement::create(*document);
    EXPECT_TRUE(element->isJavaScriptURLAttribute(evilAttribute)) <<
        "The XLink 'href' attribute with a JavaScript URL value should be "
        "identified as a JavaScript URL attribute, even if the attribute "
        "doesn't use the typical 'xlink' prefix.";
}

TEST(
    UnsafeSVGAttributeSanitizationTest,
    isSVGAnimationAttributeSettingJavaScriptURL_fromContainingJavaScriptURL)
{
    Attribute evilAttribute(SVGNames::fromAttr, "javascript:alert()");
    RefPtrWillBeRawPtr<Document> document = Document::create();
    RefPtrWillBeRawPtr<Element> element = SVGAnimateElement::create(*document);
    EXPECT_TRUE(
        element->isSVGAnimationAttributeSettingJavaScriptURL(evilAttribute)) <<
        "The animate element should identify a 'from' attribute with a "
        "JavaScript URL value as setting a JavaScript URL.";
}

TEST(
    UnsafeSVGAttributeSanitizationTest,
    isSVGAnimationAttributeSettingJavaScriptURL_toContainingJavaScripURL)
{
    Attribute evilAttribute(SVGNames::toAttr, "javascript:window.close()");
    RefPtrWillBeRawPtr<Document> document = Document::create();
    RefPtrWillBeRawPtr<Element> element = SVGSetElement::create(*document);
    EXPECT_TRUE(
        element->isSVGAnimationAttributeSettingJavaScriptURL(evilAttribute)) <<
        "The set element should identify a 'to' attribute with a JavaScript "
        "URL value as setting a JavaScript URL.";
}

TEST(
    UnsafeSVGAttributeSanitizationTest,
    isSVGAnimationAttributeSettingJavaScriptURL_valuesContainingJavaScriptURL)
{
    Attribute evilAttribute(SVGNames::valuesAttr, "hi!; javascript:confirm()");
    RefPtrWillBeRawPtr<Document> document = Document::create();
    RefPtrWillBeRawPtr<Element> element = SVGAnimateElement::create(*document);
    element = SVGAnimateElement::create(*document);
    EXPECT_TRUE(
        element->isSVGAnimationAttributeSettingJavaScriptURL(evilAttribute)) <<
        "The animate element should identify a 'values' attribute with a "
        "JavaScript URL value as setting a JavaScript URL.";
}

TEST(
    UnsafeSVGAttributeSanitizationTest,
    isSVGAnimationAttributeSettingJavaScriptURL_innocuousAnimationAttribute)
{
    Attribute fineAttribute(SVGNames::fromAttr, "hello, world!");
    RefPtrWillBeRawPtr<Document> document = Document::create();
    RefPtrWillBeRawPtr<Element> element = SVGSetElement::create(*document);
    EXPECT_FALSE(
        element->isSVGAnimationAttributeSettingJavaScriptURL(fineAttribute)) <<
        "The animate element should not identify a 'from' attribute with an "
        "innocuous value as setting a JavaScript URL.";
}

} // namespace blink
