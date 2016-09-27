/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"

#include "modules/websockets/WebSocketFrame.h"

namespace blink {

WebSocketFrame::WebSocketFrame(OpCode opCode, const char* payload, size_t payloadLength, Flags flags)
    : opCode(opCode)
    , final(flags & Final)
    , compress(flags & Compress)
    , reserved2(flags & Reserved2)
    , reserved3(flags & Reserved3)
    , masked(flags & Masked)
    , payload(payload)
    , payloadLength(payloadLength)
{
}

} // namespace blink
