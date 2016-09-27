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
#include "core/inspector/DOMEditor.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/DOMException.h"
#include "core/dom/Element.h"
#include "core/dom/Node.h"
#include "core/dom/Text.h"
#include "core/editing/markup.h"
#include "core/inspector/DOMPatchSupport.h"
#include "core/inspector/InspectorHistory.h"
#include "wtf/RefPtr.h"

namespace blink {

class DOMEditor::RemoveChildAction final : public InspectorHistory::Action {
    WTF_MAKE_NONCOPYABLE(RemoveChildAction);
public:
    RemoveChildAction(ContainerNode* parentNode, Node* node)
        : InspectorHistory::Action("RemoveChild")
        , m_parentNode(parentNode)
        , m_node(node)
    {
    }

    virtual bool perform(ExceptionState& exceptionState) override
    {
        m_anchorNode = m_node->nextSibling();
        return redo(exceptionState);
    }

    virtual bool undo(ExceptionState& exceptionState) override
    {
        m_parentNode->insertBefore(m_node.get(), m_anchorNode.get(), exceptionState);
        return !exceptionState.hadException();
    }

    virtual bool redo(ExceptionState& exceptionState) override
    {
        m_parentNode->removeChild(m_node.get(), exceptionState);
        return !exceptionState.hadException();
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_parentNode);
        visitor->trace(m_node);
        visitor->trace(m_anchorNode);
        InspectorHistory::Action::trace(visitor);
    }

private:
    RefPtrWillBeMember<ContainerNode> m_parentNode;
    RefPtrWillBeMember<Node> m_node;
    RefPtrWillBeMember<Node> m_anchorNode;
};

class DOMEditor::InsertBeforeAction final : public InspectorHistory::Action {
    WTF_MAKE_NONCOPYABLE(InsertBeforeAction);
public:
    InsertBeforeAction(ContainerNode* parentNode, PassRefPtrWillBeRawPtr<Node> node, Node* anchorNode)
        : InspectorHistory::Action("InsertBefore")
        , m_parentNode(parentNode)
        , m_node(node)
        , m_anchorNode(anchorNode)
    {
    }

    virtual bool perform(ExceptionState& exceptionState) override
    {
        if (m_node->parentNode()) {
            m_removeChildAction = adoptRefWillBeNoop(new RemoveChildAction(m_node->parentNode(), m_node.get()));
            if (!m_removeChildAction->perform(exceptionState))
                return false;
        }
        m_parentNode->insertBefore(m_node.get(), m_anchorNode.get(), exceptionState);
        return !exceptionState.hadException();
    }

    virtual bool undo(ExceptionState& exceptionState) override
    {
        m_parentNode->removeChild(m_node.get(), exceptionState);
        if (exceptionState.hadException())
            return false;
        if (m_removeChildAction)
            return m_removeChildAction->undo(exceptionState);
        return true;
    }

    virtual bool redo(ExceptionState& exceptionState) override
    {
        if (m_removeChildAction && !m_removeChildAction->redo(exceptionState))
            return false;
        m_parentNode->insertBefore(m_node.get(), m_anchorNode.get(), exceptionState);
        return !exceptionState.hadException();
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_parentNode);
        visitor->trace(m_node);
        visitor->trace(m_anchorNode);
        visitor->trace(m_removeChildAction);
        InspectorHistory::Action::trace(visitor);
    }

private:
    RefPtrWillBeMember<ContainerNode> m_parentNode;
    RefPtrWillBeMember<Node> m_node;
    RefPtrWillBeMember<Node> m_anchorNode;
    RefPtrWillBeMember<RemoveChildAction> m_removeChildAction;
};

class DOMEditor::RemoveAttributeAction final : public InspectorHistory::Action {
    WTF_MAKE_NONCOPYABLE(RemoveAttributeAction);
public:
    RemoveAttributeAction(Element* element, const AtomicString& name)
        : InspectorHistory::Action("RemoveAttribute")
        , m_element(element)
        , m_name(name)
    {
    }

