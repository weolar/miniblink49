/*
 * This file is part of the XSL implementation.
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple, Inc. All rights reserved.
 * Copyright (C) 2005, 2006 Alexey Proskuryakov <ap@webkit.org>
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
#include "core/xml/XSLTProcessor.h"

#include "core/dom/DOMImplementation.h"
#include "core/dom/DocumentEncodingData.h"
#include "core/dom/DocumentFragment.h"
#include "core/editing/markup.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/xml/DocumentXSLT.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/Assertions.h"

namespace blink {

static inline void transformTextStringToXHTMLDocumentString(String& text)
{
    // Modify the output so that it is a well-formed XHTML document with a <pre> tag enclosing the text.
    text.replaceWithLiteral('&', "&amp;");
    text.replaceWithLiteral('<', "&lt;");
    text = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
        "<head><title/></head>\n"
        "<body>\n"
        "<pre>" + text + "</pre>\n"
        "</body>\n"
        "</html>\n";
}

XSLTProcessor::~XSLTProcessor()
{
#if !ENABLE(OILPAN)
    // Stylesheet shouldn't outlive its root node.
    ASSERT(!m_stylesheetRootNode || !m_stylesheet || m_stylesheet->hasOneRef());
#endif
}

PassRefPtrWillBeRawPtr<Document> XSLTProcessor::createDocumentFromSource(const String& sourceString,
    const String& sourceEncoding, const String& sourceMIMEType, Node* sourceNode, LocalFrame* frame)
{
    RefPtrWillBeRawPtr<Document> ownerDocument(sourceNode->document());
    bool sourceIsDocument = (sourceNode == ownerDocument.get());
    String documentSource = sourceString;

    RefPtrWillBeRawPtr<Document> result = nullptr;
    DocumentInit init(sourceIsDocument ? ownerDocument->url() : KURL(), frame);

    bool forceXHTML = sourceMIMEType == "text/plain";
    if (forceXHTML)
        transformTextStringToXHTMLDocumentString(documentSource);

    if (frame) {
        RefPtrWillBeRawPtr<Document> oldDocument = frame->document();
        // Before parsing, we need to save & detach the old document and get the new document
        // in place. Document::detach() tears down the FrameView, so remember whether or not
        // there was one.
        bool hasView = frame->view();
        oldDocument->detach();
        // Re-create the FrameView if needed.
        if (hasView)
            frame->loader().client()->transitionToCommittedForNewPage();
        result = frame->localDOMWindow()->installNewDocument(sourceMIMEType, init, forceXHTML);

        if (oldDocument) {
            DocumentXSLT::from(*result).setTransformSourceDocument(oldDocument.get());
            result->updateSecurityOrigin(oldDocument->securityOrigin());
            result->setCookieURL(oldDocument->cookieURL());

            RefPtr<ContentSecurityPolicy> csp = ContentSecurityPolicy::create();
            csp->copyStateFrom(oldDocument->contentSecurityPolicy());
            result->initContentSecurityPolicy(csp);
        }
    } else {
        result = LocalDOMWindow::createDocument(sourceMIMEType, init, forceXHTML);
    }

    DocumentEncodingData data;
    data.setEncoding(sourceEncoding.isEmpty() ? UTF8Encoding() : WTF::TextEncoding(sourceEncoding));
    result->setEncodingData(data);
    result->setContent(documentSource);

    return result.release();
}

PassRefPtrWillBeRawPtr<Document> XSLTProcessor::transformToDocument(Node* sourceNode)
{
    String resultMIMEType;
    String resultString;
    String resultEncoding;
    if (!transformToString(sourceNode, resultMIMEType, resultString, resultEncoding))
        return nullptr;
    return createDocumentFromSource(resultString, resultEncoding, resultMIMEType, sourceNode, 0);
}

PassRefPtrWillBeRawPtr<DocumentFragment> XSLTProcessor::transformToFragment(Node* sourceNode, Document* outputDoc)
{
    String resultMIMEType;
    String resultString;
    String resultEncoding;

    // If the output document is HTML, default to HTML method.
    if (outputDoc->isHTMLDocument())
        resultMIMEType = "text/html";

    if (!transformToString(sourceNode, resultMIMEType, resultString, resultEncoding))
        return nullptr;
    return createFragmentForTransformToFragment(resultString, resultMIMEType, *outputDoc);
}

void XSLTProcessor::setParameter(const String& /*namespaceURI*/, const String& localName, const String& value)
{
    // FIXME: namespace support?
    // should make a QualifiedName here but we'd have to expose the impl
    m_parameters.set(localName, value);
}

String XSLTProcessor::getParameter(const String& /*namespaceURI*/, const String& localName) const
{
    // FIXME: namespace support?
    // should make a QualifiedName here but we'd have to expose the impl
    return m_parameters.get(localName);
}

void XSLTProcessor::removeParameter(const String& /*namespaceURI*/, const String& localName)
{
    // FIXME: namespace support?
    m_parameters.remove(localName);
}

void XSLTProcessor::reset()
{
    m_stylesheet.clear();
    m_stylesheetRootNode.clear();
    m_parameters.clear();
}

DEFINE_TRACE(XSLTProcessor)
{
    visitor->trace(m_stylesheet);
    visitor->trace(m_stylesheetRootNode);
    visitor->trace(m_document);
}

} // namespace blink
