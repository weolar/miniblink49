/*
 * Copyright (C) 2004, 2006, 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2006-2009 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "public/web/mac/WebInputEventFactory.h"

#include <ApplicationServices/ApplicationServices.h>
#import <AvailabilityMacros.h>
#import <Cocoa/Cocoa.h>

#include "platform/WindowsKeyboardCodes.h"
#include "public/web/WebInputEvent.h"
#include "wtf/ASCIICType.h"

#if __MAC_OS_X_VERSION_MAX_ALLOWED == 1060

// Additional Lion APIs.
enum {
    NSEventPhaseNone        = 0,
    NSEventPhaseBegan       = 0x1 << 0,
    NSEventPhaseStationary  = 0x1 << 1,
    NSEventPhaseChanged     = 0x1 << 2,
    NSEventPhaseEnded       = 0x1 << 3,
    NSEventPhaseCancelled   = 0x1 << 4
};
typedef NSUInteger NSEventPhase;

#endif  // __MAC_OS_X_VERSION_MAX_ALLOWED == 1060

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1080

// Additional Mountain Lion APIs.
enum {
    NSEventPhaseMayBegin    = 0x1 << 5
};

enum {
    NSEventTypeSmartMagnify = 32
};

#endif  // __MAC_OS_X_VERSION_MAX_ALLOWED < 1080

// Redeclare methods only available on OSX 10.7+ to suppress -Wpartial-availability warnings.

#if !defined(MAC_OS_X_VERSION_10_7) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_7

@interface NSEvent (LionSDKDeclarations)
- (NSEventPhase)phase;
- (NSEventPhase)momentumPhase;
@end

#endif  // !defined(MAC_OS_X_VERSION_10_7) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_7

namespace blink {

static int windowsKeyCodeForKeyCode(uint16_t keyCode)
{
    static const int windowsKeyCode[] = {
        /* 0 */ VK_A,
        /* 1 */ VK_S,
        /* 2 */ VK_D,
        /* 3 */ VK_F,
        /* 4 */ VK_H,
        /* 5 */ VK_G,
        /* 6 */ VK_Z,
        /* 7 */ VK_X,
        /* 8 */ VK_C,
        /* 9 */ VK_V,
        /* 0x0A */ VK_OEM_3, // "Section" - key to the left from 1 (ISO Keyboard Only)
        /* 0x0B */ VK_B,
        /* 0x0C */ VK_Q,
        /* 0x0D */ VK_W,
        /* 0x0E */ VK_E,
        /* 0x0F */ VK_R,
        /* 0x10 */ VK_Y,
        /* 0x11 */ VK_T,
        /* 0x12 */ VK_1,
        /* 0x13 */ VK_2,
        /* 0x14 */ VK_3,
        /* 0x15 */ VK_4,
        /* 0x16 */ VK_6,
        /* 0x17 */ VK_5,
        /* 0x18 */ VK_OEM_PLUS, // =+
        /* 0x19 */ VK_9,
        /* 0x1A */ VK_7,
        /* 0x1B */ VK_OEM_MINUS, // -_
        /* 0x1C */ VK_8,
        /* 0x1D */ VK_0,
        /* 0x1E */ VK_OEM_6, // ]}
        /* 0x1F */ VK_O,
        /* 0x20 */ VK_U,
        /* 0x21 */ VK_OEM_4, // {[
        /* 0x22 */ VK_I,
        /* 0x23 */ VK_P,
        /* 0x24 */ VK_RETURN, // Return
        /* 0x25 */ VK_L,
        /* 0x26 */ VK_J,
        /* 0x27 */ VK_OEM_7, // '"
        /* 0x28 */ VK_K,
        /* 0x29 */ VK_OEM_1, // ;:
        /* 0x2A */ VK_OEM_5, // \|
        /* 0x2B */ VK_OEM_COMMA, // ,<
        /* 0x2C */ VK_OEM_2, // /?
        /* 0x2D */ VK_N,
        /* 0x2E */ VK_M,
        /* 0x2F */ VK_OEM_PERIOD, // .>
        /* 0x30 */ VK_TAB,
        /* 0x31 */ VK_SPACE,
        /* 0x32 */ VK_OEM_3, // `~
        /* 0x33 */ VK_BACK, // Backspace
        /* 0x34 */ 0, // n/a
        /* 0x35 */ VK_ESCAPE,
        /* 0x36 */ VK_APPS, // Right Command
        /* 0x37 */ VK_LWIN, // Left Command
        /* 0x38 */ VK_LSHIFT, // Left Shift
        /* 0x39 */ VK_CAPITAL, // Caps Lock
        /* 0x3A */ VK_LMENU, // Left Option
        /* 0x3B */ VK_LCONTROL, // Left Ctrl
        /* 0x3C */ VK_RSHIFT, // Right Shift
        /* 0x3D */ VK_RMENU, // Right Option
        /* 0x3E */ VK_RCONTROL, // Right Ctrl
        /* 0x3F */ 0, // fn
        /* 0x40 */ VK_F17,
        /* 0x41 */ VK_DECIMAL, // Num Pad .
        /* 0x42 */ 0, // n/a
        /* 0x43 */ VK_MULTIPLY, // Num Pad *
        /* 0x44 */ 0, // n/a
        /* 0x45 */ VK_ADD, // Num Pad +
        /* 0x46 */ 0, // n/a
        /* 0x47 */ VK_CLEAR, // Num Pad Clear
        /* 0x48 */ VK_VOLUME_UP,
        /* 0x49 */ VK_VOLUME_DOWN,
        /* 0x4A */ VK_VOLUME_MUTE,
        /* 0x4B */ VK_DIVIDE, // Num Pad /
        /* 0x4C */ VK_RETURN, // Num Pad Enter
        /* 0x4D */ 0, // n/a
        /* 0x4E */ VK_SUBTRACT, // Num Pad -
        /* 0x4F */ VK_F18,
        /* 0x50 */ VK_F19,
        /* 0x51 */ VK_OEM_PLUS, // Num Pad =. There is no such key on common PC keyboards, mapping to normal "+=".
        /* 0x52 */ VK_NUMPAD0,
        /* 0x53 */ VK_NUMPAD1,
        /* 0x54 */ VK_NUMPAD2,
        /* 0x55 */ VK_NUMPAD3,
        /* 0x56 */ VK_NUMPAD4,
        /* 0x57 */ VK_NUMPAD5,
        /* 0x58 */ VK_NUMPAD6,
        /* 0x59 */ VK_NUMPAD7,
        /* 0x5A */ VK_F20,
        /* 0x5B */ VK_NUMPAD8,
        /* 0x5C */ VK_NUMPAD9,
        /* 0x5D */ 0, // Yen (JIS Keyboard Only)
        /* 0x5E */ 0, // Underscore (JIS Keyboard Only)
        /* 0x5F */ 0, // KeypadComma (JIS Keyboard Only)
        /* 0x60 */ VK_F5,
        /* 0x61 */ VK_F6,
        /* 0x62 */ VK_F7,
        /* 0x63 */ VK_F3,
        /* 0x64 */ VK_F8,
        /* 0x65 */ VK_F9,
        /* 0x66 */ 0, // Eisu (JIS Keyboard Only)
        /* 0x67 */ VK_F11,
        /* 0x68 */ 0, // Kana (JIS Keyboard Only)
        /* 0x69 */ VK_F13,
        /* 0x6A */ VK_F16,
        /* 0x6B */ VK_F14,
        /* 0x6C */ 0, // n/a
        /* 0x6D */ VK_F10,
        /* 0x6E */ 0, // n/a (Windows95 key?)
        /* 0x6F */ VK_F12,
        /* 0x70 */ 0, // n/a
        /* 0x71 */ VK_F15,
        /* 0x72 */ VK_INSERT, // Help
        /* 0x73 */ VK_HOME, // Home
        /* 0x74 */ VK_PRIOR, // Page Up
        /* 0x75 */ VK_DELETE, // Forward Delete
        /* 0x76 */ VK_F4,
        /* 0x77 */ VK_END, // End
        /* 0x78 */ VK_F2,
        /* 0x79 */ VK_NEXT, // Page Down
        /* 0x7A */ VK_F1,
        /* 0x7B */ VK_LEFT, // Left Arrow
        /* 0x7C */ VK_RIGHT, // Right Arrow
        /* 0x7D */ VK_DOWN, // Down Arrow
        /* 0x7E */ VK_UP, // Up Arrow
        /* 0x7F */ 0 // n/a
    };

    if (keyCode >= 0x80)
        return 0;

     return windowsKeyCode[keyCode];
}

