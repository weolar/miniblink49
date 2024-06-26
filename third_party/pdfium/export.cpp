
#include "third_party/pdfium/public/fpdfview.h"
#include "third_party/pdfium/public/fpdf_ext.h"
#include "third_party/pdfium/public/fpdf_doc.h"

#include <windows.h>
#include <vector>
#include <xstring>

_ACRTIMP_ALT FILE* __cdecl __acrt_iob_func(unsigned);

extern "C" FILE * __cdecl __iob_func(unsigned i)
{
    return __acrt_iob_func(i);
}

int __except_handler4_common = 0;

class Rect {
public:
    Rect(int x, int y, int width, int height)
    {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
    }

    Rect()
    {
        m_x = 0;
        m_y = 0;
        m_width = 0;
        m_height = 0;
    }

    void set_width(int width)
    {
        m_width = width;
    }

    void set_height(int height)
    {
        m_height = height;
    }

    void Offset(int horizontal, int vertical)
    {
        m_x += horizontal;
        m_y += vertical;
    }

    int width() const
    {
        return m_width;
    }

    int height() const
    {
        return m_height;
    }

    int x() const
    {
        return m_x;
    }

    int y() const
    {
        return m_y;
    }

private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
};

struct RenderingSettings {
    RenderingSettings(int dpi_x,
        int dpi_y,
        const Rect& bounds,
        bool fit_to_bounds,
        bool stretch_to_bounds,
        bool keep_aspect_ratio,
        bool center_in_bounds,
        bool autorotate)
        : dpi_x(dpi_x), dpi_y(dpi_y), bounds(bounds),
        fit_to_bounds(fit_to_bounds), stretch_to_bounds(stretch_to_bounds),
        keep_aspect_ratio(keep_aspect_ratio),
        center_in_bounds(center_in_bounds), autorotate(autorotate)
    {
    }
    int dpi_x;
    int dpi_y;
    Rect bounds;
    bool fit_to_bounds;
    bool stretch_to_bounds;
    bool keep_aspect_ratio;
    bool center_in_bounds;
    bool autorotate;
};

double ConvertUnitDouble(double value, double old_unit, double new_unit)
{
    return value * new_unit / old_unit;
}

const int kPointsPerInch = 72;

// Length of an inch in CSS's 1px unit.
// http://dev.w3.org/csswg/css3-values/#the-px-unit
const int kPixelsPerInch = 96;

int CalculatePosition(FPDF_PAGE page, RenderingSettings& settings, Rect* dest)
{
    int page_width = static_cast<int>(ConvertUnitDouble(FPDF_GetPageWidth(page), kPointsPerInch, settings.dpi_x));
    int page_height = static_cast<int>(ConvertUnitDouble(FPDF_GetPageHeight(page), kPointsPerInch, settings.dpi_y));

    // Start by assuming that we will draw exactly to the bounds rect
    // specified.
    *dest = settings.bounds;

    int rotate = 0;  // normal orientation.

    // Auto-rotate landscape pages to print correctly.
    if (settings.autorotate && (dest->width() > dest->height()) != (page_width > page_height)) {
        rotate = 3;  // 90 degrees counter-clockwise.
        //std::swap(page_width, page_height);
        int tempVal = page_width;
        page_width = page_height;
        page_height = tempVal;
    }

    // See if we need to scale the output
    bool scale_to_bounds = false;
    if (settings.fit_to_bounds && ((page_width > dest->width()) || (page_height > dest->height()))) {
        scale_to_bounds = true;
    } else if (settings.stretch_to_bounds && ((page_width < dest->width()) || (page_height < dest->height()))) {
        scale_to_bounds = true;
    }

    if (scale_to_bounds) {
        // If we need to maintain aspect ratio, calculate the actual width and
        // height.
        if (settings.keep_aspect_ratio) {
            double scale_factor_x = page_width;
            scale_factor_x /= dest->width();
            double scale_factor_y = page_height;
            scale_factor_y /= dest->height();
            if (scale_factor_x > scale_factor_y) {
                dest->set_height(page_height / scale_factor_x);
            } else {
                dest->set_width(page_width / scale_factor_y);
            }
        }
    } else {
        // We are not scaling to bounds. Draw in the actual page size. If the
        // actual page size is larger than the bounds, the output will be
        // clipped.
        dest->set_width(page_width);
        dest->set_height(page_height);
    }

    if (settings.center_in_bounds) {
        dest->Offset((settings.bounds.width() - dest->width()) / 2, (settings.bounds.height() - dest->height()) / 2);
    }
    return rotate;
}

