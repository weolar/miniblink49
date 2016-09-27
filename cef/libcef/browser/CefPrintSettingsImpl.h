// Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PRINT_SETTINGS_IMPL_H_
#define CEF_LIBCEF_BROWSER_PRINT_SETTINGS_IMPL_H_
#pragma once

#include "include/cef_print_settings.h"
// #include "libcef/common/value_base.h"
// 
// #include "printing/print_settings.h"

// CefPrintSettings implementation

// class CefPrintSettingsImpl
//     : public CefValueBase<CefPrintSettings, printing::PrintSettings> {
//  public:
//   CefPrintSettingsImpl(printing::PrintSettings* value,
//                        bool will_delete,
//                        bool read_only);
// 
//   // CefPrintSettings methods.
//   bool IsValid() override;
//   bool IsReadOnly() override;
//   CefRefPtr<CefPrintSettings> Copy() override;
//   void SetOrientation(bool landscape) override;
//   bool IsLandscape() override;
//   void SetPrinterPrintableArea(
//       const CefSize& physical_size_device_units,
//       const CefRect& printable_area_device_units,
//       bool landscape_needs_flip) override;
//   void SetDeviceName(const CefString& name) override;
//   CefString GetDeviceName() override;
//   void SetDPI(int dpi) override;
//   int GetDPI() override;
//   void SetPageRanges(const PageRangeList& ranges) override;
//   size_t GetPageRangesCount() override;
//   void GetPageRanges(PageRangeList& ranges) override;
//   void SetSelectionOnly(bool selection_only) override;
//   bool IsSelectionOnly() override;
//   void SetCollate(bool collate) override;
//   bool WillCollate() override;
//   void SetColorModel(ColorModel model) override;
//   ColorModel GetColorModel() override;
//   void SetCopies(int copies) override;
//   int GetCopies() override;
//   void SetDuplexMode(DuplexMode mode) override;
//   DuplexMode GetDuplexMode() override;
// 
//   // Must hold the controller lock while using this value.
//   const printing::PrintSettings& print_settings() { return const_value(); }
// 
//   DISALLOW_COPY_AND_ASSIGN(CefPrintSettingsImpl);
// };

#endif  // CEF_LIBCEF_BROWSER_PRINT_SETTINGS_IMPL_H_
