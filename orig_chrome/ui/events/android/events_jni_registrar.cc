// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/android/events_jni_registrar.h"

#include "base/android/jni_android.h"
#include "base/android/jni_registrar.h"
#include "base/macros.h"
#include "ui/events/android/motion_event_android.h"

namespace ui {
namespace events {
    namespace android {

        static base::android::RegistrationMethod kAndroidRegisteredMethods[] = {
            { "MotionEventAndroid", ui::MotionEventAndroid::RegisterMotionEventAndroid },
        };

        bool RegisterJni(JNIEnv* env)
        {
            return RegisterNativeMethods(env, kAndroidRegisteredMethods,
                arraysize(kAndroidRegisteredMethods));
        }

    } // namespace android
} // namespace events
} // namespace ui
