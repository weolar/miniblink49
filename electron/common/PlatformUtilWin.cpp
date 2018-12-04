// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/PlatformUtil.h"

#include <windows.h>
#include <commdlg.h>
#include <comdef.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shlobj.h>

#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/win/scoped_co_mem.h"
#include "base/win/scoped_com_initializer.h"
#include "base/win/scoped_comptr.h"
#include "base/win/windows_version.h"
#include "base/win/shell.h"

#include "v8.h"
#include "gin/arguments.h"

namespace {

// Old ShellExecute crashes the process when the command for a given scheme
// is empty. This function tells if it is.
bool ValidateShellCommandForScheme(const std::string& scheme) {
//     base::win::RegKey key;
//     base::string16 registry_path = base::ASCIIToWide(scheme) + L"\\shell\\open\\command";
//     key.Open(HKEY_CLASSES_ROOT, registry_path.c_str(), KEY_READ);
//     if (!key.Valid())
//         return false;
//     DWORD size = 0;
//     key.ReadValue(NULL, NULL, &size, NULL);
//     if (size <= 2)
//         return false;
    DebugBreak();
    return true;
}


// Required COM implementation of IFileOperationProgressSink so we can
// precheck files before deletion to make sure they can be move to the
// Recycle Bin.
class DeleteFileProgressSink : public IFileOperationProgressSink {
public:
    DeleteFileProgressSink();

private:
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppvObj);
    HRESULT STDMETHODCALLTYPE StartOperations(void);
    HRESULT STDMETHODCALLTYPE FinishOperations(HRESULT);
    HRESULT STDMETHODCALLTYPE PreRenameItem(
        DWORD, IShellItem*, LPCWSTR);
    HRESULT STDMETHODCALLTYPE PostRenameItem(
        DWORD, IShellItem*, LPCWSTR, HRESULT, IShellItem*);
    HRESULT STDMETHODCALLTYPE PreMoveItem(
        DWORD, IShellItem*, IShellItem*, LPCWSTR);
    HRESULT STDMETHODCALLTYPE PostMoveItem(
        DWORD, IShellItem*, IShellItem*, LPCWSTR, HRESULT, IShellItem*);
    HRESULT STDMETHODCALLTYPE PreCopyItem(
        DWORD, IShellItem*, IShellItem*, LPCWSTR);
    HRESULT STDMETHODCALLTYPE PostCopyItem(
        DWORD, IShellItem*, IShellItem*, LPCWSTR, HRESULT, IShellItem*);
    HRESULT STDMETHODCALLTYPE PreDeleteItem(DWORD, IShellItem*);
    HRESULT STDMETHODCALLTYPE PostDeleteItem(
        DWORD, IShellItem*, HRESULT, IShellItem*);
    HRESULT STDMETHODCALLTYPE PreNewItem(
        DWORD, IShellItem*, LPCWSTR);
    HRESULT STDMETHODCALLTYPE PostNewItem(
        DWORD, IShellItem*, LPCWSTR, LPCWSTR, DWORD, HRESULT, IShellItem*);
    HRESULT STDMETHODCALLTYPE UpdateProgress(UINT, UINT);
    HRESULT STDMETHODCALLTYPE ResetTimer(void);
    HRESULT STDMETHODCALLTYPE PauseTimer(void);
    HRESULT STDMETHODCALLTYPE ResumeTimer(void);

    ULONG m_cRef;
};

DeleteFileProgressSink::DeleteFileProgressSink() {
    m_cRef = 0;
}

HRESULT DeleteFileProgressSink::PreDeleteItem(DWORD dwFlags, IShellItem*) {
    if (!(dwFlags & TSF_DELETE_RECYCLE_IF_POSSIBLE)) {
        // TSF_DELETE_RECYCLE_IF_POSSIBLE will not be set for items that cannot be
        // recycled.  In this case, we abort the delete operation.  This bubbles
        // up and stops the Delete in IFileOperation.
        return E_ABORT;
    }
    // Returns S_OK if successful, or an error value otherwise. In the case of an
    // error value, the delete operation and all subsequent operations pending
    // from the call to IFileOperation are canceled.
    return S_OK;
}