    virtual bool perform(ExceptionState& exceptionState) override
    {
        m_value = m_element->getAttribute(m_name);
        return redo(exceptionState);
    }

    virtual bool undo(ExceptionState& exceptionState) override
    {
        m_element->setAttribute(m_name, m_value, exceptionState);
        return true;
    }

    virtual bool redo(ExceptionState&) override
    {
        m_element->removeAttribute(m_name);
        return true;
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_element);
        InspectorHistory::Action::trace(visitor);
    }

private:
    RefPtrWillBeMember<Element> m_element;
    AtomicString m_name;
    AtomicString m_value;
};

class DOMEditor::SetAttributeAction final : public InspectorHistory::Action {
    WTF_MAKE_NONCOPYABLE(SetAttributeAction);
public:
    SetAttributeAction(Element* element, const AtomicString& name, const AtomicString& value)
        : InspectorHistory::Action("SetAttribute")
        , m_element(element)
        , m_name(name)
        , m_value(value)
        , m_hadAttribute(false)
    {
    }

    virtual bool perform(ExceptionState& exceptionState) override
    {
        const AtomicString& value = m_element->getAttribute(m_name);
        m_hadAttribute = !value.isNull();
        if (m_hadAttribute)
            m_oldValue = value;
        return redo(exceptionState);
    }

    virtual bool undo(ExceptionState& exceptionState) override
    {
        if (m_hadAttribute)
            m_element->setAttribute(m_name, m_oldValue, exceptionState);
        else
            m_element->removeAttribute(m_name);
        return true;
    }

    virtual bool redo(ExceptionState& exceptionState) override
    {
        m_element->setAttribute(m_name, m_value, exceptionState);
        return true;
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_element);
        InspectorHistory::Action::trace(visitor);
    }

private:
    RefPtrWillBeMember<Element> m_element;
    AtomicString m_name;
    AtomicString m_value;
    bool m_hadAttribute;
    AtomicString m_oldValue;
};

class DOMEditor::SetOuterHTMLAction final : public InspectorHistory::Action {
    WTF_MAKE_NONCOPYABLE(SetOuterHTMLAction);
public:
    SetOuterHTMLAction(Node* node, const String& html)
        : InspectorHistory::Action("SetOuterHTML")
        , m_node(node)
        , m_nextSibling(node->nextSibling())
        , m_html(html)
        , m_newNode(nullptr)
        , m_history(adoptPtrWillBeNoop(new InspectorHistory()))
        , m_domEditor(adoptPtrWillBeNoop(new DOMEditor(m_history.get())))
    {
    }

    virtual bool perform(ExceptionState& exceptionState) override
    {
        m_oldHTML = createMarkup(m_node.get());
        ASSERT(m_node->ownerDocument());
        DOMPatchSupport domPatchSupport(m_domEditor.get(), *m_node->ownerDocument());
        m_newNode = domPatchSupport.patchNode(m_node.get(), m_html, exceptionState);
        return !exceptionState.hadException();
    }

    virtual bool undo(ExceptionState& exceptionState) override
    {
        return m_history->undo(exceptionState);
    }

    virtual bool redo(ExceptionState& exceptionState) override
    {
        return m_history->redo(exceptionState);
    }

    Node* newNode()
    {
        return m_newNode;
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_node);
        visitor->trace(m_nextSibling);
        visitor->trace(m_newNode);
        visitor->trace(m_history);
        visitor->trace(m_domEditor);
        InspectorHistory::Action::trace(visitor);
    }

private:
    RefPtrWillBeMember<Node> m_node;
    RefPtrWillBeMember<Node> m_nextSibling;
    String m_html;
    String m_oldHTML;
    RawPtrWillBeMember<Node> m_newNode;
    OwnPtrWillBeMember<InspectorHistory> m_history;
    OwnPtrWillBeMember<DOMEditor> m_domEditor;
};

