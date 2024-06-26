// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PROTO_CC_PROTO_EXPORT_H_
#define CC_PROTO_CC_PROTO_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(CC_PROTO_IMPLEMENTATION)
#define CC_PROTO_EXPORT __declspec(dllexport)
#else
#define CC_PROTO_EXPORT __declspec(dllimport)
#endif // defined(CC_PROTO_IMPLEMENTATION)

#else // defined(WIN32)
#if defined(CC_PROTO_IMPLEMENTATION)
#define CC_PROTO_EXPORT __attribute__((visibility("default")))
#else
#define CC_PROTO_EXPORT
#endif // defined(CC_PROTO_IMPLEMENTATION)
#endif

#else // defined(COMPONENT_BUILD)
#define CC_PROTO_EXPORT
#endif

#endif // CC_PROTO_CC_PROTO_EXPORT_H_
