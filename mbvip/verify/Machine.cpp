
#include "verify/Machine.h"

#include <windows.h>
#include <tlhelp32.h>
#include <intrin.h>
#include <set>
#include <string>

#define PROTECT_Ansi(n) n

static void fillHex(
    __in const void *data,
    __in size_t dataLength,
    __out char *buffer,
    __in size_t inLength,
    __out size_t& outLength)
{
    if (inLength >= outLength + dataLength * 2) {
        const unsigned char *ptr = (const unsigned char *)data;
        char *out = &buffer[outLength];
        outLength += dataLength * 2;
        while (dataLength--) {
            unsigned __int8 v = *ptr >> 4;
            *out++ = v < 10 ? v + '0' : v - 10 + 'a';
            v = *ptr & 0xF;
            *out++ = v < 10 ? v + '0' : v - 10 + 'a';
            ++ptr;
        }
    }
}

static void fillDword(
    __in unsigned __int32 val,
    __out char *buffer,
    __in size_t inLength,
    __out size_t& outLength)
{
    if (inLength >= outLength + 8) {
        for (size_t i = 0; i < 8; ++i) {
            unsigned __int8 v = val & 0xF;
            buffer[outLength + 7 - i] = v < 10 ? v + '0' : v - 10 + 'a';
            val >>= 4;
        }
        outLength += 8;
    }
}

static size_t __strlen(const char *str)
{
    const char *p = str;
    while (*p++ != '\0');
    return p - str - 1;
}

static int __strcmp(const char *str1, const char *str2)
{
    for (; *str1 == *str2; ++str1, ++str2) {
        if (*str1 == '\0')
            return 0;
    }
    return (*(unsigned char *)str1 < *(unsigned char *)str2) ? -1 : 1;
}

static void *__memcpy(void *dst, const void *src, size_t len)
{
    unsigned char *p1 = (unsigned char *)dst;
    const unsigned char *p2 = (const unsigned char *)src;
    while (len--)
        *p1++ = *p2++;
    return dst;
}

typedef enum _CPUID_REGISTER {
    CPUID_EAX = 0,
    CPUID_EBX,
    CPUID_ECX,
    CPUID_EDX,
    CPUID_MAX_REGISTER
} CPUID_REGISTER;

static void recordCPUID(
    __in unsigned __int32 eax,
    __in unsigned __int32 ecx,
    __in int data[CPUID_MAX_REGISTER],
    __out char *buffer,
    __in size_t inLength,
    __out size_t& outLength)
{
    if (inLength >= outLength + 53) {
        fillDword(eax, buffer, inLength, outLength);
        fillDword(ecx, buffer, inLength, outLength);
        buffer[outLength++] = ' ';
        fillDword(data[CPUID_EAX], buffer, inLength, outLength);
        buffer[outLength++] = ' ';
        fillDword(data[CPUID_EBX], buffer, inLength, outLength);
        buffer[outLength++] = ' ';
        fillDword(data[CPUID_ECX], buffer, inLength, outLength);
        buffer[outLength++] = ' ';
        fillDword(data[CPUID_EDX], buffer, inLength, outLength);
        buffer[outLength++] = '\n';
    }
}

