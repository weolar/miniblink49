#include "stdafx.h"
#include "W3Client.h"
#include <cstring>

using namespace std;

#define __HTTP_VERB_GET "GET"
#define __HTTP_VERB_POST "POST"
#define __HTTP_ACCEPT_TYPE "*/*"
#define __HTTP_ACCEPT "Accept: */*\r\n"

#define __DEFAULT_BUF_SIZE 1024

void __w3cexcept(const char *szaddress, long nport, W3Client::w3t t, const char *szmsg){
#ifdef _DEBUG
    string sztmp;
    sztmp += "[ ";
    switch (t) {
    case W3Client::w3http:
        sztmp += "http://";
        break;
    case W3Client::w3https:
        sztmp += "https://";
        break;
    case W3Client::w3ftp:
        sztmp += "ftp://";
        break;
    }
    sztmp += szaddress;
    sztmp += ":";
    char szp[10] = "\0";
    sprintf(szp, "%d", nport);
    sztmp += szp;
    sztmp += " ] ";
    sztmp += szmsg;

    ::OutputDebugStringA(sztmp.c_str());

    DWORD err = ::GetLastError();
    LPVOID     lpMsgBuffer;
    DWORD dwRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
        GetModuleHandleA("wininet.dll"),
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPTSTR>(&lpMsgBuffer),
        0,
        NULL);

    char szmsg2[1024] = "\0";
    sprintf(szmsg2, "[%d, 0x%08x] %s", err, err, reinterpret_cast<LPCSTR>(lpMsgBuffer));
    OutputDebugStringA(szmsg2);
    ::LocalFree(lpMsgBuffer);
#endif      
}

void __w3cexcept(const char *szaddress, long nport, W3Client::w3t t, const char *szuri, const char *szmsg){
#ifdef _DEBUG
    string sztmp;
    sztmp += "[ ";
    switch (t) {
    case W3Client::w3http:
        sztmp += "http://";
        break;
    case W3Client::w3https:
        sztmp += "https://";
        break;
    case W3Client::w3ftp:
        sztmp += "ftp://";
        break;
    }
    sztmp += szaddress;
    sztmp += ":";
    char szp[10] = "\0";
    sprintf(szp, "%d", nport);
    sztmp += szp;
    sztmp += szuri;
    sztmp += " ] ";
    sztmp += szmsg;

    ::OutputDebugStringA(sztmp.c_str());

    LPVOID lpMsgBuffer;
    DWORD err = ::GetLastError();
    DWORD dwRet = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
        GetModuleHandleA("wininet.dll"),
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&lpMsgBuffer),
        0,
        NULL);
    char szmsg2[1024] = "\0";
    sprintf(szmsg2, "[%d, 0x%08x] %s", err, err, reinterpret_cast<LPCSTR>(lpMsgBuffer));
    OutputDebugStringA(szmsg2);
    ::LocalFree(lpMsgBuffer);
#endif  
}

// W3Client : synchronized www client
bool W3Client::Connect(const char *szaddress,
    const char *szuser /*=NULL*/,
    const char *szpassword /*=NULL*/,
    const char *szagent /*=__W3_DEFAULT_AGENT*/){

    char szp[__DEFAULT_BUF_SIZE] = "\0", szus[__DEFAULT_BUF_SIZE] = "\0", szpw[__DEFAULT_BUF_SIZE] = "\0";
    char sza[__DEFAULT_BUF_SIZE] = "\0", szuri[__DEFAULT_BUF_SIZE] = "\0";
    unsigned long port = 0;

    __w3curlparse(szaddress, szp, szus, szpw, sza, port, szuri);

    w3t wt;

    if (!strncmp(szp, "https", 5))
        wt = w3https;
    else if (!strncmp(szp, "http", 4))
        wt = w3http;
    else if (!strncmp(szp, "ftp", 3))
        wt = w3ftp;

    _szuri = szuri;

    return Connect(sza, port,
        ((szuser || (!szuser && strlen(szus) == 0)) ? szuser : szus),
        ((szpassword || (!szpassword && strlen(szpw) == 0)) ? szpassword : szpw),
        wt, szagent);
}

bool W3Client::Connect(const char *szaddress,
    long nport,
    const char *szuser /*=NULL*/,
    const char *szpassword /*=NULL*/,
    w3t t /*=w3http*/,
    const char *szagent /*=__W3_DEFAULT_AGENT*/){

    bool r = true;

    try{
        _hOpen = ::InternetOpenA(szagent, // agent
            INTERNET_OPEN_TYPE_PRECONFIG, // access type
            NULL, // proxy
            NULL, // proxy by pass
            0); // flags
        if (!_hOpen || _hOpen == INVALID_HANDLE_VALUE)
            throw "open internet failed...";

        switch (t) {
        case w3ftp:
            _hConnection = ::InternetConnectA(_hOpen,
                szaddress, (nport ? nport : INTERNET_DEFAULT_FTP_PORT),
                szuser, szpassword,
                INTERNET_SERVICE_FTP,
                INTERNET_FLAG_PASSIVE,
                NULL);
            break;
        case w3http:
            _hConnection = ::InternetConnectA(_hOpen,
                szaddress, (nport ? nport : INTERNET_DEFAULT_HTTP_PORT),
                szuser, szpassword,
                INTERNET_SERVICE_HTTP,
                0,
                NULL);
            break;
        case w3https:
            _hConnection = ::InternetConnectA(_hOpen,
                szaddress, (nport ? nport : INTERNET_DEFAULT_HTTPS_PORT),
                szuser, szpassword,
                INTERNET_SERVICE_HTTP,
                0,
                NULL);
            break;
        }

        if (!_hConnection || _hConnection == INVALID_HANDLE_VALUE)
            throw "connect failed...";

        if (::InternetAttemptConnect(NULL) != ERROR_SUCCESS)
            throw "connect failed...";

        _szaddress = szaddress;
        if (!nport){
            switch (t) {
            case w3ftp:
                _nport = INTERNET_DEFAULT_FTP_PORT;
                break;
            case w3http:
                _nport = INTERNET_DEFAULT_HTTP_PORT;
                break;
            case w3https:
                _nport = INTERNET_DEFAULT_HTTPS_PORT;
                break;
            }
        }
        else
            _nport = nport;
        _t = t;

        if (szuser)
            _szuser = szuser;

        if (szpassword)
            _szpassword = szpassword;

        InitializeCookies();
        InitializePostArguments();

    }
    catch (const char *szm){
        r = false;
        if (_hOpen || _hOpen != INVALID_HANDLE_VALUE)
            ::InternetCloseHandle(_hOpen);
        if (_hConnection || _hConnection != INVALID_HANDLE_VALUE)
            ::CloseHandle(_hConnection);
        __w3cexcept(szaddress, nport, t, szm);
    }
    catch (...){
        r = false;
        if (_hOpen || _hOpen != INVALID_HANDLE_VALUE)
            ::InternetCloseHandle(_hOpen);
        if (_hConnection || _hConnection != INVALID_HANDLE_VALUE)
            ::CloseHandle(_hConnection);
        __w3cexcept(szaddress, nport, t, "unknown exception...");
    }

    return r;
}

