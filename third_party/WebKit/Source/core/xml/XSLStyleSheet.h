/*
 * This file is part of the XSL implementation.
 *
 * Copyright (C) 2004, 2006, 2008, 2012 Apple Inc. All rights reserved.
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
 *
 */

#ifndef XSLStyleSheet_h
#define XSLStyleSheet_h

#include "core/css/StyleSheet.h"
#include "core/dom/ProcessingInstruction.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/PassRefPtr.h"
#include <libxml/tree.h>
#include "third_party/libxslt/libxslt/transform.h"

namespace blink {

class XSLImportRule;

class XSLStyleSheet final : public StyleSheet {
public:
    static PassRefPtrWillBeRawPtr<XSLStyleSheet> create(XSLImportRule* parentImport, const String& originalURL, const KURL& finalURL)
    {
        ASSERT(RuntimeEnabledFeatures::xsltEnabled());
        return adoptRefWillBeNoop(new XSLStyleSheet(parentImport, originalURL, finalURL));
    }
    static PassRefPtrWillBeRawPtr<XSLStyleSheet> create(ProcessingInstruction* parentNode, const String& originalURL, const KURL& finalURL)
    {
        ASSERT(RuntimeEnabledFeatures::xsltEnabled());
        return adoptRefWillBeNoop(new XSLStyleSheet(parentNode, originalURL, finalURL, false));
    }
    static PassRefPtrWillBeRawPtr<XSLStyleSheet> createEmbedded(ProcessingInstruction* parentNode, const KURL& finalURL)
    {
        ASSERT(RuntimeEnabledFeatures::xsltEnabled());
        return adoptRefWillBeNoop(new XSLStyleSheet(parentNode, finalURL.string(), finalURL, true));
    }

    // Taking an arbitrary node is unsafe, because owner node pointer can become
    // stale. XSLTProcessor ensures that the stylesheet doesn't outlive its
    // parent, in part by not exposing it to JavaScript.
    static PassRefPtrWillBeRawPtr<XSLStyleSheet> createForXSLTProcessor(Document* document, Node* stylesheetRootNode, const String& originalURL, const KURL& finalURL)
    {
        ASSERT(RuntimeEnabledFeatures::xsltEnabled());
        return adoptRefWillBeNoop(new XSLStyleSheet(document, stylesheetRootNode, originalURL, finalURL, false));
    }

    ~XSLStyleSheet() override;

    bool parseString(const String&);

    void checkLoaded();

    const KURL& finalURL() const { return m_finalURL; }

    void loadChildSheets();
    void loadChildSheet(const String& href);

    Document* ownerDocument();
    XSLStyleSheet* parentStyleSheet() const override { return m_parentStyleSheet; }
    void setParentStyleSheet(XSLStyleSheet*);

    xmlDocPtr document();
    xsltStylesheetPtr compileStyleSheet();
    xmlDocPtr locateStylesheetSubResource(xmlDocPtr parentDoc, const xmlChar* uri);

    void clearDocuments();

    void markAsProcessed();
    bool processed() const { return m_processed; }

    String type() const override { return "text/xml"; }
    bool disabled() const override { return m_isDisabled; }
    void setDisabled(bool b) override { m_isDisabled = b; }
    Node* ownerNode() const override { return m_ownerNode; }
    String href() const override { return m_originalURL; }
    String title() const override { return emptyString(); }

    void clearOwnerNode() override { m_ownerNode = nullptr; }
    KURL baseURL() const override { return m_finalURL; }
    bool isLoading() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    XSLStyleSheet(Node* parentNode, const String& originalURL, const KURL& finalURL, bool embedded);
    XSLStyleSheet(Document* ownerDocument, Node* styleSheetRootNode, const String& originalURL, const KURL& finalURL, bool embedded);
    XSLStyleSheet(XSLImportRule* parentImport, const String& originalURL, const KURL& finalURL);

    RawPtrWillBeMember<Node> m_ownerNode;
    String m_originalURL;
    KURL m_finalURL;
    bool m_isDisabled;

    PersistentHeapVectorWillBeHeapVector<Member<XSLImportRule>> m_children;

    bool m_embedded;
    bool m_processed;

    xmlDocPtr m_stylesheetDoc;
    bool m_stylesheetDocTaken;
    bool m_compilationFailed;

    RawPtrWillBeMember<XSLStyleSheet> m_parentStyleSheet;
    RefPtrWillBeMember<Document> m_ownerDocument;
};

DEFINE_TYPE_CASTS(XSLStyleSheet, StyleSheet, sheet, !sheet->isCSSStyleSheet(), !sheet.isCSSStyleSheet());

} // namespace blink

#endif // XSLStyleSheet_h