static int windowsKeyCodeForCharCode(unichar charCode)
{
    switch (charCode) {

        case 'a': case 'A': return VK_A;
        case 'b': case 'B': return VK_B;
        case 'c': case 'C': return VK_C;
        case 'd': case 'D': return VK_D;
        case 'e': case 'E': return VK_E;
        case 'f': case 'F': return VK_F;
        case 'g': case 'G': return VK_G;
        case 'h': case 'H': return VK_H;
        case 'i': case 'I': return VK_I;
        case 'j': case 'J': return VK_J;
        case 'k': case 'K': return VK_K;
        case 'l': case 'L': return VK_L;
        case 'm': case 'M': return VK_M;
        case 'n': case 'N': return VK_N;
        case 'o': case 'O': return VK_O;
        case 'p': case 'P': return VK_P;
        case 'q': case 'Q': return VK_Q;
        case 'r': case 'R': return VK_R;
        case 's': case 'S': return VK_S;
        case 't': case 'T': return VK_T;
        case 'u': case 'U': return VK_U;
        case 'v': case 'V': return VK_V;
        case 'w': case 'W': return VK_W;
        case 'x': case 'X': return VK_X;
        case 'y': case 'Y': return VK_Y;
        case 'z': case 'Z': return VK_Z;

        // AppKit generates Unicode PUA character codes for some function keys; using these when key code is not known.
        case NSPauseFunctionKey: return VK_PAUSE;
        case NSSelectFunctionKey: return VK_SELECT;
        case NSPrintFunctionKey: return VK_PRINT;
        case NSExecuteFunctionKey: return VK_EXECUTE;
        case NSPrintScreenFunctionKey: return VK_SNAPSHOT;
        case NSInsertFunctionKey: return VK_INSERT;

        case NSF21FunctionKey: return VK_F21;
        case NSF22FunctionKey: return VK_F22;
        case NSF23FunctionKey: return VK_F23;
        case NSF24FunctionKey: return VK_F24;
        case NSScrollLockFunctionKey: return VK_SCROLL;

        // This is for U.S. keyboard mapping, and doesn't necessarily make sense for different keyboard layouts.
        // For example, '"' on Windows Russian layout is VK_2, not VK_OEM_7.
        case ';': case ':': return VK_OEM_1;
        case '=': case '+': return VK_OEM_PLUS;
        case ',': case '<': return VK_OEM_COMMA;
        case '-': case '_': return VK_OEM_MINUS;
        case '.': case '>': return VK_OEM_PERIOD;
        case '/': case '?': return VK_OEM_2;
        case '`': case '~': return VK_OEM_3;
        case '[': case '{': return VK_OEM_4;
        case '\\': case '|': return VK_OEM_5;
        case ']': case '}': return VK_OEM_6;
        case '\'': case '"': return VK_OEM_7;

    }

    return 0;
}