void W3Client::Close(){
    if (_hRequest)
        ::InternetCloseHandle(_hRequest);

    if (_hConnection)
        ::InternetCloseHandle(_hConnection);

    if (_hOpen)
        ::InternetCloseHandle(_hOpen);

    return;
}

bool W3Client::Request(const char *szuri, w3m m, const char *szref /*=NULL*/){

    bool bflag = false;
    try{
        switch (m) {
        case reqPost:
            bflag = RequestPost(szuri, szref);
            break;
        case reqPostMultipartsFormdata:
            bflag = RequestPost2(szuri, szref);
            break;
        case reqGet:
            bflag = RequestGet(szuri, szref);
            break;
        }

        if (bflag){
            _szuri = szuri;
        }
        else{
            ::InternetCloseHandle(_hRequest);
            _hRequest = NULL;
        }

    }
    catch (const char *szm){
        ::InternetCloseHandle(_hRequest);
        _hRequest = NULL;
        __w3cexcept(_szaddress.c_str(), _nport, _t, szuri, szm);
    }
    catch (...){
        ::InternetCloseHandle(_hRequest);
        _hRequest = NULL;
        __w3cexcept(_szaddress.c_str(), _nport, _t, szuri, "unknown exception...");
    }

    return bflag;
}

bool W3Client::RequestGet(const char *szuri, const char *szref /*=NULL*/){

    static LPCSTR szAcceptType = (__HTTP_ACCEPT_TYPE);

    if (!_hConnection || _hConnection == INVALID_HANDLE_VALUE)
        throw "handle not opened...";

    _hRequest = ::HttpOpenRequestA(_hConnection,
        __HTTP_VERB_GET, // HTTP Verb
        szuri, // Object Name
        HTTP_VERSIONA, // Version
        szref, // Reference
        &szAcceptType, // Accept Type
        INTERNET_FLAG_NO_CACHE_WRITE | (_t == w3https ? INTERNET_FLAG_SECURE : 0),
        NULL); // context call-back point

    if (!_hRequest || _hRequest == INVALID_HANDLE_VALUE)
        throw "request failed...";

    // REPLACE HEADER
    if (!::HttpAddRequestHeadersA(_hRequest,
        __HTTP_ACCEPT, strlen(__HTTP_ACCEPT),
        HTTP_ADDREQ_FLAG_REPLACE))
        throw "additional header failed...";

    // COOKIE ADD
    if (_listcookies.size() > 0){
        string szurl;
        switch (_t) {
        case w3http:
            szurl = "http://";
            break;
        case w3https:
            szurl = "https://";
            break;
        }
        szurl += _szaddress.c_str();
        if (!((_t == w3http && _nport == INTERNET_DEFAULT_HTTP_PORT) || (_t == w3https && _nport == INTERNET_DEFAULT_HTTPS_PORT))){
            char tmp[10] = "\0";
            sprintf_s(tmp, ":%d", _nport);
            szurl += tmp;
        }
        szurl += szuri;

        for (list<HTTP_COOKIE*>::iterator it = _listcookies.begin(); it != _listcookies.end(); it++){
            HTTP_COOKIE *pc = reinterpret_cast<HTTP_COOKIE*>(*it);
            if (!::InternetSetCookieA(szurl.c_str(), pc->name.c_str(), pc->value.c_str()))
                throw "add cookie failed...";
        }
    }

    // SEND REQUEST
    if (!::HttpSendRequestA(_hRequest,  // handle by returned HttpOpenRequest
        NULL, // additional HTTP header
        0, // additional HTTP header length
        NULL, // additional data in HTTP Post or HTTP Put
        0) // additional data length
        )
        throw "request failed...";

    return true;
}

void W3Client::InitializePostArguments(){

    if (_listargs.size() > 0){
        for (list<HTTP_ARG*>::iterator it = _listargs.begin(); it != _listargs.end(); it++){
            HTTP_ARG *p = reinterpret_cast<HTTP_ARG*>(*it);
            delete p;
        }
        _listargs.clear();
    }
    return;
}

void W3Client::AddPostArgument(const char *szname, const int nvalue){
    HTTP_ARG *pa = new HTTP_ARG(szname, nvalue);
    _listargs.push_back(pa);
    return;
}

void W3Client::AddPostArgument(const char *szname, const long nvalue){
    HTTP_ARG *pa = new HTTP_ARG(szname, nvalue);
    _listargs.push_back(pa);
    return;
}

void W3Client::AddPostArgument(const char *szname, const float nvalue){
    HTTP_ARG *pa = new HTTP_ARG(szname, nvalue);
    _listargs.push_back(pa);
    return;
}

void W3Client::AddPostArgument(const char *szname, const double nvalue){
    HTTP_ARG *pa = new HTTP_ARG(szname, nvalue);
    _listargs.push_back(pa);
    return;
}

void W3Client::AddPostArgument(const char *szname, const char *szvalue, bool bfile /*=false*/){
    HTTP_ARG *pa = new HTTP_ARG(szname, szvalue, bfile);
    _listargs.push_back(pa);
    return;
}

void W3Client::InitializeCookies(){
    if (_listcookies.size() > 0){
        for (list<HTTP_COOKIE*>::iterator it = _listcookies.begin(); it != _listcookies.end(); it++){
            HTTP_COOKIE *p = reinterpret_cast<HTTP_COOKIE*>(*it);
            delete p;
        }
        _listcookies.clear();
    }
    return;
}

