// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Singly or multiply-included shared traits file depending upon circumstances.
// This allows the use of IPC serialization macros in more than one IPC message
// file.
#ifndef UI_GFX_IPC_GFX_PARAM_TRAITS_MACROS_H_
#define UI_GFX_IPC_GFX_PARAM_TRAITS_MACROS_H_

#include "ipc/ipc_message_macros.h"
#include "ui/gfx/buffer_types.h"
#include "ui/gfx/ipc/gfx_ipc_export.h"

#if defined(USE_OZONE)
#include "ui/gfx/native_pixmap_handle_ozone.h"
#endif

#undef IPC_MESSAGE_EXPORT
#define IPC_MESSAGE_EXPORT GFX_IPC_EXPORT

IPC_ENUM_TRAITS_MAX_VALUE(gfx::BufferFormat, gfx::BufferFormat::LAST)

IPC_ENUM_TRAITS_MAX_VALUE(gfx::BufferUsage, gfx::BufferUsage::LAST)

#if defined(USE_OZONE)
IPC_STRUCT_TRAITS_BEGIN(gfx::NativePixmapHandle)
IPC_STRUCT_TRAITS_MEMBER(fd)
IPC_STRUCT_TRAITS_MEMBER(stride)
IPC_STRUCT_TRAITS_END()
#endif

#undef IPC_MESSAGE_EXPORT
#define IPC_MESSAGE_EXPORT

#endif // UI_GFX_IPC_GFX_PARAM_TRAITS_MACROS_H_