// Return true if the target modifier key is up. OS X has an "official" flag
// to test whether either left or right versions of a modifier key are held,
// and "unofficial" flags for the left and right versions independently. This
// function verifies that |targetKeyMask| and |otherKeyMask| (which should be
// the left and right versions of a modifier) are consistent with with the
// state of |eitherKeyMask| (which should be the corresponding ""official"
// flag). If they are consistent, it tests |targetKeyMask|; otherwise it tests
// |eitherKeyMask|.
static inline bool isModifierKeyUp(
    unsigned int flags, unsigned int targetKeyMask, unsigned int otherKeyMask,
    unsigned int eitherKeyMask)
{
  bool eitherKeyDown = (flags & eitherKeyMask) != 0;
  bool targetKeyDown = (flags & targetKeyMask) != 0;
  bool otherKeyDown = (flags & otherKeyMask) != 0;
  if (eitherKeyDown != (targetKeyDown || otherKeyDown))
    return !eitherKeyDown;
  return !targetKeyDown;
}

static inline bool isKeyUpEvent(NSEvent* event)
{
    if ([event type] != NSFlagsChanged)
        return [event type] == NSKeyUp;

    // Unofficial bit-masks for left- and right-hand versions of modifier keys.
    // These values were determined empirically.
    const unsigned int kLeftControlKeyMask = 1 << 0;
    const unsigned int kLeftShiftKeyMask = 1 << 1;
    const unsigned int kRightShiftKeyMask = 1 << 2;
    const unsigned int kLeftCommandKeyMask = 1 << 3;
    const unsigned int kRightCommandKeyMask = 1 << 4;
    const unsigned int kLeftAlternateKeyMask = 1 << 5;
    const unsigned int kRightAlternateKeyMask = 1 << 6;
    const unsigned int kRightControlKeyMask = 1 << 13;

    switch ([event keyCode]) {
    case 54: // Right Command
        return isModifierKeyUp([event modifierFlags], kRightCommandKeyMask,
                               kLeftCommandKeyMask, NSCommandKeyMask);
    case 55: // Left Command
        return isModifierKeyUp([event modifierFlags], kLeftCommandKeyMask,
                               kRightCommandKeyMask, NSCommandKeyMask);

    case 57: // Capslock
        return ([event modifierFlags] & NSAlphaShiftKeyMask) == 0;

    case 56: // Left Shift
        return isModifierKeyUp([event modifierFlags], kLeftShiftKeyMask,
                               kRightShiftKeyMask, NSShiftKeyMask);
    case 60: // Right Shift
        return isModifierKeyUp([event modifierFlags],kRightShiftKeyMask,
                               kLeftShiftKeyMask, NSShiftKeyMask);

    case 58: // Left Alt
        return isModifierKeyUp([event modifierFlags], kLeftAlternateKeyMask,
                               kRightAlternateKeyMask, NSAlternateKeyMask);
    case 61: // Right Alt
        return isModifierKeyUp([event modifierFlags], kRightAlternateKeyMask,
                               kLeftAlternateKeyMask, NSAlternateKeyMask);

    case 59: // Left Ctrl
        return isModifierKeyUp([event modifierFlags], kLeftControlKeyMask,
                               kRightControlKeyMask, NSControlKeyMask);
    case 62: // Right Ctrl
        return isModifierKeyUp([event modifierFlags], kRightControlKeyMask,
                               kLeftControlKeyMask, NSControlKeyMask);

    case 63: // Function
        return ([event modifierFlags] & NSFunctionKeyMask) == 0;
    }
    return false;
}

static bool isKeypadEvent(NSEvent* event)
{
    // Check that this is the type of event that has a keyCode.
    switch ([event type]) {
    case NSKeyDown:
    case NSKeyUp:
    case NSFlagsChanged:
        break;
    default:
        return false;
    }

    switch ([event keyCode]) {
    case 71: // Clear
    case 81: // =
    case 75: // /
    case 67: // *
    case 78: // -
    case 69: // +
    case 76: // Enter
    case 65: // .
    case 82: // 0
    case 83: // 1
    case 84: // 2
    case 85: // 3
    case 86: // 4
    case 87: // 5
    case 88: // 6
    case 89: // 7
    case 91: // 8
    case 92: // 9
        return true;
    }

    return false;
}

static int windowsKeyCodeForKeyEvent(NSEvent* event)
{
    int code = 0;
    // There are several kinds of characters for which we produce key code from char code:
    // 1. Roman letters. Windows keyboard layouts affect both virtual key codes and character codes for these,
    //    so e.g. 'A' gets the same keyCode on QWERTY, AZERTY or Dvorak layouts.
    // 2. Keys for which there is no known Mac virtual key codes, like PrintScreen.
    // 3. Certain punctuation keys. On Windows, these are also remapped depending on current keyboard layout,
    //    but see comment in windowsKeyCodeForCharCode().
    if (!isKeypadEvent(event) && ([event type] == NSKeyDown || [event type] == NSKeyUp)) {
        // Cmd switches Roman letters for Dvorak-QWERTY layout, so try modified characters first.
        NSString* s = [event characters];
        code = [s length] > 0 ? windowsKeyCodeForCharCode([s characterAtIndex:0]) : 0;
        if (code)
            return code;

        // Ctrl+A on an AZERTY keyboard would get VK_Q keyCode if we relied on -[NSEvent keyCode] below.
        s = [event charactersIgnoringModifiers];
        code = [s length] > 0 ? windowsKeyCodeForCharCode([s characterAtIndex:0]) : 0;
        if (code)
            return code;
    }

    // Map Mac virtual key code directly to Windows one for any keys not handled above.
    // E.g. the key next to Caps Lock has the same Event.keyCode on U.S. keyboard ('A') and on Russian keyboard (CYRILLIC LETTER EF).
    return windowsKeyCodeForKeyCode([event keyCode]);
}

