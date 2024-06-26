
#include "core/mb.h"
#include "core/MbWebView.h"
#include "common/LiveIdDetect.h"
#include <xstring>

namespace printing {

struct FromInfo {
    DWORD flags;
    std::wstring name;
    SIZEL size;
    RECTL imageableArea;
    int paperType;
};

struct PrintSettings {
    int dpi;
    mbSize physicalSizeDeviceUnits;
    mbRect printableAreaDeviceUnits; // in points
    mbSize size; // 最终决定打印的大小，为像素单位
};

struct RenderSettings {
    int targetDpi;
    bool fitToBounds;
    bool stretchToBounds;
    bool keepAspectRatio;
    bool centerInBounds;
    bool autorotate;
};

inline bool isContains(const mbRect& a, const mbRect& b)
{
    return (b.x >= a.x && b.x + b.w <= a.x + a.w && b.y >= a.y && b.y + b.h <= a.y + a.h);
}

inline bool isContains(const mbSize& size, const mbRect& rect)
{
    mbRect r = { 0, 0, size.w, size.h };
    return isContains(r, rect);
}

inline bool isEmptyR(const mbRect& r)
{
    return r.h * r.w == 0;
}

inline bool isNum(char c)
{
    if ('1' <= c && '0' >= c)
        return true;
    return false;
}

// 1-5
inline bool parsePageCountParam(const std::string& printerParams, std::vector<std::pair<int, int>>* pagesCount)
{
    int numBegin = -1;
    int numEnd = -1;
    size_t pos = printerParams.find('-');
    if (std::string::npos == pos) {
        numBegin = atoi(printerParams.c_str());
        numEnd = numBegin;
        if (numBegin < 0)
            return false;

        pagesCount->push_back(std::pair<int, int>(numBegin, numEnd));
        return true;
    }
    std::string numBeginStr = printerParams.substr(0, pos);
    std::string numEndStr = printerParams.substr(pos + 1);

    if (numBeginStr.size() == 0)
        return false;
    numBegin = atoi(numBeginStr.c_str());

    if (numEndStr.size() == 0)
        return false;
    numEnd = atoi(numEndStr.c_str());

    if (numBegin < 0 || numEnd < 0)
        return false;

    if (numBegin >= numEnd)
        return false;

    pagesCount->push_back(std::pair<int, int>(numBegin, numEnd));
    return true;
}

// 1-5/6-20
inline bool parsePagesCountParams(const std::string& printerParams, std::vector<std::pair<int, int>>* pagesCount)
{
    for (size_t i = 0; i < printerParams.size(); ++i) {
        if (!isNum(printerParams[i]) || '-' == printerParams[i] || '/' == printerParams[i])
            continue;
        return false;
    }
    std::pair<int, int> onePair;

    size_t pos = 0;
    size_t tokenEnd = 0;
    do {
        tokenEnd = printerParams.find('/', pos);
        if (std::string::npos == tokenEnd) {
            tokenEnd = printerParams.size();
        }
        std::string token = printerParams.substr(pos, tokenEnd - pos);
        if (0 == token.size())
            return false;

        bool b = parsePageCountParam(token, pagesCount);
        if (!b)
            return false;

        pos = tokenEnd;
        pos++;
    } while (tokenEnd < printerParams.size());

    return true;
}

// 1-5/6-20|devname
inline bool parseDoPrintParams(const std::string& printerParams, std::vector<std::pair<int, int>>* pagesCount, std::string* devName, int* copies)
{
    size_t posCopies = printerParams.find('|');
    if (std::string::npos == posCopies)
        return false;

    std::string printCopiesText = printerParams.substr(0, posCopies);
    *copies = atoi(printCopiesText.c_str());
    if (*copies < 1 || *copies > 30)
        return false;
    ++posCopies;

    size_t pos = printerParams.find('|', posCopies);
    if (std::string::npos == posCopies)
        return false;

    bool b = parsePagesCountParams(printerParams.substr(posCopies, pos - posCopies), pagesCount);
    if (!b)
        return false;

    *devName = printerParams.substr(pos + 1);
    if (devName->size() == 0)
        return false;
    return true;
}

inline std::vector<int> createNeedPrintedPages(int pageNum, const std::vector<std::pair<int, int>>& pagesCount)
{
    std::vector<int> result;
    result.resize(pageNum);
    memset(&result[0], 0, sizeof(int) * pageNum);

    for (size_t i = 0; i < pagesCount.size(); ++i) {
        const std::pair<int, int>& onePair = pagesCount[i];
        if (onePair.first <= 0 || onePair.second <= 0 || onePair.first > onePair.second)
            continue;
        for (int j = onePair.first; j <= onePair.second && j <= pageNum; ++j) {
            result[j - 1] = 1;
        }
    }
    return result;
}

inline void initializeDC(HDC context)
{
    // Enables world transformation.
    // If the GM_ADVANCED graphics mode is set, GDI always draws arcs in the
    // counterclockwise direction in logical space. This is equivalent to the
    // statement that, in the GM_ADVANCED graphics mode, both arc control points
    // and arcs themselves fully respect the device context's world-to-device
    // transformation.
    BOOL res = SetGraphicsMode(context, GM_ADVANCED);
    //SkASSERT(res != 0);

    // Enables dithering.
    res = SetStretchBltMode(context, HALFTONE);
    //SkASSERT(res != 0);
    // As per SetStretchBltMode() documentation, SetBrushOrgEx() must be called
    // right after.
    res = SetBrushOrgEx(context, 0, 0, NULL);
    //SkASSERT(res != 0);

    // Sets up default orientation.
    res = SetArcDirection(context, AD_CLOCKWISE);
    //SkASSERT(res != 0);

    // Sets up default colors.
    res = SetBkColor(context, RGB(255, 255, 255));
    //SkASSERT(res != CLR_INVALID);
    res = SetTextColor(context, RGB(0, 0, 0));
    //SkASSERT(res != CLR_INVALID);
    res = SetDCBrushColor(context, RGB(255, 255, 255));
    //SkASSERT(res != CLR_INVALID);
    res = SetDCPenColor(context, RGB(0, 0, 0));
    //SkASSERT(res != CLR_INVALID);

    // Sets up default transparency.
    res = SetBkMode(context, OPAQUE);
    //SkASSERT(res != 0);
    res = SetROP2(context, R2_COPYPEN);
    //SkASSERT(res != 0);
}

inline void drawPage(HDC hdc, UINT Page)
{
    wchar_t line[150];
    int nWidth, nHeight;

    int offsetX = ::GetDeviceCaps(hdc, PHYSICALOFFSETX);
    int offsetY = ::GetDeviceCaps(hdc, PHYSICALOFFSETY);
    int dpiX = ::GetDeviceCaps(hdc, LOGPIXELSX);
    int dpiY = ::GetDeviceCaps(hdc, LOGPIXELSY);
    int physicalWidth = ::GetDeviceCaps(hdc, PHYSICALWIDTH);
    int physicalHeight = ::GetDeviceCaps(hdc, PHYSICALHEIGHT);

    nWidth = GetDeviceCaps(hdc, HORZRES);
    nHeight = GetDeviceCaps(hdc, VERTRES);

    SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(255, 0, 0)));
    Rectangle(hdc, 0, 0, nWidth - 4, nHeight - 2);
    wsprintf(line,
        L"Test WkePrinting: page#%u width x height (%dx%d), (%d,%d,%d,%d)(%d,%d)",
        Page,
        nWidth, nHeight,
        offsetX, offsetY, dpiX, dpiY,
        physicalWidth, physicalHeight
    );
    SetBkMode(hdc, TRANSPARENT);
    TextOutW(hdc, 4, 4, line, lstrlen(line));
}

