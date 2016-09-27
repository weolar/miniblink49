/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef AbstractInlineTextBox_h
#define AbstractInlineTextBox_h

#include "core/CoreExport.h"
#include "core/dom/Range.h"
#include "core/layout/LayoutText.h"
#include "core/layout/line/InlineTextBox.h"
#include "wtf/HashMap.h"
#include "wtf/RefPtr.h"

namespace blink {

class InlineTextBox;

// High-level abstraction of InlineTextBox to allow the accessibility module to
// get information about InlineTextBoxes without tight coupling.
class CORE_EXPORT AbstractInlineTextBox : public RefCounted<AbstractInlineTextBox> {
private:
    AbstractInlineTextBox(LayoutText* layoutText, InlineTextBox* inlineTextBox)
        : m_layoutText(layoutText)
        , m_inlineTextBox(inlineTextBox)
    {
    }

    static PassRefPtr<AbstractInlineTextBox> getOrCreate(LayoutText*, InlineTextBox*);
    static void willDestroy(InlineTextBox*);

    friend class LayoutText;
    friend class InlineTextBox;

public:
    struct WordBoundaries {
        WordBoundaries(int startIndex, int endIndex) : startIndex(startIndex), endIndex(endIndex) { }
        int startIndex;
        int endIndex;
    };

    enum Direction {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop
    };

    ~AbstractInlineTextBox();

    LayoutText* layoutText() const { return m_layoutText; }

    PassRefPtr<AbstractInlineTextBox> nextInlineTextBox() const;
    LayoutRect bounds() const;
    unsigned len() const;
    Direction direction() const;
    void characterWidths(Vector<float>&) const;
    void wordBoundaries(Vector<WordBoundaries>&) const;
    String text() const;
    bool isFirst() const;
    bool isLast() const;
    PassRefPtr<AbstractInlineTextBox> nextOnLine() const;
    PassRefPtr<AbstractInlineTextBox> previousOnLine() const;

private:
    void detach();

    // Weak ptrs; these are nulled when InlineTextBox::destroy() calls AbstractInlineTextBox::willDestroy.
    LayoutText* m_layoutText;
    InlineTextBox* m_inlineTextBox;

    typedef HashMap<InlineTextBox*, RefPtr<AbstractInlineTextBox>> InlineToAbstractInlineTextBoxHashMap;
    static InlineToAbstractInlineTextBoxHashMap* gAbstractInlineTextBoxMap;
};

} // namespace blink

#endif // AbstractInlineTextBox_h
