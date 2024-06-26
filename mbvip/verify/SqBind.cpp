
#include "verify/SqBind.h"

#include "verify/VerifyJsCnutRes.h"
#include "squirrel.h"
#include "sqstdio.h"
#include "sqstdaux.h"
#include "sqstdmath.h"
#include "sqstdblob.h"
#include "sqstdsystem.h"
#include "sqstdstring.h"
#include "verify/Machine.h"
#include "common/StringUtil.h"
#include "common/SimpleRsa.h"
#include "common/Util.h"

#include <stdarg.h>
#include <windows.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#include <vector>
extern HMODULE g_hModule;

static SQInteger registerGlobalFunc(HSQUIRRELVM v, SQFUNCTION f, const SQChar* fname)
{
    sq_pushroottable(v);
    sq_pushstring(v, fname, -1);
    sq_newclosure(v, f, 0); //create a new function
    sq_newslot(v, -3, SQFalse);
    sq_pop(v, 1); //pops the root table  
    return 0;
}

SQInteger nativeGetVolumeID(HSQUIRRELVM v)
{
    const SQInteger size = 512;
    char buffer[size] = { 0 };
    unsigned int len = size;

    len = Machine::getVolumeID(buffer, len);
    if (0 == len) {
        OutputDebugStringA("nativeGetVolumeID fail\n");
        return 0;
    }

    std::string result = common::base64Encode((const unsigned char *)buffer, len);
    std::wstring resultW = common::utf8ToUtf16(result);
    sq_pushstring(v, resultW.c_str(), resultW.length());

    return 1;
}

SQInteger nativeGetMacID(HSQUIRRELVM v)
{
    const SQInteger size = 512;
    char buffer[size] = { 0 };
    unsigned int len = size;

    len = Machine::getAllMacId(buffer, len);
    if (0 == len) {
        OutputDebugStringA("nativeGetMacID fail\n");
        return 0;
    }

    std::string result = common::base64Encode((const unsigned char *)buffer, len);
    std::wstring resultW = common::utf8ToUtf16(result);
    sq_pushstring(v, resultW.c_str(), resultW.length());

    return 1;
}

SQInteger nativeGetDiskID(HSQUIRRELVM v)
{
    const SQInteger size = 512;
    char buffer[size] = { 0 };
    unsigned int len = size;

    len = Machine::getDiskId(buffer, len);
    if (0 == len) {
        OutputDebugStringA("nativeGetDiskID fail\n");
        return 0;
    }

    std::string result = common::base64Encode((const unsigned char *)buffer, len);
    std::wstring resultW = common::utf8ToUtf16(result);
    sq_pushstring(v, resultW.c_str(), resultW.length());

    return 1;
}

SQInteger nativeGetCpuID(HSQUIRRELVM v)
{
    const SQInteger size = 512;
    char buffer[size] = { 0 };
    unsigned int len = size;

    len = Machine::getAllCpuId(buffer, len);
    if (0 == len) {
        OutputDebugStringA("nativeGetCpuID fail\n");
        return 0;
    }

    std::string result = common::base64Encode((const unsigned char *)buffer, len);
    std::wstring resultW = common::utf8ToUtf16(result);
    sq_pushstring(v, resultW.c_str(), resultW.length());

    return 1;
}

const WCHAR* blackParentProcessList[] = {
    L"devenv.exe",
    L"e.exe",
    L"delphi32.exe",
    nullptr,
};

SQInteger nativeAlert(HSQUIRRELVM v)
{
    // weolar
    const wchar_t * name = Machine::getParentName();

    bool find = false;
    for (int i = 0; blackParentProcessList[i]; ++i) {
        if (0 == _wcsicmp(name, blackParentProcessList[i])) {
            find = true;
            break;
        }
    }
    delete name;

    HMODULE hMod = LoadLibraryW(L"Kernel32.dll");
    typedef BOOL (__stdcall* FN_IsDebuggerPresent)();
    FN_IsDebuggerPresent pIsDebuggerPresent = (FN_IsDebuggerPresent)GetProcAddress(hMod, "IsDebuggerPresent");

    if (find || pIsDebuggerPresent()) {
        ::MessageBoxW(nullptr, L"您使用的是未注册版本，请到miniblink.net注册后使用，支持下正版", L"未注册", 0);
        ::ExitProcess(-1);
    }
    return 0;
}

