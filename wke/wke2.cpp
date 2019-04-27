
#include "wke/wke2.h"
#include "wke/wkeString.h"
#include "wke/wkeWebView.h"
#include "wke/wkeWebWindow.h"
#include "wke/wkeGlobalVar.h"
#include "content/browser/WebPage.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/web_impl_win/WebThreadImpl.h"
#include "content/resources/HeaderFooterHtml.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/web/WebSettingsImpl.h"
#include "third_party/WebKit/public/web/WebPrintScalingOption.h"
#include "third_party/WebKit/public/web/WebPrintParams.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkDocument.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "skia/ext/refptr.h"
#include "wtf/text/WTFString.h"
#include "wtf/text/WTFStringUtil.h"
#include "base/values.h"
#include "base/json/json_writer.h"
#include "net/WebURLLoaderInternal.h"
#include "net/InitializeHandleInfo.h"
#include <v8.h>
#include <shlwapi.h>
namespace net {
	void WSCI_setHook(void* j, void* hook);
	void WSCI_sendtext(void* j, char* buf, size_t len);
	void WSCI_sendblob(void* j, char* buf, size_t len);
}
namespace wke {

void printingTest(wkeWebView webview);
wkeMemBuf* printingTest2(wkeWebView webview);

const int kPointsPerInch = 72;

// Length of an inch in CSS's 1px unit.
// http://dev.w3.org/csswg/css3-values/#the-px-unit
const int kPixelsPerInch = 96;
const float kPrintingMinimumShrinkFactor = 1.25f;

void changeRequestUrl(wkeNetJob jobPtr, const char* url)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    blink::KURL newUrl(blink::ParsedURLString, url);
    job->m_response.setURL(newUrl);
    job->firstRequest()->setURL(newUrl);
    job->m_initializeHandleInfo->url = url;
    ASSERT(!job->m_url);
}

bool setDebugConfig(wkeWebView webview, const char* debugString, const char* param)
{
    if (0 == strcmp(debugString, "changeRequestUrl")) {
        wkeNetJob job = (wkeNetJob)webview;
        changeRequestUrl(job, param);
        return true;
    }

    content::WebPage* webpage = nullptr;
    blink::WebViewImpl* webViewImpl = nullptr;
    blink::WebSettingsImpl* settings = nullptr;
    if (webview)
        webpage = webview->getWebPage();
    if (webpage)
        webViewImpl = webpage->webViewImpl();
    if (webViewImpl)
        settings = webViewImpl->settingsImpl();

    String stringDebug(debugString);
    Vector<String> result;
    stringDebug.split(",", result);

    if (result.size() == 0)
        return true;

    String item = result[0];
    if ("setCookieJarPath" == item || "setCookieJarFullPath" == item) {
        std::string pathStr(param);
        if (pathStr.size() == 0)
            return true;

        if ("setCookieJarPath" == item) {
            if (pathStr[pathStr.size() - 1] != '\\' && pathStr[pathStr.size() - 1] != '/')
                pathStr += '\\';

            pathStr += "cookies.dat";
        }

        std::vector<char> result;
        WTF::Utf8ToMByte(pathStr.c_str(), pathStr.size(), &result, CP_ACP);
        if (0 == result.size())
            return true;
        result.push_back('\0');
        webview->setCookieJarFullPath(&result[0]);

        return true;
    } else if ("setLocalStorageFullPath" == item) {
        std::string pathStr(param);
        if (pathStr.size() == 0)
            return true;

        webview->setLocalStorageFullPath(pathStr.c_str());

        return true;
    } else if ("smootTextEnable" == item) {
        wke::g_smootTextEnable = atoi(param) == 1;
    }
	else if ("wsCallback" == item) {
		webview->webPage()->wkeHandler().wsCallback = (void*)(param);
		return true;
	}
	else if ("wsCallbackParam" == item) {
		webview->webPage()->wkeHandler().wsCallbackParam = (void*)(param);
		return true;
	}

    return false;
}

