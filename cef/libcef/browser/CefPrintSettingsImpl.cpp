// Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/browser/CefPrintSettingsImpl.h"

//#include "base/logging.h"

// CefPrintSettingsImpl::CefPrintSettingsImpl(printing::PrintSettings* value,
//                                            bool will_delete,
//                                            bool read_only)
//   : CefValueBase<CefPrintSettings, printing::PrintSettings>(
//         value, NULL, will_delete ? kOwnerWillDelete : kOwnerNoDelete,
//         read_only, NULL) {
// }
// 
// bool CefPrintSettingsImpl::IsValid() {
//   return !detached();
// }
// 
// bool CefPrintSettingsImpl::IsReadOnly() {
//   return read_only();
// }
// 
// CefRefPtr<CefPrintSettings> CefPrintSettingsImpl::Copy() {
//   CEF_VALUE_VERIFY_RETURN(false, NULL);
//   printing::PrintSettings* new_settings = new printing::PrintSettings;
//   *new_settings = const_value();
//   return new CefPrintSettingsImpl(new_settings, true, false);
// }
// 
// void CefPrintSettingsImpl::SetOrientation(bool landscape) {
//   CEF_VALUE_VERIFY_RETURN_VOID(true);
//   mutable_value()->SetOrientation(landscape);
// }
// 
// bool CefPrintSettingsImpl::IsLandscape() {
//   CEF_VALUE_VERIFY_RETURN(false, false);
//   return const_value().landscape();
// }
// 
// void CefPrintSettingsImpl::SetPrinterPrintableArea(
//     const CefSize& physical_size_device_units,
//     const CefRect& printable_area_device_units,
//     bool landscape_needs_flip) {
//   CEF_VALUE_VERIFY_RETURN_VOID(true);
//   gfx::Size size(physical_size_device_units.width,
//                  physical_size_device_units.height);
//   gfx::Rect rect(printable_area_device_units.x,
//                  printable_area_device_units.y,
//                  printable_area_device_units.width,
//                  printable_area_device_units.height);
//   mutable_value()->SetPrinterPrintableArea(size, rect, landscape_needs_flip);
// }
// 
// void CefPrintSettingsImpl::SetDeviceName(const CefString& name) {
//   CEF_VALUE_VERIFY_RETURN_VOID(true);
//   mutable_value()->set_device_name(name.ToString16());
// }
// 
// CefString CefPrintSettingsImpl::GetDeviceName() {
//   CEF_VALUE_VERIFY_RETURN(false, CefString());
//   return const_value().device_name();
// }
// 
// void CefPrintSettingsImpl::SetDPI(int dpi) {
//   CEF_VALUE_VERIFY_RETURN_VOID(true);
//   mutable_value()->set_dpi(dpi);
// }
// 
// int CefPrintSettingsImpl::GetDPI() {
//   CEF_VALUE_VERIFY_RETURN(false, 0);
//   return const_value().dpi();
// }
// 
// void CefPrintSettingsImpl::SetPageRanges(const PageRangeList& ranges) {
//   CEF_VALUE_VERIFY_RETURN_VOID(true);
//   printing::PageRanges page_ranges;
//   PageRangeList::const_iterator it = ranges.begin();
//   for(; it != ranges.end(); ++it) {
//     const CefPageRange& cef_range = *it;
//     printing::PageRange range;
//     range.from = cef_range.from;
//     range.to = cef_range.to;
//     page_ranges.push_back(range);
//   }
//   mutable_value()->set_ranges(page_ranges);
// }
// 
// size_t CefPrintSettingsImpl::GetPageRangesCount() {
//   CEF_VALUE_VERIFY_RETURN(false, 0);
//   return const_value().ranges().size();
// }
// 
// void CefPrintSettingsImpl::GetPageRanges(PageRangeList& ranges) {
//   CEF_VALUE_VERIFY_RETURN_VOID(false);
//   if (!ranges.empty())
//     ranges.clear();
//   const printing::PageRanges& page_ranges = const_value().ranges();
//   printing::PageRanges::const_iterator it = page_ranges.begin();
//   for (; it != page_ranges.end(); ++it) {
//     const printing::PageRange& range = *it;
//     ranges.push_back(CefPageRange(range.from, range.to));
//   }
// }
// 
// void CefPrintSettingsImpl::SetSelectionOnly(bool selection_only) {
//   CEF_VALUE_VERIFY_RETURN_VOID(true);
//   mutable_value()->set_selection_only(selection_only);
// }
// 
// bool CefPrintSettingsImpl::IsSelectionOnly() {
//   CEF_VALUE_VERIFY_RETURN(false, false);
//   return const_value().selection_only();
// }
// 
// void CefPrintSettingsImpl::SetCollate(bool collate) {
//   CEF_VALUE_VERIFY_RETURN_VOID(true);
//   mutable_value()->set_collate(collate);
// }
// 
// bool CefPrintSettingsImpl::WillCollate() {
//   CEF_VALUE_VERIFY_RETURN(false, false);
//   return const_value().collate();
// }
// 
// void CefPrintSettingsImpl::SetColorModel(ColorModel model) {
//   CEF_VALUE_VERIFY_RETURN_VOID(true);
//   mutable_value()->set_color(static_cast<printing::ColorModel>(model));
// }
// 
// CefPrintSettings::ColorModel CefPrintSettingsImpl::GetColorModel() {
//   CEF_VALUE_VERIFY_RETURN(false, COLOR_MODEL_UNKNOWN);
//   return static_cast<ColorModel>(const_value().color());
// }
// 
// void CefPrintSettingsImpl::SetCopies(int copies) {
//   CEF_VALUE_VERIFY_RETURN_VOID(true);
//   mutable_value()->set_copies(copies);
// }
// 
// int CefPrintSettingsImpl::GetCopies() {
//   CEF_VALUE_VERIFY_RETURN(false, false);
//   return const_value().copies();
// }
// 
// void CefPrintSettingsImpl::SetDuplexMode(DuplexMode mode) {
//   CEF_VALUE_VERIFY_RETURN_VOID(true);
//   mutable_value()->set_duplex_mode(static_cast<printing::DuplexMode>(mode));
// }
// 
// CefPrintSettings::DuplexMode CefPrintSettingsImpl::GetDuplexMode() {
//   CEF_VALUE_VERIFY_RETURN(false, DUPLEX_MODE_UNKNOWN);
//   return static_cast<DuplexMode>(const_value().duplex_mode());
// }
// 

// CefPrintSettings implementation.

// static
CefRefPtr<CefPrintSettings> CefPrintSettings::Create() {
    //return new CefPrintSettingsImpl(new printing::PrintSettings(), true, false);
    DebugBreak();
    return nullptr;
}

#endif