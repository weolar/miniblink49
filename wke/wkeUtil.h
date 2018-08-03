#ifndef wke_wkeUtil_h
#define wke_wkeUtil_h

#include "v8.h"

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

namespace wke {

void recordJsExceptionInfo(const v8::TryCatch& tryCatch);

class AutoAllowRecordJsExceptionInfo {
public:
    AutoAllowRecordJsExceptionInfo();
    ~AutoAllowRecordJsExceptionInfo();

    static bool isAllow() { return 0 != m_allowCount; }

private:
    static int m_allowCount;
};

void freeV8TempObejctOnOneFrameBefore();

class AutoDisableFreeV8TempObejct {
public:
    AutoDisableFreeV8TempObejct();
    ~AutoDisableFreeV8TempObejct();

    static bool isDisable() { return 0 != m_disableCount; }

private:
    static int m_disableCount;
};

bool checkThreadCallIsValid(const char* funcName);

}

#endif
#endif // wke_wkeUtil_h