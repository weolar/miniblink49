// Copyright (c) 2014 The Chromium Embedded Framework Authors.
// Portions Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cefclient/browser/print_handler_gtk.h"

#include <vector>

#include "include/base/cef_logging.h"
#include "include/base/cef_macros.h"
#include "include/wrapper/cef_helpers.h"

namespace client {

namespace {

// CUPS Duplex attribute and values.
const char kCUPSDuplex[] = "cups-Duplex";
const char kDuplexNone[] = "None";
const char kDuplexTumble[] = "DuplexTumble";
const char kDuplexNoTumble[] = "DuplexNoTumble";

// CUPS color mode attribute and values.
const char kCUPSColorMode[] = "cups-ColorMode";
const char kCUPSColorModel[] = "cups-ColorModel";
const char kCUPSPrintoutMode[] = "cups-PrintoutMode";
const char kCUPSProcessColorModel[] = "cups-ProcessColorModel";
const char kBlack[] = "Black";
const char kCMYK[] = "CMYK";
const char kCMY_K[] = "CMY+K";
const char kCMY[] = "CMY";
const char kColor[] = "Color";
const char kGray[] = "Gray";
const char kGrayscale[] = "Grayscale";
const char kGreyscale[] = "Greyscale";
const char kMonochrome[] = "Monochrome";
const char kNormal[] = "Normal";
const char kNormalGray[] = "Normal.Gray";
const char kRGB[] = "RGB";
const char kRGBA[] = "RGBA";
const char kRGB16[] = "RGB16";

// Default margin settings.
const double kTopMarginInInch = 0.25;
const double kBottomMarginInInch = 0.56;
const double kLeftMarginInInch = 0.25;
const double kRightMarginInInch = 0.25;

// Length of an inch in CSS's 1px unit.
// http://dev.w3.org/csswg/css3-values/#the-px-unit
const int kPixelsPerInch = 96;

// LETTER: 8.5 x 11 inches
const float kLetterWidthInch = 8.5f;
const float kLetterHeightInch = 11.0f;

class StickyPrintSettingGtk {
 public:
  StickyPrintSettingGtk() : last_used_settings_(gtk_print_settings_new()) {
  }
  ~StickyPrintSettingGtk() {
    NOTREACHED();  // The instance is intentionally leaked.
  }

  GtkPrintSettings* settings() {
    return last_used_settings_;
  }

  void SetLastUsedSettings(GtkPrintSettings* settings) {
    DCHECK(last_used_settings_);
    g_object_unref(last_used_settings_);
    last_used_settings_ = gtk_print_settings_copy(settings);
  }

 private:
  GtkPrintSettings* last_used_settings_;

  DISALLOW_COPY_AND_ASSIGN(StickyPrintSettingGtk);
};

// Lazily initialize the singleton instance.
StickyPrintSettingGtk* GetLastUsedSettings() {
  static StickyPrintSettingGtk* settings = NULL;
  if (!settings)
    settings = new StickyPrintSettingGtk();
  return settings;
}

// Helper class to track GTK printers.
class GtkPrinterList {
 public:
  GtkPrinterList() : default_printer_(NULL) {
    gtk_enumerate_printers(SetPrinter, this, NULL, TRUE);
  }

  ~GtkPrinterList() {
    for (std::vector<GtkPrinter*>::iterator it = printers_.begin();
         it < printers_.end(); ++it) {
      g_object_unref(*it);
    }
  }

  // Can return NULL if there's no default printer. E.g. Printer on a laptop
  // is "home_printer", but the laptop is at work.
  GtkPrinter* default_printer() {
    return default_printer_;
  }

  // Can return NULL if the printer cannot be found due to:
  // - Printer list out of sync with printer dialog UI.
  // - Querying for non-existant printers like 'Print to PDF'.
  GtkPrinter* GetPrinterWithName(const std::string& name) {
    if (name.empty())
      return NULL;

    for (std::vector<GtkPrinter*>::iterator it = printers_.begin();
         it < printers_.end(); ++it) {
      if (gtk_printer_get_name(*it) == name) {
        return *it;
      }
    }

    return NULL;
  }