bool getDebugConfig(wkeWebView webview, const char* debugString, void **ret)
{
	if (strcmp("setwshook", debugString) == 0) {
		*ret = (void*)net::WSCI_setHook;
		return true;
	}
	else if (strcmp("sendtext", debugString) == 0) {
		*ret = (void*)net::WSCI_sendtext;
		return true;
	}
	else if (strcmp("sendblob", debugString) == 0) {
		*ret = (void*)net::WSCI_sendblob;
		return true;
	}
	content::WebPage* webpage = nullptr;
	blink::WebViewImpl* webViewImpl = nullptr;
	blink::WebSettingsImpl* settings = nullptr;
	if (webview)
		webpage = webview->getWebPage();
	if (webpage)
		webViewImpl = webpage->webViewImpl();
	if (webViewImpl)
		settings = webViewImpl->settingsImpl();

	String stringDebug(debugString);
	Vector<String> result;
	stringDebug.split(",", result);

	if (result.size() == 0)
		return true;

	String item = result[0];

	return false;
}

void printingTest(wkeWebView webview)
{
    content::WebPage* webpage = webview->getWebPage();
    blink::WebFrame* frame = webpage->mainFrame();

    blink::WebRect printContentArea(0, 0, 500, 600);
    blink::WebRect printableArea(0, 0, 500, 600);
    blink::WebSize paperSize(500, 600);
    const int kPointsPerInch = 72;
    int printerDPI = kPointsPerInch;
    blink::WebPrintScalingOption printScalingOption = blink::WebPrintScalingOptionSourceSize;
    blink::WebPrintParams webkitParams(printContentArea, printableArea, paperSize, printerDPI, printScalingOption);

    blink::WebView* blinkWebView = frame->view();
    blinkWebView->settings()->setShouldPrintBackgrounds(false);

    int pageCount = frame->printBegin(webkitParams);

    double cssScaleFactor = 1.0f;
    float webkitPageShrinkFactor = frame->getPrintPageShrink(0);
    float scaleFactor = cssScaleFactor * webkitPageShrinkFactor;

    SkPictureRecorder recorder;
    recorder.beginRecording(500 / scaleFactor, 600 / scaleFactor, NULL, 0);

    SkCanvas* canvas = recorder.getRecordingCanvas();

    frame->printPage(0, canvas);
    frame->printEnd();

    skia::RefPtr<SkPicture> content = skia::AdoptRef(recorder.endRecordingAsPicture());

    SkDynamicMemoryWStream pdfStream;
    skia::RefPtr<SkDocument> pdfDoc = skia::AdoptRef(SkDocument::CreatePDF(&pdfStream));

    SkRect contentArea = SkRect::MakeIWH(500, 600);
    SkCanvas* pdfCanvas = pdfDoc->beginPage(500, 600, nullptr);

    pdfCanvas->scale(scaleFactor, scaleFactor);
    pdfCanvas->drawPicture(content.get());
    pdfCanvas->flush();

    pdfDoc->endPage();
    pdfDoc->close();

    SkData* pdfData = (pdfStream.copyToData());

    HANDLE hFile = CreateFileW(L"D:\\1.pdf", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!hFile || INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD numberOfBytesWritten = 0;
    ::WriteFile(hFile, pdfData->data(), pdfData->size(), &numberOfBytesWritten, NULL);
    ::CloseHandle(hFile);

    pdfData->unref();
}

void readFile(const wchar_t* path, std::vector<char>* buffer)
{
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD fileSizeHigh;
    const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);

    DWORD numberOfBytesRead = 0;
    buffer->resize(bufferSize);
    BOOL b = ::ReadFile(hFile, &buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
    ::CloseHandle(hFile);
    b = b;
}

static void executeScript(blink::WebFrame* frame, const char* scriptFormat, const base::Value& parameters)
{
    std::string json;
    base::JSONWriter::Write(parameters, &json);

    const int strSize = 2 * (strlen(scriptFormat) + json.size());
    char* script = (char*)malloc(strSize);
    sprintf(script, scriptFormat, json.c_str());

    blink::WebScriptSource source(blink::WebString::fromUTF8(script));
    frame->executeScript(source);
    free(script);
}

const char kPageLoadScriptFormat[] = "document.open(); document.write(%s); document.close();";
const char kSettingHeaderFooterDate[] = "date";
const char kPageSetupScriptFormat[] = "setup(%s);";

static int getWindowDPI(HWND hWnd)
{
    HDC screenDC = ::GetDC(hWnd);
    int dpiX = ::GetDeviceCaps(screenDC, LOGPIXELSX);//96是100%、120是125%
    ::ReleaseDC(nullptr, screenDC);

    return dpiX;
}

int convertUnit(int value, int oldUnit, int newUnit)
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

void printHeaderAndFooter(
    bool isPrintPageHeadAndFooter,
    blink::WebCanvas* canvas,
    int windowDPI,
    int pageNumber,
    int totalPages,
    const blink::WebFrame& sourceFrame,
    float webkitScaleFactor,
    blink::WebSize pageSizeInPt,
    int topMargin,
    int bottomMargin
    )
{
    blink::WebSize pageSizeInPx(convertUnit(pageSizeInPt.width, kPointsPerInch, windowDPI), convertUnit(pageSizeInPt.height, kPointsPerInch, windowDPI));

    SkAutoCanvasRestore autoRestore(canvas, true);
    canvas->scale(1 / webkitScaleFactor, 1 / webkitScaleFactor);

    blink::WebView* webView = blink::WebView::create(NULL);
    webView->settings()->setJavaScriptEnabled(true);

    blink::WebLocalFrame* frame = blink::WebLocalFrame::create(blink::WebTreeScopeType::Document, NULL);
    webView->setMainFrame(frame);

#if 0
    std::vector<char> buffer;
    readFile(L"E:\\mycode\\miniblink49\\trunk\\content\\resources\\HeaderFooterHtml.htm", &buffer);
    base::StringValue html(std::string(buffer.data(), buffer.size()));
#else
    base::StringValue html(std::string(content::kHeaderFooterHtml, sizeof(content::kHeaderFooterHtml)));
#endif

    // Load page with script to avoid async operations.
    executeScript(frame, kPageLoadScriptFormat, html);

    SYSTEMTIME st = { 0 };
    ::GetLocalTime(&st);

    char dataBuffer[0x100];
    sprintf(dataBuffer, "%d-%d-%d", st.wYear, st.wMonth, st.wDay);

    base::DictionaryValue* options = new base::DictionaryValue();
    options->SetBoolean("isPrintPageHeadAndFooter", isPrintPageHeadAndFooter);
    options->SetString("date", dataBuffer);
    options->SetDouble("width", pageSizeInPt.width);
    options->SetDouble("height", pageSizeInPt.height);
    options->SetDouble("topMargin", topMargin);
    options->SetDouble("bottomMargin", bottomMargin);

    char* pageNumberStr = (char*)malloc(0x100);
    sprintf(pageNumberStr, "%d/%d", pageNumber, totalPages);
    options->SetString("pageNumber", pageNumberStr);
    free(pageNumberStr);

    blink::KURL kurl = sourceFrame.document().url();
    options->SetString("url", kurl.getUTF8String().utf8().data());

    std::string title = sourceFrame.document().title().utf8();
    options->SetString("title", title.empty() ? "print document" : title);

    executeScript(frame, kPageSetupScriptFormat, *options);

    blink::WebPrintParams webkitParams(pageSizeInPt);
    webkitParams.printerDPI = windowDPI;

    frame->printBegin(webkitParams);
    frame->printPage(0, canvas);
    frame->printEnd();

    webView->close();
    frame->close();

    delete options;
}

const wkePdfDatas* printToPdf(wkeWebView webView, blink::WebFrame* webFrame, const wkePrintSettings* params)
{
    content::WebPage* webpage = webView->getWebPage();
    if (!webpage)
        return nullptr;

    blink::WebFrame* frame = webFrame;
    if (!frame)
        frame = webpage->mainFrame();

    if (!frame)
        return nullptr;

    int windowDPI = 72; // chromium是用desired_dpi=72硬编码，而不是getWindowDPI(webView->windowHandle());
    int dpi = params->dpi;
    int srcWidth = convertUnit(params->width, dpi, kPointsPerInch); // 转换成pt，但由于DPI是72，所以也可以说是px
    int srcHeight = convertUnit(params->height, dpi, kPointsPerInch);

    int marginTop = convertUnit(params->marginTop, dpi, kPointsPerInch);
    int marginBottom = convertUnit(params->marginBottom, dpi, kPointsPerInch);
    int marginLeft = convertUnit(params->marginLeft, dpi, kPointsPerInch);
    int marginRight = convertUnit(params->marginRight, dpi, kPointsPerInch);
    
    // PrepareFrameAndViewForPrint, ComputeWebKitPrintParamsInDesiredDpi
    blink::WebSize printContentSize(srcWidth - marginLeft - marginRight, srcHeight - marginTop - marginBottom);

    blink::WebRect printContentArea(0, 0, (printContentSize.width) /*/ kPrintingMinimumShrinkFactor*/, (printContentSize.height) /*/ kPrintingMinimumShrinkFactor*/);
    blink::WebSize paperSize(srcWidth, srcHeight);
    blink::WebRect printableArea(0, 0, paperSize.width, paperSize.height);

    SkRect clipRect = SkRect::MakeIWH(printContentSize.width, printContentSize.height);

    float webkitScaleFactor = 1.0 / kPrintingMinimumShrinkFactor;

    blink::WebPrintScalingOption printScalingOption = blink::WebPrintScalingOptionSourceSize;
    blink::WebPrintParams webkitParams(printContentArea, printableArea, paperSize, dpi, printScalingOption);

    blink::WebView* blinkWebView = frame->view();
    blink::WebSize oldSize = webpage->viewportSize();

    blink::WebSize printLayoutSize(printContentArea.width, printContentArea.height);

    if (params->isLandscape)
        printLayoutSize.width = ((int)((printLayoutSize.width) * kPrintingMinimumShrinkFactor));
    else
        printLayoutSize.height = ((int)((printLayoutSize.height) * kPrintingMinimumShrinkFactor));

    printLayoutSize.width = printLayoutSize.width; // convertUnit(printLayoutSize.width, kPointsPerInch, windowDPI); // pt -> px
    printLayoutSize.height = printLayoutSize.height; // convertUnit(printLayoutSize.height, kPointsPerInch, windowDPI);

    webpage->setViewportSize(printLayoutSize); // paperSize

//     double oldZoom = blinkWebView->zoomLevel();
//     double zoomLevel = blink::WebView::zoomFactorToZoomLevel(0.5);
//     blinkWebView->setZoomLevel(zoomLevel);

//     float oldZoom = blinkWebView->pageScaleFactor();
//     blinkWebView->setDefaultPageScaleLimits(0.5, 1.5);
//     blinkWebView->setPageScaleFactor(0.5);

    int pageCount = frame->printBegin(webkitParams);
    if (0 == pageCount)
        return nullptr;

    blinkWebView->settings()->setShouldPrintBackgrounds(params->isPrintBackgroud);

    wkePdfDatas* result = new wkePdfDatas();
    result->count = pageCount;
    result->sizes = (size_t*)malloc(pageCount * sizeof(size_t*));
    result->datas = (const void**)malloc(pageCount * sizeof(void*));

    for (int i = 0; i < pageCount; ++i) {
        float webkitPageShrinkFactor = frame->getPrintPageShrink(i);

        SkDynamicMemoryWStream pdfStream;
        skia::RefPtr<SkDocument> document = skia::AdoptRef(SkDocument::CreatePDF(&pdfStream, kPointsPerInch));

        SkCanvas* canvas = document->beginPage(srcWidth / webkitPageShrinkFactor, srcHeight / webkitPageShrinkFactor);
        canvas->scale(webkitPageShrinkFactor, webkitPageShrinkFactor);
        
        printHeaderAndFooter(params->isPrintPageHeadAndFooter, canvas, windowDPI, i + 1, pageCount, *frame, webkitPageShrinkFactor, paperSize, marginTop, marginBottom);

        SkRect clipRectTemp = SkRect::MakeIWH(clipRect.width() / webkitPageShrinkFactor, clipRect.height() / webkitPageShrinkFactor);

        canvas->save();
        canvas->translate(marginLeft / webkitPageShrinkFactor, marginTop / webkitPageShrinkFactor);
        canvas->clipRect(clipRectTemp);
        frame->printPage(i, canvas);
        canvas->restore();

        canvas->flush();
        document->endPage();
        document->close();

        SkData* pdfData = pdfStream.copyToData();

        result->sizes[i] = pdfData->size();
        result->datas[i] = malloc(pdfData->size());
        memcpy((void*)(result->datas[i]), pdfData->data(), pdfData->size());

        pdfData->unref();
    }

    frame->printEnd();

    //blinkWebView->setZoomLevel(oldZoom);
    //blinkWebView->setPageScaleFactor(oldZoom);
    webpage->setViewportSize(oldSize);

    return result;
}

const wkeMemBuf* printToBitmap(wkeWebView webView, const wkeScreenshotSettings* settings)
{
    content::WebPage* webpage = webView->getWebPage();
    if (!webpage)
        return nullptr;
    blink::WebFrame* webFrame = webpage->mainFrame();
    if (!webFrame)
        return nullptr;

    blink::WebSize contentsSize = webFrame->contentsSize();

    int width = contentsSize.width;
    int height = contentsSize.height;

    if (width <= 0 || height <= 0)
        return nullptr;

    SkBitmap bitmap;
    bitmap.allocN32Pixels(width + 0.5, height + 0.5);

    SkCanvas canvas(bitmap);

    blink::WebString css;
    webFrame->drawInCanvas(blink::WebRect(0, 0, width, height), css, &canvas);

    canvas.flush();

    size_t size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bitmap.getSize();

    BITMAPFILEHEADER fileHeader = {
        0x4d42,
        sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size,
        0, 0,
        sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
    };

    BITMAPINFOHEADER bmiHeader = {
        sizeof(BITMAPINFOHEADER),
        width,
        -height,
        1,
        32,
        BI_RGB,
    };

    std::vector<char> bmpData;
    bmpData.resize(size);

    char* destBuffer = reinterpret_cast<char*>(&bmpData.at(0));
    size_t offset = 0;

    memcpy(destBuffer, &fileHeader, sizeof(fileHeader));
    offset += sizeof(fileHeader);

    memcpy(destBuffer + offset, &bmiHeader, sizeof(bmiHeader));
    offset += sizeof(bmiHeader);

    bitmap.copyPixelsTo(destBuffer + offset, size);

    wkeMemBuf* result = wkeCreateMemBuf(nullptr, destBuffer, size);
    return result;
}

}

void wkeUtilRelasePrintPdfDatas(const wkePdfDatas* datas)
{
    for (int i = 0; i < datas->count; ++i) {
        free((void *)(datas->datas[i]));
    }

    free((void *)(datas->sizes));
    free((void *)(datas->datas));
    delete datas;
}

const wkePdfDatas* wkeUtilPrintToPdf(wkeWebView webView, wkeWebFrameHandle frameId, const wkePrintSettings* settings)
{
    content::WebPage* webPage = webView->webPage();
    blink::WebFrame* webFrame = webPage->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(webPage, frameId));
    return wke::printToPdf(webView, webFrame, settings);
}

const wkeMemBuf* wkePrintToBitmap(wkeWebView webView, wkeWebFrameHandle frameId, const wkeScreenshotSettings* settings)
{
    return wke::printToBitmap(webView, settings);
}