static bool getCPUID(char *buffer, size_t inLength, bool bAbsoluteStable, size_t& outLength)
{
    // Check CPUID.
    ULONG_PTR eflag1, eflag2;
    eflag1 = __readeflags();
    eflag1 ^= 0x00200000;
    __writeeflags(eflag1);
    eflag2 = __readeflags();
    if ((eflag1 & 0x00200000) != (eflag2 & 0x00200000))
        return false;

    // Now read.
    int data[CPUID_MAX_REGISTER];
    // Standard features.
    __cpuidex(data, 0, 0);
    unsigned __int32 maxIndex = data[0];
    if (inLength >= outLength + 9) {
        fillDword(maxIndex, buffer, inLength, outLength);
        buffer[outLength++] = '\n';
    }

    // Vendor.
    char vendor[13];
    *(int *)&vendor[0] = data[CPUID_EBX];
    *(int *)&vendor[4] = data[CPUID_EDX];
    *(int *)&vendor[8] = data[CPUID_ECX];
    vendor[12] = 0;
    size_t vendorLen = __strlen(vendor);
    if (inLength >= outLength + vendorLen + 1) {
        __memcpy(&buffer[outLength], vendor, vendorLen);
        outLength += vendorLen;
        buffer[outLength++] = '\n';
    }
    bool bIntel = false;
    bool bAMD = false;
    if (0 == __strcmp(vendor, PROTECT_Ansi("GenuineIntel")))
        bIntel = true;
    else if (0 == __strcmp(vendor, PROTECT_Ansi("AuthenticAMD")))
        bAMD = true;

    // Extended features.
    __cpuidex(data, 0x80000000, 0);
    unsigned __int32 maxExtendedIndex = data[0];
    if (inLength >= outLength + 9) {
        fillDword(maxExtendedIndex, buffer, inLength, outLength);
        buffer[outLength++] = '\n';
    }

    // Processor brand string.
    if (maxExtendedIndex >= 0x80000004) {
        char brand[49];
        int *op_ptr = (int *)brand;
        for (unsigned __int32 brandIndex = 0x80000002; brandIndex <= 0x80000004; ++brandIndex) {
            __cpuidex(data, brandIndex, 0);
            for (int i = 0; i < CPUID_MAX_REGISTER; ++i)
                *op_ptr++ = data[i];
        }
        brand[48] = 0;
        size_t brandLen = __strlen(brand);
        if (inLength >= outLength + brandLen + 1) {
            __memcpy(&buffer[outLength], brand, brandLen);
            outLength += brandLen;
            buffer[outLength++] = '\n';
        }
    }

    // Now get other features.
    bool bPSN = false;
    // Processor and Processor Feature Identifiers.
    if (maxIndex >= 1) {
        __cpuidex(data, 1, 0);
        // We keep Initial_APIC_ID because we check every core of CPU.
        if (bIntel)
            bPSN = !!(data[CPUID_EDX] & 0x40000);
        if (bAbsoluteStable)
            data[CPUID_EBX] = data[CPUID_ECX] = data[CPUID_EDX] = 0; // This may disabled by BIOS.
        recordCPUID(1, 0, data, buffer, inLength, outLength);
    }
    if (bPSN && maxIndex >= 3) {
        __cpuidex(data, 3, 0);
        if (bAbsoluteStable)
            data[CPUID_EAX] = data[CPUID_EBX] = 0; // Reserved.
        recordCPUID(3, 0, data, buffer, inLength, outLength);
    }

    // Structured Extended Feature Identifiers.
    if (!bAbsoluteStable && maxIndex >= 7) {
        // Ignore sub leafs.
        __cpuidex(data, 7, 0);
        recordCPUID(7, 0, data, buffer, inLength, outLength);
    }

    if (!bAbsoluteStable && maxExtendedIndex >= 0x80000001) {
        __cpuidex(data, 0x80000001, 0);
        data[CPUID_ECX] &= 0xFFFFFFFE; // Erase LAHF/SAHF available (when in 64-bit mode).
        data[CPUID_EDX] &= 0xFFFFF7FF; // Erase SYSCALL/SYSRET available (when in 64-bit mode).
        recordCPUID(0x80000001, 0, data, buffer, inLength, outLength);
    }
    return true;
}

static bool getAllCPUID(char *buffer, size_t inLength, bool bAbsoluteStable, size_t& outLength)
{
    bool bRet = false;
    DWORD_PTR my = 0, sys = 0;
    if (!GetProcessAffinityMask(GetCurrentProcess(), &my, &sys))
        return false;

    for (int i = 0; i < sizeof(DWORD_PTR) * 8; ++i) {
        if (((DWORD_PTR)1 << i) & sys) {
            if (SetProcessAffinityMask(GetCurrentProcess(), (DWORD_PTR)1 << i)) {
                if (inLength >= outLength + 7) {
                    buffer[outLength++] = '-';
                    buffer[outLength++] = 'C';
                    buffer[outLength++] = 'P';
                    buffer[outLength++] = 'U';
                    buffer[outLength++] = i / 10 + '0';
                    buffer[outLength++] = i % 10 + '0';
                    buffer[outLength++] = '\n';
                }
                if (getCPUID(buffer, inLength, bAbsoluteStable, outLength))
                    bRet = true;
            }
        }
    }
    // Restore.
    SetProcessAffinityMask(GetCurrentProcess(), my);
    return bRet;
}

size_t getCpuId(char* buffer, size_t length)
{
    size_t len = 0;
    getCPUID(buffer, length, false, len);
    return len;
}

