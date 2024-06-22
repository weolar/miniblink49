// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/web_impl_win/npapi/WebPluginImeWin.h"
#include "content/web_impl_win/npapi/WebPluginUtil.h"

#include "wtf/text/WTFStringUtil.h"

#include <cstring>
#include <string>
#include <vector>
#include <algorithm>

#pragma comment(lib, "imm32.lib")

namespace content {

// A critical section that prevents two or more plugins from accessing a
// WebPluginIMEWin instance through our patch function.
CRITICAL_SECTION* g_webPluginImeLock = nullptr;

WebPluginIMEWin* WebPluginIMEWin::m_instance = NULL;

WebPluginIMEWin::WebPluginIMEWin()
    : m_cursorPosition(0)
    , m_deltaStart(0)
    , m_composingText(false)
    , m_supportImeMessages(false)
    , m_statusUpdated(false)
    , m_inputType(1)
{
    memset(m_resultClauses, 0, sizeof(m_resultClauses));
}

WebPluginIMEWin::~WebPluginIMEWin()
{
}

void WebPluginIMEWin::compositionUpdated(const String& text, std::vector<int> clauses, std::vector<int> target, int cursorPosition)
{
    // Send a WM_IME_STARTCOMPOSITION message when a user starts a composition.
    NPEvent np_event;
    if (!m_composingText) {
        m_composingText = true;
        m_resultText = L"";

        np_event.event = WM_IME_STARTCOMPOSITION;
        np_event.wParam = 0;
        np_event.lParam = 0;
        m_events.push_back(np_event);
    }

    // We can update the following values from this event: GCS_COMPSTR,
    // GCS_COMPATTR, GCSCOMPCLAUSE, GCS_CURSORPOS, and GCS_DELTASTART. We send a
    // WM_IME_COMPOSITION message to notify the list of updated values.
    np_event.event = WM_IME_COMPOSITION;
    np_event.wParam = 0;
    np_event.lParam = GCS_COMPSTR | GCS_COMPATTR | GCS_COMPCLAUSE | GCS_CURSORPOS | GCS_DELTASTART;
    m_events.push_back(np_event);

    // Converts this event to the IMM32 data so we do not have to convert it every
    // time when a plugin call an IMM32 function.
    m_compositionText = text;

    // Create the composition clauses returned when a plugin calls
    // ImmGetCompositionString() with GCS_COMPCLAUSE.
    m_compositionClauses.clear();
    for (size_t i = 0; i < clauses.size(); ++i)
        m_compositionClauses.push_back(clauses[i]);

    // Create the composition attributes used by GCS_COMPATTR.
    if (target.size() == 2) {
        m_compositionAttributes.assign(text.length(), ATTR_CONVERTED);
        for (int i = target[0]; i < target[1]; ++i)
            m_compositionAttributes[i] = ATTR_TARGET_CONVERTED;
    } else {
        m_compositionAttributes.assign(text.length(), ATTR_INPUT);
    }

    m_cursorPosition = cursorPosition;
    m_deltaStart = cursorPosition;
}

void WebPluginIMEWin::compositionCompleted(const String& text)
{
    m_composingText = false;

    // We should update the following values when we finish a composition:
    // GCS_RESULTSTR, GCS_RESULTCLAUSE, GCS_CURSORPOS, and GCS_DELTASTART. We
    // send a WM_IME_COMPOSITION message to notify the list of updated values.
    NPEvent np_event;
    np_event.event = WM_IME_COMPOSITION;
    np_event.wParam = 0;
    np_event.lParam = GCS_CURSORPOS | GCS_DELTASTART | GCS_RESULTSTR |
        GCS_RESULTCLAUSE;
    m_events.push_back(np_event);

    // We also send a WM_IME_ENDCOMPOSITION message after the final
    // WM_IME_COMPOSITION message (i.e. after finishing a composition).
    np_event.event = WM_IME_ENDCOMPOSITION;
    np_event.wParam = 0;
    np_event.lParam = 0;
    m_events.push_back(np_event);

    // If the target plugin does not seem to support IME messages, we send
    // each character in IME text with a WM_CHAR message so the plugin can
    // insert the IME text.
    if (!m_supportImeMessages) {
        np_event.event = WM_CHAR;
        np_event.wParam = 0;
        np_event.lParam = 0;
        for (size_t i = 0; i < m_resultText.length(); ++i) {
            np_event.wParam = m_resultText[i];
            m_events.push_back(np_event);
        }
    }

    // Updated the result text and its clause. (Unlike composition clauses, a
    // result clause consists of only one region.)
    m_resultText = text;

    m_resultClauses[0] = 0;
    m_resultClauses[1] = m_resultText.length();

    m_cursorPosition = m_resultClauses[1];
    m_deltaStart = m_resultClauses[1];
}

void WebPluginIMEWin::sendEvents(int id, NPP instance, NPP_HandleEventProcPtr eventPtr)
{
    if (!eventPtr)
        return;

    // We allow the patch functions to access this WebPluginIMEWin instance only
    // while we send IME events to the plugin.
    std::vector<NPEvent>* events = new std::vector<NPEvent>();
    {
        ScopedLock lock(this);
        
        for (std::vector<NPEvent>::iterator it = m_events.begin(); it != m_events.end(); ++it) {
            NPEvent evt = *it;
            events->push_back(evt);
        }
        m_events.clear();
    }

    WebPluginUtil::get()->postTaskToNpThread(FROM_HERE, [id, events, instance, eventPtr] {
        if (WebPluginUtil::isLive(id)) {
            for (std::vector<NPEvent>::iterator it = events->begin(); it != events->end(); ++it) {
                eventPtr(instance, &(*it));
            }
        }
        delete events;
    });    
}

bool WebPluginIMEWin::getStatus(int* inputType, blink::IntRect* caretRect)
{
    *inputType = m_inputType;
    *caretRect = m_caretRect;
    return true;
}

static DWORD WINAPI immGetProperty(HKL hkl, DWORD flag)
{
    return 0xd000a;
}

static BOOL WINAPI immIsIME(HKL hkl)
{
    return FALSE;
}

static BOOL WINAPI immNotifyIME(HIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue)
{
    return FALSE;
}

static BOOL WINAPI immGetCompositionFontW(IN HIMC, _Out_ LPLOGFONTW lplf)
{
    return FALSE;
}

static BOOL WINAPI immGetOpenStatus(IN HIMC)
{
    return FALSE;
}

static BOOL WINAPI immGetConversionStatus(IN HIMC, _Out_opt_ LPDWORD lpfdwConversion, _Out_opt_ LPDWORD lpfdwSentence)
{
    return FALSE;
}

// static
FARPROC WebPluginIMEWin::getProcAddress(LPCSTR name)
{
    static const struct {
        const char* name;
        FARPROC function;
    } kImm32Functions[] = {
        { "ImmAssociateContextEx", reinterpret_cast<FARPROC>(immAssociateContextEx) },
        { "ImmGetCompositionStringW", reinterpret_cast<FARPROC>(immGetCompositionStringW) },
        { "ImmGetContext", reinterpret_cast<FARPROC>(immGetContext) },
        { "ImmReleaseContext", reinterpret_cast<FARPROC>(immReleaseContext) },
        { "ImmSetCandidateWindow", reinterpret_cast<FARPROC>(immSetCandidateWindow) },
        { "ImmSetOpenStatus", reinterpret_cast<FARPROC>(immSetOpenStatus) },
        { "ImmIsIME", reinterpret_cast<FARPROC>(immIsIME) },
        { "ImmGetProperty", reinterpret_cast<FARPROC>(immGetProperty) },
//         { "ImmGetCompositionFontW", reinterpret_cast<FARPROC>(immGetCompositionFontW) },
//         { "ImmGetOpenStatus", reinterpret_cast<FARPROC>(immGetOpenStatus) },
//         { "ImmGetConversionStatus", reinterpret_cast<FARPROC>(immGetConversionStatus) },
        
    };
    for (int i = 0; i < arraysize(kImm32Functions); ++i) {
        if (!lstrcmpiA(name, kImm32Functions[i].name))
            return kImm32Functions[i].function;
    }

    return NULL;
}

void WebPluginIMEWin::lock()
{
    if (!g_webPluginImeLock) {
        g_webPluginImeLock = new CRITICAL_SECTION();
        ::InitializeCriticalSection(g_webPluginImeLock);
    }
    ::EnterCriticalSection(g_webPluginImeLock);
    m_instance = this;
}

void WebPluginIMEWin::unlock()
{
    m_instance = NULL;
    ::LeaveCriticalSection(g_webPluginImeLock);
}

// static
WebPluginIMEWin* WebPluginIMEWin::getInstance(HIMC context)
{
    return m_instance && context == reinterpret_cast<HIMC>(m_instance) ? m_instance : NULL;
}

// static
BOOL WINAPI WebPluginIMEWin::immAssociateContextEx(HWND window, HIMC context, DWORD flags)
{
    //OutputDebugStringA("immAssociateContextEx\n");

    WebPluginIMEWin* instance = getInstance(context);
    if (!instance)
        return ::ImmAssociateContextEx(window, context, flags);

    int inputType = !context && !flags;
    instance->m_inputType = inputType;
    instance->m_statusUpdated = true;
    return TRUE;
}

// static
LONG WINAPI WebPluginIMEWin::immGetCompositionStringW(HIMC context, DWORD index, LPVOID dstData, DWORD dstSize)
{
    WebPluginIMEWin* instance = getInstance(context);
    if (!instance)
        return ::ImmGetCompositionStringW(context, index, dstData, dstSize);

    const void* srcData = NULL;
    DWORD srcSize = 0;
    switch (index) {
    case GCS_COMPSTR:
        srcData = instance->m_compositionText.characters16();
        srcSize = instance->m_compositionText.length() * sizeof(wchar_t);
        break;

    case GCS_COMPATTR:
//         srcData = instance->m_compositionAttributes.c_str();
//         srcSize = instance->m_compositionAttributes.length();
        return 0;
        break;

    case GCS_COMPCLAUSE:
//         if (instance->m_compositionClauses.size() > 0)
//             srcData = &instance->m_compositionClauses[0];
//         srcSize = instance->m_compositionClauses.size() * sizeof(uint32);
        return 0;
        break;

    case GCS_CURSORPOS:
        return instance->m_cursorPosition;

    case GCS_DELTASTART:
        return instance->m_deltaStart;

    case GCS_RESULTSTR:
        srcData = instance->m_resultText.characters16();
        srcSize = instance->m_resultText.length() * sizeof(wchar_t);
        break;

    case GCS_RESULTCLAUSE:
        srcData = &instance->m_resultClauses[0];
        srcSize = sizeof(instance->m_resultClauses);
        break;

    default:
        break;
    }
    if (!srcData || !srcSize)
        return IMM_ERROR_NODATA;

    if (dstSize >= srcSize)
        memcpy(dstData, srcData, srcSize);

    return srcSize;
}

// static
HIMC WINAPI WebPluginIMEWin::immGetContext(HWND window)
{
    // Call the original immGetContext() function if the given window is the one
    // created in WebPluginDelegateImpl::WindowedCreatePlugin(). (We attached IME
    // context only with the windows created in this function.) On the other hand,
    // some windowless plugins (such as Flash) call this function with a dummy
    // window handle. We return our dummy IME context for these plugins so they
    // can use our IME emulator.
    const wchar_t kNativeWindowClassName[] = L"NativeWindowClass";
    if (IsWindow(window)) {
        wchar_t name[128];
        GetClassName(window, &name[0], arraysize(name));
        if (!wcscmp(&name[0], kNativeWindowClassName))
            return ::ImmGetContext(window);
    }

    WebPluginIMEWin* instance = m_instance;
    if (instance)
        instance->m_supportImeMessages = true;
    return reinterpret_cast<HIMC>(instance);
}

// static
BOOL WINAPI WebPluginIMEWin::immReleaseContext(HWND window, HIMC context)
{
    if (!getInstance(context))
        return ::ImmReleaseContext(window, context);
    return TRUE;
}

// static
BOOL WINAPI WebPluginIMEWin::immSetCandidateWindow(HIMC context, CANDIDATEFORM* candidate)
{
    WebPluginIMEWin* instance = getInstance(context);
    if (!instance)
        return ::ImmSetCandidateWindow(context, candidate);

    blink::IntRect caret_rect(candidate->rcArea.left, candidate->rcArea.top, candidate->rcArea.right - candidate->rcArea.left, candidate->rcArea.bottom - candidate->rcArea.top);
    if (/*(candidate->dwStyle & CFS_EXCLUDE) &&*/ instance->m_caretRect != caret_rect) {
        instance->m_caretRect = caret_rect;
        instance->m_statusUpdated = true;
    }
    return TRUE;
}

// static
BOOL WINAPI WebPluginIMEWin::immSetOpenStatus(HIMC context, BOOL open)
{
    WebPluginIMEWin* instance = getInstance(context);
    if (!instance)
        return ::ImmSetOpenStatus(context, open);

    int inputType = open ? 1 : 0;
    if (instance->m_inputType != inputType) {
        instance->m_inputType = inputType;
        instance->m_statusUpdated = true;
    }

    return TRUE;
}

#ifndef _M_X64

static bool importAddressTableHook(HMODULE hModule, LPCSTR pImageName, LPCVOID pTargetFuncAddr, LPCVOID pReplaceFuncAddr)
{
    IMAGE_DOS_HEADER* imgDosHdr = (IMAGE_DOS_HEADER*)hModule;
    IMAGE_OPTIONAL_HEADER* imgOptHdr = (IMAGE_OPTIONAL_HEADER*)((DWORD)hModule + imgDosHdr->e_lfanew + 24);
    IMAGE_IMPORT_DESCRIPTOR* imgImportDes = (IMAGE_IMPORT_DESCRIPTOR*)((DWORD)hModule + imgOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    IMAGE_THUNK_DATA* imgThunkData = NULL;

    std::string targetLibraryName;
    DWORD oldProtect = 0;
    LPDWORD funcAddress = NULL;

    while (imgImportDes->Characteristics != 0) {
        LPCSTR lpszName = ((LPCSTR)(DWORD)hModule + imgImportDes->Name);
        targetLibraryName = lpszName;
        std::transform(targetLibraryName.begin(), targetLibraryName.end(), targetLibraryName.begin(), tolower);

        if (targetLibraryName.compare(pImageName) == 0) {
            imgThunkData = (IMAGE_THUNK_DATA*)((DWORD)hModule + imgImportDes->FirstThunk);
            break;
        }
        imgImportDes++;
    }

    if (!imgThunkData)
        return false;
    
    while (imgThunkData->u1.Function) {
        funcAddress = (LPDWORD)& (imgThunkData->u1.Function);
        if (*funcAddress == (DWORD)pTargetFuncAddr) {
            ::VirtualProtect(funcAddress, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
            if (!::WriteProcessMemory((HANDLE)-1, funcAddress, &pReplaceFuncAddr, 4, NULL)) {
                return false;
            }
            ::VirtualProtect(funcAddress, sizeof(DWORD), oldProtect, 0);
            return true;
        }
        imgThunkData++;
    }
    return false;
}

#endif

bool WebPluginIMEWin::hookGetProcAddress(HMODULE module)
{
#ifndef _M_X64
    LPDWORD targetFuncAddr = NULL;
    HMODULE lib = ::GetModuleHandleA("kernel32.dll");
    if (NULL != lib) {
        targetFuncAddr = (LPDWORD)::GetProcAddress(lib, "GetProcAddress");
        return importAddressTableHook(module, "kernel32.dll", targetFuncAddr, WebPluginIMEWin::GetProcAddressPatch);
    }
#endif
    return false;
}

FARPROC WINAPI WebPluginIMEWin::GetProcAddressPatch(HMODULE module, LPCSTR name)
{
    FARPROC immFunction = WebPluginIMEWin::getProcAddress(name);
    if (immFunction)
        return immFunction;
    return ::GetProcAddress(module, name);
}

}  // namespace content
