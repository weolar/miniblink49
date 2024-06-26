// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/keycodes/dom/keycode_converter.h"

#include "base/logging.h"
#include "base/macros.h"
#include "base/strings/utf_string_conversion_utils.h"
#include "build/build_config.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/keycodes/dom/dom_key.h"

namespace ui {

namespace {

// Table of USB codes (equivalent to DomCode values), native scan codes,
// and DOM Level 3 |code| strings.
#if defined(OS_WIN)
#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) \
    {                                                   \
        usb, win, code                                  \
    }
#elif defined(OS_LINUX)
#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) \
    {                                                   \
        usb, xkb, code                                  \
    }
#elif defined(OS_MACOSX)
#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) \
    {                                                   \
        usb, mac, code                                  \
    }
#elif defined(OS_ANDROID)
#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) \
    {                                                   \
        usb, evdev, code                                \
    }
#else
#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) \
    {                                                   \
        usb, 0, code                                    \
    }
#endif
#define USB_KEYMAP_DECLARATION const KeycodeMapEntry usb_keycode_map[] =
#include "ui/events/keycodes/dom/keycode_converter_data.inc"
#undef USB_KEYMAP
#undef USB_KEYMAP_DECLARATION

    const size_t kKeycodeMapEntries = arraysize(usb_keycode_map);

    // Table of DomKey enum values and DOM Level 3 |key| strings.
    struct DomKeyMapEntry {
        DomKey dom_key;
        const char* string;
    };

#define DOM_KEY_MAP_DECLARATION const DomKeyMapEntry dom_key_map[] =
#define DOM_KEY_UNI(key, id, value) \
    {                               \
        DomKey::id, key             \
    }
#define DOM_KEY_MAP(key, id, value) \
    {                               \
        DomKey::id, key             \
    }
#include "ui/events/keycodes/dom/dom_key_data.inc"
#undef DOM_KEY_MAP_DECLARATION
#undef DOM_KEY_MAP
#undef DOM_KEY_UNI

