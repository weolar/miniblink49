/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/page/PageSerializer.h"

#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/css/CSSFontFaceRule.h"
#include "core/css/CSSFontFaceSrcValue.h"
#include "core/css/CSSImageValue.h"
#include "core/css/CSSImportRule.h"
#include "core/css/CSSRuleList.h"
#include "core/css/CSSStyleDeclaration.h"
#include "core/css/CSSStyleRule.h"
#include "core/css/CSSValueList.h"
#include "core/css/StylePropertySet.h"
#include "core/css/StyleRule.h"
#include "core/css/StyleSheetContents.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/Text.h"
#include "core/editing/MarkupAccumulator.h"
#include "core/fetch/FontResource.h"
#include "core/fetch/ImageResource.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLLinkElement.h"
#include "core/html/HTMLMetaElement.h"
#include "core/html/HTMLStyleElement.h"
#include "core/html/ImageDocument.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/page/Page.h"
#include "core/style/StyleFetchedImage.h"
#include "core/style/StyleImage.h"
#include "platform/SerializedResource.h"
#include "platform/graphics/Image.h"
#include "wtf/OwnPtr.h"
#include "wtf/text/CString.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/TextEncoding.h"
#include "wtf/text/WTFString.h"

namespace blink {

static bool isCharsetSpecifyingNode(const Node& node)
{
    if (!isHTMLMetaElement(node))
        return false;

    const HTMLMetaElement& element = toHTMLMetaElement(node);
    HTMLAttributeList attributeList;
    AttributeCollection attributes = element.attributes();
    for (const Attribute& attr: attributes) {
        // FIXME: We should deal appropriately with the attribute if they have a namespace.
        attributeList.append(std::make_pair(attr.name().localName(), attr.value().string()));
    }
    WTF::TextEncoding textEncoding = encodingFromMetaAttributes(attributeList);
    return textEncoding.isValid();
}

static bool shouldIgnoreElement(const Element& element)
{
    return isHTMLScriptElement(element) || isHTMLNoScriptElement(element) || isCharsetSpecifyingNode(element);
}

static const QualifiedName& frameOwnerURLAttributeName(const HTMLFrameOwnerElement& frameOwner)
{
    // FIXME: We should support all frame owners including applets.
    return isHTMLObjectElement(frameOwner) ? HTMLNames::dataAttr : HTMLNames::srcAttr;
}

class SerializerMarkupAccumulator : public MarkupAccumulator {
    STACK_ALLOCATED();
public:
    SerializerMarkupAccumulator(PageSerializer*, const Document&, WillBeHeapVector<RawPtrWillBeMember<Node>>&);
    virtual ~SerializerMarkupAccumulator();

protected:
    virtual void appendText(StringBuilder& out, Text&) override;
    virtual bool shouldIgnoreAttribute(const Attribute&) override;
    virtual void appendElement(StringBuilder& out, Element&, Namespaces*) override;
    virtual void appendCustomAttributes(StringBuilder& out, const Element&, Namespaces*) override;
    virtual void appendStartTag(Node&, Namespaces* = nullptr) override;
    virtual void appendEndTag(const Element&) override;

    const Document& document();

private:
    PageSerializer* m_serializer;
    RawPtrWillBeMember<const Document> m_document;

