// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/keycodes/platform_key_map_win.h"

#include <utility>

#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/threading/thread_local_storage.h"

#include "ui/events/event_constants.h"
#include "ui/events/keycodes/dom/dom_code.h"

namespace ui {

namespace {

    struct DomCodeEntry {
        DomCode dom_code;
        int scan_code;
    };
#define USB_KEYMAP_DECLARATION const DomCodeEntry supported_dom_code_list[] =
#define USB_KEYMAP(usb, evdev, xkb, win, mac, code, id) \
    {                                                   \
        DomCode::id, win                                \
    }
#include "ui/events/keycodes/dom/keycode_converter_data.inc"
#undef USB_KEYMAP
#undef USB_KEYMAP_DECLARATION

    // List of modifiers mentioned in https://w3c.github.io/uievents/#keys-modifiers
    // Some modifiers are commented out because they usually don't change keys.
    const EventFlags modifier_flags[] = {
        EF_SHIFT_DOWN,
        EF_CONTROL_DOWN,
        EF_ALT_DOWN,
        // EF_COMMAND_DOWN,
        EF_ALTGR_DOWN,
        // EF_NUM_LOCK_ON,
        EF_CAPS_LOCK_ON,
        // EF_SCROLL_LOCK_ON
    };
    const int kModifierFlagsCombinations = (1 << arraysize(modifier_flags)) - 1;

    int GetModifierFlags(int combination)
    {
        int flags = EF_NONE;
        for (size_t i = 0; i < arraysize(modifier_flags); ++i) {
            if (combination & (1 << i))
                flags |= modifier_flags[i];
        }
        return flags;
    }

    void SetModifierState(BYTE* keyboard_state, int flags)
    {
        // According to MSDN GetKeyState():
        // 1. If the high-order bit is 1, the key is down; otherwise, it is up.
        // 2. If the low-order bit is 1, the key is toggled. A key, such as the
        //    CAPS LOCK key, is toggled if it is turned on. The key is off and
        //    untoggled if the low-order bit is 0.
        // See https://msdn.microsoft.com/en-us/library/windows/desktop/ms646301.aspx
        if (flags & EF_SHIFT_DOWN)
            keyboard_state[VK_SHIFT] |= 0x80;

        if (flags & EF_CONTROL_DOWN)
            keyboard_state[VK_CONTROL] |= 0x80;

        if (flags & EF_ALT_DOWN)
            keyboard_state[VK_MENU] |= 0x80;

        if (flags & EF_ALTGR_DOWN) {
            // AltGr should be RightAlt+LeftControl within Windows, but actually only
            // the non-located keys will work here.
            keyboard_state[VK_MENU] |= 0x80;
            keyboard_state[VK_CONTROL] |= 0x80;
        }

        if (flags & EF_COMMAND_DOWN)
            keyboard_state[VK_LWIN] |= 0x80;

        if (flags & EF_NUM_LOCK_ON)
            keyboard_state[VK_NUMLOCK] |= 0x01;

        if (flags & EF_CAPS_LOCK_ON)
            keyboard_state[VK_CAPITAL] |= 0x01;

        if (flags & EF_SCROLL_LOCK_ON)
            keyboard_state[VK_SCROLL] |= 0x01;
    }

    void CleanupKeyMapTls(void* data)
    {
        PlatformKeyMap* key_map = reinterpret_cast<PlatformKeyMap*>(data);
        delete key_map;
    }

    struct PlatformKeyMapInstanceTlsTraits
        : public base::DefaultLazyInstanceTraits<base::ThreadLocalStorage::Slot> {
        static base::ThreadLocalStorage::Slot* New(void* instance)
        {
            // Use placement new to initialize our instance in our preallocated space.
            // TODO(chongz): Use std::default_delete instead of providing own function.
            return new (instance) base::ThreadLocalStorage::Slot(CleanupKeyMapTls);
        }
    };

