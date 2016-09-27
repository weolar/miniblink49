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

#ifndef ColorChooserClient_h
#define ColorChooserClient_h

#include "platform/ColorSuggestion.h"
#include "platform/geometry/IntRect.h"
#include "platform/heap/Handle.h"
#include "wtf/Vector.h"

namespace blink {

class Element;

class ColorChooserClient : public WillBeGarbageCollectedMixin {
public:
    virtual ~ColorChooserClient();
    DEFINE_INLINE_VIRTUAL_TRACE() { }

    virtual void didChooseColor(const Color&) = 0;
    virtual void didEndChooser() = 0;
    virtual Element& ownerElement() const = 0;
    virtual IntRect elementRectRelativeToViewport() const = 0;
    virtual Color currentColor() = 0;
    virtual bool shouldShowSuggestions() const = 0;
    virtual Vector<ColorSuggestion> suggestions() const = 0;
};

} // namespace blink

#endif // ColorChooserClient_h