static inline NSString* textFromEvent(NSEvent* event)
{
    if ([event type] == NSFlagsChanged)
        return @"";
    return [event characters];
}

static inline NSString* unmodifiedTextFromEvent(NSEvent* event)
{
    if ([event type] == NSFlagsChanged)
        return @"";
    return [event charactersIgnoringModifiers];
}

static NSString* keyIdentifierForKeyEvent(NSEvent* event)
{
    if ([event type] == NSFlagsChanged) {
        switch ([event keyCode]) {
        case 54: // Right Command
        case 55: // Left Command
            return @"Meta";

        case 57: // Capslock
            return @"CapsLock";

        case 56: // Left Shift
        case 60: // Right Shift
            return @"Shift";

        case 58: // Left Alt
        case 61: // Right Alt
            return @"Alt";

        case 59: // Left Ctrl
        case 62: // Right Ctrl
            return @"Control";

// Begin non-Apple addition/modification --------------------------------------
        case 63: // Function
            return @"Function";

        default: // Unknown, but this may be a strange/new keyboard.
            return @"Unidentified";
// End non-Apple addition/modification ----------------------------------------
        }
    }

    NSString* s = [event charactersIgnoringModifiers];
    if ([s length] != 1)
        return @"Unidentified";

    unichar c = [s characterAtIndex:0];
    switch (c) {
    // Each identifier listed in the DOM spec is listed here.
    // Many are simply commented out since they do not appear on standard Macintosh keyboards
    // or are on a key that doesn't have a corresponding character.

    // "Accept"
    // "AllCandidates"

    // "Alt"
    case NSMenuFunctionKey:
        return @"Alt";

    // "Apps"
    // "BrowserBack"
    // "BrowserForward"
    // "BrowserHome"
    // "BrowserRefresh"
    // "BrowserSearch"
    // "BrowserStop"
    // "CapsLock"

    // "Clear"
    case NSClearLineFunctionKey:
        return @"Clear";

    // "CodeInput"
    // "Compose"
    // "Control"
    // "Crsel"
    // "Convert"
    // "Copy"
    // "Cut"

    // "Down"
    case NSDownArrowFunctionKey:
        return @"Down";
    // "End"
    case NSEndFunctionKey:
        return @"End";
    // "Enter"
    case 0x3: case 0xA: case 0xD: // Macintosh calls the one on the main keyboard Return, but Windows calls it Enter, so we'll do the same for the DOM
        return @"Enter";

    // "EraseEof"

    // "Execute"
    case NSExecuteFunctionKey:
        return @"Execute";

    // "Exsel"

    // "F1"
    case NSF1FunctionKey:
        return @"F1";
    // "F2"
    case NSF2FunctionKey:
        return @"F2";
    // "F3"
    case NSF3FunctionKey:
        return @"F3";
    // "F4"
    case NSF4FunctionKey:
        return @"F4";
    // "F5"
    case NSF5FunctionKey:
        return @"F5";
    // "F6"
    case NSF6FunctionKey:
        return @"F6";
    // "F7"
    case NSF7FunctionKey:
        return @"F7";
    // "F8"
    case NSF8FunctionKey:
        return @"F8";
    // "F9"
    case NSF9FunctionKey:
        return @"F9";
    // "F10"
    case NSF10FunctionKey:
        return @"F10";
    // "F11"
    case NSF11FunctionKey:
        return @"F11";
    // "F12"
    case NSF12FunctionKey:
        return @"F12";
    // "F13"
    case NSF13FunctionKey:
        return @"F13";
    // "F14"
    case NSF14FunctionKey:
        return @"F14";
    // "F15"
    case NSF15FunctionKey:
        return @"F15";
    // "F16"
    case NSF16FunctionKey:
        return @"F16";
    // "F17"
    case NSF17FunctionKey:
        return @"F17";
    // "F18"
    case NSF18FunctionKey:
        return @"F18";
    // "F19"
    case NSF19FunctionKey:
        return @"F19";
    // "F20"
    case NSF20FunctionKey:
        return @"F20";
    // "F21"
    case NSF21FunctionKey:
        return @"F21";
    // "F22"
    case NSF22FunctionKey:
        return @"F22";
    // "F23"
    case NSF23FunctionKey:
        return @"F23";
    // "F24"
    case NSF24FunctionKey:
        return @"F24";

    // "FinalMode"

    // "Find"
    case NSFindFunctionKey:
        return @"Find";

    // "FullWidth"
    // "HalfWidth"
    // "HangulMode"
    // "HanjaMode"

    // "Help"
    case NSHelpFunctionKey:
        return @"Help";

    // "Hiragana"

    // "Home"
    case NSHomeFunctionKey:
        return @"Home";
    // "Insert"
    case NSInsertFunctionKey:
        return @"Insert";

    // "JapaneseHiragana"
    // "JapaneseKatakana"
    // "JapaneseRomaji"
    // "JunjaMode"
    // "KanaMode"
    // "KanjiMode"
    // "Katakana"
    // "LaunchApplication1"
    // "LaunchApplication2"
    // "LaunchMail"

    // "Left"
    case NSLeftArrowFunctionKey:
        return @"Left";

    // "Meta"
    // "MediaNextTrack"
    // "MediaPlayPause"
    // "MediaPreviousTrack"
    // "MediaStop"

    // "ModeChange"
    case NSModeSwitchFunctionKey:
        return @"ModeChange";

    // "Nonconvert"
    // "NumLock"

    // "PageDown"
    case NSPageDownFunctionKey:
        return @"PageDown";
    // "PageUp"
    case NSPageUpFunctionKey:
        return @"PageUp";

    // "Paste"

    // "Pause"
    case NSPauseFunctionKey:
        return @"Pause";

    // "Play"
    // "PreviousCandidate"

    // "PrintScreen"
    case NSPrintScreenFunctionKey:
        return @"PrintScreen";

    // "Process"
    // "Props"

    // "Right"
    case NSRightArrowFunctionKey:
        return @"Right";

    // "RomanCharacters"

    // "Scroll"
    case NSScrollLockFunctionKey:
        return @"Scroll";
    // "Select"
    case NSSelectFunctionKey:
        return @"Select";

    // "SelectMedia"
    // "Shift"

    // "Stop"
    case NSStopFunctionKey:
        return @"Stop";
    // "Up"
    case NSUpArrowFunctionKey:
        return @"Up";
    // "Undo"
    case NSUndoFunctionKey:
        return @"Undo";

    // "VolumeDown"
    // "VolumeMute"
    // "VolumeUp"
    // "Win"
    // "Zoom"

    // More function keys, not in the key identifier specification.
    case NSF25FunctionKey:
        return @"F25";
    case NSF26FunctionKey:
        return @"F26";
    case NSF27FunctionKey:
        return @"F27";
    case NSF28FunctionKey:
        return @"F28";
    case NSF29FunctionKey:
        return @"F29";
    case NSF30FunctionKey:
        return @"F30";
    case NSF31FunctionKey:
        return @"F31";
    case NSF32FunctionKey:
        return @"F32";
    case NSF33FunctionKey:
        return @"F33";
    case NSF34FunctionKey:
        return @"F34";
    case NSF35FunctionKey:
        return @"F35";

    // Turn 0x7F into 0x08, because backspace needs to always be 0x08.
    case 0x7F:
        return @"U+0008";
    // Standard says that DEL becomes U+007F.
    case NSDeleteFunctionKey:
        return @"U+007F";

    // Always use 0x09 for tab instead of AppKit's backtab character.
    case NSBackTabCharacter:
        return @"U+0009";

    case NSBeginFunctionKey:
    case NSBreakFunctionKey:
    case NSClearDisplayFunctionKey:
    case NSDeleteCharFunctionKey:
    case NSDeleteLineFunctionKey:
    case NSInsertCharFunctionKey:
    case NSInsertLineFunctionKey:
    case NSNextFunctionKey:
    case NSPrevFunctionKey:
    case NSPrintFunctionKey:
    case NSRedoFunctionKey:
    case NSResetFunctionKey:
    case NSSysReqFunctionKey:
    case NSSystemFunctionKey:
    case NSUserFunctionKey:
        // FIXME: We should use something other than the vendor-area Unicode values for the above keys.
        // For now, just fall through to the default.
    default:
        return [NSString stringWithFormat:@"U+%04X", WTF::toASCIIUpper(c)];
    }
}