HRESULT DeleteFileProgressSink::QueryInterface(REFIID riid, LPVOID* ppvObj) {
    // Always set out parameter to NULL, validating it first.
    if (!ppvObj)
        return E_INVALIDARG;
    *ppvObj = nullptr;
    DebugBreak();

//     if (riid == IID_IUnknown || riid == IID_IFileOperationProgressSink) {
//         // Increment the reference count and return the pointer.
//         *ppvObj = reinterpret_cast<IUnknown*>(this);
//         AddRef();
//         return NOERROR;
//     }
    return E_NOINTERFACE;
}

ULONG DeleteFileProgressSink::AddRef() {
    //InterlockedIncrement(&m_cRef);
    DebugBreak();
    return m_cRef;
}

ULONG DeleteFileProgressSink::Release() {
    // Decrement the object's internal counter.
//     ULONG ulRefCount = InterlockedDecrement(&m_cRef);
//     if (0 == m_cRef) {
//         delete this;
//     }
//    return ulRefCount;
    DebugBreak();
    return 0;
}

HRESULT DeleteFileProgressSink::StartOperations() {
    return S_OK;
}

HRESULT DeleteFileProgressSink::FinishOperations(HRESULT) {
    return S_OK;
}

HRESULT DeleteFileProgressSink::PreRenameItem(DWORD, IShellItem*, LPCWSTR) {
    return S_OK;
}

HRESULT DeleteFileProgressSink::PostRenameItem(
    DWORD, IShellItem*, LPCWSTR, HRESULT, IShellItem*) {
    return E_NOTIMPL;
}

HRESULT DeleteFileProgressSink::PreMoveItem(
    DWORD, IShellItem*, IShellItem*, LPCWSTR) {
    return E_NOTIMPL;
}

HRESULT DeleteFileProgressSink::PostMoveItem(
    DWORD, IShellItem*, IShellItem*, LPCWSTR, HRESULT, IShellItem*) {
    return E_NOTIMPL;
}

HRESULT DeleteFileProgressSink::PreCopyItem(
    DWORD, IShellItem*, IShellItem*, LPCWSTR) {
    return E_NOTIMPL;
}

HRESULT DeleteFileProgressSink::PostCopyItem(
    DWORD, IShellItem*, IShellItem*, LPCWSTR, HRESULT, IShellItem*) {
    return E_NOTIMPL;
}

HRESULT DeleteFileProgressSink::PostDeleteItem(
    DWORD, IShellItem*, HRESULT, IShellItem*) {
    return S_OK;
}

HRESULT DeleteFileProgressSink::PreNewItem(
    DWORD dwFlags, IShellItem*, LPCWSTR) {
    return E_NOTIMPL;
}

HRESULT DeleteFileProgressSink::PostNewItem(
    DWORD, IShellItem*, LPCWSTR, LPCWSTR, DWORD, HRESULT, IShellItem*) {
    return E_NOTIMPL;
}

HRESULT DeleteFileProgressSink::UpdateProgress(UINT, UINT) {
    return S_OK;
}

HRESULT DeleteFileProgressSink::ResetTimer() {
    return S_OK;
}

HRESULT DeleteFileProgressSink::PauseTimer() {
    return S_OK;
}

HRESULT DeleteFileProgressSink::ResumeTimer() {
    return S_OK;
}

}  // namespace

