/*
 * Copyright (C) 2011 Nokia Inc. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#ifndef LayoutQuote_h
#define LayoutQuote_h

#include "core/layout/LayoutInline.h"
#include "core/style/ComputedStyle.h"
#include "core/style/ComputedStyleConstants.h"
#include "core/style/QuotesData.h"

namespace blink {

class Document;
class LayoutTextFragment;

class LayoutQuote final : public LayoutInline {
public:
    LayoutQuote(Document*, const QuoteType);
    virtual ~LayoutQuote();
    void attachQuote();

    virtual const char* name() const override { return "LayoutQuote"; }

private:
    void detachQuote();

    virtual void willBeDestroyed() override;
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectQuote || LayoutInline::isOfType(type); }
    virtual void styleDidChange(StyleDifference, const ComputedStyle*) override;
    virtual void willBeRemovedFromTree() override;

    String computeText() const;
    void updateText();
    const QuotesData* quotesData() const;
    void updateDepth();
    bool isAttached() { return m_attached; }

    LayoutTextFragment* findFragmentChild() const;

    QuoteType m_type;
    int m_depth;
    LayoutQuote* m_next;
    LayoutQuote* m_previous;
    bool m_attached;
    String m_text;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutQuote, isQuote());

} // namespace blink

#endif // LayoutQuote_h
