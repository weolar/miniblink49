/*
 * (C) 1999 Lars Knoll (knoll@kde.org)
 * (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004-2009, 2013 Apple Inc. All rights reserved.
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

#ifndef LayoutText_h
#define LayoutText_h

#include "core/CoreExport.h"
#include "core/dom/Text.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/TextRunConstructor.h"
#include "platform/LengthFunctions.h"
#include "platform/text/TextPath.h"
#include "wtf/Forward.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class AbstractInlineTextBox;
class InlineTextBox;

class CORE_EXPORT LayoutText : public LayoutObject {
public:
    // FIXME: If the node argument is not a Text node or the string argument is
    // not the content of the Text node, updating text-transform property
    // doesn't re-transform the string.
    LayoutText(Node*, PassRefPtr<StringImpl>);
#if ENABLE(ASSERT)
    virtual ~LayoutText();
#endif

    virtual const char* name() const override { return "LayoutText"; }

    virtual bool isTextFragment() const;
    virtual bool isWordBreak() const;

    virtual PassRefPtr<StringImpl> originalText() const;

    void extractTextBox(InlineTextBox*);
    void attachTextBox(InlineTextBox*);
    void removeTextBox(InlineTextBox*);

    const String& text() const { return m_text; }
    virtual unsigned textStartOffset() const { return 0; }
    String plainText() const;

    InlineTextBox* createInlineTextBox(int start, unsigned short length);
    void dirtyOrDeleteLineBoxesIfNeeded(bool fullLayout);
    void dirtyLineBoxes();

    virtual void absoluteRects(Vector<IntRect>&, const LayoutPoint& accumulatedOffset) const override final;
    void absoluteRectsForRange(Vector<IntRect>&, unsigned startOffset = 0, unsigned endOffset = INT_MAX, bool useSelectionHeight = false, bool* wasFixed = nullptr);

    virtual void absoluteQuads(Vector<FloatQuad>&, bool* wasFixed) const override final;
    void absoluteQuadsForRange(Vector<FloatQuad>&, unsigned startOffset = 0, unsigned endOffset = INT_MAX, bool useSelectionHeight = false, bool* wasFixed = nullptr);

    enum ClippingOption { NoClipping, ClipToEllipsis };
    void absoluteQuads(Vector<FloatQuad>&, bool* wasFixed = nullptr, ClippingOption = NoClipping) const;

    virtual PositionWithAffinity positionForPoint(const LayoutPoint&) override;

    bool is8Bit() const { return m_text.is8Bit(); }
    const LChar* characters8() const { return m_text.impl()->characters8(); }
    const UChar* characters16() const { return m_text.impl()->characters16(); }
    bool hasEmptyText() const { return m_text.isEmpty(); }
    UChar characterAt(unsigned) const;
    UChar uncheckedCharacterAt(unsigned) const;
    UChar operator[](unsigned i) const { return uncheckedCharacterAt(i); }
    unsigned textLength() const { return m_text.length(); } // non virtual implementation of length()
    void positionLineBox(InlineBox*);

    virtual float width(unsigned from, unsigned len, const Font&, LayoutUnit xPos, TextDirection, HashSet<const SimpleFontData*>* fallbackFonts = nullptr, FloatRect* glyphBounds = nullptr) const;
    virtual float width(unsigned from, unsigned len, LayoutUnit xPos, TextDirection, bool firstLine = false, HashSet<const SimpleFontData*>* fallbackFonts = nullptr, FloatRect* glyphBounds = nullptr) const;

    float minLogicalWidth() const;
    float maxLogicalWidth() const;

    void trimmedPrefWidths(LayoutUnit leadWidth,
        LayoutUnit& firstLineMinWidth, bool& hasBreakableStart,
        LayoutUnit& lastLineMinWidth, bool& hasBreakableEnd,
        bool& hasBreakableChar, bool& hasBreak,
        LayoutUnit& firstLineMaxWidth, LayoutUnit& lastLineMaxWidth,
        LayoutUnit& minWidth, LayoutUnit& maxWidth, bool& stripFrontSpaces,
        TextDirection);

    virtual IntRect linesBoundingBox() const;
    LayoutRect linesVisualOverflowBoundingBox() const;

    FloatPoint firstRunOrigin() const;
    float firstRunX() const;
    float firstRunY() const;

    virtual void setText(PassRefPtr<StringImpl>, bool force = false);
    void setTextWithOffset(PassRefPtr<StringImpl>, unsigned offset, unsigned len, bool force = false);

    virtual void transformText();

    virtual bool canBeSelectionLeaf() const override { return true; }
    virtual void setSelectionState(SelectionState) override final;
    virtual LayoutRect selectionRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer) const override;
    virtual LayoutRect localCaretRect(InlineBox*, int caretOffset, LayoutUnit* extraWidthToEndOfLine = nullptr) override;

    InlineTextBox* firstTextBox() const { return m_firstTextBox; }
    InlineTextBox* lastTextBox() const { return m_lastTextBox; }

    virtual int caretMinOffset() const override;
    virtual int caretMaxOffset() const override;
    unsigned resolvedTextLength() const;

    virtual int previousOffset(int current) const override final;
    virtual int previousOffsetForBackwardDeletion(int current) const override final;
    virtual int nextOffset(int current) const override final;

    bool containsReversedText() const { return m_containsReversedText; }

    bool isSecure() const { return style()->textSecurity() != TSNONE; }
    void momentarilyRevealLastTypedCharacter(unsigned lastTypedCharacterOffset);

    bool isAllCollapsibleWhitespace() const;

    bool canUseSimpleFontCodePath() const { return m_canUseSimpleFontCodePath; }

    void removeAndDestroyTextBoxes();

    PassRefPtr<AbstractInlineTextBox> firstAbstractInlineTextBox();

    float hyphenWidth(const Font&, TextDirection);

protected:
    virtual void willBeDestroyed() override;

    virtual void styleWillChange(StyleDifference, const ComputedStyle&) override final { }
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

    virtual void setTextInternal(PassRefPtr<StringImpl>);
    virtual UChar previousCharacter() const;

    virtual void addLayerHitTestRects(LayerHitTestRects&, const DeprecatedPaintLayer* currentLayer, const LayoutPoint& layerOffset, const LayoutRect& containerRect) const override;

    virtual InlineTextBox* createTextBox(int start, unsigned short length); // Subclassed by SVG.

    virtual void invalidateDisplayItemClients(const LayoutBoxModelObject& paintInvalidationContainer) const override;

private:
    void computePreferredLogicalWidths(float leadWidth);
    void computePreferredLogicalWidths(float leadWidth, HashSet<const SimpleFontData*>& fallbackFonts, FloatRect& glyphBounds);

    bool computeCanUseSimpleFontCodePath() const;

    // Make length() private so that callers that have a LayoutText*
    // will use the more efficient textLength() instead, while
    // callers with a LayoutObject* can continue to use length().
    virtual unsigned length() const override final { return textLength(); }

    virtual void paint(const PaintInfo&, const LayoutPoint&) override final { ASSERT_NOT_REACHED(); }
    virtual void layout() override final { ASSERT_NOT_REACHED(); }
    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation&, const LayoutPoint&, HitTestAction) override final { ASSERT_NOT_REACHED(); return false; }

    void deleteTextBoxes();
    bool containsOnlyWhitespace(unsigned from, unsigned len) const;
    float widthFromFont(const Font&, int start, int len, float leadWidth, float textWidthSoFar, TextDirection, HashSet<const SimpleFontData*>* fallbackFonts, FloatRect* glyphBoundsAccumulation) const;

    void secureText(UChar mask);

    bool isText() const = delete; // This will catch anyone doing an unnecessary check.

    virtual LayoutRect clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const override;

    void checkConsistency() const;

    // We put the bitfield first to minimize padding on 64-bit.
    bool m_hasBreakableChar : 1; // Whether or not we can be broken into multiple lines.
    bool m_hasBreak : 1; // Whether or not we have a hard break (e.g., <pre> with '\n').
    bool m_hasTab : 1; // Whether or not we have a variable width tab character (e.g., <pre> with '\t').
    bool m_hasBreakableStart : 1;
    bool m_hasBreakableEnd : 1;
    bool m_hasEndWhiteSpace : 1;
    // This bit indicates that the text run has already dirtied specific
    // line boxes, and this hint will enable layoutInlineChildren to avoid
    // just dirtying everything when character data is modified (e.g., appended/inserted
    // or removed).
    bool m_linesDirty : 1;
    bool m_containsReversedText : 1;
    bool m_canUseSimpleFontCodePath : 1;
    mutable bool m_knownToHaveNoOverflowAndNoFallbackFonts : 1;

    float m_minWidth;
    float m_maxWidth;
    float m_firstLineMinWidth;
    float m_lastLineLineMinWidth;

    String m_text;

    InlineTextBox* m_firstTextBox;
    InlineTextBox* m_lastTextBox;
};

inline UChar LayoutText::uncheckedCharacterAt(unsigned i) const
{
    ASSERT_WITH_SECURITY_IMPLICATION(i < textLength());
    return is8Bit() ? characters8()[i] : characters16()[i];
}

inline UChar LayoutText::characterAt(unsigned i) const
{
    if (i >= textLength())
        return 0;

    return uncheckedCharacterAt(i);
}

inline float LayoutText::hyphenWidth(const Font& font, TextDirection direction)
{
    const ComputedStyle& style = styleRef();
    return font.width(constructTextRun(this, font, style.hyphenString().string(), style, direction));
}

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutText, isText());

#if !ENABLE(ASSERT)
inline void LayoutText::checkConsistency() const
{
}
#endif

inline LayoutText* Text::layoutObject() const
{
    return toLayoutText(CharacterData::layoutObject());
}

void applyTextTransform(const ComputedStyle*, String&, UChar);

} // namespace blink

#endif // LayoutText_h
