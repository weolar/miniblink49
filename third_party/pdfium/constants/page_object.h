// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSTANTS_PAGE_OBJECT_H_
#define CONSTANTS_PAGE_OBJECT_H_

namespace pdfium {
namespace page_object {

// PDF 1.7 spec, table 3.27.
// Entries in a page object.

constexpr char kType[] = "Type";
constexpr char kParent[] = "Parent";
constexpr char kResources[] = "Resources";
constexpr char kMediaBox[] = "MediaBox";
constexpr char kCropBox[] = "CropBox";
constexpr char kBleedBox[] = "BleedBox";
constexpr char kTrimBox[] = "TrimBox";
constexpr char kArtBox[] = "ArtBox";
constexpr char kContents[] = "Contents";
constexpr char kRotate[] = "Rotate";

}  // namespace page_object
}  // namespace pdfium

#endif  // CONSTANTS_PAGE_OBJECT_H_
