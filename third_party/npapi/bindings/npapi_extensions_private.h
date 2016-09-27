/* Copyright (c) 2006-2010 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _NP_EXTENSIONS_PRIVATE_H_
#define _NP_EXTENSIONS_PRIVATE_H_

#include "third_party/npapi/bindings/npapi.h"

// Some reserved GetStateContext/SetStateContext selectors.
typedef enum {
  NPExtensionsReservedStateSharedMemory = 66536,
  // Used by the Device2D and Audio devices to return a pointer to the
  // structure used to implement the shared memory buffer for the device.
  NPExtensionsReservedStateSharedMemorySize = 66537,
  // Used by the Audio device to return a pointer to the
  // structure used to implement the shared memory buffer for the device.
  NPExtensionsReservedStateSyncChannel = 66538,
  // Used by the Audio device to return a pointer to the
  // structure used to implement the synchronization channel for the device.
  NPExtensionsReservedStateSharedMemoryChecksum = 66539
  // Used by the Device2D to return the CRC32 checksum of the content
  // stored in the shared memory buffer for the device.
} NPExtensionsReservedStates;

#endif  /* _NP_EXTENSIONS_PRIVATE_H_ */
