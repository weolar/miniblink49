// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LineLayoutItem_h
#define LineLayoutItem_h

#include "core/layout/LayoutObject.h"

#include "platform/LayoutUnit.h"

namespace blink {

class ComputedStyle;
class Document;
class LayoutObject;

class LineLayoutItem {
public:
    explicit LineLayoutItem(LayoutObject* layoutObject)
        : m_layoutObject(layoutObject)
    {
    }

    LineLayoutItem(const LineLayoutItem& item) : m_layoutObject(item.m_layoutObject) { }

    LineLayoutItem(std::nullptr_t)
        : m_layoutObject(0)
    {
    }

    LineLayoutItem() : m_layoutObject(0) { }

    // TODO(pilgrim): Remove this. It's only here to make things compile before
    // switching all of core/layout/line to using the API.
    // https://crbug.com/499321
    operator LayoutObject*() const { return m_layoutObject; }

    LineLayoutItem* operator->() { return this; }

    LineLayoutItem parent() const
    {
        return LineLayoutItem(m_layoutObject->parent());
    }

    LineLayoutItem nextSibling() const
    {
        return LineLayoutItem(m_layoutObject->nextSibling());
    }

    LineLayoutItem previousSibling() const
    {
        return LineLayoutItem(m_layoutObject->previousSibling());
    }

    LineLayoutItem slowFirstChild() const
    {
        return LineLayoutItem(m_layoutObject->slowFirstChild());
    }

    LineLayoutItem slowLastChild() const
    {
        return LineLayoutItem(m_layoutObject->slowLastChild());
    }

    const ComputedStyle* style() const
    {
        return m_layoutObject->style();
    }

    const ComputedStyle& styleRef() const
    {
        return m_layoutObject->styleRef();
    }

    Document& document() const
    {
        return m_layoutObject->document();
    }

    bool preservesNewline() const
    {
        return m_layoutObject->preservesNewline();
    }

    unsigned length() const
    {
        return m_layoutObject->length();
    }

    bool isFloatingOrOutOfFlowPositioned() const
    {
        return m_layoutObject->isFloatingOrOutOfFlowPositioned();
    }

    bool isFloating() const
    {
        return m_layoutObject->isFloating();
    }

    bool isOutOfFlowPositioned() const
    {
        return m_layoutObject->isOutOfFlowPositioned();
    }

    bool isBox() const
    {
        return m_layoutObject->isBox();
    }

    bool isBR() const
    {
        return m_layoutObject->isBR();
    }

    bool isHorizontalWritingMode() const
    {
        return m_layoutObject->isHorizontalWritingMode();
    }

    bool isImage() const
    {
        return m_layoutObject->isImage();
    }

    bool isLayoutBlockFlow() const
    {
        return m_layoutObject->isLayoutBlockFlow();
    }

    bool isLayoutInline() const
    {
        return m_layoutObject->isLayoutInline();
    }

    bool isListMarker() const
    {
        return m_layoutObject->isListMarker();
    }

    bool isReplaced() const
    {
        return m_layoutObject->isReplaced();
    }

    bool isRubyRun() const
    {
        return m_layoutObject->isRubyRun();
    }

    bool isSVGInlineText() const
    {
        return m_layoutObject->isSVGInlineText();
    }

    bool isTableCell() const
    {
        return m_layoutObject->isTableCell();
    }

    bool isText() const
    {
        return m_layoutObject->isText();
    }

protected:
    LayoutObject* layoutObject() { return m_layoutObject; }
    const LayoutObject* layoutObject() const { return m_layoutObject; }

private:
    LayoutObject* m_layoutObject;
};

} // namespace blink

#endif // LineLayoutItem_h