class DOMEditor::ReplaceWholeTextAction final : public InspectorHistory::Action {
    WTF_MAKE_NONCOPYABLE(ReplaceWholeTextAction);
public:
    ReplaceWholeTextAction(Text* textNode, const String& text)
        : InspectorHistory::Action("ReplaceWholeText")
        , m_textNode(textNode)
        , m_text(text)
    {
    }

    virtual bool perform(ExceptionState& exceptionState) override
    {
        m_oldText = m_textNode->wholeText();
        return redo(exceptionState);
    }

    virtual bool undo(ExceptionState&) override
    {
        m_textNode->replaceWholeText(m_oldText);
        return true;
    }

    virtual bool redo(ExceptionState&) override
    {
        m_textNode->replaceWholeText(m_text);
        return true;
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_textNode);
        InspectorHistory::Action::trace(visitor);
    }

private:
    RefPtrWillBeMember<Text> m_textNode;
    String m_text;
    String m_oldText;
};

class DOMEditor::ReplaceChildNodeAction final : public InspectorHistory::Action {
    WTF_MAKE_NONCOPYABLE(ReplaceChildNodeAction);
public:
    ReplaceChildNodeAction(ContainerNode* parentNode, PassRefPtrWillBeRawPtr<Node> newNode, Node* oldNode)
        : InspectorHistory::Action("ReplaceChildNode")
        , m_parentNode(parentNode)
        , m_newNode(newNode)
        , m_oldNode(oldNode)
    {
    }

    virtual bool perform(ExceptionState& exceptionState) override
    {
        return redo(exceptionState);
    }

    virtual bool undo(ExceptionState& exceptionState) override
    {
        m_parentNode->replaceChild(m_oldNode, m_newNode.get(), exceptionState);
        return !exceptionState.hadException();
    }

    virtual bool redo(ExceptionState& exceptionState) override
    {
        m_parentNode->replaceChild(m_newNode, m_oldNode.get(), exceptionState);
        return !exceptionState.hadException();
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_parentNode);
        visitor->trace(m_newNode);
        visitor->trace(m_oldNode);
        InspectorHistory::Action::trace(visitor);
    }

private:
    RefPtrWillBeMember<ContainerNode> m_parentNode;
    RefPtrWillBeMember<Node> m_newNode;
    RefPtrWillBeMember<Node> m_oldNode;
};

class DOMEditor::SetNodeValueAction final : public InspectorHistory::Action {
    WTF_MAKE_NONCOPYABLE(SetNodeValueAction);
public:
    SetNodeValueAction(Node* node, const String& value)
        : InspectorHistory::Action("SetNodeValue")
        , m_node(node)
        , m_value(value)
    {
    }

    virtual bool perform(ExceptionState&) override
    {
        m_oldValue = m_node->nodeValue();
        return redo(IGNORE_EXCEPTION);
    }

    virtual bool undo(ExceptionState&) override
    {
        m_node->setNodeValue(m_oldValue);
        return true;
    }

    virtual bool redo(ExceptionState&) override
    {
        m_node->setNodeValue(m_value);
        return true;
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_node);
        InspectorHistory::Action::trace(visitor);
    }

private:
    RefPtrWillBeMember<Node> m_node;
    String m_value;
    String m_oldValue;
};

DOMEditor::DOMEditor(InspectorHistory* history) : m_history(history) { }

bool DOMEditor::insertBefore(ContainerNode* parentNode, PassRefPtrWillBeRawPtr<Node> node, Node* anchorNode, ExceptionState& exceptionState)
{
    return m_history->perform(adoptRefWillBeNoop(new InsertBeforeAction(parentNode, node, anchorNode)), exceptionState);
}

bool DOMEditor::removeChild(ContainerNode* parentNode, Node* node, ExceptionState& exceptionState)
{
    return m_history->perform(adoptRefWillBeNoop(new RemoveChildAction(parentNode, node)), exceptionState);
}