inline void drawRect(HDC hdc, int x, int y, int w, int h, int det)
{
    MoveToEx(hdc, x + det, y + det, NULL);
    LineTo(hdc, x + w - det, y + det);
    LineTo(hdc, x + w - det, y + h - det);
    LineTo(hdc, x + det, y + h - det);
    LineTo(hdc, x + det, y + det);
}

inline void cmdDevMode(DEVMODE* dev1, DEVMODE* dev2)
{
    std::vector<wchar_t> text;
    text.resize(3600);
    wsprintf(text.data(),
        L"cmdDevMode::(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)"
        "(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)"
        "(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%d,%d)(%ws,%ws)\n",
        dev1->dmSpecVersion, dev2->dmSpecVersion,
        dev1->dmDriverVersion, dev2->dmDriverVersion,
        dev1->dmDriverExtra, dev2->dmDriverExtra,
        dev1->dmFields, dev2->dmFields,
        dev1->dmOrientation, dev2->dmOrientation,
        dev1->dmPaperSize, dev2->dmPaperSize,
        dev1->dmPaperLength, dev2->dmPaperLength,
        dev1->dmPaperWidth, dev2->dmPaperWidth,
        dev1->dmScale, dev2->dmScale,
        dev1->dmCopies, dev2->dmCopies,
        dev1->dmDefaultSource, dev2->dmDefaultSource,
        dev1->dmPrintQuality, dev2->dmPrintQuality,
        dev1->dmPosition.x, dev2->dmPosition.x,
        dev1->dmPosition.y, dev2->dmPosition.y,
        dev1->dmDisplayOrientation, dev2->dmDisplayOrientation,
        dev1->dmDisplayFixedOutput, dev2->dmDisplayFixedOutput,
        dev1->dmColor, dev2->dmColor,
        dev1->dmDuplex, dev2->dmDuplex,
        dev1->dmYResolution, dev2->dmYResolution,
        dev1->dmTTOption, dev2->dmTTOption,
        dev1->dmCollate, dev2->dmCollate,
        dev1->dmLogPixels, dev2->dmLogPixels,
        dev1->dmBitsPerPel, dev2->dmBitsPerPel,
        dev1->dmPelsWidth, dev2->dmPelsWidth,
        dev1->dmPelsHeight, dev2->dmPelsHeight,
        dev1->dmDisplayFlags, dev2->dmDisplayFlags,
        dev1->dmDisplayFrequency, dev2->dmDisplayFrequency,
        dev1->dmPanningWidth, dev2->dmPanningWidth,
        dev1->dmPanningHeight, dev2->dmPanningHeight,
        dev1->dmFormName, dev2->dmFormName
    );
    OutputDebugStringW(text.data());
}