size_t Machine::getAllCpuId(char* buffer, size_t length)
{
    size_t len = 0;
    getAllCPUID(buffer, length, false, len);
    return len;
}

static bool getDiskIDImpl(char *buffer, size_t inLength, bool bAbsoluteStable, size_t& outLength)
{
    bool bRet = false;
    // Only get physical drive 0, because of data stability.
    HANDLE hDisk = CreateFileA("\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hDisk != INVALID_HANDLE_VALUE) {
        if (inLength >= outLength + 7) {
            buffer[outLength++] = '-';
            buffer[outLength++] = 'D';
            buffer[outLength++] = 'i';
            buffer[outLength++] = 's';
            buffer[outLength++] = 'k';
            buffer[outLength++] = '0';
            buffer[outLength++] = '\n';
        }
        unsigned char buf[0x1000];
        STORAGE_PROPERTY_QUERY query;
        query.PropertyId = StorageDeviceProperty;
        query.QueryType = PropertyStandardQuery;
        DWORD re;
        if (DeviceIoControl(hDisk, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), buf, sizeof(buf), &re, NULL) &&
            re >= sizeof(STORAGE_DEVICE_DESCRIPTOR)) {
            STORAGE_DEVICE_DESCRIPTOR *descriptor = (STORAGE_DEVICE_DESCRIPTOR *)buf;
            if (re >= descriptor->Size) {
                if (!bAbsoluteStable) {
                    unsigned __int32 tmp;
                    tmp = (unsigned __int32)descriptor->DeviceType |
                        (unsigned __int32)descriptor->DeviceTypeModifier << 8 |
                        (unsigned __int32)(descriptor->RemovableMedia ? 1 : 0) << 16 |
                        (unsigned __int32)(descriptor->CommandQueueing ? 1 : 0) << 17 |
                        (unsigned __int32)(descriptor->BusType & 0xFF) << 24;
                    if (inLength >= outLength + 9) {
                        fillDword(tmp, buffer, inLength, outLength);
                        buffer[outLength++] = '\n';
                    }
                }
                if (descriptor->VendorIdOffset != 0) {
                    char *str = (char *)descriptor + descriptor->VendorIdOffset;
                    size_t len = strlen(str);
                    if (inLength >= outLength + len + 1) {
                        memcpy(&buffer[outLength], str, len);
                        outLength += len;
                        buffer[outLength++] = '\n';
                    }
                }
                if (descriptor->ProductIdOffset != 0) {
                    char *str = (char *)descriptor + descriptor->ProductIdOffset;
                    size_t len = strlen(str);
                    if (inLength >= outLength + len + 1) {
                        memcpy(&buffer[outLength], str, len);
                        outLength += len;
                        buffer[outLength++] = '\n';
                    }
                }
                if (descriptor->ProductRevisionOffset != 0) {
                    char *str = (char *)descriptor + descriptor->ProductRevisionOffset;
                    size_t len = strlen(str);
                    if (inLength >= outLength + len + 1) {
                        memcpy(&buffer[outLength], str, len);
                        outLength += len;
                        buffer[outLength++] = '\n';
                    }
                }
                if (descriptor->SerialNumberOffset != 0) {
                    char *str = (char *)descriptor + descriptor->SerialNumberOffset;
                    size_t len = strlen(str);
                    if (inLength >= outLength + len + 1) {
                        memcpy(&buffer[outLength], str, len);
                        outLength += len;
                        buffer[outLength++] = '\n';
                    }
                }
                bRet = true;
            }
        }
        if (DeviceIoControl(hDisk, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, buf, sizeof(buf), &re, NULL) &&
            re >= FIELD_OFFSET(DISK_GEOMETRY_EX, Data)) {
            PDISK_GEOMETRY_EX pInfo = (PDISK_GEOMETRY_EX)buf;
            if (!bAbsoluteStable && inLength >= outLength + 53) {
                fillDword(pInfo->Geometry.Cylinders.HighPart, buffer, inLength, outLength);
                fillDword(pInfo->Geometry.Cylinders.LowPart, buffer, inLength, outLength);
                buffer[outLength++] = '\n';
                fillDword(pInfo->Geometry.MediaType, buffer, inLength, outLength);
                buffer[outLength++] = '\n';
                fillDword(pInfo->Geometry.TracksPerCylinder, buffer, inLength, outLength);
                buffer[outLength++] = '\n';
                fillDword(pInfo->Geometry.SectorsPerTrack, buffer, inLength, outLength);
                buffer[outLength++] = '\n';
                fillDword(pInfo->Geometry.BytesPerSector, buffer, inLength, outLength);
                buffer[outLength++] = '\n';
            }
            if (inLength >= outLength + 17) {
                fillDword(pInfo->DiskSize.HighPart, buffer, inLength, outLength);
                fillDword(pInfo->DiskSize.LowPart, buffer, inLength, outLength);
                buffer[outLength++] = '\n';
            }
            bRet = true;
        }
        CloseHandle(hDisk);
    }
    return bRet;
}

