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

#include "config.h"

#include "core/layout/LayoutRuby.h"

#include "core/frame/UseCounter.h"
#include "core/layout/LayoutRubyRun.h"

namespace blink {

// === generic helper functions to avoid excessive code duplication ===

static LayoutRubyRun* lastRubyRun(const LayoutObject* ruby)
{
    LayoutObject* child = ruby->slowLastChild();
    ASSERT(!child || child->isRubyRun());
    return toLayoutRubyRun(child);
}

static inline LayoutRubyRun* findRubyRunParent(LayoutObject* child)
{
    while (child && !child->isRubyRun())
        child = child->parent();
    return toLayoutRubyRun(child);
}

// === ruby as inline object ===

LayoutRubyAsInline::LayoutRubyAsInline(Element* element)
    : LayoutInline(element)
{
    UseCounter::count(document(), UseCounter::RenderRuby);
}

LayoutRubyAsInline::~LayoutRubyAsInline()
{
}

void LayoutRubyAsInline::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutInline::styleDidChange(diff, oldStyle);
    propagateStyleToAnonymousChildren();
}

void LayoutRubyAsInline::addChild(LayoutObject* child, LayoutObject* beforeChild)
{
    // If the child is a ruby run, just add it normally.
    if (child->isRubyRun()) {
        LayoutInline::addChild(child, beforeChild);
        return;
    }

    if (beforeChild) {
        // insert child into run
        LayoutObject* run = beforeChild;
        while (run && !run->isRubyRun())
            run = run->parent();
        if (run) {
            if (beforeChild == run)
                beforeChild = toLayoutRubyRun(beforeChild)->firstChild();
            ASSERT(!beforeChild || beforeChild->isDescendantOf(run));
            run->addChild(child, beforeChild);
            return;
        }
        ASSERT_NOT_REACHED(); // beforeChild should always have a run as parent!
        // Emergency fallback: fall through and just append.
    }

    // If the new child would be appended, try to add the child to the previous run
    // if possible, or create a new run otherwise.
    // (The LayoutRubyRun object will handle the details)
    LayoutRubyRun* lastRun = lastRubyRun(this);
    if (!lastRun || lastRun->hasRubyText()) {
        lastRun = LayoutRubyRun::staticCreateRubyRun(this);
        LayoutInline::addChild(lastRun, beforeChild);
    }
    lastRun->addChild(child);
}

void LayoutRubyAsInline::removeChild(LayoutObject* child)
{
    // If the child's parent is *this (must be a ruby run), just use the normal remove method.
    if (child->parent() == this) {
        ASSERT(child->isRubyRun());
        LayoutInline::removeChild(child);
        return;
    }

    // Otherwise find the containing run and remove it from there.
    LayoutRubyRun* run = findRubyRunParent(child);
    ASSERT(run);
    run->removeChild(child);
}

// === ruby as block object ===

LayoutRubyAsBlock::LayoutRubyAsBlock(Element* element)
    : LayoutBlockFlow(element)
{
    UseCounter::count(document(), UseCounter::RenderRuby);
}

LayoutRubyAsBlock::~LayoutRubyAsBlock()
{
}

void LayoutRubyAsBlock::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBlockFlow::styleDidChange(diff, oldStyle);
    propagateStyleToAnonymousChildren();
}

void LayoutRubyAsBlock::addChild(LayoutObject* child, LayoutObject* beforeChild)
{
    // If the child is a ruby run, just add it normally.
    if (child->isRubyRun()) {
        LayoutBlockFlow::addChild(child, beforeChild);
        return;
    }

    if (beforeChild) {
        // insert child into run
        LayoutObject* run = beforeChild;
        while (run && !run->isRubyRun())
            run = run->parent();
        if (run) {
            if (beforeChild == run)
                beforeChild = toLayoutRubyRun(beforeChild)->firstChild();
            ASSERT(!beforeChild || beforeChild->isDescendantOf(run));
            run->addChild(child, beforeChild);
            return;
        }
        ASSERT_NOT_REACHED(); // beforeChild should always have a run as parent!
        // Emergency fallback: fall through and just append.
    }

    // If the new child would be appended, try to add the child to the previous run
    // if possible, or create a new run otherwise.
    // (The LayoutRubyRun object will handle the details)
    LayoutRubyRun* lastRun = lastRubyRun(this);
    if (!lastRun || lastRun->hasRubyText()) {
        lastRun = LayoutRubyRun::staticCreateRubyRun(this);
        LayoutBlockFlow::addChild(lastRun, beforeChild);
    }
    lastRun->addChild(child);
}

void LayoutRubyAsBlock::removeChild(LayoutObject* child)
{
    // If the child's parent is *this (must be a ruby run), just use the normal remove method.
    if (child->parent() == this) {
        ASSERT(child->isRubyRun());
        LayoutBlockFlow::removeChild(child);
        return;
    }

    // Otherwise find the containing run and remove it from there.
    LayoutRubyRun* run = findRubyRunParent(child);
    ASSERT(run);
    run->removeChild(child);
}

} // namespace blink