static HDC TestGetPrinterDC(DEVMODE* devMode)
{
    PRINTDLG pdlg;

    /* Initialize the PRINTDLG structure. */
    memset(&pdlg, 0, sizeof(PRINTDLG));
    pdlg.lStructSize = sizeof(PRINTDLG);
    /* Set the flag to return printer DC. */
    pdlg.Flags = PD_RETURNDC;

    /* Invoke the printer dialog box. */
    PrintDlg(&pdlg);

    DEVMODE* devMode2 = (DEVMODE*)GlobalLock(pdlg.hDevMode);
    if (devMode2) {
        //DocumentProperties(this.Handle, handle, printerName, pDevMode, pDevMode, DM_IN_BUFFER | DM_PROMPT | DM_OUT_BUFFER);
        cmdDevMode(devMode, devMode2);

        devMode->dmFields = devMode2->dmFields;
        devMode->dmPaperSize = devMode2->dmPaperSize;
        memcpy(devMode->dmFormName, devMode2->dmFormName, sizeof(WCHAR) * CCHFORMNAME);

        GlobalUnlock(pdlg.hDevMode);
    }

    // hDC member of the PRINTDLG structure contains the printer DC.
    return pdlg.hDC;
}

const int kMaxPrintPageNum = 40;

// DocumentProperties可以获取打印机相关信息，EnumPrintersW在枚举的时候也能获取
// DeviceCapabilities 能获取纸张信息，EnumFormsW也可以
// devMode是包含打印机私有数据的，要记得大小
// 参考文档：E:\mycode\win2k\private\shell\comdlg32\prnsetup.c
// https://www.xuebuyuan.com/1765077.html
// https://github.com/wxWidgets/wxWidgets/blob/master/src/msw/printdlg.cpp
// https://support.microsoft.com/en-us/help/167345/how-to-modify-printer-settings-with-the-documentproperties-function
// https://support.microsoft.com/en-us/help/140285/how-to-modify-printer-settings-by-using-setprinter
// https://www.amyuni.com/forum/viewtopic.php?t=1224
// https://www.codeproject.com/search.aspx?q=printer&doctypeid=1%3b2%3b3%3b13

// 某些设备水平和垂直方向的DPI可能不同，所以要分别得到。得到水平方向的转换比例  
inline float getXScale(HDC printDC)  // printDC: 打印机DC设备句柄  
{
    // 得到打印机水平方向的DPI  
    int xDPI = GetDeviceCaps(printDC, LOGPIXELSX);

    // 254.0: 每英寸25.4毫米  
    float xScale = (float)(254.0 / xDPI);
    return xScale;
}