SQInteger nativeBase64ToBlob(HSQUIRRELVM v)
{
    const SQChar* license = 0;
    sq_getstring(v, 2, &license);

    std::wstring licenseStr(license);
    if (licenseStr.size() == 0)
        return 0;

    std::vector<unsigned char> base64 = common::base64Decode(common::utf16ToUtf8(licenseStr.c_str()));
    if (base64.size() == 0)
        return 0;

    sq_newarray(v, 0);

    for (size_t i = 0; i < base64.size(); ++i) {
        unsigned char c = base64[i];
        sq_pushinteger(v, c);
        sq_arrayappend(v, -2);
    }
    return 1;
}

SQInteger nativeUnsignToInt(HSQUIRRELVM v)
{
    SQInteger i1 = 0;
    SQInteger i2 = 0;
    SQInteger i3 = 0;
    SQInteger i4 = 0;
    sq_getinteger(v, 2, &i1);
    sq_getinteger(v, 3, &i2);
    sq_getinteger(v, 4, &i3);
    sq_getinteger(v, 5, &i4);

    int result = 0;
    result = (unsigned char)i1;
    result += ((unsigned char)i2) << 8;
    result += ((unsigned char)i3) << 16;
    result += ((unsigned char)i4) << 24;

    sq_pushinteger(v, result);    
    return 1;
}

// static bool loadLicenseFromReg(std::vector<char>& buffer)
// {
//     HKEY resultKey = NULL;
//     if (::RegOpenKeyEx(
//         HKEY_LOCAL_MACHINE,
//         L"Software\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards",
//         0,
//         KEY_READ,
//         &resultKey) != ERROR_SUCCESS) {
//         return false;
//     }
// 
//     LONG result = RegQueryValueEx(resultKey, name, 0, REG_SZ,
//        reinterpret_cast<LPBYTE>(data), dsize);
// }

static bool loadLicenseAndVerifyImpl(HSQUIRRELVM v, const std::wstring& defaultPath)
{
    std::vector<char> buffer;
    
    if (defaultPath.empty()) {
        std::vector<wchar_t> path;
        path.resize(MAX_PATH + 1);
        memset(&path[0], 0, sizeof(wchar_t) * (MAX_PATH + 1));
        ::GetModuleFileNameW(g_hModule, &path[0], MAX_PATH);
        ::PathRemoveFileSpecW(&path[0]);
        ::PathAppendW(&path[0], L"license.key");

        common::readFile(&path[0], &buffer);
        OutputDebugStringW(L"loadLicenseAndVerify1:");
        OutputDebugStringW(&path[0]);
    } else {
        common::readFile(defaultPath.c_str(), &buffer);
        OutputDebugStringW(L"loadLicenseAndVerify2:");
        OutputDebugStringW(defaultPath.c_str());
    }
    OutputDebugStringW(L"\n");

    if (0 == buffer.size()) {
        OutputDebugStringA("loadLicenseAndVerifyImpl buffer is empty\n");
        return false;
    }

    std::string license(buffer.data(), buffer.size());
    bool b = SqBind::verifyLicenseImpl(v, license);
    return true; // weolar
}

bool SqBind::loadLicenseAndVerify(const std::wstring& defaultPath)
{
    return loadLicenseAndVerifyImpl(m_v, defaultPath);
}

SQInteger nativeLoadLicenseAndVerify(HSQUIRRELVM v)
{
    bool b = loadLicenseAndVerifyImpl(v, std::wstring());
    sq_pushbool(v, b ? SQTrue : SQFalse);
    return 1;
}

