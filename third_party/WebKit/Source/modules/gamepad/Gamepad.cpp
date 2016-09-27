/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include "config.h"
#include "modules/gamepad/Gamepad.h"

namespace blink {

Gamepad::Gamepad()
    : m_index(0)
    , m_timestamp(0)
{
}

Gamepad::~Gamepad()
{
}

void Gamepad::setAxes(unsigned count, const double* data)
{
    m_axes.resize(count);
    if (count)
        std::copy(data, data + count, m_axes.begin());
}

void Gamepad::setButtons(unsigned count, const WebGamepadButton* data)
{
    if (m_buttons.size() != count) {
        m_buttons.resize(count);
        for (unsigned i = 0; i < count; ++i)
            m_buttons[i] = GamepadButton::create();
    }
    for (unsigned i = 0; i < count; ++i) {
        m_buttons[i]->setValue(data[i].value);
        m_buttons[i]->setPressed(data[i].pressed);
    }
}

DEFINE_TRACE(Gamepad)
{
    visitor->trace(m_buttons);
}

} // namespace blink