size_t Machine::getDiskId(char* buffer, size_t length)
{
    size_t len = 0;
    getDiskIDImpl(buffer, length, false, len);
    return len;
}

static bool getAllMacImpl(char *buffer, size_t inLength, size_t& outLength)
{
    HKEY hNetworkCards;
    LSTATUS lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards", 0, KEY_READ, &hNetworkCards);
    if (lRet != ERROR_SUCCESS)
        return false;
    DWORD nSubKeys;
    lRet = RegQueryInfoKeyA(hNetworkCards, NULL, NULL, NULL, &nSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    if (lRet != ERROR_SUCCESS) {
        RegCloseKey(hNetworkCards);
        return false;
    }

    std::set<std::string> macSet;
    for (DWORD i = 0; i < nSubKeys; ++i) {
        char subName[0x100]; // To receive id. (my: 13 16)
        DWORD subNameLen = sizeof(subName) - 1;
        lRet = RegEnumKeyExA(hNetworkCards, i, subName, &subNameLen, NULL, NULL, NULL, NULL);
        if (lRet != ERROR_SUCCESS) {
            RegCloseKey(hNetworkCards);
            return false;
        }

        subName[subNameLen] = 0;
        // Query sub key.
        HKEY hSubKey;
        lRet = RegOpenKeyExA(hNetworkCards, subName, 0, KEY_READ, &hSubKey);
        if (ERROR_SUCCESS == lRet) {
            char value[0x100]; // To receive GUID.
            DWORD valueLen = sizeof(value) - 5;
            DWORD valueType;
            lRet = RegQueryValueExA(hSubKey, "ServiceName", NULL, &valueType, (LPBYTE)value + 4, &valueLen);
            if (ERROR_SUCCESS == lRet && REG_SZ == valueType) {
                value[4 + valueLen] = 0;
                // Add "\\\\.\\"
                *(unsigned __int32 *)value = 0x5C2E5C5C;
                HANDLE hNDIS = CreateFileA(value, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
                if (hNDIS != INVALID_HANDLE_VALUE) {
                    typedef ULONG NDIS_OID, *PNDIS_OID;
#define OID_802_3_PERMANENT_ADDRESS 0x01010101
                    NDIS_OID oid = OID_802_3_PERMANENT_ADDRESS;
                    unsigned char out[32];
                    DWORD re;
#define _NDIS_CONTROL_CODE(request,method) CTL_CODE(FILE_DEVICE_PHYSICAL_NETCARD, request, method, FILE_ANY_ACCESS)
#define IOCTL_NDIS_QUERY_GLOBAL_STATS _NDIS_CONTROL_CODE(0, METHOD_OUT_DIRECT)
                    if (DeviceIoControl(hNDIS, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid), out, sizeof(out), &re, NULL) && re > 0) {
                        char tmpStr[65];
                        size_t tmpStrLen = 0;
                        fillHex(out, re, tmpStr, 64, tmpStrLen);
                        tmpStr[tmpStrLen] = 0;
                        macSet.insert(tmpStr);
                    }
                    CloseHandle(hNDIS);
                }
            }
            RegCloseKey(hSubKey);
        }
    }
    RegCloseKey(hNetworkCards);
    if (!macSet.empty() && inLength >= outLength + 6) {
        buffer[outLength++] = '-';
        buffer[outLength++] = 'M';
        buffer[outLength++] = 'A';
        buffer[outLength++] = 'C';
        buffer[outLength++] = 's';
        buffer[outLength++] = '\n';
        for (std::set<std::string>::iterator it = macSet.begin(); it != macSet.end(); ++it) {
            if (inLength >= outLength + it->length() + 1) {
                memcpy(&buffer[outLength], it->data(), it->length());
                outLength += it->length();
                buffer[outLength++] = '\n';
            }
        }
        return true;
    }
    return false;
}

size_t Machine::getAllMacId(char *buffer, size_t length)
{
    size_t len = 0;
    getAllMacImpl(buffer, length, len);
    return len;
}

static bool getVolumeIdImpl(char *buffer, size_t inLength, size_t& outLength)
{
    const char *volume = "C:\\";
    char tmp[MAX_PATH];
    DWORD dwRet = GetWindowsDirectoryA(tmp, MAX_PATH);
    if (dwRet >= 3 && dwRet < MAX_PATH) {
        tmp[3] = 0;
        volume = tmp;
    }
    DWORD sn = 0;
    if (GetVolumeInformationA(volume, NULL, 0, &sn, NULL, NULL, NULL, 0)) {
        if (inLength >= outLength + 15) {
            buffer[outLength++] = '-';
            buffer[outLength++] = 'V';
            buffer[outLength++] = 'o';
            buffer[outLength++] = 'l';
            buffer[outLength++] = '0';
            buffer[outLength++] = '\n';
            fillDword(sn, buffer, inLength, outLength);
            buffer[outLength++] = '\n';
        }
        return true;
    }
    return false;
}

size_t Machine::getVolumeID(char* buffer, size_t length)
{
    size_t len = 0;
    getVolumeIdImpl(buffer, length, len);
    return len;
}

static LPWSTR getProcessNameById(DWORD ProcessID)
{
    HANDLE hProcessSnap;

    LPWSTR result = new WCHAR[MAX_PATH + 1];
    PROCESSENTRY32 pe32;
    //获取系统中全部进程的快照 Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hProcessSnap) return(FALSE);

    pe32.dwSize = sizeof(PROCESSENTRY32);

    // 获取第一个进程的信息 Retrieve information about the first process,
    // 若获取失败则退出 and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        // 清除快照 clean the snapshot object
        CloseHandle(hProcessSnap);
        printf("!!! Failed to gather information on system processes! \n");
        return(NULL);
    }

    //匹配进程ID Matching Process ID
    do {
        if (ProcessID == pe32.th32ProcessID) {
            //拷贝进程名 Copy Process Name
            wcscpy(result, (LPWSTR)pe32.szExeFile);
            break;
        }
    } while (::Process32Next(hProcessSnap, &pe32));

    // 清除快照 clean the snapshot object
    ::CloseHandle(hProcessSnap);

    return result;
}

