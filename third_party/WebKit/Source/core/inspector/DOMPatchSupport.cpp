/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "core/inspector/DOMPatchSupport.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/Attribute.h"
#include "core/dom/ContextFeatures.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/Node.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/XMLDocument.h"
#include "core/html/HTMLBodyElement.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLHeadElement.h"
#include "core/html/parser/HTMLDocumentParser.h"
#include "core/inspector/DOMEditor.h"
#include "core/inspector/InspectorHistory.h"
#include "core/xml/parser/XMLDocumentParser.h"
#include "platform/Crypto.h"
#include "public/platform/Platform.h"
#include "wtf/Deque.h"
#include "wtf/HashTraits.h"
#include "wtf/RefPtr.h"
#include "wtf/text/Base64.h"
#include "wtf/text/CString.h"

namespace blink {

struct DOMPatchSupport::Digest {
    explicit Digest(Node* node) : m_node(node) { }

    String m_sha1;
    String m_attrsSHA1;
    Node* m_node;
    Vector<OwnPtr<Digest> > m_children;
};

void DOMPatchSupport::patchDocument(Document& document, const String& markup)
{
    InspectorHistory history;
    DOMEditor domEditor(&history);
    DOMPatchSupport patchSupport(&domEditor, document);
    patchSupport.patchDocument(markup);
}

DOMPatchSupport::DOMPatchSupport(DOMEditor* domEditor, Document& document)
    : m_domEditor(domEditor)
    , m_document(document)
{
}

void DOMPatchSupport::patchDocument(const String& markup)
{
    RefPtrWillBeRawPtr<Document> newDocument = nullptr;
    if (document().isHTMLDocument())
        newDocument = HTMLDocument::create();
    else if (document().isSVGDocument())
        newDocument = XMLDocument::createSVG();
    else if (document().isXHTMLDocument())
        newDocument = XMLDocument::createXHTML();
    else if (document().isXMLDocument())
        newDocument = XMLDocument::create();

    ASSERT(newDocument);
    newDocument->setContextFeatures(document().contextFeatures());
    if (!document().isHTMLDocument()) {
        RefPtrWillBeRawPtr<DocumentParser> parser = XMLDocumentParser::create(*newDocument, nullptr);
        parser->append(markup);
        parser->finish();
        parser->detach();

        // Avoid breakage on non-well-formed documents.
        if (!static_cast<XMLDocumentParser*>(parser.get())->wellFormed())
            return;
    }
    newDocument->setContent(markup);
    OwnPtr<Digest> oldInfo = createDigest(document().documentElement(), nullptr);
    OwnPtr<Digest> newInfo = createDigest(newDocument->documentElement(), &m_unusedNodesMap);

    if (!innerPatchNode(oldInfo.get(), newInfo.get(), IGNORE_EXCEPTION)) {
        // Fall back to rewrite.
        document().write(markup);
        document().close();
    }
}

Node* DOMPatchSupport::patchNode(Node* node, const String& markup, ExceptionState& exceptionState)
{
    // Don't parse <html> as a fragment.
    if (node->isDocumentNode() || (node->parentNode() && node->parentNode()->isDocumentNode())) {
        patchDocument(markup);
        return nullptr;
    }

    Node* previousSibling = node->previousSibling();
    RefPtrWillBeRawPtr<DocumentFragment> fragment = DocumentFragment::create(document());
    Node* targetNode = node->parentElementOrShadowRoot() ? node->parentElementOrShadowRoot() : document().documentElement();

    // Use the document BODY as the context element when editing immediate shadow root children,
    // as it provides an equivalent parsing context.
    if (targetNode->isShadowRoot())
        targetNode = document().body();
    Element* targetElement = toElement(targetNode);

    // FIXME: This code should use one of createFragment* in markup.h
    if (document().isHTMLDocument())
        fragment->parseHTML(markup, targetElement);
    else
        fragment->parseXML(markup, targetElement);

    // Compose the old list.
    ContainerNode* parentNode = node->parentNode();
    Vector<OwnPtr<Digest> > oldList;
    for (Node* child = parentNode->firstChild(); child; child = child->nextSibling())
        oldList.append(createDigest(child, 0));

    // Compose the new list.
    String markupCopy = markup.lower();
    Vector<OwnPtr<Digest> > newList;
    for (Node* child = parentNode->firstChild(); child != node; child = child->nextSibling())
        newList.append(createDigest(child, 0));
    for (Node* child = fragment->firstChild(); child; child = child->nextSibling()) {
        if (isHTMLHeadElement(*child) && !child->hasChildren() && markupCopy.find("</head>") == kNotFound)
            continue; // HTML5 parser inserts empty <head> tag whenever it parses <body>
        if (isHTMLBodyElement(*child) && !child->hasChildren() && markupCopy.find("</body>") == kNotFound)
            continue; // HTML5 parser inserts empty <body> tag whenever it parses </head>
        newList.append(createDigest(child, &m_unusedNodesMap));
    }
    for (Node* child = node->nextSibling(); child; child = child->nextSibling())
        newList.append(createDigest(child, 0));

    if (!innerPatchChildren(parentNode, oldList, newList, exceptionState)) {
        // Fall back to total replace.
        if (!m_domEditor->replaceChild(parentNode, fragment.release(), node, exceptionState))
            return nullptr;
    }
    return previousSibling ? previousSibling->nextSibling() : parentNode->firstChild();
}

bool DOMPatchSupport::innerPatchNode(Digest* oldDigest, Digest* newDigest, ExceptionState& exceptionState)
{
    if (oldDigest->m_sha1 == newDigest->m_sha1)
        return true;

    Node* oldNode = oldDigest->m_node;
    Node* newNode = newDigest->m_node;

    if (newNode->nodeType() != oldNode->nodeType() || newNode->nodeName() != oldNode->nodeName())
        return m_domEditor->replaceChild(oldNode->parentNode(), newNode, oldNode, exceptionState);

    if (oldNode->nodeValue() != newNode->nodeValue()) {
        if (!m_domEditor->setNodeValue(oldNode, newNode->nodeValue(), exceptionState))
            return false;
    }

    if (!oldNode->isElementNode())
        return true;

    // Patch attributes
    Element* oldElement = toElement(oldNode);
    Element* newElement = toElement(newNode);
    if (oldDigest->m_attrsSHA1 != newDigest->m_attrsSHA1) {
        // FIXME: Create a function in Element for removing all properties. Take in account whether did/willModifyAttribute are important.
        while (oldElement->attributesWithoutUpdate().size()) {
            const Attribute& attribute = oldElement->attributesWithoutUpdate().at(0);
            if (!m_domEditor->removeAttribute(oldElement, attribute.name().toString(), exceptionState))
                return false;
        }

        // FIXME: Create a function in Element for copying properties. cloneDataFromElement() is close but not enough for this case.
        for (auto& attribute : newElement->attributesWithoutUpdate()) {
            if (!m_domEditor->setAttribute(oldElement, attribute.name().toString(), attribute.value(), exceptionState))
                return false;
        }
    }

    bool result = innerPatchChildren(oldElement, oldDigest->m_children, newDigest->m_children, exceptionState);
    m_unusedNodesMap.remove(newDigest->m_sha1);
    return result;
}

pair<DOMPatchSupport::ResultMap, DOMPatchSupport::ResultMap>
DOMPatchSupport::diff(const Vector<OwnPtr<Digest> >& oldList, const Vector<OwnPtr<Digest> >& newList)
{
    ResultMap newMap(newList.size());
    ResultMap oldMap(oldList.size());

    for (size_t i = 0; i < oldMap.size(); ++i) {
        oldMap[i].first = 0;
        oldMap[i].second = 0;
    }

    for (size_t i = 0; i < newMap.size(); ++i) {
        newMap[i].first = 0;
        newMap[i].second = 0;
    }

    // Trim head and tail.
    for (size_t i = 0; i < oldList.size() && i < newList.size() && oldList[i]->m_sha1 == newList[i]->m_sha1; ++i) {
        oldMap[i].first = oldList[i].get();
        oldMap[i].second = i;
        newMap[i].first = newList[i].get();
        newMap[i].second = i;
    }
    for (size_t i = 0; i < oldList.size() && i < newList.size() && oldList[oldList.size() - i - 1]->m_sha1 == newList[newList.size() - i - 1]->m_sha1; ++i) {
        size_t oldIndex = oldList.size() - i - 1;
        size_t newIndex = newList.size() - i - 1;
        oldMap[oldIndex].first = oldList[oldIndex].get();
        oldMap[oldIndex].second = newIndex;
        newMap[newIndex].first = newList[newIndex].get();
        newMap[newIndex].second = oldIndex;
    }

    typedef HashMap<String, Vector<size_t> > DiffTable;
    DiffTable newTable;
    DiffTable oldTable;

    for (size_t i = 0; i < newList.size(); ++i) {
        newTable.add(newList[i]->m_sha1, Vector<size_t>()).storedValue->value.append(i);
    }

    for (size_t i = 0; i < oldList.size(); ++i) {
        oldTable.add(oldList[i]->m_sha1, Vector<size_t>()).storedValue->value.append(i);
    }

    for (auto& newIt : newTable) {
        if (newIt.value.size() != 1)
            continue;

        DiffTable::iterator oldIt = oldTable.find(newIt.key);
        if (oldIt == oldTable.end() || oldIt->value.size() != 1)
            continue;

        newMap[newIt.value[0]] = std::make_pair(newList[newIt.value[0]].get(), oldIt->value[0]);
        oldMap[oldIt->value[0]] = std::make_pair(oldList[oldIt->value[0]].get(), newIt.value[0]);
    }

    for (size_t i = 0; newList.size() > 0 && i < newList.size() - 1; ++i) {
        if (!newMap[i].first || newMap[i + 1].first)
            continue;

        size_t j = newMap[i].second + 1;
        if (j < oldMap.size() && !oldMap[j].first && newList[i + 1]->m_sha1 == oldList[j]->m_sha1) {
            newMap[i + 1] = std::make_pair(newList[i + 1].get(), j);
            oldMap[j] = std::make_pair(oldList[j].get(), i + 1);
        }
    }

    for (size_t i = newList.size() - 1; newList.size() > 0 && i > 0; --i) {
        if (!newMap[i].first || newMap[i - 1].first || newMap[i].second <= 0)
            continue;

        size_t j = newMap[i].second - 1;
        if (!oldMap[j].first && newList[i - 1]->m_sha1 == oldList[j]->m_sha1) {
            newMap[i - 1] = std::make_pair(newList[i - 1].get(), j);
            oldMap[j] = std::make_pair(oldList[j].get(), i - 1);
        }
    }

#ifdef DEBUG_DOM_PATCH_SUPPORT
    dumpMap(oldMap, "OLD");
    dumpMap(newMap, "NEW");
#endif

    return std::make_pair(oldMap, newMap);
}

bool DOMPatchSupport::innerPatchChildren(ContainerNode* parentNode, const Vector<OwnPtr<Digest> >& oldList, const Vector<OwnPtr<Digest> >& newList, ExceptionState& exceptionState)
{
    pair<ResultMap, ResultMap> resultMaps = diff(oldList, newList);
    ResultMap& oldMap = resultMaps.first;
    ResultMap& newMap = resultMaps.second;

    Digest* oldHead = nullptr;
    Digest* oldBody = nullptr;

    // 1. First strip everything except for the nodes that retain. Collect pending merges.
    HashMap<Digest*, Digest*> merges;
    HashSet<size_t, WTF::IntHash<size_t>, WTF::UnsignedWithZeroKeyHashTraits<size_t> > usedNewOrdinals;
    for (size_t i = 0; i < oldList.size(); ++i) {
        if (oldMap[i].first) {
            if (usedNewOrdinals.add(oldMap[i].second).isNewEntry)
                continue;
            oldMap[i].first = 0;
            oldMap[i].second = 0;
        }

        // Always match <head> and <body> tags with each other - we can't remove them from the DOM
        // upon patching.
        if (isHTMLHeadElement(*oldList[i]->m_node)) {
            oldHead = oldList[i].get();
            continue;
        }
        if (isHTMLBodyElement(*oldList[i]->m_node)) {
            oldBody = oldList[i].get();
            continue;
        }

        // Check if this change is between stable nodes. If it is, consider it as "modified".
        if (!m_unusedNodesMap.contains(oldList[i]->m_sha1) && (!i || oldMap[i - 1].first) && (i == oldMap.size() - 1 || oldMap[i + 1].first)) {
            size_t anchorCandidate = i ? oldMap[i - 1].second + 1 : 0;
            size_t anchorAfter = (i == oldMap.size() - 1) ? anchorCandidate + 1 : oldMap[i + 1].second;
            if (anchorAfter - anchorCandidate == 1 && anchorCandidate < newList.size())
                merges.set(newList[anchorCandidate].get(), oldList[i].get());
            else {
                if (!removeChildAndMoveToNew(oldList[i].get(), exceptionState))
                    return false;
            }
        } else {
            if (!removeChildAndMoveToNew(oldList[i].get(), exceptionState))
                return false;
        }
    }

    // Mark retained nodes as used, do not reuse node more than once.
    HashSet<size_t, WTF::IntHash<size_t>, WTF::UnsignedWithZeroKeyHashTraits<size_t> >  usedOldOrdinals;
    for (size_t i = 0; i < newList.size(); ++i) {
        if (!newMap[i].first)
            continue;
        size_t oldOrdinal = newMap[i].second;
        if (usedOldOrdinals.contains(oldOrdinal)) {
            // Do not map node more than once
            newMap[i].first = 0;
            newMap[i].second = 0;
            continue;
        }
        usedOldOrdinals.add(oldOrdinal);
        markNodeAsUsed(newMap[i].first);
    }

    // Mark <head> and <body> nodes for merge.
    if (oldHead || oldBody) {
        for (size_t i = 0; i < newList.size(); ++i) {
            if (oldHead && isHTMLHeadElement(*newList[i]->m_node))
                merges.set(newList[i].get(), oldHead);
            if (oldBody && isHTMLBodyElement(*newList[i]->m_node))
                merges.set(newList[i].get(), oldBody);
        }
    }

    // 2. Patch nodes marked for merge.
    for (auto& merge: merges) {
        if (!innerPatchNode(merge.value, merge.key, exceptionState))
            return false;
    }

    // 3. Insert missing nodes.
    for (size_t i = 0; i < newMap.size(); ++i) {
        if (newMap[i].first || merges.contains(newList[i].get()))
            continue;
        if (!insertBeforeAndMarkAsUsed(parentNode, newList[i].get(), NodeTraversal::childAt(*parentNode, i), exceptionState))
            return false;
    }

    // 4. Then put all nodes that retained into their slots (sort by new index).
    for (size_t i = 0; i < oldMap.size(); ++i) {
        if (!oldMap[i].first)
            continue;
        RefPtrWillBeRawPtr<Node> node = oldMap[i].first->m_node;
        Node* anchorNode = NodeTraversal::childAt(*parentNode, oldMap[i].second);
        if (node == anchorNode)
            continue;
        if (isHTMLBodyElement(*node) || isHTMLHeadElement(*node))
            continue; // Never move head or body, move the rest of the nodes around them.

        if (!m_domEditor->insertBefore(parentNode, node.release(), anchorNode, exceptionState))
            return false;
    }
    return true;
}

static void addStringToDigestor(WebCryptoDigestor* digestor, const String& string)
{
    digestor->consume(reinterpret_cast<const unsigned char*>(string.utf8().data()), string.length());
}

PassOwnPtr<DOMPatchSupport::Digest> DOMPatchSupport::createDigest(Node* node, UnusedNodesMap* unusedNodesMap)
{
    Digest* digest = new Digest(node);

    OwnPtr<WebCryptoDigestor> digestor = createDigestor(HashAlgorithmSha1);
    DigestValue digestResult;

    Node::NodeType nodeType = node->nodeType();
    digestor->consume(reinterpret_cast<const unsigned char*>(&nodeType), sizeof(nodeType));
    addStringToDigestor(digestor.get(), node->nodeName());
    addStringToDigestor(digestor.get(), node->nodeValue());

    if (node->isElementNode()) {
        Element& element = toElement(*node);
        Node* child = element.firstChild();
        while (child) {
            OwnPtr<Digest> childInfo = createDigest(child, unusedNodesMap);
            addStringToDigestor(digestor.get(), childInfo->m_sha1);
            child = child->nextSibling();
            digest->m_children.append(childInfo.release());
        }

        AttributeCollection attributes = element.attributesWithoutUpdate();
        if (!attributes.isEmpty()) {
            OwnPtr<WebCryptoDigestor> attrsDigestor = createDigestor(HashAlgorithmSha1);
            for (auto& attribute : attributes) {
                addStringToDigestor(attrsDigestor.get(), attribute.name().toString());
                addStringToDigestor(attrsDigestor.get(), attribute.value().string());
            }
            finishDigestor(attrsDigestor.get(), digestResult);
            digest->m_attrsSHA1 = base64Encode(reinterpret_cast<const char*>(digestResult.data()), 10);
            addStringToDigestor(digestor.get(), digest->m_attrsSHA1);
            digestResult.clear();
        }
    }
    finishDigestor(digestor.get(), digestResult);
    digest->m_sha1 = base64Encode(reinterpret_cast<const char*>(digestResult.data()), 10);

    if (unusedNodesMap)
        unusedNodesMap->add(digest->m_sha1, digest);
    return adoptPtr(digest);
}

bool DOMPatchSupport::insertBeforeAndMarkAsUsed(ContainerNode* parentNode, Digest* digest, Node* anchor, ExceptionState& exceptionState)
{
    bool result = m_domEditor->insertBefore(parentNode, digest->m_node, anchor, exceptionState);
    markNodeAsUsed(digest);
    return result;
}

bool DOMPatchSupport::removeChildAndMoveToNew(Digest* oldDigest, ExceptionState& exceptionState)
{
    RefPtrWillBeRawPtr<Node> oldNode = oldDigest->m_node;
    if (!m_domEditor->removeChild(oldNode->parentNode(), oldNode.get(), exceptionState))
        return false;

    // Diff works within levels. In order not to lose the node identity when user
    // prepends his HTML with "<div>" (i.e. all nodes are shifted to the next nested level),
    // prior to dropping the original node on the floor, check whether new DOM has a digest
    // with matching sha1. If it does, replace it with the original DOM chunk. Chances are
    // high that it will get merged back into the original DOM during the further patching.
    UnusedNodesMap::iterator it = m_unusedNodesMap.find(oldDigest->m_sha1);
    if (it != m_unusedNodesMap.end()) {
        Digest* newDigest = it->value;
        Node* newNode = newDigest->m_node;
        if (!m_domEditor->replaceChild(newNode->parentNode(), oldNode, newNode, exceptionState))
            return false;
        newDigest->m_node = oldNode.get();
        markNodeAsUsed(newDigest);
        return true;
    }

    for (size_t i = 0; i < oldDigest->m_children.size(); ++i) {
        if (!removeChildAndMoveToNew(oldDigest->m_children[i].get(), exceptionState))
            return false;
    }
    return true;
}

void DOMPatchSupport::markNodeAsUsed(Digest* digest)
{
    Deque<Digest*> queue;
    queue.append(digest);
    while (!queue.isEmpty()) {
        Digest* first = queue.takeFirst();
        m_unusedNodesMap.remove(first->m_sha1);
        for (size_t i = 0; i < first->m_children.size(); ++i)
            queue.append(first->m_children[i].get());
    }
}

#ifdef DEBUG_DOM_PATCH_SUPPORT
static String nodeName(Node* node)
{
    if (node->document().isXHTMLDocument())
         return node->nodeName();
    return node->nodeName().lower();
}

void DOMPatchSupport::dumpMap(const ResultMap& map, const String& name)
{
    fprintf(stderr, "\n\n");
    for (size_t i = 0; i < map.size(); ++i)
        fprintf(stderr, "%s[%lu]: %s (%p) - [%lu]\n", name.utf8().data(), i, map[i].first ? nodeName(map[i].first->m_node).utf8().data() : "", map[i].first, map[i].second);
}
#endif

} // namespace blink

