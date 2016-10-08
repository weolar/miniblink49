/***
* comdef.h - Native C++ compiler COM support - main definitions header
*
*       Copyright (C) 1996-1999 Microsoft Corporation
*       All rights reserved.
*
****/

#if !defined(_INC_COMDEF)
#define _INC_COMDEF

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef  __cplusplus
#error Native Compiler support only available in C++ compiler
#endif

#include <ole2.h>
#include <olectl.h>

#include <comutil.h>

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable: 4244)
#pragma warning(disable: 4290)

#pragma comment(lib, "comsupp.lib")

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

class _com_error;

void __stdcall
        _com_raise_error(HRESULT hr, IErrorInfo* perrinfo = 0) throw(_com_error);

void __stdcall
        _com_issue_error(HRESULT) throw(_com_error);
void __stdcall
        _com_issue_errorex(HRESULT, IUnknown*, REFIID) throw(_com_error);

HRESULT __stdcall
        _com_dispatch_propget(IDispatch*, DISPID, VARTYPE, void*) throw(_com_error);
HRESULT __cdecl
        _com_dispatch_propput(IDispatch*, DISPID, VARTYPE, ...) throw(_com_error);
HRESULT __cdecl
        _com_dispatch_method(IDispatch*, DISPID, WORD, VARTYPE, void*,
                                                 const wchar_t*, ...) throw(_com_error);

HRESULT __stdcall
        _com_dispatch_raw_propget(IDispatch*, DISPID, VARTYPE, void*) throw();
HRESULT __cdecl
        _com_dispatch_raw_propput(IDispatch*, DISPID, VARTYPE, ...) throw();
HRESULT __cdecl
        _com_dispatch_raw_method(IDispatch*, DISPID, WORD, VARTYPE, void*,
                                                         const wchar_t*, ...) throw();

class _com_error {
public:
        // Constructors
        //
        _com_error(HRESULT hr,
               IErrorInfo* perrinfo = NULL,
               bool fAddRef = false) throw();
        _com_error(const _com_error& that) throw();

        // Destructor
        //
        virtual ~_com_error() throw();

        // Assignment operator
        //
        _com_error& operator=(const _com_error& that) throw();

        // Accessors
        //
        HRESULT Error() const throw();
        WORD WCode() const throw();
        IErrorInfo * ErrorInfo() const throw();

        // IErrorInfo method accessors
        //
        _bstr_t Description() const throw(_com_error);
        DWORD HelpContext() const throw();
        _bstr_t HelpFile() const throw(_com_error);
        _bstr_t Source() const throw(_com_error);
        GUID GUID() const throw();

        // FormatMessage accessors
        const TCHAR * ErrorMessage() const throw();

        // EXCEPINFO.wCode <-> HRESULT mappers
        static HRESULT WCodeToHRESULT(WORD wCode) throw();
        static WORD HRESULTToWCode(HRESULT hr) throw();

private:
        enum {
                WCODE_HRESULT_FIRST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x200),
                WCODE_HRESULT_LAST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF+1, 0) - 1
        };
        const HRESULT                   m_hresult;
        IErrorInfo *                    m_perrinfo;
        mutable TCHAR *                 m_pszMsg;
};

inline _com_error::_com_error(HRESULT hr,
                              IErrorInfo* perrinfo,
                              bool fAddRef) throw()
        : m_hresult(hr), m_perrinfo(perrinfo), m_pszMsg(NULL)
{
        if (m_perrinfo != NULL && fAddRef) {
                m_perrinfo->AddRef();
        }
}

inline _com_error::_com_error(const _com_error& that) throw()
        : m_hresult(that.m_hresult), m_perrinfo(that.m_perrinfo), m_pszMsg(NULL)
{
        if (m_perrinfo != NULL) {
                m_perrinfo->AddRef();
        }
}

inline _com_error::~_com_error() throw()
{
        if (m_perrinfo != NULL) {
                m_perrinfo->Release();
        }
        if (m_pszMsg != NULL) {
                LocalFree((HLOCAL)m_pszMsg);
        }
}

inline _com_error& _com_error::operator=(const _com_error& that) throw()
{
        if (this != &that) {
                this->_com_error::~_com_error();
                this->_com_error::_com_error(that);
        }
        return *this;
}

inline HRESULT _com_error::Error() const throw()
{
        return m_hresult;
}

inline WORD _com_error::WCode() const throw()
{
        return HRESULTToWCode(m_hresult);
}

inline IErrorInfo * _com_error::ErrorInfo() const throw()
{
        if (m_perrinfo != NULL) {
                m_perrinfo->AddRef();
        }
        return m_perrinfo;
}

inline _bstr_t _com_error::Description() const throw(_com_error)
{
        BSTR bstr = NULL;
        if (m_perrinfo != NULL) {
                m_perrinfo->GetDescription(&bstr);
        }
        return _bstr_t(bstr, false);
}

inline DWORD _com_error::HelpContext() const throw()
{
        DWORD dwHelpContext = 0;
        if (m_perrinfo != NULL) {
                m_perrinfo->GetHelpContext(&dwHelpContext);
        }
        return dwHelpContext;
}

inline _bstr_t _com_error::HelpFile() const throw(_com_error)
{
        BSTR bstr = NULL;
        if (m_perrinfo != NULL) {
                m_perrinfo->GetHelpFile(&bstr);
        }
        return _bstr_t(bstr, false);
}

inline _bstr_t _com_error::Source() const throw(_com_error)
{
        BSTR bstr = NULL;
        if (m_perrinfo != NULL) {
                m_perrinfo->GetSource(&bstr);
        }
        return _bstr_t(bstr, false);
}

inline _GUID _com_error::GUID() const throw()
{
        _GUID guid;
        memcpy(&guid, &__uuidof(NULL), sizeof(_GUID));
        if (m_perrinfo != NULL) {
                m_perrinfo->GetGUID(&guid);
        }
        return guid;
}

inline const TCHAR * _com_error::ErrorMessage() const throw()
{
        if (m_pszMsg == NULL) {
                FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                                          NULL,
                                          m_hresult,
                                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                          (LPTSTR)&m_pszMsg,
                                          0,
                                          NULL );
                if (m_pszMsg != NULL) {
                        int nLen = lstrlen(m_pszMsg);
                        if (nLen > 1 && m_pszMsg[nLen - 1] == '\n') {
                                m_pszMsg[nLen - 1] = 0;
                                if (m_pszMsg[nLen - 2] == '\r') {
                                        m_pszMsg[nLen - 2] = 0;
                                }
                        }
                } else {
                        m_pszMsg = (LPTSTR)LocalAlloc(0, 32 * sizeof(TCHAR));
                        if (m_pszMsg != NULL) {
                                WORD wCode = WCode();
#ifdef _STRSAFE_H_INCLUDED_
                                if (wCode != 0) {
                                    StringCchPrintf(m_pszMsg, 32, TEXT("IDispatch error #%d"), wCode);
                                } else {
                                    StringCchPrintf(m_pszMsg, 32, TEXT("Unknown error 0x%0lX"), m_hresult);
                                }
#else
                                if (wCode != 0) {
                                        wsprintf(m_pszMsg, TEXT("IDispatch error #%d"), wCode);
                                } else {
                                        wsprintf(m_pszMsg, TEXT("Unknown error 0x%0lX"), m_hresult);
                                }
#endif
                        }
                }
        }
        return m_pszMsg;
}

inline HRESULT _com_error::WCodeToHRESULT(WORD wCode) throw()
{
        return wCode >= 0xFE00 ? WCODE_HRESULT_LAST : WCODE_HRESULT_FIRST + wCode;
}

inline WORD _com_error::HRESULTToWCode(HRESULT hr) throw()
{
        return (hr >= WCODE_HRESULT_FIRST && hr <= WCODE_HRESULT_LAST)
                ? WORD(hr - WCODE_HRESULT_FIRST)
                : 0;
}

#if !defined(_COM_SMARTPTR)
 #if !defined(_INC_COMIP)
  #include <comip.h>
 #endif
 #define _COM_SMARTPTR        _com_ptr_t
 #define _COM_SMARTPTR_LEVEL2 _com_IIID
#endif
#if defined(_COM_SMARTPTR)
 #if !defined(_COM_SMARTPTR_TYPEDEF)
  #if defined(_COM_SMARTPTR_LEVEL2)
   #define _COM_SMARTPTR_TYPEDEF(Interface, IID) \
    typedef _COM_SMARTPTR<_COM_SMARTPTR_LEVEL2<Interface, &IID> > \
            Interface ## Ptr
  #else
   #define _COM_SMARTPTR_TYPEDEF(Interface, IID) \
    typedef _COM_SMARTPTR<Interface, &IID> \
            Interface ## Ptr
  #endif
 #endif
#endif

#if !defined(_COM_NO_STANDARD_GUIDS_)

#ifdef _WIN64
#define     IActiveScriptParse                  IActiveScriptParse64
#define     IActiveScriptParseProcedureOld      IActiveScriptParseProcedureOld64
#define     IActiveScriptParseProcedure         IActiveScriptParseProcedure64
#define     IActiveScriptParseProcedure2        IActiveScriptParseProcedure2_64
#else
#define     IActiveScriptParse                  IActiveScriptParse32
#define     IActiveScriptParseProcedureOld      IActiveScriptParseProcedureOld32
#define     IActiveScriptParseProcedure         IActiveScriptParseProcedure32
#define     IActiveScriptParseProcedure2        IActiveScriptParseProcedure2_32
#endif

// Interfaces:

