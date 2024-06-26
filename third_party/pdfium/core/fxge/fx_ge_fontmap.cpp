// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <memory>

#include "core/fxge/cfx_fontmapper.h"
#include "core/fxge/systemfontinfo_iface.h"

static ByteString GetStringFromTable(const uint8_t* string_ptr,
                                     uint32_t string_ptr_length,
                                     uint16_t offset,
                                     uint16_t length) {
  if (string_ptr_length < static_cast<uint32_t>(offset + length)) {
    return ByteString();
  }
  return ByteString(string_ptr + offset, length);
}

ByteString GetNameFromTT(const uint8_t* name_table,
                         uint32_t name_table_size,
                         uint32_t name_id) {
  if (!name_table || name_table_size < 6) {
    return ByteString();
  }
  uint32_t name_count = GET_TT_SHORT(name_table + 2);
  uint32_t string_offset = GET_TT_SHORT(name_table + 4);
  // We will ignore the possibility of overlap of structures and
  // string table as if it's all corrupt there's not a lot we can do.
  if (name_table_size < string_offset) {
    return ByteString();
  }

  const uint8_t* string_ptr = name_table + string_offset;
  uint32_t string_ptr_size = name_table_size - string_offset;
  name_table += 6;
  name_table_size -= 6;
  if (name_table_size < name_count * 12) {
    return ByteString();
  }

  for (uint32_t i = 0; i < name_count; i++, name_table += 12) {
    if (GET_TT_SHORT(name_table + 6) == name_id &&
        GET_TT_SHORT(name_table) == 1 && GET_TT_SHORT(name_table + 2) == 0) {
      return GetStringFromTable(string_ptr, string_ptr_size,
                                GET_TT_SHORT(name_table + 10),
                                GET_TT_SHORT(name_table + 8));
    }
  }
  return ByteString();
}
#ifdef PDF_ENABLE_XFA
void* SystemFontInfoIface::MapFontByUnicode(uint32_t dwUnicode,
                                            int weight,
                                            bool bItalic,
                                            int pitch_family) {
  return nullptr;
}
#endif  // PDF_ENABLE_XFA

int SystemFontInfoIface::GetFaceIndex(void* hFont) {
  return 0;
}

#if _FX_OS_ == _FX_OS_ANDROID_
std::unique_ptr<SystemFontInfoIface> SystemFontInfoIface::CreateDefault(
    const char** pUnused) {
  return nullptr;
}
#endif
