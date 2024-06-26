
#include "printing/PdfiumLoad.h"
#include "common/StringUtil.h"
#include <xstring>

namespace printing {

PdfiumLoad* PdfiumLoad::m_inst;

PdfiumLoad::PdfiumLoad()
{
    m_pRenderPDFPageToDC = nullptr;
    m_pGetPDFPageSizeByIndex = nullptr;
    m_pGetPDFDocInfo = nullptr;
}

PdfiumLoad* PdfiumLoad::get()
{
    if (m_inst)
        return m_inst;

    //HMODULE hMod = LoadLibraryW(L"D:\\Program Files (x86)\\Tencent\\WeChat\\pdf.dll");
    //HMODULE hMod = LoadLibraryW(L"E:\\mycode\\chromium.bb-master\\src\\out\\Debug\\blppdfutil.dll");
    //HMODULE hMod = ::LoadLibraryW(L"E:\\mycode\\miniblink49\\trunk\\out\\Debug\\pdfiumall.dll");

    std::wstring path = common::getPluginDirectory();
#ifdef _WIN64
    path += L"pdfium_x64.dll";
#else
    path += L"pdfium.dll";
#endif
    HMODULE hMod = ::LoadLibraryW(path.c_str());
    if (!hMod) {
        OutputDebugStringW(L"不能打开：");
        OutputDebugStringW(path.c_str());
        OutputDebugStringW(L"\n");
        return nullptr;
    }

    m_inst = new PdfiumLoad();

    m_inst->m_pRenderPDFPageToDC = (PN_RenderPDFPageToDC)::GetProcAddress(hMod, "RenderPDFPageToDC");
    m_inst->m_pGetPDFPageSizeByIndex = (PN_GetPDFPageSizeByIndex)::GetProcAddress(hMod, "GetPDFPageSizeByIndex");
    m_inst->m_pGetPDFDocInfo = (PN_GetPDFDocInfo)::GetProcAddress(hMod, "GetPDFDocInfo");
    m_inst->m_pFreeMem = (PN_FreeMem)::GetProcAddress(hMod, "FreeMem");
    return m_inst;
}

BOOL PdfiumLoad::renderPDFPageToDC(const void* pdf_buffer,
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
    return m_pRenderPDFPageToDC(pdf_buffer,
        buffer_size,
        page_number,
        dc,
        dpi,
        bounds_origin_x,
        bounds_origin_y,
        bounds_width,
        bounds_height,
        fit_to_bounds,
        stretch_to_bounds,
        keep_aspect_ratio,
        center_in_bounds,
        autorotate);
}

BOOL PdfiumLoad::getPDFPageSizeByIndex(
    const void* pdf_buffer,
    int pdf_buffer_size,
    int page_number,
    double* width,
    double* height)
{
    return m_pGetPDFPageSizeByIndex(pdf_buffer, pdf_buffer_size, page_number, width, height);
}

BOOL PdfiumLoad::getPDFDocInfo(const void* pdfBuffer,
    int bufferSize,
    int* pageCount,
    double* maxPageWidth,
    wchar_t** bookmarkJson)
{
    return m_pGetPDFDocInfo(pdfBuffer, bufferSize, pageCount, maxPageWidth, bookmarkJson);
}

void PdfiumLoad::freeMem(const void* buffer)
{
    return m_pFreeMem(buffer);
}

}