    const size_t kDomKeyMapEntries = arraysize(dom_key_map);

} // namespace

// static
size_t KeycodeConverter::NumKeycodeMapEntriesForTest()
{
    return kKeycodeMapEntries;
}

// static
const KeycodeMapEntry* KeycodeConverter::GetKeycodeMapForTest()
{
    return &usb_keycode_map[0];
}

// static
const char* KeycodeConverter::DomKeyStringForTest(size_t index)
{
    if (index >= kDomKeyMapEntries)
        return nullptr;
    return dom_key_map[index].string;
}

// static
int KeycodeConverter::InvalidNativeKeycode()
{
    return usb_keycode_map[0].native_keycode;
}

// static
DomCode KeycodeConverter::NativeKeycodeToDomCode(int native_keycode)
{
    for (size_t i = 0; i < kKeycodeMapEntries; ++i) {
        if (usb_keycode_map[i].native_keycode == native_keycode) {
            if (usb_keycode_map[i].code != NULL)
                return static_cast<DomCode>(usb_keycode_map[i].usb_keycode);
            break;
        }
    }
    return DomCode::NONE;
}

// static
int KeycodeConverter::DomCodeToNativeKeycode(DomCode code)
{
    return UsbKeycodeToNativeKeycode(static_cast<uint32_t>(code));
}

// static
DomCode KeycodeConverter::CodeStringToDomCode(const std::string& code)
{
    if (code.empty())
        return DomCode::NONE;
    for (size_t i = 0; i < kKeycodeMapEntries; ++i) {
        if (usb_keycode_map[i].code && code == usb_keycode_map[i].code) {
            return static_cast<DomCode>(usb_keycode_map[i].usb_keycode);
        }
    }
    LOG(WARNING) << "unrecognized code string '" << code << "'";
    return DomCode::NONE;
}

// static
const char* KeycodeConverter::DomCodeToCodeString(DomCode dom_code)
{
    for (size_t i = 0; i < kKeycodeMapEntries; ++i) {
        if (usb_keycode_map[i].usb_keycode == static_cast<uint32_t>(dom_code)) {
            if (usb_keycode_map[i].code)
                return usb_keycode_map[i].code;
            break;
        }
    }
    return "";
}

// static
DomKeyLocation KeycodeConverter::DomCodeToLocation(DomCode dom_code)
{
    static const struct {
        DomCode code;
        DomKeyLocation location;
    } kLocations[] = { { DomCode::CONTROL_LEFT, DomKeyLocation::LEFT },
        { DomCode::SHIFT_LEFT, DomKeyLocation::LEFT },
        { DomCode::ALT_LEFT, DomKeyLocation::LEFT },
        { DomCode::OS_LEFT, DomKeyLocation::LEFT },
        { DomCode::CONTROL_RIGHT, DomKeyLocation::RIGHT },
        { DomCode::SHIFT_RIGHT, DomKeyLocation::RIGHT },
        { DomCode::ALT_RIGHT, DomKeyLocation::RIGHT },
        { DomCode::OS_RIGHT, DomKeyLocation::RIGHT },
        { DomCode::NUMPAD_DIVIDE, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_MULTIPLY, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_SUBTRACT, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_ADD, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_ENTER, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD1, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD2, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD3, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD4, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD5, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD6, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD7, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD8, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD9, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD0, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_DECIMAL, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_EQUAL, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_COMMA, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_PAREN_LEFT, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_PAREN_RIGHT, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_BACKSPACE, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_MEMORY_STORE, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_MEMORY_RECALL, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_MEMORY_CLEAR, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_MEMORY_ADD, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_MEMORY_SUBTRACT, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_SIGN_CHANGE, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_CLEAR, DomKeyLocation::NUMPAD },
        { DomCode::NUMPAD_CLEAR_ENTRY, DomKeyLocation::NUMPAD } };
    for (const auto& key : kLocations) {
        if (key.code == dom_code)
            return key.location;
    }
    return DomKeyLocation::STANDARD;
}

// static
DomKey KeycodeConverter::KeyStringToDomKey(const std::string& key)
{
    if (key.empty())
        return DomKey::NONE;
    // Check for standard key names.
    for (size_t i = 0; i < kDomKeyMapEntries; ++i) {
        if (dom_key_map[i].string && key == dom_key_map[i].string) {
            return dom_key_map[i].dom_key;
        }
    }
    if (key == "Dead") {
        // The web KeyboardEvent string does not encode the combining character,
        // so we just set it to the Unicode designated non-character 0xFFFF.
        // This will round-trip convert back to 'Dead' but take no part in
        // character composition.
        return DomKey::DeadKeyFromCombiningCharacter(0xFFFF);
    }
    // Otherwise, if the string contains a single Unicode character,
    // the key value is that character.
    int32_t char_index = 0;
    uint32_t character;
    if (base::ReadUnicodeCharacter(key.c_str(),
            static_cast<int32_t>(key.length()),
            &char_index, &character)
        && key[++char_index] == 0) {
        return DomKey::FromCharacter(character);
    }
    return DomKey::NONE;
}

// static
std::string KeycodeConverter::DomKeyToKeyString(DomKey dom_key)
{
    if (dom_key.IsDeadKey()) {
        // All dead-key combining codes collapse to 'Dead', as UI Events
        // KeyboardEvent represents the combining character separately.
        return "Dead";
    }
    for (size_t i = 0; i < kDomKeyMapEntries; ++i) {
        if (dom_key_map[i].dom_key == dom_key) {
            if (dom_key_map[i].string)
                return dom_key_map[i].string;
            break;
        }
    }
    if (dom_key.IsCharacter()) {
        std::string s;
        base::WriteUnicodeCharacter(dom_key.ToCharacter(), &s);
        return s;
    }
    return std::string();
}

// static
bool KeycodeConverter::IsDomKeyForModifier(DomKey dom_key)
{
    switch (dom_key) {
    case DomKey::ACCEL:
    case DomKey::ALT:
    case DomKey::ALT_GRAPH:
    case DomKey::CAPS_LOCK:
    case DomKey::CONTROL:
    case DomKey::FN:
    case DomKey::FN_LOCK:
    case DomKey::HYPER:
    case DomKey::META:
    case DomKey::NUM_LOCK:
    case DomKey::SCROLL_LOCK:
    case DomKey::SHIFT:
    case DomKey::SUPER:
    case DomKey::SYMBOL:
    case DomKey::SYMBOL_LOCK:
    case DomKey::SHIFT_LEVEL5:
        return true;
    default:
        return false;
    }
}

// USB keycodes
// Note that USB keycodes are not part of any web standard.
// Please don't use USB keycodes in new code.

// static
uint32_t KeycodeConverter::InvalidUsbKeycode()
{
    return usb_keycode_map[0].usb_keycode;
}

// static
int KeycodeConverter::UsbKeycodeToNativeKeycode(uint32_t usb_keycode)
{
    // Deal with some special-cases that don't fit the 1:1 mapping.
    if (usb_keycode == 0x070032) // non-US hash.
        usb_keycode = 0x070031; // US backslash.
#if defined(OS_MACOSX)
    if (usb_keycode == 0x070046) // PrintScreen.
        usb_keycode = 0x070068; // F13.
#endif

    for (size_t i = 0; i < kKeycodeMapEntries; ++i) {
        if (usb_keycode_map[i].usb_keycode == usb_keycode)
            return usb_keycode_map[i].native_keycode;
    }
    return InvalidNativeKeycode();
}

// static
uint32_t KeycodeConverter::NativeKeycodeToUsbKeycode(int native_keycode)
{
    for (size_t i = 0; i < kKeycodeMapEntries; ++i) {
        if (usb_keycode_map[i].native_keycode == native_keycode)
            return usb_keycode_map[i].usb_keycode;
    }
    return InvalidUsbKeycode();
}

// static
DomCode KeycodeConverter::UsbKeycodeToDomCode(uint32_t usb_keycode)
{
    for (size_t i = 0; i < kKeycodeMapEntries; ++i) {
        if (usb_keycode_map[i].usb_keycode == usb_keycode)
            return static_cast<DomCode>(usb_keycode);
    }
    return DomCode::NONE;
}

// static
uint32_t KeycodeConverter::DomCodeToUsbKeycode(DomCode dom_code)
{
    for (size_t i = 0; i < kKeycodeMapEntries; ++i) {
        if (usb_keycode_map[i].usb_keycode == static_cast<uint32_t>(dom_code))
            return usb_keycode_map[i].usb_keycode;
    }
    return InvalidUsbKeycode();
}

// static
uint32_t KeycodeConverter::CodeToUsbKeycode(const std::string& code)
{
    if (code.empty())
        return InvalidUsbKeycode();

    for (size_t i = 0; i < kKeycodeMapEntries; ++i) {
        if (usb_keycode_map[i].code && code == usb_keycode_map[i].code) {
            return usb_keycode_map[i].usb_keycode;
        }
    }
    return InvalidUsbKeycode();
}

} // namespace ui
