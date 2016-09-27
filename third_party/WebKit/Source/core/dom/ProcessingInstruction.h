/*
 * Copyright (C) 2000 Peter Kelly (pmk@post.com)
 * Copyright (C) 2006 Apple Inc. All rights reserved.
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

#ifndef ProcessingInstruction_h
#define ProcessingInstruction_h

#include "core/dom/CharacterData.h"
#include "core/fetch/ResourceOwner.h"
#include "core/fetch/StyleSheetResource.h"
#include "core/fetch/StyleSheetResourceClient.h"

namespace blink {

class StyleSheet;
class CSSStyleSheet;
class EventListener;

class ProcessingInstruction final : public CharacterData, private ResourceOwner<StyleSheetResource> {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<ProcessingInstruction> create(Document&, const String& target, const String& data);
    ~ProcessingInstruction() override;
    DECLARE_VIRTUAL_TRACE();

    const String& target() const { return m_target; }

    void setCreatedByParser(bool createdByParser) { m_createdByParser = createdByParser; }

    const String& localHref() const { return m_localHref; }
    StyleSheet* sheet() const { return m_sheet.get(); }

    bool isCSS() const { return m_isCSS; }
    bool isXSL() const { return m_isXSL; }

    void didAttributeChanged();
    bool isLoading() const;

    // For XSLT
    class DetachableEventListener {
    public:
        virtual ~DetachableEventListener() { }

        void ref() { refDetachableEventListener(); }
        void deref() { derefDetachableEventListener(); }

        virtual EventListener* toEventListener() = 0;

        // Detach event listener from its processing instruction.
        virtual void detach() = 0;

    private:
        virtual void refDetachableEventListener() = 0;
        virtual void derefDetachableEventListener() = 0;
    };

    void setEventListenerForXSLT(PassRefPtr<DetachableEventListener> listener) { m_listenerForXSLT = listener; }
    EventListener* eventListenerForXSLT();
    void clearEventListenerForXSLT();

private:
    ProcessingInstruction(Document&, const String& target, const String& data);

    String nodeName() const override;
    NodeType nodeType() const override;
    PassRefPtrWillBeRawPtr<Node> cloneNode(bool deep = true) override;

    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void removedFrom(ContainerNode*) override;

    bool checkStyleSheet(String& href, String& charset);
    void process(const String& href, const String& charset);

    void setCSSStyleSheet(const String& href, const KURL& baseURL, const String& charset, const CSSStyleSheetResource*) override;
    void setXSLStyleSheet(const String& href, const KURL& baseURL, const String& sheet) override;

    bool sheetLoaded() override;

    void parseStyleSheet(const String& sheet);
    void clearSheet();

    String m_target;
    String m_localHref;
    String m_title;
    String m_media;
    RefPtrWillBeMember<StyleSheet> m_sheet;
    bool m_loading;
    bool m_alternate;
    bool m_createdByParser;
    bool m_isCSS;
    bool m_isXSL;

    RefPtr<DetachableEventListener> m_listenerForXSLT;
};

DEFINE_NODE_TYPE_CASTS(ProcessingInstruction, nodeType() == Node::PROCESSING_INSTRUCTION_NODE);

inline bool isXSLStyleSheet(const Node& node)
{
    return node.nodeType() == Node::PROCESSING_INSTRUCTION_NODE && toProcessingInstruction(node).isXSL();
}

} // namespace blink

#endif // ProcessingInstruction_h