void W3Client::AddCookie(const char *szname, const char *szvalue){
    HTTP_COOKIE *pc = new HTTP_COOKIE(szname, szvalue);
    _listcookies.push_back(pc);

    return;
}

void W3Client::AddCookie(const char *szname, const int value){
    HTTP_COOKIE *pc = new HTTP_COOKIE(szname, value);
    _listcookies.push_back(pc);
    return;
}

void W3Client::AddCookie(const char *szname, const long value){
    HTTP_COOKIE *pc = new HTTP_COOKIE(szname, value);
    _listcookies.push_back(pc);
    return;
}

void W3Client::AddCookie(const char *szname, const float value){
    HTTP_COOKIE *pc = new HTTP_COOKIE(szname, value);
    _listcookies.push_back(pc);
    return;
}

void W3Client::AddCookie(const char *szname, const double value){
    HTTP_COOKIE *pc = new HTTP_COOKIE(szname, value);
    _listcookies.push_back(pc);
    return;
}


unsigned long W3Client::GetPostArgumentsLength(){
    unsigned long len = 0;
    if (_listargs.size() > 0){
        for (list<HTTP_ARG*>::iterator it = _listargs.begin(); it != _listargs.end(); it++){
            HTTP_ARG *p = reinterpret_cast<HTTP_ARG*>(*it);
            len += p->length() + 1;
        }
    }
    return len ? len - 1 : 0;
}

unsigned long W3Client::GetPostData(unsigned char *buf, unsigned long len){

    unsigned long l = 0;
    if (len > GetPostArgumentsLength() && _listargs.size() > 0){
        for (list<HTTP_ARG*>::iterator it = _listargs.begin(); it != _listargs.end(); it++){
            HTTP_ARG *p = reinterpret_cast<HTTP_ARG*>(*it);
            p->dump(buf + l, p->length());
            buf[l + p->length()] = '&';
            l += p->length() + 1;
        }
        buf[l - 1] = '\0';
    }
    return l ? l - 1 : 0;
}

bool W3Client::RequestPost(const char *szuri, const char *szref /*=NULL*/){

    static LPCSTR szAcceptType = __HTTP_ACCEPT_TYPE;
    static LPCSTR szContentType = "Content-Type: application/x-www-form-urlencoded\r\n";

    unsigned char *buf = NULL;
    unsigned long len = 0;

    if (!_hConnection || _hConnection == INVALID_HANDLE_VALUE)
        throw "handle not opened...";


    _hRequest = ::HttpOpenRequestA(_hConnection,
        __HTTP_VERB_POST, // HTTP Verb
        szuri, // Object Name
        HTTP_VERSIONA, // Version
        szref, // Reference
        &szAcceptType, // Accept Type                                                           
        INTERNET_FLAG_KEEP_CONNECTION |
        INTERNET_FLAG_NO_CACHE_WRITE |
        INTERNET_FLAG_FORMS_SUBMIT |
        (_t == w3https ? INTERNET_FLAG_SECURE : 0),
        NULL); // context call-back point

    if (!_hRequest || _hRequest == INVALID_HANDLE_VALUE)
        throw "request failed...";

    // REPLACE HEADER
    if (!::HttpAddRequestHeadersA(_hRequest, __HTTP_ACCEPT, strlen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
        throw "additional header failed...";

    // COOKIE ADD
    if (_listcookies.size() > 0){
        string szurl;
        switch (_t) {
        case w3http:
            szurl = "http://";
            break;
        case w3https:
            szurl = "https://";
            break;
        }
        szurl += _szaddress.c_str();
        if (!((_t == w3http && _nport == INTERNET_DEFAULT_HTTP_PORT) || (_t == w3https && _nport == INTERNET_DEFAULT_HTTPS_PORT))){
            char tmp[10] = "\0";
            sprintf_s(tmp, ":%d", _nport);
            szurl += tmp;
        }
        szurl += szuri;

        for (list<HTTP_COOKIE*>::iterator it = _listcookies.begin(); it != _listcookies.end(); it++){
            HTTP_COOKIE *pc = reinterpret_cast<HTTP_COOKIE*>(*it);
            if (!::InternetSetCookieA(szurl.c_str(), pc->name.c_str(), pc->value.c_str()))
                throw "add cookie failed...";
        }
    }

    // GET POST ARGUMENTS
    buf = reinterpret_cast<unsigned char*>(::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, GetPostArgumentsLength() + 1));
    len = GetPostData(buf, GetPostArgumentsLength() + 1);

    // SEND REQUEST WITH POST ARGUEMENTS
    if (!::HttpSendRequestA(_hRequest,  // handle by returned HttpOpenRequest
        szContentType, // additional HTTP header
        strlen(szContentType), // additional HTTP header length
        reinterpret_cast<LPVOID>(buf), // additional data in HTTP Post or HTTP Put
        len) // additional data length
        && ::GetLastError() != 12168){
        ::HeapFree(::GetProcessHeap(), 0, buf);
        throw "request failed...";
    }

    ::HeapFree(::GetProcessHeap(), 0, buf);

    return true;
}

unsigned long W3Client::GetMultiPartsFormDataLength(){
    unsigned long len = 0;
    if (_listargs.size() > 0){
        for (list<HTTP_ARG*>::iterator it = _listargs.begin(); it != _listargs.end(); it++){
            HTTP_ARG *p = reinterpret_cast<HTTP_ARG*>(*it);
            len += p->length2();
        }
    }
    return len;
}

unsigned long W3Client::AllocMultiPartsFormData(unsigned char *&buf, const char *szboundry){

    unsigned long len = 0;
    unsigned long ns = GetMultiPartsFormDataLength() + 1;

    if (buf)
        return 0;

    buf = reinterpret_cast<unsigned char*>(::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, ns));

    if (_listargs.size() > 0){
        for (list<HTTP_ARG*>::iterator it = _listargs.begin(); it != _listargs.end(); it++){
            HTTP_ARG *pa = reinterpret_cast<HTTP_ARG*>(*it);
            len += pa->dump2(buf + len, ns - len, szboundry);
        }
    }

    memcpy(buf + len, "--", 2);
    memcpy(buf + len + 2, szboundry, strlen(szboundry));
    memcpy(buf + len + 2 + strlen(szboundry), "--\r\n", 4);

    len += 2 + strlen(szboundry) + 4;

    return len;
}