// 得到垂直方向的转换比例  
inline float getYScale(HDC printDC)  // printDC: 打印机DC设备句柄  
{
    //得到打印机垂直方向的DPI  
    int yDPI = GetDeviceCaps(printDC, LOGPIXELSY);

    float yScale = (float)(254.0 / yDPI);

    return yScale;
}

// 厘米转换为设备像素，水平方向  
inline double cm2Unit_W(float xScale, double w) // w单位为厘米  
{
    return w * 100 / xScale;
}

// 厘米转换为设备像素，垂直方向  
inline double cm2Unit_H(float yScale, double h) // h单位为厘米  
{
    return h * 100 / yScale;
}

inline std::wstring replaceStr(const std::wstring& str, const std::wstring& toReplaced, const std::wstring& newchars)
{
    std::wstring result = str;
    for (std::wstring::size_type pos(0); pos != std::wstring::npos; pos += newchars.length()) {
        pos = result.find(toReplaced, pos);
        if (pos != std::wstring::npos)
            result.replace(pos, toReplaced.length(), newchars);
        else
            break;
    }
    return result;
}

inline std::wstring trimQuote(const std::wstring& name)
{
    std::wstring newName;
    for (size_t i = 0; i < name.size(); ++i) {
        wchar_t c = name[i];
        if (L'\"' == c)
            c = L' ';
        newName += c;
    }

    return newName;
}

inline std::wstring trimSlash(const std::wstring& name)
{
    return replaceStr(name, L"\\", L"_backslash_");//_backquote_
}

inline std::wstring recoverSlash(const std::wstring& name)
{
    return replaceStr(name, L"_backslash_", L"\\");//_backquote_
}

inline std::wstring cutOffTitle(const std::wstring& title)
{
    if (title.size() < 15)
        return trimSlash(title);

    std::wstring result = title;
    result = result.substr(0, 20);
    result += L"...";

    return trimSlash(result);
}

// Interstice or gap between different header footer components. Hardcoded to
// about 0.5cm, match the value in PrintSettings::SetPrinterPrintableArea.
const float kSettingHeaderFooterInterstice = 14.2f;

// Length of a thousandth of inches in 0.01mm unit.
const int kHundrethsMMPerInch = 2540;

// Length of an inch in CSS's 1pt unit.
// http://dev.w3.org/csswg/css3-values/#absolute-length-units-cm-mm.-in-pt-pc
const int kPointsPerInch = 72;

// Length of an inch in CSS's 1px unit.
// http://dev.w3.org/csswg/css3-values/#the-px-unit
const int kPixelsPerInch = 96;

// Dpi used to save to PDF or Cloud Print.
const int kDefaultPdfDpi = 300;

// LETTER: 8.5 x 11 inches
const float kLetterWidthInch = 8.5f;
const float kLetterHeightInch = 11.0f;

// LEGAL: 8.5 x 14 inches
const float kLegalWidthInch = 8.5f;
const float kLegalHeightInch = 14.0f;

// A4: 8.27 x 11.69 inches
const float kA4WidthInch = 8.27f;
const float kA4HeightInch = 11.69f;

// A3: 11.69 x 16.54 inches
const float kA3WidthInch = 11.69f;
const float kA3HeightInch = 16.54f;

inline double convertUnitDouble(double value, double oldUnit, double newUnit)
{
    return value * newUnit / oldUnit;
}

inline int convertUnit(int value, int oldUnit, int newUnit)
{
    // With integer arithmetic, to divide a value with correct rounding, you need
    // to add half of the divisor value to the dividend value. You need to do the
    // reverse with negative number.
    if (value >= 0) {
        return ((value * newUnit) + (oldUnit / 2)) / oldUnit;
    } else {
        return ((value * newUnit) - (oldUnit / 2)) / oldUnit;
    }
}

