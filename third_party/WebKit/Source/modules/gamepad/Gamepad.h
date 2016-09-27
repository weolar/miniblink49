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

#ifndef Gamepad_h
#define Gamepad_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/gamepad/GamepadButton.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebGamepad.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Gamepad final : public GarbageCollectedFinalized<Gamepad>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static Gamepad* create()
    {
        return new Gamepad;
    }
    ~Gamepad();

    typedef Vector<double> DoubleVector;

    const String& id() const { return m_id; }
    void setId(const String& id) { m_id = id; }

    unsigned index() const { return m_index; }
    void setIndex(unsigned val) { m_index = val; }

    bool connected() const { return m_connected; }
    void setConnected(bool val) { m_connected = val; }

    unsigned long long timestamp() const { return m_timestamp; }
    void setTimestamp(unsigned long long val) { m_timestamp = val; }

    const String& mapping() const { return m_mapping; }
    void setMapping(const String& val) { m_mapping = val; }

    const DoubleVector& axes() const { return m_axes; }
    void setAxes(unsigned count, const double* data);

    const GamepadButtonVector& buttons() const { return m_buttons; }
    void setButtons(unsigned count, const WebGamepadButton* data);

    DECLARE_TRACE();

private:
    Gamepad();

    String m_id;
    unsigned m_index;
    bool m_connected;
    unsigned long long m_timestamp;
    String m_mapping;
    DoubleVector m_axes;
    GamepadButtonVector m_buttons;
};

} // namespace blink

#endif // Gamepad_h
