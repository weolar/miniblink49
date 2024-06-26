
#ifndef printing_PdfiumLoad_h
#define printing_PdfiumLoad_h

#include <windows.h>

namespace printing {

typedef BOOL(__cdecl *PN_RenderPDFPageToDC)(const void* pdf_buffer,
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
    bool autorotate);

typedef BOOL(__cdecl *PN_GetPDFPageSizeByIndex)(
    const void* pdf_buffer,
    int pdf_buffer_size,
    int page_number,
    double* width,
    double* height);

typedef BOOL(__cdecl* PN_GetPDFDocInfo)(const void* pdfBuffer,
    int bufferSize,
    int* pageCount,
    double* maxPageWidth,
    wchar_t** bookmarkJson);

typedef void(__cdecl* PN_FreeMem)(const void* buffer);

class PdfiumLoad {
public:
    static PdfiumLoad* get();

    BOOL renderPDFPageToDC(const void* pdf_buffer,
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
        bool autorotate);

    BOOL getPDFPageSizeByIndex(
        const void* pdf_buffer,
        int pdf_buffer_size,
        int page_number,
        double* width,
        double* height);

    BOOL getPDFDocInfo(const void* pdfBuffer,
        int bufferSize,
        int* pageCount,
        double* maxPageWidth,
        wchar_t** bookmarkJson);

    void freeMem(const void* buffer);

private:
    PdfiumLoad();
    static PdfiumLoad* m_inst;

    PN_RenderPDFPageToDC m_pRenderPDFPageToDC;
    PN_GetPDFPageSizeByIndex m_pGetPDFPageSizeByIndex;
    PN_GetPDFDocInfo m_pGetPDFDocInfo;
    PN_FreeMem m_pFreeMem;
};

}

#endif // printing_PdfiumLoad_h