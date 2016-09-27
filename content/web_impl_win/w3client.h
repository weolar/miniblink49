/*
* $ dally project : www client
* ---------------------------------------------------------------------------------------
* description :
*    W3Client ; synchronized www client class
*    AsyncW3Client ; asynchronized www client class
*
*    - HTTP/HTTPS support
*       - GET method support
*       - POST method support
*       - POST Multiparts/form-data method support
*    - FTP support
*       - Get support
*       - Put support
*    - ETC
*       - url parse
*
* special thanks.
*    Yun Myungwook, Peter Newman, ...
*
* ----------------------------------------------------------------------------------------
* coded by Heo Yongseon ( gooshin@zzem.net )
*
* This code distributed under BSD License Style.
*
* -----------------------------------------------------------------------------------------
* TO DO :
*
* -----------------------------------------------------------------------------------------
* sample
*
*   // synchronized(blocked) www client example
*   #include <iostream>
*   #include <net/w3c.h>
*
*   using namespace std;
*   using namespace dally;
*
*   int main(int argc, char *argv[]){
*
*      W3Client w3;
*
*      if(w3.Connect("http://google.com")){
*          if(w3.Request("/")){
*              char buf[1024]="\0";
*              while(w3.Response(reinterpret_cast<unsigned char *>(buf), 1024))
*                  cout << buf ;
*          }
*          w3.Close();
*      }
*      return 0;
*   }
*
*   // asynchronized www client example
*   #include <iostream>
*   #include <net/w3c.h>
*   #include <wt.h>
*   #include <windows.h>
*
*   using namespace std;
*   using namespace dally;
*
*   CRITICAL_SECTION __cs;
*
*   class AsDown : public AsyncW3Client, public IWORKERTHREAD {
*   public:
*      AsDown(unsigned int idx):AsyncW3Client(), IWORKERTHREAD(idx){}
*      virtual ~AsDown(){}
*   private:
*      virtual void OnWork(){
*          while(true){
*              WaitCompleteRequest();
*
*              unsigned char buf[1024]="\0";
*              while(Response(buf, 1024)){
*                      ::EnterCriticalSection(&__cs);
*                      cout << reinterpret_cast<char*>(buf);
*                      ::LeaveCriticalSection(&__cs);
*                      memset(buf, 0x00, 1024);
*              }
*
*              ::Sleep(500);
*          }
*      }
*   };
*
*   void CALLBACK __getstatus(  HINTERNET hInternet,
*                                                    DWORD_PTR dwContext,
*                                                    DWORD dwInternetStatus,
*                                                    LPVOID lpvStatusInformation,
*                                                    DWORD dwStatusInformationLength
*                                                    ){
*
*      AsyncW3Client *pcontext=reinterpret_cast<AsyncW3Client*>(dwContext);
*
*      unsigned long nbytes=0;
*      ::EnterCriticalSection(&__cs);
*      switch(dwInternetStatus){
*      case INTERNET_STATUS_SENDING_REQUEST:
*          cout << "request sending..." << endl;
*          break;
*      case INTERNET_STATUS_REQUEST_SENT:
*          {
*              unsigned long *pnsent=(unsigned long*)lpvStatusInformation;
*              cout << "bytes sent: " << *pnsent << endl;
*              nbytes+=*pnsent;
*              cout << "request sent..." << endl;
*          }
*          break;
*      case INTERNET_STATUS_REQUEST_COMPLETE:
*          {
*              INTERNET_ASYNC_RESULT *pAsyncRes = (INTERNET_ASYNC_RESULT *)lpvStatusInformation;
*              cout << "Function call finished" << endl;
*              cout << "dwResult: " << pAsyncRes->dwResult << endl;
*              cout << "dwError:  " << pAsyncRes->dwError << endl;
*              cout.flush();
*              pcontext->SetCompleteRequest();
*              cout << "request complete..." << endl;
*          }
*          break;
*      }
*      ::LeaveCriticalSection(&__cs);
*
*      return;
*   }
*
*   int main(int argc, char *argv[]){
*
*      ::InitializeCriticalSection(&__cs);
*
*      AsDown client(3);
*
*      if(client.Connect("http://gooshin.zzem.net", __getstatus)){
*
*          __wtstart(client);
*
*          client.Request("/test.php");
*          Sleep(5000);
*
*          client.InitializePostArguments();
*          client.AddPostArgument("f[]", "d:\\log1.txt", true);
*          client.AddPostArgument("f[]", "d:\\log2.txt", true);
*          client.AddPostArgument("f[]", "d:\\log3.txt", true);
*          client.Request("/test.php", AsDown::reqPostMultipartsFormdata);
*          Sleep(5000);
*
*          client.InitializePostArguments();
*          client.AddPostArgument("f", "sss");
*          client.Request("/test2.php", AsDown::reqPost);
*          Sleep(5000);
*
*          __wtwait(client);
*          client.Close();
*      }
*
*      ::DeleteCriticalSection(&__cs);
*
*      return 0;
*   }
*
*/

