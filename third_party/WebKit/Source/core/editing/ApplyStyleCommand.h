/*
 * Copyright (C) 2005, 2006, 2008, 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ApplyStyleCommand_h
#define ApplyStyleCommand_h

#include "core/editing/CompositeEditCommand.h"
#include "core/editing/WritingDirection.h"
#include "core/html/HTMLElement.h"

namespace blink {

class EditingStyle;
class HTMLSpanElement;
class StyleChange;

enum ShouldIncludeTypingStyle {
    IncludeTypingStyle,
    IgnoreTypingStyle
};

class ApplyStyleCommand final : public CompositeEditCommand {
public:
    enum EPropertyLevel { PropertyDefault, ForceBlockProperties };
    enum InlineStyleRemovalMode { RemoveIfNeeded, RemoveAlways, RemoveNone };
    enum EAddStyledElement { AddStyledElement, DoNotAddStyledElement };
    typedef bool (*IsInlineElementToRemoveFunction)(const Element*);

    static PassRefPtrWillBeRawPtr<ApplyStyleCommand> create(Document& document, const EditingStyle* style, EditAction action = EditActionChangeAttributes, EPropertyLevel level = PropertyDefault)
    {
        return adoptRefWillBeNoop(new ApplyStyleCommand(document, style, action, level));
    }
    static PassRefPtrWillBeRawPtr<ApplyStyleCommand> create(Document& document, const EditingStyle* style, const Position& start, const Position& end, EditAction action = EditActionChangeAttributes, EPropertyLevel level = PropertyDefault)
    {
        return adoptRefWillBeNoop(new ApplyStyleCommand(document, style, start, end, action, level));
    }
    static PassRefPtrWillBeRawPtr<ApplyStyleCommand> create(PassRefPtrWillBeRawPtr<Element> element, bool removeOnly = false, EditAction action = EditActionChangeAttributes)
    {
        return adoptRefWillBeNoop(new ApplyStyleCommand(element, removeOnly, action));
    }
    static PassRefPtrWillBeRawPtr<ApplyStyleCommand> create(Document& document, const EditingStyle* style, IsInlineElementToRemoveFunction isInlineElementToRemoveFunction, EditAction action = EditActionChangeAttributes)
    {
        return adoptRefWillBeNoop(new ApplyStyleCommand(document, style, isInlineElementToRemoveFunction, action));
    }

    DECLARE_VIRTUAL_TRACE();

private:
    ApplyStyleCommand(Document&, const EditingStyle*, EditAction, EPropertyLevel);
    ApplyStyleCommand(Document&, const EditingStyle*, const Position& start, const Position& end, EditAction, EPropertyLevel);
    ApplyStyleCommand(PassRefPtrWillBeRawPtr<Element>, bool removeOnly, EditAction);
    ApplyStyleCommand(Document&, const EditingStyle*, bool (*isInlineElementToRemove)(const Element*), EditAction);

    void doApply() override;
    EditAction editingAction() const override;

    // style-removal helpers
    bool isStyledInlineElementToRemove(Element*) const;
    bool shouldApplyInlineStyleToRun(EditingStyle*, Node* runStart, Node* pastEndNode);
    void removeConflictingInlineStyleFromRun(EditingStyle*, RefPtrWillBeMember<Node>& runStart, RefPtrWillBeMember<Node>& runEnd, PassRefPtrWillBeRawPtr<Node> pastEndNode);
    bool removeInlineStyleFromElement(EditingStyle*, PassRefPtrWillBeRawPtr<HTMLElement>, InlineStyleRemovalMode = RemoveIfNeeded, EditingStyle* extractedStyle = nullptr);
    inline bool shouldRemoveInlineStyleFromElement(EditingStyle* style, HTMLElement* element) {return removeInlineStyleFromElement(style, element, RemoveNone);}
    void replaceWithSpanOrRemoveIfWithoutAttributes(HTMLElement*);
    bool removeImplicitlyStyledElement(EditingStyle*, HTMLElement*, InlineStyleRemovalMode, EditingStyle* extractedStyle);
    bool removeCSSStyle(EditingStyle*, HTMLElement*, InlineStyleRemovalMode = RemoveIfNeeded, EditingStyle* extractedStyle = nullptr);
    HTMLElement* highestAncestorWithConflictingInlineStyle(EditingStyle*, Node*);
    void applyInlineStyleToPushDown(Node*, EditingStyle*);
    void pushDownInlineStyleAroundNode(EditingStyle*, Node*);
    void removeInlineStyle(EditingStyle* , const Position& start, const Position& end);
    bool elementFullySelected(HTMLElement&, const Position& start, const Position& end) const;

    // style-application helpers
    void applyBlockStyle(EditingStyle*);
    void applyRelativeFontStyleChange(EditingStyle*);
    void applyInlineStyle(EditingStyle*);
    void fixRangeAndApplyInlineStyle(EditingStyle*, const Position& start, const Position& end);
    void applyInlineStyleToNodeRange(EditingStyle*, PassRefPtrWillBeRawPtr<Node> startNode, PassRefPtrWillBeRawPtr<Node> pastEndNode);
    void addBlockStyle(const StyleChange&, HTMLElement*);
    void addInlineStyleIfNeeded(EditingStyle*, PassRefPtrWillBeRawPtr<Node> start, PassRefPtrWillBeRawPtr<Node> end, EAddStyledElement = AddStyledElement);
    Position positionToComputeInlineStyleChange(PassRefPtrWillBeRawPtr<Node>, RefPtrWillBeMember<HTMLSpanElement>& dummyElement);
    void applyInlineStyleChange(PassRefPtrWillBeRawPtr<Node> startNode, PassRefPtrWillBeRawPtr<Node> endNode, StyleChange&, EAddStyledElement);
    void splitTextAtStart(const Position& start, const Position& end);
    void splitTextAtEnd(const Position& start, const Position& end);
    void splitTextElementAtStart(const Position& start, const Position& end);
    void splitTextElementAtEnd(const Position& start, const Position& end);
    bool shouldSplitTextElement(Element*, EditingStyle*);
    bool isValidCaretPositionInTextNode(const Position& position);
    bool mergeStartWithPreviousIfIdentical(const Position& start, const Position& end);
    bool mergeEndWithNextIfIdentical(const Position& start, const Position& end);
    void cleanupUnstyledAppleStyleSpans(ContainerNode* dummySpanAncestor);

    void surroundNodeRangeWithElement(PassRefPtrWillBeRawPtr<Node> start, PassRefPtrWillBeRawPtr<Node> end, PassRefPtrWillBeRawPtr<Element>);
    float computedFontSize(Node*);
    void joinChildTextNodes(ContainerNode*, const Position& start, const Position& end);

    HTMLElement* splitAncestorsWithUnicodeBidi(Node*, bool before, WritingDirection allowedDirection);
    void removeEmbeddingUpToEnclosingBlock(Node*, HTMLElement* unsplitAncestor);

    void updateStartEnd(const Position& newStart, const Position& newEnd);
    Position startPosition();
    Position endPosition();

    RefPtrWillBeMember<EditingStyle> m_style;
    EditAction m_editingAction;
    EPropertyLevel m_propertyLevel;
    Position m_start;
    Position m_end;
    bool m_useEndingSelection;
    RefPtrWillBeMember<Element> m_styledInlineElement;
    bool m_removeOnly;
    IsInlineElementToRemoveFunction m_isInlineElementToRemoveFunction;
};

enum ShouldStyleAttributeBeEmpty { AllowNonEmptyStyleAttribute, StyleAttributeShouldBeEmpty };
bool isEmptyFontTag(const Element*, ShouldStyleAttributeBeEmpty = StyleAttributeShouldBeEmpty);
bool isLegacyAppleHTMLSpanElement(const Node*);
bool isStyleSpanOrSpanWithOnlyStyleAttribute(const Element*);
PassRefPtrWillBeRawPtr<HTMLSpanElement> createStyleSpanElement(Document&);

} // namespace blink

#endif