class PrintingUtil {
public:
    static std::vector<char>* renderPdfPageToBitmap(
        mbWebView mbSrcView, 
        HWND hWnd, 
        int count, 
        const PrintSettings& printSettings,
        const RenderSettings& renderSettings,
        const void* pdfData, 
        size_t pdfDataSize
        )
    {
        if (!PdfiumLoad::get())
            return nullptr;

        std::vector<char>* result = new std::vector<char>();

        HDC refDeviceContext = ::GetDC(hWnd);
        int targetDpi = renderSettings.targetDpi; // ::GetDeviceCaps(refDeviceContext, LOGPIXELSX);
        int printDpi = printSettings.dpi;

        // 把打印设备的像素数，换算成屏幕像素数
        // 不使用printSettings.size，而是使用printSettings.physicalSizeDeviceUnits，因为发现有的打印机，size比physicalSizeDeviceUnits小了一些
        int width = (printSettings.physicalSizeDeviceUnits.w + 0) * targetDpi / printDpi;
        int height = (printSettings.physicalSizeDeviceUnits.h + 0) * targetDpi / printDpi;

        int scaleX = 1;
        int scaleY = 1;
        const int bytesPerRow = width * scaleX * 4;
        const long imageDataSize = bytesPerRow * height * scaleY;

        // Use the device context of the parent HWND as a template to create a new
        // device context.
        HDC hdc = ::CreateCompatibleDC(refDeviceContext);

        BITMAPINFO bmi = { { sizeof(BITMAPINFOHEADER), width * scaleX, height * scaleY, 1, 32, BI_RGB } };

        // Create a new bitmap object
        void *buffer = (void*)NULL;
        HBITMAP bitmap = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &buffer, 0, 0);

        // Set the new bitmap object as a backing surface for the device context.
        // The original backing surface is saved in originalSurface
        HGDIOBJ originalSurface = ::SelectObject(hdc, bitmap);

        BITMAPFILEHEADER fileHeader = {
            0x4d42,
            sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imageDataSize,
            0,
            0,
            sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
        };

        ::ReleaseDC(hWnd, refDeviceContext);

        // This is the best stretch mode
        ::SetStretchBltMode(hdc, HALFTONE);

        HBRUSH hbrush = ::CreateSolidBrush(RGB(255, 255, 255)); // 创建蓝色画刷
        ::SelectObject(hdc, hbrush);
        RECT rc = { 0, 0, (width)* scaleX, (height)* scaleY };
        ::FillRect(hdc, &rc, hbrush);
        ::DeleteObject(hbrush);

        BOOL b = PdfiumLoad::get()->renderPDFPageToDC(pdfData,
            (int)pdfDataSize,
            count,
            hdc, targetDpi,
            0, 0, (width) * scaleX, (height) * scaleY,
            renderSettings.fitToBounds, 
            renderSettings.stretchToBounds,
            renderSettings.keepAspectRatio, 
            renderSettings.centerInBounds,
            renderSettings.autorotate);

        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)mbSrcView);

        if (webview && webview->m_printingCallback) {
            mbPrintintSettings settings;
            settings.dpi = targetDpi;
            settings.width = width;
            settings.height = height;
            settings.scale = 1;
            webview->m_printingCallback(mbSrcView, webview->m_printingCallbackParam, kPrintintStepPreview, hdc, &settings, count);
        }

//         if (0) {
//             HANDLE hFile = CreateFileW(L"D:\\1.pdf", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//             if (hFile && INVALID_HANDLE_VALUE != hFile) {
//                 DWORD numberOfBytesWritten = 0;
//                 ::WriteFile(hFile, pdfData, pdfDataSize, &numberOfBytesWritten, NULL);
//                 ::CloseHandle(hFile);
//             }
//         }

        result->resize(sizeof(fileHeader) + sizeof(bmi.bmiHeader) + imageDataSize);
        int pos = 0;
        memcpy(result->data() + pos, &fileHeader, sizeof(BITMAPFILEHEADER));
        pos += sizeof(BITMAPFILEHEADER);

        memcpy(result->data() + pos, &bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
        pos += sizeof(BITMAPINFOHEADER);

        memcpy(result->data() + pos, buffer, imageDataSize);

//         if (0) {
//             HANDLE hFile = CreateFileW(L"D:\\3.bmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//             if (!hFile || INVALID_HANDLE_VALUE == hFile)
//                 return result;
// 
//             DWORD numberOfBytesWritten = 0;
//             ::WriteFile(hFile, result->data(), result->size(), &numberOfBytesWritten, NULL);
//             ::CloseHandle(hFile);
//         }

        ::DeleteObject(bitmap);
        ::DeleteDC(hdc);

        return result;
    }

};

}