BOOL RenderPDFPageToDC(const void* pdf_buffer,
    int buffer_size,
    int page_number,
    const RenderingSettings& settings,
    HDC dc)
{
    FPDF_DOCUMENT doc = FPDF_LoadMemDocument(pdf_buffer, buffer_size, NULL);
    if (!doc)
        return false;
    FPDF_PAGE page = FPDF_LoadPage(doc, page_number);
    if (!page) {
        FPDF_CloseDocument(doc);
        return false;
    }
    RenderingSettings new_settings = settings;
    // calculate the page size
    if (new_settings.dpi_x == -1)
        new_settings.dpi_x = GetDeviceCaps(dc, LOGPIXELSX);
    if (new_settings.dpi_y == -1)
        new_settings.dpi_y = GetDeviceCaps(dc, LOGPIXELSY);

    Rect dest;
    int rotate = CalculatePosition(page, new_settings, &dest);

    int save_state = SaveDC(dc);
    // The caller wanted all drawing to happen within the bounds specified.
    // Based on scale calculations, our destination rect might be larger
    // than the bounds. Set the clip rect to the bounds.
    IntersectClipRect(dc, settings.bounds.x(), settings.bounds.y(),
        settings.bounds.x() + settings.bounds.width(),
        settings.bounds.y() + settings.bounds.height());

    // A temporary hack. PDFs generated by Cairo (used by Chrome OS to generate
    // a PDF output from a webpage) result in very large metafiles and the
    // rendering using FPDF_RenderPage is incorrect. In this case, render as a
    // bitmap. Note that this code does not kick in for PDFs printed from Chrome
    // because in that case we create a temp PDF first before printing and this
    // temp PDF does not have a creator string that starts with "cairo".
    bool use_bitmap = false;
//     if (base::StartsWith(GetDocumentMetadata(doc, "Creator"), "cairo",
//         base::CompareCase::INSENSITIVE_ASCII)) {
//         use_bitmap = true;
//     }

    // Another temporary hack. Some PDFs seems to render very slowly if
    // FPDF_RenderPage is directly used on a printer DC. I suspect it is
    // because of the code to talk Postscript directly to the printer if
    // the printer supports this. Need to discuss this with PDFium. For now,
    // render to a bitmap and then blit the bitmap to the DC if we have been
    // supplied a printer DC.
    int device_type = GetDeviceCaps(dc, TECHNOLOGY);
    if (false && (use_bitmap || (device_type == DT_RASPRINTER) || (device_type == DT_PLOTTER))) {
        FPDF_BITMAP bitmap = FPDFBitmap_Create(dest.width(), dest.height(), FPDFBitmap_BGRx);
        // Clear the bitmap
        FPDFBitmap_FillRect(bitmap, 0, 0, dest.width(), dest.height(), 0xFFFFFFFF);
        FPDF_RenderPageBitmap(
            bitmap, page, 0, 0, dest.width(), dest.height(), rotate,
            FPDF_ANNOT | FPDF_PRINTING | FPDF_NO_CATCH);
        int stride = FPDFBitmap_GetStride(bitmap);
        BITMAPINFO bmi;
        memset(&bmi, 0, sizeof(bmi));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = dest.width();
        bmi.bmiHeader.biHeight = -dest.height();  // top-down image
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = stride * dest.height();
        StretchDIBits(dc, dest.x(), dest.y(), dest.width(), dest.height(),
            0, 0, dest.width(), dest.height(),
            FPDFBitmap_GetBuffer(bitmap), &bmi, DIB_RGB_COLORS, SRCCOPY);
        FPDFBitmap_Destroy(bitmap);
    } else {
        FPDF_RenderPage(dc, page, dest.x(), dest.y(), dest.width(), dest.height(), rotate, FPDF_ANNOT | FPDF_PRINTING | FPDF_NO_CATCH);
    }
    RestoreDC(dc, save_state);
    FPDF_ClosePage(page);
    FPDF_CloseDocument(doc);
    return true;
}