typedef enum enumSYSTEM_INFORMATION_CLASS {
    SystemBasicInformation,
    SystemProcessorInformation,
    SystemPerformanceInformation,
    SystemTimeOfDayInformation,
}SYSTEM_INFORMATION_CLASS;

typedef struct tagPROCESS_BASIC_INFORMATION {
    DWORD ExitStatus;
    DWORD PebBaseAddress;
    DWORD AffinityMask;
    DWORD BasePriority;
    ULONG UniqueProcessId;
    ULONG InheritedFromUniqueProcessId;
}PROCESS_BASIC_INFORMATION;

typedef LONG(WINAPI *PNTQUERYINFORMATIONPROCESS)(HANDLE, UINT, PVOID, ULONG, PULONG);
PNTQUERYINFORMATIONPROCESS	pNtQueryInformationProcess = NULL;

//#define PRINT_LINE	printf("---------------------------------------------\n")

static int getParentProcessID(DWORD dwId)
{
    LONG                      status;
    DWORD                     dwParentPID = 0;
    HANDLE                    hProcess;
    PROCESS_BASIC_INFORMATION pbi;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwId);
    if (!hProcess)
        return -1;

    if (!pNtQueryInformationProcess)
        pNtQueryInformationProcess = (PNTQUERYINFORMATIONPROCESS)::GetProcAddress(::LoadLibraryW(L"ntdll.dll"), "NtQueryInformationProcess");

    status = pNtQueryInformationProcess(hProcess, SystemBasicInformation, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
    if (!status)
        dwParentPID = pbi.InheritedFromUniqueProcessId;

    CloseHandle(hProcess);
    return dwParentPID;
}

const wchar_t* Machine::getParentName()
{
    DWORD parentProcessID = getParentProcessID(::GetCurrentProcessId());
    LPWSTR name = getProcessNameById(parentProcessID);

    return name;
}