/*
 * Copyright (c) 2013, Opera Software ASA. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Opera Software ASA nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VTTCue_h
#define VTTCue_h

#include "core/html/track/TextTrackCue.h"
#include "platform/heap/Handle.h"

namespace blink {

class Document;
class DoubleOrAutoKeyword;
class ExecutionContext;
class VTTCue;
class VTTScanner;

struct VTTDisplayParameters {
    VTTDisplayParameters();

    FloatPoint position;
    float size;
    CSSValueID direction;
    CSSValueID textAlign;
    CSSValueID writingMode;
    float snapToLinesPosition;
};

class VTTCueBox final : public HTMLDivElement {
public:
    static PassRefPtrWillBeRawPtr<VTTCueBox> create(Document& document)
    {
        return adoptRefWillBeNoop(new VTTCueBox(document));
    }

    void applyCSSProperties(const VTTDisplayParameters&);

private:
    explicit VTTCueBox(Document&);

    LayoutObject* createLayoutObject(const ComputedStyle&) override;

    // The computed line position for snap-to-lines layout, and NaN for
    // non-snap-to-lines layout where no adjustment should take place.
    // This is set in applyCSSProperties and propagated to LayoutVTTCue.
    float m_snapToLinesPosition;
};

class VTTCue final : public TextTrackCue {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<VTTCue> create(Document& document, double startTime, double endTime, const String& text)
    {
        return adoptRefWillBeNoop(new VTTCue(document, startTime, endTime, text));
    }

    ~VTTCue() override;

    const String& vertical() const;
    void setVertical(const String&);

    bool snapToLines() const { return m_snapToLines; }
    void setSnapToLines(bool);

    void line(DoubleOrAutoKeyword&) const;
    void setLine(const DoubleOrAutoKeyword&);

    void position(DoubleOrAutoKeyword&) const;
    void setPosition(const DoubleOrAutoKeyword&, ExceptionState&);

    double size() const { return m_cueSize; }
    void setSize(double, ExceptionState&);

    const String& align() const;
    void setAlign(const String&);

    const String& text() const { return m_text; }
    void setText(const String&);

    void parseSettings(const String&);

    // Applies CSS override style from user settings.
    void applyUserOverrideCSSProperties();

    PassRefPtrWillBeRawPtr<DocumentFragment> getCueAsHTML();

    const String& regionId() const { return m_regionId; }
    void setRegionId(const String&);

    void updateDisplay(HTMLDivElement& container) override;

    void updatePastAndFutureNodes(double movieTime) override;

    void removeDisplayTree(RemovalNotification) override;

    float calculateComputedLinePosition() const;

    enum WritingDirection {
        Horizontal = 0,
        VerticalGrowingLeft,
        VerticalGrowingRight,
        NumberOfWritingDirections
    };
    WritingDirection getWritingDirection() const { return m_writingDirection; }

    enum CueAlignment {
        Start = 0,
        Middle,
        End,
        Left,
        Right,
        NumberOfAlignments
    };
    CueAlignment cueAlignment() const { return m_cueAlignment; }

    ExecutionContext* executionContext() const override;

#ifndef NDEBUG
    String toString() const override;
#endif

    DECLARE_VIRTUAL_TRACE();

private:
    VTTCue(Document&, double startTime, double endTime, const String& text);

    Document& document() const;

    PassRefPtrWillBeRawPtr<VTTCueBox> getDisplayTree();

    void cueDidChange() override;

    void createVTTNodeTree();
    void copyVTTNodeToDOMTree(ContainerNode* vttNode, ContainerNode* root);

    bool lineIsAuto() const;
    bool textPositionIsAuto() const;

    VTTDisplayParameters calculateDisplayParameters() const;
    float calculateComputedTextPosition() const;
    CueAlignment calculateComputedCueAlignment() const;

    enum CueSetting {
        None,
        Vertical,
        Line,
        Position,
        Size,
        Align,
        RegionId
    };
    CueSetting settingName(VTTScanner&) const;

    String m_text;
    float m_linePosition;
    float m_textPosition;
    float m_cueSize;
    WritingDirection m_writingDirection;
    CueAlignment m_cueAlignment;
    String m_regionId;

    RefPtrWillBeMember<DocumentFragment> m_vttNodeTree;
    RefPtrWillBeMember<HTMLDivElement> m_cueBackgroundBox;
    RefPtrWillBeMember<VTTCueBox> m_displayTree;

    bool m_snapToLines : 1;
    bool m_displayTreeShouldChange : 1;
};

// VTTCue is currently the only TextTrackCue subclass.
DEFINE_TYPE_CASTS(VTTCue, TextTrackCue, cue, true, true);

} // namespace blink

#endif // VTTCue_h
