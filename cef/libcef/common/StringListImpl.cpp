// Copyright (c) 2009 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include <vector>
#include "include/internal/cef_string_list.h"
#include "wtf/Vector.h"

typedef WTF::Vector<CefString> StringList;

CEF_EXPORT cef_string_list_t cef_string_list_alloc() {
  return new StringList;
}

CEF_EXPORT int cef_string_list_size(cef_string_list_t list) {
  ASSERT(list);
  StringList* impl = reinterpret_cast<StringList*>(list);
  return impl->size();
}

CEF_EXPORT int cef_string_list_value(cef_string_list_t list, int index,
                                     cef_string_t* value) {
    ASSERT(list);
    ASSERT(value);
    StringList* impl = reinterpret_cast<StringList*>(list);
    ASSERT(index >= 0);
    ASSERT(index <= static_cast<int>(impl->size()));
    if (index < 0 || index >= static_cast<int>(impl->size()))
        return false;
    const CefString& str = (*impl)[index];
    return cef_string_copy(str.c_str(), str.length(), value);
}

CEF_EXPORT void cef_string_list_append(cef_string_list_t list,
                                       const cef_string_t* value) {
    ASSERT(list);
      StringList* impl = reinterpret_cast<StringList*>(list);
      impl->append(CefString(value));
}

CEF_EXPORT void cef_string_list_clear(cef_string_list_t list) {
    ASSERT(list);
      StringList* impl = reinterpret_cast<StringList*>(list);
      impl->clear(); // TODO delete cef_string
}

CEF_EXPORT void cef_string_list_free(cef_string_list_t list) {
    ASSERT(list);
    StringList* impl = reinterpret_cast<StringList*>(list);
    delete impl; // TODO delete cef_string
}

CEF_EXPORT cef_string_list_t cef_string_list_copy(cef_string_list_t list) {
    ASSERT(list);
    StringList* impl = reinterpret_cast<StringList*>(list);
    return new StringList(*impl);
}
#endif