struct __declspec(uuid("0000013D-0000-0000-C000-000000000046")) IClientSecurity;
struct __declspec(uuid("0000013E-0000-0000-C000-000000000046")) IServerSecurity;
struct __declspec(uuid("00000140-0000-0000-C000-000000000046")) IClassActivator;
struct __declspec(uuid("00020d00-0000-0000-c000-000000000046")) IRichEditOle;
struct __declspec(uuid("00020d03-0000-0000-c000-000000000046")) IRichEditOleCallback;
struct __declspec(uuid("000214e1-0000-0000-c000-000000000046")) INewShortcutHookA;
struct __declspec(uuid("000214e2-0000-0000-c000-000000000046")) IShellBrowser;
struct __declspec(uuid("000214e3-0000-0000-c000-000000000046")) IShellView;
struct __declspec(uuid("000214e4-0000-0000-c000-000000000046")) IContextMenu;
struct __declspec(uuid("000214e5-0000-0000-c000-000000000046")) IShellIcon;
struct __declspec(uuid("000214e6-0000-0000-c000-000000000046")) IShellFolder;
struct __declspec(uuid("000214e8-0000-0000-c000-000000000046")) IShellExtInit;
struct __declspec(uuid("000214e9-0000-0000-c000-000000000046")) IShellPropSheetExt;
struct __declspec(uuid("000214ea-0000-0000-c000-000000000046")) IPersistFolder;
struct __declspec(uuid("000214eb-0000-0000-c000-000000000046")) IExtractIconA;
struct __declspec(uuid("000214ee-0000-0000-c000-000000000046")) IShellLinkA;
struct __declspec(uuid("000214f0-0000-0000-c000-000000000046")) IFileViewerA;
struct __declspec(uuid("000214f1-0000-0000-c000-000000000046")) ICommDlgBrowser;
struct __declspec(uuid("000214f2-0000-0000-c000-000000000046")) IEnumIDList;
struct __declspec(uuid("000214f3-0000-0000-c000-000000000046")) IFileViewerSite;
struct __declspec(uuid("000214f4-0000-0000-c000-000000000046")) IContextMenu2;
struct __declspec(uuid("000214f5-0000-0000-c000-000000000046")) IShellExecuteHookA;
struct __declspec(uuid("000214f7-0000-0000-c000-000000000046")) INewShortcutHookW;
struct __declspec(uuid("000214f8-0000-0000-c000-000000000046")) IFileViewerW;
struct __declspec(uuid("000214f9-0000-0000-c000-000000000046")) IShellLinkW;
struct __declspec(uuid("000214fa-0000-0000-c000-000000000046")) IExtractIconW;
struct __declspec(uuid("000214fb-0000-0000-c000-000000000046")) IShellExecuteHookW;
struct __declspec(uuid("00021500-0000-0000-c000-000000000046")) IQueryInfo;
struct __declspec(uuid("0002DF05-0000-0000-C000-000000000046")) IWebBrowserApp;
struct __declspec(uuid("0002E000-0000-0000-C000-000000000046")) IEnumGUID;
struct __declspec(uuid("0002E011-0000-0000-C000-000000000046")) IEnumCATEGORYINFO;
struct __declspec(uuid("0002E012-0000-0000-C000-000000000046")) ICatRegister;
struct __declspec(uuid("0002E013-0000-0000-C000-000000000046")) ICatInformation;
struct __declspec(uuid("012dd920-7b26-11d0-8ca9-00a0c92dbfe8")) IDockingWindow;
struct __declspec(uuid("3050f4e9-98b5-11cf-bb82-00aa00bdce0b")) IHTMLControlElement;
struct __declspec(uuid("085FB2C0-0DF8-11D1-8F4B-00A0C905413F")) ISubscriptionMgr;
struct __declspec(uuid("08EC3E00-50B0-11CF-960C-0080C7F4EE85")) FolderItemVerb;
struct __declspec(uuid("0c6c4200-c589-11d0-999a-00c04fd655e1")) IShellIconOverlayIdentifier;
struct __declspec(uuid("1008C4A0-7613-11CF-9AF1-0020AF6E72F4")) IChannelHook;
struct __declspec(uuid("163BB1E0-6E00-11CF-837A-48DC04C10000")) IHTMLLocation;
struct __declspec(uuid("1CFF0050-6FDD-11D0-9328-00A0C90DCAA9")) IActiveScriptParseProcedureOld32;
struct __declspec(uuid("1F8352C0-50B0-11CF-960C-0080C7F4EE85")) FolderItemVerbs;
struct __declspec(uuid("1ac3d9f0-175c-11d1-95be-00609797ea4f")) IPersistFolder2;
struct __declspec(uuid("21F57128-08C9-4638-BA12-22D15D88DC5C")) IActiveScriptParseProcedureOld64;
struct __declspec(uuid("275C23E1-3747-11D0-9FEA-00AA003F8646")) IMultiLanguage;
struct __declspec(uuid("275C23E3-3747-11D0-9FEA-00AA003F8646")) IEnumCodePage;
struct __declspec(uuid("2a342fc2-7b26-11d0-8ca9-00a0c92dbfe8")) IDockingWindowSite;
struct __declspec(uuid("3050F1D8-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLBodyElement;
struct __declspec(uuid("3050F1D9-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLFontElement;
struct __declspec(uuid("3050F1DA-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLAnchorElement;
struct __declspec(uuid("3050F1DD-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLUListElement;
struct __declspec(uuid("3050F1DE-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLOListElement;
struct __declspec(uuid("3050F1E0-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLLIElement;
struct __declspec(uuid("3050F1F0-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLBRElement;
struct __declspec(uuid("3050F1F1-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLDListElement;
struct __declspec(uuid("3050F1F2-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLDDElement;
struct __declspec(uuid("3050F1F3-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLDTElement;
struct __declspec(uuid("3050F1F4-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLHRElement;
struct __declspec(uuid("3050F1F5-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLParaElement;
struct __declspec(uuid("3050F1F6-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLHeaderElement;
struct __declspec(uuid("3050F1F7-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLFormElement;
struct __declspec(uuid("3050F1FF-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLElement;
struct __declspec(uuid("3050F200-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLDivElement;
struct __declspec(uuid("3050F202-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLBaseFontElement;
struct __declspec(uuid("3050F203-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLMetaElement;
struct __declspec(uuid("3050F204-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLBaseElement;
struct __declspec(uuid("3050F205-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLLinkElement;
struct __declspec(uuid("3050F206-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLIsIndexElement;
struct __declspec(uuid("3050F207-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLNextIdElement;
struct __declspec(uuid("3050F208-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLBlockElement;
struct __declspec(uuid("3050F209-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLUnknownElement;
struct __declspec(uuid("3050F20A-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLPhraseElement;
struct __declspec(uuid("3050F20C-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLCommentElement;
struct __declspec(uuid("3050F20E-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLListElement;
struct __declspec(uuid("3050F211-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLOptionElement;
struct __declspec(uuid("3050F212-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLDivPosition;
struct __declspec(uuid("3050F216-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLDialog;
struct __declspec(uuid("3050F218-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTextElement;
struct __declspec(uuid("3050F21E-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTable;
struct __declspec(uuid("3050F21F-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLElementCollection;
struct __declspec(uuid("3050F220-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTxtRange;
struct __declspec(uuid("3050F230-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTextContainer;
struct __declspec(uuid("3050F23A-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTableCol;
struct __declspec(uuid("3050F23B-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTableSection;
struct __declspec(uuid("3050F23C-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTableRow;
struct __declspec(uuid("3050F23D-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTableCell;
struct __declspec(uuid("3050F240-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLImgElement;
struct __declspec(uuid("3050F244-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLSelectElement;
struct __declspec(uuid("3050F24F-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLObjectElement;
struct __declspec(uuid("3050F25A-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLSelectionObject;
struct __declspec(uuid("3050F25E-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLStyle;
struct __declspec(uuid("3050F25F-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLEmbedElement;
struct __declspec(uuid("3050F265-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLAreaElement;
struct __declspec(uuid("3050F266-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLMapElement;
struct __declspec(uuid("3050F28B-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLScriptElement;
struct __declspec(uuid("3050F29C-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLControlRange;
struct __declspec(uuid("3050F2A4-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLInputHiddenElement;
struct __declspec(uuid("3050F2A6-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLInputTextElement;
struct __declspec(uuid("3050F2AA-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTextAreaElement;
struct __declspec(uuid("3050F2AD-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLInputFileElement;
struct __declspec(uuid("3050F2B2-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLInputButtonElement;
struct __declspec(uuid("3050F2B5-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLMarqueeElement;
struct __declspec(uuid("3050F2BB-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLButtonElement;
struct __declspec(uuid("3050F2BC-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLOptionButtonElement;
struct __declspec(uuid("3050F2C2-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLInputImage;
struct __declspec(uuid("3050F2E3-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLStyleSheet;
struct __declspec(uuid("3050F2E5-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLStyleSheetRulesCollection;
struct __declspec(uuid("3050F2EB-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTableCaption;
struct __declspec(uuid("3050F2F4-98B5-11CF-BB82-00AA00BDCE0B")) IViewFilterSite;
struct __declspec(uuid("3050F311-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLFrameBase;
struct __declspec(uuid("3050F313-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLFrameElement;
struct __declspec(uuid("3050F315-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLIFrameElement;
struct __declspec(uuid("3050F319-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLFrameSetElement;
struct __declspec(uuid("3050F322-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLTitleElement;
struct __declspec(uuid("3050F32A-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLLabelElement;
struct __declspec(uuid("3050F32D-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLEventObj;
struct __declspec(uuid("3050F357-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLStyleSheetRule;
struct __declspec(uuid("3050F35C-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLScreen;
struct __declspec(uuid("3050F35F-98B5-11CF-BB82-00AA00BDCE0B")) ITimerService;
struct __declspec(uuid("3050F360-98B5-11CF-BB82-00AA00BDCE0B")) ITimer;
struct __declspec(uuid("3050F361-98B5-11CF-BB82-00AA00BDCE0B")) ITimerSink;
struct __declspec(uuid("3050F369-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLBGsound;
struct __declspec(uuid("3050F372-98B5-11CF-BB82-00AA00BDCE0B")) IViewTransition;
struct __declspec(uuid("3050F373-98B5-11CF-BB82-00AA00BDCE0B")) IViewTransitionSite;
struct __declspec(uuid("3050F375-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLStyleElement;
struct __declspec(uuid("3050F376-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLFontNamesCollection;
struct __declspec(uuid("3050F377-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLFontSizesCollection;
struct __declspec(uuid("3050F378-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLOptionsHolder;
struct __declspec(uuid("3050F37E-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLStyleSheetsCollection;
struct __declspec(uuid("3050F383-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLAreasCollection;
struct __declspec(uuid("3050F38A-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLNoShowElement;
struct __declspec(uuid("3050F38C-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLOptionElementFactory;
struct __declspec(uuid("3050F38E-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLImageElementFactory;
struct __declspec(uuid("3050F3CF-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLRuleStyle;
struct __declspec(uuid("3050F3D5-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLStyleFontFace;
struct __declspec(uuid("3050F3E5-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLSpanFlow;
struct __declspec(uuid("3050F3E7-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLFieldSetElement;
struct __declspec(uuid("3050F3EA-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLLegendElement;
struct __declspec(uuid("3050F3EC-98B5-11CF-BB82-00AA00BDCE0B")) ICSSFilter;
struct __declspec(uuid("3050F3ED-98B5-11CF-BB82-00AA00BDCE0B")) ICSSFilterSite;
struct __declspec(uuid("3050F3EE-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLFiltersCollection;
struct __declspec(uuid("3050F3F0-98B5-11CF-BB82-00AA00BDCE0B")) ICustomDoc;
struct __declspec(uuid("3050F3F2-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLDatabinding;
struct __declspec(uuid("3050F3F3-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLSpanElement;
struct __declspec(uuid("3050F3FC-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLMimeTypesCollection;
struct __declspec(uuid("3050F3FD-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLPluginsCollection;
struct __declspec(uuid("3050F401-98B5-11CF-BB82-00AA00BDCE0B")) IHTMLOpsProfile;
struct __declspec(uuid("3050f3d7-98b5-11cf-bb82-00aa00bdce0b")) IImgCtx;
struct __declspec(uuid("332C4425-26CB-11D0-B483-00C04FD90119")) IHTMLDocument2;
struct __declspec(uuid("332C4426-26CB-11D0-B483-00C04FD90119")) IHTMLFramesCollection2;
struct __declspec(uuid("332C4427-26CB-11D0-B483-00C04FD90119")) IHTMLWindow2;
struct __declspec(uuid("359F3441-BD4A-11D0-B188-00AA0038C969")) IMLangFontLink;
struct __declspec(uuid("359F3443-BD4A-11D0-B188-00AA0038C969")) IMLangCodePages;
struct __declspec(uuid("3C374A41-BAE4-11CF-BF7D-00AA006946EE")) IUrlHistoryStg;
struct __declspec(uuid("3C374A42-BAE4-11CF-BF7D-00AA006946EE")) IEnumSTATURL;
struct __declspec(uuid("3DC39D1D-C030-11D0-B81B-00C04FC9B31F")) IEnumRfc1766;
struct __declspec(uuid("47d2657a-7b27-11d0-8ca9-00a0c92dbfe8")) IDockingWindowFrame;
struct __declspec(uuid("539698A0-CDCA-11CF-A5EB-00AA0047A063")) IActiveScriptSiteInterruptPoll;
struct __declspec(uuid("618736e0-3c3d-11cf-810c-00aa00389b71")) IAccessible;
struct __declspec(uuid("626FC520-A41E-11CF-A731-00A0C9082637")) IHTMLDocument;
struct __declspec(uuid("63CDBCB0-C1B1-11D0-9336-00A0C90DCAA9")) IBindEventHandler;
struct __declspec(uuid("68284faa-6a48-11d0-8c78-00c04fd918b4")) IInputObject;
struct __declspec(uuid("71EE5B20-FB04-11d1-B3A8-00A0C911E8B2")) IActiveScriptParseProcedure2_32;
struct __declspec(uuid("729FE2F8-1EA8-11D1-8F85-00C04FC2FBE1")) IShellUIHelper;
struct __declspec(uuid("744129E0-CBE5-11CE-8350-444553540000")) FolderItems;
struct __declspec(uuid("79EAC9C2-BAF9-11CE-8C82-00AA004BA90B")) IHlinkSite;
struct __declspec(uuid("79EAC9C3-BAF9-11CE-8C82-00AA004BA90B")) IHlink;
struct __declspec(uuid("79EAC9C4-BAF9-11CE-8C82-00AA004BA90B")) IHlinkTarget;
struct __declspec(uuid("79EAC9C5-BAF9-11CE-8C82-00AA004BA90B")) IHlinkFrame;
struct __declspec(uuid("79EAC9C6-BAF9-11CE-8C82-00AA004BA90B")) IEnumHLITEM;
struct __declspec(uuid("79EAC9C7-BAF9-11CE-8C82-00AA004BA90B")) IHlinkBrowseContext;
struct __declspec(uuid("79EAC9CB-BAF9-11CE-8C82-00AA004BA90B")) IExtensionServices;
struct __declspec(uuid("7d688a70-c613-11d0-999b-00c04fd655e1")) IShellIconOverlay;
struct __declspec(uuid("85BD8E82-0FBA-11D1-90C3-00C04FC2F568")) IChannelMgr;
struct __declspec(uuid("85CB6900-4D95-11CF-960C-0080C7F4EE85")) IShellWindows;
struct __declspec(uuid("88A05C00-F000-11CE-8350-444553540000")) IShellLinkDual;
struct __declspec(uuid("88e39e80-3578-11cf-ae69-08002b2e1262")) IShellView2;
struct __declspec(uuid("89BCB740-6119-101A-BCB7-00DD010655AF")) IFilter;
struct __declspec(uuid("91A565C1-E38F-11D0-94BF-00A0C9055CBF")) IPersistHistory;
struct __declspec(uuid("9BA05970-F6A8-11CF-A442-00A0C90A8F39")) IFolderViewOC;
struct __declspec(uuid("A3CCEDF3-2DE2-11D0-86F4-00A0C913F750")) IImageDecodeFilter;
struct __declspec(uuid("A4C65425-0F82-11D1-90C3-00C04FC2F568")) IEnumChannels;
struct __declspec(uuid("A6EF9860-C720-11D0-9337-00A0C90DCAA9")) IDispatchEx;
struct __declspec(uuid("A6EF9861-C720-11D0-9337-00A0C90DCAA9")) IDispError;
struct __declspec(uuid("A6EF9862-C720-11D0-9337-00A0C90DCAA9")) IVariantChangeType;
struct __declspec(uuid("AA5B6A80-B834-11D0-932F-00A0C90DCAA9")) IActiveScriptParseProcedure32;
struct __declspec(uuid("AFA0DC11-C313-11D0-831A-00C04FD5AE38")) IUrlHistoryStg2;
struct __declspec(uuid("B722BCC5-4E68-101B-A2BC-00AA00404770")) IOleDocument;
struct __declspec(uuid("B722BCC6-4E68-101B-A2BC-00AA00404770")) IOleDocumentView;
struct __declspec(uuid("B722BCC7-4E68-101B-A2BC-00AA00404770")) IOleDocumentSite;
struct __declspec(uuid("B722BCC8-4E68-101B-A2BC-00AA00404770")) IEnumOleDocumentViews;
struct __declspec(uuid("B722BCC9-4E68-101B-A2BC-00AA00404770")) IPrint;
struct __declspec(uuid("B722BCCA-4E68-101B-A2BC-00AA00404770")) IContinueCallback;
struct __declspec(uuid("B722BCCB-4E68-101B-A2BC-00AA00404770")) IOleCommandTarget;
struct __declspec(uuid("B8DA6310-E19B-11D0-933C-00A0C90DCAA9")) IActiveScriptStats;
struct __declspec(uuid("BAA342A0-2DED-11D0-86F4-00A0C913F750")) IImageDecodeEventSink;
struct __declspec(uuid("BB1A2AE1-A4F9-11CF-8F20-00805F2CD064")) IActiveScript;
struct __declspec(uuid("BB1A2AE2-A4F9-11CF-8F20-00805F2CD064")) IActiveScriptParse32;
struct __declspec(uuid("BC40BEC1-C493-11D0-831B-00C04FD5AE38")) IUrlHistoryNotify;
struct __declspec(uuid("BD3F23C0-D43E-11CF-893B-00AA00BDCE1A")) IDocHostUIHandler;
struct __declspec(uuid("C04D65CE-B70D-11D0-B188-00AA0038C969")) IMLangString;
struct __declspec(uuid("C04D65D0-B70D-11D0-B188-00AA0038C969")) IMLangStringWStr;
struct __declspec(uuid("C04D65D2-B70D-11D0-B188-00AA0038C969")) IMLangStringAStr;
struct __declspec(uuid("C4D244B0-D43E-11CF-893B-00AA00BDCE1A")) IDocHostShowUI;
struct __declspec(uuid("C64713B6-E029-4CC5-9200-438B72890B6A")) IActiveScriptParseProcedure64;
struct __declspec(uuid("C7EF7658-E1EE-480E-97EA-D52CB4D76D17")) IActiveScriptParse64;
struct __declspec(uuid("CA04B7E6-0D21-11D1-8CC5-00C04FC2B085")) IObjectIdentity;
struct __declspec(uuid("CB5BDC81-93C1-11CF-8F20-00805F2CD064")) IObjectSafety;
struct __declspec(uuid("D10F6761-83E9-11CF-8F20-00805F2CD064")) IActiveScriptSiteWindow;
struct __declspec(uuid("D24ACD21-BA72-11D0-B188-00AA0038C969")) IMLangStringBufW;
struct __declspec(uuid("D24ACD23-BA72-11D0-B188-00AA0038C969")) IMLangStringBufA;
struct __declspec(uuid("D30C1661-CDAF-11D0-8A3E-00C04FC9E26E")) IWebBrowser2;
struct __declspec(uuid("D66D6F98-CDAA-11D0-B822-00C04FC9B31F")) IMLangConvertCharset;
struct __declspec(uuid("D8F015C0-C278-11CE-A49E-444553540000")) IShellDispatch;
struct __declspec(uuid("D9E89500-30FA-11D0-B724-00AA006C1A01")) IMapMIMEToCLSID;
struct __declspec(uuid("DB01A1E3-A42B-11CF-8F20-00805F2CD064")) IActiveScriptSite;
struct __declspec(uuid("E0E270C0-C0BE-11D0-8FE4-00A0C90A6341")) OLEDBSimpleProvider;
struct __declspec(uuid("E0E270C1-C0BE-11D0-8FE4-00A0C90A6341")) OLEDBSimpleProviderListener;
struct __declspec(uuid("E7A1AF80-4D96-11CF-960C-0080C7F4EE85")) IShellFolderViewDual;
struct __declspec(uuid("EAB22AC1-30C1-11CF-A7EB-0000C05BAE0B")) IWebBrowser;
struct __declspec(uuid("EAE1BA61-A4ED-11CF-8F20-00805F2CD064")) IActiveScriptError;
struct __declspec(uuid("F5BE2EE1-BFD7-11D0-B188-00AA0038C969")) IMLangLineBreakConsole;
struct __declspec(uuid("F77459A0-BF9A-11CF-BA4E-00C04FD70816")) IMimeInfo;
struct __declspec(uuid("FAC32C80-CBE4-11CE-8350-444553540000")) FolderItem;
struct __declspec(uuid("FE7C4271-210C-448D-9F54-76DAB7047B28")) IActiveScriptParseProcedure2_64;
struct __declspec(uuid("FECEAAA2-8405-11CF-8BA1-00AA00476DA6")) IOmHistory;
struct __declspec(uuid("FECEAAA5-8405-11CF-8BA1-00AA00476DA6")) IOmNavigator;
struct __declspec(uuid("ac60f6a0-0fd9-11d0-99cb-00c04fd64497")) IURLSearchHook;
struct __declspec(uuid("bcfce0a0-ec17-11d0-8d10-00a0c90f2719")) IContextMenu3;
struct __declspec(uuid("cabb0da0-da57-11cf-9974-0020afd79762")) IUniformResourceLocatorW;
struct __declspec(uuid("eb0fe172-1a3a-11d0-89b3-00a0c90a90ac")) IDeskBand;
struct __declspec(uuid("f1db8392-7331-11d0-8c99-00a0c92dbfe8")) IInputObjectSite;
struct __declspec(uuid("f490eb00-1240-11d1-9888-006097deacf9")) IActiveDesktop;
struct __declspec(uuid("fbf23b80-e3f0-101b-8488-00aa003e56f8")) IUniformResourceLocatorA;

// CoClasses:

class __declspec(uuid("00000017-0000-0000-c000-000000000046")) StdMarshal;
class __declspec(uuid("0000001b-0000-0000-c000-000000000046")) IdentityUnmarshal;
class __declspec(uuid("0000001c-0000-0000-c000-000000000046")) InProcFreeMarshaler;
class __declspec(uuid("0000030c-0000-0000-c000-000000000046")) PSGenObject;
class __declspec(uuid("0000030d-0000-0000-c000-000000000046")) PSClientSite;
class __declspec(uuid("0000030e-0000-0000-c000-000000000046")) PSClassObject;
class __declspec(uuid("0000030f-0000-0000-c000-000000000046")) PSInPlaceActive;
class __declspec(uuid("00000310-0000-0000-c000-000000000046")) PSInPlaceFrame;
class __declspec(uuid("00000311-0000-0000-c000-000000000046")) PSDragDrop;
class __declspec(uuid("00000312-0000-0000-c000-000000000046")) PSBindCtx;
class __declspec(uuid("00000313-0000-0000-c000-000000000046")) PSEnumerators;
class __declspec(uuid("00000315-0000-0000-c000-000000000046")) Picture_Metafile;
class __declspec(uuid("00000315-0000-0000-c000-000000000046")) StaticMetafile;
class __declspec(uuid("00000316-0000-0000-c000-000000000046")) Picture_Dib;
class __declspec(uuid("00000316-0000-0000-c000-000000000046")) StaticDib;
class __declspec(uuid("00000319-0000-0000-c000-000000000046")) Picture_EnhMetafile;
class __declspec(uuid("0000031d-0000-0000-c000-000000000046")) DCOMAccessControl;
class __declspec(uuid("00021400-0000-0000-c000-000000000046")) ShellDesktop;
class __declspec(uuid("00021401-0000-0000-c000-000000000046")) ShellLink;
class __declspec(uuid("0002DF01-0000-0000-C000-000000000046")) InternetExplorer;
class __declspec(uuid("0002e005-0000-0000-c000-000000000046")) StdComponentCategoriesMgr;
class __declspec(uuid("08165ea0-e946-11cf-9c87-00aa005127ed")) WebCrawlerAgent;
class __declspec(uuid("0A89A860-D7B1-11CE-8350-444553540000")) ShellDispatchInproc;
class __declspec(uuid("0D04D285-6BEC-11CF-8B97-00AA00476DA6")) OldHTMLFormElement;
class __declspec(uuid("0be35200-8f91-11ce-9de3-00aa004bb851")) CFontPropPage;
class __declspec(uuid("0be35201-8f91-11ce-9de3-00aa004bb851")) CColorPropPage;
class __declspec(uuid("0be35202-8f91-11ce-9de3-00aa004bb851")) CPicturePropPage;
class __declspec(uuid("0be35203-8f91-11ce-9de3-00aa004bb851")) StdFont;
class __declspec(uuid("0be35204-8f91-11ce-9de3-00aa004bb851")) StdPicture;
class __declspec(uuid("11219420-1768-11D1-95BE-00609797EA4F")) ShellLinkObject;
class __declspec(uuid("163BB1E1-6E00-11CF-837A-48DC04C10000")) HTMLLocation;
class __declspec(uuid("1820FED0-473E-11D0-A96C-00C04FD705A2")) WebViewFolderContents;
class __declspec(uuid("25336920-03F9-11CF-8FD0-00AA00686F13")) HTMLDocument;
class __declspec(uuid("25336921-03f9-11cf-8fd0-00aa00686f13")) HTMLPluginDocument;
class __declspec(uuid("275C23E2-3747-11D0-9FEA-00AA003F8646")) CMultiLanguage;
class __declspec(uuid("3050F241-98B5-11CF-BB82-00AA00BDCE0B")) HTMLImg;
class __declspec(uuid("3050F245-98B5-11CF-BB82-00AA00BDCE0B")) HTMLSelectElement;
class __declspec(uuid("3050F246-98B5-11CF-BB82-00AA00BDCE0B")) HTMLTableCell;
class __declspec(uuid("3050F248-98B5-11CF-BB82-00AA00BDCE0B")) HTMLAnchorElement;
class __declspec(uuid("3050F249-98B5-11CF-BB82-00AA00BDCE0B")) HTMLDivPosition;
class __declspec(uuid("3050F24A-98B5-11CF-BB82-00AA00BDCE0B")) HTMLBody;
class __declspec(uuid("3050F24D-98B5-11CF-BB82-00AA00BDCE0B")) HTMLOptionElement;
class __declspec(uuid("3050F24E-98B5-11CF-BB82-00AA00BDCE0B")) HTMLObjectElement;
class __declspec(uuid("3050F251-98B5-11CF-BB82-00AA00BDCE0B")) HTMLFormElement;
class __declspec(uuid("3050F252-98B5-11CF-BB82-00AA00BDCE0B")) HTMLHRElement;
class __declspec(uuid("3050F25D-98B5-11CF-BB82-00AA00BDCE0B")) HTMLEmbed;
class __declspec(uuid("3050F268-98B5-11CF-BB82-00AA00BDCE0B")) HTMLUnknownElement;
class __declspec(uuid("3050F269-98B5-11CF-BB82-00AA00BDCE0B")) HTMLUListElement;
class __declspec(uuid("3050F26A-98B5-11CF-BB82-00AA00BDCE0B")) HTMLTextElement;
class __declspec(uuid("3050F26B-98B5-11CF-BB82-00AA00BDCE0B")) HTMLTable;
class __declspec(uuid("3050F26C-98B5-11CF-BB82-00AA00BDCE0B")) HTMLTableCol;
class __declspec(uuid("3050F26D-98B5-11CF-BB82-00AA00BDCE0B")) HTMLTableRow;
class __declspec(uuid("3050F26E-98B5-11CF-BB82-00AA00BDCE0B")) HTMLPhraseElement;
class __declspec(uuid("3050F26F-98B5-11CF-BB82-00AA00BDCE0B")) HTMLParaElement;
class __declspec(uuid("3050F270-98B5-11CF-BB82-00AA00BDCE0B")) HTMLOListElement;
class __declspec(uuid("3050F271-98B5-11CF-BB82-00AA00BDCE0B")) HTMLMapElement;
class __declspec(uuid("3050F272-98B5-11CF-BB82-00AA00BDCE0B")) HTMLListElement;
class __declspec(uuid("3050F273-98B5-11CF-BB82-00AA00BDCE0B")) HTMLLIElement;
class __declspec(uuid("3050F275-98B5-11CF-BB82-00AA00BDCE0B")) HTMLMetaElement;
class __declspec(uuid("3050F276-98B5-11CF-BB82-00AA00BDCE0B")) HTMLBaseElement;
class __declspec(uuid("3050F277-98B5-11CF-BB82-00AA00BDCE0B")) HTMLLinkElement;
class __declspec(uuid("3050F278-98B5-11CF-BB82-00AA00BDCE0B")) HTMLIsIndexElement;
class __declspec(uuid("3050F279-98B5-11CF-BB82-00AA00BDCE0B")) HTMLNextIdElement;
class __declspec(uuid("3050F27A-98B5-11CF-BB82-00AA00BDCE0B")) HTMLHeaderElement;
class __declspec(uuid("3050F27B-98B5-11CF-BB82-00AA00BDCE0B")) HTMLFontElement;
class __declspec(uuid("3050F27C-98B5-11CF-BB82-00AA00BDCE0B")) HTMLDTElement;
class __declspec(uuid("3050F27D-98B5-11CF-BB82-00AA00BDCE0B")) HTMLDListElement;
class __declspec(uuid("3050F27E-98B5-11CF-BB82-00AA00BDCE0B")) HTMLDivElement;
class __declspec(uuid("3050F27F-98B5-11CF-BB82-00AA00BDCE0B")) HTMLDDElement;
class __declspec(uuid("3050F280-98B5-11CF-BB82-00AA00BDCE0B")) HTMLBRElement;
class __declspec(uuid("3050F281-98B5-11CF-BB82-00AA00BDCE0B")) HTMLBlockElement;
class __declspec(uuid("3050F282-98B5-11CF-BB82-00AA00BDCE0B")) HTMLBaseFontElement;
class __declspec(uuid("3050F283-98B5-11CF-BB82-00AA00BDCE0B")) HTMLAreaElement;
class __declspec(uuid("3050F284-98B5-11CF-BB82-00AA00BDCE0B")) HTMLTitleElement;
class __declspec(uuid("3050F285-98B5-11CF-BB82-00AA00BDCE0B")) HTMLStyle;
class __declspec(uuid("3050F28A-98B5-11CF-BB82-00AA00BDCE0B")) HTMLDialog;
class __declspec(uuid("3050F28C-98B5-11CF-BB82-00AA00BDCE0B")) HTMLScriptElement;
class __declspec(uuid("3050F2AB-98B5-11CF-BB82-00AA00BDCE0B")) HTMLInputTextElement;
class __declspec(uuid("3050F2AC-98B5-11CF-BB82-00AA00BDCE0B")) HTMLTextAreaElement;
class __declspec(uuid("3050F2AE-98B5-11CF-BB82-00AA00BDCE0B")) HTMLInputFileElement;
class __declspec(uuid("3050F2B4-98B5-11CF-BB82-00AA00BDCE0B")) HTMLInputButtonElement;
class __declspec(uuid("3050F2B9-98B5-11CF-BB82-00AA00BDCE0B")) HTMLMarqueeElement;
class __declspec(uuid("3050F2BE-98B5-11CF-BB82-00AA00BDCE0B")) HTMLOptionButtonElement;
class __declspec(uuid("3050F2C4-98B5-11CF-BB82-00AA00BDCE0B")) HTMLInputImage;
class __declspec(uuid("3050F2C6-98B5-11CF-BB82-00AA00BDCE0B")) HTMLButtonElement;
class __declspec(uuid("3050F2E4-98B5-11CF-BB82-00AA00BDCE0B")) HTMLStyleSheet;
class __declspec(uuid("3050F2E9-98B5-11CF-BB82-00AA00BDCE0B")) HTMLTableSection;
class __declspec(uuid("3050F2EC-98B5-11CF-BB82-00AA00BDCE0B")) HTMLTableCaption;
class __declspec(uuid("3050F312-98B5-11CF-BB82-00AA00BDCE0B")) HTMLFrameBase;
class __declspec(uuid("3050F314-98B5-11CF-BB82-00AA00BDCE0B")) HTMLFrameElement;
class __declspec(uuid("3050F316-98B5-11CF-BB82-00AA00BDCE0B")) HTMLIFrame;
class __declspec(uuid("3050F317-98B5-11CF-BB82-00AA00BDCE0B")) HTMLCommentElement;
class __declspec(uuid("3050F31A-98B5-11CF-BB82-00AA00BDCE0B")) HTMLFrameSetSite;
class __declspec(uuid("3050F32B-98B5-11CF-BB82-00AA00BDCE0B")) HTMLLabelElement;
class __declspec(uuid("3050F35D-98B5-11CF-BB82-00AA00BDCE0B")) HTMLScreen;
class __declspec(uuid("3050F370-98B5-11CF-BB82-00AA00BDCE0B")) HTMLBGsound;
class __declspec(uuid("3050F37D-98B5-11CF-BB82-00AA00BDCE0B")) HTMLStyleElement;
class __declspec(uuid("3050F37F-98B5-11CF-BB82-00AA00BDCE0B")) HTMLStyleSheetsCollection;
class __declspec(uuid("3050F38B-98B5-11CF-BB82-00AA00BDCE0B")) HTMLNoShowElement;
class __declspec(uuid("3050F38D-98B5-11CF-BB82-00AA00BDCE0B")) HTMLOptionElementFactory;
class __declspec(uuid("3050F38F-98B5-11CF-BB82-00AA00BDCE0B")) HTMLImageElementFactory;
class __declspec(uuid("3050F391-98B5-11CF-BB82-00AA00BDCE0B")) HTMLWindowProxy;
class __declspec(uuid("3050F3CD-98B5-11CF-BB82-00AA00BDCE0B")) HTMLStyleSheetRulesCollection;
class __declspec(uuid("3050F3CE-98B5-11CF-BB82-00AA00BDCE0B")) HTMLStyleSheetRule;
class __declspec(uuid("3050F3D0-98B5-11CF-BB82-00AA00BDCE0B")) HTMLRuleStyle;
class __declspec(uuid("3050F3D4-98B5-11CF-BB82-00AA00BDCE0B")) HTMLStyleFontFace;
class __declspec(uuid("3050F3E6-98B5-11CF-BB82-00AA00BDCE0B")) HTMLSpanFlow;
class __declspec(uuid("3050F3E8-98B5-11CF-BB82-00AA00BDCE0B")) HTMLFieldSetElement;
class __declspec(uuid("3050F3E9-98B5-11CF-BB82-00AA00BDCE0B")) HTMLLegendElement;
class __declspec(uuid("3050F3EF-98B5-11CF-BB82-00AA00BDCE0B")) HTMLFiltersCollection;
class __declspec(uuid("3050F3F5-98B4-11CF-BB82-00AA00BDCE0B")) HTMLSpanElement;
class __declspec(uuid("3050F3FE-98B5-11CF-BB82-00AA00BDCE0B")) CMimeTypes;
class __declspec(uuid("3050F3FF-98B5-11CF-BB82-00AA00BDCE0B")) CPlugins;
class __declspec(uuid("3050F402-98B5-11CF-BB82-00AA00BDCE0B")) COpsProfile;
class __declspec(uuid("3050f3d9-98b5-11cf-bb82-00aa00bdce0b")) MHTMLDocument;
class __declspec(uuid("32b533bb-edae-11d0-bd5a-00aa00b92af1")) ClassInstallFilter;
class __declspec(uuid("3c374a40-bae4-11cf-bf7d-00aa006946ee")) CUrlHistory;
class __declspec(uuid("3dd53d40-7b8b-11d0-b013-00aa0059ce02")) CdlProtocol;
class __declspec(uuid("54c37cd0-d944-11d0-a9f4-006097942311")) StdEncodingFilterFac;
class __declspec(uuid("56fdf344-fd6d-11d0-958a-006097c9a090")) TaskbarList;
class __declspec(uuid("62112AA1-EBE4-11CF-A5FB-0020AFE7292D")) ShellFolderView;
class __declspec(uuid("63b51f81-c868-11d0-999c-00c04fd655e1")) CFSIconOverlayManager;
class __declspec(uuid("64AB4BB7-111E-11D1-8F79-00C04FC2FBE1")) ShellUIHelper;
class __declspec(uuid("75048700-ef1f-11d0-9888-006097deacf9")) ActiveDesktop;
class __declspec(uuid("79eac9d0-baf9-11ce-8c82-00aa004ba90b")) StdHlink;
class __declspec(uuid("79eac9d1-baf9-11ce-8c82-00aa004ba90b")) StdHlinkBrowseContext;
class __declspec(uuid("79eac9e0-baf9-11ce-8c82-00aa004ba90b")) StdURLMoniker;
class __declspec(uuid("79eac9e1-baf9-11ce-8c82-00aa004ba90b")) StdURLProtocol;
class __declspec(uuid("79eac9e2-baf9-11ce-8c82-00aa004ba90b")) HttpProtocol;
class __declspec(uuid("79eac9e3-baf9-11ce-8c82-00aa004ba90b")) FtpProtocol;
class __declspec(uuid("79eac9e4-baf9-11ce-8c82-00aa004ba90b")) GopherProtocol;
class __declspec(uuid("79eac9e5-baf9-11ce-8c82-00aa004ba90b")) HttpSProtocol;
class __declspec(uuid("79eac9e6-baf9-11ce-8c82-00aa004ba90b")) MkProtocol;
class __declspec(uuid("79eac9e7-baf9-11ce-8c82-00aa004ba90b")) FileProtocol;
class __declspec(uuid("79eac9f2-baf9-11ce-8c82-00aa004ba90b")) UrlMkBindCtx;
class __declspec(uuid("7b8a2d94-0ac9-11d1-896c-00c04fb6bfc4")) InternetSecurityManager;
class __declspec(uuid("7b8a2d95-0ac9-11d1-896c-00c04fb6bfc4")) InternetZoneManager;
class __declspec(uuid("7d559c10-9fe9-11d0-93f7-00aa0059ce02")) CDLAgent;
class __declspec(uuid("7d688a77-c613-11d0-999b-00c04fd655e1")) OverlayIdentifier_SlowFile;
class __declspec(uuid("7ebdaae0-8120-11cf-899f-00aa00688b10")) StockFontPage;
class __declspec(uuid("7ebdaae1-8120-11cf-899f-00aa00688b10")) StockColorPage;
class __declspec(uuid("7ebdaae2-8120-11cf-899f-00aa00688b10")) StockPicturePage;
class __declspec(uuid("8856F961-340A-11D0-A96B-00C04FD705A2")) WebBrowser;
class __declspec(uuid("8f6b0360-b80d-11d0-a9b3-006097942311")) DeCompMimeFilter;
class __declspec(uuid("9BA05971-F6A8-11CF-A442-00A0C90A8F39")) ShellFolderViewOC;
class __declspec(uuid("9BA05972-F6A8-11CF-A442-00A0C90A8F39")) ShellWindows;
class __declspec(uuid("ABBE31D0-6DAE-11D0-BECA-00C04FD940BE")) SubscriptionMgr;
class __declspec(uuid("B3CDAE90-D170-11D0-802B-00C04FD75D13")) ChannelMgr;
class __declspec(uuid("C04D65CF-B70D-11D0-B188-00AA0038C969")) CMLangString;
class __declspec(uuid("D48A6EC6-6A4A-11CF-94A7-444553540000")) HTMLWindow2;
class __declspec(uuid("D48A6EC9-6A4A-11CF-94A7-444553540000")) OldHTMLDocument;
class __declspec(uuid("D66D6F99-CDAA-11D0-B822-00C04FC9B31F")) CMLangConvertCharset;
class __declspec(uuid("EAB22AC3-30C1-11CF-A7EB-0000C05BAE0B")) WebBrowser_V1;
class __declspec(uuid("FECEAAA3-8405-11CF-8BA1-00AA00476DA6")) HTMLHistory;
class __declspec(uuid("FECEAAA6-8405-11CF-8BA1-00AA00476DA6")) HTMLNavigator;
class __declspec(uuid("b15b8dc0-c7e1-11d0-8680-00aa00bdcb71")) SoftDistExt;
class __declspec(uuid("cfbfae00-17a6-11d0-99cb-00c04fd64497")) CURLSearchHook;
class __declspec(uuid("e3a8bde6-abce-11d0-bc4b-00c04fd929db")) ChannelAgent;
class __declspec(uuid("fb8f0821-0164-101b-84ed-08002b2ec713")) PersistPropset;
class __declspec(uuid("fb8f0822-0164-101b-84ed-08002b2ec713")) ConvertVBX;
class __declspec(uuid("fbf23b40-e3f0-101b-8488-00aa003e56f8")) InternetShortcut;

// Interface Smart Pointers:

_COM_SMARTPTR_TYPEDEF(FolderItem, __uuidof(FolderItem));
_COM_SMARTPTR_TYPEDEF(FolderItemVerb, __uuidof(FolderItemVerb));
_COM_SMARTPTR_TYPEDEF(FolderItemVerbs, __uuidof(FolderItemVerbs));
_COM_SMARTPTR_TYPEDEF(FolderItems, __uuidof(FolderItems));
_COM_SMARTPTR_TYPEDEF(IAccessible, __uuidof(IAccessible));
_COM_SMARTPTR_TYPEDEF(IActiveDesktop, __uuidof(IActiveDesktop));
_COM_SMARTPTR_TYPEDEF(IActiveScript, __uuidof(IActiveScript));
_COM_SMARTPTR_TYPEDEF(IActiveScriptError, __uuidof(IActiveScriptError));
_COM_SMARTPTR_TYPEDEF(IActiveScriptParse, __uuidof(IActiveScriptParse));
_COM_SMARTPTR_TYPEDEF(IActiveScriptParseProcedure, __uuidof(IActiveScriptParseProcedure));
_COM_SMARTPTR_TYPEDEF(IActiveScriptParseProcedure2, __uuidof(IActiveScriptParseProcedure2));
_COM_SMARTPTR_TYPEDEF(IActiveScriptParseProcedureOld, __uuidof(IActiveScriptParseProcedureOld));
_COM_SMARTPTR_TYPEDEF(IActiveScriptSite, __uuidof(IActiveScriptSite));
_COM_SMARTPTR_TYPEDEF(IActiveScriptSiteInterruptPoll, __uuidof(IActiveScriptSiteInterruptPoll));
_COM_SMARTPTR_TYPEDEF(IActiveScriptSiteWindow, __uuidof(IActiveScriptSiteWindow));
_COM_SMARTPTR_TYPEDEF(IActiveScriptStats, __uuidof(IActiveScriptStats));
_COM_SMARTPTR_TYPEDEF(IAdviseSink, __uuidof(IAdviseSink));
_COM_SMARTPTR_TYPEDEF(IAdviseSink2, __uuidof(IAdviseSink2));
_COM_SMARTPTR_TYPEDEF(IAdviseSinkEx, __uuidof(IAdviseSinkEx));
_COM_SMARTPTR_TYPEDEF(IAsyncManager, __uuidof(IAsyncManager));
_COM_SMARTPTR_TYPEDEF(IAuthenticate, __uuidof(IAuthenticate));
_COM_SMARTPTR_TYPEDEF(IBindCtx, __uuidof(IBindCtx));
_COM_SMARTPTR_TYPEDEF(IBindEventHandler, __uuidof(IBindEventHandler));
_COM_SMARTPTR_TYPEDEF(IBindHost, __uuidof(IBindHost));
_COM_SMARTPTR_TYPEDEF(IBindProtocol, __uuidof(IBindProtocol));
_COM_SMARTPTR_TYPEDEF(IBindStatusCallback, __uuidof(IBindStatusCallback));
_COM_SMARTPTR_TYPEDEF(IBinding, __uuidof(IBinding));
_COM_SMARTPTR_TYPEDEF(ICSSFilter, __uuidof(ICSSFilter));
_COM_SMARTPTR_TYPEDEF(ICSSFilterSite, __uuidof(ICSSFilterSite));
_COM_SMARTPTR_TYPEDEF(ICancelMethodCalls, __uuidof(ICancelMethodCalls));
_COM_SMARTPTR_TYPEDEF(ICatInformation, __uuidof(ICatInformation));
_COM_SMARTPTR_TYPEDEF(ICatRegister, __uuidof(ICatRegister));
_COM_SMARTPTR_TYPEDEF(IChannelHook, __uuidof(IChannelHook));
_COM_SMARTPTR_TYPEDEF(IChannelMgr, __uuidof(IChannelMgr));
_COM_SMARTPTR_TYPEDEF(IClassActivator, __uuidof(IClassActivator));
_COM_SMARTPTR_TYPEDEF(IClassFactory, __uuidof(IClassFactory));
_COM_SMARTPTR_TYPEDEF(IClassFactory2, __uuidof(IClassFactory2));
_COM_SMARTPTR_TYPEDEF(IClientSecurity, __uuidof(IClientSecurity));
_COM_SMARTPTR_TYPEDEF(ICodeInstall, __uuidof(ICodeInstall));
_COM_SMARTPTR_TYPEDEF(ICommDlgBrowser, __uuidof(ICommDlgBrowser));
_COM_SMARTPTR_TYPEDEF(IConnectionPoint, __uuidof(IConnectionPoint));
_COM_SMARTPTR_TYPEDEF(IConnectionPointContainer, __uuidof(IConnectionPointContainer));
_COM_SMARTPTR_TYPEDEF(IContextMenu, __uuidof(IContextMenu));
_COM_SMARTPTR_TYPEDEF(IContextMenu2, __uuidof(IContextMenu2));
_COM_SMARTPTR_TYPEDEF(IContextMenu3, __uuidof(IContextMenu3));
_COM_SMARTPTR_TYPEDEF(IContinue, __uuidof(IContinue));
_COM_SMARTPTR_TYPEDEF(IContinueCallback, __uuidof(IContinueCallback));
_COM_SMARTPTR_TYPEDEF(ICreateErrorInfo, __uuidof(ICreateErrorInfo));
_COM_SMARTPTR_TYPEDEF(ICreateTypeInfo, __uuidof(ICreateTypeInfo));
_COM_SMARTPTR_TYPEDEF(ICreateTypeInfo2, __uuidof(ICreateTypeInfo2));
_COM_SMARTPTR_TYPEDEF(ICreateTypeLib, __uuidof(ICreateTypeLib));
_COM_SMARTPTR_TYPEDEF(ICreateTypeLib2, __uuidof(ICreateTypeLib2));
_COM_SMARTPTR_TYPEDEF(ICustomDoc, __uuidof(ICustomDoc));
_COM_SMARTPTR_TYPEDEF(IDataAdviseHolder, __uuidof(IDataAdviseHolder));
_COM_SMARTPTR_TYPEDEF(IDataFilter, __uuidof(IDataFilter));
_COM_SMARTPTR_TYPEDEF(IDataObject, __uuidof(IDataObject));
_COM_SMARTPTR_TYPEDEF(IDeskBand, __uuidof(IDeskBand));
_COM_SMARTPTR_TYPEDEF(IDirectWriterLock, __uuidof(IDirectWriterLock));
_COM_SMARTPTR_TYPEDEF(IDispError, __uuidof(IDispError));
_COM_SMARTPTR_TYPEDEF(IDispatch, __uuidof(IDispatch));
_COM_SMARTPTR_TYPEDEF(IDispatchEx, __uuidof(IDispatchEx));
_COM_SMARTPTR_TYPEDEF(IDocHostShowUI, __uuidof(IDocHostShowUI));
_COM_SMARTPTR_TYPEDEF(IDocHostUIHandler, __uuidof(IDocHostUIHandler));
_COM_SMARTPTR_TYPEDEF(IDockingWindow, __uuidof(IDockingWindow));
_COM_SMARTPTR_TYPEDEF(IDockingWindowFrame, __uuidof(IDockingWindowFrame));
_COM_SMARTPTR_TYPEDEF(IDockingWindowSite, __uuidof(IDockingWindowSite));
_COM_SMARTPTR_TYPEDEF(IDropSource, __uuidof(IDropSource));
_COM_SMARTPTR_TYPEDEF(IDropTarget, __uuidof(IDropTarget));
_COM_SMARTPTR_TYPEDEF(IEncodingFilterFactory, __uuidof(IEncodingFilterFactory));
_COM_SMARTPTR_TYPEDEF(IEnumCATEGORYINFO, __uuidof(IEnumCATEGORYINFO));
_COM_SMARTPTR_TYPEDEF(IEnumChannels, __uuidof(IEnumChannels));
_COM_SMARTPTR_TYPEDEF(IEnumCodePage, __uuidof(IEnumCodePage));
_COM_SMARTPTR_TYPEDEF(IEnumConnectionPoints, __uuidof(IEnumConnectionPoints));
_COM_SMARTPTR_TYPEDEF(IEnumConnections, __uuidof(IEnumConnections));
_COM_SMARTPTR_TYPEDEF(IEnumFORMATETC, __uuidof(IEnumFORMATETC));
_COM_SMARTPTR_TYPEDEF(IEnumGUID, __uuidof(IEnumGUID));
_COM_SMARTPTR_TYPEDEF(IEnumHLITEM, __uuidof(IEnumHLITEM));
_COM_SMARTPTR_TYPEDEF(IEnumIDList, __uuidof(IEnumIDList));
_COM_SMARTPTR_TYPEDEF(IEnumMoniker, __uuidof(IEnumMoniker));
_COM_SMARTPTR_TYPEDEF(IEnumOLEVERB, __uuidof(IEnumOLEVERB));
_COM_SMARTPTR_TYPEDEF(IEnumOleDocumentViews, __uuidof(IEnumOleDocumentViews));
_COM_SMARTPTR_TYPEDEF(IEnumOleUndoUnits, __uuidof(IEnumOleUndoUnits));
_COM_SMARTPTR_TYPEDEF(IEnumRfc1766, __uuidof(IEnumRfc1766));
_COM_SMARTPTR_TYPEDEF(IEnumSTATDATA, __uuidof(IEnumSTATDATA));
_COM_SMARTPTR_TYPEDEF(IEnumSTATPROPSETSTG, __uuidof(IEnumSTATPROPSETSTG));
_COM_SMARTPTR_TYPEDEF(IEnumSTATPROPSTG, __uuidof(IEnumSTATPROPSTG));
_COM_SMARTPTR_TYPEDEF(IEnumSTATSTG, __uuidof(IEnumSTATSTG));
_COM_SMARTPTR_TYPEDEF(IEnumSTATURL, __uuidof(IEnumSTATURL));
_COM_SMARTPTR_TYPEDEF(IEnumString, __uuidof(IEnumString));
_COM_SMARTPTR_TYPEDEF(IEnumUnknown, __uuidof(IEnumUnknown));
_COM_SMARTPTR_TYPEDEF(IEnumVARIANT, __uuidof(IEnumVARIANT));
_COM_SMARTPTR_TYPEDEF(IErrorInfo, __uuidof(IErrorInfo));
_COM_SMARTPTR_TYPEDEF(IErrorLog, __uuidof(IErrorLog));
_COM_SMARTPTR_TYPEDEF(IExtensionServices, __uuidof(IExtensionServices));
_COM_SMARTPTR_TYPEDEF(IExternalConnection, __uuidof(IExternalConnection));
_COM_SMARTPTR_TYPEDEF(IExtractIconA, __uuidof(IExtractIconA));
_COM_SMARTPTR_TYPEDEF(IExtractIconW, __uuidof(IExtractIconW));
_COM_SMARTPTR_TYPEDEF(IFileViewerA, __uuidof(IFileViewerA));
_COM_SMARTPTR_TYPEDEF(IFileViewerSite, __uuidof(IFileViewerSite));
_COM_SMARTPTR_TYPEDEF(IFileViewerW, __uuidof(IFileViewerW));
_COM_SMARTPTR_TYPEDEF(IFillLockBytes, __uuidof(IFillLockBytes));
_COM_SMARTPTR_TYPEDEF(IFilter, __uuidof(IFilter));
_COM_SMARTPTR_TYPEDEF(IFolderViewOC, __uuidof(IFolderViewOC));
_COM_SMARTPTR_TYPEDEF(IFont, __uuidof(IFont));
_COM_SMARTPTR_TYPEDEF(IFontDisp, __uuidof(IFontDisp));
_COM_SMARTPTR_TYPEDEF(IFontEventsDisp, __uuidof(IFontEventsDisp));
_COM_SMARTPTR_TYPEDEF(IGlobalInterfaceTable, __uuidof(IGlobalInterfaceTable));
_COM_SMARTPTR_TYPEDEF(IHTMLAnchorElement, __uuidof(IHTMLAnchorElement));
_COM_SMARTPTR_TYPEDEF(IHTMLAreaElement, __uuidof(IHTMLAreaElement));
_COM_SMARTPTR_TYPEDEF(IHTMLAreasCollection, __uuidof(IHTMLAreasCollection));
_COM_SMARTPTR_TYPEDEF(IHTMLBGsound, __uuidof(IHTMLBGsound));
_COM_SMARTPTR_TYPEDEF(IHTMLBRElement, __uuidof(IHTMLBRElement));
_COM_SMARTPTR_TYPEDEF(IHTMLBaseElement, __uuidof(IHTMLBaseElement));
_COM_SMARTPTR_TYPEDEF(IHTMLBaseFontElement, __uuidof(IHTMLBaseFontElement));
_COM_SMARTPTR_TYPEDEF(IHTMLBlockElement, __uuidof(IHTMLBlockElement));
_COM_SMARTPTR_TYPEDEF(IHTMLBodyElement, __uuidof(IHTMLBodyElement));
_COM_SMARTPTR_TYPEDEF(IHTMLButtonElement, __uuidof(IHTMLButtonElement));
_COM_SMARTPTR_TYPEDEF(IHTMLCommentElement, __uuidof(IHTMLCommentElement));
_COM_SMARTPTR_TYPEDEF(IHTMLControlElement, __uuidof(IHTMLControlElement));
_COM_SMARTPTR_TYPEDEF(IHTMLControlRange, __uuidof(IHTMLControlRange));
_COM_SMARTPTR_TYPEDEF(IHTMLDDElement, __uuidof(IHTMLDDElement));
_COM_SMARTPTR_TYPEDEF(IHTMLDListElement, __uuidof(IHTMLDListElement));
_COM_SMARTPTR_TYPEDEF(IHTMLDTElement, __uuidof(IHTMLDTElement));
_COM_SMARTPTR_TYPEDEF(IHTMLDatabinding, __uuidof(IHTMLDatabinding));
_COM_SMARTPTR_TYPEDEF(IHTMLDialog, __uuidof(IHTMLDialog));
_COM_SMARTPTR_TYPEDEF(IHTMLDivElement, __uuidof(IHTMLDivElement));
_COM_SMARTPTR_TYPEDEF(IHTMLDivPosition, __uuidof(IHTMLDivPosition));
_COM_SMARTPTR_TYPEDEF(IHTMLDocument, __uuidof(IHTMLDocument));
_COM_SMARTPTR_TYPEDEF(IHTMLDocument2, __uuidof(IHTMLDocument2));
_COM_SMARTPTR_TYPEDEF(IHTMLElement, __uuidof(IHTMLElement));
_COM_SMARTPTR_TYPEDEF(IHTMLElementCollection, __uuidof(IHTMLElementCollection));
_COM_SMARTPTR_TYPEDEF(IHTMLEmbedElement, __uuidof(IHTMLEmbedElement));
_COM_SMARTPTR_TYPEDEF(IHTMLEventObj, __uuidof(IHTMLEventObj));
_COM_SMARTPTR_TYPEDEF(IHTMLFieldSetElement, __uuidof(IHTMLFieldSetElement));
_COM_SMARTPTR_TYPEDEF(IHTMLFiltersCollection, __uuidof(IHTMLFiltersCollection));
_COM_SMARTPTR_TYPEDEF(IHTMLFontElement, __uuidof(IHTMLFontElement));
_COM_SMARTPTR_TYPEDEF(IHTMLFontNamesCollection, __uuidof(IHTMLFontNamesCollection));
_COM_SMARTPTR_TYPEDEF(IHTMLFontSizesCollection, __uuidof(IHTMLFontSizesCollection));
_COM_SMARTPTR_TYPEDEF(IHTMLFormElement, __uuidof(IHTMLFormElement));
_COM_SMARTPTR_TYPEDEF(IHTMLFrameBase, __uuidof(IHTMLFrameBase));
_COM_SMARTPTR_TYPEDEF(IHTMLFrameElement, __uuidof(IHTMLFrameElement));
_COM_SMARTPTR_TYPEDEF(IHTMLFrameSetElement, __uuidof(IHTMLFrameSetElement));
_COM_SMARTPTR_TYPEDEF(IHTMLFramesCollection2, __uuidof(IHTMLFramesCollection2));
_COM_SMARTPTR_TYPEDEF(IHTMLHRElement, __uuidof(IHTMLHRElement));
_COM_SMARTPTR_TYPEDEF(IHTMLHeaderElement, __uuidof(IHTMLHeaderElement));
_COM_SMARTPTR_TYPEDEF(IHTMLIFrameElement, __uuidof(IHTMLIFrameElement));
_COM_SMARTPTR_TYPEDEF(IHTMLImageElementFactory, __uuidof(IHTMLImageElementFactory));
_COM_SMARTPTR_TYPEDEF(IHTMLImgElement, __uuidof(IHTMLImgElement));
_COM_SMARTPTR_TYPEDEF(IHTMLInputButtonElement, __uuidof(IHTMLInputButtonElement));
_COM_SMARTPTR_TYPEDEF(IHTMLInputFileElement, __uuidof(IHTMLInputFileElement));
_COM_SMARTPTR_TYPEDEF(IHTMLInputHiddenElement, __uuidof(IHTMLInputHiddenElement));
_COM_SMARTPTR_TYPEDEF(IHTMLInputImage, __uuidof(IHTMLInputImage));
_COM_SMARTPTR_TYPEDEF(IHTMLInputTextElement, __uuidof(IHTMLInputTextElement));
_COM_SMARTPTR_TYPEDEF(IHTMLIsIndexElement, __uuidof(IHTMLIsIndexElement));
_COM_SMARTPTR_TYPEDEF(IHTMLLIElement, __uuidof(IHTMLLIElement));
_COM_SMARTPTR_TYPEDEF(IHTMLLabelElement, __uuidof(IHTMLLabelElement));
_COM_SMARTPTR_TYPEDEF(IHTMLLegendElement, __uuidof(IHTMLLegendElement));
_COM_SMARTPTR_TYPEDEF(IHTMLLinkElement, __uuidof(IHTMLLinkElement));
_COM_SMARTPTR_TYPEDEF(IHTMLListElement, __uuidof(IHTMLListElement));
_COM_SMARTPTR_TYPEDEF(IHTMLLocation, __uuidof(IHTMLLocation));
_COM_SMARTPTR_TYPEDEF(IHTMLMapElement, __uuidof(IHTMLMapElement));
_COM_SMARTPTR_TYPEDEF(IHTMLMarqueeElement, __uuidof(IHTMLMarqueeElement));
_COM_SMARTPTR_TYPEDEF(IHTMLMetaElement, __uuidof(IHTMLMetaElement));
_COM_SMARTPTR_TYPEDEF(IHTMLMimeTypesCollection, __uuidof(IHTMLMimeTypesCollection));
_COM_SMARTPTR_TYPEDEF(IHTMLNextIdElement, __uuidof(IHTMLNextIdElement));
_COM_SMARTPTR_TYPEDEF(IHTMLNoShowElement, __uuidof(IHTMLNoShowElement));
_COM_SMARTPTR_TYPEDEF(IHTMLOListElement, __uuidof(IHTMLOListElement));
_COM_SMARTPTR_TYPEDEF(IHTMLObjectElement, __uuidof(IHTMLObjectElement));
_COM_SMARTPTR_TYPEDEF(IHTMLOpsProfile, __uuidof(IHTMLOpsProfile));
_COM_SMARTPTR_TYPEDEF(IHTMLOptionButtonElement, __uuidof(IHTMLOptionButtonElement));
_COM_SMARTPTR_TYPEDEF(IHTMLOptionElement, __uuidof(IHTMLOptionElement));
_COM_SMARTPTR_TYPEDEF(IHTMLOptionElementFactory, __uuidof(IHTMLOptionElementFactory));
_COM_SMARTPTR_TYPEDEF(IHTMLOptionsHolder, __uuidof(IHTMLOptionsHolder));
_COM_SMARTPTR_TYPEDEF(IHTMLParaElement, __uuidof(IHTMLParaElement));
_COM_SMARTPTR_TYPEDEF(IHTMLPhraseElement, __uuidof(IHTMLPhraseElement));
_COM_SMARTPTR_TYPEDEF(IHTMLPluginsCollection, __uuidof(IHTMLPluginsCollection));
_COM_SMARTPTR_TYPEDEF(IHTMLRuleStyle, __uuidof(IHTMLRuleStyle));
_COM_SMARTPTR_TYPEDEF(IHTMLScreen, __uuidof(IHTMLScreen));
_COM_SMARTPTR_TYPEDEF(IHTMLScriptElement, __uuidof(IHTMLScriptElement));
_COM_SMARTPTR_TYPEDEF(IHTMLSelectElement, __uuidof(IHTMLSelectElement));
_COM_SMARTPTR_TYPEDEF(IHTMLSelectionObject, __uuidof(IHTMLSelectionObject));
_COM_SMARTPTR_TYPEDEF(IHTMLSpanElement, __uuidof(IHTMLSpanElement));
_COM_SMARTPTR_TYPEDEF(IHTMLSpanFlow, __uuidof(IHTMLSpanFlow));
_COM_SMARTPTR_TYPEDEF(IHTMLStyle, __uuidof(IHTMLStyle));
_COM_SMARTPTR_TYPEDEF(IHTMLStyleElement, __uuidof(IHTMLStyleElement));
_COM_SMARTPTR_TYPEDEF(IHTMLStyleFontFace, __uuidof(IHTMLStyleFontFace));
_COM_SMARTPTR_TYPEDEF(IHTMLStyleSheet, __uuidof(IHTMLStyleSheet));
_COM_SMARTPTR_TYPEDEF(IHTMLStyleSheetRule, __uuidof(IHTMLStyleSheetRule));
_COM_SMARTPTR_TYPEDEF(IHTMLStyleSheetRulesCollection, __uuidof(IHTMLStyleSheetRulesCollection));
_COM_SMARTPTR_TYPEDEF(IHTMLStyleSheetsCollection, __uuidof(IHTMLStyleSheetsCollection));
_COM_SMARTPTR_TYPEDEF(IHTMLTable, __uuidof(IHTMLTable));
_COM_SMARTPTR_TYPEDEF(IHTMLTableCaption, __uuidof(IHTMLTableCaption));
_COM_SMARTPTR_TYPEDEF(IHTMLTableCell, __uuidof(IHTMLTableCell));
_COM_SMARTPTR_TYPEDEF(IHTMLTableCol, __uuidof(IHTMLTableCol));
_COM_SMARTPTR_TYPEDEF(IHTMLTableRow, __uuidof(IHTMLTableRow));
_COM_SMARTPTR_TYPEDEF(IHTMLTableSection, __uuidof(IHTMLTableSection));
_COM_SMARTPTR_TYPEDEF(IHTMLTextAreaElement, __uuidof(IHTMLTextAreaElement));
_COM_SMARTPTR_TYPEDEF(IHTMLTextContainer, __uuidof(IHTMLTextContainer));
_COM_SMARTPTR_TYPEDEF(IHTMLTextElement, __uuidof(IHTMLTextElement));
_COM_SMARTPTR_TYPEDEF(IHTMLTitleElement, __uuidof(IHTMLTitleElement));
_COM_SMARTPTR_TYPEDEF(IHTMLTxtRange, __uuidof(IHTMLTxtRange));
_COM_SMARTPTR_TYPEDEF(IHTMLUListElement, __uuidof(IHTMLUListElement));
_COM_SMARTPTR_TYPEDEF(IHTMLUnknownElement, __uuidof(IHTMLUnknownElement));
_COM_SMARTPTR_TYPEDEF(IHTMLWindow2, __uuidof(IHTMLWindow2));
_COM_SMARTPTR_TYPEDEF(IHlink, __uuidof(IHlink));
_COM_SMARTPTR_TYPEDEF(IHlinkBrowseContext, __uuidof(IHlinkBrowseContext));
_COM_SMARTPTR_TYPEDEF(IHlinkFrame, __uuidof(IHlinkFrame));
_COM_SMARTPTR_TYPEDEF(IHlinkSite, __uuidof(IHlinkSite));
_COM_SMARTPTR_TYPEDEF(IHlinkTarget, __uuidof(IHlinkTarget));
_COM_SMARTPTR_TYPEDEF(IHttpNegotiate, __uuidof(IHttpNegotiate));
_COM_SMARTPTR_TYPEDEF(IHttpSecurity, __uuidof(IHttpSecurity));
_COM_SMARTPTR_TYPEDEF(IImageDecodeEventSink, __uuidof(IImageDecodeEventSink));
_COM_SMARTPTR_TYPEDEF(IImageDecodeFilter, __uuidof(IImageDecodeFilter));
_COM_SMARTPTR_TYPEDEF(IImgCtx, __uuidof(IImgCtx));
_COM_SMARTPTR_TYPEDEF(IInputObject, __uuidof(IInputObject));
_COM_SMARTPTR_TYPEDEF(IInputObjectSite, __uuidof(IInputObjectSite));
_COM_SMARTPTR_TYPEDEF(IInternet, __uuidof(IInternet));
_COM_SMARTPTR_TYPEDEF(IInternetBindInfo, __uuidof(IInternetBindInfo));
_COM_SMARTPTR_TYPEDEF(IInternetHostSecurityManager, __uuidof(IInternetHostSecurityManager));
_COM_SMARTPTR_TYPEDEF(IInternetPriority, __uuidof(IInternetPriority));
_COM_SMARTPTR_TYPEDEF(IInternetProtocol, __uuidof(IInternetProtocol));
_COM_SMARTPTR_TYPEDEF(IInternetProtocolInfo, __uuidof(IInternetProtocolInfo));
_COM_SMARTPTR_TYPEDEF(IInternetProtocolRoot, __uuidof(IInternetProtocolRoot));
_COM_SMARTPTR_TYPEDEF(IInternetProtocolSink, __uuidof(IInternetProtocolSink));
_COM_SMARTPTR_TYPEDEF(IInternetSecurityManager, __uuidof(IInternetSecurityManager));
_COM_SMARTPTR_TYPEDEF(IInternetSecurityMgrSite, __uuidof(IInternetSecurityMgrSite));
_COM_SMARTPTR_TYPEDEF(IInternetSession, __uuidof(IInternetSession));
_COM_SMARTPTR_TYPEDEF(IInternetThreadSwitch, __uuidof(IInternetThreadSwitch));
_COM_SMARTPTR_TYPEDEF(IInternetZoneManager, __uuidof(IInternetZoneManager));
_COM_SMARTPTR_TYPEDEF(ILayoutStorage, __uuidof(ILayoutStorage));
_COM_SMARTPTR_TYPEDEF(ILockBytes, __uuidof(ILockBytes));
_COM_SMARTPTR_TYPEDEF(IMLangCodePages, __uuidof(IMLangCodePages));
_COM_SMARTPTR_TYPEDEF(IMLangConvertCharset, __uuidof(IMLangConvertCharset));
_COM_SMARTPTR_TYPEDEF(IMLangFontLink, __uuidof(IMLangFontLink));
_COM_SMARTPTR_TYPEDEF(IMLangLineBreakConsole, __uuidof(IMLangLineBreakConsole));
_COM_SMARTPTR_TYPEDEF(IMLangString, __uuidof(IMLangString));
_COM_SMARTPTR_TYPEDEF(IMLangStringAStr, __uuidof(IMLangStringAStr));
_COM_SMARTPTR_TYPEDEF(IMLangStringBufA, __uuidof(IMLangStringBufA));
_COM_SMARTPTR_TYPEDEF(IMLangStringBufW, __uuidof(IMLangStringBufW));
_COM_SMARTPTR_TYPEDEF(IMLangStringWStr, __uuidof(IMLangStringWStr));
_COM_SMARTPTR_TYPEDEF(IMalloc, __uuidof(IMalloc));
_COM_SMARTPTR_TYPEDEF(IMallocSpy, __uuidof(IMallocSpy));
_COM_SMARTPTR_TYPEDEF(IMapMIMEToCLSID, __uuidof(IMapMIMEToCLSID));
_COM_SMARTPTR_TYPEDEF(IMarshal, __uuidof(IMarshal));
_COM_SMARTPTR_TYPEDEF(IMessageFilter, __uuidof(IMessageFilter));
_COM_SMARTPTR_TYPEDEF(IMimeInfo, __uuidof(IMimeInfo));
_COM_SMARTPTR_TYPEDEF(IMoniker, __uuidof(IMoniker));
_COM_SMARTPTR_TYPEDEF(IMultiLanguage, __uuidof(IMultiLanguage));
_COM_SMARTPTR_TYPEDEF(IMultiQI, __uuidof(IMultiQI));
_COM_SMARTPTR_TYPEDEF(INewShortcutHookA, __uuidof(INewShortcutHookA));
_COM_SMARTPTR_TYPEDEF(INewShortcutHookW, __uuidof(INewShortcutHookW));
_COM_SMARTPTR_TYPEDEF(IObjectIdentity, __uuidof(IObjectIdentity));
_COM_SMARTPTR_TYPEDEF(IObjectSafety, __uuidof(IObjectSafety));
_COM_SMARTPTR_TYPEDEF(IObjectWithSite, __uuidof(IObjectWithSite));
_COM_SMARTPTR_TYPEDEF(IOleAdviseHolder, __uuidof(IOleAdviseHolder));
_COM_SMARTPTR_TYPEDEF(IOleCache, __uuidof(IOleCache));
_COM_SMARTPTR_TYPEDEF(IOleCache2, __uuidof(IOleCache2));
_COM_SMARTPTR_TYPEDEF(IOleCacheControl, __uuidof(IOleCacheControl));
_COM_SMARTPTR_TYPEDEF(IOleClientSite, __uuidof(IOleClientSite));
_COM_SMARTPTR_TYPEDEF(IOleCommandTarget, __uuidof(IOleCommandTarget));
_COM_SMARTPTR_TYPEDEF(IOleContainer, __uuidof(IOleContainer));
_COM_SMARTPTR_TYPEDEF(IOleControl, __uuidof(IOleControl));
_COM_SMARTPTR_TYPEDEF(IOleControlSite, __uuidof(IOleControlSite));
_COM_SMARTPTR_TYPEDEF(IOleDocument, __uuidof(IOleDocument));
_COM_SMARTPTR_TYPEDEF(IOleDocumentSite, __uuidof(IOleDocumentSite));
_COM_SMARTPTR_TYPEDEF(IOleDocumentView, __uuidof(IOleDocumentView));
_COM_SMARTPTR_TYPEDEF(IOleInPlaceActiveObject, __uuidof(IOleInPlaceActiveObject));
_COM_SMARTPTR_TYPEDEF(IOleInPlaceFrame, __uuidof(IOleInPlaceFrame));
_COM_SMARTPTR_TYPEDEF(IOleInPlaceObject, __uuidof(IOleInPlaceObject));
_COM_SMARTPTR_TYPEDEF(IOleInPlaceObjectWindowless, __uuidof(IOleInPlaceObjectWindowless));
_COM_SMARTPTR_TYPEDEF(IOleInPlaceSite, __uuidof(IOleInPlaceSite));
_COM_SMARTPTR_TYPEDEF(IOleInPlaceSiteEx, __uuidof(IOleInPlaceSiteEx));
_COM_SMARTPTR_TYPEDEF(IOleInPlaceSiteWindowless, __uuidof(IOleInPlaceSiteWindowless));
_COM_SMARTPTR_TYPEDEF(IOleInPlaceUIWindow, __uuidof(IOleInPlaceUIWindow));
_COM_SMARTPTR_TYPEDEF(IOleItemContainer, __uuidof(IOleItemContainer));
_COM_SMARTPTR_TYPEDEF(IOleLink, __uuidof(IOleLink));
_COM_SMARTPTR_TYPEDEF(IOleObject, __uuidof(IOleObject));
_COM_SMARTPTR_TYPEDEF(IOleParentUndoUnit, __uuidof(IOleParentUndoUnit));
_COM_SMARTPTR_TYPEDEF(IOleUndoManager, __uuidof(IOleUndoManager));
_COM_SMARTPTR_TYPEDEF(IOleUndoUnit, __uuidof(IOleUndoUnit));
_COM_SMARTPTR_TYPEDEF(IOleWindow, __uuidof(IOleWindow));
_COM_SMARTPTR_TYPEDEF(IOmHistory, __uuidof(IOmHistory));
_COM_SMARTPTR_TYPEDEF(IOmNavigator, __uuidof(IOmNavigator));
_COM_SMARTPTR_TYPEDEF(IPSFactoryBuffer, __uuidof(IPSFactoryBuffer));
_COM_SMARTPTR_TYPEDEF(IParseDisplayName, __uuidof(IParseDisplayName));
_COM_SMARTPTR_TYPEDEF(IPerPropertyBrowsing, __uuidof(IPerPropertyBrowsing));
_COM_SMARTPTR_TYPEDEF(IPersist, __uuidof(IPersist));
_COM_SMARTPTR_TYPEDEF(IPersistFile, __uuidof(IPersistFile));
_COM_SMARTPTR_TYPEDEF(IPersistFolder, __uuidof(IPersistFolder));
_COM_SMARTPTR_TYPEDEF(IPersistFolder2, __uuidof(IPersistFolder2));
_COM_SMARTPTR_TYPEDEF(IPersistHistory, __uuidof(IPersistHistory));
_COM_SMARTPTR_TYPEDEF(IPersistMemory, __uuidof(IPersistMemory));
_COM_SMARTPTR_TYPEDEF(IPersistMoniker, __uuidof(IPersistMoniker));
_COM_SMARTPTR_TYPEDEF(IPersistPropertyBag, __uuidof(IPersistPropertyBag));
_COM_SMARTPTR_TYPEDEF(IPersistPropertyBag2, __uuidof(IPersistPropertyBag2));
_COM_SMARTPTR_TYPEDEF(IPersistStorage, __uuidof(IPersistStorage));
_COM_SMARTPTR_TYPEDEF(IPersistStream, __uuidof(IPersistStream));
_COM_SMARTPTR_TYPEDEF(IPersistStreamInit, __uuidof(IPersistStreamInit));
_COM_SMARTPTR_TYPEDEF(IPicture, __uuidof(IPicture));
_COM_SMARTPTR_TYPEDEF(IPictureDisp, __uuidof(IPictureDisp));
_COM_SMARTPTR_TYPEDEF(IPointerInactive, __uuidof(IPointerInactive));
_COM_SMARTPTR_TYPEDEF(IPrint, __uuidof(IPrint));
_COM_SMARTPTR_TYPEDEF(IProgressNotify, __uuidof(IProgressNotify));
_COM_SMARTPTR_TYPEDEF(IPropertyBag, __uuidof(IPropertyBag));
_COM_SMARTPTR_TYPEDEF(IPropertyBag2, __uuidof(IPropertyBag2));
_COM_SMARTPTR_TYPEDEF(IPropertyNotifySink, __uuidof(IPropertyNotifySink));
_COM_SMARTPTR_TYPEDEF(IPropertyPage, __uuidof(IPropertyPage));
_COM_SMARTPTR_TYPEDEF(IPropertyPage2, __uuidof(IPropertyPage2));
_COM_SMARTPTR_TYPEDEF(IPropertyPageSite, __uuidof(IPropertyPageSite));
_COM_SMARTPTR_TYPEDEF(IPropertySetStorage, __uuidof(IPropertySetStorage));
_COM_SMARTPTR_TYPEDEF(IPropertyStorage, __uuidof(IPropertyStorage));
_COM_SMARTPTR_TYPEDEF(IProvideClassInfo, __uuidof(IProvideClassInfo));
_COM_SMARTPTR_TYPEDEF(IProvideClassInfo2, __uuidof(IProvideClassInfo2));
_COM_SMARTPTR_TYPEDEF(IProvideMultipleClassInfo, __uuidof(IProvideMultipleClassInfo));
_COM_SMARTPTR_TYPEDEF(IQueryInfo, __uuidof(IQueryInfo));
_COM_SMARTPTR_TYPEDEF(IQuickActivate, __uuidof(IQuickActivate));
_COM_SMARTPTR_TYPEDEF(IROTData, __uuidof(IROTData));
_COM_SMARTPTR_TYPEDEF(IRecordInfo, __uuidof(IRecordInfo));
_COM_SMARTPTR_TYPEDEF(IRichEditOle, __uuidof(IRichEditOle));
_COM_SMARTPTR_TYPEDEF(IRichEditOleCallback, __uuidof(IRichEditOleCallback));
_COM_SMARTPTR_TYPEDEF(IRootStorage, __uuidof(IRootStorage));
_COM_SMARTPTR_TYPEDEF(IRpcChannelBuffer, __uuidof(IRpcChannelBuffer));
_COM_SMARTPTR_TYPEDEF(IRpcChannelBuffer2, __uuidof(IRpcChannelBuffer2));
_COM_SMARTPTR_TYPEDEF(IRpcChannelBuffer3, __uuidof(IRpcChannelBuffer3));
_COM_SMARTPTR_TYPEDEF(IRpcProxyBuffer, __uuidof(IRpcProxyBuffer));
_COM_SMARTPTR_TYPEDEF(IRpcStubBuffer, __uuidof(IRpcStubBuffer));
_COM_SMARTPTR_TYPEDEF(IRunnableObject, __uuidof(IRunnableObject));
_COM_SMARTPTR_TYPEDEF(IRunningObjectTable, __uuidof(IRunningObjectTable));
_COM_SMARTPTR_TYPEDEF(ISequentialStream, __uuidof(ISequentialStream));
_COM_SMARTPTR_TYPEDEF(IServerSecurity, __uuidof(IServerSecurity));
_COM_SMARTPTR_TYPEDEF(IServiceProvider, __uuidof(IServiceProvider));
_COM_SMARTPTR_TYPEDEF(IShellBrowser, __uuidof(IShellBrowser));
_COM_SMARTPTR_TYPEDEF(IShellDispatch, __uuidof(IShellDispatch));
_COM_SMARTPTR_TYPEDEF(IShellExecuteHookA, __uuidof(IShellExecuteHookA));
_COM_SMARTPTR_TYPEDEF(IShellExecuteHookW, __uuidof(IShellExecuteHookW));
_COM_SMARTPTR_TYPEDEF(IShellExtInit, __uuidof(IShellExtInit));
_COM_SMARTPTR_TYPEDEF(IShellFolder, __uuidof(IShellFolder));
_COM_SMARTPTR_TYPEDEF(IShellFolderViewDual, __uuidof(IShellFolderViewDual));
_COM_SMARTPTR_TYPEDEF(IShellIcon, __uuidof(IShellIcon));
_COM_SMARTPTR_TYPEDEF(IShellIconOverlay, __uuidof(IShellIconOverlay));
_COM_SMARTPTR_TYPEDEF(IShellIconOverlayIdentifier, __uuidof(IShellIconOverlayIdentifier));
_COM_SMARTPTR_TYPEDEF(IShellLinkA, __uuidof(IShellLinkA));
_COM_SMARTPTR_TYPEDEF(IShellLinkDual, __uuidof(IShellLinkDual));
_COM_SMARTPTR_TYPEDEF(IShellLinkW, __uuidof(IShellLinkW));
_COM_SMARTPTR_TYPEDEF(IShellPropSheetExt, __uuidof(IShellPropSheetExt));
_COM_SMARTPTR_TYPEDEF(IShellUIHelper, __uuidof(IShellUIHelper));
_COM_SMARTPTR_TYPEDEF(IShellView, __uuidof(IShellView));
_COM_SMARTPTR_TYPEDEF(IShellView2, __uuidof(IShellView2));
_COM_SMARTPTR_TYPEDEF(IShellWindows, __uuidof(IShellWindows));
_COM_SMARTPTR_TYPEDEF(ISimpleFrameSite, __uuidof(ISimpleFrameSite));
_COM_SMARTPTR_TYPEDEF(ISoftDistExt, __uuidof(ISoftDistExt));
_COM_SMARTPTR_TYPEDEF(ISpecifyPropertyPages, __uuidof(ISpecifyPropertyPages));
_COM_SMARTPTR_TYPEDEF(IStdMarshalInfo, __uuidof(IStdMarshalInfo));
_COM_SMARTPTR_TYPEDEF(IStorage, __uuidof(IStorage));
_COM_SMARTPTR_TYPEDEF(IStream, __uuidof(IStream));
_COM_SMARTPTR_TYPEDEF(ISubscriptionMgr, __uuidof(ISubscriptionMgr));
_COM_SMARTPTR_TYPEDEF(ISupportErrorInfo, __uuidof(ISupportErrorInfo));
_COM_SMARTPTR_TYPEDEF(ISurrogate, __uuidof(ISurrogate));
_COM_SMARTPTR_TYPEDEF(ISynchronize, __uuidof(ISynchronize));
_COM_SMARTPTR_TYPEDEF(ISynchronizeEvent, __uuidof(ISynchronizeEvent));
_COM_SMARTPTR_TYPEDEF(ISynchronizeMutex, __uuidof(ISynchronizeMutex));
_COM_SMARTPTR_TYPEDEF(ITimer, __uuidof(ITimer));
_COM_SMARTPTR_TYPEDEF(ITimerService, __uuidof(ITimerService));
_COM_SMARTPTR_TYPEDEF(ITimerSink, __uuidof(ITimerSink));
_COM_SMARTPTR_TYPEDEF(ITypeChangeEvents, __uuidof(ITypeChangeEvents));
_COM_SMARTPTR_TYPEDEF(ITypeComp, __uuidof(ITypeComp));
_COM_SMARTPTR_TYPEDEF(ITypeFactory, __uuidof(ITypeFactory));
_COM_SMARTPTR_TYPEDEF(ITypeInfo, __uuidof(ITypeInfo));
_COM_SMARTPTR_TYPEDEF(ITypeInfo2, __uuidof(ITypeInfo2));
_COM_SMARTPTR_TYPEDEF(ITypeLib, __uuidof(ITypeLib));
_COM_SMARTPTR_TYPEDEF(ITypeLib2, __uuidof(ITypeLib2));
_COM_SMARTPTR_TYPEDEF(ITypeMarshal, __uuidof(ITypeMarshal));
_COM_SMARTPTR_TYPEDEF(IURLSearchHook, __uuidof(IURLSearchHook));
_COM_SMARTPTR_TYPEDEF(IUniformResourceLocatorA, __uuidof(IUniformResourceLocatorA));
_COM_SMARTPTR_TYPEDEF(IUniformResourceLocatorW, __uuidof(IUniformResourceLocatorW));
_COM_SMARTPTR_TYPEDEF(IUnknown, __uuidof(IUnknown));
_COM_SMARTPTR_TYPEDEF(IUrlHistoryNotify, __uuidof(IUrlHistoryNotify));
_COM_SMARTPTR_TYPEDEF(IUrlHistoryStg, __uuidof(IUrlHistoryStg));
_COM_SMARTPTR_TYPEDEF(IUrlHistoryStg2, __uuidof(IUrlHistoryStg2));
_COM_SMARTPTR_TYPEDEF(IUrlMon, __uuidof(IUrlMon));
_COM_SMARTPTR_TYPEDEF(IVariantChangeType, __uuidof(IVariantChangeType));
_COM_SMARTPTR_TYPEDEF(IViewFilterSite, __uuidof(IViewFilterSite));
_COM_SMARTPTR_TYPEDEF(IViewObject, __uuidof(IViewObject));
_COM_SMARTPTR_TYPEDEF(IViewObject2, __uuidof(IViewObject2));
_COM_SMARTPTR_TYPEDEF(IViewObjectEx, __uuidof(IViewObjectEx));
_COM_SMARTPTR_TYPEDEF(IViewTransition, __uuidof(IViewTransition));
_COM_SMARTPTR_TYPEDEF(IViewTransitionSite, __uuidof(IViewTransitionSite));
_COM_SMARTPTR_TYPEDEF(IWaitMultiple, __uuidof(IWaitMultiple));
_COM_SMARTPTR_TYPEDEF(IWebBrowser, __uuidof(IWebBrowser));
_COM_SMARTPTR_TYPEDEF(IWebBrowser2, __uuidof(IWebBrowser2));
_COM_SMARTPTR_TYPEDEF(IWebBrowserApp, __uuidof(IWebBrowserApp));
_COM_SMARTPTR_TYPEDEF(IWinInetHttpInfo, __uuidof(IWinInetHttpInfo));
_COM_SMARTPTR_TYPEDEF(IWinInetInfo, __uuidof(IWinInetInfo));
_COM_SMARTPTR_TYPEDEF(IWindowForBindingUI, __uuidof(IWindowForBindingUI));
_COM_SMARTPTR_TYPEDEF(IXMLDocument, __uuidof(IXMLDocument));
_COM_SMARTPTR_TYPEDEF(IXMLElement, __uuidof(IXMLElement));
_COM_SMARTPTR_TYPEDEF(IXMLElementCollection, __uuidof(IXMLElementCollection));
_COM_SMARTPTR_TYPEDEF(IXMLError, __uuidof(IXMLError));
_COM_SMARTPTR_TYPEDEF(OLEDBSimpleProvider, __uuidof(OLEDBSimpleProvider));
_COM_SMARTPTR_TYPEDEF(OLEDBSimpleProviderListener, __uuidof(OLEDBSimpleProviderListener));

#endif  /* _COM_NO_STANDARD_GUIDS_ */

#if _MSC_VER >= 1200
#pragma warning(pop)
#endif

#endif  /* _INC_COMDEF */


