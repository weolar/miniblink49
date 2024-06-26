// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/media_resources.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"

namespace media {

static LocalizedStringProvider g_localized_string_provider = nullptr;

void SetLocalizedStringProvider(LocalizedStringProvider func)
{
    g_localized_string_provider = func;
}

#if !defined(OS_IOS)
std::string GetLocalizedStringUTF8(MessageId message_id)
{
    return base::UTF16ToUTF8(GetLocalizedStringUTF16(message_id));
}

base::string16 GetLocalizedStringUTF16(MessageId message_id)
{
    return g_localized_string_provider ? g_localized_string_provider(message_id)
                                       : base::string16();
}
#endif

} // namespace media
