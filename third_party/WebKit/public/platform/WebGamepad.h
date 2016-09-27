// Copyright (C) 2011, Google Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.

#ifndef WebGamepad_h
#define WebGamepad_h

#include "WebCommon.h"

#if BLINK_IMPLEMENTATION
#include "wtf/Assertions.h"
#endif

namespace blink {

#pragma pack(push, 1)

class WebGamepadButton {
public:
    WebGamepadButton()
        : pressed(false)
        , value(0.)
    {
    }
    WebGamepadButton(bool pressed, double value)
        : pressed(pressed)
        , value(value)
    {
    }
    bool pressed;
    double value;
};

// This structure is intentionally POD and fixed size so that it can be shared
// memory between hardware polling threads and the rest of the browser. See
// also WebGamepads.h.
class WebGamepad {
public:
    static const size_t idLengthCap = 128;
    static const size_t mappingLengthCap = 16;
    static const size_t axesLengthCap = 16;
    static const size_t buttonsLengthCap = 32;

    WebGamepad()
        : connected(false)
        , timestamp(0)
        , axesLength(0)
        , buttonsLength(0)
    {
        id[0] = 0;
        mapping[0] = 0;
    }

    // Is there a gamepad connected at this index?
    bool connected;

    // Device identifier (based on manufacturer, model, etc.).
    WebUChar id[idLengthCap];

    // Monotonically increasing value referring to when the data were last
    // updated.
    unsigned long long timestamp;

    // Number of valid entries in the axes array.
    unsigned axesLength;

    // Normalized values representing axes, in the range [-1..1].
    double axes[axesLengthCap];

    // Number of valid entries in the buttons array.
    unsigned buttonsLength;

    // Button states
    WebGamepadButton buttons[buttonsLengthCap];

    // Mapping type (for example "standard")
    WebUChar mapping[mappingLengthCap];
};

#if BLINK_IMPLEMENTATION
static_assert(sizeof(WebGamepad) == 721, "WebGamepad has wrong size");
#endif

#pragma pack(pop)

}

#endif // WebGamepad_h