SQInteger nativeIsPrime(HSQUIRRELVM v)
{
    SQInteger i = 0;
    sq_getinteger(v, 2, &i);

    sq_pushbool(v, common::isPrime(i));
    return 1;
}

SQInteger nativeGenRsa(HSQUIRRELVM v)
{
    SQInteger a = 0;
    sq_getinteger(v, 2, &a);

    SQInteger b = 0;
    sq_getinteger(v, 3, &b);

    SQInteger c = 0;
    sq_getinteger(v, 4, &c);

    sq_pushinteger(v, common::genRsa(a, b, c));
    return 1;
}

static void printfunc(HSQUIRRELVM v, const SQChar *s, ...)
{
    va_list argptr;
    int cnt;
    va_start(argptr, s);
    wchar_t* buffer = (wchar_t*)malloc(0x2000);
    cnt = wvsprintf(buffer, s, argptr);
    va_end(argptr);
    OutputDebugStringW(buffer);
    OutputDebugStringW(L"\n");
    free(buffer);
}

static void errorfunc(HSQUIRRELVM v, const SQChar *s, ...)
{
    va_list argptr;
    int cnt;
    va_start(argptr, s);
    wchar_t* buffer = (wchar_t*)malloc(0x2000);
    cnt = wvsprintf(buffer, s, argptr);
    va_end(argptr);
    OutputDebugStringW(buffer);
    free(buffer);
}

SqBind* SqBind::m_inst = nullptr;

SqBind::SqBind()
{
    m_v = nullptr;
}

SqBind::~SqBind()
{
}

SqBind* SqBind::getInst()
{
    if (m_inst)
        return m_inst;
    m_inst = new SqBind();
    m_inst->initSqScript();
    return m_inst;
}

class SqFileReadBlob {
public:
    SqFileReadBlob(const unsigned char* data, size_t size)
    {
        m_pos = 0;
        m_size = size;
        m_data = data;
    }

    static SQInteger fileRead(SQUserPointer file, SQUserPointer buf, SQInteger size)
    {
        SqFileReadBlob* self = (SqFileReadBlob*)file;
        return self->fileReadImpl(buf, size);
    }

    SQInteger fileReadImpl(SQUserPointer buf, SQInteger size)
    {
        SQInteger ret = size;

        if (m_pos >= m_size)
            return -1;

        if (m_pos + size > m_size)
            ret = m_size - m_pos;

        memcpy(buf, m_data + m_pos, ret);
        m_pos += ret;

        return ret;
    }

private:
    size_t m_pos;
    size_t m_size;
    const unsigned char* m_data;
};

