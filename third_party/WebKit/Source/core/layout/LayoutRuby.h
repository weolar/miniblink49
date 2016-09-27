/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef LayoutRuby_h
#define LayoutRuby_h

#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutInline.h"

namespace blink {

// Following the HTML 5 spec, the box object model for a <ruby> element allows several runs of ruby
// bases with their respective ruby texts looks as follows:
//
// 1 LayoutRuby object, corresponding to the whole <ruby> HTML element
//      1+ LayoutRubyRun (anonymous)
//          0 or 1 LayoutRubyText - shuffled to the front in order to re-use existing block layouting
//              0-n inline object(s)
//          0 or 1 LayoutRubyBase - contains the inline objects that make up the ruby base
//              1-n inline object(s)
//
// Note: <rp> elements are defined as having 'display:none' and thus normally are not assigned a layoutObject.
//
// Generated :before/:after content is shunted into anonymous inline blocks

// <ruby> when used as 'display:inline'
class LayoutRubyAsInline final : public LayoutInline {
public:
    LayoutRubyAsInline(Element*);
    virtual ~LayoutRubyAsInline();

    virtual void addChild(LayoutObject* child, LayoutObject* beforeChild = nullptr) override;
    virtual void removeChild(LayoutObject* child) override;

    virtual const char* name() const override { return "LayoutRuby (inline)"; }

protected:
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectRuby || LayoutInline::isOfType(type); }
    virtual bool createsAnonymousWrapper() const override { return true; }
};

// <ruby> when used as 'display:block' or 'display:inline-block'
class LayoutRubyAsBlock final : public LayoutBlockFlow {
public:
    LayoutRubyAsBlock(Element*);
    virtual ~LayoutRubyAsBlock();

    virtual void addChild(LayoutObject* child, LayoutObject* beforeChild = nullptr) override;
    virtual void removeChild(LayoutObject* child) override;

    virtual const char* name() const override { return "LayoutRuby (block)"; }

protected:
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectRuby || LayoutBlockFlow::isOfType(type); }
    virtual bool createsAnonymousWrapper() const override { return true; }
    virtual void removeLeftoverAnonymousBlock(LayoutBlock*) override { ASSERT_NOT_REACHED(); }
};

} // namespace blink

#endif // LayoutRuby_h
