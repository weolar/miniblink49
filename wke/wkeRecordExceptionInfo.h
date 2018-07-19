#ifndef wkeRecordExceptionInfo_h
#define wkeRecordExceptionInfo_h

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

}

#endif
#endif // wkeJsBindFreeTempObject_h