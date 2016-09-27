/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef Text_h
#define Text_h

#include "core/CoreExport.h"
#include "core/dom/CharacterData.h"

namespace blink {

class ExceptionState;
class LayoutText;

class CORE_EXPORT Text : public CharacterData {
    DEFINE_WRAPPERTYPEINFO();
public:
    static const unsigned defaultLengthLimit = 1 << 16;

    static PassRefPtrWillBeRawPtr<Text> create(Document&, const String&);
    static PassRefPtrWillBeRawPtr<Text> createEditingText(Document&, const String&);

    LayoutText* layoutObject() const;

    // mergeNextSiblingNodesIfPossible() merges next sibling nodes if possible
    // then returns a node not merged.
    PassRefPtrWillBeRawPtr<Node> mergeNextSiblingNodesIfPossible();
    PassRefPtrWillBeRawPtr<Text> splitText(unsigned offset, ExceptionState&);

    // DOM Level 3: http://www.w3.org/TR/DOM-Level-3-Core/core.html#ID-1312295772

    String wholeText() const;
    PassRefPtrWillBeRawPtr<Text> replaceWholeText(const String&);

    void recalcTextStyle(StyleRecalcChange, Text* nextTextSibling);
    bool textLayoutObjectIsNeeded(const ComputedStyle&, const LayoutObject& parent);
    LayoutText* createTextLayoutObject(const ComputedStyle&);
    void updateTextLayoutObject(unsigned offsetOfReplacedData, unsigned lengthOfReplacedData, RecalcStyleBehavior = DoNotRecalcStyle);

    void attach(const AttachContext& = AttachContext()) final;
    void reattachIfNeeded(const AttachContext& = AttachContext());

    bool canContainRangeEndPoint() const final { return true; }
    NodeType nodeType() const override;

    DECLARE_VIRTUAL_TRACE();

protected:
    Text(TreeScope& treeScope, const String& data, ConstructionType type)
        : CharacterData(treeScope, data, type) { }

private:
    String nodeName() const override;
    PassRefPtrWillBeRawPtr<Node> cloneNode(bool deep = true) final;

    bool isTextNode() const = delete; // This will catch anyone doing an unnecessary check.

    bool needsWhitespaceLayoutObject();

    virtual PassRefPtrWillBeRawPtr<Text> cloneWithData(const String&);

#ifndef NDEBUG
    void formatForDebugger(char* buffer, unsigned length) const override;
#endif
};

DEFINE_NODE_TYPE_CASTS(Text, isTextNode());

} // namespace blink

#endif // Text_h
