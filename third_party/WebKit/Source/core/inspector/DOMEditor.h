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

#ifndef DOMEditor_h
#define DOMEditor_h

#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ContainerNode;
class Element;
class ExceptionState;
class InspectorHistory;
class Node;
class Text;

typedef String ErrorString;

class DOMEditor final : public NoBaseWillBeGarbageCollected<DOMEditor> {
    WTF_MAKE_NONCOPYABLE(DOMEditor);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(DOMEditor);
public:
    explicit DOMEditor(InspectorHistory*);

    DECLARE_TRACE();

    bool insertBefore(ContainerNode* parentNode, PassRefPtrWillBeRawPtr<Node>, Node* anchorNode, ExceptionState&);
    bool removeChild(ContainerNode* parentNode, Node*, ExceptionState&);
    bool setAttribute(Element*, const String& name, const String& value, ExceptionState&);
    bool removeAttribute(Element*, const String& name, ExceptionState&);
    bool setOuterHTML(Node*, const String& html, Node** newNode, ExceptionState&);
    bool replaceWholeText(Text*, const String& text, ExceptionState&);
    bool replaceChild(ContainerNode* parentNode, PassRefPtrWillBeRawPtr<Node> newNode, Node* oldNode, ExceptionState&);
    bool setNodeValue(Node* parentNode, const String& value, ExceptionState&);

    bool insertBefore(ContainerNode* parentNode, PassRefPtrWillBeRawPtr<Node>, Node* anchorNode, ErrorString*);
    bool removeChild(ContainerNode* parentNode, Node*, ErrorString*);
    bool setAttribute(Element*, const String& name, const String& value, ErrorString*);
    bool removeAttribute(Element*, const String& name, ErrorString*);
    bool setOuterHTML(Node*, const String& html, Node** newNode, ErrorString*);
    bool replaceWholeText(Text*, const String& text, ErrorString*);

private:
    class DOMAction;
    class RemoveChildAction;
    class InsertBeforeAction;
    class RemoveAttributeAction;
    class SetAttributeAction;
    class SetOuterHTMLAction;
    class ReplaceWholeTextAction;
    class ReplaceChildNodeAction;
    class SetNodeValueAction;

    RawPtrWillBeMember<InspectorHistory> m_history;
};


} // namespace blink

#endif // !defined(DOMEditor_h)