 private:
  // Callback function used by gtk_enumerate_printers() to get all printer.
  static gboolean SetPrinter(GtkPrinter* printer, gpointer data) {
    GtkPrinterList* printer_list = reinterpret_cast<GtkPrinterList*>(data);
    if (gtk_printer_is_default(printer))
      printer_list->default_printer_ = printer;

    g_object_ref(printer);
    printer_list->printers_.push_back(printer);

    return FALSE;
  }

  std::vector<GtkPrinter*> printers_;
  GtkPrinter* default_printer_;
};

void GetColorModelForMode(CefPrintSettings::ColorModel color_mode,
                          std::string* color_setting_name,
                          std::string* color_value) {
  color_setting_name->assign(kCUPSColorModel);
  switch (color_mode) {
    case COLOR_MODEL_COLOR:
      color_value->assign(kColor);
      break;
    case COLOR_MODEL_CMYK:
      color_value->assign(kCMYK);
      break;
    case COLOR_MODEL_PRINTOUTMODE_NORMAL:
      color_value->assign(kNormal);
      color_setting_name->assign(kCUPSPrintoutMode);
      break;
    case COLOR_MODEL_PRINTOUTMODE_NORMAL_GRAY:
      color_value->assign(kNormalGray);
      color_setting_name->assign(kCUPSPrintoutMode);
      break;
    case COLOR_MODEL_RGB16:
      color_value->assign(kRGB16);
      break;
    case COLOR_MODEL_RGBA:
      color_value->assign(kRGBA);
      break;
    case COLOR_MODEL_RGB:
      color_value->assign(kRGB);
      break;
    case COLOR_MODEL_CMY:
      color_value->assign(kCMY);
      break;
    case COLOR_MODEL_CMY_K:
      color_value->assign(kCMY_K);
      break;
    case COLOR_MODEL_BLACK:
      color_value->assign(kBlack);
      break;
    case COLOR_MODEL_GRAY:
      color_value->assign(kGray);
      break;
    case COLOR_MODEL_COLORMODE_COLOR:
      color_setting_name->assign(kCUPSColorMode);
      color_value->assign(kColor);
      break;
    case COLOR_MODEL_COLORMODE_MONOCHROME:
      color_setting_name->assign(kCUPSColorMode);
      color_value->assign(kMonochrome);
      break;
    case COLOR_MODEL_HP_COLOR_COLOR:
      color_setting_name->assign(kColor);
      color_value->assign(kColor);
      break;
    case COLOR_MODEL_HP_COLOR_BLACK:
      color_setting_name->assign(kColor);
      color_value->assign(kBlack);
      break;
    case COLOR_MODEL_PROCESSCOLORMODEL_CMYK:
      color_setting_name->assign(kCUPSProcessColorModel);
      color_value->assign(kCMYK);
      break;
    case COLOR_MODEL_PROCESSCOLORMODEL_GREYSCALE:
      color_setting_name->assign(kCUPSProcessColorModel);
      color_value->assign(kGreyscale);
      break;
    case COLOR_MODEL_PROCESSCOLORMODEL_RGB:
      color_setting_name->assign(kCUPSProcessColorModel);
      color_value->assign(kRGB);
      break;
    default:
      color_value->assign(kGrayscale);
      break;
  }
}

void InitPrintSettings(GtkPrintSettings* settings,
                       GtkPageSetup* page_setup,
                       CefRefPtr<CefPrintSettings> print_settings) {
  DCHECK(settings);
  DCHECK(page_setup);

  std::string device_name;
  const gchar* name = gtk_print_settings_get_printer(settings);
  if (name)
    device_name = name;
  print_settings->SetDeviceName(device_name);

  CefSize physical_size_device_units;
  CefRect printable_area_device_units;
  int dpi = gtk_print_settings_get_resolution(settings);
  if (dpi) {
    // Initialize page_setup_device_units_.
    physical_size_device_units.Set(
        gtk_page_setup_get_paper_width(page_setup, GTK_UNIT_INCH) * dpi,
        gtk_page_setup_get_paper_height(page_setup, GTK_UNIT_INCH) * dpi);
    printable_area_device_units.Set(
        gtk_page_setup_get_left_margin(page_setup, GTK_UNIT_INCH) * dpi,
        gtk_page_setup_get_top_margin(page_setup, GTK_UNIT_INCH) * dpi,
        gtk_page_setup_get_page_width(page_setup, GTK_UNIT_INCH) * dpi,
        gtk_page_setup_get_page_height(page_setup, GTK_UNIT_INCH) * dpi);
  } else {
    // Use default values if we cannot get valid values from the print dialog.
    dpi = kPixelsPerInch;
    double page_width_in_pixel = kLetterWidthInch * dpi;
    double page_height_in_pixel = kLetterHeightInch * dpi;
    physical_size_device_units.Set(
        static_cast<int>(page_width_in_pixel),
        static_cast<int>(page_height_in_pixel));
    printable_area_device_units.Set(
        static_cast<int>(kLeftMarginInInch * dpi),
        static_cast<int>(kTopMarginInInch * dpi),
        page_width_in_pixel - (kLeftMarginInInch + kRightMarginInInch) * dpi,
        page_height_in_pixel - (kTopMarginInInch + kBottomMarginInInch) * dpi);
  }

  print_settings->SetDPI(dpi);

  // Note: With the normal GTK print dialog, when the user selects the landscape
  // orientation, all that does is change the paper size. Which seems to be
  // enough to render the right output and send it to the printer.
  // The orientation value stays as portrait and does not actually affect
  // printing.
  // Thus this is only useful in print preview mode, where we manually set the
  // orientation and change the paper size ourselves.
  GtkPageOrientation orientation = gtk_print_settings_get_orientation(settings);
  // Set before SetPrinterPrintableArea to make it flip area if necessary.
  print_settings->SetOrientation(orientation == GTK_PAGE_ORIENTATION_LANDSCAPE);
  print_settings->SetPrinterPrintableArea(physical_size_device_units,
                                          printable_area_device_units,
                                          true);
}

}  // namespace


ClientPrintHandlerGtk::ClientPrintHandlerGtk()
    : dialog_(NULL),
      gtk_settings_(NULL),
      page_setup_(NULL),
      printer_(NULL) {
}

void ClientPrintHandlerGtk::OnPrintStart(CefRefPtr<CefBrowser> browser) {
}

void ClientPrintHandlerGtk::OnPrintSettings(
    CefRefPtr<CefPrintSettings> settings,
    bool get_defaults) {
  if (get_defaults) {
    DCHECK(!page_setup_);
    DCHECK(!printer_);

    // |gtk_settings_| is a new copy.
    gtk_settings_ =
        gtk_print_settings_copy(GetLastUsedSettings()->settings());
    page_setup_ = gtk_page_setup_new();
  } else {
    if (!gtk_settings_) {
      gtk_settings_ =
          gtk_print_settings_copy(GetLastUsedSettings()->settings());
    }

    GtkPrinterList* printer_list = new GtkPrinterList;
    printer_ = printer_list->GetPrinterWithName(settings->GetDeviceName());
    if (printer_) {
      g_object_ref(printer_);
      gtk_print_settings_set_printer(gtk_settings_,
                                     gtk_printer_get_name(printer_));
      if (!page_setup_) {
        page_setup_ = gtk_printer_get_default_page_size(printer_);
      }
    }

    gtk_print_settings_set_n_copies(gtk_settings_, settings->GetCopies());
    gtk_print_settings_set_collate(gtk_settings_, settings->WillCollate());

    std::string color_value;
    std::string color_setting_name;
    GetColorModelForMode(settings->GetColorModel(), &color_setting_name,
                         &color_value);
    gtk_print_settings_set(gtk_settings_, color_setting_name.c_str(),
                           color_value.c_str());

    if (settings->GetDuplexMode() != DUPLEX_MODE_UNKNOWN) {
      const char* cups_duplex_mode = NULL;
      switch (settings->GetDuplexMode()) {
        case DUPLEX_MODE_LONG_EDGE:
          cups_duplex_mode = kDuplexNoTumble;
          break;
        case DUPLEX_MODE_SHORT_EDGE:
          cups_duplex_mode = kDuplexTumble;
          break;
        case DUPLEX_MODE_SIMPLEX:
          cups_duplex_mode = kDuplexNone;
          break;
        default:  // UNKNOWN_DUPLEX_MODE
          NOTREACHED();
          break;
      }
      gtk_print_settings_set(gtk_settings_, kCUPSDuplex, cups_duplex_mode);
    }

    if (!page_setup_)
      page_setup_ = gtk_page_setup_new();

    gtk_print_settings_set_orientation(
        gtk_settings_,
        settings->IsLandscape() ? GTK_PAGE_ORIENTATION_LANDSCAPE :
                                  GTK_PAGE_ORIENTATION_PORTRAIT);

    delete printer_list;
  }

  InitPrintSettings(gtk_settings_, page_setup_, settings);
}

bool ClientPrintHandlerGtk::OnPrintDialog(
    bool has_selection,
    CefRefPtr<CefPrintDialogCallback> callback) {
  dialog_callback_ = callback;

  // TODO(cef): Identify the correct parent window.
  GtkWindow* parent = NULL;
  // TODO(estade): We need a window title here.
  dialog_ = gtk_print_unix_dialog_new(NULL, parent);
  g_signal_connect(dialog_, "delete-event",
                   G_CALLBACK(gtk_widget_hide_on_delete), NULL);


  // Set modal so user cannot focus the same tab and press print again.
  gtk_window_set_modal(GTK_WINDOW(dialog_), TRUE);

  // Since we only generate PDF, only show printers that support PDF.
  // TODO(thestig) Add more capabilities to support?
  GtkPrintCapabilities cap = static_cast<GtkPrintCapabilities>(
      GTK_PRINT_CAPABILITY_GENERATE_PDF |
      GTK_PRINT_CAPABILITY_PAGE_SET |
      GTK_PRINT_CAPABILITY_COPIES |
      GTK_PRINT_CAPABILITY_COLLATE |
      GTK_PRINT_CAPABILITY_REVERSE);
  gtk_print_unix_dialog_set_manual_capabilities(GTK_PRINT_UNIX_DIALOG(dialog_),
                                                cap);
  gtk_print_unix_dialog_set_embed_page_setup(GTK_PRINT_UNIX_DIALOG(dialog_),
                                             TRUE);
  gtk_print_unix_dialog_set_support_selection(GTK_PRINT_UNIX_DIALOG(dialog_),
                                              TRUE);
  gtk_print_unix_dialog_set_has_selection(GTK_PRINT_UNIX_DIALOG(dialog_),
                                          has_selection);
  gtk_print_unix_dialog_set_settings(GTK_PRINT_UNIX_DIALOG(dialog_),
                                     gtk_settings_);
  g_signal_connect(dialog_, "response", G_CALLBACK(OnDialogResponseThunk),
                   this);
  gtk_widget_show(dialog_);

  return true;
}

bool ClientPrintHandlerGtk::OnPrintJob(
    const CefString& document_name,
    const CefString& pdf_file_path,
    CefRefPtr<CefPrintJobCallback> callback) {
  // If |printer_| is NULL then somehow the GTK printer list changed out under
  // us. In which case, just bail out.
  if (!printer_)
    return false;

  job_callback_ = callback;

  // Save the settings for next time.
  GetLastUsedSettings()->SetLastUsedSettings(gtk_settings_);

  GtkPrintJob* print_job = gtk_print_job_new(
      document_name.ToString().c_str(),
      printer_,
      gtk_settings_,
      page_setup_);
  gtk_print_job_set_source_file(print_job,
                                pdf_file_path.ToString().c_str(),
                                NULL);
  gtk_print_job_send(print_job, OnJobCompletedThunk, this, NULL);

  return true;
}

void ClientPrintHandlerGtk::OnPrintReset() {
  if (dialog_) {
    gtk_widget_destroy(dialog_);
    dialog_ = NULL;
  }
  if (gtk_settings_) {
    g_object_unref(gtk_settings_);
    gtk_settings_ = NULL;
  }
  if (page_setup_) {
    g_object_unref(page_setup_);
    page_setup_ = NULL;
  }
  if (printer_) {
    g_object_unref(printer_);
    printer_ = NULL;
  }
}

CefSize ClientPrintHandlerGtk::GetPdfPaperSize(int device_units_per_inch) {
  GtkPageSetup* page_setup = gtk_page_setup_new();

  float width = gtk_page_setup_get_paper_width(page_setup, GTK_UNIT_INCH);
  float height = gtk_page_setup_get_paper_height(page_setup, GTK_UNIT_INCH);

  g_object_unref(page_setup);

  return CefSize(width * device_units_per_inch, height * device_units_per_inch);
}

void ClientPrintHandlerGtk::OnDialogResponse(GtkDialog *dialog,
                                             gint response_id) {
  int num_matched_handlers = g_signal_handlers_disconnect_by_func(
      dialog_, reinterpret_cast<gpointer>(&OnDialogResponseThunk), this);
  DCHECK_EQ(1, num_matched_handlers);

  gtk_widget_hide(dialog_);

  switch (response_id) {
    case GTK_RESPONSE_OK: {
      if (gtk_settings_)
        g_object_unref(gtk_settings_);
      gtk_settings_ = gtk_print_unix_dialog_get_settings(
          GTK_PRINT_UNIX_DIALOG(dialog_));

      if (printer_)
        g_object_unref(printer_);
      printer_ = gtk_print_unix_dialog_get_selected_printer(
          GTK_PRINT_UNIX_DIALOG(dialog_));
      g_object_ref(printer_);

      if (page_setup_)
        g_object_unref(page_setup_);
      page_setup_ = gtk_print_unix_dialog_get_page_setup(
          GTK_PRINT_UNIX_DIALOG(dialog_));
      g_object_ref(page_setup_);

      // Handle page ranges.
      CefPrintSettings::PageRangeList ranges_vector;
      gint num_ranges;
      bool print_selection_only = false;
      switch (gtk_print_settings_get_print_pages(gtk_settings_)) {
        case GTK_PRINT_PAGES_RANGES: {
          GtkPageRange* gtk_range =
              gtk_print_settings_get_page_ranges(gtk_settings_, &num_ranges);
          if (gtk_range) {
            for (int i = 0; i < num_ranges; ++i) {
              ranges_vector.push_back(
                  CefPageRange(gtk_range[i].start, gtk_range[i].end));
            }
            g_free(gtk_range);
          }
          break;
        }
        case GTK_PRINT_PAGES_SELECTION:
          print_selection_only = true;
          break;
        case GTK_PRINT_PAGES_ALL:
          // Leave |ranges_vector| empty to indicate print all pages.
          break;
        case GTK_PRINT_PAGES_CURRENT:
        default:
          NOTREACHED();
          break;
      }

      CefRefPtr<CefPrintSettings> settings = CefPrintSettings::Create();
      settings->SetPageRanges(ranges_vector);
      settings->SetSelectionOnly(print_selection_only);
      InitPrintSettings(gtk_settings_, page_setup_, settings);
      dialog_callback_->Continue(settings);
      dialog_callback_ = NULL;
      return;
    }
    case GTK_RESPONSE_DELETE_EVENT:  // Fall through.
    case GTK_RESPONSE_CANCEL: {
      dialog_callback_->Cancel();
      dialog_callback_ = NULL;
      return;
    }
    case GTK_RESPONSE_APPLY:
    default: {
      NOTREACHED();
    }
  }
}

void ClientPrintHandlerGtk::OnJobCompleted(GtkPrintJob* print_job,
                                           GError* error) {
  job_callback_->Continue();
  job_callback_ = NULL;
}

}  // namespace client