bool DOMEditor::setAttribute(Element* element, const String& name, const String& value, ExceptionState& exceptionState)
{
    return m_history->perform(adoptRefWillBeNoop(new SetAttributeAction(element, AtomicString(name), AtomicString(value))), exceptionState);
}

bool DOMEditor::removeAttribute(Element* element, const String& name, ExceptionState& exceptionState)
{
    return m_history->perform(adoptRefWillBeNoop(new RemoveAttributeAction(element, AtomicString(name))), exceptionState);
}

bool DOMEditor::setOuterHTML(Node* node, const String& html, Node** newNode, ExceptionState& exceptionState)
{
    RefPtrWillBeRawPtr<SetOuterHTMLAction> action = adoptRefWillBeNoop(new SetOuterHTMLAction(node, html));
    bool result = m_history->perform(action, exceptionState);
    if (result)
        *newNode = action->newNode();
    return result;
}

bool DOMEditor::replaceWholeText(Text* textNode, const String& text, ExceptionState& exceptionState)
{
    return m_history->perform(adoptRefWillBeNoop(new ReplaceWholeTextAction(textNode, text)), exceptionState);
}

bool DOMEditor::replaceChild(ContainerNode* parentNode, PassRefPtrWillBeRawPtr<Node> newNode, Node* oldNode, ExceptionState& exceptionState)
{
    return m_history->perform(adoptRefWillBeNoop(new ReplaceChildNodeAction(parentNode, newNode, oldNode)), exceptionState);
}

bool DOMEditor::setNodeValue(Node* node, const String& value, ExceptionState& exceptionState)
{
    return m_history->perform(adoptRefWillBeNoop(new SetNodeValueAction(node, value)), exceptionState);
}

static void populateErrorString(ExceptionState& exceptionState, ErrorString* errorString)
{
    if (exceptionState.hadException())
        *errorString = DOMException::getErrorName(exceptionState.code());
}

bool DOMEditor::insertBefore(ContainerNode* parentNode, PassRefPtrWillBeRawPtr<Node> node, Node* anchorNode, ErrorString* errorString)
{
    TrackExceptionState exceptionState;
    bool result = insertBefore(parentNode, node, anchorNode, exceptionState);
    populateErrorString(exceptionState, errorString);
    return result;
}

bool DOMEditor::removeChild(ContainerNode* parentNode, Node* node, ErrorString* errorString)
{
    TrackExceptionState exceptionState;
    bool result = removeChild(parentNode, node, exceptionState);
    populateErrorString(exceptionState, errorString);
    return result;
}

bool DOMEditor::setAttribute(Element* element, const String& name, const String& value, ErrorString* errorString)
{
    TrackExceptionState exceptionState;
    bool result = setAttribute(element, name, value, exceptionState);
    populateErrorString(exceptionState, errorString);
    return result;
}

bool DOMEditor::removeAttribute(Element* element, const String& name, ErrorString* errorString)
{
    TrackExceptionState exceptionState;
    bool result = removeAttribute(element, name, exceptionState);
    populateErrorString(exceptionState, errorString);
    return result;
}

bool DOMEditor::setOuterHTML(Node* node, const String& html, Node** newNode, ErrorString* errorString)
{
    TrackExceptionState exceptionState;
    bool result = setOuterHTML(node, html, newNode, exceptionState);
    populateErrorString(exceptionState, errorString);
    return result;
}

bool DOMEditor::replaceWholeText(Text* textNode, const String& text, ErrorString* errorString)
{
    TrackExceptionState exceptionState;
    bool result = replaceWholeText(textNode, text, exceptionState);
    populateErrorString(exceptionState, errorString);
    return result;
}

DEFINE_TRACE(DOMEditor)
{
    visitor->trace(m_history);
}

} // namespace blink