// End Apple code.
// ----------------------------------------------------------------------------

static inline int modifiersFromEvent(NSEvent* event) {
    int modifiers = 0;

    if ([event modifierFlags] & NSControlKeyMask)
        modifiers |= WebInputEvent::ControlKey;
    if ([event modifierFlags] & NSShiftKeyMask)
        modifiers |= WebInputEvent::ShiftKey;
    if ([event modifierFlags] & NSAlternateKeyMask)
        modifiers |= WebInputEvent::AltKey;
    if ([event modifierFlags] & NSCommandKeyMask)
        modifiers |= WebInputEvent::MetaKey;
    if ([event modifierFlags] & NSAlphaShiftKeyMask)
        modifiers |= WebInputEvent::CapsLockOn;

    // The return value of 1 << 0 corresponds to the left mouse button,
    // 1 << 1 corresponds to the right mouse button,
    // 1 << n, n >= 2 correspond to other mouse buttons.
    NSUInteger pressedButtons = [NSEvent pressedMouseButtons];

    if (pressedButtons & (1 << 0))
        modifiers |= WebInputEvent::LeftButtonDown;
    if (pressedButtons & (1 << 1))
        modifiers |= WebInputEvent::RightButtonDown;
    if (pressedButtons & (1 << 2))
        modifiers |= WebInputEvent::MiddleButtonDown;

    return modifiers;
}

static inline void setWebEventLocationFromEventInView(WebMouseEvent* result,
                                                      NSEvent* event,
                                                      NSView* view) {
    NSPoint windowLocal = [event locationInWindow];

    NSPoint screenLocal = [[view window] convertBaseToScreen:windowLocal];
    result->globalX = screenLocal.x;
    // Flip y.
    NSScreen* primaryScreen = ([[NSScreen screens] count] > 0) ?
        [[NSScreen screens] objectAtIndex:0] : nil;
    if (primaryScreen)
        result->globalY = [primaryScreen frame].size.height - screenLocal.y;
    else
        result->globalY = screenLocal.y;

    NSPoint contentLocal = [view convertPoint:windowLocal fromView:nil];
    result->x = contentLocal.x;
    result->y = [view frame].size.height - contentLocal.y;  // Flip y.

    result->windowX = result->x;
    result->windowY = result->y;

    result->movementX = [event deltaX];
    result->movementY = [event deltaY];
}