void SqBind::initSqScript()
{
    if (m_v)
        return;

    HSQUIRRELVM v;
    v = sq_open(1024);

    srand((unsigned int)::GetTickCount());

    sqstd_seterrorhandlers(v);
    sq_setprintfunc(v, printfunc, errorfunc);

    sq_pushroottable(v);

    sqstd_register_bloblib(v);
    sqstd_register_iolib(v);
    sqstd_register_systemlib(v);
    sqstd_register_mathlib(v);
    sqstd_register_stringlib(v);

    registerGlobalFunc(v, nativeGetVolumeID, _SC("nativeGetVolumeID"));
    registerGlobalFunc(v, nativeGetMacID, _SC("nativeGetMacID"));
    registerGlobalFunc(v, nativeGetDiskID, _SC("nativeGetDiskID"));
    registerGlobalFunc(v, nativeGetCpuID, _SC("nativeGetCpuID"));
    registerGlobalFunc(v, nativeAlert, _SC("nativeAlert"));
    registerGlobalFunc(v, nativeBase64ToBlob, _SC("nativeBase64ToBlob"));
    registerGlobalFunc(v, nativeUnsignToInt, _SC("nativeUnsignToInt"));
    registerGlobalFunc(v, nativeLoadLicenseAndVerify, _SC("nativeLoadLicenseAndVerify"));
    registerGlobalFunc(v, nativeIsPrime, _SC("nativeIsPrime"));
    registerGlobalFunc(v, nativeGenRsa, _SC("nativeGenRsa"));

    m_v = v;

    SQRESULT hr = 0;
#if 0
    const SQChar* sourceCode = _SC("E:\\mycode\\mbvip\\mtmb\\verify\\verify.js");
    hr = sqstd_loadfile(v, sourceCode, SQTrue);

    sqstd_writeclosuretofile(v, _SC("E:\\mycode\\mbvip\\mtmb\\verify\\verify.cnut"));
    sq_poptop(v);

    sqstd_dofile(v, _SC("E:\\mycode\\mbvip\\mtmb\\verify\\verify.cnut"), SQFalse, SQTrue);
#elif 0
    sqstd_dofile(v, _SC("E:\\mycode\\mbvip\\mtmb\\verify\\verify.js"), SQFalse, SQTrue);
#else
    SqFileReadBlob fileReadBlob(kVerifyJsCnut, sizeof(kVerifyJsCnut));
    hr = sq_readclosure(v, SqFileReadBlob::fileRead, &fileReadBlob);

    sq_push(v, -2);
    hr = sq_call(v, 1, SQFalse, SQTrue);
    sq_remove(v, -1);
#endif
}

void SqBind::uninit()
{
    sq_close(m_v);
}

std::string SqBind::createRequestCode(const char* regInfo)
{
    std::wstring regInfoW = common::utf8ToUtf16(regInfo);
    SQInteger top = sq_gettop(m_v);
    sq_pushroottable(m_v);
    sq_pushstring(m_v, _SC("onNativeCreateRequestCode"), -1);
    if (SQ_SUCCEEDED(sq_get(m_v, -2))) {
        sq_pushroottable(m_v);
        sq_pushstring(m_v, regInfoW.c_str(), regInfoW.size());
        sq_call(m_v, 2, SQTrue, SQTrue);
    }

    std::vector<int> buffer;

    SQObjectType type = sq_gettype(m_v, -1);
    if (OT_ARRAY != (type))
        return "";

    SQInteger nLen = sq_getsize(m_v, -1);
    if (0 == nLen)
        return "";

    for (int i = 0; i < nLen; ++i) {
        sq_pushinteger(m_v, i);
        sq_get(m_v, -2);

        SQInteger attrValInt = 0;

        type = sq_gettype(m_v, -1);
        if (type != OT_INTEGER)
            return "";
        sq_getinteger(m_v, -1, &attrValInt);

        buffer.push_back(attrValInt);

        sq_pop(m_v, 1);
    }
    sq_settop(m_v, top);

    const unsigned char* data = (const unsigned char *)buffer.data();
    std::string result = common::base64Encode(data, buffer.size() * sizeof(int));

    //////////////////////////////////////////////////////////////////////////
    //std::vector<unsigned char> xx = common::base64Decode(result);
    //////////////////////////////////////////////////////////////////////////
    return result;
}

