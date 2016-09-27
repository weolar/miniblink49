/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
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

#ifndef Editor_h
#define Editor_h

#include "core/CoreExport.h"
#include "core/clipboard/DataTransferAccessPolicy.h"
#include "core/dom/DocumentMarker.h"
#include "core/editing/EditAction.h"
#include "core/editing/EditingBehavior.h"
#include "core/editing/EphemeralRange.h"
#include "core/editing/FindOptions.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/VisibleSelection.h"
#include "core/editing/WritingDirection.h"
#include "core/editing/iterators/TextIterator.h"
#include "platform/PasteMode.h"
#include "platform/heap/Handle.h"

namespace blink {

class CompositeEditCommand;
class DummyPageHolder;
class EditCommandComposition;
class EditorClient;
class EditorInternalCommand;
class LocalFrame;
class HitTestResult;
class KillRing;
class Pasteboard;
class SpellChecker;
class StylePropertySet;
class TextEvent;
class UndoStack;

enum EditorCommandSource { CommandFromMenuOrKeyBinding, CommandFromDOM };
enum EditorParagraphSeparator { EditorParagraphSeparatorIsDiv, EditorParagraphSeparatorIsP };

class CORE_EXPORT Editor final : public NoBaseWillBeGarbageCollectedFinalized<Editor> {
    WTF_MAKE_NONCOPYABLE(Editor);
public:
    static PassOwnPtrWillBeRawPtr<Editor> create(LocalFrame&);
    ~Editor();

    EditorClient& client() const;

    CompositeEditCommand* lastEditCommand() { return m_lastEditCommand.get(); }

    void handleKeyboardEvent(KeyboardEvent*);
    bool handleTextEvent(TextEvent*);

    bool canEdit() const;
    bool canEditRichly() const;

    bool canDHTMLCut();
    bool canDHTMLCopy();

    bool canCut() const;
    bool canCopy() const;
    bool canPaste() const;
    bool canDelete() const;
    bool canSmartCopyOrDelete() const;

    void cut();
    void copy();
    void paste();
    void pasteAsPlainText();
    void performDelete();

    static void countEvent(ExecutionContext*, const Event*);
    void copyImage(const HitTestResult&);

    void transpose();

    void respondToChangedContents(const VisibleSelection& endingSelection);

    bool selectionStartHasStyle(CSSPropertyID, const String& value) const;
    TriState selectionHasStyle(CSSPropertyID, const String& value) const;
    String selectionStartCSSPropertyValue(CSSPropertyID);

    void removeFormattingAndStyle();

    void clearLastEditCommand();

    bool deleteWithDirection(SelectionDirection, TextGranularity, bool killRing, bool isTypingAction);
    void deleteSelectionWithSmartDelete(bool smartDelete);

    void applyStyle(StylePropertySet*, EditAction = EditActionUnspecified);
    void applyParagraphStyle(StylePropertySet*, EditAction = EditActionUnspecified);
    void applyStyleToSelection(StylePropertySet*, EditAction);
    void applyParagraphStyleToSelection(StylePropertySet*, EditAction);

    void appliedEditing(PassRefPtrWillBeRawPtr<CompositeEditCommand>);
    void unappliedEditing(PassRefPtrWillBeRawPtr<EditCommandComposition>);
    void reappliedEditing(PassRefPtrWillBeRawPtr<EditCommandComposition>);

    void setShouldStyleWithCSS(bool flag) { m_shouldStyleWithCSS = flag; }
    bool shouldStyleWithCSS() const { return m_shouldStyleWithCSS; }

    class CORE_EXPORT Command {
        STACK_ALLOCATED();
    public:
        Command();
        Command(const EditorInternalCommand*, EditorCommandSource, PassRefPtrWillBeRawPtr<LocalFrame>);

        bool execute(const String& parameter = String(), Event* triggeringEvent = nullptr) const;
        bool execute(Event* triggeringEvent) const;

        bool isSupported() const;
        bool isEnabled(Event* triggeringEvent = nullptr) const;

        TriState state(Event* triggeringEvent = nullptr) const;
        String value(Event* triggeringEvent = nullptr) const;

        bool isTextInsertion() const;

        // Returns 0 if this Command is not supported.
        int idForHistogram() const;
    private:
        LocalFrame& frame() const
        {
            ASSERT(m_frame);
            return *m_frame;
        }

        const EditorInternalCommand* m_command;
        EditorCommandSource m_source;
        RefPtrWillBeMember<LocalFrame> m_frame;
    };
    Command command(const String& commandName); // Command source is CommandFromMenuOrKeyBinding.
    Command command(const String& commandName, EditorCommandSource);

    // |Editor::executeCommand| is implementation of |WebFrame::executeCommand|
    // rather than |Document::execCommand|.
    bool executeCommand(const String&);
    bool executeCommand(const String& commandName, const String& value);

    bool insertText(const String&, KeyboardEvent* triggeringEvent);
    bool insertTextWithoutSendingTextEvent(const String&, bool selectInsertedText, TextEvent* triggeringEvent);
    bool insertLineBreak();
    bool insertParagraphSeparator();

    bool isOverwriteModeEnabled() const { return m_overwriteModeEnabled; }
    void toggleOverwriteModeEnabled();

