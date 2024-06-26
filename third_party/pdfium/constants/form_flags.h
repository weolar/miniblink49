// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSTANTS_FORM_FLAGS_H_
#define CONSTANTS_FORM_FLAGS_H_

namespace pdfium {
namespace form_flags {

// PDF 1.7 spec, table 8.70.
// Field flags common to all field types.
constexpr uint32_t kReadOnly = 1 << 0;
constexpr uint32_t kRequired = 1 << 1;
constexpr uint32_t kNoExport = 1 << 2;

}  // namespace form_flags
}  // namespace pdfium

#endif  // CONSTANTS_FORM_FLAGS_H_