void W3Client::FreeMultiPartsFormData(unsigned char *buf){ ::HeapFree(::GetProcessHeap(), 0, buf); return; }

bool W3Client::RequestPost2(const char *szuri, const char *szref /*=NULL*/){
    static LPCSTR szAcceptType = __HTTP_ACCEPT_TYPE;
    static LPCSTR szContentType = "Content-Type: multipart/form-data; boundary=--MULTI-PARTS-FORM-DATA-BOUNDARY\r\n";

    unsigned char *buf = NULL;
    unsigned long len = 0;

    if (!_hConnection || _hConnection == INVALID_HANDLE_VALUE)
        throw "handle not opened...";


    _hRequest = ::HttpOpenRequestA(_hConnection,
        __HTTP_VERB_POST, // HTTP Verb
        szuri, // Object Name
        HTTP_VERSIONA, // Version
        szref, // Reference
        &szAcceptType, // Accept Type                                                           
        INTERNET_FLAG_KEEP_CONNECTION |
        INTERNET_FLAG_NO_CACHE_WRITE |
        INTERNET_FLAG_FORMS_SUBMIT |
        (_t == w3https ? INTERNET_FLAG_SECURE : 0),
        NULL); // context call-back point

    if (!_hRequest || _hRequest == INVALID_HANDLE_VALUE)
        throw "request failed...";

    // REPLACE HEADER
    if (!::HttpAddRequestHeadersA(_hRequest, __HTTP_ACCEPT, strlen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
        throw "additional header failed...";

    if (!::HttpAddRequestHeadersA(_hRequest, szContentType, strlen(szContentType), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
        throw "additional header failed...";

    // COOKIE ADD
    if (_listcookies.size() > 0){
        string szurl;
        switch (_t) {
        case w3http:
            szurl = "http://";
            break;
        case w3https:
            szurl = "https://";
            break;
        }
        szurl += _szaddress.c_str();
        if (!(
            (_t == w3http && _nport == INTERNET_DEFAULT_HTTP_PORT) ||
            (_t == w3https && _nport == INTERNET_DEFAULT_HTTPS_PORT)
            )){
            char tmp[10] = "\0";
            sprintf_s(tmp, ":%d", _nport);
            szurl += tmp;
        }
        szurl += szuri;

        for (list<HTTP_COOKIE*>::iterator it = _listcookies.begin(); it != _listcookies.end(); it++){
            HTTP_COOKIE *pc = reinterpret_cast<HTTP_COOKIE*>(*it);
            if (!::InternetSetCookieA(szurl.c_str(), pc->name.c_str(), pc->value.c_str()))
                throw "add cookie failed...";
        }
    }

    // build multi-parts/form-data
    len = AllocMultiPartsFormData(buf, "--MULTI-PARTS-FORM-DATA-BOUNDARY");

    // ADD HEADER CONTENT LENGTH
    char szcl[__DEFAULT_BUF_SIZE] = "\0";
    sprintf_s(szcl, "Content-Length: %d\r\n", len);

    if (!::HttpAddRequestHeadersA(_hRequest, szcl, strlen(szcl), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
        throw "additional header failed...";

    // SEND REQUEST WITH HttpSendRequestEx and InternetWriteFile
    static INTERNET_BUFFERSA InternetBufferIn = { 0 };
    InternetBufferIn.dwStructSize = sizeof(INTERNET_BUFFERS);
    InternetBufferIn.Next = NULL;

    if (!::HttpSendRequestExA(_hRequest, &InternetBufferIn, NULL, HSR_INITIATE, 0)){
        // free
        FreeMultiPartsFormData(buf);
        throw "request failed";
    }

    unsigned long nout = 0;
    DWORD dwOutPostBufferLength = 0;
    if (!::InternetWriteFile(_hRequest, buf, len, &nout)){
        // free
        FreeMultiPartsFormData(buf);
        throw "request failed";
    }

    if (!::HttpEndRequestA(_hRequest, NULL, HSR_INITIATE, 0)){
        // free
        FreeMultiPartsFormData(buf);
        throw "request failed";
    }

    // free multi-parts/form-data
    FreeMultiPartsFormData(buf);

    return true;
}

unsigned long W3Client::QueryCookie(unsigned char *buf, unsigned long len, unsigned long idx /*=0*/){
    if (!::HttpQueryInfoA(_hRequest, HTTP_QUERY_SET_COOKIE, buf, &len, &idx)){
        __w3cexcept(_szaddress.c_str(), _nport, _t, _szuri.c_str(), "query cookie failed...");
        return 0;
    }
    return len;
}

unsigned long W3Client::QueryContentLength(){
    char szt[__DEFAULT_BUF_SIZE] = "\0";
    unsigned long nread = __DEFAULT_BUF_SIZE;
    memset(szt, 0x00, __DEFAULT_BUF_SIZE);
    if (!::HttpQueryInfoA(_hRequest, HTTP_QUERY_CONTENT_LENGTH, szt, reinterpret_cast<unsigned long*>(&nread), NULL)){
        __w3cexcept(_szaddress.c_str(), _nport, _t, _szuri.c_str(), "query content-length failed...");
        return 0;
    }

    return atol(szt);
}

const char * W3Client::QueryContentType(){
    static char szt[__DEFAULT_BUF_SIZE] = "\0";
    unsigned long nread = __DEFAULT_BUF_SIZE;
    memset(szt, 0x00, __DEFAULT_BUF_SIZE);
    if (!::HttpQueryInfoA(_hRequest, HTTP_QUERY_CONTENT_TYPE, szt, reinterpret_cast<unsigned long*>(&nread), NULL)){
        __w3cexcept(_szaddress.c_str(), _nport, _t, _szuri.c_str(), "query content-type failed...");
        return NULL;
    }
    return const_cast<char*>(szt);
}

unsigned int W3Client::QueryResult() {
    char szt[__DEFAULT_BUF_SIZE] = "\0";
    unsigned long nread = __DEFAULT_BUF_SIZE;
    memset(szt, 0x00, __DEFAULT_BUF_SIZE);
    if (!::HttpQueryInfoA(_hRequest, HTTP_QUERY_STATUS_CODE, szt, reinterpret_cast<unsigned long*>(&nread), NULL)){
        __w3cexcept(_szaddress.c_str(), _nport, _t, _szuri.c_str(), "query status code failed...");
        return 404;
    }

    return atoi(szt);
}


unsigned long W3Client::QueryRawHeader(unsigned char *buf, unsigned long len){
    if (!::HttpQueryInfoA(_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, buf, &len, NULL)){
        __w3cexcept(_szaddress.c_str(), _nport, _t, _szuri.c_str(), "query cookie failed...");
        return 0;
    }
    return len;
}

unsigned long W3Client::Response(unsigned char *buf, unsigned long len){

    unsigned long nread = 0;

    try{
        if (!_hRequest)
            throw "connection failed...";

        if (!::InternetReadFile(_hRequest, buf, len, &nread))
            throw "response failed...";

    }
    catch (const char *szm){
        ::InternetCloseHandle(_hRequest);
        _hRequest = NULL;
        __w3cexcept(_szaddress.c_str(), _nport, _t, _szuri.c_str(), szm);
    }
    catch (...){
        ::InternetCloseHandle(_hRequest);
        _hRequest = NULL;
        __w3cexcept(_szaddress.c_str(), _nport, _t, _szuri.c_str(), "unknown exception...");
    }

    return nread;
}

bool W3Client::GetFile(const char *szuri, const char *szfile, bool ascii /*=false*/){

    bool r = true;

    try{
        if (!_hOpen || !_hConnection)
            throw "connection failed";

        _hRequest = ::FtpOpenFileA(_hConnection,
            szuri,
            GENERIC_READ,
            (ascii ? INTERNET_FLAG_TRANSFER_ASCII : INTERNET_FLAG_TRANSFER_BINARY),
            NULL);

        if (!_hRequest || _hRequest == INVALID_HANDLE_VALUE)
            throw "request failed...";

        HANDLE f = ::CreateFileA(szfile, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

        unsigned char buf[__DEFAULT_BUF_SIZE] = "\0";
        unsigned long nread = 0;
        if (f){
            while (::InternetReadFile(_hRequest, buf, __DEFAULT_BUF_SIZE, &nread) && nread > 0)
                ::WriteFile(f, buf, nread, &nread, NULL);
            ::CloseHandle(f);
        }

    }
    catch (const char *szm){
        r = false;
        __w3cexcept(_szaddress.c_str(), _nport, _t, szuri, szm);
    }
    catch (...){
        r = false;
        __w3cexcept(_szaddress.c_str(), _nport, _t, szuri, "unknown exception...");
    }

    ::InternetCloseHandle(_hRequest);
    _hRequest = NULL;

    return r;
}

unsigned long W3Client::GetFile(const char *szuri, unsigned char *buf, unsigned long len, bool ascii /*=false*/){

    unsigned long nread = 0;

    try{
        if (!_hOpen || !_hConnection)
            throw "connection failed";

        if (!_hRequest)
            _hRequest = ::FtpOpenFileA(_hConnection,
            szuri,
            GENERIC_READ,
            (ascii ? INTERNET_FLAG_TRANSFER_ASCII : INTERNET_FLAG_TRANSFER_BINARY),
            NULL);

        if (!_hRequest || _hRequest == INVALID_HANDLE_VALUE)
            throw "request failed...";

        if (!::InternetReadFile(_hRequest, buf, len, &nread) || nread <= 0){
            ::InternetCloseHandle(_hRequest);
            _hRequest = NULL;
        }


    }
    catch (const char *szm){
        nread = 0;
        __w3cexcept(_szaddress.c_str(), _nport, _t, szuri, szm);
    }
    catch (...){
        nread = 0;
        __w3cexcept(_szaddress.c_str(), _nport, _t, szuri, "unknown exception...");
    }

    return nread;
}

bool W3Client::PutFile(const char *szuri, const char *szfile, bool ascii /*=false*/){

    bool r = true;

    try{
        if (!_hOpen || !_hConnection)
            throw "connection failed";

        _hRequest = ::FtpOpenFileA(_hConnection,
            szuri,
            GENERIC_WRITE,
            (ascii ? INTERNET_FLAG_TRANSFER_ASCII : INTERNET_FLAG_TRANSFER_BINARY),
            NULL);

        if (!_hRequest || _hRequest == INVALID_HANDLE_VALUE)
            throw "request failed...";

        HANDLE f = ::CreateFileA(szfile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        unsigned char buf[__DEFAULT_BUF_SIZE] = "\0";
        unsigned long nread = 0;
        if (f){
            while (::ReadFile(f, buf, __DEFAULT_BUF_SIZE, &nread, 0) && nread > 0)
                ::InternetWriteFile(_hRequest, buf, nread, &nread);
            ::CloseHandle(f);
        }

    }
    catch (const char *szm){
        r = false;
        __w3cexcept(_szaddress.c_str(), _nport, _t, szuri, szm);
    }
    catch (...){
        r = false;
        __w3cexcept(_szaddress.c_str(), _nport, _t, szuri, "unknown exception...");
    }

    ::InternetCloseHandle(_hRequest);
    _hRequest = NULL;

    return r;
}

unsigned long W3Client::PutFile(const char *szuri, unsigned char *buf, unsigned long len, bool ascii){

    unsigned long nread = 0;

    try{
        if (!_hOpen || !_hConnection)
            throw "connection failed";

        if (!_hRequest)
            _hRequest = ::FtpOpenFileA(_hConnection,
            szuri,
            GENERIC_WRITE,
            (ascii ? INTERNET_FLAG_TRANSFER_ASCII : INTERNET_FLAG_TRANSFER_BINARY),
            NULL);

        if (!_hRequest || _hRequest == INVALID_HANDLE_VALUE)
            throw "request failed...";

        if (!::InternetWriteFile(_hRequest, buf, nread, &nread) || nread <= 0){
            ::InternetCloseHandle(_hRequest);
            _hRequest = NULL;
        }

    }
    catch (const char *szm){
        nread = 0;
        __w3cexcept(_szaddress.c_str(), _nport, _t, szuri, szm);
    }
    catch (...){
        nread = 0;
        __w3cexcept(_szaddress.c_str(), _nport, _t, szuri, "unknown exception...");
    }

    return nread;
}



void __w3curlparse(const char *szurl,
    char *szprotocol, char *szuser, char *szpassword,
    char *szaddress, unsigned long &nport, char *szuri){

    char szport[1024] = "\0";
    unsigned long npos = 0;
    bool bflag = false;

    while (strlen(szurl) > 0 && npos < strlen(szurl) && strncmp((szurl + npos), ":", 1))
        ++npos;

    if (!strncmp((szurl + npos + 1), "/", 1)){   // is protocol
        if (szprotocol){
            strncpy(szprotocol, szurl, npos);
            szprotocol[npos] = 0;
        }
        bflag = true;
    }
    else{  // is host
        if (szprotocol){
            strncpy(szprotocol, "http", 4);
            szprotocol[5] = '\0';
        }
    }

    unsigned long nsp = 0, usp = 0;

    if (bflag){
        usp = nsp = npos += 3;
    }
    else{
        usp = nsp = npos = 0;
    }

    while (strlen(szurl) > 0 && usp < strlen(szurl) && strncmp((szurl + usp), "@", 1))
        ++usp;

    if (usp < strlen(szurl)){ // find username and find password
        unsigned long ssp = nsp;
        while (strlen(szurl) > 0 && npos < strlen(szurl) && strncmp((szurl + ssp), ":", 1))
            ++ssp;

        if (ssp < usp){// find
            strncpy(szuser, szurl + nsp, ssp - nsp);
            szuser[ssp - nsp + 1] = '\0';
            strncpy(szpassword, szurl + ssp + 1, usp - ssp - 1);
            szpassword[usp - ssp] = '\0';
        }

        nsp = npos = usp + 1;
    }

    bflag = false;
    while (strlen(szurl) > 0 && npos < strlen(szurl) && strncmp((szurl + npos), "/", 1))
        ++npos;

    unsigned long nf = nsp;

    for (; nf <= npos; nf++){
        if (!strncmp((szurl + nf), ":", 1)){ // find PORT
            bflag = true;
            break;
        }
    }

    if (bflag){
        char sztmp[1024] = "\0";
        strncpy(sztmp, (szurl + nf + 1), npos - nf);
        nport = atol(sztmp);
        strncpy(szaddress, (szurl + nsp), nf - nsp);
    }
    else if (!strcmp(szprotocol, "https")){
        nport = INTERNET_DEFAULT_HTTPS_PORT;
        strncpy(szaddress, (szurl + nsp), npos - nsp);
    }
    else if (!strcmp(szprotocol, "ftp")){
        nport = INTERNET_DEFAULT_FTP_PORT;
        strncpy(szaddress, (szurl + nsp), npos - nsp);
    }
    else {
        nport = INTERNET_DEFAULT_HTTP_PORT;
        strncpy(szaddress, (szurl + nsp), npos - nsp);
    }

    if (npos < strlen(szurl)){ // find URI
        strncpy(szuri, (szurl + npos), strlen(szurl) - npos);
    }
    else{
        szuri[0] = '/';
        szuri[1] = '\0';
    }

    return;
}

// Asynchronized www client
bool AsyncW3Client::Connect(const char *szaddress,
    INTERNET_STATUS_CALLBACK lpfn,
    const char *szuser /*=NULL*/,
    const char *szpassword /*=NULL*/,
    const char *szagent /*=__W3_DEFAULT_AGENT*/){

    char szp[__DEFAULT_BUF_SIZE] = "\0", szus[__DEFAULT_BUF_SIZE] = "\0", szpw[__DEFAULT_BUF_SIZE] = "\0";
    char sza[__DEFAULT_BUF_SIZE] = "\0", szuri[__DEFAULT_BUF_SIZE] = "\0";
    unsigned long port = 0;

    __w3curlparse(szaddress, szp, szus, szpw, sza, port, szuri);

    w3t wt;

    if (!strncmp(szp, "https", 5))
        wt = w3https;
    else if (!strncmp(szp, "http", 4))
        wt = w3http;
    else if (!strncmp(szp, "ftp", 3))
        wt = w3ftp;

    _szuri = szuri;

    return Connect(sza, port,
        lpfn,
        ((szuser || (!szuser && strlen(szus) == 0)) ? szuser : szus),
        ((szpassword || (!szpassword && strlen(szpw) == 0)) ? szpassword : szpw),
        wt, szagent);
}

bool AsyncW3Client::Connect(const char *szaddress,
    long nport,
    INTERNET_STATUS_CALLBACK lpfn,
    const char *szuser /*=NULL*/,
    const char *szpassword /*=NULL*/,
    w3t t /*=w3http*/,
    const char *szagent /*=__W3_DEFAULT_AGENT*/){

    bool r = true;

    try{
        _hOpen = ::InternetOpenA(szagent, // agent
            INTERNET_OPEN_TYPE_PRECONFIG, // access type
            NULL, // proxy
            NULL, // proxy by pass
            INTERNET_FLAG_ASYNC); // flags
        if (!_hOpen || _hOpen == INVALID_HANDLE_VALUE)
            throw "open internet failed...";

        INTERNET_STATUS_CALLBACK pc = ::InternetSetStatusCallbackA(_hOpen, lpfn);
        //if(!pc || pc==INTERNET_INVALID_STATUS_CALLBACK )
        //  throw "set status call-back functino failed...";

        switch (t) {
        case w3ftp:
            _hConnection = ::InternetConnectA(_hOpen,
                szaddress, (nport ? nport : INTERNET_DEFAULT_FTP_PORT),
                szuser, szpassword,
                INTERNET_SERVICE_FTP,
                INTERNET_FLAG_PASSIVE,
                reinterpret_cast<unsigned long>(this));
            break;
        case w3http:
            _hConnection = ::InternetConnectA(_hOpen,
                szaddress, (nport ? nport : INTERNET_DEFAULT_HTTP_PORT),
                szuser, szpassword,
                INTERNET_SERVICE_HTTP,
                0,
                reinterpret_cast<unsigned long>(this));
            break;
        case w3https:
            _hConnection = ::InternetConnectA(_hOpen,
                szaddress, (nport ? nport : INTERNET_DEFAULT_HTTPS_PORT),
                szuser, szpassword,
                INTERNET_SERVICE_HTTP,
                0,
                reinterpret_cast<unsigned long>(this));
            break;
        }

        if (!_hConnection || _hConnection == INVALID_HANDLE_VALUE)
            throw "connect failed...";

        if (::InternetAttemptConnect(NULL) != ERROR_SUCCESS)
            throw "connect failed...";

        _szaddress = szaddress;
        if (!nport){
            switch (t) {
            case w3ftp:
                _nport = INTERNET_DEFAULT_FTP_PORT;
                break;
            case w3http:
                _nport = INTERNET_DEFAULT_HTTP_PORT;
                break;
            case w3https:
                _nport = INTERNET_DEFAULT_HTTPS_PORT;
                break;
            }
        }
        else
            _nport = nport;
        _t = t;

        if (szuser)
            _szuser = szuser;

        if (szpassword)
            _szpassword = szpassword;

        InitializeCookies();
        InitializePostArguments();

    }
    catch (const char *szm){
        r = false;
        if (_hOpen || _hOpen != INVALID_HANDLE_VALUE)
            ::InternetCloseHandle(_hOpen);
        if (_hConnection || _hConnection != INVALID_HANDLE_VALUE)
            ::InternetCloseHandle(_hConnection);
        __w3cexcept(szaddress, nport, t, szm);
    }
    catch (...){
        r = false;
        if (_hOpen || _hOpen != INVALID_HANDLE_VALUE)
            ::CloseHandle(_hOpen);
        if (_hConnection || _hConnection != INVALID_HANDLE_VALUE)
            ::CloseHandle(_hConnection);
        __w3cexcept(szaddress, nport, t, "unknown exception...");
    }

    return r;
}

bool AsyncW3Client::RequestGet(const char *szuri, const char *szref /*=NULL*/){

    static LPCSTR szAcceptType = (__HTTP_ACCEPT_TYPE);

    if (!_hConnection || _hConnection == INVALID_HANDLE_VALUE)
        throw "handle not opened...";

    _hRequest = ::HttpOpenRequestA(_hConnection,
        __HTTP_VERB_GET, // HTTP Verb
        szuri, // Object Name
        HTTP_VERSIONA, // Version
        szref, // Reference
        &szAcceptType, // Accept Type
        INTERNET_FLAG_NO_CACHE_WRITE | (_t == w3https ? INTERNET_FLAG_SECURE : 0),
        reinterpret_cast<unsigned long>(this)); // context call-back point

    if (!_hRequest || _hRequest == INVALID_HANDLE_VALUE)
        throw "request failed...";

    // REPLACE HEADER
    if (!::HttpAddRequestHeadersA(_hRequest,
        __HTTP_ACCEPT, strlen(__HTTP_ACCEPT),
        HTTP_ADDREQ_FLAG_REPLACE))
        throw "additional header failed...";

    // COOKIE ADD
    if (_listcookies.size() > 0){
        string szurl;
        switch (_t) {
        case w3http:
            szurl = "http://";
            break;
        case w3https:
            szurl = "https://";
            break;
        }
        szurl += _szaddress.c_str();
        if (!((_t == w3http && _nport == INTERNET_DEFAULT_HTTP_PORT) || (_t == w3https && _nport == INTERNET_DEFAULT_HTTPS_PORT))){
            char tmp[10] = "\0";
            sprintf_s(tmp, ":%d", _nport);
            szurl += tmp;
        }
        szurl += szuri;

        for (list<HTTP_COOKIE*>::iterator it = _listcookies.begin(); it != _listcookies.end(); it++){
            HTTP_COOKIE *pc = reinterpret_cast<HTTP_COOKIE*>(*it);
            if (!::InternetSetCookieA(szurl.c_str(), pc->name.c_str(), pc->value.c_str()))
                throw "add cookie failed...";
        }
    }

    // SEND REQUEST
    if (!::HttpSendRequest(_hRequest,   // handle by returned HttpOpenRequest
        NULL, // additional HTTP header
        0, // additional HTTP header length
        NULL, // additional data in HTTP Post or HTTP Put
        0)// additional data length
        &&
        ::GetLastError() != ERROR_IO_PENDING
        ){
        throw "asynchronized request failed...";
    }

    return true;
}

bool AsyncW3Client::RequestPost(const char *szuri, const char *szref /*=NULL*/){

    static LPCSTR szAcceptType = __HTTP_ACCEPT_TYPE;
    static LPCSTR szContentType = "Content-Type: application/x-www-form-urlencoded\r\n";

    unsigned char *buf = NULL;
    unsigned long len = 0;

    if (!_hConnection || _hConnection == INVALID_HANDLE_VALUE)
        throw "handle not opened...";


    _hRequest = ::HttpOpenRequestA(_hConnection,
        __HTTP_VERB_POST, // HTTP Verb
        szuri, // Object Name
        HTTP_VERSIONA, // Version
        szref, // Reference
        &szAcceptType, // Accept Type                                                           
        INTERNET_FLAG_KEEP_CONNECTION |
        INTERNET_FLAG_NO_CACHE_WRITE |
        INTERNET_FLAG_FORMS_SUBMIT |
        (_t == w3https ? INTERNET_FLAG_SECURE : 0),
        reinterpret_cast<unsigned long>(this)); // context call-back point

    if (!_hRequest || _hRequest == INVALID_HANDLE_VALUE)
        throw "request failed...";

    // REPLACE HEADER
    if (!::HttpAddRequestHeadersA(_hRequest, __HTTP_ACCEPT, strlen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
        throw "additional header failed...";

    // COOKIE ADD
    if (_listcookies.size() > 0){
        string szurl;
        switch (_t) {
        case w3http:
            szurl = "http://";
            break;
        case w3https:
            szurl = "https://";
            break;
        }
        szurl += _szaddress.c_str();
        if (!((_t == w3http && _nport == INTERNET_DEFAULT_HTTP_PORT) || (_t == w3https && _nport == INTERNET_DEFAULT_HTTPS_PORT))){
            char tmp[10] = "\0";
            sprintf_s(tmp, ":%d", _nport);
            szurl += tmp;
        }
        szurl += szuri;

        for (list<HTTP_COOKIE*>::iterator it = _listcookies.begin(); it != _listcookies.end(); it++){
            HTTP_COOKIE *pc = reinterpret_cast<HTTP_COOKIE*>(*it);
            if (!::InternetSetCookieA(szurl.c_str(), pc->name.c_str(), pc->value.c_str()))
                throw "add cookie failed...";
        }
    }

    // GET POST ARGUMENTS
    buf = reinterpret_cast<unsigned char*>(::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, GetPostArgumentsLength() + 1));
    len = GetPostData(buf, GetPostArgumentsLength() + 1);

    // SEND REQUEST WITH HttpSendRequestEx and InternetWriteFile
    static INTERNET_BUFFERS InternetBufferIn = { 0 };
    InternetBufferIn.dwStructSize = sizeof(INTERNET_BUFFERS);
    InternetBufferIn.Next = NULL;
    InternetBufferIn.lpcszHeader = s2ws(szContentType).c_str();
    InternetBufferIn.dwHeadersLength = strlen(szContentType);
    InternetBufferIn.lpvBuffer = buf;
    InternetBufferIn.dwBufferLength = len;

    if (!::HttpSendRequestEx(_hRequest, &InternetBufferIn, NULL, HSR_INITIATE, reinterpret_cast<unsigned long>(this))
        &&
        ::GetLastError() != ERROR_IO_PENDING
        ){
        // free
        ::HeapFree(::GetProcessHeap(), 0, buf);
        throw "request failed";
    }

    WaitCompleteRequest();

    if (!::HttpEndRequest(_hRequest, NULL, HSR_ASYNC | HSR_INITIATE, reinterpret_cast<unsigned long>(this))
        &&
        ::GetLastError() != ERROR_IO_PENDING
        ){
        // free
        ::HeapFree(::GetProcessHeap(), 0, buf);
        throw "request failed";
    }

    ::HeapFree(::GetProcessHeap(), 0, buf);

    return true;
}

bool AsyncW3Client::RequestPost2(const char *szuri, const char *szref /*=NULL*/){
    static LPCSTR szAcceptType = __HTTP_ACCEPT_TYPE;
    static LPCSTR szContentType = "Content-Type: multipart/form-data; boundary=--MULTI-PARTS-FORM-DATA-BOUNDARY\r\n";

    unsigned char *buf = NULL;
    unsigned long len = 0;

    if (!_hConnection || _hConnection == INVALID_HANDLE_VALUE)
        throw "handle not opened...";


    _hRequest = ::HttpOpenRequestA(_hConnection,
        __HTTP_VERB_POST, // HTTP Verb
        szuri, // Object Name
        HTTP_VERSIONA, // Version
        szref, // Reference
        &szAcceptType, // Accept Type                                                           
        INTERNET_FLAG_KEEP_CONNECTION |
        INTERNET_FLAG_NO_CACHE_WRITE |
        INTERNET_FLAG_FORMS_SUBMIT |
        (_t == w3https ? INTERNET_FLAG_SECURE : 0),
        reinterpret_cast<unsigned long>(this)); // context call-back point

    if (!_hRequest || _hRequest == INVALID_HANDLE_VALUE)
        throw "request failed...";

    // REPLACE HEADER
    if (!::HttpAddRequestHeadersA(_hRequest, __HTTP_ACCEPT, strlen(__HTTP_ACCEPT), HTTP_ADDREQ_FLAG_REPLACE))
        throw "additional header failed...";

    if (!::HttpAddRequestHeadersA(_hRequest, szContentType, strlen(szContentType), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
        throw "additional header failed...";

    // COOKIE ADD
    if (_listcookies.size() > 0){
        string szurl;
        switch (_t) {
        case w3http:
            szurl = "http://";
            break;
        case w3https:
            szurl = "https://";
            break;
        }
        szurl += _szaddress.c_str();
        if (!(
            (_t == w3http && _nport == INTERNET_DEFAULT_HTTP_PORT) ||
            (_t == w3https && _nport == INTERNET_DEFAULT_HTTPS_PORT)
            )){
            char tmp[10] = "\0";
            sprintf_s(tmp, ":%d", _nport);
            szurl += tmp;
        }
        szurl += szuri;

        for (list<HTTP_COOKIE*>::iterator it = _listcookies.begin(); it != _listcookies.end(); it++){
            HTTP_COOKIE *pc = reinterpret_cast<HTTP_COOKIE*>(*it);
            if (!::InternetSetCookieA(szurl.c_str(), pc->name.c_str(), pc->value.c_str()))
                throw "add cookie failed...";
        }
    }

    // build multi-parts/form-data
    len = AllocMultiPartsFormData(buf, "--MULTI-PARTS-FORM-DATA-BOUNDARY");

    // ADD HEADER CONTENT LENGTH
    char szcl[__DEFAULT_BUF_SIZE] = "\0";
    sprintf_s(szcl, "Content-Length: %d\r\n", len);

    if (!::HttpAddRequestHeadersA(_hRequest, szcl, strlen(szcl), HTTP_ADDREQ_FLAG_ADD_IF_NEW))
        throw "additional header failed...";

    // SEND REQUEST WITH HttpSendRequestEx and InternetWriteFile
    static INTERNET_BUFFERSA InternetBufferIn = { 0 };
    InternetBufferIn.dwStructSize = sizeof(INTERNET_BUFFERS);
    InternetBufferIn.Next = NULL;
    InternetBufferIn.lpvBuffer = buf;
    InternetBufferIn.dwBufferLength = len;

    if (!::HttpSendRequestExA(_hRequest, &InternetBufferIn, NULL, HSR_INITIATE, reinterpret_cast<unsigned long>(this))
        &&
        ::GetLastError() != ERROR_IO_PENDING
        ){
        // free
        FreeMultiPartsFormData(buf);
        throw "request failed";
    }

    WaitCompleteRequest();

    if (!::HttpEndRequest(_hRequest, NULL, HSR_ASYNC | HSR_INITIATE, reinterpret_cast<unsigned long>(this))
        &&
        ::GetLastError() != ERROR_IO_PENDING
        ){
        // free
        FreeMultiPartsFormData(buf);
        throw "request failed";
    }

    // free multi-parts/form-data
    FreeMultiPartsFormData(buf);

    return true;
}

void AsyncW3Client::SetCompleteRequest(){ ::SetEvent(_hCompleteRequestEvent); }
bool AsyncW3Client::WaitCompleteRequest(unsigned long ntime /*=INFINITE*/){
    if (!_hCompleteRequestEvent || _hCompleteRequestEvent == INVALID_HANDLE_VALUE)
        false;
    return ::WaitForSingleObject(_hCompleteRequestEvent, ntime) == WAIT_OBJECT_0 ? true : false;
}