    // FIXME: |PageSerializer| uses |m_nodes| for collecting nodes in document
    // included into serialized text then extracts image, object, etc. The size
    // of this vector isn't small for large document. It is better to use
    // callback like functionality.
    WillBeHeapVector<RawPtrWillBeMember<Node>>& m_nodes;
};

SerializerMarkupAccumulator::SerializerMarkupAccumulator(PageSerializer* serializer, const Document& document, WillBeHeapVector<RawPtrWillBeMember<Node>>& nodes)
    : MarkupAccumulator(ResolveAllURLs)
    , m_serializer(serializer)
    , m_document(&document)
    , m_nodes(nodes)
{
}

SerializerMarkupAccumulator::~SerializerMarkupAccumulator()
{
}

void SerializerMarkupAccumulator::appendText(StringBuilder& result, Text& text)
{
    Element* parent = text.parentElement();
    if (parent && !shouldIgnoreElement(*parent))
        MarkupAccumulator::appendText(result, text);
}

bool SerializerMarkupAccumulator::shouldIgnoreAttribute(const Attribute& attribute)
{
    PageSerializer::Delegate* delegate = m_serializer->delegate();
    if (delegate)
        return delegate->shouldIgnoreAttribute(attribute);

    return MarkupAccumulator::shouldIgnoreAttribute(attribute);
}

void SerializerMarkupAccumulator::appendElement(StringBuilder& result, Element& element, Namespaces* namespaces)
{
    if (!shouldIgnoreElement(element))
        MarkupAccumulator::appendElement(result, element, namespaces);

    // TODO(tiger): Refactor MarkupAccumulator so it is easier to append an element like this, without special cases for XHTML
    if (isHTMLHeadElement(element)) {
        result.appendLiteral("<meta http-equiv=\"Content-Type\" content=\"");
        MarkupFormatter::appendAttributeValue(result, m_document->suggestedMIMEType(), m_document->isHTMLDocument());
        result.appendLiteral("; charset=");
        MarkupFormatter::appendAttributeValue(result, m_document->charset(), m_document->isHTMLDocument());
        if (m_document->isXHTMLDocument())
            result.appendLiteral("\" />");
        else
            result.appendLiteral("\">");
    }

    // FIXME: For object (plugins) tags and video tag we could replace them by an image of their current contents.
}

void SerializerMarkupAccumulator::appendCustomAttributes(StringBuilder& result, const Element& element, Namespaces* namespaces)
{
    if (!element.isFrameOwnerElement())
        return;

    const HTMLFrameOwnerElement& frameOwner = toHTMLFrameOwnerElement(element);
    Frame* frame = frameOwner.contentFrame();
    // FIXME: RemoteFrames not currently supported here.
    if (!frame || !frame->isLocalFrame())
        return;

    KURL url = toLocalFrame(frame)->document()->url();
    if (url.isValid() && !url.protocolIsAbout())
        return;

    // We need to give a fake location to blank frames so they can be referenced by the serialized frame.
    url = m_serializer->urlForBlankFrame(toLocalFrame(frame));
    appendAttribute(result, element, Attribute(frameOwnerURLAttributeName(frameOwner), AtomicString(url.string())), namespaces);
}

void SerializerMarkupAccumulator::appendStartTag(Node& node, Namespaces* namespaces)
{
    MarkupAccumulator::appendStartTag(node, namespaces);
    m_nodes.append(&node);
}

void SerializerMarkupAccumulator::appendEndTag(const Element& element)
{
    if (!shouldIgnoreElement(element))
        MarkupAccumulator::appendEndTag(element);
}

const Document& SerializerMarkupAccumulator::document()
{
    return *m_document;
}

// TODO(tiger): Right now there is no support for rewriting URLs inside CSS
// documents which leads to bugs like <https://crbug.com/251898>. Not being
// able to rewrite URLs inside CSS documents means that resources imported from
// url(...) statements in CSS might not work when rewriting links for the
// "Webpage, Complete" method of saving a page. It will take some work but it
// needs to be done if we want to continue to support non-MHTML saved pages.
//
// Once that is fixed it would make sense to make link rewriting a bit more
// general. A new method, String& rewriteURL(String&) or similar, could be added
// to PageSerializer.Delegate that would allow clients to control this. Some of
// the change link logic could be moved back to WebPageSerializer.
//
// The remaining code in LinkChangeSerializerMarkupAccumulator could probably
// be merged back into SerializerMarkupAccumulator with additional methods in
// PageSerializer.Delegate to control MOTW and Base tag rewrite.
class LinkChangeSerializerMarkupAccumulator final : public SerializerMarkupAccumulator {
    STACK_ALLOCATED();
public:
    LinkChangeSerializerMarkupAccumulator(PageSerializer*, const Document&, WillBeHeapVector<RawPtrWillBeMember<Node>>&, HashMap<String, String>& rewriteURLs, const String& rewriteFolder);

private:
    void appendElement(StringBuilder&, Element&, Namespaces*) override;
    void appendAttribute(StringBuilder&, const Element&, const Attribute&, Namespaces*) override;

