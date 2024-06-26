
#ifndef printing_printing_h
#define printing_printing_h

#include "wke/wkedefine.h"
#include <string>
#include <xstring>
#include <map>
#include <vector>
#include <functional>

typedef struct _wkePdfDatas wkePdfDatas;

namespace printing {

struct PrintSettings;
class PdfViewerPlugin;

class PdfDataVisitor {
public:
    PdfDataVisitor(const PdfViewerPlugin* plugin);
    PdfDataVisitor(const wkePdfDatas* pdfdatas);
    ~PdfDataVisitor();

    bool isValid();
    int getCount() const;

    const void* getData(int pageNum);
    int getDataSize(int pageNum);

    bool isPdfPluginMode() const
    {
        return m_isPdfPluginMode;
    }

private:
    bool m_isPdfPluginMode;

    const wkePdfDatas* m_pdfdatas;
    std::vector<char> m_pdfData;
    int m_pageCount;
};

class WkePrinting {
public:
    WkePrinting(wkeWebView webView, wkeWebFrameHandle frameId);
    void run(const wkePrintSettings*);
    ~WkePrinting();

private:    
    void createPreviewWin();
    bool enumPrinters();
    bool enumNetworkPrinters(std::vector<std::wstring>* printerNames);
    bool enumLocalPrinters(std::vector<std::wstring>* printerNames);

    void doPrint(jsExecState es, const std::string& printerName, int64_t queryId);
    void doPrintImpl(const std::vector<int>& needPrintedPages, DEVMODE* devMode, const std::wstring& devName);

    void getPreview(jsExecState es, int64_t queryId, const utf8* request);
    void getPdfDataInBlinkThread(jsExecState es, int64_t queryId, const std::string& printerName);
    bool getPdfDataFromPdfViewerInBlinkThread(jsExecState es, int64_t queryId);

    std::string* parseGetPreviewParams(const utf8* request);

    void onDocumentReady(wkeWebView webView);
    void onLoadUrlBegin(wkeWebView webView, const char *url, wkeNetJob job);
    void onPaintUpdated(wkeWebView webView, const HDC hdc, int x, int y, int cx, int cy);

    void onGetPrintPreviewSrc(const std::string& urlStr, wkeNetJob job);

    static void WKE_CALL_TYPE onPrintingDocumentReadyCallback(wkeWebView webView, void* param);
    static bool WKE_CALL_TYPE onPrintingLoadUrlBegin(wkeWebView webView, void* param, const char* url, wkeNetJob job);
    static void WKE_CALL_TYPE onPaintUpdatedCallback(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy);
    static BOOL WKE_CALL_TYPE onDestroyCallback(wkeWebView webView, void* param, void* unuse);
    static void WKE_CALL_TYPE onDidCreateScriptContext(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* context, int extensionGroup, int worldId);
    static jsValue WKE_CALL_TYPE jsmbQuery(jsExecState es, void* param);

    wkeWebView m_mbSrcView;
    int m_mbSrcViewId;
    wkeWebView m_mbPreview;

    wkeWebFrameHandle m_frameId;

    PdfDataVisitor* m_pdfDataVisitor;

    PrintSettings* m_curPrinterSettings;
    bool m_landscape;
    bool m_isPrintPageHeadAndFooter;
    bool m_isPrintBackgroud;
    bool m_isGettingPreviewData;
    int m_duplex;

    wkeSize m_userSelectPaperSize;
    int m_userSelectPaperType;
    int m_copies;

    std::function<void(void)>* m_delayRunClosure;

    std::map<unsigned int, int> m_nameToPrinterDefaultPaperType; // HP打印机很奇怪，如果不强制用这个值，打印出来的是不居中的
public:
    class DevnameToDeviceMode {
    public:
        ~DevnameToDeviceMode() { clear(); }

        DEVMODE* find(const std::wstring& name)
        {
            unsigned int hash = getHash(name.c_str());
            std::map<unsigned int, std::vector<char>*>::iterator it = m_nameToDeviceMode.find(hash);
            if (it == m_nameToDeviceMode.end())
                return nullptr;

            std::vector<char>* devModeBuf = it->second;
            if (!devModeBuf || 0 == devModeBuf->size())
                return nullptr;

            return (DEVMODE*)(&devModeBuf->at(0));
        }

        void clear()
        {
            std::map<unsigned int, std::vector<char>*>::iterator it = m_nameToDeviceMode.begin();
            for (; it != m_nameToDeviceMode.end(); ++it) {
                std::vector<char>* devModeBuf = it->second;
                if (!devModeBuf)
                    continue;
                delete devModeBuf;
            }
            m_nameToDeviceMode.clear();
        }

        void add(const PRINTER_INFO_2* info)
        {
            size_t size = sizeof(DEVMODE);
            std::vector<char>* deviceMode = new std::vector<char>();
            if (info->pDevMode) {
                size = info->pDevMode->dmSize + info->pDevMode->dmDriverExtra;

                size_t sizeOfDEVMODE = sizeof(DEVMODE);
                if (size < sizeOfDEVMODE)
                    return;

                deviceMode->resize(size);
                memcpy(&deviceMode->at(0), (const char*)info->pDevMode, size);
            } else {
                deviceMode->resize(size);
                memset(&deviceMode->at(0), 0, size);
            }

            unsigned int hash = getHash(info->pPrinterName);
            m_nameToDeviceMode.insert(std::pair<unsigned int, std::vector<char>*>(hash, deviceMode));
        }

        static unsigned int getHash(const wchar_t* str)
        {
            unsigned int hash = 0;

            while (*str) {
                hash = (*str++) + (hash << 6) + (hash << 16) - hash;
            }

            return (hash & 0x7FFFFFFF);
        }
        std::map<unsigned int, std::vector<char>*> m_nameToDeviceMode;
    };
    DevnameToDeviceMode m_nameToDeviceMode;
};

}

#endif