    bool canUndo();
    void undo();
    bool canRedo();
    void redo();

    void setBaseWritingDirection(WritingDirection);

    // smartInsertDeleteEnabled and selectTrailingWhitespaceEnabled are
    // mutually exclusive, meaning that enabling one will disable the other.
    bool smartInsertDeleteEnabled() const;
    bool isSelectTrailingWhitespaceEnabled() const;

    bool preventRevealSelection() const { return m_preventRevealSelection; }

    void setStartNewKillRingSequence(bool);

    void clear();

    VisibleSelection selectionForCommand(Event*);

    KillRing& killRing() const { return *m_killRing; }

    EditingBehavior behavior() const;

    EphemeralRange selectedRange();

    // TODO(yosin) We should get rid of |addToKillRing()| with |Range| for
    // Oilpan.
    void addToKillRing(Range*, bool prepend);
    void addToKillRing(const EphemeralRange&, bool prepend);

    void pasteAsFragment(PassRefPtrWillBeRawPtr<DocumentFragment>, bool smartReplace, bool matchStyle);
    void pasteAsPlainText(const String&, bool smartReplace);

    Element* findEventTargetFrom(const VisibleSelection&) const;

    bool findString(const String&, FindOptions);

    PassRefPtrWillBeRawPtr<Range> findStringAndScrollToVisible(const String&, Range*, FindOptions);

    const VisibleSelection& mark() const; // Mark, to be used as emacs uses it.
    void setMark(const VisibleSelection&);

    void computeAndSetTypingStyle(StylePropertySet* , EditAction = EditActionUnspecified);

    IntRect firstRectForRange(Range*) const;

    void respondToChangedSelection(const VisibleSelection& oldSelection, FrameSelection::SetSelectionOptions);

    bool markedTextMatchesAreHighlighted() const;
    void setMarkedTextMatchesAreHighlighted(bool);

    void replaceSelectionWithFragment(PassRefPtrWillBeRawPtr<DocumentFragment>, bool selectReplacement, bool smartReplace, bool matchStyle);
    void replaceSelectionWithText(const String&, bool selectReplacement, bool smartReplace);

    EditorParagraphSeparator defaultParagraphSeparator() const { return m_defaultParagraphSeparator; }
    void setDefaultParagraphSeparator(EditorParagraphSeparator separator) { m_defaultParagraphSeparator = separator; }

    class RevealSelectionScope {
        WTF_MAKE_NONCOPYABLE(RevealSelectionScope);
        STACK_ALLOCATED();
    public:
        explicit RevealSelectionScope(Editor*);
        ~RevealSelectionScope();
    private:
        RawPtrWillBeMember<Editor> m_editor;
    };
    friend class RevealSelectionScope;

    DECLARE_TRACE();

private:
    RawPtrWillBeMember<LocalFrame> m_frame;
    RefPtrWillBeMember<CompositeEditCommand> m_lastEditCommand;
    int m_preventRevealSelection;
    bool m_shouldStartNewKillRingSequence;
    bool m_shouldStyleWithCSS;
    const OwnPtr<KillRing> m_killRing;
    VisibleSelection m_mark;
    bool m_areMarkedTextMatchesHighlighted;
    EditorParagraphSeparator m_defaultParagraphSeparator;
    bool m_overwriteModeEnabled;

    explicit Editor(LocalFrame&);

    LocalFrame& frame() const
    {
        ASSERT(m_frame);
        return *m_frame;
    }

    bool canDeleteRange(const EphemeralRange&) const;
    bool shouldDeleteRange(const EphemeralRange&) const;

    UndoStack* undoStack() const;

    bool tryDHTMLCopy();
    bool tryDHTMLCut();
    bool tryDHTMLPaste(PasteMode);

    bool canSmartReplaceWithPasteboard(Pasteboard*);
    void pasteAsPlainTextWithPasteboard(Pasteboard*);
    void pasteWithPasteboard(Pasteboard*);
    void writeSelectionToPasteboard();
    bool dispatchCPPEvent(const AtomicString&, DataTransferAccessPolicy, PasteMode = AllMimeTypes);

    void revealSelectionAfterEditingOperation(const ScrollAlignment& = ScrollAlignment::alignCenterIfNeeded, RevealExtentOption = DoNotRevealExtent);
    void changeSelectionAfterCommand(const VisibleSelection& newSelection, FrameSelection::SetSelectionOptions);
    void notifyComponentsOnChangedSelection(const VisibleSelection& oldSelection, FrameSelection::SetSelectionOptions);

    Element* findEventTargetFromSelection() const;

    PassRefPtrWillBeRawPtr<Range> findRangeOfString(const String&, Range*, FindOptions);

    SpellChecker& spellChecker() const;

    bool handleEditingKeyboardEvent(KeyboardEvent*);
};

inline void Editor::setStartNewKillRingSequence(bool flag)
{
    m_shouldStartNewKillRingSequence = flag;
}

inline const VisibleSelection& Editor::mark() const
{
    return m_mark;
}

inline void Editor::setMark(const VisibleSelection& selection)
{
    m_mark = selection;
}

inline bool Editor::markedTextMatchesAreHighlighted() const
{
    return m_areMarkedTextMatchesHighlighted;
}


} // namespace blink

#endif // Editor_h