    base::LazyInstance<base::ThreadLocalStorage::Slot,
        PlatformKeyMapInstanceTlsTraits>
        g_platform_key_map_tls_lazy = LAZY_INSTANCE_INITIALIZER;

} // anonymous namespace

PlatformKeyMap::PlatformKeyMap() { }

PlatformKeyMap::PlatformKeyMap(HKL layout)
{
    UpdateLayout(layout);
}

PlatformKeyMap::~PlatformKeyMap() { }

DomKey PlatformKeyMap::DomCodeAndFlagsToDomKey(DomCode code, int flags) const
{
    const int flags_to_try[] = {
        // Trying to match Firefox's behavior and UIEvents DomKey guidelines.
        // If the combination doesn't produce a printable character, the key value
        // should be the key with no modifiers except for Shift and AltGr.
        // See https://w3c.github.io/uievents/#keys-guidelines
        flags,
        flags & (EF_SHIFT_DOWN | EF_ALTGR_DOWN | EF_CAPS_LOCK_ON),
        flags & (EF_SHIFT_DOWN | EF_CAPS_LOCK_ON),
        EF_NONE,
    };

    DomKey key = DomKey::NONE;
    for (auto try_flags : flags_to_try) {
        const auto& it = code_to_key_.find(std::make_pair(static_cast<int>(code),
            try_flags));
        if (it != code_to_key_.end()) {
            key = it->second;
            if (key != DomKey::NONE)
                break;
        }
    }
    return key;
}

// static
DomKey PlatformKeyMap::DomCodeAndFlagsToDomKeyStatic(DomCode code, int flags)
{
    // Use TLS because KeyboardLayout is per thread.
    // However currently PlatformKeyMap will only be used by the host application,
    // which is just one process and one thread.
    base::ThreadLocalStorage::Slot* platform_key_map_tls = g_platform_key_map_tls_lazy.Pointer();
    PlatformKeyMap* platform_key_map = reinterpret_cast<PlatformKeyMap*>(platform_key_map_tls->Get());
    if (!platform_key_map) {
        platform_key_map = new PlatformKeyMap();
        platform_key_map_tls->Set(platform_key_map);
    }

    HKL current_layout = ::GetKeyboardLayout(0);
    platform_key_map->UpdateLayout(current_layout);
    return platform_key_map->DomCodeAndFlagsToDomKey(code, flags);
}

void PlatformKeyMap::UpdateLayout(HKL layout)
{
    if (layout == keyboard_layout_)
        return;

    // TODO(chongz): Optimize layout switching (see crbug.com/587147).
    keyboard_layout_ = layout;
    code_to_key_.clear();
    // Map size for some sample keyboard layouts:
    // US: 428, French: 554, Persian: 434, Vietnamese: 1388
    code_to_key_.reserve(500);

    BYTE keyboard_state_to_restore[256];
    ::GetKeyboardState(keyboard_state_to_restore);

    for (int eindex = 0; eindex <= kModifierFlagsCombinations; ++eindex) {
        BYTE keyboard_state[256];
        memset(keyboard_state, 0, sizeof(keyboard_state));
        int flags = GetModifierFlags(eindex);
        SetModifierState(keyboard_state, flags);
        for (const auto& dom_code_entry : supported_dom_code_list) {
            wchar_t translated_chars[5];
            int key_code = ::MapVirtualKeyEx(dom_code_entry.scan_code,
                MAPVK_VSC_TO_VK, keyboard_layout_);
            int rv = ::ToUnicodeEx(key_code, 0, keyboard_state, translated_chars,
                arraysize(translated_chars), 0, keyboard_layout_);

            if (rv == -1) {
                // Dead key, injecting VK_SPACE to get character representation.
                BYTE empty_state[256];
                memset(empty_state, 0, sizeof(empty_state));
                rv = ::ToUnicodeEx(VK_SPACE, 0, empty_state, translated_chars,
                    arraysize(translated_chars), 0, keyboard_layout_);
                // Expecting a dead key character (not followed by a space).
                if (rv == 1) {
                    code_to_key_[std::make_pair(static_cast<int>(dom_code_entry.dom_code),
                        flags)]
                        = DomKey::DeadKeyFromCombiningCharacter(translated_chars[0]);
                } else {
                    // TODO(chongz): Check if this will actually happen.
                }
            } else if (rv == 1) {
                if (translated_chars[0] >= 0x20) {
                    code_to_key_[std::make_pair(static_cast<int>(dom_code_entry.dom_code),
                        flags)]
                        = DomKey::FromCharacter(translated_chars[0]);
                } else {
                    // Ignores legacy non-printable control characters.
                }
            } else {
                // TODO(chongz): Handle rv <= -2 and rv >= 2.
            }
        }
    }
    ::SetKeyboardState(keyboard_state_to_restore);
}

} // namespace ui