// std::string SqBind::createLicense(const std::string& requestCode) // 在服务端运行
// {
//     std::vector<char> buffer;
//     buffer.resize(((int)(requestCode.size() / 4)) * 4);
// 
//     for (size_t i = 0; i < buffer.size(); ++i) {
//         if (i < requestCode.size())
//             buffer[i] = requestCode[i];
//         else
//             buffer[i] = '\0';
//     }
//     int* plaintextBuffer = (int*)buffer.data();
//     int plaintextBufferLen = buffer.size() / 4;
// 
//     std::vector<int> cipheBuffer;
//     
//     for (int i = 0; i < plaintextBufferLen; ++i) {
//         int intC = plaintextBuffer[i];
//         int cipheC = common::genRsa(intC, common::kLicense_publicKey, common::kLicense_N);
//         cipheBuffer.push_back(cipheC);
// 
//         //////////////////////////////////////////////////////////////////////////
//         int intC2 = common::genRsa(cipheC, common::kLicense_privateKey, common::kLicense_N);
//         intC2 = intC;
//         //////////////////////////////////////////////////////////////////////////
//     }
// 
//     const unsigned char* ptr = (const unsigned char *)cipheBuffer.data();
//     std::string result = common::base64Encode(ptr, cipheBuffer.size() * sizeof(int));
// 
//     //////////////////////////////////////////////////////////////////////////
//     std::vector<unsigned char> base64 = common::base64Decode(result);
//     std::vector<int> testBuffer;
//     for (size_t i = 0; i < cipheBuffer.size(); ++i) {
//         testBuffer.push_back(common::genRsa(cipheBuffer[i], common::kLicense_privateKey, common::kLicense_N));
//     }
//     const char* xx = (const char*)testBuffer.data();
//     //////////////////////////////////////////////////////////////////////////
// 
//     return result;
// }

std::string SqBind::createLicense(const std::string& requestCode) // 在服务端运行
{
    std::vector<int> cipheBuffer;

    for (size_t i = 0; i < requestCode.size(); ++i) {
        int intC = requestCode[i];
        int cipheC = common::genRsa(intC, common::kLicense_publicKey, common::kLicense_N);
        cipheBuffer.push_back(cipheC);

        //////////////////////////////////////////////////////////////////////////
        int intC2 = common::genRsa(cipheC, common::kLicense_privateKey, common::kLicense_N);
        if (intC2 != intC)
            DebugBreak();
        //////////////////////////////////////////////////////////////////////////
    }

    const unsigned char* ptr = (const unsigned char *)cipheBuffer.data();
    std::string result = common::base64Encode(ptr, cipheBuffer.size() * sizeof(int));

    //////////////////////////////////////////////////////////////////////////
//     for (int i = 0; i < 23256; ++i) {
//         int intC = i;
//         int cipheC = common::genRsa(intC, common::kLicense_publicKey, common::kLicense_N);
// 
//         int intC2 = common::genRsa(cipheC, common::kLicense_privateKey, common::kLicense_N);
//         if (intC2 != intC)
//             DebugBreak();
//     }

//     std::vector<unsigned char> base64 = common::base64Decode(result);
//     std::vector<char> testBuffer;
//     for (size_t i = 0; i < cipheBuffer.size(); ++i) {
//         int c = common::genRsa(cipheBuffer[i], common::kLicense_privateKey, common::kLicense_N);
//         testBuffer.push_back((char)c);
//     }
//     const char* xx = (const char*)testBuffer.data();
    //////////////////////////////////////////////////////////////////////////

    return result;
}

bool SqBind::verifyLicenseImpl(HSQUIRRELVM v, const std::string& license)
{
    std::wstring licenseW = common::utf8ToUtf16(license);
    SQInteger top = sq_gettop(v);
    sq_pushroottable(v);
    sq_pushstring(v, _SC("onNativeVerifyLicense"), -1);
    if (!(SQ_SUCCEEDED(sq_get(v, -2))))
        return false;

    sq_pushroottable(v);
    sq_pushstring(v, licenseW.c_str(), licenseW.size());
    sq_call(v, 2, SQTrue, SQTrue);

    SQBool b = false;
    sq_getbool(v, -1, &b);

    return !!b;
}

bool SqBind::verifyLicense(const std::string& license)
{
    return verifyLicenseImpl(m_v, license);
}

void SqBind::timerFire()
{
    HSQUIRRELVM v = m_v;
    SQInteger top = sq_gettop(v); 
    sq_pushroottable(v);
    sq_pushstring(v, _SC("onNativeTimerFire"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) {
        sq_pushroottable(v);
        sq_call(v, 1, SQFalse, SQTrue);
    }
    sq_settop(v, top);
}