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

#import <Cocoa/Cocoa.h>
#include <gtest/gtest.h>

#include "core/events/KeyboardEvent.h"
#include "platform/WindowsKeyboardCodes.h"
#include "public/web/WebInputEvent.h"
#include "public/web/mac/WebInputEventFactory.h"

using blink::WebInputEventFactory;
using blink::WebKeyboardEvent;
using blink::WebInputEvent;

namespace {

struct KeyMappingEntry {
    int macKeyCode;
    unichar character;
    int windowsKeyCode;
};

struct ModifierKey {
    int macKeyCode;
    int leftOrRightMask;
    int nonSpecificMask;
};

// Modifier keys, grouped into left/right pairs.
ModifierKey modifierKeys[] = {
    { 56, 1 <<  1, NSShiftKeyMask },      // Left Shift
    { 60, 1 <<  2, NSShiftKeyMask },      // Right Shift
    { 55, 1 <<  3, NSCommandKeyMask },    // Left Command
    { 54, 1 <<  4, NSCommandKeyMask },    // Right Command
    { 58, 1 <<  5, NSAlternateKeyMask },  // Left Alt
    { 61, 1 <<  6, NSAlternateKeyMask },  // Right Alt
    { 59, 1 <<  0, NSControlKeyMask },    // Left Control
    { 62, 1 << 13, NSControlKeyMask },    // Right Control
};

NSEvent* BuildFakeKeyEvent(NSUInteger keyCode, unichar character, NSUInteger modifierFlags, NSEventType eventType)
{
    NSString* string = [NSString stringWithCharacters:&character length:1];
    return [NSEvent keyEventWithType:eventType
                            location:NSZeroPoint
                       modifierFlags:modifierFlags
                           timestamp:0.0
                        windowNumber:0
                             context:nil
                          characters:string
         charactersIgnoringModifiers:string
                           isARepeat:NO
                             keyCode:keyCode];
}

} // namespace

// Test that arrow keys don't have numpad modifier set.
TEST(WebInputEventFactoryTestMac, ArrowKeyNumPad)
{
    // Left
    NSEvent* macEvent = BuildFakeKeyEvent(0x7B, NSLeftArrowFunctionKey,
                                        NSNumericPadKeyMask, NSKeyDown);
    WebKeyboardEvent webEvent = WebInputEventFactory::keyboardEvent(macEvent);
    EXPECT_EQ(0, webEvent.modifiers);

    // Right
    macEvent = BuildFakeKeyEvent(0x7C, NSRightArrowFunctionKey,
                                 NSNumericPadKeyMask, NSKeyDown);
    webEvent = WebInputEventFactory::keyboardEvent(macEvent);
    EXPECT_EQ(0, webEvent.modifiers);

    // Down
    macEvent = BuildFakeKeyEvent(0x7D, NSDownArrowFunctionKey,
                                 NSNumericPadKeyMask, NSKeyDown);
    webEvent = WebInputEventFactory::keyboardEvent(macEvent);
    EXPECT_EQ(0, webEvent.modifiers);

    // Up
    macEvent = BuildFakeKeyEvent(0x7E, NSUpArrowFunctionKey,
                                 NSNumericPadKeyMask, NSKeyDown);
    webEvent = WebInputEventFactory::keyboardEvent(macEvent);
    EXPECT_EQ(0, webEvent.modifiers);
}

// Test that numpad keys get mapped correctly.
TEST(WebInputEventFactoryTestMac, NumPadMapping)
{
    KeyMappingEntry table[] =
    {
        {65, '.', VK_DECIMAL},
        {67, '*', VK_MULTIPLY},
        {69, '+', VK_ADD},
        {71, NSClearLineFunctionKey, VK_CLEAR},
        {75, '/', VK_DIVIDE},
        {76, 3,   VK_RETURN},
        {78, '-', VK_SUBTRACT},
        {81, '=', VK_OEM_PLUS},
        {82, '0', VK_NUMPAD0},
        {83, '1', VK_NUMPAD1},
        {84, '2', VK_NUMPAD2},
        {85, '3', VK_NUMPAD3},
        {86, '4', VK_NUMPAD4},
        {87, '5', VK_NUMPAD5},
        {88, '6', VK_NUMPAD6},
        {89, '7', VK_NUMPAD7},
        {91, '8', VK_NUMPAD8},
        {92, '9', VK_NUMPAD9},
    };

    for (size_t i = 0; i < arraysize(table); ++i) {
        NSEvent* macEvent = BuildFakeKeyEvent(table[i].macKeyCode,
                                              table[i].character, 0, NSKeyDown);
        WebKeyboardEvent webEvent = WebInputEventFactory::keyboardEvent(macEvent);
        EXPECT_EQ(table[i].windowsKeyCode, webEvent.windowsKeyCode);
    }
}

// Test that left- and right-hand modifier keys are interpreted correctly when
// pressed simultaneously.
TEST(WebInputEventFactoryTestMac, SimultaneousModifierKeys)
{
    for (size_t i = 0; i < arraysize(modifierKeys) / 2; ++i) {
        const ModifierKey& left = modifierKeys[2 * i];
        const ModifierKey& right = modifierKeys[2 * i + 1];
        // Press the left key.
        NSEvent* macEvent = BuildFakeKeyEvent(
            left.macKeyCode, 0, left.leftOrRightMask | left.nonSpecificMask,
            NSFlagsChanged);
        WebKeyboardEvent webEvent = WebInputEventFactory::keyboardEvent(macEvent);
        EXPECT_EQ(WebInputEvent::RawKeyDown, webEvent.type);
        // Press the right key
        macEvent = BuildFakeKeyEvent(
            right.macKeyCode, 0,
            left.leftOrRightMask | right.leftOrRightMask | left.nonSpecificMask,
            NSFlagsChanged);
        webEvent = WebInputEventFactory::keyboardEvent(macEvent);
        EXPECT_EQ(WebInputEvent::RawKeyDown, webEvent.type);
        // Release the right key
        macEvent = BuildFakeKeyEvent(
            right.macKeyCode, 0, left.leftOrRightMask | left.nonSpecificMask,
            NSFlagsChanged);
        // Release the left key
        macEvent = BuildFakeKeyEvent(left.macKeyCode, 0, 0,NSFlagsChanged);
        webEvent = WebInputEventFactory::keyboardEvent(macEvent);
        EXPECT_EQ(WebInputEvent::KeyUp, webEvent.type);
    }
}

// Test that individual modifier keys are still reported correctly, even if the
// undocumented left- or right-hand flags are not set.
TEST(WebInputEventFactoryTestMac, MissingUndocumentedModifierFlags)
{
    for (size_t i = 0; i < arraysize(modifierKeys); ++i) {
        const ModifierKey& key = modifierKeys[i];
        NSEvent* macEvent = BuildFakeKeyEvent(
            key.macKeyCode, 0, key.nonSpecificMask, NSFlagsChanged);
        WebKeyboardEvent webEvent = WebInputEventFactory::keyboardEvent(macEvent);
        EXPECT_EQ(WebInputEvent::RawKeyDown, webEvent.type);
        macEvent = BuildFakeKeyEvent(key.macKeyCode, 0, 0, NSFlagsChanged);
        webEvent = WebInputEventFactory::keyboardEvent(macEvent);
        EXPECT_EQ(WebInputEvent::KeyUp, webEvent.type);
    }
}
