// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WTF_ContainerAnnotations_h
#define WTF_ContainerAnnotations_h

#include "wtf/CPU.h"

// TODO(ochang): Remove the CPU(X86_64) condition to enable this for X86 once
// the crashes there have been fixed: http://crbug.com/461406
#if defined(ADDRESS_SANITIZER) && OS(LINUX) && CPU(X86_64)
#define ANNOTATE_CONTIGUOUS_CONTAINER
#define ANNOTATE_NEW_BUFFER(buffer, capacity, newSize) \
    if (buffer) { \
        __sanitizer_annotate_contiguous_container(buffer, buffer + capacity, buffer + capacity, buffer + newSize); \
    }
#define ANNOTATE_DELETE_BUFFER(buffer, capacity, oldSize) \
    if (buffer) { \
        __sanitizer_annotate_contiguous_container(buffer, buffer + capacity, buffer + oldSize, buffer + capacity); \
    }
#define ANNOTATE_CHANGE_SIZE(buffer, capacity, oldSize, newSize) \
    if (buffer) { \
        __sanitizer_annotate_contiguous_container(buffer, buffer + capacity, buffer + oldSize, buffer + newSize); \
    }
#define ANNOTATE_CHANGE_CAPACITY(buffer, oldCapacity, bufferSize, newCapacity) \
    ANNOTATE_DELETE_BUFFER(buffer, oldCapacity, bufferSize); \
    ANNOTATE_NEW_BUFFER(buffer, newCapacity, bufferSize);
// Annotations require buffers to begin on an 8-byte boundary.
#else // defined(ADDRESS_SANITIZER) && OS(LINUX) && CPU(X86_64)
#define ANNOTATE_NEW_BUFFER(buffer, capacity, newSize)
#define ANNOTATE_DELETE_BUFFER(buffer, capacity, oldSize)
#define ANNOTATE_CHANGE_SIZE(buffer, capacity, oldSize, newSize)
#define ANNOTATE_CHANGE_CAPACITY(buffer, oldCapacity, bufferSize, newCapacity)
#endif // defined(ADDRESS_SANITIZER) && OS(LINUX) && CPU(X86_64)

#endif // WTF_ContainerAnnotations_h