#ifdef WIN32

#ifndef __DALLY_HTTP_CLIENT
#define __DALLY_HTTP_CLIENT
#include "stdafx.h"

#pragma warning(disable:4786)

#include <windows.h>
#include <winreg.h>
#include <wininet.h>
#include <string>
#include <list>
#include <cstdio>

#pragma comment(lib, "wininet.lib")

#define __W3_DEFAULT_AGENT "Mozilla/4.0 (compatible; )"
#define __DEFAULT_BOUNDRY_TAG "--MULTI-PARTS-FORM-DATA-BOUNDARY-"
#define __SIZE_HTTP_HEAD_LINE   2048

void __w3curlparse(const char *szurl,
    char *szprotocol, char *szuser, char *szpassword,
    char *szaddress, unsigned long &nport, char *szuri);

// synchronized www client
class W3Client {
public:
    enum w3t { w3ftp, w3http, w3https };
    enum w3m { reqGet, reqPost, reqPostMultipartsFormdata };
    typedef struct __HTTP_ARG {
        std::string name;
        std::string value;
        bool f;
        __HTTP_ARG(const char *szn, const char *szv, bool b = false) : name(szn), value(szv) { if (b) f = true; else f = false; }
        __HTTP_ARG(const char *szn, const int v) : name(szn), f(false) { char t[15] = "\0"; sprintf(t, "%d", v); value = t; }
        __HTTP_ARG(const char *szn, const long v) : name(szn), f(false) { char t[15] = "\0"; sprintf(t, "%d", v); value = t; }
        __HTTP_ARG(const char *szn, const float v) : name(szn), f(false) { char t[15] = "\0"; sprintf(t, "%f", v); value = t; }
        __HTTP_ARG(const char *szn, const double v) : name(szn), f(false) { char t[15] = "\0"; sprintf_s(t, "%f", v); value = t; }
        const char *type(){
            static char szt[1024] = "\0";
            long nlen = 1024;
            unsigned long ndot = 0;

            for (ndot = name.length(); ndot>0; ndot--)
                if (!strncmp(name.c_str() + ndot, ".", 1))
                    break;

            HKEY    hKEY;
            const char *szword = name.c_str() + ndot;

            if (RegOpenKeyExA(HKEY_CLASSES_ROOT, szword, 0, KEY_QUERY_VALUE, &hKEY) == ERROR_SUCCESS){
                if (RegQueryValueExA(hKEY, ("Content Type"), NULL, NULL, (LPBYTE)szt, (unsigned long*)&nlen) != ERROR_SUCCESS)
                    strncpy(szt, "application/octet-stream", strlen("application/octet-stream"));
                RegCloseKey(hKEY);
            }
            else{
                strncpy(szt, "application/octet-stream", strlen("application/octet-stream"));
            }
            return szt;
        }
        unsigned long length(){ return name.length() + value.length() + 1; }
        unsigned long dump(unsigned char *buf, unsigned long s){
            memcpy(buf, name.c_str(), name.length());
            memcpy(buf + name.length(), "=", 1);
            memcpy(buf + name.length() + 1, value.c_str(), value.length());
            return name.length() + value.length() + 1;
        }
        unsigned long length2() {
            unsigned long len = 0;
            if (f){
                HANDLE hFile = ::CreateFileA(value.c_str(),
                    GENERIC_READ, // desired access
                    FILE_SHARE_READ, // share mode
                    NULL, // security attribute
                    OPEN_EXISTING, // create disposition
                    FILE_ATTRIBUTE_NORMAL, // flag and attributes
                    NULL); // template file handle

                len = __SIZE_HTTP_HEAD_LINE * 3 + ::GetFileSize(hFile, NULL);
                ::CloseHandle(hFile);
            }
            else
                len = __SIZE_HTTP_HEAD_LINE * 4;
            return len;
        }
        unsigned long dump2(unsigned char *buf, unsigned long s, const char *boundry = __DEFAULT_BOUNDRY_TAG){
            unsigned long l = 0;
            if (f){ // file
                /*
                sprintf( reinterpret_cast<char*>(buf),
                "--%s\r\n"
                "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
                "\r\n"
                "%s\r\n",
                boundry, name.c_str(), value.c_str(), type());
                l=2+strlen(boundry)+2+38+name.length()+13+value.length()+5+strlen(type())+2;
                */

                sprintf_s(reinterpret_cast<char*>(buf),
                    "--%s\r\n"
                    "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
                    "\r\n",
                    boundry, name.c_str(), value.c_str());
                l = 2 + strlen(boundry) + 2 + 38 + name.length() + 13 + value.length() + 5;

                HANDLE hFile = ::CreateFileA(value.c_str(),
                    GENERIC_READ, // desired access
                    FILE_SHARE_READ, // share mode
                    NULL, // security attribute
                    OPEN_EXISTING, // create disposition
                    FILE_ATTRIBUTE_NORMAL, // flag and attributes
                    NULL); // template file handle

                if (hFile && hFile != INVALID_HANDLE_VALUE){

                    unsigned long ns = ::GetFileSize(hFile, NULL);
                    unsigned char *pbuf = buf + l;
                    unsigned long nread = 0;
                    unsigned long nt = 0;

                    while (::ReadFile(hFile, pbuf + nt, 1024, &nread, NULL) && nread>0 && nt <= ns)
                        nt += nread;

                    memcpy(&(buf[l + nt]), "\r\n", 2);
                    ::CloseHandle(hFile);

                    l += nt;
                    l += 2;
                }
            }
            else{ // general form data
                sprintf(reinterpret_cast<char*>(buf),
                    "--%s\r\n"
                    "Content-Disposition: form-data; name=\"%s\"\r\n"
                    "\r\n"
                    "%s\r\n",
                    boundry, name.c_str(), value.c_str());
                l = 2 + strlen(boundry) + 2 + 38 + name.length() + 5 + value.length() + 2;
            }
            return l;
        }
    } HTTP_ARG, *PHTTP_ARG;