    // m_rewriteURLs include all pairs of local resource paths and corresponding original links.
    HashMap<String, String> m_rewriteURLs;
    String m_rewriteFolder;
};

LinkChangeSerializerMarkupAccumulator::LinkChangeSerializerMarkupAccumulator(PageSerializer* serializer, const Document& document, WillBeHeapVector<RawPtrWillBeMember<Node>>& nodes, HashMap<String, String>& rewriteURLs, const String& rewriteFolder)
    : SerializerMarkupAccumulator(serializer, document, nodes)
    , m_rewriteURLs(rewriteURLs)
    , m_rewriteFolder(rewriteFolder)
{
}

void LinkChangeSerializerMarkupAccumulator::appendElement(StringBuilder& result, Element& element, Namespaces* namespaces)
{
    if (element.hasTagName(HTMLNames::htmlTag)) {
        // Add MOTW (Mark of the Web) declaration before html tag.
        // See http://msdn2.microsoft.com/en-us/library/ms537628(VS.85).aspx.
        result.append('\n');
        MarkupFormatter::appendComment(result, String::format(" saved from url=(%04d)%s ",
            static_cast<int>(document().url().string().utf8().length()),
            document().url().string().utf8().data()));
        result.append('\n');
    }

    if (element.hasTagName(HTMLNames::baseTag)) {
        // TODO(tiger): Refactor MarkupAccumulator so it is easier to append an element like this, without special cases for XHTML
        // Append a new base tag declaration.
        result.appendLiteral("<base href=\".\"");
        if (!document().baseTarget().isEmpty()) {
            result.appendLiteral(" target=\"");
            MarkupFormatter::appendAttributeValue(result, document().baseTarget(), document().isHTMLDocument());
            result.append('"');
        }
        if (document().isXHTMLDocument())
            result.appendLiteral(" />");
        else
            result.appendLiteral(">");
    } else {
        SerializerMarkupAccumulator::appendElement(result, element, namespaces);
    }
}

void LinkChangeSerializerMarkupAccumulator::appendAttribute(StringBuilder& result, const Element& element, const Attribute& attribute, Namespaces* namespaces)
{
    if (!m_rewriteURLs.isEmpty() && element.isURLAttribute(attribute)) {

        String completeURL = document().completeURL(attribute.value());

        if (m_rewriteURLs.contains(completeURL)) {
            // TODO(tiger): Refactor MarkupAccumulator so it is easier to append an attribute like this.
            result.append(' ');
            result.append(attribute.name().toString());
            result.appendLiteral("=\"");
            if (!m_rewriteFolder.isEmpty())
                MarkupFormatter::appendAttributeValue(result, m_rewriteFolder + "/", document().isHTMLDocument());
            MarkupFormatter::appendAttributeValue(result, m_rewriteURLs.get(completeURL), document().isHTMLDocument());
            result.appendLiteral("\"");
            return;
        }
    }
    MarkupAccumulator::appendAttribute(result, element, attribute, namespaces);
}


PageSerializer::PageSerializer(Vector<SerializedResource>* resources, PassOwnPtr<Delegate> delegate)
    : m_resources(resources)
    , m_blankFrameCounter(0)
    , m_delegate(delegate)
{
}

void PageSerializer::serialize(Page* page)
{
    serializeFrame(page->deprecatedLocalMainFrame());
}

void PageSerializer::serializeFrame(LocalFrame* frame)
{
    ASSERT(frame->document());
    Document& document = *frame->document();
    KURL url = document.url();
    // FIXME: This probably wants isAboutBlankURL? to exclude other about: urls (like about:srcdoc)?
    if (!url.isValid() || url.protocolIsAbout()) {
        // For blank frames we generate a fake URL so they can be referenced by their containing frame.
        url = urlForBlankFrame(frame);
    }

    if (m_resourceURLs.contains(url)) {
        // FIXME: We could have 2 frame with the same URL but which were dynamically changed and have now
        // different content. So we should serialize both and somehow rename the frame src in the containing
        // frame. Arg!
        return;
    }

    // If frame is an image document, add the image and don't continue
    if (document.isImageDocument()) {
        ImageDocument& imageDocument = toImageDocument(document);
        addImageToResources(imageDocument.cachedImage(), imageDocument.imageElement()->layoutObject(), url);
        return;
    }

    WillBeHeapVector<RawPtrWillBeMember<Node>> serializedNodes;
    String text;
    if (!m_rewriteURLs.isEmpty()) {
        LinkChangeSerializerMarkupAccumulator accumulator(this, document, serializedNodes, m_rewriteURLs, m_rewriteFolder);
        text = serializeNodes<EditingStrategy>(accumulator, document, IncludeNode);
    } else {
        SerializerMarkupAccumulator accumulator(this, document, serializedNodes);
        text = serializeNodes<EditingStrategy>(accumulator, document, IncludeNode);
    }

    CString frameHTML = document.encoding().normalizeAndEncode(text, WTF::EntitiesForUnencodables);
    m_resources->append(SerializedResource(url, document.suggestedMIMEType(), SharedBuffer::create(frameHTML.data(), frameHTML.length())));
    m_resourceURLs.add(url);

    for (Node* node: serializedNodes) {
        ASSERT(node);
        if (!node->isElementNode())
            continue;

        Element& element = toElement(*node);
        // We have to process in-line style as it might contain some resources (typically background images).
        if (element.isStyledElement()) {
            retrieveResourcesForProperties(element.inlineStyle(), document);
            retrieveResourcesForProperties(element.presentationAttributeStyle(), document);
        }

        if (isHTMLImageElement(element)) {
            HTMLImageElement& imageElement = toHTMLImageElement(element);
            KURL url = document.completeURL(imageElement.getAttribute(HTMLNames::srcAttr));
            ImageResource* cachedImage = imageElement.cachedImage();
            addImageToResources(cachedImage, imageElement.layoutObject(), url);
        } else if (isHTMLInputElement(element)) {
            HTMLInputElement& inputElement = toHTMLInputElement(element);
            if (inputElement.type() == InputTypeNames::image && inputElement.imageLoader()) {
                KURL url = inputElement.src();
                ImageResource* cachedImage = inputElement.imageLoader()->image();
                addImageToResources(cachedImage, inputElement.layoutObject(), url);
            }
        } else if (isHTMLLinkElement(element)) {
            HTMLLinkElement& linkElement = toHTMLLinkElement(element);
            if (CSSStyleSheet* sheet = linkElement.sheet()) {
                KURL url = document.completeURL(linkElement.getAttribute(HTMLNames::hrefAttr));
                serializeCSSStyleSheet(*sheet, url);
                ASSERT(m_resourceURLs.contains(url));
            }
        } else if (isHTMLStyleElement(element)) {
            HTMLStyleElement& styleElement = toHTMLStyleElement(element);
            if (CSSStyleSheet* sheet = styleElement.sheet())
                serializeCSSStyleSheet(*sheet, KURL());
        }
    }

    for (Frame* childFrame = frame->tree().firstChild(); childFrame; childFrame = childFrame->tree().nextSibling()) {
        if (childFrame->isLocalFrame())
            serializeFrame(toLocalFrame(childFrame));
    }
}

void PageSerializer::serializeCSSStyleSheet(CSSStyleSheet& styleSheet, const KURL& url)
{
    StringBuilder cssText;
    for (unsigned i = 0; i < styleSheet.length(); ++i) {
        CSSRule* rule = styleSheet.item(i);
        String itemText = rule->cssText();
        if (!itemText.isEmpty()) {
            cssText.append(itemText);
            if (i < styleSheet.length() - 1)
                cssText.appendLiteral("\n\n");
        }

        // Some rules have resources associated with them that we need to retrieve.
        serializeCSSRule(rule);
    }

    if (url.isValid() && !m_resourceURLs.contains(url)) {
        // FIXME: We should check whether a charset has been specified and if none was found add one.
        WTF::TextEncoding textEncoding(styleSheet.contents()->charset());
        ASSERT(textEncoding.isValid());
        String textString = cssText.toString();
        CString text = textEncoding.normalizeAndEncode(textString, WTF::EntitiesForUnencodables);
        m_resources->append(SerializedResource(url, String("text/css"), SharedBuffer::create(text.data(), text.length())));
        m_resourceURLs.add(url);
    }
}

void PageSerializer::serializeCSSRule(CSSRule* rule)
{
    ASSERT(rule->parentStyleSheet()->ownerDocument());
    Document& document = *rule->parentStyleSheet()->ownerDocument();

    switch (rule->type()) {
    case CSSRule::STYLE_RULE:
        retrieveResourcesForProperties(&toCSSStyleRule(rule)->styleRule()->properties(), document);
        break;

    case CSSRule::IMPORT_RULE: {
        CSSImportRule* importRule = toCSSImportRule(rule);
        KURL sheetBaseURL = rule->parentStyleSheet()->baseURL();
        ASSERT(sheetBaseURL.isValid());
        KURL importURL = KURL(sheetBaseURL, importRule->href());
        if (m_resourceURLs.contains(importURL))
            break;
        if (importRule->styleSheet())
            serializeCSSStyleSheet(*importRule->styleSheet(), importURL);
        break;
    }

    // Rules inheriting CSSGroupingRule
    case CSSRule::MEDIA_RULE:
    case CSSRule::SUPPORTS_RULE: {
        CSSRuleList* ruleList = rule->cssRules();
        for (unsigned i = 0; i < ruleList->length(); ++i)
            serializeCSSRule(ruleList->item(i));
        break;
    }

    case CSSRule::FONT_FACE_RULE:
        retrieveResourcesForProperties(&toCSSFontFaceRule(rule)->styleRule()->properties(), document);
        break;

    // Rules in which no external resources can be referenced
    case CSSRule::CHARSET_RULE:
    case CSSRule::PAGE_RULE:
    case CSSRule::KEYFRAMES_RULE:
    case CSSRule::KEYFRAME_RULE:
    case CSSRule::VIEWPORT_RULE:
        break;

    default:
        ASSERT_NOT_REACHED();
    }
}

bool PageSerializer::shouldAddURL(const KURL& url)
{
    return url.isValid() && !m_resourceURLs.contains(url) && !url.protocolIsData();
}

void PageSerializer::addToResources(Resource* resource, PassRefPtr<SharedBuffer> data, const KURL& url)
{
    if (!data) {
        WTF_LOG_ERROR("No data for resource %s", url.string().utf8().data());
        return;
    }

    String mimeType = resource->response().mimeType();
    m_resources->append(SerializedResource(url, mimeType, data));
    m_resourceURLs.add(url);
}

void PageSerializer::addImageToResources(ImageResource* image, LayoutObject* imageLayoutObject, const KURL& url)
{
    if (!shouldAddURL(url))
        return;

    if (!image || !image->hasImage() || image->errorOccurred())
        return;

    RefPtr<SharedBuffer> data = imageLayoutObject ? image->imageForLayoutObject(imageLayoutObject)->data() : nullptr;
    if (!data)
        data = image->image()->data();

    addToResources(image, data, url);
}

void PageSerializer::addFontToResources(FontResource* font)
{
    if (!font || !shouldAddURL(font->url()) || !font->isLoaded() || !font->resourceBuffer())
        return;

    RefPtr<SharedBuffer> data(font->resourceBuffer());

    addToResources(font, data, font->url());
}

void PageSerializer::retrieveResourcesForProperties(const StylePropertySet* styleDeclaration, Document& document)
{
    if (!styleDeclaration)
        return;

    // The background-image and list-style-image (for ul or ol) are the CSS properties
    // that make use of images. We iterate to make sure we include any other
    // image properties there might be.
    unsigned propertyCount = styleDeclaration->propertyCount();
    for (unsigned i = 0; i < propertyCount; ++i) {
        RefPtrWillBeRawPtr<CSSValue> cssValue = styleDeclaration->propertyAt(i).value();
        retrieveResourcesForCSSValue(cssValue.get(), document);
    }
}

void PageSerializer::retrieveResourcesForCSSValue(CSSValue* cssValue, Document& document)
{
    if (cssValue->isImageValue()) {
        CSSImageValue* imageValue = toCSSImageValue(cssValue);
        StyleImage* styleImage = imageValue->cachedOrPendingImage();
        // Non cached-images are just place-holders and do not contain data.
        if (!styleImage || !styleImage->isImageResource())
            return;

        addImageToResources(styleImage->cachedImage(), nullptr, styleImage->cachedImage()->url());
    } else if (cssValue->isFontFaceSrcValue()) {
        CSSFontFaceSrcValue* fontFaceSrcValue = toCSSFontFaceSrcValue(cssValue);
        if (fontFaceSrcValue->isLocal()) {
            return;
        }

        addFontToResources(fontFaceSrcValue->fetch(&document));
    } else if (cssValue->isValueList()) {
        CSSValueList* cssValueList = toCSSValueList(cssValue);
        for (unsigned i = 0; i < cssValueList->length(); i++)
            retrieveResourcesForCSSValue(cssValueList->item(i), document);
    }
}

void PageSerializer::registerRewriteURL(const String& from, const String& to)
{
    m_rewriteURLs.set(from, to);
}

void PageSerializer::setRewriteURLFolder(const String& rewriteFolder)
{
    m_rewriteFolder = rewriteFolder;
}

KURL PageSerializer::urlForBlankFrame(LocalFrame* frame)
{
    BlankFrameURLMap::iterator iter = m_blankFrameURLs.find(frame);
    if (iter != m_blankFrameURLs.end())
        return iter->value;
    String url = "wyciwyg://frame/" + String::number(m_blankFrameCounter++);
    KURL fakeURL(ParsedURLString, url);
    m_blankFrameURLs.add(frame, fakeURL);

    return fakeURL;
}

PageSerializer::Delegate* PageSerializer::delegate()
{
    return m_delegate.get();
}

} // namespace blink