bool WebInputEventFactory::isSystemKeyEvent(const WebKeyboardEvent& event)
{
    // Windows and Linux set |isSystemKey| if alt is down. Blink looks at this
    // flag to decide if it should handle a key or not. E.g. alt-left/right
    // shouldn't be used by Blink to scroll the current page, because we want
    // to get that key back for it to do history navigation. Hence, the
    // corresponding situation on OS X is to set this for cmd key presses.
    // cmd-b and and cmd-i are system wide key bindings that OS X doesn't
    // handle for us, so the editor handles them.
    return event.modifiers & WebInputEvent::MetaKey
           && event.windowsKeyCode != VK_B
           && event.windowsKeyCode != VK_I;
}

WebKeyboardEvent WebInputEventFactory::keyboardEvent(NSEvent* event)
{
    WebKeyboardEvent result;

    result.type =
        isKeyUpEvent(event) ? WebInputEvent::KeyUp : WebInputEvent::RawKeyDown;

    result.modifiers = modifiersFromEvent(event);

    if (isKeypadEvent(event))
        result.modifiers |= WebInputEvent::IsKeyPad;

    if (([event type] != NSFlagsChanged) && [event isARepeat])
        result.modifiers |= WebInputEvent::IsAutoRepeat;

    int windowsKeyCode = windowsKeyCodeForKeyEvent(event);
    result.windowsKeyCode = WebKeyboardEvent::windowsKeyCodeWithoutLocation(windowsKeyCode);
    result.modifiers |= WebKeyboardEvent::locationModifiersFromWindowsKeyCode(windowsKeyCode);
    result.nativeKeyCode = [event keyCode];
    NSString* textStr = textFromEvent(event);
    NSString* unmodifiedStr = unmodifiedTextFromEvent(event);
    NSString* identifierStr = keyIdentifierForKeyEvent(event);

    // Begin Apple code, copied from KeyEventMac.mm

    // Always use 13 for Enter/Return -- we don't want to use AppKit's
    // different character for Enter.
    if (result.windowsKeyCode == '\r') {
        textStr = @"\r";
        unmodifiedStr = @"\r";
    }

    // The adjustments below are only needed in backward compatibility mode,
    // but we cannot tell what mode we are in from here.

    // Turn 0x7F into 8, because backspace needs to always be 8.
    if ([textStr isEqualToString:@"\x7F"])
        textStr = @"\x8";
    if ([unmodifiedStr isEqualToString:@"\x7F"])
        unmodifiedStr = @"\x8";
    // Always use 9 for tab -- we don't want to use AppKit's different character
    // for shift-tab.
    if (result.windowsKeyCode == 9) {
        textStr = @"\x9";
        unmodifiedStr = @"\x9";
    }

    // End Apple code.

    if ([textStr length] < WebKeyboardEvent::textLengthCap &&
        [unmodifiedStr length] < WebKeyboardEvent::textLengthCap) {
        [textStr getCharacters:&result.text[0]];
        [unmodifiedStr getCharacters:&result.unmodifiedText[0]];
    } else
        ASSERT_NOT_REACHED();

    [identifierStr getCString:&result.keyIdentifier[0]
                    maxLength:sizeof(result.keyIdentifier)
                     encoding:NSASCIIStringEncoding];

    result.timeStampSeconds = [event timestamp];
    result.isSystemKey = isSystemKeyEvent(result);

    return result;
}

// WebMouseEvent --------------------------------------------------------------