    typedef struct __HTTP_COOKIE {
        std::string name;
        std::string value;
        __HTTP_COOKIE(const __HTTP_COOKIE& c) : name(c.name), value(c.value) {}
        __HTTP_COOKIE(const char *szn, const char*szv) : name(szn), value(szv) {}
        __HTTP_COOKIE(const char *szn, const int v) : name(szn) { char t[15] = "\0"; sprintf(t, "%d", v); value = t; }
        __HTTP_COOKIE(const char *szn, const long v) : name(szn) { char t[15] = "\0"; sprintf(t, "%d", v); value = t; }
        __HTTP_COOKIE(const char *szn, const float v) : name(szn) { char t[15] = "\0"; sprintf(t, "%f", v); value = t; }
        __HTTP_COOKIE(const char *szn, const double v) : name(szn) { char t[15] = "\0"; sprintf(t, "%f", v); value = t; }
    } HTTP_COOKIE, *PHTTP_COOKIE;
public:
    W3Client(){ _hOpen = NULL; _hConnection = NULL, _hRequest = NULL; }
    virtual ~W3Client(){ InitializePostArguments(); InitializeCookies(); }
public:
    bool Connect(const char *szaddress,
        const char *szuser = NULL, const char *szpassword = NULL, const char *szagent = __W3_DEFAULT_AGENT);
    virtual bool Connect(const char *szaddress, long nport,
        const char *szuser = NULL, const char *szpassword = NULL,
        w3t t = w3http, const char *szagent = __W3_DEFAULT_AGENT);
    const char *GetURI(){ return _szuri.c_str(); }
    void Close();