void Unsupported_Handler(UNSUPPORT_INFO*, int type)
{

}

UNSUPPORT_INFO g_unsupported_info = {
    1,
    Unsupported_Handler
};

static bool g_sdk_initialized_via_pepper = false;

bool InitializeSDK()
{
    if (g_sdk_initialized_via_pepper)
        return true;

    FPDF_LIBRARY_CONFIG config;
    config.version = 2;
    config.m_pUserFontPaths = nullptr;
    config.m_pIsolate = nullptr;
    config.m_v8EmbedderSlot = 0;
    FPDF_InitLibraryWithConfig(&config);

// #if defined(OS_LINUX)
//     // Font loading doesn't work in the renderer sandbox in Linux.
//     FPDF_SetSystemFontInfo(&g_font_info);
// #endif

    FSDK_SetUnSpObjProcessHandler(&g_unsupported_info);

    g_sdk_initialized_via_pepper = true;

    return true;
}

extern "C" __declspec(dllexport) BOOL GetPDFPageSizeByIndex(
    const void* pdf_buffer,
    int pdf_buffer_size,
    int page_number,
    double* width,
    double* height)
{
    if (!InitializeSDK())
        return FALSE;

    FPDF_DOCUMENT doc = FPDF_LoadMemDocument(pdf_buffer, pdf_buffer_size, NULL);
    if (!doc)
        return FALSE;
    BOOL success = FPDF_GetPageSizeByIndex(doc, page_number, width, height) != 0;
    FPDF_CloseDocument(doc);
    return success;
}

// {
//      "title":"xxx",
//      "action" : {"type":"goto", "pageIndex" : 1}
//      "children" : [
//      {
//         "title":"xxx",
//          "action" : {"type":"goto", "pageIndex" : 1}
//         "children" : []
//      },
//      {
//         "title":"xxx",
//         "action" : {"type":"goto", "pageIndex" : 1}
//         "children" : []
//      },
//      ]
// }

static std::wstring createOneBookmarkItemJson(FPDF_DOCUMENT doc, FPDF_BOOKMARK bookmark);

static std::wstring createBookmarkItemsJson(FPDF_DOCUMENT doc, FPDF_BOOKMARK bookmark)
{
    std::wstring item = createOneBookmarkItemJson(doc, bookmark);

    FPDF_BOOKMARK next = FPDFBookmark_GetNextSibling(doc, bookmark);
    while (next) {
        item += L",\n";
        item += createOneBookmarkItemJson(doc, next);

        next = FPDFBookmark_GetNextSibling(doc, next);
    };

    return item;
}

static std::wstring urlencode(const std::wstring& text)
{
    std::wstring result;
    for (size_t i = 0; i < text.size(); ++i) {
        wchar_t c = text[i];
        wchar_t temp[30];
        wsprintf(temp, L"%%%02x%%%02x", (c & 0xff), (c >> 8));
        result += temp;
    }
    return result;
}