WebMouseEvent WebInputEventFactory::mouseEvent(NSEvent* event, NSView* view)
{
    WebMouseEvent result;

    result.clickCount = 0;

    switch ([event type]) {
    case NSMouseExited:
        result.type = WebInputEvent::MouseLeave;
        result.button = WebMouseEvent::ButtonNone;
        break;
    case NSLeftMouseDown:
        result.type = WebInputEvent::MouseDown;
        result.clickCount = [event clickCount];
        result.button = WebMouseEvent::ButtonLeft;
        break;
    case NSOtherMouseDown:
        result.type = WebInputEvent::MouseDown;
        result.clickCount = [event clickCount];
        result.button = WebMouseEvent::ButtonMiddle;
        break;
    case NSRightMouseDown:
        result.type = WebInputEvent::MouseDown;
        result.clickCount = [event clickCount];
        result.button = WebMouseEvent::ButtonRight;
        break;
    case NSLeftMouseUp:
        result.type = WebInputEvent::MouseUp;
        result.clickCount = [event clickCount];
        result.button = WebMouseEvent::ButtonLeft;
        break;
    case NSOtherMouseUp:
        result.type = WebInputEvent::MouseUp;
        result.clickCount = [event clickCount];
        result.button = WebMouseEvent::ButtonMiddle;
        break;
    case NSRightMouseUp:
        result.type = WebInputEvent::MouseUp;
        result.clickCount = [event clickCount];
        result.button = WebMouseEvent::ButtonRight;
        break;
    case NSMouseMoved:
    case NSMouseEntered:
        result.type = WebInputEvent::MouseMove;
        break;
    case NSLeftMouseDragged:
        result.type = WebInputEvent::MouseMove;
        result.button = WebMouseEvent::ButtonLeft;
        break;
    case NSOtherMouseDragged:
        result.type = WebInputEvent::MouseMove;
        result.button = WebMouseEvent::ButtonMiddle;
        break;
    case NSRightMouseDragged:
        result.type = WebInputEvent::MouseMove;
        result.button = WebMouseEvent::ButtonRight;
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    setWebEventLocationFromEventInView(&result, event, view);

    result.modifiers = modifiersFromEvent(event);

    result.timeStampSeconds = [event timestamp];

    return result;
}

// WebMouseWheelEvent ---------------------------------------------------------

static WebMouseWheelEvent::Phase phaseForNSEventPhase(NSEventPhase eventPhase)
{
    uint32_t phase = WebMouseWheelEvent::PhaseNone;
    if (eventPhase & NSEventPhaseBegan)
        phase |= WebMouseWheelEvent::PhaseBegan;
    if (eventPhase & NSEventPhaseStationary)
        phase |= WebMouseWheelEvent::PhaseStationary;
    if (eventPhase & NSEventPhaseChanged)
        phase |= WebMouseWheelEvent::PhaseChanged;
    if (eventPhase & NSEventPhaseEnded)
        phase |= WebMouseWheelEvent::PhaseEnded;
    if (eventPhase & NSEventPhaseCancelled)
        phase |= WebMouseWheelEvent::PhaseCancelled;
    if (eventPhase & NSEventPhaseMayBegin)
        phase |= WebMouseWheelEvent::PhaseMayBegin;
    return static_cast<WebMouseWheelEvent::Phase>(phase);
}

static WebMouseWheelEvent::Phase phaseForEvent(NSEvent *event)
{
    if (![event respondsToSelector:@selector(phase)])
        return WebMouseWheelEvent::PhaseNone;

    NSEventPhase eventPhase = [event phase];
    return phaseForNSEventPhase(eventPhase);
}

static WebMouseWheelEvent::Phase momentumPhaseForEvent(NSEvent *event)
{
    if (![event respondsToSelector:@selector(momentumPhase)])
        return WebMouseWheelEvent::PhaseNone;

    NSEventPhase eventMomentumPhase = [event momentumPhase];
    return phaseForNSEventPhase(eventMomentumPhase);
}

WebMouseWheelEvent WebInputEventFactory::mouseWheelEvent(NSEvent* event, NSView* view, bool canRubberbandLeft, bool canRubberbandRight)
{
    WebMouseWheelEvent result;

    result.type = WebInputEvent::MouseWheel;
    result.button = WebMouseEvent::ButtonNone;

    result.modifiers = modifiersFromEvent(event);

    setWebEventLocationFromEventInView(&result, event, view);

    result.canRubberbandLeft = canRubberbandLeft;
    result.canRubberbandRight = canRubberbandRight;

    // Of Mice and Men
    // ---------------
    //
    // There are three types of scroll data available on a scroll wheel CGEvent.
    // Apple's documentation ([1]) is rather vague in their differences, and not
    // terribly helpful in deciding which to use. This is what's really going on.
    //
    // First, these events behave very differently depending on whether a standard
    // wheel mouse is used (one that scrolls in discrete units) or a
    // trackpad/Mighty Mouse is used (which both provide continuous scrolling).
    // You must check to see which was used for the event by testing the
    // kCGScrollWheelEventIsContinuous field.
    //
    // Second, these events refer to "axes". Axis 1 is the y-axis, and axis 2 is
    // the x-axis.
    //
    // Third, there is a concept of mouse acceleration. Scrolling the same amount
    // of physical distance will give you different results logically depending on
    // whether you scrolled a little at a time or in one continuous motion. Some
    // fields account for this while others do not.
    //
    // Fourth, for trackpads there is a concept of chunkiness. When scrolling
    // continuously, events can be delivered in chunks. That is to say, lots of
    // scroll events with delta 0 will be delivered, and every so often an event
    // with a non-zero delta will be delivered, containing the accumulated deltas
    // from all the intermediate moves. [2]
    //
    // For notchy wheel mice (kCGScrollWheelEventIsContinuous == 0)
    // ------------------------------------------------------------
    //
    // kCGScrollWheelEventDeltaAxis*
    //   This is the rawest of raw events. For each mouse notch you get a value of
    //   +1/-1. This does not take acceleration into account and thus is less
    //   useful for building UIs.
    //
    // kCGScrollWheelEventPointDeltaAxis*
    //   This is smarter. In general, for each mouse notch you get a value of
    //   +1/-1, but this _does_ take acceleration into account, so you will get
    //   larger values on longer scrolls. This field would be ideal for building
    //   UIs except for one nasty bug: when the shift key is pressed, this set of
    //   fields fails to move the value into the axis2 field (the other two types
    //   of data do). This wouldn't be so bad except for the fact that while the
    //   number of axes is used in the creation of a CGScrollWheelEvent, there is
    //   no way to get that information out of the event once created.
    //
    // kCGScrollWheelEventFixedPtDeltaAxis*
    //   This is a fixed value, and for each mouse notch you get a value of
    //   +0.1/-0.1 (but, like above, scaled appropriately for acceleration). This
    //   value takes acceleration into account, and in fact is identical to the
    //   results you get from -[NSEvent delta*]. (That is, if you linked on Tiger
    //   or greater; see [2] for details.)
    //
    // A note about continuous devices
    // -------------------------------
    //
    // There are two devices that provide continuous scrolling events (trackpads
    // and Mighty Mouses) and they behave rather differently. The Mighty Mouse
    // behaves a lot like a regular mouse. There is no chunking, and the
    // FixedPtDelta values are the PointDelta values multiplied by 0.1. With the
    // trackpad, though, there is chunking. While the FixedPtDelta values are
    // reasonable (they occur about every fifth event but have values five times
    // larger than usual) the Delta values are unreasonable. They don't appear to
    // accumulate properly.
    //
    // For continuous devices (kCGScrollWheelEventIsContinuous != 0)
    // -------------------------------------------------------------
    //
    // kCGScrollWheelEventDeltaAxis*
    //   This provides values with no acceleration. With a trackpad, these values
    //   are chunked but each non-zero value does not appear to be cumulative.
    //   This seems to be a bug.
    //
    // kCGScrollWheelEventPointDeltaAxis*
    //   This provides values with acceleration. With a trackpad, these values are
    //   not chunked and are highly accurate.
    //
    // kCGScrollWheelEventFixedPtDeltaAxis*
    //   This provides values with acceleration. With a trackpad, these values are
    //   chunked but unlike Delta events are properly cumulative.
    //
    // Summary
    // -------
    //
    // In general the best approach to take is: determine if the event is
    // continuous. If it is not, then use the FixedPtDelta events (or just stick
    // with Cocoa events). They provide both acceleration and proper horizontal
    // scrolling. If the event is continuous, then doing pixel scrolling with the
    // PointDelta is the way to go. In general, avoid the Delta events. They're
    // the oldest (dating back to 10.4, before CGEvents were public) but they lack
    // acceleration and precision, making them useful only in specific edge cases.
    //
    // References
    // ----------
    //
    // [1] <http://developer.apple.com/documentation/Carbon/Reference/QuartzEventServicesRef/Reference/reference.html>
    // [2] <http://developer.apple.com/releasenotes/Cocoa/AppKitOlderNotes.html>
    //     Scroll to the section headed "NSScrollWheel events".
    //
    // P.S. The "smooth scrolling" option in the system preferences is utterly
    // unrelated to any of this.

    CGEventRef cgEvent = [event CGEvent];
    ASSERT(cgEvent);

    // Wheel ticks are supposed to be raw, unaccelerated values, one per physical
    // mouse wheel notch. The delta event is perfect for this (being a good
    // "specific edge case" as mentioned above). Trackpads, unfortunately, do
    // event chunking, and sending mousewheel events with 0 ticks causes some
    // websites to malfunction. Therefore, for all continuous input devices we use
    // the point delta data instead, since we cannot distinguish trackpad data
    // from data from any other continuous device.

    // Conversion between wheel delta amounts and number of pixels to scroll.
    static const double scrollbarPixelsPerCocoaTick = 40.0;

    if (CGEventGetIntegerValueField(cgEvent, kCGScrollWheelEventIsContinuous)) {
        result.deltaX = CGEventGetIntegerValueField(cgEvent, kCGScrollWheelEventPointDeltaAxis2);
        result.deltaY = CGEventGetIntegerValueField(cgEvent, kCGScrollWheelEventPointDeltaAxis1);
        result.wheelTicksX = result.deltaX / scrollbarPixelsPerCocoaTick;
        result.wheelTicksY = result.deltaY / scrollbarPixelsPerCocoaTick;
        result.hasPreciseScrollingDeltas = true;
    } else {
        result.deltaX = [event deltaX] * scrollbarPixelsPerCocoaTick;
        result.deltaY = [event deltaY] * scrollbarPixelsPerCocoaTick;
        result.wheelTicksY = CGEventGetIntegerValueField(cgEvent, kCGScrollWheelEventDeltaAxis1);
        result.wheelTicksX = CGEventGetIntegerValueField(cgEvent, kCGScrollWheelEventDeltaAxis2);
    }

    result.timeStampSeconds = [event timestamp];

    result.phase              = phaseForEvent(event);
    result.momentumPhase      = momentumPhaseForEvent(event);

    return result;
}

WebGestureEvent WebInputEventFactory::gestureEvent(NSEvent *event, NSView *view)
{
    WebGestureEvent result;

    // Use a temporary WebMouseEvent to get the location.
    WebMouseEvent temp;

    setWebEventLocationFromEventInView(&temp, event, view);
    result.x = temp.x;
    result.y = temp.y;
    result.globalX = temp.globalX;
    result.globalY = temp.globalY;

    result.modifiers = modifiersFromEvent(event);
    result.timeStampSeconds = [event timestamp];

    result.sourceDevice = WebGestureDeviceTouchpad;
    switch ([event type]) {
    case NSEventTypeMagnify:
        result.type = WebInputEvent::GesturePinchUpdate;
        result.data.pinchUpdate.scale = [event magnification] + 1.0;
        break;
    case NSEventTypeSmartMagnify:
        // Map the Cocoa "double-tap with two fingers" zoom gesture to regular
        // GestureDoubleTap, because the effect is similar to single-finger
        // double-tap zoom on mobile platforms. Note that tapCount is set to 1
        // because the gesture type already encodes that information.
        result.type = WebInputEvent::GestureDoubleTap;
        result.data.tap.tapCount = 1;
        break;
    case NSEventTypeBeginGesture:
    case NSEventTypeEndGesture:
        // The specific type of a gesture is not defined when the gesture begin
        // and end NSEvents come in. Leave them undefined. The caller will need
        // to specify them when the gesture is differentiated.
        result.type = WebInputEvent::Undefined;
        break;
    default:
        ASSERT_NOT_REACHED();
        result.type = WebInputEvent::Undefined;
    }

    return result;
}

} // namespace blink