namespace platform_util {

void showItemInFolder(const base::FilePath& full_path) {
    base::win::ScopedCOMInitializer com_initializer;
    if (!com_initializer.succeeded())
        return;

    base::FilePath dir = full_path.DirName().AsEndingWithSeparator();
    // ParseDisplayName will fail if the directory is "C:", it must be "C:\\".
    if (dir.empty())
        return;

    typedef HRESULT(WINAPI *SHOpenFolderAndSelectItemsFuncPtr)(
        ITEMIDLIST* pidl_Folder,
        UINT cidl,
        const ITEMIDLIST** pidls,
        DWORD flags);

    static SHOpenFolderAndSelectItemsFuncPtr open_folder_and_select_itemsPtr = NULL;
    static bool initialize_open_folder_proc = true;
    if (initialize_open_folder_proc) {
        initialize_open_folder_proc = false;
        // The SHOpenFolderAndSelectItems API is exposed by shell32 version 6
        // and does not exist in Win2K. We attempt to retrieve this function export
        // from shell32 and if it does not exist, we just invoke ShellExecute to
        // open the folder thus losing the functionality to select the item in
        // the process.
        HMODULE shell32_base = GetModuleHandle(L"shell32.dll");
        if (!shell32_base) {
            //NOTREACHED() << " " << __FUNCTION__ << "(): Can't open shell32.dll";
            return;
        }
        open_folder_and_select_itemsPtr = reinterpret_cast<SHOpenFolderAndSelectItemsFuncPtr>(GetProcAddress(shell32_base, "SHOpenFolderAndSelectItems"));
    }
    if (!open_folder_and_select_itemsPtr) {
        ui::win::OpenFolderViaShell(dir);
        return;
    }

    base::win::ScopedComPtr<IShellFolder> desktop;
    HRESULT hr = SHGetDesktopFolder(desktop.Receive());
    if (FAILED(hr))
        return;

    base::win::ScopedCoMem<ITEMIDLIST> dir_item;
    hr = desktop->ParseDisplayName(NULL, NULL, const_cast<wchar_t *>(dir.value().c_str()),
        NULL, &dir_item, NULL);
    if (FAILED(hr)) {
        ui::win::OpenFolderViaShell(dir);
        return;
    }

    base::win::ScopedCoMem<ITEMIDLIST> file_item;
    hr = desktop->ParseDisplayName(NULL, NULL, const_cast<wchar_t *>(full_path.value().c_str()),
        NULL, &file_item, NULL);
    if (FAILED(hr)) {
        ui::win::OpenFolderViaShell(dir);
        return;
    }

    const ITEMIDLIST* highlight[] = { file_item };

    hr = (*open_folder_and_select_itemsPtr)(dir_item, arraysize(highlight),
        highlight, NULL);

    if (FAILED(hr)) {
        // On some systems, the above call mysteriously fails with "file not
        // found" even though the file is there.  In these cases, ShellExecute()
        // seems to work as a fallback (although it won't select the file).
        if (hr == ERROR_FILE_NOT_FOUND) {
            ui::win::OpenFolderViaShell(dir);
        }
        else {
            LPTSTR message = NULL;
            DWORD message_length = FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                0, hr, 0, reinterpret_cast<LPTSTR>(&message), 0, NULL);
//            LOG(WARNING) << " " << __FUNCTION__
//                << "(): Can't open full_path = \""
//                 << full_path.value() << "\""
//                 << " hr = " << hr
//                 << " " << reinterpret_cast<LPTSTR>(&message);
            if (message)
                LocalFree(message);

            ui::win::OpenFolderViaShell(dir);
        }
    }
}

void openItem(const base::FilePath& full_path) {
    if (::PathIsDirectoryW(full_path.value().c_str()))
        ui::win::OpenFolderViaShell(full_path);
    else
        ui::win::OpenFileViaShell(full_path);
}

class OpenExternal {
public:
    OpenExternal(const base::string16& url, bool activate)
        : m_url(url)
        , m_activate(activate)
        , m_wait(0) {
    }

    bool call() {
        bool retVal = false;
        m_retVal = &retVal;
        ::CreateThread(nullptr, 0, &OpenExternal::threadEnter, this, 0, 0);
        while (0 == m_wait) { ::Sleep(10); }
        return retVal;
    }

    static DWORD WINAPI threadEnter(LPVOID lpThreadParameter) {
        OpenExternal* self = (OpenExternal*)lpThreadParameter;
        *(self->m_retVal) = self->openExternalImpl(self->m_url, self->m_activate);
        self->m_wait = 1;
        return 0;
    }

