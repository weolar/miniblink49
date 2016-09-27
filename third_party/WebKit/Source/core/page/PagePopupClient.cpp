/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "core/page/PagePopupClient.h"

#include "wtf/text/CharacterNames.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

#define addLiteral(literal, data)    data->append(literal, sizeof(literal) - 1)

void PagePopupClient::addJavaScriptString(const String& str, SharedBuffer* data)
{
    addLiteral("\"", data);
    StringBuilder builder;
    builder.reserveCapacity(str.length());
    for (unsigned i = 0; i < str.length(); ++i) {
        if (str[i] == '\r') {
            builder.append("\\r");
        } else if (str[i] == '\n') {
            builder.append("\\n");
        } else if (str[i] == '\\' || str[i] == '"') {
            builder.append('\\');
            builder.append(str[i]);
        } else if (str[i] == '<') {
            // Need to avoid to add "</script>" because the resultant string is
            // typically embedded in <script>.
            builder.append("\\x3C");
        } else if (str[i] < 0x20 || str[i] == lineSeparator || str[i] == paragraphSeparator) {
            builder.append(String::format("\\u%04X", str[i]));
        } else {
            builder.append(str[i]);
        }
    }
    addString(builder.toString(), data);
    addLiteral("\"", data);
}

void PagePopupClient::addProperty(const char* name, const String& value, SharedBuffer* data)
{
    data->append(name, strlen(name));
    addLiteral(": ", data);
    addJavaScriptString(value, data);
    addLiteral(",\n", data);
}

void PagePopupClient::addProperty(const char* name, int value, SharedBuffer* data)
{
    data->append(name, strlen(name));
    addLiteral(": ", data);
    addString(String::number(value), data);
    addLiteral(",\n", data);
}

void PagePopupClient::addProperty(const char* name, unsigned value, SharedBuffer* data)
{
    data->append(name, strlen(name));
    addLiteral(": ", data);
    addString(String::number(value), data);
    addLiteral(",\n", data);
}

void PagePopupClient::addProperty(const char* name, bool value, SharedBuffer* data)
{
    data->append(name, strlen(name));
    addLiteral(": ", data);
    if (value)
        addLiteral("true", data);
    else
        addLiteral("false", data);
    addLiteral(",\n", data);
}

void PagePopupClient::addProperty(const char* name, double value, SharedBuffer* data)
{
    data->append(name, strlen(name));
    addLiteral(": ", data);
    addString(String::number(value), data);
    addLiteral(",\n", data);
}

void PagePopupClient::addProperty(const char* name, const Vector<String>& values, SharedBuffer* data)
{
    data->append(name, strlen(name));
    addLiteral(": [", data);
    for (unsigned i = 0; i < values.size(); ++i) {
        if (i)
            addLiteral(",", data);
        addJavaScriptString(values[i], data);
    }
    addLiteral("],\n", data);
}

void PagePopupClient::addProperty(const char* name, const IntRect& rect, SharedBuffer* data)
{
    data->append(name, strlen(name));
    addLiteral(": {", data);
    addProperty("x", rect.x(), data);
    addProperty("y", rect.y(), data);
    addProperty("width", rect.width(), data);
    addProperty("height", rect.height(), data);
    addLiteral("},\n", data);
}

} // namespace blink

