#ifndef wkeJsBindFreeTempObject_h
#define wkeJsBindFreeTempObject_h

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

namespace wke {

void freeV8TempObejctOnOneFrameBefore();

class AutoDisableFreeV8TempObejct {
public:
    AutoDisableFreeV8TempObejct();
    ~AutoDisableFreeV8TempObejct();

    static bool isDisable() { return 0 != m_disableCount; }

private:
    static int m_disableCount;
};

}

#endif
#endif // wkeJsBindFreeTempObject_h