// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_ANDROID_EVENTS_JNI_REGISTRAR_H_
#define UI_EVENTS_ANDROID_EVENTS_JNI_REGISTRAR_H_

#include <jni.h>

#include "ui/events/events_export.h"

namespace ui {
namespace events {
    namespace android {

        // Register all JNI bindings necessary for chrome.
        EVENTS_EXPORT bool RegisterJni(JNIEnv* env);

    } // namespace android
} // namespace events
} // namespace ui

#endif // UI_EVENTS_ANDROID_EVENTS_JNI_REGISTRAR_H_
