
#ifndef printing_printing_h
#define printing_printing_h

#include "core/mb.h"
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

class Printing {
public:
    Printing(mbWebView webView, mbWebFrameHandle frameId);
    void run(const mbPrintSettings*);
    ~Printing();

private:    
    void createPreviewWin();
    bool enumPrinters();
    bool enumNetworkPrinters(std::vector<std::wstring>* printerNames);
    bool enumLocalPrinters(std::vector<std::wstring>* printerNames);

    void doPrint(const std::string& printerName, int64_t queryId);
    void doPrintImpl(const std::vector<int>& needPrintedPages, DEVMODE* devMode, const std::wstring& devName);

    void getPreview(int64_t queryId, int64_t id, const utf8* request);
    void getPdfDataInBlinkThread(int64_t queryId, const std::string& printerName);
    bool getPdfDataFromPdfViewerInBlinkThread(int64_t queryId);

    std::string* parseGetPreviewParams(const utf8* request);

    void onDocumentReady(mbWebView webView);
    void onJsQuery(mbWebView webView, mbJsExecState es, int64_t queryId, int customMsg, const utf8* request);
    void onLoadUrlBegin(mbWebView webView, const char *url, mbNetJob job);
    void onPaintUpdated(mbWebView webView, const HDC hdc, int x, int y, int cx, int cy);

    void onGetPrintPreviewSrc(const std::string& urlStr, mbNetJob job);

    static void MB_CALL_TYPE onPrintingDocumentReadyCallback(mbWebView webView, void* param, mbWebFrameHandle frameId);
    static void MB_CALL_TYPE onPrintingJsQueryCallback(mbWebView webView, void* param, mbJsExecState es, int64_t queryId, int customMsg, const utf8* request);
    static BOOL MB_CALL_TYPE onPrintingLoadUrlBegin(mbWebView webView, void* param, const char* url, mbNetJob job);
    static void MB_CALL_TYPE onPaintUpdatedCallback(mbWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy);
    static BOOL MB_CALL_TYPE onDestroyCallback(mbWebView webView, void* param, void* unuse);

    mbWebView m_mbSrcView;
    mbWebView m_mbPreview;

    mbWebFrameHandle m_frameId;

    PdfDataVisitor* m_pdfDataVisitor;

    PrintSettings* m_curPrinterSettings;
    bool m_landscape;
    bool m_isPrintPageHeadAndFooter;
    bool m_isPrintBackgroud;
    bool m_isGettingPreviewData;
    int m_duplex;

    mbSize m_userSelectPaperSize;
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