    void InitializePostArguments();
    void AddPostArgument(const char *szname, const int nvalue);
    void AddPostArgument(const char *szname, const long nvalue);
    void AddPostArgument(const char *szname, const float nvalue);
    void AddPostArgument(const char *szname, const double nvalue);
    void AddPostArgument(const char *szname, const char *szvalue, bool bfile = false);

    void InitializeCookies();
    void AddCookie(const char *szname, const double value);
    void AddCookie(const char *szname, const float value);
    void AddCookie(const char *szname, const long value);
    void AddCookie(const char *szname, const int value);
    void AddCookie(const char *szname, const char *szvalue);

    bool Request(const char *szuri, w3m m = reqGet, const char *szref = NULL);
    unsigned long Response(unsigned char *buf, unsigned long len);
    unsigned int QueryResult();
    const char * QueryContentType();
    unsigned long QueryContentLength();
    unsigned long QueryCookie(unsigned char *buf, unsigned long len, unsigned long idx = 0);
    unsigned long QueryRawHeader(unsigned char *buf, unsigned long len);

    bool PutFile(const char *szuri, const char *szfile, bool ascii = false);
    bool GetFile(const char *szuri, const char *szfile, bool ascii = false);
    unsigned long PutFile(const char *szuri, unsigned char *buf, unsigned long len, bool ascii = false);
    unsigned long GetFile(const char *szuri, unsigned char *buf, unsigned long len, bool ascii = false);

protected:
    unsigned long GetMultiPartsFormDataLength();
    void FreeMultiPartsFormData(unsigned char *buf);
    unsigned long AllocMultiPartsFormData(unsigned char *&buf, const char *szboundry = __DEFAULT_BOUNDRY_TAG);
    unsigned long GetPostData(unsigned char *buf, unsigned long len);
    unsigned long GetPostArgumentsLength();
private:
    virtual bool RequestPost2(const char *szuri, const char *szref = NULL);
    virtual bool RequestPost(const char *szuri, const char *szref = NULL);
    virtual bool RequestGet(const char *szuri, const char *szref = NULL);
protected:
    HINTERNET _hOpen; // internet open handle
    HINTERNET _hConnection; // internet connection handle
    HINTERNET _hRequest; // internet request handle
    std::string _szaddress;
    std::string _szuser;
    std::string _szpassword;
    std::string _szuri;
    long _nport;
    w3t _t;

    std::list<HTTP_ARG*> _listargs;
    std::list<HTTP_COOKIE*> _listcookies;
};

// Asynchronized www client
class AsyncW3Client : public W3Client {
public:
    AsyncW3Client() :W3Client(){ _hCompleteRequestEvent = NULL; }
public:
    bool Connect(const char *szaddress,
        INTERNET_STATUS_CALLBACK lpfn,
        const char *szuser = NULL,
        const char *szpassword = NULL,
        const char *szagent = __W3_DEFAULT_AGENT);
    bool Connect(const char *szaddress, long nport,
        INTERNET_STATUS_CALLBACK lpfn,
        const char *szuser = NULL, const char *szpassword = NULL,
        w3t t = w3http, const char *szagent = __W3_DEFAULT_AGENT);
    bool Request(const char *szuri, w3m m = reqGet, const char *szref = NULL){
        _hCompleteRequestEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        return W3Client::Request(szuri, m, szref);
    }
    unsigned long Response(unsigned char *buf, unsigned long len){
        ::CloseHandle(_hCompleteRequestEvent);
        _hCompleteRequestEvent = NULL;
        return W3Client::Response(buf, len);
    }
public:
    void SetCompleteRequest();
    bool WaitCompleteRequest(unsigned long ntime = INFINITE);
private:
    virtual bool RequestPost2(const char *szuri, const char *szref = NULL);
    virtual bool RequestPost(const char *szuri, const char *szref = NULL);
    virtual bool RequestGet(const char *szuri, const char *szref = NULL);
private:
    HANDLE _hCompleteRequestEvent;
};

#endif // !defined(__DALLY_HTTP_CLIENT)

#endif // defined(WIN32)