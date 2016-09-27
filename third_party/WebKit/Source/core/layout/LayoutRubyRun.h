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

#ifndef LayoutRubyRun_h
#define LayoutRubyRun_h

#include "core/layout/LayoutBlockFlow.h"

namespace blink {

class LayoutRubyBase;
class LayoutRubyText;

// LayoutRubyRun are 'inline-block/table' like objects,and wrap a single pairing of a ruby base with its ruby text(s).
// See LayoutRuby.h for further comments on the structure

class LayoutRubyRun final : public LayoutBlockFlow {
public:
    virtual ~LayoutRubyRun();

    bool hasRubyText() const;
    bool hasRubyBase() const;
    LayoutRubyText* rubyText() const;
    LayoutRubyBase* rubyBase() const;
    LayoutRubyBase* rubyBaseSafe(); // creates the base if it doesn't already exist

    virtual LayoutObject* layoutSpecialExcludedChild(bool relayoutChildren, SubtreeLayoutScope&) override;
    virtual void layout() override;

    virtual bool isChildAllowed(LayoutObject*, const ComputedStyle&) const override;
    virtual void addChild(LayoutObject* child, LayoutObject* beforeChild = nullptr) override;
    virtual void removeChild(LayoutObject* child) override;

    void getOverhang(bool firstLine, LayoutObject* startLayoutObject, LayoutObject* endLayoutObject, int& startOverhang, int& endOverhang) const;

    static LayoutRubyRun* staticCreateRubyRun(const LayoutObject* parentRuby);

    virtual const char* name() const override { return "LayoutRubyRun"; }

protected:
    LayoutRubyBase* createRubyBase() const;

private:
    LayoutRubyRun();

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectRubyRun || LayoutBlockFlow::isOfType(type); }
    virtual bool createsAnonymousWrapper() const override { return true; }
    virtual void removeLeftoverAnonymousBlock(LayoutBlock*) override { }
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutRubyRun, isRubyRun());

} // namespace blink

#endif // LayoutRubyRun_h
