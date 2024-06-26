// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/linux/text_edit_key_bindings_delegate_auralinux.h"

namespace ui {

namespace {
    // Optional delegate. Unowned pointer.
    TextEditKeyBindingsDelegateAuraLinux* text_edit_keybinding_delegate_ = 0;
}

void SetTextEditKeyBindingsDelegate(
    TextEditKeyBindingsDelegateAuraLinux* delegate)
{
    text_edit_keybinding_delegate_ = delegate;
}

TextEditKeyBindingsDelegateAuraLinux* GetTextEditKeyBindingsDelegate()
{
    return text_edit_keybinding_delegate_;
}

} // namespace ui