static std::wstring createOneBookmarkItemJson(FPDF_DOCUMENT doc, FPDF_BOOKMARK bookmark)
{
    std::vector<char> buf;
    buf.resize(2002);
    memset(&buf[0], 0, 2002);
    FPDFBookmark_GetTitle(bookmark, &buf[0], 2000);

    unsigned long pageIndex = -1;

    FPDF_ACTION action = FPDFBookmark_GetAction(bookmark);
    unsigned long actionType = FPDFAction_GetType(action);
    if (PDFACTION_GOTO == actionType) {
        FPDF_DEST dest = FPDFAction_GetDest(doc, action);
        //pageIndex = FPDFDest_GetPageIndex(doc, dest);
        DebugBreak();
    }

    std::wstring title((const wchar_t*)&buf.at(0));
    //std::wstring title(L"aabb");
    if (title.size() > 20)
        title = title.substr(0, 20);

    title = urlencode(title);

    std::wstring json = L"{\n";
    json += L"   'title':'";
    json += title;
    json += L"',\n";
    json += L"   'action':{";
    if (-1 != pageIndex) {
        wchar_t temp[0x20] = { 0 };
        wsprintf(temp, L"'pageIndex':%d", pageIndex);
        json += temp;
    }
    json += L"},\n";

    FPDF_BOOKMARK childBookmark = FPDFBookmark_GetFirstChild(doc, bookmark);
    if (childBookmark) {
        std::wstring childJson = createBookmarkItemsJson(doc, childBookmark);
        json += L"  'children':[";
        json += childJson;
        json += L"]\n";
    }

    json += L"}\n";

    return json;
}

static std::wstring createBookmarkJson(FPDF_DOCUMENT doc)
{
    FPDF_BOOKMARK bookmark = FPDFBookmark_GetFirstChild(doc, nullptr);
    if (!bookmark)
        return L"";
    std::wstring result = L"[";
    result += createBookmarkItemsJson(doc, bookmark);
    result += L"]";
    return result;
}

extern "C" __declspec(dllexport) void FreeMem(void* buffer)
{
    free(buffer);
}

extern "C" __declspec(dllexport) BOOL GetPDFDocInfo(const void* pdf_buffer,
    int buffer_size,
    int* page_count,
    double* max_page_width,
    wchar_t** bookmarkJson)
{
    if (!InitializeSDK())
        return FALSE;

    FPDF_DOCUMENT doc = FPDF_LoadMemDocument(pdf_buffer, buffer_size, NULL);
    if (!doc)
        return FALSE;

    if (bookmarkJson) {
        std::wstring json = createBookmarkJson(doc);
        if (0 != json.size()) {
            wchar_t* buf = (wchar_t*)malloc((1 + json.size()) * sizeof(wchar_t));
            memset(buf, 0, (1 + json.size()) * sizeof(wchar_t));
            memcpy(buf, json.c_str(), (json.size()) * sizeof(wchar_t));
            *bookmarkJson = buf;
        }
    }

    int page_count_local = FPDF_GetPageCount(doc);
    if (page_count) {
        *page_count = page_count_local;
    }
    if (max_page_width) {
        *max_page_width = 0;
        for (int page_number = 0; page_number < page_count_local; page_number++) {
            double page_width = 0;
            double page_height = 0;
            FPDF_GetPageSizeByIndex(doc, page_number, &page_width, &page_height);
            if (page_width > *max_page_width) {
                *max_page_width = page_width;
            }
        }
    }
    FPDF_CloseDocument(doc);
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL RenderPDFPageToDC(
    const void* pdf_buffer,
    int buffer_size,
    int page_number,
    HDC dc,
    int dpi,
    int bounds_origin_x,
    int bounds_origin_y,
    int bounds_width,
    int bounds_height,
    bool fit_to_bounds,
    bool stretch_to_bounds,
    bool keep_aspect_ratio,
    bool center_in_bounds,
    bool autorotate)
{
    if (!g_sdk_initialized_via_pepper) {
        if (!InitializeSDK())
            return FALSE;
        g_sdk_initialized_via_pepper = true;
    }

    RenderingSettings settings(dpi, dpi, Rect(bounds_origin_x, bounds_origin_y, bounds_width, bounds_height),
        fit_to_bounds, stretch_to_bounds, keep_aspect_ratio, center_in_bounds,
        autorotate);
    BOOL ret = RenderPDFPageToDC(pdf_buffer, buffer_size, page_number, settings, dc);
    return ret;
}
