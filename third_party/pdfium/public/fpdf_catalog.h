// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PUBLIC_FPDF_CATALOG_H_
#define PUBLIC_FPDF_CATALOG_H_

// NOLINTNEXTLINE(build/include)
#include "fpdfview.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * Experimental API.
 *
 * Determine if |document| represents a tagged PDF.
 *
 * For the definition of tagged PDF, See (see 10.7 "Tagged PDF" in PDF
 * Reference 1.7).
 *
 *   document - handle to a document.
 *
 * Returns |true| iff |document| is a tagged PDF.
 */
FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFCatalog_IsTagged(FPDF_DOCUMENT document);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // PUBLIC_FPDF_CATALOG_H_
