// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/keycodes/platform_key_map_win.h"

#include "base/macros.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/event_constants.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/keycodes/dom/dom_key.h"
#include "ui/events/keycodes/dom/keycode_converter.h"

namespace ui {

namespace {

    const wchar_t* LAYOUT_US = L"00000409";
    const wchar_t* LAYOUT_FR = L"0000040c";

    struct TestKey {
        // Have to use KeyboardCode instead of DomCode because we don't know the
        // physical keyboard layout for try bots.
        KeyboardCode vk;
        const char* normal;
        const char* shift;
        const char* capslock;
        const char* altgr;
        const char* shift_capslock;
        const char* shift_altgr;
        const char* altgr_capslock;
    };

    void CheckDomCodeToKeyString(const char* label,
        const PlatformKeyMap& keymap,
        const TestKey& t,
        HKL layout)
    {
        int scan_code = ::MapVirtualKeyEx(t.vk, MAPVK_VK_TO_VSC, layout);
        DomCode dom_code = KeycodeConverter::NativeKeycodeToDomCode(scan_code);
        EXPECT_STREQ(t.normal, KeycodeConverter::DomKeyToKeyString(keymap.DomCodeAndFlagsToDomKey(dom_code, EF_NONE)).c_str()) << label;
        EXPECT_STREQ(t.shift, KeycodeConverter::DomKeyToKeyString(keymap.DomCodeAndFlagsToDomKey(dom_code, EF_SHIFT_DOWN)).c_str()) << label;
        EXPECT_STREQ(t.capslock, KeycodeConverter::DomKeyToKeyString(keymap.DomCodeAndFlagsToDomKey(dom_code, EF_CAPS_LOCK_ON)).c_str()) << label;
        EXPECT_STREQ(t.altgr, KeycodeConverter::DomKeyToKeyString(keymap.DomCodeAndFlagsToDomKey(dom_code, EF_ALTGR_DOWN)).c_str()) << label;
        EXPECT_STREQ(t.shift_capslock, KeycodeConverter::DomKeyToKeyString(keymap.DomCodeAndFlagsToDomKey(dom_code, EF_SHIFT_DOWN | EF_CAPS_LOCK_ON)).c_str()) << label;
        EXPECT_STREQ(t.shift_altgr, KeycodeConverter::DomKeyToKeyString(keymap.DomCodeAndFlagsToDomKey(dom_code, EF_SHIFT_DOWN | EF_ALTGR_DOWN)).c_str()) << label;
        EXPECT_STREQ(t.altgr_capslock, KeycodeConverter::DomKeyToKeyString(keymap.DomCodeAndFlagsToDomKey(dom_code, EF_ALTGR_DOWN | EF_CAPS_LOCK_ON)).c_str()) << label;
    }

} // anonymous namespace

class PlatformKeyMapTest : public testing::Test {
public:
    PlatformKeyMapTest() { }
    ~PlatformKeyMapTest() override { }

private:
    DISALLOW_COPY_AND_ASSIGN(PlatformKeyMapTest);
};

TEST_F(PlatformKeyMapTest, USLayout)
{
    HKL layout = ::LoadKeyboardLayout(LAYOUT_US, 0);
    PlatformKeyMap keymap(layout);

    const TestKey USKeys[] = {
        //       n    s    c    a    sc   sa   ac
        { VKEY_0, "0", ")", "0", "0", ")", ")", "0" },
        { VKEY_1, "1", "!", "1", "1", "!", "!", "1" },
        { VKEY_2, "2", "@", "2", "2", "@", "@", "2" },
        { VKEY_3, "3", "#", "3", "3", "#", "#", "3" },
        { VKEY_4, "4", "$", "4", "4", "$", "$", "4" },
        { VKEY_5, "5", "%", "5", "5", "%", "%", "5" },
        { VKEY_6, "6", "^", "6", "6", "^", "^", "6" },
        { VKEY_7, "7", "&", "7", "7", "&", "&", "7" },
        { VKEY_8, "8", "*", "8", "8", "*", "*", "8" },
        { VKEY_9, "9", "(", "9", "9", "(", "(", "9" },
        { VKEY_A, "a", "A", "A", "a", "a", "A", "A" },
        { VKEY_B, "b", "B", "B", "b", "b", "B", "B" },
        { VKEY_C, "c", "C", "C", "c", "c", "C", "C" },
        { VKEY_D, "d", "D", "D", "d", "d", "D", "D" },
        { VKEY_E, "e", "E", "E", "e", "e", "E", "E" },
        { VKEY_F, "f", "F", "F", "f", "f", "F", "F" },
        { VKEY_G, "g", "G", "G", "g", "g", "G", "G" },
        { VKEY_H, "h", "H", "H", "h", "h", "H", "H" },
        { VKEY_I, "i", "I", "I", "i", "i", "I", "I" },
        { VKEY_J, "j", "J", "J", "j", "j", "J", "J" },
        { VKEY_K, "k", "K", "K", "k", "k", "K", "K" },
        { VKEY_L, "l", "L", "L", "l", "l", "L", "L" },
        { VKEY_M, "m", "M", "M", "m", "m", "M", "M" },
        { VKEY_N, "n", "N", "N", "n", "n", "N", "N" },
        { VKEY_O, "o", "O", "O", "o", "o", "O", "O" },
        { VKEY_P, "p", "P", "P", "p", "p", "P", "P" },
        { VKEY_Q, "q", "Q", "Q", "q", "q", "Q", "Q" },
        { VKEY_R, "r", "R", "R", "r", "r", "R", "R" },
        { VKEY_S, "s", "S", "S", "s", "s", "S", "S" },
        { VKEY_T, "t", "T", "T", "t", "t", "T", "T" },
        { VKEY_U, "u", "U", "U", "u", "u", "U", "U" },
        { VKEY_V, "v", "V", "V", "v", "v", "V", "V" },
        { VKEY_W, "w", "W", "W", "w", "w", "W", "W" },
        { VKEY_X, "x", "X", "X", "x", "x", "X", "X" },
        { VKEY_Y, "y", "Y", "Y", "y", "y", "Y", "Y" },
        { VKEY_Z, "z", "Z", "Z", "z", "z", "Z", "Z" },
    };

    for (const auto& k : USKeys) {
        CheckDomCodeToKeyString("USLayout", keymap, k, layout);
    }
}

TEST_F(PlatformKeyMapTest, FRLayout)
{
    HKL layout = ::LoadKeyboardLayout(LAYOUT_FR, 0);
    PlatformKeyMap keymap(layout);

    const TestKey FRKeys[] = {
        //       n     s    c    a       sc    sa   ac
        { VKEY_0, "à", "0", "0", "@", "à", "0", "@" },
        { VKEY_1, "&", "1", "1", "&", "&", "1", "1" },
        { VKEY_2, "é", "2", "2", "Dead", "é", "2", "Dead" },
        { VKEY_3, "\"", "3", "3", "#", "\"", "3", "#" },
        { VKEY_4, "\'", "4", "4", "{", "\'", "4", "{" },
        { VKEY_5, "(", "5", "5", "[", "(", "5", "[" },
        { VKEY_6, "-", "6", "6", "|", "-", "6", "|" },
        { VKEY_7, "è", "7", "7", "Dead", "è", "7", "Dead" },
        { VKEY_8, "_", "8", "8", "\\", "_", "8", "\\" },
        { VKEY_9, "ç", "9", "9", "^", "ç", "9", "^" },
        { VKEY_A, "a", "A", "A", "a", "a", "A", "A" },
        { VKEY_B, "b", "B", "B", "b", "b", "B", "B" },
        { VKEY_C, "c", "C", "C", "c", "c", "C", "C" },
        { VKEY_D, "d", "D", "D", "d", "d", "D", "D" },
        { VKEY_E, "e", "E", "E", "€", "e", "E", "€" },
        { VKEY_F, "f", "F", "F", "f", "f", "F", "F" },
        { VKEY_G, "g", "G", "G", "g", "g", "G", "G" },
        { VKEY_H, "h", "H", "H", "h", "h", "H", "H" },
        { VKEY_I, "i", "I", "I", "i", "i", "I", "I" },
        { VKEY_J, "j", "J", "J", "j", "j", "J", "J" },
        { VKEY_K, "k", "K", "K", "k", "k", "K", "K" },
        { VKEY_L, "l", "L", "L", "l", "l", "L", "L" },
        { VKEY_M, "m", "M", "M", "m", "m", "M", "M" },
        { VKEY_N, "n", "N", "N", "n", "n", "N", "N" },
        { VKEY_O, "o", "O", "O", "o", "o", "O", "O" },
        { VKEY_P, "p", "P", "P", "p", "p", "P", "P" },
        { VKEY_Q, "q", "Q", "Q", "q", "q", "Q", "Q" },
        { VKEY_R, "r", "R", "R", "r", "r", "R", "R" },
        { VKEY_S, "s", "S", "S", "s", "s", "S", "S" },
        { VKEY_T, "t", "T", "T", "t", "t", "T", "T" },
        { VKEY_U, "u", "U", "U", "u", "u", "U", "U" },
        { VKEY_V, "v", "V", "V", "v", "v", "V", "V" },
        { VKEY_W, "w", "W", "W", "w", "w", "W", "W" },
        { VKEY_X, "x", "X", "X", "x", "x", "X", "X" },
        { VKEY_Y, "y", "Y", "Y", "y", "y", "Y", "Y" },
        { VKEY_Z, "z", "Z", "Z", "z", "z", "Z", "Z" },
    };

    for (const auto& k : FRKeys) {
        CheckDomCodeToKeyString("FRLayout", keymap, k, layout);
    }
}

} // namespace ui
