// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSTANTS_STREAM_DICT_COMMON_H_
#define CONSTANTS_STREAM_DICT_COMMON_H_

namespace pdfium {
namespace stream {

// PDF 1.7 spec, table 3.4.
// Entries common to all stream dictionaries.

// TODO(https://crbug.com/pdfium/1049): Examine all usages of "Length",
// "Filter", and "F".
constexpr char kLength[] = "Length";
constexpr char kFilter[] = "Filter";
constexpr char kDecodeParms[] = "DecodeParms";
constexpr char kF[] = "F";
// constexpr char kFFilter[] = "FFilter";
// constexpr char kFDecodeParms[] = "FDecodeParms";
constexpr char kDL[] = "DL";

}  // namespace stream
}  // namespace pdfium

#endif  // CONSTANTS_STREAM_DICT_COMMON_H_