    bool openExternalImpl(const base::string16& url, bool activate) {
        // Quote the input scheme to be sure that the command does not have
        // parameters unexpected by the external program. This url should already
        // have been escaped.
        base::string16 escaped_url = L"\"" + url + L"\"";

        if (reinterpret_cast<ULONG_PTR>(ShellExecuteW(NULL, L"open", escaped_url.c_str(), NULL, NULL, SW_SHOWNORMAL)) <= 32) {
            // We fail to execute the call. We could display a message to the user.
            // TODO(nsylvain): we should also add a dialog to warn on errors. See
            // bug 1136923.
            return false;
        }
        return true;
    }

private:
    base::string16 m_url;
    bool m_activate;
    bool* m_retVal;
    int m_wait;
};

bool openExternal(const base::string16& url, bool activate) {
    OpenExternal threader(url, activate);
    return threader.call();
}

#define FOFX_ADDUNDORECORD 0x20000000 
#define FOFX_RECYCLEONDELETE 0x00080000 

bool moveItemToTrash(const base::FilePath& path) {
    base::win::ScopedCOMInitializer com_initializer;
    if (!com_initializer.succeeded())
        return false;

    base::win::ScopedComPtr<IFileOperation> pfo;
//     if (FAILED(pfo.CreateInstance(CLSID_FileOperation)))
//         return false;
    DebugBreak();

    // Elevation prompt enabled for UAC protected files.  This overrides the
    // SILENT, NO_UI and NOERRORUI flags.

    if (base::win::GetVersion() >= base::win::VERSION_WIN8) {
        // Windows 8 introduces the flag RECYCLEONDELETE and deprecates the
        // ALLOWUNDO in favor of ADDUNDORECORD.
        if (FAILED(pfo->SetOperationFlags(FOF_NO_UI |
            FOFX_ADDUNDORECORD |
            FOF_NOERRORUI |
            FOF_SILENT |
            FOFX_SHOWELEVATIONPROMPT |
            FOFX_RECYCLEONDELETE)))
            return false;
    }
    else {
        // For Windows 7 and Vista, RecycleOnDelete is the default behavior.
        if (FAILED(pfo->SetOperationFlags(FOF_NO_UI |
            FOF_ALLOWUNDO |
            FOF_NOERRORUI |
            FOF_SILENT |
            FOFX_SHOWELEVATIONPROMPT)))
            return false;
    }

    // Create an IShellItem from the supplied source path.
    base::win::ScopedComPtr<IShellItem> delete_item;
//     if (FAILED(SHCreateItemFromParsingName(path.value().c_str(), NULL, IID_PPV_ARGS(delete_item.Receive()))))
//         return false;

    // https://stackoverflow.com/questions/20885556/replacement-for-shcreateitemfromparsingname-on-windows-xp
    // http://blog.csdn.net/infoworld/article/details/54574953
    IFileDialog *pfd = nullptr;
    LPITEMIDLIST pidl = nullptr;
    HRESULT hresult = ::SHParseDisplayName(path.value().c_str(), 0, &pidl, SFGAO_FOLDER, 0);
    if (SUCCEEDED(hresult)) {
        IShellFolder* psf = nullptr;
        hresult = ::SHGetDesktopFolder(&psf);
        if (SUCCEEDED(hresult)) {
            hresult = psf->BindToObject(pidl, 0, IID_IShellFolder, (void**)delete_item.Receive());
            if (SUCCEEDED(hresult)) {
                pfd->SetFolder(delete_item.get());
            }
        }
    }

    base::win::ScopedComPtr<IFileOperationProgressSink> delete_sink(new DeleteFileProgressSink);
    if (!delete_sink)
        return false;

    // Processes the queued command DeleteItem. This will trigger
    // the DeleteFileProgressSink to check for Recycle Bin.
    bool b = SUCCEEDED(pfo->DeleteItem(delete_item.get(), delete_sink.get())) && SUCCEEDED(pfo->PerformOperations());
    return b;
}

void beep() {
    MessageBeep(MB_OK);
}

}  // namespace platform_util
